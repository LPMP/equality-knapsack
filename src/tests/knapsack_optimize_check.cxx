#include <iostream>
#include <math.h>
#include <algorithm>
#include <tclap/CmdLine.h>
#include <map>
#include <stack>
#include <math.h>
#include <tuple>

#include "test_config.h"
#include <knapsack_object.hxx>
#include <knapsack_parser.hxx>
#include <solve_relaxation.hxx>
#include "BranchAndBound/pegging.hxx"
#include "BranchAndBound/incumbent.hxx"
#include "BranchAndBound/branch_and_bound.hxx"
#include "enumeration.hxx"

double round(double a,size_t n = 0){
  return std::round(std::pow(10.0,n)*a)/std::pow(10.0,n);
}

void PrintSolution(ekp::ekp_instance& e){
  std::vector<double> x;
  e.solution(x);
  size_t w = 0;

  printf("-------------------------------------------------------");
  printf("\n");

  printf("%-20s ","");
  for(auto it=e.Begin();it!=e.End();it=it->next){ printf(" %3.0f ",(double) it->var); }
  printf("\n");

  printf("%-20s:","solution (sorted)");
  for(auto it=e.Begin();it!=e.End();it=it->next){ printf(" %.2f",it->val); w +=((double) it->weight)*it->val; }
  printf("\n\n");


  printf("%-20s ","");
  for(size_t i=0;i<e.numberOfVars();i++){ printf(" %3.0f ",(double) i); }
  printf("\n");
  printf("%-20s:","solution (unsorted)");
  for(auto v : x ){ printf(" %.2f",v); }
  printf("\n\n");
  printf("%20s: %.0f == %.1f (%.2e) \n","constraint",(double) e.rhs(),(double) w,std::fabs(w-((double) e.rhs())));
  printf("%20s: %.5f\n","optimal cost",e.cost());
  printf("-------------------------------------------------------\n\n");
}

using solver_type = ekp::enumeration<ekp::ekp_instance>;

void ekp_branch_check_002(){
  std::vector<double> c = {0.29702142830793665, 0.6745502238048002, 1.107260954765586, 0.7064556873643915, 1.2651679337852038, 1.1509290520327573, 1.1679088868211753, 1.0587595066112934, 1.1219243406684403, 1.2818408049568233};
  std::vector<size_t> w = {8, 11, 12, 7, 11, 9, 9, 8, 8, 8};
  size_t rhs = 58;
  ekp::std_relaxation<ekp::ekp_instance::knapsack_item> solve_relax;
  auto ekp = ekp::ekp_instance(c,w,rhs);

  ekp::branch_and_bound bb(ekp);
  bb.presolve();
  bb.solve();

  std::vector<double> sol = {1,1,1,1,1,1,0,0,0,0};

  std::vector<double> x;
  bb.Solution(x);
  for(size_t i=0;i<x.size();i++){ assert( x[i] == sol[i] ); }
}

void ekp_enum_check_001(){
  std::vector<double> c = {1.0828797670063957, 0.4436115763580485, 1.187170333397907, 1.343593386871627, 1.3111571404683648};
  std::vector<size_t> w = {12, 7, 7, 6, 8};
  size_t rhs = 22;
  ekp::std_relaxation<ekp::ekp_instance::knapsack_item> solve_relax;
  auto ekp = ekp::ekp_instance(c,w,rhs);

  solver_type solver(ekp);
  solver.solve();

  std::vector<size_t> x;
  bool found = solver.solution_valid(x);
}

void ekp_branch_check_001(){
  std::vector<double> c = {1.0828797670063957, 0.4436115763580485, 1.187170333397907, 1.343593386871627, 1.3111571404683648};
  std::vector<size_t> w = {12, 7, 7, 6, 8};
  size_t rhs = 22;
  ekp::std_relaxation<ekp::ekp_instance::knapsack_item> solve_relax;
  auto ekp = ekp::ekp_instance(c,w,rhs);

  ekp::branch_and_bound bb(ekp);
  bb.presolve();
  bb.solve();

  std::vector<double> x;
  bb.Solution(x);
  for( auto v : x ){ assert( v == 0.0 || v == 1.0 ); }
}

void ekp_branch_check(){
  std::vector<double> c = {0.9063014555369366, 1.1017854118138553, 0.953227439835186, 0.8494630980106754, 1.0706706675178057, 0.470918422958737};
  std::vector<size_t> w = {11, 7, 11, 6, 12, 8};
  size_t rhs = 28;
  ekp::std_relaxation<ekp::ekp_instance::knapsack_item> solve_relax;
  auto ekp = ekp::ekp_instance(c,w,rhs);
  ekp::visitor_bb visitor;

  ekp::branch_and_bound bb(ekp);
  bb.presolve();
  bb.solve(visitor);

  std::vector<double> x;
  bb.Solution(x);

  size_t wk = 0;
  assert( x.size() == w.size() );
  std::vector<double> sol = {1,0,1,1,0,0};
  for(size_t i=0;i<x.size();i++){
    wk += x[i]*w[i];
    assert( x[i] == sol[i] );
  }
  assert( rhs == wk );
  assert( bb.UpperBound() - bb.LowerBound() < 1e-6 );

  assert( round(bb.UpperBound(),5) == 2.70899 );
}

void ekp_pegging_check(){
  std::vector<double> c = {0.1,0.1,0.3,0.5};
  std::vector<size_t> w = {2,1,3,2};
  size_t rhs = 4;
  auto ekp = ekp::ekp_instance(c,w,rhs);

  ekp::std_relaxation<ekp::ekp_instance::knapsack_item> solve_relax;
  solve_relax.solve(ekp.Begin(),ekp.End(),ekp.rhs());

  std::vector<double> x;
  ekp.solution(x);

  assert( x[0] == 1.0 );
  assert( x[1] == 0.0 );
  assert( round(x[2],10) == round(2.0/3.0,10) );
  assert( x[3] == 0.0 );
  assert( round(ekp.cost(),10) == 0.3 );
  double relax_cost = ekp.cost();

  std::vector<double> incumbent;
  bool found = calculate_incumbent(ekp,solve_relax.OptimalElement(),incumbent);
  assert( found == true );
  assert( incumbent[0] == 0.0 );
  assert( incumbent[1] == 1.0 );
  assert( incumbent[2] == 1.0 );
  assert( incumbent[3] == 0.0 );
  assert( round(ekp.cost(),10) == 0.4 );

  size_t pegged = knapsack_pegging(ekp,solve_relax.OptimalElement(),relax_cost);

  ekp::ekp_instance ekp_copy(ekp);
  assert( ekp.numberOfVars() == ekp_copy.numberOfVars()+1 );
  assert( ekp.rhs() == ekp_copy.rhs() );

}

void ekp_incumbent_check(){
  std::vector<double> c = {0.1,0.1,0.3,0.5};
  std::vector<size_t> w = {2,1,3,2};
  size_t rhs = 4;
  auto ekp = ekp::ekp_instance(c,w,rhs);

  ekp::std_relaxation<ekp::ekp_instance::knapsack_item> solve_relax;
  solve_relax.solve(ekp.Begin(),ekp.End(),ekp.rhs());

  std::vector<double> x;
  ekp.solution(x);

  assert( x[0] == 1.0 );
  assert( x[1] == 0.0 );
  assert( round(x[2],10) == round(2.0/3.0,10) );
  assert( x[3] == 0.0 );
  assert( round(ekp.cost(),5) == 0.3 );

  std::vector<double> incumbent;
  bool found = calculate_incumbent(ekp,solve_relax.OptimalElement(),incumbent);
  assert( found == true );
  assert( incumbent[0] == 0.0 );
  assert( incumbent[1] == 1.0 );
  assert( incumbent[2] == 1.0 );
  assert( incumbent[3] == 0.0 );
  assert( round(ekp.cost(),10) == 0.4 );
}

void ekp_relaxation_test(){

  {
    std::vector<double> c = {0.1,0.3,0.2,0.5};
    std::vector<size_t> w = {1,1,3,2};
    size_t rhs = 5;
    auto ekp = ekp::ekp_instance(c,w,rhs);

    ekp::std_relaxation<ekp::ekp_instance::knapsack_item> solve_relax;
    solve_relax.solve(ekp.Begin(),ekp.End(),ekp.rhs());

    std::vector<double> x;
    ekp.solution(x);

    // printf("Solution: ");
    // for( auto k : x ){ printf(" %.4f",k); }
    // printf("\n");
    // printf("Cost: %.5f \n",ekp.cost());

    assert( ekp.cost() == 0.55 );
    assert( x[0] == 1 );
    assert( x[1] == 0 );
    assert( x[2] == 1 );
    assert( x[3] == 0.5 );
  }

  {
    std::vector<double> c = {0.1,0.3,0.2,0.5};
    std::vector<size_t> w = {1,1,3,2};
    size_t rhs = 5;
    auto ekp = ekp::ekp_instance(c,w,rhs);

    auto item = ekp.item(1);
    item->val = 1.0;
    ekp.remove(item);

    ekp::std_relaxation<ekp::ekp_instance::knapsack_item> solve_relax;
    solve_relax.solve(ekp.Begin(),ekp.End(),ekp.rhs());

    std::vector<double> x;
    ekp.solution(x);

    // printf("Solution: ");
    // for( auto k : x ){ printf(" %.4f",k); }
    // printf("\n");
    // printf("Cost: %.5f \n",ekp.cost());

    assert( x[0] == 1 );
    assert( x[1] == 1 );
    assert( x[2] == 1 );
    assert( x[3] == 0 );
    assert( round(ekp.cost(),1) == 0.6 );
  }
}


int main(int argc, char** argv){
  ekp_relaxation_test();
  ekp_incumbent_check();
  ekp_pegging_check();
  //ekp_branch_check();
  //ekp_enum_check_001();
  //ekp_branch_check_001();
  ekp_branch_check_002();
  return 0;
}
