#include <stdio.h>
#include "../file_handler.h"

int main(int argc, char *argv[])
{
    char *name = NULL;
    FILE *fp = NULL;
    int i = 0;

    if(argc > 1)
    {
        for(i = 1; i < argc; ++i)
        {
            name = GetFileName(argv[i], STAGE_FIRST);
            printf("%s\n", name);
            fp = OpenFile(name, "r");
            if(fp)
            {
                fclose(fp);
            }
        }
    }

    return 0;
}