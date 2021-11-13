#include <stdio.h>
#include "../utills/test_input.h"

#define MAX_STRING_LENGTH 80
#define MIN_ROW 1

void RunSingleConTest(FILE* input);
void contract(char s1[], char s2[]);
int IsNeighbor(char a, char b);
int IsNumerical(char note);

int main (int argc, char *argv[])
{
    if(argc == 1)
        printInputInstructions(MAX_STRING_LENGTH);
        
    handleInput(argc, argv, &RunSingleConTest);

    return 0;
}

void RunSingleConTest(FILE* input)
{
    char inputString[MAX_STRING_LENGTH];
    char outputString[MAX_STRING_LENGTH];

    fgets(inputString, MAX_STRING_LENGTH, input);

    contract(inputString, outputString);

    printf("before: ");
    printf("%s\n\n", inputString);
    printf("after: ");
    printf("%s\n\n", outputString);
}

void contract(char s1[], char s2[])
{
    int s1IndexStartRun = 0, s1IndexRunner = 0, 
    s2Index = 0, neighborsCounter = 0;
    char prevChar = '\0';

    while('\0' != s1[s1IndexStartRun] && EOF != s1[s1IndexStartRun])
    {
        neighborsCounter = 0;
        prevChar = s1[s1IndexStartRun];
        ++s1IndexRunner;

        while('\0' != s1[s1IndexRunner] && EOF != s1[s1IndexRunner] && 
            IsNeighbor(s1[s1IndexRunner], prevChar))
        {
            ++neighborsCounter;
            prevChar = s1[s1IndexRunner++];
        }

        s2[s2Index++] = s1[s1IndexStartRun];

        if(0 != neighborsCounter)
        {
            if(MIN_ROW < neighborsCounter)
            {
                s2[s2Index++] = '-';
            }
            
            s2[s2Index++] = prevChar;
        }

        s1IndexStartRun = s1IndexRunner;
    }

    s2[s2Index] = '\0';
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
