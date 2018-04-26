#ifndef EKP_KNAPSACK_ENUMERATION_HXX
#define EKP_KNAPSACK_ENUMERATION_HXX

#include <config.hxx>
#include <math.h>
#include <vector>
#include <numeric>
#include <iostream>

namespace ekp {

    template<typename EKP>
    class enumeration {
            struct knapsack_element {
                knapsack_element* covered; // link to list of covered elements (those that will not fit in anymore and that need to be added.
                knapsack_element* next_profit;
                knapsack_element* prev_profit; // double linked list of available elements that can still possibly fit into knapsack. Sorted according to profit.
                knapsack_element* next_weight;
                knapsack_element* prev_weight; // double linked list of available elements as above, but sorted according to weight.
                std::size_t w; // weight of element
                double c; // objective

                void insert()
                {
                    // add to profit list
                    next_profit->prev_profit = this;
                    prev_profit->next_profit = this;

                    // add to weight list
                    next_weight->prev_weight = this;
                    prev_weight->next_weight = this; 
                }

                void remove()
                {
                    // remove from profit list
                    prev_profit->next_profit = next_profit;
                    next_profit->prev_profit = prev_profit;

                    // remove from weight list
                    prev_weight->next_weight = next_weight;
                    next_weight->prev_weight = prev_weight; 
                }
                void cover_element(knapsack_element* o)
                {
                    // add to covered list
                    assert(o->covered == nullptr);
                    covered = this->covered;
                    this->covered = o; 

                    o->remove();
                }

                void uncover_elements()
                {
                    // right order to uncover elements again? I think so.
                    auto* c = this->covered;
                    while(c != nullptr) {
                        c->insert();

                        //
                        auto* next = c->covered;
                        c->covered = nullptr;
                        c = next;
                    }
                }
            };

        public:
            enumeration (EKP& e) : e_(e) 
            { 
                current_solution.reserve(e.numberOfVars());
                best_solution.reserve(e.numberOfVars());


                // populate knapsack_elements vector;
                knapsack_elements.reserve(e.numberOfVars());
                for(std::size_t i=0; i<e.numberOfVars(); ++i) {
                    knapsack_element elem;
                    elem.w = e.weight(i);
                    elem.c = e.cost(i);
                    knapsack_elements.push_back(elem);
                }

                // set links for knapsack_elements
                // (i) sorted profit list
                for(std::size_t i=0; i<e.numberOfVars(); ++i) {
                    knapsack_elements[i].covered = nullptr;

                    knapsack_elements[i].next_profit = &knapsack_elements[ (i+1)%e.numberOfVars() ];
                    knapsack_elements[i].prev_profit = &knapsack_elements[ (i-1)%e.numberOfVars() ];
                }

                // (ii) sorted weight list
                auto weight_sort_func = [&e](const std::size_t i, const std::size_t j) {
                    return e.weight(i) < e.weight(j); 
                };
                std::vector<std::size_t> weight_sort_indices(e.numberOfVars());
                std::iota(weight_sort_indices.begin(), weight_sort_indices.end(), 0);
                std::sort(weight_sort_indices.begin(), weight_sort_indices.end(), weight_sort_func);

                highest_weight = &knapsack_elements[ weight_sort_indices[0] ];
                highest_weight->next_profit = highest_weight;
                highest_weight->prev_profit = highest_weight;
                auto* cur_elem = highest_weight;
                
                for(std::size_t i=0; i<e.numberOfVars(); ++i) {
                    const auto idx = weight_sort_indices[ i ];
                    const auto prev_idx = weight_sort_indices[ (i-1)%e.numberOfVars() ];
                    const auto next_idx = weight_sort_indices[ (i+1)%e.numberOfVars() ];

                    knapsack_elements[idx].next_weight = &knapsack_elements[ next_idx ];
                    knapsack_elements[idx].prev_weight = &knapsack_elements[ prev_idx ];
                } 

                best_objective = std::numeric_limits<double>::infinity();
                best_solution.clear();
            }

            void solve()
            {
                current_solution.clear();
                remaining_capacity = e_.rhs();
                partial_objective = 0.0;

                backtrack(&knapsack_elements[0]);

                //std::cout << "best original solution = ";
                //for(auto x : best_solution) { std::cout << e_.index(x) << ","; }
                //std::cout << "\n";
            }

            bool check_solution(const std::vector<std::size_t>& sol)
            {
                
                if(sol.size() > e_.numberOfVars()) { return false; }
                if(!std::is_sorted(sol.begin(), sol.end())) { return false; }
                if(std::adjacent_find(sol.begin(), sol.end()) != sol.end()) { return false; }
                return true;
            }

            void backtrack(knapsack_element* elem)
            {
                assert(remaining_capacity >= elem->w);
                assert(elem->c >= 0.0); 

                // first search for solution with current element in it
                const auto elem_no = std::distance(&knapsack_elements[0], elem);
                //std::cout << "investigating elem = " << elem_no;
                assert(elem_no < e_.numberOfVars());
                current_solution.push_back(elem_no);
                assert(check_solution(current_solution));
                auto prev_remaining_capacity = remaining_capacity;
                remaining_capacity -= elem->w;
                auto prev_partial_objective = partial_objective;
                partial_objective += elem->c;
                //std::cout << ", remaining capacity = " << remaining_capacity << ", partial objective = " << partial_objective << "\n";


                elem->remove();
                if(remaining_capacity == 0) {
                    //std::cout << "found solution\n";

                    if(partial_objective < best_objective) {
                        best_objective = partial_objective;
                        best_solution.clear();
                        std::copy(current_solution.begin(), current_solution.end(), std::back_inserter(best_solution));
                    }
                } 


                // check whether all elements have been considered (i.e. we are at a leave of the search tree)
                if(elem->next_profit == elem) { 
                    elem->insert();

                    assert(current_solution.back() == elem_no);
                    current_solution.resize(current_solution.size()-1);
                    remaining_capacity = prev_remaining_capacity;
                    partial_objective = prev_partial_objective;

                    return; 
                }

                if(remaining_capacity > 0) {
                    // remove all elements that cannot fit anymore. Add those to the covered list
                    for(auto* candidate=elem->next_weight; candidate!=elem; candidate=candidate->next_weight) {
                        if(candidate->w <= remaining_capacity) { break; }
                        elem->cover_element(candidate); 
                    }

                    // recurse
                    //std::cout << "recurse with element in\n";
                    backtrack(elem->next_profit);
                }

                // now search for solution with current element excluded
                assert(current_solution.back() == elem_no);
                current_solution.resize(current_solution.size()-1);
                remaining_capacity = prev_remaining_capacity;
                partial_objective = prev_partial_objective;
                // add all covered elements back again
                elem->uncover_elements();
                //std::cout << "recurse with element out\n";
                backtrack(elem->next_profit); 

                elem->insert();
            }

        private:

            std::vector<knapsack_element> knapsack_elements; // sorted according to profit
            knapsack_element* highest_weight;
            std::size_t remaining_capacity;
            double partial_objective;

            std::vector<std::size_t> current_solution;
            std::vector<std::size_t> best_solution;
            double best_objective;
            EKP& e_;
  };
  
}


#endif // EKP_KNAPSACK_ENUMERATION_HXX

