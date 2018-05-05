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
#ifndef NDEBUG
                bool in_list;
#endif
            };

            std::size_t element_number(knapsack_element* elem) const
            {
                return elem - &knapsack_elements[0];
            }

            void insert(knapsack_element* elem)
            {
                //std::cout << "inserting element " << element_number(elem) << "\n";
#ifndef NDEBUG
                assert(!elem->in_list);
                elem->in_list = true;
#endif
                if(highest_weight == nullptr) {
                    assert(no_active_elements() == 0);
                    highest_weight = elem;
                } else if(highest_weight->w <= elem->w) {
                    //std::cout << "in insert: new highest element = " << elem - &knapsack_elements[0] << "\n";
                    highest_weight = elem;
                }
                assert(highest_weight_valid());

                // add to profit list
                elem->next_profit->prev_profit = elem;
                elem->prev_profit->next_profit = elem;

                // add to weight list
                elem->next_weight->prev_weight = elem;
                elem->prev_weight->next_weight = elem;

                if(elem->next_weight != elem) {
                    assert(profit_sorting_valid(elem->next_profit));
                }
                assert(weight_sorting_valid() == true);
                assert(profit_sorting_valid(elem) == true);
            }

            void remove(knapsack_element* elem)
            {
                assert(weight_sorting_valid() == true);
                assert(profit_sorting_valid(elem) == true);
#ifndef NDEBUG
                assert(elem->in_list);
                elem->in_list = false;
#endif
                //std::cout << "removing element " << elem - &knapsack_elements[0] << "\n";
                if(highest_weight == highest_weight->next_weight) {
                    highest_weight = nullptr;
                } else if(elem == highest_weight) {
                    //std::cout << "in remove: new highest element = " << highest_weight->next_weight - &knapsack_elements[0] << "\n";
                    highest_weight = highest_weight->next_weight;
                }
                assert(highest_weight_valid());

                // remove from profit list
                elem->prev_profit->next_profit = elem->next_profit;
                elem->next_profit->prev_profit = elem->prev_profit;

                // remove from weight list
                elem->prev_weight->next_weight = elem->next_weight;
                elem->next_weight->prev_weight = elem->prev_weight;

                if(highest_weight != nullptr) {
                    assert(weight_sorting_valid() == true);
                    assert(profit_sorting_valid(elem->next_profit) == true);
                }
            }

            knapsack_element* cover_elements(knapsack_element* elem, knapsack_element* next_profit_element, const std::size_t remaining_capacity) // elem will cover o
            {
                //std::cout << "in cover_elements: next_profit_elem = " << element_number(next_profit_element) << "; ";
                assert(elem->covered == nullptr);
                assert(elem->in_list == false); // must first be removed
                assert(next_profit_element->in_list);
                // add to covered list
                while(highest_weight != nullptr && highest_weight->w > remaining_capacity) {
                    if(highest_weight == next_profit_element) {
                        assert(weight_sorting_valid() == true);
                        next_profit_element = highest_weight->next_profit;
                        //std::cout << "switching to next_profit_element " << element_number(next_profit_element) << "\n";
                        assert(next_profit_element->in_list);
                    }

                    assert(highest_weight->covered == nullptr);
                    highest_weight->covered = elem->covered;
                    elem->covered = highest_weight;
                    remove(highest_weight);
                    assert(highest_weight_valid());
                }
                assert(next_profit_element->in_list);

                if(highest_weight != nullptr) {
                    assert(weight_sorting_valid() == true);
                    assert(profit_sorting_valid(highest_weight) == true);
                }
                //print_covered_list(elem);

                //std::cout << "next_profit_element after covering = " << element_number(next_profit_element) << "\n";
                assert(next_profit_element->in_list == true);
                return next_profit_element;
            }

            void uncover_elements(knapsack_element* elem)
            {
                //std::cout << "in uncover_elements: ";
                //print_covered_list(elem);
                assert(elem->in_list == false); // must be added after uncovering
                // right order to uncover elements again? I think so.
                assert(elem->covered != elem);
                auto* c = elem->covered;
                elem->covered = nullptr;
                while(c != nullptr) {
                    insert(c);
                    assert(c->covered != c);

                    auto* next = c->covered;
                    c->covered = nullptr;
                    c = next;
                }


                assert(weight_sorting_valid() == true);
                assert(profit_sorting_valid(highest_weight) == true);
            }

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
#ifndef NDEBUG
            elem.in_list = true;
#endif
            knapsack_elements.push_back(elem);
        }

        // set links for knapsack_elements
        // (i) sorted profit list
        for(std::size_t i=0; i<e.numberOfVars(); ++i) {
            knapsack_elements[i].covered = nullptr;

            knapsack_elements[i].next_profit = &knapsack_elements[ (i+1)%e.numberOfVars() ];
            knapsack_elements[i].prev_profit = &knapsack_elements[ (e.numberOfVars()+i-1)%e.numberOfVars() ]; // takes care of underflow
        }
        assert(profit_sorting_valid(&knapsack_elements[0]));

        // (ii) sorted weight list
        auto weight_sort_func = [&e](const std::size_t i, const std::size_t j) {
            return e.weight(i) > e.weight(j);
        };
        std::vector<std::size_t> weight_sort_indices(e.numberOfVars());
        std::iota(weight_sort_indices.begin(), weight_sort_indices.end(), 0);
        std::sort(weight_sort_indices.begin(), weight_sort_indices.end(), weight_sort_func);

        //for(auto x : weight_sort_indices) { std::cout << x << ","; }
        //std::cout << "\n";

        highest_weight = &knapsack_elements[0];

        for(std::size_t i=0; i<e.numberOfVars(); ++i) {
            const auto idx = weight_sort_indices[ i ];
            const auto prev_idx = weight_sort_indices[ (e.numberOfVars()+i-1)%e.numberOfVars() ]; // takes care of underflow
            const auto next_idx = weight_sort_indices[ (i+1)%e.numberOfVars() ];

            //std::cout << idx << ", " << prev_idx << ", " << next_idx << "\n";
            knapsack_elements[idx].next_weight = &knapsack_elements[ next_idx ];
            knapsack_elements[idx].prev_weight = &knapsack_elements[ prev_idx ];

            if(highest_weight->w < knapsack_elements[idx].w) {
                highest_weight = &knapsack_elements[idx];
            }
        }

        assert(weight_sorting_valid());
        assert(highest_weight_valid());

        best_objective = std::numeric_limits<double>::infinity();
        best_solution.clear();
    }

    void solve()
    {
        current_solution.clear();
        remaining_capacity = e_.rhs();
        partial_objective = 0.0;

        backtrack(&knapsack_elements[0]);

        // std::cout << "best solution = ";
        // for(auto x : best_solution) { std::cout << e_.index(x) << ","; }
        // std::cout << "\n";
    }

    void solution(std::vector<size_t>& x){
      x.resize(e_.numberOfVars(),0);
      for(auto i : best_solution ){ x[e_.index(i)] = 1; }
    }

    bool solution_valid(const std::vector<std::size_t>& sol) const
    {

        if(sol.size() > e_.numberOfVars()) { return false; }
        if(!std::is_sorted(sol.begin(), sol.end())) { return false; }
        if(std::adjacent_find(sol.begin(), sol.end()) != sol.end()) { return false; }
        return true;
    }

    void print_covered_list(knapsack_element* elem)
    {
        std::cout << "element " << element_number(elem) << " covers: ";
        auto* c = elem->covered;
        while(c != nullptr) {
            std::cout << element_number(c) << ", ";
            c = c->covered;
        }
        std::cout << "\n";
    }

    std::size_t length_of_profit_list(knapsack_element* elem) const
    {
        std::size_t no_elements_in_list = 1;
        for(auto* candidate=elem->next_profit; candidate!=elem; candidate=candidate->next_profit) {
            ++no_elements_in_list;
        }
        return no_elements_in_list;
    }

    std::size_t length_of_weight_list() const
    {
        std::size_t no_elements_in_list = 1;
        for(auto* candidate=highest_weight->next_weight; candidate!=highest_weight; candidate=candidate->next_weight) {
            ++no_elements_in_list;
        }
        return no_elements_in_list;
    }

#ifndef NDEBUG
    std::size_t no_active_elements() const
    {
        std::size_t no_active = 0;
        for(const auto& elem : knapsack_elements) {
            if(elem.in_list == true) { ++no_active; }
        }
        return no_active;
    }

    bool profit_list_active(knapsack_element* elem) const
    {
        if(elem->in_list == false) { return false; }
        for(auto* candidate=elem->next_profit; candidate!=elem; candidate=candidate->next_profit) {
            if(candidate->in_list == false) { return false; }
        }
        return true;
    }

    bool weight_list_active() const
    {
        if(highest_weight->in_list == false) { return false; }
        for(auto* candidate=highest_weight->next_profit; candidate!=highest_weight; candidate=candidate->next_profit) {
            if(candidate->in_list == false) { return false; }
        }
        return true;
    }
#endif

    bool weight_sorting_valid() const
    {
#ifndef NDEBUG
        if(weight_list_active() == false) { return false; }
        if(no_active_elements() != length_of_weight_list()) { return false; }
        if(highest_weight->in_list == false) { return false; }
#endif
        if(highest_weight != nullptr) {
            //std::cout << "weight sorting: " << element_number(highest_weight);
            std::size_t w = highest_weight->w;
            for(auto* candidate=highest_weight->next_weight; candidate!=highest_weight; candidate=candidate->next_weight) {
                //std::cout << "," << element_number(candidate);
                if(w < candidate->w) { return false; }
                w = candidate->w;
            }
            //std::cout << "\n";
        }
        return true;
    }

    bool profit_sorting_valid(knapsack_element* elem) const
    {
#ifndef NDEBUG
        if(profit_list_active(elem) == false) { return false; }
        if(no_active_elements() != length_of_profit_list(elem)) { return false; }
        if(elem->in_list == false) { return false; }
#endif
        // first find element with greatest profit
        knapsack_element* highest_profit_elem = elem;
        double p = elem->w/elem->c;
        for(auto* candidate=elem->next_profit; candidate!=elem; candidate=candidate->next_profit) {
            double p_next = candidate->w/candidate->c;
            if(p_next > p) {
                highest_profit_elem = candidate;
                p = p_next;
            }
        }
        for(auto* candidate=highest_profit_elem->next_profit; candidate!=highest_profit_elem; candidate=candidate->next_profit) {
            double p_next = candidate->w/candidate->c;
            if(p_next > p) { return false; }
            p = p_next;
        }
        return true;
    }

    bool highest_weight_valid() const
    {
#ifndef NDEBUG
        const auto no_active_elements = std::count_if(knapsack_elements.begin(), knapsack_elements.end(), [](const knapsack_element& e) { return e.in_list; });
        if(no_active_elements > 0) {
            if(highest_weight->in_list == false) { return false; }
            for(const auto& e : knapsack_elements) {
                if(e.in_list && highest_weight->w < e.w) { return false; }
            }
            return true;
        } else {
            return highest_weight == nullptr;
        }
#endif
        return true;
    }

    void backtrack(knapsack_element* elem)
    {
        assert(elem->in_list);
        assert(remaining_capacity >= elem->w);
        assert(elem->c >= 0.0);

        // first search for solution with current element in it
        const auto elem_no = std::distance(&knapsack_elements[0], elem);
        //std::cout << "investigating elem = " << elem_no;
        assert(elem_no < e_.numberOfVars());
        current_solution.push_back(elem_no);
        assert(solution_valid(current_solution));
        auto prev_remaining_capacity = remaining_capacity;
        remaining_capacity -= elem->w;
        auto prev_partial_objective = partial_objective;
        partial_objective += elem->c;
        //std::cout << ", remaining capacity = " << remaining_capacity << ", partial objective = " << partial_objective << "\n";

        auto* next_profit_element = elem->next_profit;
        assert(next_profit_element->in_list);

        if(remaining_capacity == 0) {
            //std::cout << "found solution with objective = " << partial_objective << ", best objective = " << best_objective << "\n";

            if(partial_objective < best_objective) {
                best_objective = partial_objective;
                best_solution.clear();
                std::copy(current_solution.begin(), current_solution.end(), std::back_inserter(best_solution));
            }

            current_solution.resize(current_solution.size()-1);
            remaining_capacity = prev_remaining_capacity;
            partial_objective = prev_partial_objective;

            return;
        }

        // check whether already assigned costs outweigh best solution cost or whether all elements have been considered (i.e. we are at a leave of the search tree)
        if( partial_objective >=  best_objective || elem->next_profit == elem ) {
            assert(current_solution.back() == elem_no);
            current_solution.resize(current_solution.size()-1);
            remaining_capacity = prev_remaining_capacity;
            partial_objective = prev_partial_objective;

            return;
        }

        remove(elem);

        assert(weight_sorting_valid());
        // remove all elements that cannot fit anymore. Add those to the covered list
        next_profit_element = cover_elements(elem, next_profit_element, remaining_capacity);
        assert(next_profit_element->in_list);

        // recurse
        // find next element that is still active
        //std::cout << "recurse with element " << elem_no << " in\n";
        backtrack(next_profit_element);

        // now search for solution with current element excluded
        assert(current_solution.back() == elem_no);
        current_solution.resize(current_solution.size()-1);
        remaining_capacity = prev_remaining_capacity;
        partial_objective = prev_partial_objective;
        // add all covered elements back again
        uncover_elements(elem);
        //std::cout << "recurse with element " << elem_no << " out\n";
        backtrack(elem->next_profit);

        insert(elem);
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
