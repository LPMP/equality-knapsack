#ifndef EKP_KNAPSACK_PEGGING_HXX
#define EKP_KNAPSACK_PEGGING_HXX

#include <math.h>

namespace ekp {

  template<typename EKP,typename ITEM>
  INDEX knapsack_pegging(EKP& e,ITEM* f,REAL relax_cost){
    INDEX count = 0;
    REAL int_cost = e.cost();
    assert( int_cost < EKPINF );
    assert( relax_cost < EKPINF );

    REAL pf = f->cost/((REAL) f->weight);

    auto it = e.Begin();
    auto peg = [&](double c,double w){
      return relax_cost + c + pf*w;
    };

    for(;it!=e.End();it=it->next){
      assert( it->val == 0.0 || it->val == 1.0 );
      if( it->val == 1.0 && peg((-1.0)*it->cost,it->weight) >= int_cost ){
        e.remove(it);
        count++;
      }
      if( it->val == 0.0 && peg(it->cost,(-1.0)*it->weight) >= int_cost ){
        e.remove(it);
        count++;
      }
    }

    return count;
  }

}

#endif // EKP_KNAPSACK_PEGGING_HXX
