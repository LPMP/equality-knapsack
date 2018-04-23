#ifndef EKP_TEST_H
#define EKP_TEST_H

#include <stdexcept>
#include <string>

inline void test(const bool& pred)
{
  if (!pred)
    throw std::runtime_error("Test failed.");
}

#endif // EKP_TEST_H