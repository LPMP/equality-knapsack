#include "EKP_Object_test.cxx"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

PYBIND11_MODULE(EKPSolvers,m){
  m.doc() = "Wrapper for EKP test";
  
  m.def("ekp_test",&ekp_test,"Solving EKP");
  m.def("GetProblem",&GetProblem,"Get EKP instance");
}
