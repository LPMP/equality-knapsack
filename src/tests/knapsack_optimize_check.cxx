#include <iostream>
#include "test_config.h"
#include <knapsack_object.hxx>
#include <knapsack_parser.hxx>
#include <solve_relaxation.hxx>
#include <math.h>
#include <algorithm>
#include <tclap/CmdLine.h>
#include <map>
#include <math.h>


void ekp_relaxation_test(){

  {
    std::vector<double> c = {0.1,0.3,0.2,0.5};
    std::vector<size_t> w = {1,1,3,2};
    size_t rhs = 5;
    auto ekp = ekp::ekp_instance(c,w,rhs);

    ekp::std_relaxation<ekp::ekp_instance::knapsack_item> solve_relax;
    solve_relax.solve(ekp.Begin(),ekp.End(),ekp.rhs());

    std::vector<double> x;
    ekp.solution(x);

    // printf("Solution: ");
    // for( auto k : x ){ printf(" %.4f",k); }
    // printf("\n");
    // printf("Cost: %.5f \n",ekp.cost());

    assert( ekp.cost() == 0.55 );
    assert( x[0] == 1 );
    assert( x[1] == 0 );
    assert( x[2] == 1 );
    assert( x[3] == 0.5 );
  }

  {
    std::vector<double> c = {0.1,0.3,0.2,0.5};
    std::vector<size_t> w = {1,1,3,2};
    size_t rhs = 5;
    auto ekp = ekp::ekp_instance(c,w,rhs);

    auto item = ekp.item(1);
    item->val = 1.0;
    ekp.remove(item);

    ekp::std_relaxation<ekp::ekp_instance::knapsack_item> solve_relax;
    solve_relax.solve(ekp.Begin(),ekp.End(),ekp.rhs());

    std::vector<double> x;
    ekp.solution(x);

    // printf("Solution: ");
    // for( auto k : x ){ printf(" %.4f",k); }
    // printf("\n");
    // printf("Cost: %.5f \n",ekp.cost());

    double cost = 10.0*ekp.cost();
    cost = round(cost)/10.0;

    assert( cost == 0.6 );
    assert( x[0] == 1 );
    assert( x[1] == 1 );
    assert( x[2] == 1 );
    assert( x[3] == 0 );
  }
}


int main(int argc, char** argv){
  ekp_relaxation_test();
  return 0;
}
