#ifndef EKP_KNAPSACK_STD_SOLVER_HXX
#define EKP_KNAPSACK_STD_SOLVER_HXX

#include <config.hxx>
#include <math.h>

namespace ekp {

  template<typename EKP>
  class std_relaxation{
  public:
    std_relaxation(EKP& e) : e_(e),currentJ_(e.numberOfVars()) { }

    REAL OptimalCost(){ return currentCost_; }
    INDEX OptimalIndex(){ return currentJ_; }
    bool isInteger(){ return integer_; }
    
    void solve(){

      REAL w = 0;
      REAL z = 0;
      
      for(INDEX i=0;i<e_.numberOfVars();i++){
	if( w + e_.weight(i) - e_.rhs() >= 1e-5  ){
	  z += (e_.cost(i)/e_.weight(i))*(e_.rhs() - w);

	  currentJ_ = i;
	  currentCost_ = z;
	  break;
	}
	if( std::fabs(w + e_.weight(i) - e_.rhs()) < 1e-5  ){
	  z += e_.cost(i);

	  currentCost_ = z;
	  currentJ_ = i;
	  integer_ = true;
	  break;
	}
	z += e_.cost(i);
	w += e_.weight(i);	
      }
      
    }
    
  private:
    EKP& e_;

    REAL currentCost_ = EKPINF;
    INDEX currentJ_;
    bool integer_ = false;
  };
  
}


#endif // EKP_KNAPSACK_STD_SOLVER_HXX
