#include <stdio.h> /* FILE, printf, getc */
#include <stdlib.h> /* atoi, malloc, free */
#include <string.h> /* memset */
#include <limits.h> /* INT_MAX */

#include <test_input.h> /* HandleInput */

#define N 5
#define FALSE 0
#define TRUE 1

static void RunMagic(FILE* in); 
static int ParseInput(FILE* in, int* all_valid, int* is_basic, int magic_matrix[][N]);
static void CheckValidity(int curr, int* all_valid, int* is_basic, char* num_LUT);
static void HandleInputErr(int scan_res, int index);
static void PrintMatrix(int magic_matrix[][N]);
static int CalculateIsMagic(int magic_matrix[][N]);
static int CheckCols(int magic_matrix[][N], int* sum);
static int CheckDiag(int magic_matrix[][N], int* sum);
static int CheckRows(int magic_matrix[][N], int* sum);

int main(int argc, char *argv[])
{
    printf("\n\n************************* Welcome to the MAGIC MATRIX! ************************\n*\n");
    printf("* This program will receive %d numbers as input from the user,\n", N*N);
    printf("* will print the matrix input and after calculation will\n");
    printf("* determine if the matrix is a magic matrix or not.\n*\n");

    printf("* A basic magic matrix has the same sum in all\n");
    printf("* rows, columns and the main two diagonals,\n");
    printf("* and all numbers are different from each other.\n*\n");

    printf("* If user input is larger or smaller than the matrix,\n");
    printf("* or if user input is not an integer, (white spaces allowed),\n");
    printf("* then the program will end.\n*\n");

    printf("* User input can be used using:\n");
    printf("* the terminal command < and a file name to be used as stdin,\n");
    printf("* by mentioning several files names when running the program,\n");
    printf("* or by running the program and entering the input manually (finish with EOF)\n*\n");

    printf("********************************   Enjoy.  ************************************\n\n");

    HandleInput(argc, argv, RunMagic);

    return 0;
}

void RunMagic(FILE* in)
{
    int magic_matrix[N][N] = {0};
    int input_range_valid = TRUE;
    int is_basic = TRUE;
    
    if(ParseInput(in, &input_range_valid, &is_basic, magic_matrix))
    {
        PrintMatrix(magic_matrix);
        
        if(CalculateIsMagic(magic_matrix))
        {
            if(is_basic && input_range_valid)
              printf("the matrix is basic magical!\n");
            else
              printf("the matrix is magical, but not basic!\n");
        }
        else
            printf("the matrix is not basic and not magical..\n");
    }
    
    printf("Goodbye.\n\n");
}

int ParseInput(FILE* in, int* all_valid, int* is_basic, int magic_matrix[][N])
{
    char* num_LUT = NULL;
    int curr = 0;
    int i = 0;
    int scan_ret_val = 0;
    
    /* 
        using look-up-table (LUT) to verify all numbers are different,
        and to check each of the numbers in O(1) time complexity.
    */
    num_LUT = (char*)malloc(N * N + 1); 
    
    if(!num_LUT)
    {
      printf("allocation fail, sorry..\n");
      return FALSE;
    }
    
    memset(num_LUT, 0, N * N + 1); /* initialize num_LUT - all 0's*/ 
    
    while ((scan_ret_val = fscanf(in, "%d", &curr)) != EOF &&  /* not End Of File && scanned successfully at least 1 item. */
            scan_ret_val != 0)
    {
        if(i < N*N)
        {              
            CheckValidity(curr, all_valid, is_basic, num_LUT);
            magic_matrix[i / N][i % N] = curr;
        }    
        else
        {
            ++i; /* for err handling */
            break;
        }    

        ++i;
    }
    
    free(num_LUT);
    num_LUT = NULL;
    
    if(i == N * N && scan_ret_val == EOF) /* read input successfully, matrix is full */
    {
        return TRUE;
    }
    else
    {
        HandleInputErr(scan_ret_val, i);
        return FALSE;
    }
    
    return i == N * N;
}

void CheckValidity(int curr, int* all_valid, int* is_basic, char* num_LUT)
{
  /* if input is out of valid range, we do not check if magic to save time. */
  if(curr < 1 || curr > N*N) 
  {
      *all_valid = FALSE;
  }
  else if(num_LUT[curr]) /* number is valid but already exist in matrix */
  {
      *is_basic = FALSE;
  }
  else                   /* number is valid and not yet exist in matrix */
  {
      ++num_LUT[curr];
  }
}

static void HandleInputErr(int scan_res, int index)
{
    printf("input is invalid! : ");
    
    if(index > N * N)
    {
        printf("too many variables for matrix, %d\n", index);
    }
    else if(scan_res == EOF)
    {
        printf("too few variables for matrix, %d\n", index);
    }
    else
    {
        printf("not a an integer.\n");
    }
}

void PrintMatrix(int magic_matrix[][N])
{
    int r = 0, c = 0;
    
    for(r = 0; r < N; ++r) /* rows */
    {
        for(c = 0; c < N - 1; ++c) /* columns */
        {
            printf("%3d\t", magic_matrix[r][c]);
        }

        printf("%3d\n", magic_matrix[r][c]);
    }
}

int CalculateIsMagic(int magic_matrix[][N])
{
    int sum = 0;
    if(CheckRows(magic_matrix, &sum))
        if(CheckCols(magic_matrix, &sum))
            return CheckDiag(magic_matrix, &sum);
    return 0;
}

int CheckRows(int magic_matrix[][N], int* sum)
{
    int r = 0, c = 0;
    int curr_sum = 0;

    for(r = 0; r < N; ++r) /* rows */
    {
        curr_sum = 0;

        for(c = 0; c < N; ++c) /* columns */
        {
            curr_sum += magic_matrix[r][c];
            
            if(curr_sum < 0) /* if sum passed the max of signed int */
                return FALSE;  
        }

        if(0 == r) /* determine the first sum */
            *sum = curr_sum;
        else if(*sum != curr_sum)
            return FALSE;
    }

    return TRUE;
}


int CheckCols(int magic_matrix[][N], int* sum)
{
    int r = 0, c = 0;
    int curr_sum = 0;
    
    for(c = 0; c < N; ++c) /* columns */
    {
        curr_sum = 0;

        for(r = 0; r < N; ++r) /* rows */
        {
            curr_sum += magic_matrix[r][c];

            if(curr_sum < 0) /* if sum passed the max of signed int */
                return FALSE;    
        }

        if(*sum != curr_sum)
            return FALSE;
    }

    return TRUE;
}

int CheckDiag(int magic_matrix[][N], int* sum)
{
    int d = 0, u = 0;
    int curr_sum = 0;
    
    for(d = 0; d < N; ++d) /* up left to down right diag */
    {
        curr_sum += magic_matrix[d][d];

        if(curr_sum < 0) /* if sum passed the max of signed int */
            return FALSE;
    }

    if(*sum != curr_sum)
        return FALSE;

    curr_sum = 0;
    for(u = N - 1; u >= 0; --u) /* down left to up right*/
    {
        curr_sum += magic_matrix[u][N - 1 - u];

        if(curr_sum < 0) /* if sum passed the max of signed int */
            return FALSE;
    }
    
    return *sum == curr_sum;
}


