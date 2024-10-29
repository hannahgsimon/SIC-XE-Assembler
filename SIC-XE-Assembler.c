#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

int main()
{
    printf("Author Info: Hannah Simon & Charlie Strickland\n\n");

    typedef struct OperationCodeTable
    {
        char Mnemonic[8];
        char Format;
        unsigned short int ManchineCode;
    }SIC_OPTAB;

    static SIC_OPTAB OPTAB[] =
    {
        {   "ADD",  '3',  0x18},
        {  "ADDR",  '3',  0x90},
        { "CLEAR",  '3',  0xB4},
        {  "COMP",  '3',  0x28},
        { "COMPR",  '3',  0xA0},
        {   "DIV",  '3',  0x24},
        {     "J",  '3',  0x3C},
        {   "JEQ",  '3',  0x30},
        {   "JGT",  '3',  0x34},
        {   "JLT",  '3',  0x38},
        {  "JSUB",  '3',  0x48},
        {   "LDA",  '3',  0x00},
        {   "LDB",  '3',  0x68},
        {  "LDCH",  '3',  0x50},
        {   "LDL",  '3',  0x08},
        {   "LDT",  '3',  0x74},
        {   "LDX",  '3',  0x04},
        {   "MUL",  '3',  0x20},
        {    "RD",  '3',  0xD8},
        {  "RSUB",  '3',  0x4C},
        {   "STA",  '3',  0x0C},
        {   "STB",  '3',  0x78},
        {  "STCH",  '3',  0x54},
        {   "STL",  '3',  0x14},
        {  "STSW",  '3',  0xE8},
        {   "STX",  '3',  0x10},
        {   "SUB",  '3',  0x1C},
        {   "SUBR",  '3', 0x94},
        {    "TD",  '3',  0xE0},
        {   "TIX",  '3',  0x2C},
        {    "WD",  '3',  0xDC},
    };

    FILE* file = fopen("C:\\Users\\Hannah\\Dropbox\\2024 Fall\\CIS335 Language Processors\\Assignments\\Assignment 4\\SIC_PROG.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    char line[256];
    char* LABEL = NULL;
    char* OPCPDE = NULL;
    char* OPERAND = NULL;
    char* context = NULL;
    int LOCCTR;
    FILE* outputFile1 = fopen("ListingFile.txt", "w");
    FILE* outputFile2 = fopen("ObjectCode.txt", "w");

    while (fgets(line, sizeof(line), file))
    {
        char lineCopy[256];
        strcpy_s(lineCopy, sizeof(lineCopy), line);

        if (line[0] == '.')
        {
            fprintf(outputFile1, "%s", lineCopy);
            continue;
        }
        else if (line[0] != ' ')
        {
            LABEL = strtok_s(line, " ", &context);
            OPCPDE = strtok_s(NULL, " ", &context);
            OPERAND = strtok_s(NULL, " ", &context);
        }
        else
        {
            LABEL = NULL;
            OPCPDE = strtok_s(line, " ", &context);
            OPERAND = strtok_s(NULL, " ", &context);
        }
        /*printf("Token 1: %s\n", LABEL);
        printf("Token 2: %s\n", OPCPDE);
        printf("Token 3: %s\n", OPERAND);*/

        if (strcmp(OPCPDE, "START") == 0)
        {
            LOCCTR = atoi(OPERAND);
            //printf("Operand: %d\n", LOCCTR);
            fprintf(outputFile1, "%s", lineCopy);
        }
        else
        {
            LOCCTR = 0;
        }
        if (strcmp(OPCPDE, "END") != 0)
        {

        }
    }
    fclose(outputFile1);
    fclose(outputFile2);
    fclose(file);
    return 0;
}
