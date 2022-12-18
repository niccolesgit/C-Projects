#include "read.c"
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
