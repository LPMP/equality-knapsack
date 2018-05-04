#ifndef EKP_KNAPSACK_OBJECT_HXX
#define EKP_KNAPSACK_OBJECT_HXX

#include <config.hxx>
#include <algorithm>
#include <vector>
#include <numeric>
#include <memory>

namespace ekp {


  class ekp_object {
  public:

    struct knapsack_item {
      REAL cost;
      INDEX weight;
      INDEX var;
      knapsack_item* prev;
      knapsack_item* next;
    };

    class iterator {
    public:
      iterator(knapsack_item* k) : item_(k) { }
      iterator& operator++() {item_ = item_->next; return *this;}
      iterator operator++(int) {item_ = item_->next; return *this;}
    private:
      knapsack_item* item_;
    };

    template<typename M>
    ekp_object(M& m)
      : ekp_object(m.costs,m.weights,m.b) { }

    ekp_object(std::vector<REAL> c,std::vector<INDEX> w,INDEX b)
      : nVars_(c.size()){
      assert(nVars_ == w.size());

      knapsack_item* cur = 0;
      for(INDEX i=0;i<nVars_;i++){
        std::shared_ptr<knapsack_item>  ptr(new knapsack_item);
        items_.push_back(ptr);

        ptr->cost = c[i];
        ptr->weight = w[i];
        ptr->var = i;
        ptr->prev = cur;
        if(i !=0 ){
          cur->next = ptr.get();
        } else {
          begin_ = ptr.get();
        }
        cur = ptr.get();
      }
      std::shared_ptr<knapsack_item>  ptr(new knapsack_item);
      items_.push_back(ptr);

      cur->next = ptr.get();
      end_ = ptr.get();
    }

    void sort(){
      auto f = [this](knapsack_item i,knapsack_item j){
        REAL iv = i.cost/((REAL) i.weight);
        REAL jv = j.cost/((REAL) j.weight);
        return iv < jv;
      };
      std::sort(begin_,end_,f);
    }

    auto Begin(){ return begin_; }
    auto End(){ return end_; }

  private:
    INDEX nVars_;
    std::vector<std::shared_ptr<knapsack_item>> items_;
    knapsack_item* begin_;
    knapsack_item* end_;

  };

  /**
    @brief Stores c,w,b for an Equality Knapsack Problem

    Vectors: c (costs), w (weights) and the right-hand-side b.

    min_x <c,x> s.t. <w,x> = b, x_i = 0 or x_i = 1
  **/
  class ekp_instance {
  public:

    template<typename M>
    ekp_instance(M& m)
      : ekp_instance(m.costs,m.weights,m.b) { }

    ekp_instance(std::vector<REAL> c,std::vector<INDEX> w,INDEX b)
      : costs_(c),weights_(w),b_(b),nVars_(costs_.size()) {

      assert( weights_.size() > 2 );
      assert( weights_.size() == costs_.size() );
      assert( weights_.size() == nVars_ );

      sol_.resize(nVars_,0);

      sorted_.resize(nVars_);
      std::iota(sorted_.begin(),sorted_.end(),0);

      auto f = [this](INDEX i,INDEX j){
        REAL iv = costs_[i]/((REAL) weights_[i]);
        REAL jv = costs_[j]/((REAL) weights_[j]);
        return iv < jv;
      };
      std::sort(sorted_.begin(),sorted_.end(),f);
    }

    // void UpdateCosts(std::vector<REAL> c){
    //   costs_ = c;
    //
    //   std::iota(sorted_.begin(),sorted_.end(),0);
    //
    //   auto f = [this](INDEX i,INDEX j){
    //     REAL iv = costs_[i]/((REAL) weights_[i]);
    //     REAL jv = costs_[j]/((REAL) weights_[j]);
    //     return iv < jv;
    //   };
    //
    //   std::sort(sorted_.begin(),sorted_.end(),f);
    // }

    auto cost(INDEX i){ assert(i < costs_.size());  return costs_[sorted_[i]]; }
    auto weight(INDEX i){ assert(i < weights_.size()); return weights_[sorted_[i]]; }
    auto index(INDEX i){ assert(i < sorted_.size()); return sorted_[i]; };
    auto rhs(){ return b_; };
    auto numberOfVars(){ return nVars_; };

    auto GetIntegerOptimal(){ return integerOptimal_; }
    auto GetRelaxedOptimal(){ return relaxedOptimal_; }
    void SetIntegerOptimal(REAL val){ integerOptimal_ = val; }
    void SetRelaxedOptimal(REAL val){ relaxedOptimal_ = val; }

    /**
      * @brief This method should only be used by other methods which supposed
      * to find an integer solution.
    **/
    void SetSolution(INDEX i,INDEX val){ assert( i < nVars_); sol_[i] = val; }

    template<typename V>
    void GetSolution(V& sol){
      sol.resize(nVars_);
      for(INDEX i=0;i<nVars_;i++){
        sol[i] = sol_[i];
      }
    }

    template<typename V>
    void GetOrigSolution(V& sol){
      sol.resize(nVars_);
      for(INDEX i=0;i<nVars_;i++){
        sol[sorted_[i]] = sol_[i];
      }
    }

  private:
    std::vector<INDEX> sorted_;
    std::vector<REAL> costs_;
    std::vector<INDEX> weights_;
    INDEX b_;
    INDEX nVars_;

    REAL relaxedOptimal_ = EKPINF;
    REAL integerOptimal_ = EKPINF;

    std::vector<INDEX> sol_;

  };

}

#endif // EKP_KNAPSACK_OBJECT_HXX
