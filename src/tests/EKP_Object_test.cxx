#include <iostream>
#include "test_config.h"
#include <knapsack_object.hxx>
#include <knapsack_parser.hxx>
#include <relax_solver/std_solver.hxx>
#include <math.h>
#include <algorithm>

int main(){
  
  std::string f = "002_MiniExample.txt";
  auto m = ekp::parser::GetEKPData(f);
  auto ekp = ekp::ekp_instance(m);

  auto ekp_relax = ekp::std_relaxation<ekp::ekp_instance>(ekp);
  ekp_relax.solve();

  if( ekp_relax.isInteger() ){
    printf("(RELAX) %d -> %.5f (I) \n",(int) ekp_relax.OptimalIndex(),ekp_relax.OptimalCost());
  } else {
    printf("(RELAX) %d -> %.5f (C) \n",(int) ekp_relax.OptimalIndex(),ekp_relax.OptimalCost());
  }

  
  
  return 0;
}
