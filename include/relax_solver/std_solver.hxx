#ifndef EKP_KNAPSACK_STD_SOLVER_HXX
#define EKP_KNAPSACK_STD_SOLVER_HXX

#include <config.hxx>
#include <math.h>

namespace ekp {

  template<typename ITEM>
  class std_relaxation{
  public:
    std_relaxation() { }

    REAL OptimalCost(){ return currentCost_; }
    ITEM* OptimalIndex(){ return currentF_; }
    bool isInteger(){ return integer_; }

    template<typename I>
    void solve(I begin,I end,REAL rhs){

      INDEX w = 0;
      REAL z = 0;

      auto it = begin;
      while( it != end ){
        if( w + it->weight > rhs ){
          z += (it->cost/it->weight)*(rhs - w);

          currentF_ = it;
          currentCost_ = z;
          break;
        }
        if( w + it->weight == rhs ){
          z += it->cost;

          currentCost_ = z;
          currentF_ = it;
          integer_ = true;
          break;
        }
        z += it->cost;
        w += it->weight;

        it = it->next;
      }

    }

  private:
    REAL currentCost_ = EKPINF;
    ITEM* currentF_;
    bool integer_ = false;
  };

}


#endif // EKP_KNAPSACK_STD_SOLVER_HXX
