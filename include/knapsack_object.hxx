#ifndef EKP_KNAPSACK_OBJECT_HXX
#define EKP_KNAPSACK_OBJECT_HXX

#include <config.hxx>
#include <algorithm>



namespace ekp {

  /**
    @brief Stores c,w,b for an Equality Knapsack Problem

    Vectors: c (costs), w (weights) and the right-hand-side b.

    min_x <c,x> s.t. <w,x> = b, x_i = 0 or x_i = 1
  */
  class ekp_instance {
  public:

    template<typename M>
    ekp_instance(M& m)
      : ekp_instance(m.costs,m.weights,m.b) { }

    ekp_instance(std::vector<REAL> c,std::vector<INDEX> w,INDEX b)
      : costs_(c),weights_(w),b_(b),nVars_(costs_.size()) {

      assert( weights_.size() > 2 );
      assert( weights_.size() == costs_.size() );
      assert( weights_.size() == nVars_ );

      sorted_.resize(nVars_);
      std::iota(sorted_.begin(),sorted_.end(),0);

      auto f = [this](INDEX i,INDEX j){
        REAL iv = costs_[i]/((REAL) weights_[i]);
        REAL jv = costs_[j]/((REAL) weights_[j]);
        return iv < jv;
      };
      std::sort(sorted_.begin(),sorted_.end(),f);
    }

    void SetCost(INDEX i,REAL value){ assert(i < costs_.size()); costs_[sorted_[i]] = value; }

    auto cost(INDEX i){ assert(i < costs_.size());  return costs_[sorted_[i]]; }
    auto weight(INDEX i){ assert(i < weights_.size()); return weights_[sorted_[i]]; }
    auto index(INDEX i){ assert(i < sorted_.size()); return sorted_[i]; };
    auto rhs(){ return b_; };
    auto numberOfVars(){ return nVars_; };

  private:

    std::vector<INDEX> sorted_;
    std::vector<REAL> costs_;
    std::vector<INDEX> weights_;
    INDEX b_;
    INDEX nVars_;
  };

}

#endif // EKP_KNAPSACK_OBJECT_HXX
