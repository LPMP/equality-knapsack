#ifndef EKP_CONFIG_HXX
#define EKP_CONFIG_HXX

#include <assert.h>
#include <meta/meta.hpp>
#include <limits>

namespace ekp{

  using REAL = double;
  using INDEX = size_t;
  auto EKPINF = std::numeric_limits<REAL>::infinity();
  
}

#endif // EKP_CONFIG_HXX
