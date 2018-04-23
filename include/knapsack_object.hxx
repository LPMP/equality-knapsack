#ifndef EKP_KNAPSACK_OBJECT_HXX
#define EKP_KNAPSACK_OBJECT_HXX

#include <config.hxx>
#include <algorithm>


namespace ekp {

  class ekp_instance {
  public:

    template<typename M>
    ekp_instance(M& m){

      costs_ = m.costs;
      weights_ = m.weights;
      
      sorted_.resize(costs_.size());
      std::iota(sorted_.begin(),sorted_.end(),0);

      auto f = [this](INDEX i,INDEX j){
	REAL iv = costs_[i]/weights_[i];
	REAL jv = costs_[j]/weights_[j];
	return iv < jv;
      };
      std::sort(sorted_.begin(),sorted_.end(),f);

      b_ = m.b;
      nVars_ = costs_.size();
    }

    auto cost(INDEX i){ assert(i < costs_.size());  return costs_[sorted_[i]]; }
    auto weight(INDEX i){ assert(i < weights_.size()); return weights_[sorted_[i]]; }
    auto index(INDEX i){ assert(i < sorted_.size()); return sorted_[i]; };
    auto rhs(){ return b_; };
    auto numberOfVars(){ return nVars_; };
    
  private:

    std::vector<INDEX> sorted_;
    std::vector<REAL> costs_;
    std::vector<REAL> weights_;
    REAL b_;
    INDEX nVars_;
  };

}

#endif // EKP_KNAPSACK_OBJECT_HXX
