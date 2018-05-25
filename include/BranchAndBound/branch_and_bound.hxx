#ifndef EKP_KNAPSACK_BRANCH_AND_BOUND_HXX
#define EKP_KNAPSACK_BRANCH_AND_BOUND_HXX

#include <queue>
#include <tuple>

#include <config.hxx>
#include <knapsack_object.hxx>
#include <solve_relaxation.hxx>
#include "BranchAndBound/pegging.hxx"
#include "BranchAndBound/incumbent.hxx"

namespace ekp {

  class visitor_empty {
  public:
    visitor_empty(){ }

    void visit(){

    }

  private:
  };

  class visitor_bb {
  public:
    visitor_bb(){ }

    void visit(){

    }

  private:
  };

  class branch_and_bound{
  public:
    using EKP = ekp_instance;
    using ITEM = typename EKP::knapsack_item;
    using node = std::tuple<REAL,EKP,ITEM*>;

    branch_and_bound(EKP& e)
    : e_(e) {  }

    template<typename VISITOR>
    void presolve(VISITOR v){
      solve_relax(e_);
      e_.solution(y_);
      bestRelaxedCost_ = e_.cost();

      INDEX count = 0;
      bool found = calculate_incumbent(e_,relax_.OptimalElement(),x_);
      if( found ){
        bestIntegerCost_ = e_.cost();
        count = knapsack_pegging(e_,relax_.OptimalElement(),bestRelaxedCost_);

        // printf("\nfound incumbent and pegged %d vars \n\n",(int) count);
        // for( auto v : x_ ){ printf(" %.6f",(double) v); }
        // printf("  (%.5f)\n\n",bestIntegerCost_);
      }
    }

    void presolve(){
      visitor_empty v;
      this->presolve(v);
    }

    template<typename Q>
    void add(EKP e,Q& node_queue){
      if( e.feasible(bestIntegerCost_) ){
        solve_relax(e); e.solution(y_);

        // for( auto v : y_ ){ printf(" %.6f",(double) v); }
        // printf("  (%.6f)",e.cost());

        if( relax_.isInteger() && e.cost() < bestIntegerCost_ ){
          std::swap(x_,y_);

          // printf(" *\n");

          bestIntegerCost_ = e.cost();
        } else if( !relax_.isInteger() ) {
          // printf(" +\n");
          assert( e.cost() >= bestRelaxedCost_ );
          node_queue.push(std::make_tuple(e.cost(),std::move(e),relax_.OptimalElement()));
        } else {
          // printf(" (*)\n");
        }
      } else {
        // printf(" # infeasible # \n");
      }

    }

    template<typename VISITOR>
    void solve(VISITOR v){

      auto compare = [](node l,node r){
        return std::get<0>(l) > std::get<0>(r);
      };

      std::priority_queue<node,std::vector<node>,decltype(compare)> node_queue(compare);
      EKP e_copy = e_;
      solve_relax(e_copy); e_copy.solution(y_);

      bestRelaxedCost_ = e_copy.cost();
      node_queue.push(std::make_tuple(bestRelaxedCost_,std::move(e_copy),relax_.OptimalElement()));

      INDEX iter = 0;
      while( !node_queue.empty() ){
        // printf("  %d ->  \n",(int) iter);
        bestRelaxedCost_ = std::get<0>(node_queue.top());
        auto e = std::get<1>(node_queue.top());
        if( bestIntegerCost_ - bestRelaxedCost_ > 1e-9 ){

          auto f = std::get<2>(node_queue.top());
          node_queue.pop();

          auto item = e.Begin();
          assert( item != NULL );

          item->val = 0.0;
          if( e.remove(item) ){
            add(e,node_queue);

            while( item != f->next ){
              e.restore();
              item->val = 1.0;
              if( !e.remove(item) ){ break; }
              item = item->next;
              item->val = 0.0;
              if( !e.remove(item) ){ break; }
              add(e,node_queue);
            }
          }
        }
        else {
          break;
        }
        iter++;
      }
    }

    void solve(){
      visitor_empty v;
      this->solve(v);
    }

    REAL LowerBound(){ return bestRelaxedCost_; }
    REAL UpperBound(){ return bestIntegerCost_; }
    void Solution(std::vector<REAL>& x){
      x.resize(x_.size());
      for(INDEX i=0;i<x.size();i++){ x[i] = x_[i]; }
    }

  private:

    void solve_relax(EKP& e){
      relax_.solve(e.Begin(),e.End(),e.rhs());
    }

    EKP& e_;
    std_relaxation<ITEM> relax_;
    std::vector<REAL> x_;
    std::vector<REAL> y_;
    REAL bestRelaxedCost_ = EKPINF;
    REAL bestIntegerCost_ = EKPINF;
  };

}

#endif // EKP_KNAPSACK_BRANCH_AND_BOUND_HXX
