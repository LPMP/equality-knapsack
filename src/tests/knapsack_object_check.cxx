#include <iostream>
#include <math.h>
#include <algorithm>
#include <map>
#include <tclap/CmdLine.h>

#include "test_config.h"
#include <knapsack_object.hxx>
#include <knapsack_parser.hxx>
#include <solve_relaxation.hxx>


void PrintSolution(ekp::ekp_instance& e,bool print = true){
  std::vector<double> x;
  e.solution(x);
  size_t w = 0;

  if( print ){
    printf("-------------------------------------------------------");
    printf("\n");

    printf("%-20s ","");
    for(auto it=e.Begin();it!=e.End();it=it->next){ printf(" %3.0f ",(double) it->var); }
    printf("\n");

    printf("%-20s:","solution (sorted)");
    for(auto it=e.Begin();it!=e.End();it=it->next){ printf(" %.2f",it->val); w +=((double) it->weight)*it->val; }
    printf("\n\n");


    printf("%-20s ","");
    for(size_t i=0;i<x.size();i++){ printf(" %3.0f ",(double) i); }
    printf("\n");
    printf("%-20s:","solution (unsorted)");
    for(auto v : x ){ printf(" %.2f",v); }
    printf("\n\n");
    printf("%20s: %.0f == %.1f (%.2e) \n","constraint",(double) e.rhs(),(double) w,std::fabs(w-((double) e.rhs())));
    printf("%20s: %.5f\n","optimal cost",e.cost());
    printf("-------------------------------------------------------\n\n");
  }
}

void ekp_object_copy_test(){

  std::vector<double> c = {0.9063014555369366, 1.1017854118138553, 0.953227439835186, 0.8494630980106754, 1.0706706675178057, 0.470918422958737};
  std::vector<size_t> w = {11, 7, 11, 6, 12, 8};
  size_t rhs = 28;
  bool print = false;

  auto ekp = ekp::ekp_instance(c,w,rhs);
  ekp::std_relaxation<ekp::ekp_instance::knapsack_item> solve_relax;

  solve_relax.solve(ekp.Begin(),ekp.End(),ekp.rhs());
  PrintSolution(ekp,print);

  auto item = ekp.item(2);

  item->val = 0.0;
  ekp.remove(item);
  ekp::ekp_instance ekp_copy = ekp;
  ekp.restore();

  item->val = 1.0;
  ekp.remove(item);
  ekp::ekp_instance ekp_copy_copy = ekp;
  ekp.restore();

  solve_relax.solve(ekp_copy.Begin(),ekp_copy.End(),ekp_copy.rhs());
  solve_relax.solve(ekp_copy_copy.Begin(),ekp_copy_copy.End(),ekp_copy_copy.rhs());

  PrintSolution(ekp_copy,print);
  PrintSolution(ekp_copy_copy,print);

  assert( ekp.numberOfVars() == ekp_copy.numberOfVars() + 1 );
  assert( ekp.numberOfVars() == ekp_copy_copy.numberOfVars() + 1 );

  assert( ekp_copy.cost() > ekp.cost() );
  assert( ekp_copy_copy.cost() > ekp.cost() );

  assert( ekp_copy.weight() == rhs );
  assert( ekp_copy_copy.weight() == rhs );
}

void ekp_object_test(){ //std::string f){

  //auto m = ekp::parser::GetEKPData(f);

  std::vector<double> c = {0.1,0.3,0.2,0.5};
  std::vector<size_t> w = {2,1,3,4};
  size_t rhs = 5;

  auto ekp = ekp::ekp_instance(c,w,rhs);


  {
    assert(ekp.rhs() == 5);

    auto k = ekp.Begin();
    auto end = ekp.End();

    assert(k->cost == c[0]);
    assert(k->weight == w[0]);
    k = k->next;
    assert(k->cost == c[2]);
    assert(k->weight == w[2]);
    k = k->next;
    assert(k->cost == c[3]);
    assert(k->weight == w[3]);
    k = k->next;
    assert(k->cost == c[1]);
    assert(k->weight == w[1]);
    assert( k->next == end );
  }

  {
    auto k = ekp.Begin();
    auto end = ekp.End();
    k = k->next;
    k->val = 1;
    ekp.remove(k);
    assert( ekp.rhs() == rhs - k->weight );
    ekp.restore();
    assert( ekp.rhs() == rhs );
  }

  {
    auto k = ekp.Begin();
    auto end = ekp.End();
    k = k->next;
    k->val = 0;
    ekp.remove(k);
    assert( ekp.rhs() == rhs );
    ekp.restore();
    assert( ekp.rhs() == rhs );
  }

  {
    auto k = ekp.Begin();
    auto end = ekp.End();
    k = k->next;
    k->val = 1;
    ekp.remove(k);

    ekp::ekp_instance ekp_copy(ekp);
    assert( ekp.numberOfVars() == ekp_copy.numberOfVars() + 1 );

    auto kc = ekp_copy.Begin();
    auto endc = ekp_copy.End();

    assert(kc->cost == c[0]);
    assert(kc->weight == w[0]);
    kc = kc->next;
    assert(kc->cost == c[3]);
    assert(kc->weight == w[3]);
    kc = kc->next;
    assert(kc->cost == c[1]);
    assert(kc->weight == w[1]);
    assert(kc->next == endc );
  }

}

int main(int argc, char** argv){
  // TCLAP::CmdLine cmd("Solve EKP", ' ', "0.1");
  // TCLAP::ValueArg<std::string> file("i","file","filename",true,"","string",cmd);
  // cmd.parse(argc,argv);

  ekp_object_test();//file.getValue());
  ekp_object_copy_test();
  return 0;
}
