#ifndef EKP_KNAPSACK_STD_REDUCTION_HXX
#define EKP_KNAPSACK_STD_REDUCTION_HXX

namespace ekp {


  template<typename EKP>
  class std_reduction {
  public:
    std_reduction(EKP& e)
      : e_(e) { }

    template<typename ITEM>
    void incumbent(ITEM* f){

      ITEM* p = e_.Begin();
      ITEM* q = f->next;

      INDEX w = 0;
      while( p != f ){
        w += p->weight;
        p-> val = 1;
        p = p->next;
      }
      p = f->next;
      while( p!= e_.End() ){
        p->val = 0;
        p = p->next;
      }
      p = e_.Begin();

      for(INDEX i=0;i<2;i++){
        w += i*f->weight;
        f->val = i;

        while( p != f ){
          p->val = 0;
          q = f->next;
          while( q != e_.End() ){
            q->val = 1;
            if( w - p->weight + q->weight == e_.rhs() ){
              store_solution();
            }
            q->val = 0;
            q = q->next;
          }
          p->val = 1;
          p = p->next;
        }
      }

      if( bestCost_ < EKPINF ){
        e_.SetIntCost(bestCost_);
      }

    }

    void store_solution(){
      auto p = e_.Begin();
      REAL cost = 0;
      while( p != e_.End() ){
        cost += ((REAL) p->val)*p->cost;
        p = p->next;
      }
      if( cost < bestCost_ ){
        p = e_.Begin();
        while( p != e_.End() ){
          p->sol = p->val;
          p = p->next;
        }
        cost = bestCost_;
      }

    }

    // void pegging(){
    //
    //     std::vector<REAL> sol;
    //     e_.GetSolution(sol);
    //     REAL zr = e_.GetRelaxCost();
    //     REAL zi = e_.GetIntCost();
    //     REAL cwf = e_.cost(f_)/e_.weight(f_);
    //
    //     pegged_.resize(sol.size(),2);
    //
    //     for(INDEX i=0;i<sol.size();i++){
    //       if( sol[i] == 1 &&
    //          zr - e_.cost(i) + cwf*e_.weight(i) >= zi ){
    //            pegged_[i] = 1;
    //            fixed_++;
    //       } else if( sol[i] == 0 &&
    //          zr + e_.cost(i) - cwf*e_.weight(i) >= zi){
    //            pegged_[i] = 0;
    //            fixed_++;
    //       } else { unknown_++; }
    //       assert( sol[i] == 1 || sol[i] == 0 );
    //
    //     }
    // }

  private:
    EKP& e_;
    REAL bestCost_ = EKPINF;
  };

}

#endif // EKP_KNAPSACK_STD_REDUCTION_HXX
