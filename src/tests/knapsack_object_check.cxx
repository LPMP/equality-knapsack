#include <iostream>
#include "test_config.h"
#include <knapsack_object.hxx>
#include <knapsack_parser.hxx>
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


  {
    assert(ekp.rhs() == 5);

    auto k = ekp.Begin();
    auto end = ekp.End();

    assert(k->cost == c[0]);
    assert(k->weight == w[0]);
    k = k->next;
    assert(k->cost == c[2]);
    assert(k->weight == w[2]);
    k = k->next;
    assert(k->cost == c[3]);
    assert(k->weight == w[3]);
    k = k->next;
    assert(k->cost == c[1]);
    assert(k->weight == w[1]);
    assert( k->next == end );
  }

  {
    auto k = ekp.Begin();
    auto end = ekp.End();
    k = k->next;
    assert(k->val == 2);
    k->val = 1;
    ekp.remove(k);
    assert( ekp.rhs() == rhs - k->weight );
    ekp.restore(k);
    assert( ekp.rhs() == rhs );
  }

  {
    auto k = ekp.Begin();
    auto end = ekp.End();
    k = k->next;
    k->val = 0;
    ekp.remove(k);
    assert( ekp.rhs() == rhs );
    ekp.restore(k);
    assert( ekp.rhs() == rhs );
  }

  {
    auto k = ekp.Begin();
    auto end = ekp.End();
    k = k->next;
    k->val = 1;
    ekp.remove(k);

    ekp::ekp_instance ekp_copy(ekp);
    assert( ekp.numberOfVars() == ekp_copy.numberOfVars() + 1 );

    auto kc = ekp_copy.Begin();
    auto endc = ekp_copy.End();

    assert(kc->cost == c[0]);
    assert(kc->weight == w[0]);
    kc = kc->next;
    assert(kc->cost == c[3]);
    assert(kc->weight == w[3]);
    kc = kc->next;
    assert(kc->cost == c[1]);
    assert(kc->weight == w[1]);
    assert(kc->next == endc );
  }

}

int main(int argc, char** argv){
  // TCLAP::CmdLine cmd("Solve EKP", ' ', "0.1");
  // TCLAP::ValueArg<std::string> file("i","file","filename",true,"","string",cmd);
  // cmd.parse(argc,argv);

  ekp_object_test();//file.getValue());
  return 0;
}
