#include <string>
#include <iostream>
#include <fstream>
#include <resolve/matrix/Coo.hpp>
#include <resolve/matrix/io.hpp>
#include "MatrixTests.hpp"

int main(int argc, char* argv[])
{
  ReSolve::tests::MatrixTests test;

  // Input to this code is location of `data` directory where matrix files are stored
  const std::string data_path = (argc == 2) ? argv[1] : "./";

  std::string matrixFileName = data_path + "matrix/data/matrix_general_coo_ordered.mtx";
  // std::cout << "Opening file " << matrixFileName << " ...\n";
  std::ifstream file(matrixFileName);
  if(!file.is_open())
  {
    std::cout << "Failed to open file ...\n";
    return -1;
  }
  test.cooMatrixImport();
  test.cooMatrixImport2(file);
  test.cooMatrixReadAndUpdate();
  test.rhsVectorReadFromFile();
  test.rhsVectorReadAndUpdate();

  file.close();

  return 0;
}