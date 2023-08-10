#include "VectorHandler.hpp"
#include <iostream>
#include "cudaKernels.h"

namespace ReSolve
{
  VectorHandler::VectorHandler()
  {
  }

  VectorHandler:: VectorHandler(LinAlgWorkspace* new_workspace)
  {
    workspace_ = new_workspace;
  }

  VectorHandler::~VectorHandler()
  {
    //delete the workspace TODO
  }

  real_type VectorHandler::dot(Vector* x, Vector* y, std::string memspace)
  { 
    if (memspace == "cuda" ){ 
      LinAlgWorkspaceCUDA* workspaceCUDA = (LinAlgWorkspaceCUDA*) workspace_;
      cublasHandle_t handle_cublas =  workspaceCUDA->getCublasHandle();
      double nrm = 0.0;
    cublasStatus_t st= cublasDdot (handle_cublas,  x->getSize(), x->getData("cuda"), 1, y->getData("cuda"), 1, &nrm);
      if (st!=0) {printf("dot product crashed with code %d \n", st);}
      return nrm;
    } else {
      if (memspace == "cpu") {
        real_type* x_data = x->getData("cpu");
        real_type* y_data = y->getData("cpu");
        real_type sum = 0.0;
        real_type c;
        real_type t, y;
        for (int i = 0; i < x->getSize(); ++i){
          y = (x_data[i] * y_data[i]) - c;
          t = sum + y;
          c = (t - sum) - y;
          sum = t;        
//   sum += (x_data[i] * y_data[i]);
         } 
        return sum;
      } else {
        std::cout<<"Not implemented (yet)"<<std::endl;
        return NAN;
      }
    }
  }

  void VectorHandler::scal(real_type* alpha, Vector* x, std::string memspace)
  {
    if (memspace == "cuda" ) { 
      LinAlgWorkspaceCUDA* workspaceCUDA = (LinAlgWorkspaceCUDA*) workspace_;
      cublasHandle_t handle_cublas =  workspaceCUDA->getCublasHandle();
       cublasStatus_t st = cublasDscal(handle_cublas, x->getSize(), alpha, x->getData("cuda"), 1);
      if (st!=0) {printf("scal crashed with code %d \n", st);}
      
    } else {
      std::cout<<"Not implemented (yet)"<<std::endl;
    }
  }
  void VectorHandler::axpy( real_type* alpha, Vector* x, Vector* y, std::string memspace )
  {

    if (memspace == "cuda" ) { 
      LinAlgWorkspaceCUDA* workspaceCUDA = (LinAlgWorkspaceCUDA*) workspace_;
      cublasHandle_t handle_cublas =  workspaceCUDA->getCublasHandle();
      cublasDaxpy(handle_cublas,
                  x->getSize(),
                  alpha,
                  x->getData("cuda"),
                  1,
                  y->getData("cuda"),
                  1);
    } else {
      std::cout<<"Not implemented (yet)"<<std::endl;
    }
  }

  //gemv:
  //if transpose = N(no), x = beta*x +  alpha*V*y,
  //where x is [n x 1], V is [n x k] and y is [k x 1]
  //if transpose =T(yes), x = beta*x + alpha*V^T*y
  //where x is [k x 1], V is [n x k] and y is [n x 1] 
  void VectorHandler::gemv(std::string transpose, index_type n, index_type k, real_type* alpha, real_type* beta, real_type* V, real_type* y, real_type* x, std::string memspace)
  {
    if (memspace == "cuda") {
      LinAlgWorkspaceCUDA* workspaceCUDA = (LinAlgWorkspaceCUDA*) workspace_;
      cublasHandle_t handle_cublas =  workspaceCUDA->getCublasHandle();
      if (transpose == "T") {

        cublasDgemv(handle_cublas,
                    CUBLAS_OP_T,
                    n,
                    k,
                    alpha,
                    V,
                    n,
                    y,
                    1,
                    beta,
                    x,
                    1);

      } else {

        cublasDgemv(handle_cublas,
                    CUBLAS_OP_N,
                    n,
                    k,
                    alpha,
                    V,
                    n,
                    y,
                    1,
                    beta,
                    x,
                    1);
      }

    } else {
      std::cout<<"Not implemented (yet)"<<std::endl;
    }
  }

  //mass axpy: y = x*alpha  where x is [n x k] and alpha is [k x 1]; x is stored columnwise
  void VectorHandler::massAxpy(index_type size, real_type* alpha, real_type k, real_type* x, real_type* y, std::string memspace)
  {
    if (memspace == "cuda") {
      if (k < 200) {
        mass_axpy(size, k, x, y,alpha);
      } else {
        LinAlgWorkspaceCUDA* workspaceCUDA = (LinAlgWorkspaceCUDA*) workspace_;
        cublasHandle_t handle_cublas =  workspaceCUDA->getCublasHandle();
        cublasDgemm(handle_cublas,
                    CUBLAS_OP_N,
                    CUBLAS_OP_N,
                    size,//m
                    1,//n
                    k + 1,//k
                    &minusone_,//alpha
                    x,//A
                    size,//lda
                    alpha,//B
                    k + 1,//ldb
                    &one_,
                    y,//c
                    size);//ldc     
      }
    } else {
      std::cout<<"Not implemented (yet)"<<std::endl;
    }
  }

  //mass dot: V^T x, where V is [n x k] and x is [k x 2], everything is stored and returned columnwise
  void VectorHandler::massDot2Vec(index_type size, real_type* V, real_type k, real_type* x, real_type* res, std::string memspace)
  {

    if (memspace == "cuda") {
      if (k < 200) {
        mass_inner_product_two_vectors(size, k, x , &x[size], V, res);
      } else {
        LinAlgWorkspaceCUDA* workspaceCUDA = (LinAlgWorkspaceCUDA*) workspace_;
        cublasHandle_t handle_cublas =  workspaceCUDA->getCublasHandle();
        cublasDgemm(handle_cublas,
                    CUBLAS_OP_T,
                    CUBLAS_OP_N,
                    k + 1,   //m
                    2,       //n
                    size,    //k
                    &one_,   //alpha
                    V,       //A
                    size,    //lda
                    x,       //B
                    size,    //ldb
                    &zero_,
                    res,     //c
                    k + 1);  //ldc 
      }
    } else {
      std::cout<<"Not implemented (yet)"<<std::endl;
    }
  }

}