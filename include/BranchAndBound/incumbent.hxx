#ifndef EKP_KNAPSACK_INCUMBENT_HXX
#define EKP_KNAPSACK_INCUMBENT_HXX

#include <math.h>

namespace ekp {

  template<typename EKP,typename ITEM>
  bool calculate_incumbent(EKP& e,ITEM* f,std::vector<REAL>& x){
    bool found = false;
    INDEX rhs = e.rhs();

    auto it = e.Begin(); INDEX w = 0;
    while( it != f ){ w += it->weight; it->val = 1.0; it = it->next; }

    it = f;
    while( it != e.End() ){ it->val = 0.0; it = it->next; }
    assert( w < rhs );

    REAL cost = EKPINF;

    auto sum_cost = [&](){
      auto it = e.Begin();
      REAL k = 0;
      while( it != f->next ){ k += it->cost*it->val; it = it->next; }
      return k;
    };

    auto search = [&](){
      auto left = e.Begin();
      auto right = f->next;
      while( left != f ){
        // set zero
        w -= left->weight;
        left->val = 0.0;

        while( right != e.End() ){
          // set one
          w += right->weight;
          right->val = 1.0;

          REAL c = sum_cost() + right->cost;

          if( rhs == w && c < cost ){
            e.solution(x);
            cost = c;
            found = true;
            assert( c == e.cost() );
          }

          // restore
          w -= right->weight;
          right->val = 0.0;

          right = right->next;
        }

        // restore
        w += left->weight;
        left->val = 1.0;

        left = left->next;
      }
    };

    f->val = 0.0;
    search();
    f->val = 1.0; w += f->weight;
    search();

    return found;
  }

}


#endif // EKP_KNAPSACK_INCUMBENT_HXX
