#include <iostream>

#include "LUSOLTests.hpp"

int main(int, char**)
{
  ReSolve::tests::TestingResults result;

  {
    std::cout << "Running tests on CPU:\n";
    ReSolve::tests::LUSOLTests test;

    result += test.lusolConstructor();
    result += test.automaticFactorization();
    result += test.simpleSolve();

    std::cout << "\n";
  }

  return result.summary();
}
