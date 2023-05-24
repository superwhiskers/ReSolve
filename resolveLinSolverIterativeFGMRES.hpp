#pragma once
#include "resolveCommon.hpp"
#include "resolveMatrix.hpp"
#include "resolveLinSolver.hpp"

namespace ReSolve 
{
  class resolveLinSolverIterativeFGMRES : public resolveLinSolverIterative
  {
    public:
      resolveLinSolverIterativeFGMRES();
      resolveLinSolverIterativeFGMRES(resolveInt restart, resolveReal tol, resolveInt maxit);
      ~resolveLinSolverIterativeFGMRES();

      int solve(resolveVector* rhs, resolveVector* x);
    private:
      //remember matrix handler and vector handler are inherited.
      resolveLinAlgWorkspace* workspace_;
      
      resolveReal tol_;
      resolveInt maxit_;
      resolveInt restart_;
      
      resolveReal* V;


  };

}
