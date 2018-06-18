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

  // TCLAP::CmdLine cmd("Solve EKP with Branch and Bound", ' ', "0.1");
  // TCLAP::ValueArg<std::string> file("i","file","filename",true,"","string",cmd);
  // cmd.parse(argc,argv);
  //
  // branch_and_bound_file(file.getValue());

  std::vector<double> c = {0.29702142830793665, 0.7623949627668456, 0.8519767354273359, 0.6161492014988188, 0.7064556873643915, 0.6745502238048002, 1.1219105276895143, 1.1509290520327573, 1.0737145508533699, 1.097038302974693, 1.1219243406684403, 1.4702897658670926, 1.107260954765586, 0.7472896197584549, 0.9867415597949147, 1.2818408049568233, 1.1679088868211753, 1.315332603883953, 1.0587595066112934, 1.2651679337852038};
  std::vector<size_t> w = {6, 12, 12, 8, 9, 8, 12, 12, 11, 11, 11, 12, 9, 6, 7, 9, 8, 8, 6, 7};
  assert( c.size() == w.size() );
  enumeration(c,w,61);
>>>>>>> jan_dev

  return 0;
}
