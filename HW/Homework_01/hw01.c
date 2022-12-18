/* 
Assignment 1: NxM Matrix Multiplication 
CSCI-2500 Fall 2021
*/

/******************************************************************************/
/* Usual suspects to include  */
/******************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

/******************************************************************************/
/* Function Calls */
/******************************************************************************/

typedef struct{
  double** data;
  int rows;
  int cols;
} matrix; 

int mm_alloc(matrix* mat);
int mm_free (matrix* mat);
int mm_print(matrix* mat);
int mm_read (char* filename, matrix* mat);
matrix* mm_matrix_mult(matrix* mat1, matrix* mat2);


int mm_alloc(matrix* mat)
{
/*
Notes:
  Allocate an array of pointers to a double
  Then, allocate an row/array of doubles and assign each double pointer that row
  Make sure you're allocating using mat->data in the matrix struct
  You can use calloc for allocation and zeroing out all the bytes
  - otherwise, you'd want to manually set all arrays to 0 (good practice)
  If your malloc/calloc returns null, the allocation failed
*/
  mat->data = calloc(mat->rows, sizeof(double*));
  //if there's nothing in the file
  if (mat->data == NULL) {
    //print error
    printf("mm_alloc: allocation failed.\n");
    exit(-1);
  }
  //if you go though all the rows 
  for (int i = 0; i < mat->rows; ++i) {
    mat->data[i] = (double *)calloc(mat->cols, sizeof(double));
    //if they're empty
    if(mat->data[i] == NULL) {
      //print error
      printf("mm_alloc: allocation failed.\n");
      exit(-1);
    }
  }
  
  return 0;
}

int mm_free(matrix* mat)
{
/*
Notes:
  Reverse the allocation process, but you'll be calling free() instead
*/
//goes through all the rows and frees all the memory
  for (int i = 0; i < mat->rows; ++i) {
    free(mat->data[i]);
  }
  //also frees the matrix itself
  free(mat->data);
  /* note: free does not return a value */
  
  return 0;
}

int mm_print(matrix* mat)
{
/*
Notes:
  Print all the data values in your matrix
  Go row by row
  At the end of each row, print a newline
  Between each data value, insert a tab
  Use 2 digits of precision after the decimal (use "%10.2lf\t" as format string)
  See output.txt for expected output formatting
*/
//printing based on format in output.txt
  printf("\n/******** START of 2-D Matrix **********************************/\n");
  //goes through and prints each element in the matrix
  for (int i = 0; i < mat->rows; ++i) {
    for (int j = 0; j < mat->cols; ++j) {
      //uses formatting in the notes above)
      printf("%10.2lf\t", mat->data[i][j]);
    }
    printf("\n");
  }
  printf("/******** END of 2-D Matrix ************************************/\n");
  
  return 0;
}

int mm_read(char* filename, matrix* mat) 
{
/*
Notes:
  Use fopen to open the matrix file, then use fgets or getline to read lines
  The first line of the matrix contains integers for "#rows #columns"
  - Use these to allocate your arrays for matrix data
  Each subsequent line contains a single data value
  - You can use sscanf to parse each matrix entry
  - First value is data[0][0], next is data[0][1] ... data[#rows-1][#columns-1]
*/
//creates a file to hold the matrix data
  FILE *m_file = NULL;
  mat->data = NULL;
  //creates a bufffer for the data so it can be editied
  // used 256 from 16*16 (hopefully big enough)
  char buff_matrix[256];
  
  //if the file doesn't exist, print error msg
  if (NULL == (m_file = fopen(filename, "r"))) {
    printf("mm_read: failed to open file.");
    exit(-1);
  }
  
  //if the file is empty, print error msg
  if (NULL == fgets(buff_matrix, 256, m_file)) {
    printf("mm_read: failed to read from file.");
    exit(-1);
  }
  //if the number of integers is not equal to 2 on the first row
  // then the matrix file has an invalid matrix
  // if it's not at least 2x2 or 1x2 then it cant be a matrix
  if (2 != sscanf(buff_matrix, "%d %d", &mat->rows, &mat->cols)) {
    printf("mm_read: failed to read matrix dimensions.\n");
    exit(-1);
  }
  
  mm_alloc(mat);
  
  //read the whole matrix, if there's any empty lines, 
  // there's an error and can't be read properly
  for (int i = 0; i < mat->rows; ++i) {
    for (int j = 0; j < mat->cols; ++j) {
      if (NULL == fgets(buff_matrix, 256, m_file)) {
        printf("mm_read: failed to read matrix values.\n");
        exit(-1);
      }
      //if there's any lines that don't have a float value, 
      // there's also an error
      if (1 != sscanf(buff_matrix, "%lf", &mat->data[i][j])) {
        printf("mm_read: failed to read matrix values.\n");
        exit(-1);
      }
    }
  }
  //also checks if there's an error closing a file. 
  if(0 != fclose(m_file)) {
    printf("mm_read: cannot close file: %s\n", strerror(errno));
  }
  
  return 0;
}


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

int main()
{
  /*
   * you must keep this function AS IS. 
   * you cannot modify it!
   */
  char filename_A[256];
  char filename_B[256];
  matrix* A = (matrix*)malloc(sizeof(matrix));
  matrix* B = (matrix*)malloc(sizeof(matrix));
  matrix* C = NULL;

  printf("Please enter the matrix A data file name: ");
  if (1 != scanf("%s", filename_A)) {
    printf("main: scanf failed to read filename\n");
    exit(-1);
  }
  mm_read(filename_A, A);
  mm_print(A);
  
  printf("Please enter the matrix B data file name: ");
  if (1 != scanf("%s", filename_B)) {
    printf("main: scanf failed to read filename\n");
    exit(-1);
  }
  mm_read(filename_B, B);
  mm_print(B);
  
  C = mm_matrix_mult(A, B);
  mm_print(C);

  mm_free(A);
  mm_free(B);
  mm_free(C);
  free(A);
  free(B);
  free(C);
  
  return 0;
}
