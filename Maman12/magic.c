#include <stdio.h> /* FILE, printf, getc */
#include <ctype.h> /* isspace, isdigit */
#include <stdlib.h> /* atoi */

#include "..\Maman11\utills\test_input.h" /* HandleInput */

#define N 5
#define FALSE 0
#define TRUE 1

#define INDEX(row, col) (row * N + col)

static void RunMagic(FILE* in); 
static int ParseInput(FILE* in, int* all_valid, int magic_matrix[]);
static void PrintMatrix(int magic_matrix[]);
static int CalculateIsMagic(int magic_matrix[]);
static int CheckCols(int magic_matrix[], int* sum);
static int CheckDiag(int magic_matrix[], int* sum);

static int CheckRows(int magic_matrix[], int* sum);

int main(int argc, char *argv[])
{
    printf("\n\n************************* Welcome to the MAGIC MATRIX! ***************************\n*\n");
    printf("* This program will receive %d numbers as input from the user,\n", N*N);
    printf("* will print the matrix input and after calculation will\n");
    printf("* determine if the matrix is a magic matrix or not.\n*\n");

    printf("* A magic matrix has the same sum in all rows, columns and the main two diagonals.\n*\n");

    printf("* If user input is larger or smaller than the matrix, then the program will end.\n");
    printf("* If user input is not an integer, (white spaces are allowed), then the program will end.\n*\n");

    printf("* User input can be used using the terminal command < and a file name to be used as stdin,\n*\
 by mentioning several files names when running the program.\n*\
 or by running the program and entering the input manually (finish with EOF)\n*\n");

    printf("********************************   Enjoy.  ****************************************\n\n");

    HandleInput(argc, argv, RunMagic);

    return 0;
}

void RunMagic(FILE* in)
{
    int magic_matrix[N*N] = {0};
    int input_range_valid = 1;
    
    if(ParseInput(in, &input_range_valid, magic_matrix) && input_range_valid)
    {
        PrintMatrix(magic_matrix);
        
        if(CalculateIsMagic(magic_matrix))
            printf("the matrix is magical!\n");
        else
            printf("the matrix is not magical..\n");
    }
    else
    {
        printf("input is invalid. I don't play like that.\nGoodbye.\n");
    }
}

int ParseInput(FILE* in, int* all_valid, int magic_matrix[])
{
    int curr = 0;
    int i = 0;
    
    while (fscanf(in, "%d", &curr) != EOF)
    {
        if(i < N*N)
        {
            /* 
                if input is int that is bigger than valid range,
                we do not check if magic to save time. 
            */
            if(curr < 1 || curr > N*N) 
                all_valid = 0;

            magic_matrix[i] = curr;
        }    
        else
        {
            printf("input has too many variables for matrix\n");
            return i != N*N;
        }    

        ++i;
    }    
    
    return i == N*N;
}

void PrintMatrix(int magic_matrix[])
{
    int r = 0, c = 0;
    
    for(r = 0; r < N; ++r) /* rows */
    {
        for(c = 0; c < N - 1; ++c) /* columns */
        {
            printf("%3d\t", magic_matrix[INDEX(r,c)]);
        }

        printf("%3d\n", magic_matrix[INDEX(r,c)]);
    }
}

int CalculateIsMagic(int magic_matrix[])
{
    int sum = 0;
    if(CheckRows(magic_matrix, &sum))
        if(CheckCols(magic_matrix, &sum))
            return CheckDiag(magic_matrix, &sum);
    return 0;
}

int CheckRows(int magic_matrix[], int* sum)
{
    int r = 0, c = 0;
    int curr_sum = 0;
    for(r = 0; r < N; ++r) /* rows */
    {
        curr_sum = 0;

        for(c = 0; c < N; ++c) /* columns */
        {
            curr_sum += magic_matrix[INDEX(r,c)];
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


int CheckCols(int magic_matrix[], int* sum)
{
    int r = 0, c = 0;
    int curr_sum = 0;
    
    for(c = 0; c < N; ++c) /* columns */
    {
        curr_sum = 0;

        for(r = 0; r < N; ++r) /* rows */
        {
            curr_sum += magic_matrix[INDEX(r,c)];

            if(curr_sum < 0) /* if sum passed the max of signed int */
                return FALSE;    
        }

        if(*sum != curr_sum)
            return FALSE;
    }

    return TRUE;
}

int CheckDiag(int magic_matrix[], int* sum)
{
    int d = 0, u = 0;
    int curr_sum = 0;
    
    for(d = 0; d < N; ++d) /* up left to down right diag */
    {
        curr_sum += magic_matrix[INDEX(d,d)];

        if(curr_sum < 0) /* if sum passed the max of signed int */
            return FALSE;
    }

    if(*sum != curr_sum)
        return FALSE;

    curr_sum = 0;
    for(u = N - 1; u >= 0; --u) /* down left to up right*/
    {
        curr_sum += magic_matrix[INDEX(u, N - 1 - u)];

        if(curr_sum < 0) /* if sum passed the max of signed int */
            return FALSE;
    }
    
    return *sum == curr_sum;
}