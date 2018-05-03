#ifndef EKP_KNAPSACK_STD_REDUCTION_HXX
#define EKP_KNAPSACK_STD_REDUCTION_HXX

namespace ekp {


  template<typename EKP>
  class std_reduction {
  public:
    std_reduction(EKP& e,INDEX f)
      : e_(e),f_(f) { }

    void pegging(){

      

    }

  private:
    EKP& e_;
    const INDEX f_;
    std::vector<REAL> pegged_;
  };

}

#endif // EKP_KNAPSACK_STD_REDUCTION_HXX
