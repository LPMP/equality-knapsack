#ifndef EKP_KNAPSACK_STD_REDUCTION_HXX
#define EKP_KNAPSACK_STD_REDUCTION_HXX

namespace ekp {


  template<typename EKP>
  class std_reduction {
  public:
    std_reduction(EKP& e,INDEX f)
      : e_(e),f_(f) { }

    void pegging(){

        std::vector<REAL> sol;
        e_.GetSolution(sol);
        REAL zr = e_.GetRelaxedOptimal();
        REAL zi = e_.GetIntegerOptimal();
        REAL cwf = e_.cost(f_)/e_.weight(f_);

        pegged_.resize(sol.size(),2);

        for(INDEX i=0;i<sol.size();i++){
          if( sol[i] == 1 &&
             zr - e_.cost(i) + cwf*e_.weight(i) >= zi ){
               pegged_[i] = 1;
               fixed_++;
          } else if( sol[i] == 0 &&
             zr + e_.cost(i) - cwf*e_.weight(i) >= zi){
               pegged_[i] = 0;
               fixed_++;
          } else { unknown_++; }
          assert( sol[i] == 1 || sol[i] == 0 );

        }
    }

    INDEX fixed(){ return fixed_; }
    INDEX unkown(){ return unknown_; }
    INDEX pegged(INDEX i){ assert(i < pegged_.size()); return pegged_[i]; }

  private:
    EKP& e_;
    const INDEX f_;
    std::vector<INDEX> pegged_;
    INDEX fixed_ = 0;
    INDEX unknown_ = 0;
  };

}

#endif // EKP_KNAPSACK_STD_REDUCTION_HXX
