#include "branch_and_bound.cxx"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

PYBIND11_MODULE(EKPSolvers,m){
  m.doc() = "Wrapper for EKP solvers";

  m.def("SolveEKP_bb",&branch_and_bound,"Using Branch and Bound to solve EKP!");
  m.def("SolveEKP_bb",&branch_and_bound_file,"Using Branch and Bound to solve EKP!");

  // m.def("SolveEKP_enum",&enumeration,"Using Branch and Bound to solve EKP!");
  // m.def("SolveEKP_enum",&enumeration_file,"Using Branch and Bound to solve EKP!");
}
