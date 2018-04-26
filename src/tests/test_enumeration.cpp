#include "knapsack_object.hxx"
#include "enumeration.hxx"
#include <knapsack_parser.hxx>
#include <iostream>

int main(int argc, char** argv)
{
    using solver_type = ekp::enumeration<ekp::ekp_instance>;

    std::string f = "002_MiniExample.txt";
    auto m = ekp::parser::GetEKPData(f);
    auto ekp = ekp::ekp_instance(m);

    solver_type solver(ekp);
    solver.solve(); 
}
