#pragma once
#include <resolve/matrix/Sparse.hpp>
#include <resolve/cusolver_defs.hpp>

namespace ReSolve { namespace matrix {

  class Csr : public Sparse
  {
    public:
      Csr();

      Csr(index_type n, index_type m, index_type nnz);

      Csr(index_type n, 
          index_type m, 
          index_type nnz,
          bool symmetric,
          bool expanded);

      ~Csr();

      virtual index_type* getRowData(std::string memspace);
      virtual index_type* getColData(std::string memspace);
      virtual real_type* getValues(std::string memspace); 

      virtual int updateData(index_type* row_data, index_type* col_data, real_type* val_data, std::string memspaceIn, std::string memspaceOut); 
      virtual int updateData(index_type* row_data, index_type* col_data, real_type* val_data, index_type new_nnz, std::string memspaceIn, std::string memspaceOut); 

      virtual int allocateMatrixData(std::string memspace); 

      virtual void print() {return;}

      int copyCsr(std::string memspaceOut);
    private:
  };

}} // namespace ReSolve::matrix
