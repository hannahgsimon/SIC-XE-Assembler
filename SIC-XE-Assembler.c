#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    printf("Author Info: Hannah Simon & Charlie Strickland\n\n");

    FILE* file = fopen("C:\\Users\\Hannah\\Dropbox\\2024 Fall\\CIS335 Language Processors\\Assignments\\Assignment 4\\SIC_PROG.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    char line[256];
    char* token1 = NULL;
    char prev_token1[256] = "";
    char* token2 = NULL;
    char* token3 = NULL;
    char* context = NULL;
    char OPCODE[6];
    int OPERAND;
    int LOCCTR;
    FILE* outputFile1 = fopen("ListingFile.txt", "w");
    FILE* outputFile2 = fopen("ObjectCode.txt", "w");

    while (fgets(line, sizeof(line), file))
    {
        char lineCopy[256];
        strcpy_s(lineCopy, sizeof(lineCopy), line);

        if (line[0] == '.')
        {
            continue;
        }
        else if (line[0] != ' ')
        {
            token1 = strtok_s(line, " ", &context);
            token2 = strtok_s(NULL, " ", &context);
            token3 = strtok_s(NULL, " ", &context);
            strcpy_s(prev_token1, sizeof(prev_token1), token1);
        }
        else
        {
            token1 = prev_token1;
            token2 = strtok_s(line, " ", &context);
            token3 = strtok_s(NULL, " ", &context);
        }
        /*printf("Token 1: %s\n", token1);
        printf("Token 2: %s\n", token2);
        printf("Token 3: %s\n", token3);*/

        if (strcmp(token2, "START") == 0)
        {
            OPERAND = atoi(token3);
            LOCCTR = OPERAND;
            //printf("Operand: %d\n", OPERAND);
            fprintf(outputFile1, "%s", lineCopy);
        }
        else
        {
            LOCCTR = 0;
        }
        if (strcmp(token2, "END") != 0)
        {

        }
    }
    fclose(outputFile1);
    fclose(outputFile2);
    fclose(file);
    return 0;
}
