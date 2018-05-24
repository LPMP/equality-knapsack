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
      REAL val = 0.0;

      knapsack_item* next = NULL;
      knapsack_item* prev = NULL;
      knapsack_item* master = NULL;

    };

    template<typename M>
    ekp_instance(M& m)
    : ekp_instance(m.costs,m.weights,m.b) { }

    ekp_instance(std::vector<REAL> c,std::vector<INDEX> w,INDEX b)
    : nVars_(c.size()),rhs_(b)
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

    REAL cost(INDEX i){ assert(i<nVars_); return items_[i]->cost; }
    INDEX weight(INDEX i){ assert(i<nVars_); return items_[i]->weight; }
    INDEX index(INDEX i){ assert(i<nVars_); return items_[i]->var; }
    INDEX rhs() const { return rhs_; }
    INDEX numberOfVars() const { return nVars_; };

    knapsack_item* item(INDEX i){ assert(i<nVars_); return items_ptr_[i].get(); }

    knapsack_item* Begin() const { return begin_; }
    knapsack_item* End() const { return end_; }

    //void setCost(INDEX i,REAL c){ assert(i<nVars_); items_ptr_[i]->cost = c; }

    void sort(){
      auto f = [&](knapsack_item* i,knapsack_item* j){
        REAL iv = i->cost/((REAL) i->weight);
        REAL jv = j->cost/((REAL) j->weight);
        return iv < jv;
      };
      std::sort(items_.begin(),items_.end(),f);
      this->UpdateList();
    }

    void remove(knapsack_item* p){
      auto prev = p->prev;
      auto next = p->next;

      assert( p->val == 0 || p->val == 1 );
      rhs_ = rhs_ - p->val*p->weight;

      if( prev != NULL ){ prev->next = next; }
      if( next != NULL ){ next->prev = prev; }
    }

    void restore(knapsack_item* p){
      auto prev = p->prev;
      auto next = p->next;

      assert( p->val == 0 || p->val == 1 );
      rhs_ = rhs_ + p->val*p->weight;

      if( prev != NULL ){ prev->next = p; }
      if( next != NULL ){ next->prev = p; }
    }

    void solution(std::vector<REAL>& x){
      x.resize(nVars_ ,0.0);
      REAL val = 0.0;
      cost_ = 0.0;
      for( auto v : items_ ){
        if( v->master == NULL){ val = v->val; }
        else { val = v->master->val; }

        assert( 0.0 <= val &&  val <= 1.0  );
        x[v->var] = val;
        cost_ += val*v->cost;
      }
    }

    REAL cost(){ assert(cost_ < EKPINF); return cost_; }

  private:

    void UpdateList(){
      auto p0 = items_.begin();
      auto p1 = items_.begin(); p1++;

      begin_ = *p0;
      end_ = NULL;
      while ( p1 != items_.end() )
      {
        (*p0)->next = *p1;
        (*p1)->prev = *p0;
        p0++;p1++;
      }
    }

    INDEX nVars_;
    INDEX rhs_;
    REAL cost_ = EKPINF;
    std::vector<knapsack_item*> items_;
    std::vector<std::shared_ptr<knapsack_item>> items_ptr_;
    knapsack_item* begin_;
    knapsack_item* end_;
  };


  template<>
  ekp_instance::ekp_instance(ekp_instance& ekp)
    : rhs_(ekp.rhs()){

    knapsack_item* it = ekp.Begin();
    knapsack_item* end = ekp.End();
    while( it != end ){
      std::shared_ptr<knapsack_item>  item(new knapsack_item);
      item->cost = it->cost;
      item->weight = it->weight;
      item->var = it->var;

      if( it->master == NULL ){
        item->master = it;
      } else {
        item->master = it->master;
      }

      items_.push_back(item.get());
      items_ptr_.push_back(item);

      it = it->next;
    }
    nVars_ = items_.size();
    this->UpdateList();
  }

}

#endif // EKP_KNAPSACK_OBJECT_HXX
