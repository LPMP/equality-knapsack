#ifndef EKP_KNAPSACK_STD_INCUMBENT_HXX
#define EKP_KNAPSACK_STD_INCUMBENT_HXX

#include <math.h>

namespace ekp {

  template<typename ITEM>
  class std_incumbent {
  public:

    std_incumbent() {}

    template<typename I>
    void find(){

      assert( f_ < e_.numberOfVars() - 1 );
      assert( f_ > 0 );

      REAL z = 0;
      INDEX w = 0;

      auto incumbent = [&,this](REAL xf,INDEX k){
        z += e_.cost(f_)*xf;
        w += e_.weight(f_)*xf;
        for(INDEX i=f_+1;i<e_.numberOfVars();i++){
          if( w + e_.weight(i) == e_.rhs() ){
            if( z + e_.cost(i) < bestCost_ ){
              j_ = i;
              k_ = k;
              xf_ = (INDEX) xf;
              bestCost_ = z + e_.cost(i);
            }
          }
        }
      };

      auto sumup = [&,this](INDEX k){
        w = 0;
        for(INDEX i=0;i<f_;i++){
          if( i != k ){
            z += e_.cost(i);
            w += e_.weight(i);
          }
        }
      };

      for(INDEX i=0;i<f_;i++){
        sumup(i);
        incumbent(0.0,i);
        incumbent(1.0,i);
      }

      if( bestCost_ < EKPINF ){
        e_.SetIntegerOptimal(bestCost_);
        for(INDEX i=0;i<f_;i++){
          e_.SetSolution(i,1);
        }
        for(INDEX i=f_+1;i<e_.numberOfVars();i++){
          e_.SetSolution(i,0);
        }
        assert( xf_ < 2 );
        e_.SetSolution(f_,xf_);
        e_.SetSolution(k_,0);
        e_.SetSolution(j_,1);
      }

    }

    REAL BestCost(){ return bestCost_; }
    INDEX IndexJ(){ return j_; }
    INDEX IndexK(){ return k_; }
    INDEX ValueF(){ return xf_; }

  private:
    EKP& e_;
    const INDEX f_;
    INDEX k_;
    INDEX j_;
    REAL bestCost_ = EKPINF;
    INDEX xf_ = 2;

  };

}


#endif // EKP_KNAPSACK_STD_INCUMBENT_HXX
