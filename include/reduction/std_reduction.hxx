#ifndef EKP_KNAPSACK_STD_REDUCTION_HXX
#define EKP_KNAPSACK_STD_REDUCTION_HXX

namespace ekp {


  template<typename EKP>
  class std_reduction {
  public:
    std_reduction(EKP& e,INDEX j) : e_(e),j_(j) { }



  private:
    EKP& e_;

  };

}

#endif // EKP_KNAPSACK_STD_REDUCTION_HXX
