#include <stdio.h>

#define MAX_STRING_LENGTH 80
#define MIN_ROW 1

void printInstructions();
void RunSingleConTest(FILE* input);
void contract(char s1[], char s2[]);
int IsNeighbor(char a, char b);
int IsNumerical(char note);

int main (int argc, char *argv[])
{
    FILE *file = stdin;

    printInstructions();

    if(argc > 1)
    {
        int i = 1;
        int testNumber = 1;    

        for(i = 1; i < argc; ++i, ++testNumber)
        {
            file = fopen(argv[i], "r");
            if(file)
            {
                printf("test number %d:\n", testNumber);
                RunSingleConTest(file);
                fclose(file);
            }
            else
            {
                printf("could not open file %s\n", argv[i]);
            }
        }
    }
    else
    {
        RunSingleConTest(file);
    }

    return 0;
}

void printInstructions()
{
    printf("Hello!\nTo run the automatic tests run again with the names of files you wish to run.\n");
    printf("Number of tests is according to number of text files in folder.\n");
    printf("If you wish to enter a manual test start the program without parameters\n");
    printf("and enter your string. The SW will check maximum %d chars from the string.\n", MAX_STRING_LENGTH);
    printf("The %d char and on will be ignored.\n\n", MAX_STRING_LENGTH + 1);
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