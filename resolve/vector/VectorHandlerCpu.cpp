#include <iostream>

#include <resolve/utilities/logger/Logger.hpp>
#include <resolve/cuda/cudaKernels.h>
#include <resolve/vector/Vector.hpp>
#include <resolve/workspace/LinAlgWorkspace.hpp>
#include <resolve/vector/VectorHandlerImpl.hpp>
#include "VectorHandlerCpu.hpp"

namespace ReSolve {
  using out = io::Logger;

  /** 
   * @brief empty constructor that does absolutely nothing        
   */
  VectorHandlerCpu::VectorHandlerCpu()
  {
  }

  /** 
   * @brief constructor
   * 
   * @param new_workspace - workspace to be set     
   */
  VectorHandlerCpu:: VectorHandlerCpu(LinAlgWorkspaceCpu* new_workspace)
  {
    workspace_ = new_workspace;
  }

  /** 
   * @brief destructor     
   */
  VectorHandlerCpu::~VectorHandlerCpu()
  {
    //delete the workspace TODO
  }

  /** 
   * @brief dot product of two vectors i.e, a = x^Ty
   * 
   * @param[in] x The first vector
   * @param[in] y The second vector
   * @param[in] memspace String containg memspace (cpu or cuda)
   * 
   * @return dot product (real number) of _x_ and _y_
   */

  real_type VectorHandlerCpu::dot(vector::Vector* x, vector::Vector* y)
  { 
    real_type* x_data = x->getData("cpu");
    real_type* y_data = y->getData("cpu");
    real_type sum = 0.0;
    real_type c = 0.0;
    // real_type t, y;
    for (int i = 0; i < x->getSize(); ++i) {
      real_type y = (x_data[i] * y_data[i]) - c;
      real_type t = sum + y;
      c = (t - sum) - y;
      sum = t;        
      //   sum += (x_data[i] * y_data[i]);
    } 
    return sum;
  }

  /** 
   * @brief scale a vector by a constant i.e, x = alpha*x where alpha is a constant
   * 
   * @param[in] alpha The constant
   * @param[in,out] x The vector
   * @param memspace string containg memspace (cpu or cuda)
   * 
   */
  void VectorHandlerCpu::scal(const real_type* alpha, vector::Vector* x)
  {
    real_type* x_data = x->getData("cpu");

    for (int i = 0; i < x->getSize(); ++i){
      x_data[i] *= (*alpha);
    }
  }

  /** 
   * @brief axpy i.e, y = alpha*x+y where alpha is a constant
   * 
   * @param[in] alpha The constant
   * @param[in] x The first vector
   * @param[in,out] y The second vector (result is return in y)
   * @param[in]  memspace String containg memspace (cpu or cuda)
   * 
   */
  void VectorHandlerCpu::axpy(const  real_type* alpha, vector::Vector* x, vector::Vector* y)
  {
    //AXPY:  y = alpha * x + y
    real_type* x_data = x->getData("cpu");
    real_type* y_data = y->getData("cpu");
    for (int i = 0; i < x->getSize(); ++i) {
      y_data[i] = (*alpha) * x_data[i] + y_data[i];
    }
  }

  /** 
   * @brief gemv computes matrix-vector product where both matrix and vectors are dense.
   *        i.e., x = beta*x +  alpha*V*y
   *
   * @param[in] Transpose - yes (T) or no (N)
   * @param[in] n Number of rows in (non-transposed) matrix
   * @param[in] k Number of columns in (non-transposed)   
   * @param[in] alpha Constant real number
   * @param[in] beta Constant real number
   * @param[in] V Multivector containing the matrix, organized columnwise
   * @param[in] y Vector, k x 1 if N and n x 1 if T
   * @param[in,out] x Vector, n x 1 if N and k x 1 if T
   * @param[in] memspace  cpu or cuda (for now)
   *
   * @pre   V is stored colum-wise, _n_ > 0, _k_ > 0
   * 
   */  
  void VectorHandlerCpu::gemv(std::string /* transpose */,
                              index_type /* n */,
                              index_type /* k */,
                              const real_type* /* alpha */,
                              const real_type* /* beta */,
                              vector::Vector* /* V */,
                              vector::Vector* /* y */,
                              vector::Vector* /* x */)
  {
    out::error() << "Not implemented (yet)" << std::endl;
  }

  /** 
   * @brief mass (bulk) axpy i.e, y = y - x*alpha where  alpha is a vector
   * 
   * @param[in] size number of elements in y
   * @param[in] alpha vector size k x 1
   * @param[in] x (multi)vector size size x k
   * @param[in,out] y vector size size x 1 (this is where the result is stored)
   * @param[in] memspace string containg memspace (cpu or cuda)
   *
   * @pre   _k_ > 0, _size_ > 0, _size_ = x->getSize()
   *
   */
  void VectorHandlerCpu::massAxpy(index_type /* size */, vector::Vector* /* alpha */, index_type /* k */, vector::Vector* /* x */, vector::Vector* /* y */)
  {
    out::error() << "Not implemented (yet)" << std::endl;
  }

  /** 
   * @brief mass (bulk) dot product i.e,  V^T x, where V is n x k dense multivector (a dense multivector consisting of k vectors size n)  
   *        and x is k x 2 dense multivector (a multivector consisiting of two vectors size n each)
   * 
   * @param[in] size Number of elements in a single vector in V
   * @param[in] V Multivector; k vectors size n x 1 each
   * @param[in] k Number of vectors in V
   * @param[in] x Multivector; 2 vectors size n x 1 each
   * @param[out] res Multivector; 2 vectors size k x 1 each (result is returned in res)
   * @param[in] memspace String containg memspace (cpu or cuda)
   *
   * @pre   _size_ > 0, _k_ > 0, size = x->getSize(), _res_ needs to be allocated
   *
   */
  void VectorHandlerCpu::massDot2Vec(index_type /* size */, vector::Vector* /* V */, index_type /* k */, vector::Vector* /* x */, vector::Vector* /* res */)
  {
    out::error() << "Not implemented (yet)" << std::endl;
  }

} // namespace ReSolve
