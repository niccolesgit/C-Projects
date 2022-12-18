#include "mult.h"
matrix* mm_matrix_mult(matrix* mat1, matrix* mat2)
{
/*
Notes:
  First check that it is actually possible to multiply the two inputs
  If so, construct a result matrix
  Then perform the multiplication
  Then return a pointer to your result matrix
*/
//if the row and column don't match, there's an error between the matricies
  if (mat1->cols != mat2->rows) {
    printf("mm_matrix_mult: dimension mismatch between matrices.\n");
    exit(-1);
  }
  //creates a new matrix to hold the result
  matrix* result_matrix = (matrix*)malloc(sizeof(matrix));
  //copies over values from the matricies
  result_matrix->rows = mat1->rows;
  result_matrix->cols = mat2->cols;
  mm_alloc(result_matrix);
  
  //loops through both matricies and multiplies them 
  for(int i = 0; i < mat1->rows; ++i) {
    for(int j = 0; j < mat2->cols; ++j) {
      for(int k = 0; k < mat1->cols; ++k) {
        //adds them to the location in the result matrix
        result_matrix->data[i][j] +=  mat1->data[i][k] * mat2->data[k][j];
      }
    }
  }
  
  return result_matrix;
}