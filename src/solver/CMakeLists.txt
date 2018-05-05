
pybind11_add_module(EKPSolvers branch_and_bound_py.cxx)
add_executable(bb_solver branch_and_bound.cxx)
