#include <iostream>
#include "test_config.h"
#include <knapsack_object.hxx>
#include <knapsack_parser.hxx>
#include <solve_relaxation.hxx>
#include <math.h>
#include <algorithm>
#include <tclap/CmdLine.h>
#include <map>



void ekp_object_test(){ //std::string f){

  //auto m = ekp::parser::GetEKPData(f);

  std::vector<double> c = {0.1,0.3,0.2,0.5};
  std::vector<size_t> w = {2,1,3,4};
  size_t rhs = 5;

  auto ekp = ekp::ekp_instance(c,w,rhs);

  auto p = ekp.Begin();
  while( p != ekp.End() ){
    printf("i= %3d  c = %.6f w = %d (%.3e)\n",(int) p->var,p->cost,(int) p->weight,p->cost/((double) p->weight));
    p = p->next;
  }
  printf("\n\n");



}

int main(int argc, char** argv){
  // TCLAP::CmdLine cmd("Solve EKP", ' ', "0.1");
  // TCLAP::ValueArg<std::string> file("i","file","filename",true,"","string",cmd);
  // cmd.parse(argc,argv);

  ekp_object_test();//file.getValue());
  return 0;
}
