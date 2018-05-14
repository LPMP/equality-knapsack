#include <iostream>
#include "test_config.h"
#include <knapsack_object.hxx>
#include <knapsack_parser.hxx>
#include <relax_solver/std_solver.hxx>
#include <reduction/std_reduction.hxx>
//#include <incumbent/std_incumbent.hxx>
#include <math.h>
#include <algorithm>
#include <tclap/CmdLine.h>
#include <map>



void ekp_object_test(std::string f){

  auto m = ekp::parser::GetEKPData(f);
  auto ekp = ekp::ekp_instance(m);

  auto p = ekp.Begin();
  while( p != ekp.End() ){
    printf("i= %3d  c = %.6f w = %d (%.3e)\n",(int) p->var,p->cost,(int) p->weight,p->cost/((ekp::REAL) p->weight));
    p = p->next;
  }
  printf("\n\n");

  auto ekp_relax = ekp::std_relaxation<ekp::ekp_instance::knapsack_item>();
  ekp_relax.solve(ekp.Begin(),ekp.End(),ekp.rhs());

  printf("optimalcost(%d): %.5f -> (%d)\n\n",(int) ekp_relax.OptimalIndex()->var,
  ekp_relax.OptimalCost(),(int) ekp_relax.isInteger());

  ekp.SetRelaxCost(ekp_relax.OptimalCost());
  if( !ekp_relax.isInteger() ){



  }

}

int main(int argc, char** argv){
  TCLAP::CmdLine cmd("Solve EKP", ' ', "0.1");
  TCLAP::ValueArg<std::string> file("i","file","filename",true,"","string",cmd);
  cmd.parse(argc,argv);

  ekp_object_test(file.getValue());
  return 0;
}
