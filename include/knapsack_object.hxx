#ifndef EKP_KNAPSACK_OBJECT_HXX
#define EKP_KNAPSACK_OBJECT_HXX

#include <config.hxx>
#include <algorithm>
#include <vector>
#include <numeric>
#include <memory>

namespace ekp {

  class ekp_instance {
  public:
    struct knapsack_item {
      REAL cost;
      REAL weight;
      INDEX var;
    };

    template<typename M>
    ekp_instance(M& m)
    : ekp_instance(m.costs,m.weights,m.b) { }

    ekp_instance(std::vector<REAL> c,std::vector<INDEX> w,INDEX b)
    : nVars_(c.size()),rhs_((REAL) b)
    {

      for(INDEX i=0;i<nVars_;i++){
        std::shared_ptr<knapsack_item>  item(new knapsack_item);
        item->cost = c[i];
        item->weight = (REAL) w[i];
        item->var = i;

        items_.push_back(item.get());
        items_ptr_.push_back(item);
      }
      this->sort();
    }

    auto cost(INDEX i){ assert(i<nVars_); return items_[i]->cost; }
    auto weight(INDEX i){ assert(i<nVars_); return items_[i]->weight; }
    auto index(INDEX i){ assert(i<nVars_); return items_[i]->var; }
    auto rhs(){ return rhs_; }
    auto numberOfVars(){ return nVars_; };

    knapsack_item* item(INDEX i){ assert(i<nVars_); return items_[i]; }

    auto Begin(){ return items_.begin(); }
    auto End(){ return items_.end(); }

    void setCost(INDEX i,REAL c){ assert(i<nVars_); items_ptr_[i]->cost = c; }

    void sort(){
      auto f = [&](knapsack_item* i,knapsack_item* j){
        REAL iv = i->cost/i->weight;
        REAL jv = j->cost/j->weight;
        return iv < jv;
      };
      std::sort(items_.begin(),items_.end(),f);
    }


  private:

    INDEX nVars_;
    REAL rhs_;
    std::vector<knapsack_item*> items_;
    std::vector<std::shared_ptr<knapsack_item>> items_ptr_;

  };


}

#endif // EKP_KNAPSACK_OBJECT_HXX
