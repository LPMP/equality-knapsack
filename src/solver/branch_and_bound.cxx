#include <iostream>
#include <tuple>

#include <tclap/CmdLine.h>
#include <knapsack_object.hxx>
#include <knapsack_parser.hxx>
#include "BranchAndBound/branch_and_bound.hxx"
#include "enumeration.hxx"

using tuple = std::tuple< double,double,std::vector<double> >;

using tuple2 = std::vector<size_t>;
using solver_enumeration = ekp::enumeration<ekp::ekp_instance>;

tuple2 enumeration_file(std::string f){
  auto m = ekp::parser::GetEKPData(f);
  auto ekp = ekp::ekp_instance(m);

  solver_enumeration solver(ekp);
  solver.solve();

  std::vector<size_t> x;
  solver.solution(x);

  return x;
}

tuple2 enumeration(std::vector<double> c,std::vector<size_t> w,size_t rhs){

  auto ekp = ekp::ekp_instance(c,w,rhs);
  solver_enumeration solver(ekp);
  solver.solve();

  std::vector<size_t> x;
  solver.solution(x);

  return x;
}

tuple branch_and_bound_file(std::string f){

  auto m = ekp::parser::GetEKPData(f);
  auto ekp = ekp::ekp_instance(m);

  ekp::branch_and_bound bb(ekp);
  bb.presolve();
  bb.solve();

  std::vector<double> x;
  bb.Solution(x);

  return std::make_tuple(bb.LowerBound(),bb.UpperBound(),x);
}

tuple branch_and_bound(std::vector<double> c,std::vector<size_t> w,size_t rhs){

  auto ekp = ekp::ekp_instance(c,w,rhs);

  ekp::branch_and_bound bb(ekp);
  bb.presolve();
  bb.solve();

  std::vector<double> x;
  bb.Solution(x);

  return std::make_tuple(bb.LowerBound(),bb.UpperBound(),x);
}



int main(int argc, char** argv){

  TCLAP::CmdLine cmd("Solve EKP with Branch and Bound", ' ', "0.1");
  TCLAP::ValueArg<std::string> file("i","file","filename",true,"","string",cmd);
  cmd.parse(argc,argv);

  branch_and_bound_file(file.getValue());

  return 0;
}
