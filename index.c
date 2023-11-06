#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    // open the file
    FILE *in_file = fopen('text-fr.txt', "r");
    // read the file size
    struct stat sb;
    stat('text-fr.txt', &sb);
    // allocation of the character array
    char *file_contents = malloc(sb.st_size);
    // display line by line
    while (fscanf(in_file, "%[^\\n ]\\n", file_contents) != EOF)
    {
        printf(">%s\\n", file_contents);
    }
    // close the file
    fclose(in_file);
    exit(EXIT_SUCCESS);
}

