#include <iostream>
#include "test_config.h"
#include <knapsack_object.hxx>
#include <knapsack_parser.hxx>
#include <relax_solver/std_solver.hxx>
#include <reduction/std_reduction.hxx>
#include <incumbent/std_incumbent.hxx>
#include <math.h>
#include <algorithm>
#include <tclap/CmdLine.h>
#include <map>



void ekp_object_test(std::string f){

  auto m = ekp::parser::GetEKPData(f);
  auto ekp = ekp::ekp_instance(m);

  for(auto p=ekp.Begin();p!=ekp.End();p++){
    printf("i= %3d  c = %.6f w = %8.0f (%.3e)\n",(int) (*p)->var,(*p)->cost,(*p)->weight,(*p)->cost/(*p)->weight);
  }
  printf("\n");

  ekp.solve_relaxation();


}

int main(int argc, char** argv){
  TCLAP::CmdLine cmd("Solve EKP", ' ', "0.1");
  TCLAP::ValueArg<std::string> file("i","file","filename",true,"","string",cmd);
  cmd.parse(argc,argv);

  ekp_object_test(file.getValue());
  return 0;
}
