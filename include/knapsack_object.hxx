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
      INDEX weight;
      INDEX var;
      INDEX val;
      INDEX sol;

      knapsack_item* next;
      knapsack_item* prev;

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

    auto Begin(){ return begin_; }
    auto End(){ return end_; }

    void setCost(INDEX i,REAL c){ assert(i<nVars_); items_ptr_[i]->cost = c; }

    void sort(){
      auto f = [&](knapsack_item* i,knapsack_item* j){
        REAL iv = i->cost/((REAL) i->weight);
        REAL jv = j->cost/((REAL) j->weight);
        return iv < jv;
      };
      std::sort(items_.begin(),items_.end(),f);

      auto p0 = items_.begin();
      auto p1 = items_.begin(); p1++;

      begin_ = *p0;
      while ( p1 != items_.end() )
      {
        (*p0)->next = *p1;
        (*p1)->prev = *p0;
        p0++;p1++;
      }
      std::shared_ptr<knapsack_item>  item(new knapsack_item);
      items_ptr_.push_back(item);

      (*p0)->next = item.get();
      item->prev = *p0;

      end_ = item.get();

    }

    auto GetRelaxCost(){ return relaxCost_; }
    auto GetIntCost(){ return intCost_; }

    void SetRelaxCost(REAL val){ relaxCost_ = val; }
    void SetIntCost(REAL val){  intCost_ = val; }

    void remove(knapsack_item* p){
      auto prev = p->prev;
      auto next = p->next;

      assert( p->sol == 0 || p->sol == 1 );
      rhs_ = rhs_ - p->sol*p->weight;

      prev->next = next;
      next->prev = prev;
    }

    void add(knapsack_item* p){
      auto prev = p->prev;
      auto next = p->next;

      assert( p->sol == 0 || p->sol == 1 );
      rhs_ = rhs_ + p->sol*p->weight;

      prev->next = p;
      next->prev = p;
    }


  private:

    INDEX nVars_;
    INDEX rhs_;
    std::vector<knapsack_item*> items_;
    std::vector<std::shared_ptr<knapsack_item>> items_ptr_;
    knapsack_item* begin_;
    knapsack_item* end_;

    REAL relaxCost_ = EKPINF;
    REAL intCost_ = EKPINF;

  };


}

#endif // EKP_KNAPSACK_OBJECT_HXX
