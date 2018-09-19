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

    template<typename EKP,typename SOLVER>
    void visit(const INDEX iteration,EKP& e,const double relax,const double integer,SOLVER* s){
      last_iter_ = iteration;
    }

    void presolve(const bool found,const INDEX count,const double relax,const double integer){

    }

  private:
    INDEX last_iter_ = 0;
  };

  class visitor_bb {
  public:
    visitor_bb(){ }

    template<typename EKP,typename SOLVER>
    void visit(const INDEX iteration,EKP& e,const double relax,const double integer,SOLVER* s){

      if( iteration % 5000 == 0 ){
        printf("%010d  %03d/%03d  %7.4f/%7.4f \n",(int) iteration, (int) e.fixed(),(int) e.numberOfVars(),relax,integer);
      }
    }

    void presolve(const bool found,const INDEX count,const double relax,const double integer){
      printf("\n\n");

      if( found ){
        printf("Presolve found an integer solution with %.5f (%.5f)!\n",integer,relax);
        printf("Pegging fixed %d variables \n",(int) count);
      }
      else{
        printf("Presolve did not find any integer solution!\n");
      }

      printf("\n\n");
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
      }

      v.presolve(found,count,bestRelaxedCost_,bestIntegerCost_);

    }

    void print_solution(EKP& e){
      std::vector<double> x;
      std::vector<char> xs;

      e.solution(x);
      e.solution_status(xs);

      printf("\n");
      for( auto v : xs ){ std::cout << v << " "; } std::cout << std::endl;
      for( auto v : x ){ printf("%d ",(int) v); }
      printf("  (%.5f/%.5f)\n\n",e.cost(),bestIntegerCost_);
    }

    void presolve(){
      visitor_empty v;
      this->presolve(v);
    }

    void print_object(EKP& e){
      for( auto it=e.Begin();it!=e.End();it=it->next ){
        printf("%02d %3.6f  %d \n",(int) it->var,(double) it->cost,(int) it->weight);
      }
    }

    /**
    * \jantodo kopiere item "e" erst im If Block
    */
    template<typename Q>
    void add(EKP& e_orig,Q& node_queue){
      if( e_orig.feasible(bestIntegerCost_) ){
        EKP e = e_orig;
        solve_relax(e); e.solution(y_);

        if( relax_.isInteger() && e.cost() < bestIntegerCost_ ){
          std::swap(x_,y_);

          bestIntegerCost_ = e.cost();
        } else if( !relax_.isInteger() ) {
          assert( e.cost() >= bestRelaxedCost_ );
          node_queue.push(std::make_tuple(e.cost(),std::move(e),relax_.OptimalElement()));
        } else { }
      } else { }
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

      if( relax_.isInteger() && e_copy.cost() < bestIntegerCost_ ){
        std::swap(x_,y_);
        bestIntegerCost_ = bestRelaxedCost_;
      }

      node_queue.push(std::make_tuple(bestRelaxedCost_,std::move(e_copy),relax_.OptimalElement()));

      INDEX iter = 0;
      while( !node_queue.empty() ){
        bestRelaxedCost_ = std::get<0>(node_queue.top());
        auto e = std::get<1>(node_queue.top());

        v.visit(iter,e,bestRelaxedCost_,bestIntegerCost_,this);

        if( bestIntegerCost_ - bestRelaxedCost_ > 1e-9 ){

          auto f = std::get<2>(node_queue.top());
          node_queue.pop();

          auto item = e.Begin();
          assert( item != NULL );

          item->val = 0.0;
          e.remove(item);
          add(e,node_queue);

          while( item != f->next ){
            e.restore();
            item->val = 1.0;
            if( e.rhs() < item->weight ){ break; }
            e.remove(item);
            item = item->next;
            item->val = 0.0;
            e.remove(item);
            add(e,node_queue);
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
