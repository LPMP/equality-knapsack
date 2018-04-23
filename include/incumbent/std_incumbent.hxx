#ifndef EKP_KNAPSACK_STD_INCUMBENT_HXX
#define EKP_KNAPSACK_STD_INCUMBENT_HXX

namespace ekp {

  template<typename EKP>
  class std_incumbent {
  public:

    std_incumbent(EKP& e,INDEX j) : e_(e),j_(j) { }

    void find(){

      assert( j_ < e_.numberOfVars() );
      assert( j_ > 0 );
      
      REAL z = 0;
      REAL w = 0;
      for(INDEX i=0;i < j_-1;i++){ z+= e_.cost(i);
      }

      for(INDEX i=j_+1;i<e_.numberOfVars();i++){
	
      }
      
    }
    
  private:
    EKP& e_;
    INDEX j_;
    REAL bestCost_ = EKPINF;
    INDEX xf_;
    INDEX xj_;
    
  };
  
}


#endif // EKP_KNAPSACK_STD_INCUMBENT_HXX
