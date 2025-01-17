#include <string>
#include <iostream>
#include <iomanip>

#include <resolve/matrix/Coo.hpp>
#include <resolve/matrix/Csr.hpp>
#include <resolve/vector/Vector.hpp>
#include <resolve/matrix/io.hpp>
#include <resolve/matrix/MatrixHandler.hpp>
#include <resolve/vector/VectorHandler.hpp>
#include <resolve/LinSolverDirectSerialILU0.hpp>
#include <resolve/GramSchmidt.hpp>
#include <resolve/LinSolverIterativeRandFGMRES.hpp>
#include <resolve/workspace/LinAlgWorkspace.hpp>
#include <cmath>
using namespace ReSolve::constants;

int main(int argc, char *argv[])
{
  // Use the same data types as those you specified in ReSolve build.
  using real_type  = ReSolve::real_type;
  using vector_type = ReSolve::vector::Vector;

  (void) argc; // TODO: Check if the number of input parameters is correct.
  std::string  matrixFileName = argv[1];
  std::string  rhsFileName = argv[2];


  ReSolve::matrix::Coo* A_coo;
  ReSolve::matrix::Csr* A;
  ReSolve::LinAlgWorkspaceCpu* workspace_Cpu = new ReSolve::LinAlgWorkspaceCpu();
  workspace_Cpu->initializeHandles();
  ReSolve::MatrixHandler* matrix_handler =  new ReSolve::MatrixHandler(workspace_Cpu);
  ReSolve::VectorHandler* vector_handler =  new ReSolve::VectorHandler(workspace_Cpu);
  real_type* rhs = nullptr;
  real_type* x   = nullptr;

  vector_type* vec_rhs;
  vector_type* vec_x;
  vector_type* vec_r;

  ReSolve::GramSchmidt* GS = new ReSolve::GramSchmidt(vector_handler, ReSolve::GramSchmidt::mgs);

  ReSolve::LinSolverDirectSerialILU0* Rf = new ReSolve::LinSolverDirectSerialILU0(workspace_Cpu);
  ReSolve::LinSolverIterativeRandFGMRES* FGMRES = new ReSolve::LinSolverIterativeRandFGMRES(matrix_handler, vector_handler, ReSolve::LinSolverIterativeRandFGMRES::fwht, GS);

  std::cout << std::endl << std::endl << std::endl;
  std::cout << "========================================================================================================================"<<std::endl;
  std::cout << "Reading: " << matrixFileName<< std::endl;
  std::cout << "========================================================================================================================"<<std::endl;
  std::cout << std::endl;
  std::ifstream mat_file(matrixFileName);
  if(!mat_file.is_open())
  {
    std::cout << "Failed to open file " << matrixFileName << "\n";
    return -1;
  }
  std::ifstream rhs_file(rhsFileName);
  if(!rhs_file.is_open())
  {
    std::cout << "Failed to open file " << rhsFileName << "\n";
    return -1;
  }
  A_coo = ReSolve::io::readMatrixFromFile(mat_file);
  A = new ReSolve::matrix::Csr(A_coo->getNumRows(),
                               A_coo->getNumColumns(),
                               A_coo->getNnz(),
                               A_coo->symmetric(),
                               A_coo->expanded());

  rhs = ReSolve::io::readRhsFromFile(rhs_file);
  x = new real_type[A->getNumRows()];
  vec_rhs = new vector_type(A->getNumRows());
  vec_x = new vector_type(A->getNumRows());
  vec_x->allocate(ReSolve::memory::HOST);
  //iinit guess is 0U
  vec_x->allocate(ReSolve::memory::HOST);
  vec_x->setToZero(ReSolve::memory::HOST);
  vec_r = new vector_type(A->getNumRows());
  std::cout<<"Finished reading the matrix and rhs, size: "<<A->getNumRows()<<" x "<<A->getNumColumns()<< ", nnz: "<< A->getNnz()<< ", symmetric? "<<A->symmetric()<< ", Expanded? "<<A->expanded()<<std::endl;
  mat_file.close();
  rhs_file.close();

  A->updateFromCoo(A_coo, ReSolve::memory::HOST);
  vec_rhs->update(rhs, ReSolve::memory::HOST, ReSolve::memory::HOST);
  //Now call direct solver
  real_type norm_b;
  matrix_handler->setValuesChanged(true, ReSolve::memory::HOST);

  Rf->setup(A, nullptr, nullptr, nullptr, nullptr, vec_rhs);
  FGMRES->setRestart(800);
  FGMRES->setMaxit(800);
  FGMRES->setTol(1e-12);
  FGMRES->setup(A);
  GS->setup(FGMRES->getKrand(), FGMRES->getRestart()); 
  //matrix_handler->setValuesChanged(true, ReSolve::memory::HOST);
  FGMRES->resetMatrix(A);
  FGMRES->setupPreconditioner("LU", Rf);
  FGMRES->setFlexible(1); 

  vec_rhs->update(rhs, ReSolve::memory::HOST, ReSolve::memory::HOST);
  FGMRES->solve(vec_rhs, vec_x);

  norm_b = vector_handler->dot(vec_rhs, vec_rhs, ReSolve::memory::HOST);
  norm_b = sqrt(norm_b);
  std::cout << "FGMRES: init nrm: " 
    << std::scientific << std::setprecision(16) 
    << FGMRES->getInitResidualNorm()/norm_b
    << " final nrm: "
    << FGMRES->getFinalResidualNorm()/norm_b
    << " iter: " << FGMRES->getNumIter() << "\n";


  delete A;
  delete A_coo;
  delete Rf;
  delete [] x;
  delete [] rhs;
  delete vec_r;
  delete vec_x;
  delete workspace_Cpu;
  delete matrix_handler;
  delete vector_handler;
  return 0;
}
