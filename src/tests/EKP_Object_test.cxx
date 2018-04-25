#include <iostream>
#include "test_config.h"
#include <knapsack_object.hxx>
#include <knapsack_parser.hxx>
#include <relax_solver/std_solver.hxx>
#include <incumbent/std_incumbent.hxx>
#include <math.h>
#include <algorithm>
#include <tclap/CmdLine.h>

std::vector<ekp::REAL> ekp_test(std::string f){

  //std::string f = file.getValue();//"002_MiniExample.txt";
  auto m = ekp::parser::GetEKPData(f);
  auto ekp = ekp::ekp_instance(m);

  /**
  * Solve Relaxation
  **/
  auto ekp_relax = ekp::std_relaxation<ekp::ekp_instance>(ekp);
  ekp_relax.solve();

  std::vector<ekp::REAL> v;
  v.push_back(ekp_relax.OptimalCost());
  v.push_back(ekp_relax.OptimalIndex());

  /**
  * Try to find an incumbent solution
  **/
  if( !ekp_relax.isInteger() ){
    auto ekp_incumbent = ekp::std_incumbent<ekp::ekp_instance>(ekp,ekp_relax.OptimalIndex());
    ekp_incumbent.find();

    v.push_back(ekp_incumbent.IndexK());
    v.push_back(ekp_incumbent.ValueF());
    v.push_back(ekp_incumbent.IndexJ());
    v.push_back(ekp_incumbent.BestCost());

    if( ekp_incumbent.BestCost() < ekp::EKPINF ){
      size_t w = 0;
      size_t k = ekp_incumbent.IndexK();
      size_t f = ekp_relax.OptimalIndex();
      size_t j = ekp_incumbent.IndexJ();
      size_t xf = ekp_incumbent.ValueF();

      assert( k < f );
      assert( f < j );
      assert( j < ekp.numberOfVars() );
      assert( xf <= 1 );
      for(size_t i=0;i < f;i++){
        if( i != k){
          w += ekp.weight(i);
        }
      }
      w += ekp.weight(j);
      w += xf*ekp.weight(f);
      assert( w == ekp.rhs() );
    }
  }

  return v;
}

int main(int argc, char** argv){
  TCLAP::CmdLine cmd("Solve EKP", ' ', "0.1");
  TCLAP::ValueArg<std::string> file("i","file","filename",true,"","string",cmd);
  cmd.parse(argc,argv);

  ekp_test(file.getValue());
  return 0;
}
