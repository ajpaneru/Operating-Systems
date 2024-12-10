#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20

// Matrix declarations
int matA[MAX][MAX];
int matB[MAX][MAX];
int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX];
int matProductResult[MAX][MAX];

// Structure to hold matrix cell indices
typedef struct {
      int row;
      int col;
} MatrixCell;

// Function to populate a matrix with random values
void fillMatrix(int matrix[MAX][MAX]) 
{
      for(int i = 0; i < MAX; i++) 
      {
            for(int j = 0; j < MAX; j++) 
            {
                  matrix[i][j] = rand() % 10 + 1;
            }
      }
}

// Function to display matrix contents
void printMatrix(int matrix[MAX][MAX]) 
{
      for(int i = 0; i < MAX; i++) 
      {
            for(int j = 0; j < MAX; j++) 
            {
                  printf("%5d", matrix[i][j]);
            }
            printf("\n");
      }
      printf("\n");
}

// Thread function to compute the sum of corresponding matrix cells
void* computeSum(void* args) 
{
      MatrixCell* cell = (MatrixCell*)args;
      int row = cell->row;
      int col = cell->col;

      matSumResult[row][col] = matA[row][col] + matB[row][col];
      free(cell);  // Free allocated memory for the matrix cell
      pthread_exit(0);
}

// Thread function to compute the difference of corresponding matrix cells
void* computeDiff(void* args) 
{
      MatrixCell* cell = (MatrixCell*)args;
      int row = cell->row;
      int col = cell->col;

      matDiffResult[row][col] = matA[row][col] - matB[row][col];
      free(cell);  // Free allocated memory for the matrix cell
      pthread_exit(0);
}

// Thread function to compute the product of corresponding matrix cells
void* computeProduct(void* args) 
{
      MatrixCell* cell = (MatrixCell*)args;
      int row = cell->row;
      int col = cell->col;

      matProductResult[row][col] = 0;

      for(int k = 0; k < MAX; k++) 
      {
            matProductResult[row][col] += matA[row][k] * matB[k][col];
      }
      free(cell);  // Free allocated memory for the matrix cell
      pthread_exit(0);
}

int main() 
{
      srand(time(0));  // Seed for random number generation

      // Initialize matrices with random values
      fillMatrix(matA);
      fillMatrix(matB);

      // Display the input matrices
      printf("Matrix A:\n");
      printMatrix(matA);
      
      printf("Matrix B:\n");
      printMatrix(matB);

      pthread_t threads[MAX * MAX * 3];  // Thread array for all operations
      int threadCount = 0;

      // Create threads for matrix operations
      for(int i = 0; i < MAX; i++) 
      {
            for(int j = 0; j < MAX; j++) 
            {
                  MatrixCell* cellSum = (MatrixCell*)malloc(sizeof(MatrixCell));
                  MatrixCell* cellDiff = (MatrixCell*)malloc(sizeof(MatrixCell));
                  MatrixCell* cellProd = (MatrixCell*)malloc(sizeof(MatrixCell));

                  cellSum->row = i;
                  cellSum->col = j;

                  cellDiff->row = i;
                  cellDiff->col = j;

                  cellProd->row = i;
                  cellProd->col = j;

                  pthread_create(&threads[threadCount++], NULL, computeSum, cellSum);
                  pthread_create(&threads[threadCount++], NULL, computeDiff, cellDiff);
                  pthread_create(&threads[threadCount++], NULL, computeProduct, cellProd);
            }
      }

      // Wait for all threads to complete execution
      for(int i = 0; i < threadCount; i++) 
      {
            pthread_join(threads[i], NULL);
      }

      // Print computed matrices
      printf("Sum Result:\n");
      printMatrix(matSumResult);

      printf("Difference Result:\n");
      printMatrix(matDiffResult);

      printf("Product Result:\n");
      printMatrix(matProductResult);

      return 0;
}
