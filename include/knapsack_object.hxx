#ifndef EKP_KNAPSACK_OBJECT_HXX
#define EKP_KNAPSACK_OBJECT_HXX

#include <config.hxx>

#include <algorithm>
#include <vector>
#include <numeric>
#include <memory>
#include <stack>
#include <tuple>
#include <math.h>

namespace ekp {

  class ekp_instance {
  public:
    using node = std::tuple<REAL,REAL,REAL,REAL>;

    struct knapsack_item {
      REAL cost;
      INDEX weight;
      INDEX var;
      REAL val = 0.0;
      bool removed = false;

      knapsack_item* next = NULL;
      knapsack_item* prev = NULL;

    };

    template<typename M>
    ekp_instance(M& m)
    : ekp_instance(m.costs,m.weights,m.b) { }

    ekp_instance(std::vector<REAL> c,std::vector<INDEX> w,INDEX b)
    : nVars_(c.size()),rhs_(b)
    {
      items_.reserve(nVars_);
      items_ptr_.reserve(nVars_);
      removed_.reserve(nVars_);

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

    void sort(){
      auto f = [&](knapsack_item* i,knapsack_item* j){
        REAL iv = i->cost/((REAL) i->weight);
        REAL jv = j->cost/((REAL) j->weight);
        return iv < jv;
      };
      std::sort(items_.begin(),items_.end(),f);
      this->UpdateList();
    }

    bool remove(knapsack_item* p){
      assert( p->removed == false );
      assert( 0.0 == p->val || p->val == 1.0 );

      p->removed = true;
      removed_.push_back(p);

      rhs_ = rhs_ - p->val*p->weight;

      auto prev = p->prev;
      auto next = p->next;
      if( prev != NULL ){ prev->next = next; } else { begin_ = next; }
      if( next != NULL ){ next->prev = prev; }

      return true;
    }

    bool restore(){
      if( removed_.empty() ){
        return false;
      } else {
        auto p = removed_.back();

        p->removed = false;

        auto prev = p->prev;
        auto next = p->next;

        assert( p->val == 0 || p->val == 1 );
        rhs_ = rhs_ + p->val*p->weight;

        if( prev != NULL ){ prev->next = p; } else { begin_ = p; }
        if( next != NULL ){ next->prev = p; }

        removed_.pop_back();
        return true;
      }
    }

    /**
    * Create solution vector with stats
    **/
    void solution_status(std::vector<char>& x){
      x.resize(nVars_ + fixed_.size() ,0.0);
      for( auto v : items_ ){
        if( v->removed ){ x[v->var] = 'r'; }
        else{ x[v->var] = 's'; }
      }
      for( auto v : fixed_ ){ x[std::get<0>(v)] = 'f'; }
    }

    /**
    * Create solution vector and calculate cost
    **/
    void solution(std::vector<REAL>& x){
      x.resize(nVars_ + fixed_.size() ,0.0);
      //printf("\nSolution:\n");
      cost_ = 0.0;
      weight_ = 0.0;
      REAL weight_removed = 0.0;
      for( auto v : items_ ){
        assert( 0.0 <= v->val &&  v->val <= 1.0  );
        //printf("%d %.5f \n",(int) v->var,(double) v->val);
        x[v->var] = v->val;
        cost_ += v->val*v->cost;
        weight_ += v->val*((REAL) v->weight);
        if( v->removed ){
          weight_removed += v->val*((REAL) v->weight);
        }
      }

      assert( fabs(weight_ - rhs_ + weight_removed) < 1e-10 );
      assert( rhs_ == 0 || weight_ > 0.0 );

      //printf("\n");
      for( auto v : fixed_ ){
        assert( 0 <= std::get<0>(v) && std::get<0>(v) < x.size() );
        assert( std::get<1>(v) == 0.0 || std::get<1>(v) == 1.0 );
        //printf("%d -> %.5f \n",(int) std::get<0>(v),(double) std::get<1>(v));
        x[std::get<0>(v)] = std::get<1>(v);
        cost_ += std::get<1>(v)*std::get<2>(v);
        weight_ += std::get<1>(v)*std::get<3>(v);
      }
    }

    /**
    * \jantodo prüfen ob "if" Teil überhaupt stimmt -> removed oder fixed können ja auch auf 0 stehen
    */
    bool feasible(REAL bestIntSol = EKPINF){
      if( begin_ == end_ ){
        assert( begin_ == NULL && end_ == NULL );
        INDEX w = 0;
        for( auto p : removed_ ){  w += p->weight; }   // <- evtl. noch *value?
        for( auto p : fixed_ ){ w += std::get<3>(p); } // <- evtl. noch *value?

        return w == rhs_;
      } else {
        INDEX w = 0; INDEX wk = begin_->weight; INDEX wwk = begin_->weight;
        INDEX bk = 0; REAL ck = EKPINF; REAL zk = 0;

        for( auto item = begin_;item!=end_;item=item->next ){
          w += item->weight;
          if( item->weight < wk ){ wwk = wk; wk = item->weight; }
          if( wk < item->weight && item->weight < wwk ){ wwk = item->weight; }
          if( item->cost < ck ){ ck = item->cost; }
        }

        for( auto p : removed_ ){ zk += p->val*p->cost; }
        for( auto p : fixed_ ){ zk += std::get<1>(p)*std::get<2>(p); }

        bool temp = true;
        if( rhs_ > 0 ){
          temp = temp && !(wk < rhs_ && rhs_ < wwk);
          temp = temp && !(rhs_ < wk);
          temp = temp && ck + zk < bestIntSol;
        }

        return temp && (w >= rhs_);
      }
    }



    REAL cost(){ assert(cost_ < EKPINF); return cost_; }
    REAL weight(){ assert( weight_ > 0.0 ); return weight_; }

  private:

    void UpdateList(){
      auto p0 = items_.begin();
      auto p1 = items_.begin(); p1++;

      begin_ = *p0;
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
    REAL weight_ = 0.0;
    std::vector<knapsack_item*> items_;
    std::vector<std::shared_ptr<knapsack_item>> items_ptr_;
    knapsack_item* begin_;
    knapsack_item* end_ = NULL;
    std::vector<knapsack_item*> removed_;
    std::vector<node> fixed_;
  };


  template<>
  ekp_instance::ekp_instance(ekp_instance& ekp)
  : rhs_(ekp.rhs()){
    items_.reserve(ekp.numberOfVars());
    items_ptr_.reserve(ekp.numberOfVars());

    knapsack_item* it = ekp.Begin();
    knapsack_item* end = ekp.End();
    while( it != end ){
      std::shared_ptr<knapsack_item>  item(new knapsack_item);
      item->cost = it->cost;
      item->weight = it->weight;
      item->var = it->var;

      items_.push_back(item.get());
      items_ptr_.push_back(item);

      it = it->next;
    }

    fixed_.reserve( ekp.removed_.size() + ekp.fixed_.size() );
    for( auto p : ekp.fixed_ ){
      fixed_.push_back(p);
    }
    for( auto p : ekp.removed_ ){
      assert( p->val == 0.0 || p->val == 1.0 );
      fixed_.push_back(std::make_tuple(p->var,p->val,p->cost,p->weight));
    }

    nVars_ = items_.size();
    if( nVars_ > 0){
      removed_.reserve(nVars_);
      this->UpdateList();
    } else {
      begin_ = NULL;
      end_ = NULL;
    }

    assert( fixed_.size() + nVars_ == ekp.numberOfVars() + ekp.fixed_.size() );
  }

}

#endif // EKP_KNAPSACK_OBJECT_HXX
