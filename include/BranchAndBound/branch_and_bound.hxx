#ifndef EKP_KNAPSACK_BRANCH_AND_BOUND_HXX
#define EKP_KNAPSACK_BRANCH_AND_BOUND_HXX

#include <queue>
#include <tuple>
#include <iomanip>
#include <sstream>
#include <string>

#include <config.hxx>
#include <knapsack_object.hxx>
#include <solve_relaxation.hxx>
#include "BranchAndBound/pegging.hxx"
#include "BranchAndBound/incumbent.hxx"

namespace ekp {

  /**
  * @class visitor_empty
  */
  template<typename SOLVER>
  class visitor_empty {
  public:
    visitor_empty(){ }



    /**
    * @name Start/End
    * @brief This methods can be called between visit() and visit_end()
    */
    /// @{
    void Init(SOLVER* s){ solver_ = s; }
    void End(){ visit_end(); }
    /// @}

    /**
    * @name Presolve
    * @brief This methods can be called between visit() and visit_end()
    */
    /// @{
    bool presolve() { return presolve_; }
    bool node_presolve(){ return presolve_node_; }
    void Set_presolve(bool v){ presolve_ = v;  }
    void Set_node_presolve(bool v){ presolve_node_ = v; }
    /// @}

    /**
    * @name Iterations
    * @brief This methods can be called between visit() and visit_end()
    */
    /// @{
    template<typename EKP>
    void visit(EKP& e){ }
    void visit_end(){ }
    /// @}

    /**
    * @name Visit Methods
    * @brief This methods can be called between visit() and visit_end()
    */
    /// @{
    void SetNumberOfTreeNodes(INDEX n){ }
    bool continue_branching();
    void add_cutoff(){ }
    void add_integer(){ }
    void add_infeas(){  }
    void add_presolve_integer(){ }
    void add_pegged(INDEX n){ }
    void add_node(){ }
    /// @}
  protected:
    SOLVER* solver_;
    REAL tol_ = 1e-9;
    bool presolve_ = true;
    bool presolve_node_ = true;
  };

  /**
  * @name Visit Methods
  */
  /// @{
  template<typename SOLVER> bool visitor_empty<SOLVER>::continue_branching(){
    return (solver_->UpperBound() - solver_->LowerBound()) > tol_;
  }
  /// @}

  template<typename SOLVER>
  class visitor_base : public visitor_empty<SOLVER> {
  public:
    using BASE = visitor_empty<SOLVER>;

    visitor_base(TCLAP::CmdLine& cmd)
    : cmd_(cmd),
    verbose_("v","verbose","turn on log",cmd_),
    tol_("","gap","optimality gap",false,1e-9,"double",cmd_),
    log_sw_("n","","log every n-th iteration",false,1000,"integer",cmd_),
    log_head_("","n_head","print every n-th iteration the headline",false,10,"integer",cmd_),
    presolve_sw_("","no_presolve","turn off presolve",cmd_,true),
    presolve_node_sw_("","no_node_presolve","turn off presolve during branch&bound",cmd_,true)
    { }

    bool presolve(){ return presolve_sw_.getValue(); }
    bool node_presolve(){ return presolve_node_sw_.getValue(); }

    void add_presolve_integer(){
      visitor_empty<SOLVER>::add_presolve_integer();
      if( iter_ == 0 && verbose_.getValue() ){
        std::cout << "Presolve found a integer solution!" << std::endl;
      }
      //if( iter > 0 && verbose_.getValue() ){
      //
      //}
    }

    void add_cutoff(){ cut_off_ = true; }

    void add_integer(){
      visitor_empty<SOLVER>::add_integer();
      found_integer_ = true;
    }

    void add_node(){ nodes_++; }

    bool continue_branching(){
      bool btol = (BASE::solver_->UpperBound()-BASE::solver_->LowerBound()) > tol_.getValue();
      if( !btol ){
        print_string(15,"finish: ","");
        print_scientific(10,BASE::solver_->UpperBound()-BASE::solver_->LowerBound(),4,"");
        std::cout << std::endl;
      }
      return btol;
    }

    template<typename EKP>
    void visit(EKP& e){
      visitor_empty<SOLVER>::template visit<EKP>(e);
      free_vars_ = e.numberOfVars();
      fixed_vars_ = e.fixed();
      iter_++;
      if( nodes_ > 0 ){ nodes_--; }
    }

    void visit_end(){
      visitor_empty<SOLVER>::visit_end();
      if( (iter_-1) % log_sw_.getValue() == 0 || found_integer_ ){

        if( log_count_ % log_head_.getValue() == 0 ){
          print_headline(135);

          print_string(10,"iteration");
          print_string(12,"fixed/free ");
          print_string(10,"nodes   ");
          print_string(13,"lower bound ");
          print_string(13,"upper bound ");
          print_string(14,"gap     ");
          print_string(3,"cut");
          std::cout << std::endl;

          print_headline(135);
        }

        if( found_integer_ ){ print_integer(9,iter_,"*|"); }
        else { print_integer(10,iter_); }

        print_integer(5,fixed_vars_,"/");
        print_integer(6,free_vars_,"|",false);
        print_integer(10,nodes_);
        print_float(13,BASE::solver_->LowerBound(),5);
        print_float(13,BASE::solver_->UpperBound(),5);
        print_scientific(14,BASE::solver_->UpperBound()-BASE::solver_->LowerBound(),3);

        if( cut_off_ ){ print_string(3,"x "); }
        else{ print_string(3,""); }

        std::cout << std::endl;
        log_count_++;
      }
      found_integer_ = false;
      cut_off_ = false;
    }

    void End(){
      visitor_empty<SOLVER>::End();
      print_headline(135);
    }


  protected:
    TCLAP::CmdLine& cmd_;
    TCLAP::SwitchArg verbose_;
    TCLAP::SwitchArg presolve_sw_;
    TCLAP::SwitchArg presolve_node_sw_;
    TCLAP::ValueArg<INDEX> log_sw_;
    TCLAP::ValueArg<INDEX> log_head_;
    TCLAP::ValueArg<REAL> tol_;

    INDEX free_vars_ = 0;
    INDEX fixed_vars_ = 0;
    INDEX nodes_ = 0;

    INDEX iter_ = 0;
    INDEX log_count_ = 0;
    bool found_integer_ = false;
    bool cut_off_ = false;

    void print_headline(INDEX n){
      std::cout << "|" << std::setfill('-') << std::setw(n) << "|";
      std::cout << std::setfill(' '); std::cout << std::endl;
    }

    void print_string(INDEX n,std::string s,std::string e = "|",bool right = true){
      if( right ){ std::cout << std::right; }
      else { std::cout << std::left; }
      std::cout << std::setw(n) << s << e;
    }

    void print_integer(INDEX n,INDEX v,std::string s = "|",bool right = true){
      if( right ){ std::cout << std::right; }
      else { std::cout << std::left;}
      std::cout << std::setw(n) << v << s;
    }

    void print_float(INDEX n,REAL v,INDEX p,std::string s = "|",bool right = true){
      std::cout << std::fixed;
      if( right ){ std::cout << std::right; }
      else { std::cout << std::left; }
      std::cout << std::setw(n) << std::setprecision(p) << v;
      std::cout << s;
    }

    void print_scientific(INDEX n,REAL v,INDEX p,std::string s = "|",bool right = true){
      std::cout << std::fixed;
      if( right ){ std::cout << std::right; }
      else { std::cout << std::left; }
      std::cout << std::setw(n) << std::scientific << std::setprecision(p) << v;
      std::cout << s;
    }

  };

  template<typename SOLVER>
  class visitor_bb : public visitor_empty<SOLVER> {
  public:
    visitor_bb(){ }

    template<typename EKP>
    void visit(const INDEX iteration,const INDEX nodes,EKP& e,const double relax,const double integer,SOLVER* s){

      std::stringstream out;
      if( iteration % 1 == 0 ){
        if( head_ == 20 ){
          out << "|" << std::setfill('-') << std::setw(135) << "|";
          out << std::setfill(' ');
          out << std::endl;

          out << std::right << std::setw(10) << "Iteration" << " | ";
          out << std::right << std::setw(11) << "fixed/free " << " | ";
          out << std::right << std::setw(10) << "nodes   " << " | ";
          out << std::right << std::setw(12) << "lower bound" << " | ";
          out << std::right << std::setw(12) << "upper bound" << " | ";
          out << std::right << std::setw(10) << "gap   " << " | ";
          out << std::right << std::setw(10) << "infeas" << " | ";
          out << std::right << std::setw(10) << "cuts  " << " | ";
          out << std::right << std::setw(10) << "integer" << " | ";
          out << std::right << std::setw(12) << "nodes (add)" << " | ";
          //out << std::right << std::setw(9) << "flags    " ;
          out << std::endl;

          out << "|" << std::setfill('-') << std::setw(135) << "|";
          out << std::setfill(' ');
          out << std::endl;
          head_ = 0;
        }
        //printf("%010d  %03d/%03d  %7.4f/%7.4f",(int) iteration, (int) e.fixed(),(int) e.numberOfVars(),relax,integer);
        print_ = true;
        head_++;
      }
      out << std::right << std::setw(10) << iteration << " | ";
      out << std::right << std::setw(5) << e.fixed() << "/"
      << std::left  << std::setw(5) << e.numberOfVars() << " | ";
      out << std::right << std::setw(10) << nodes << " | ";
      out << std::fixed;
      out << std::right << std::setw(12) << std::setprecision(6) << relax << " | ";
      out << std::right << std::setw(12) << std::setprecision(6) << integer << " | ";
      out << std::right << std::setw(10) << std::scientific << std::setprecision(3) << integer - relax << " | ";

      out_ = out.str();

      count_nodes_ = 0;
    }

    void node_add(INDEX n){
      //if( n != 1 && n != 3 ){ print_ = true; }

      if( n == 0 ){ count_integer_++; print_ = true;}
      if( n == 1 ){ count_nodes_++; }
      if( n == 2 ){ count_cuts_++; }
      if( n == 3 ){ count_infeas_++; }
    }

    void End(){
      if(print_){
        std::cout << out_;
        std::cout << std::right << std::setw(10) << count_infeas_ << " | ";
        std::cout << std::right << std::setw(10) << count_cuts_ << " | ";
        std::cout << std::right << std::setw(10) << count_integer_ << " | ";
        std::cout << std::right << std::setw(12) << count_nodes_ << " | ";
        std::cout << std::endl;
        out_ = "";
      }
      print_ = false;
    }

    void presolve(const bool found,const INDEX count,const double relax,const double integer){
      printf("\n\n");

      if( found ){
        printf("Presolve found an integer solution with %.5f (%.5f)!\n",integer,relax);
        printf("Pegging fixed %d variables \n",(int) count);
      }
      else{
        printf("Presolve did not find any integer solution!\n");
      }

      printf("\n\n");
    }

  private:
    bool print_ = false;
    INDEX head_ = 20;
    std::string out_ = "";

    INDEX count_integer_ = 0;
    INDEX count_cuts_ = 0;
    INDEX count_infeas_ = 0;
    INDEX count_nodes_ = 0;
  };

  class branch_and_bound{
  public:
    using EKP = ekp_instance;
    using ITEM = typename EKP::knapsack_item;
    using node = std::tuple<REAL,EKP,ITEM*>;

    branch_and_bound(EKP& e)
    : e_(e) {  }

    template<typename EKP,typename VISITOR>
    void presolve(EKP& e,VISITOR& v){
      REAL relax_cost = e.cost();
      INDEX count = 0;
      bool found = calculate_incumbent(e,relax_.OptimalElement(),y_);
      if( found ){
        v.add_presolve_integer();
        REAL integer_cost = e.cost();
        count = knapsack_pegging(e,relax_.OptimalElement(),relax_cost,y_);
        v.add_pegged(count);
        if( integer_cost < bestIntegerCost_ ){
          bestIntegerCost_ = integer_cost;
          v.add_integer();
          std::swap(x_,y_);
        }
        solve_relax(e); e.solution(y_);
      }
    }

    void print_solution(EKP& e){
      std::vector<double> x;
      std::vector<char> xs;

      e.solution(x);
      e.solution_status(xs);

      printf("\n");
      for( auto v : xs ){ std::cout << v << " "; } std::cout << std::endl;
      for( auto v : x ){ printf("%d ",(int) v); }
      printf("  (%.5f/%.5f)\n\n",e.cost(),bestIntegerCost_);
    }

    void print_object(EKP& e){
      for( auto it=e.Begin();it!=e.End();it=it->next ){
        printf("%02d %3.6f  %d \n",(int) it->var,(double) it->cost,(int) it->weight);
      }
    }

    /**
    * \jantodo kopiere item "e" erst im If Block
    */
    template<typename Q,typename V>
    void add(EKP& e_orig,Q& node_queue,V& visitor){
      if( e_orig.feasible(bestIntegerCost_) ){
        EKP e = e_orig;
        solve_relax(e); e.solution(y_);

        if( e.cost() > bestIntegerCost_ ){
          visitor.add_cutoff();
          return void();
        }

        if( relax_.isInteger() ){
          std::swap(x_,y_);
          bestIntegerCost_ = e.cost();
          visitor.add_integer();
        } else {
          assert( e.cost() >= bestRelaxedCost_ );

          if( visitor.node_presolve() ){
            this->presolve(e,visitor);
          }
          visitor.add_node();
          node_queue.push(std::make_tuple(e.cost(),std::move(e),relax_.OptimalElement()));
        }
      } else { visitor.add_infeas(); } // infeasible
    }

    template<typename VISITOR>
    void calc_bb(VISITOR& v){

      auto compare = [](node l,node r){
        return std::get<0>(l) > std::get<0>(r);
      };

      std::priority_queue<node,std::vector<node>,decltype(compare)> node_queue(compare);
      EKP e_copy = e_;

      solve_relax(e_copy); e_copy.solution(y_);
      bestRelaxedCost_ = e_copy.cost();

      if( relax_.isInteger() && e_copy.cost() < bestIntegerCost_ ){
        std::swap(x_,y_);
        bestIntegerCost_ = bestRelaxedCost_;
        v.add_integer();
        return void();
      }

      node_queue.push(std::make_tuple(bestRelaxedCost_,std::move(e_copy),relax_.OptimalElement()));

      while( !node_queue.empty() ){
        bestRelaxedCost_ = std::get<0>(node_queue.top());
        auto e = std::get<1>(node_queue.top());

        v.visit(e);

        if( v.continue_branching() ){

          auto f = std::get<2>(node_queue.top());
          node_queue.pop();

          auto item = e.Begin();
          assert( item != NULL );

          item->val = 0.0;
          e.remove(item);
          add(e,node_queue,v);

          while( item != f->next ){
            e.restore();
            item->val = 1.0;
            if( e.rhs() < item->weight ){ break; }
            e.remove(item);
            item = item->next;
            item->val = 0.0;
            e.remove(item);
            add(e,node_queue,v);
          }
        }
        else {
          break;
        }
        v.visit_end();
      }
    }

    template<typename VISITOR>
    void solve(VISITOR& v){
      v.Init(this);
      if(v.presolve()){
        solve_relax(e_); e_.solution(y_);
        this->presolve(e_,v);
      }
      this->calc_bb(v);
      v.End();
    }

    void solve(){
      visitor_empty<branch_and_bound> v;
      this->solve(v);
    }

    REAL LowerBound(){ return bestRelaxedCost_; }
    REAL UpperBound(){ return bestIntegerCost_; }
    void Solution(std::vector<REAL>& x){
      x.resize(x_.size());
      for(INDEX i=0;i<x.size();i++){ x[i] = x_[i]; }
    }

  private:

    void solve_relax(EKP& e){
      relax_.solve(e.Begin(),e.End(),e.rhs());
    }

    EKP& e_;
    std_relaxation<ITEM> relax_;
    std::vector<REAL> x_;
    std::vector<REAL> y_;
    REAL bestRelaxedCost_ = EKPINF;
    REAL bestIntegerCost_ = EKPINF;
  };

}

#endif // EKP_KNAPSACK_BRANCH_AND_BOUND_HXX
