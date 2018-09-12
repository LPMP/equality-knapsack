#ifndef EKP_KNAPSACK_SOLVE_RELAXATION_HXX
#define EKP_KNAPSACK_SOLVE_RELAXATION_HXX

#include <config.hxx>
#include <math.h>

namespace ekp {

  template<typename ITEM>
  class std_relaxation{
  public:
    std_relaxation() { }

    REAL OptimalCost(){ return currentCost_; }
    ITEM* OptimalElement(){ return currentF_; }
    bool isInteger(){ return integer_; }

    template<typename I>
    void solve(I begin,I end,REAL rhs){

      INDEX w = 0;
      REAL z = 0;

      auto it = begin;

      if( rhs == 0){
        it = end;
        integer_ = true;
        currentF_ = begin;
        currentCost_ = 0.0;
      }

      while( it != end ){
        it->val = 1.0;
        if( w + it->weight > rhs ){
          it->val =((REAL) (rhs - w))/((REAL) it->weight);
          z += it->val*it->cost;

          currentF_ = it;
          currentCost_ = z;

          integer_ = false;
          break;
        }
        if( w + it->weight == rhs ){
          z += it->cost;

          currentF_ = it;
          currentCost_ = z;

          integer_ = true;
          break;
        }
        z += it->cost;
        w += it->weight;

        it = it->next;
      }

      if( it != end ){
        assert( fabs(w + it->val*it->weight - rhs) < 1e-10 );
      } else {
        assert( w  == rhs );
      }

    }

  private:
    REAL currentCost_ = EKPINF;
    ITEM* currentF_;
    bool integer_ = false;
  };

}


#endif // EKP_KNAPSACK_SOLVE_RELAXATION_HXX
