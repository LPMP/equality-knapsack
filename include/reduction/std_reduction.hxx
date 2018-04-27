#ifndef EKP_KNAPSACK_STD_REDUCTION_HXX
#define EKP_KNAPSACK_STD_REDUCTION_HXX

namespace ekp {


  template<typename EKP>
  class std_reduction {
  public:
    std_reduction(EKP& e,INDEX f,REAL rz, REAL iz,std::vector<INDEX> s)
      : e_(e),f_(f),s_(s),rz_(rz),iz_(iz) { }

    void pegging(){



    }

  private:
    EKP& e_;
    const INDEX f_;
    const std::vector<INDEX> s_;
    const REAL rz_;
    const REAL iz_;
    std::vector<REAL> pegged_;
  };

}

#endif // EKP_KNAPSACK_STD_REDUCTION_HXX
