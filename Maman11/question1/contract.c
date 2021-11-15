#include <stdio.h>
#include "../utills/test_input.h"

#define MAX_STRING_LENGTH 80
#define MIN_ROW 1

static void RunSingleConTest(FILE* input);
static void Contract(char s1[], char s2[]);
static int IsNeighbor(char a, char b);
static int IsNumerical(char note);
static int IsEnd(char note);

int main (int argc, char *argv[])
{
    if(argc == 1)
        printInputInstructions(MAX_STRING_LENGTH);
        
    handleInput(argc, argv, &RunSingleConTest);

    return 0;
}

void RunSingleConTest(FILE* input)
{
    char input_str[MAX_STRING_LENGTH];
    char output_str[MAX_STRING_LENGTH];

    fgets(input_str, MAX_STRING_LENGTH, input);

    contract(input_str, output_str);

    printf("before: ");
    printf("%s\n\n", input_str);
    printf("after: ");
    printf("%s\n\n", output_str);
}

void Contract(char s1[], char s2[])
{
    int s1_index_run_start = 0, s1_index_runner = 0, 
    s2_index = 0, neighbors_cntr = 0;
    char prev_char = '\0';

    while(!IsEnd(s1[s1_index_run_start]))
    {
        neighbors_cntr = 0;
        prev_char = s1[s1_index_run_start];
        ++s1_index_runner;

        while(!IsEnd(s1[s1_index_runner]) && 
            IsNeighbor(s1[s1_index_runner], prev_char))
        {
            ++neighbors_cntr;
            prev_char = s1[s1_index_runner++];
        }

        s2[s2_index++] = s1[s1_index_run_start];

        if(0 != neighbors_cntr)
        {
            if(MIN_ROW < neighbors_cntr)
            {
                s2[s2_index++] = '-';
            }
            
            s2[s2_index++] = prev_char;
        }

        s1_index_run_start = s1_index_runner;
    }

    s2[s2_index] = '\0';
}

int IsNeighbor(char a, char b)
{
    return IsNumerical(a) && IsNumerical(b) && 1 == a - b;
}

int IsNumerical(char note)
{
    return (note >= '0' && note <= '9') || 
        (note >= 'a' && note <= 'z') ||
        (note >= 'A' && note <= 'Z');
}

int IsEnd(char note)
{
    return '\0' == note || EOF == note;
}
