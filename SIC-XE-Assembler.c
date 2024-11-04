#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>

const char* DIRECTIVES[] = { "START", "BYTE", "WORD", "RESB", "RESW", "END", "BASE", "NOBASE" };
#define DIRECTIVES_SIZE (sizeof(DIRECTIVES) / sizeof(DIRECTIVES[0]))

int isValidDirective(const char* OPCODE)
{
    for (int i = 0; i < DIRECTIVES_SIZE; i++)
    {
        if (strcmp(OPCODE, DIRECTIVES[i]) == 0)
        {
            return 1; // Directive is valid
        }
    }
    return 0;
}

typedef struct OperationCodeTable
{
    char Mnemonic[5];
    char Format;
    unsigned short int MachineCode;
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

#define OPTAB_SIZE (sizeof(OPTAB) / sizeof(SIC_OPTAB))

int isValidOpcode(char* OPCODE)
{
    for (int i = 0; i < OPTAB_SIZE; i++)
    {
        if (strcmp(OPTAB[i].Mnemonic, OPCODE) == 0)
        {
            return 1; // Opcode is valid
        }
    }
    return 0;
}

char* getObjectCode(const char* OPCODE)
{
    for (int i = 0; i < OPTAB_SIZE; i++)
    {
        if (strcmp(OPTAB[i].Mnemonic, OPCODE) == 0)
        {
            char* hexString = (char*)malloc(5);
            sprintf_s(hexString, 5, "%02X", OPTAB[i].MachineCode);
            return hexString;
        }
    }
    return NULL; // Return NULL if opcode not found
}

typedef struct Symbol
{
    char name[7]; // 6 + 1 for the null terminator
    unsigned short int address;
} Symbol;

Symbol symbolTable[100];
int lineNumber = 0;
int symbolCount = 0;

int isDuplicateSymbol(const char* label)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].name, label) == 0)
        {
            return 1; // Duplicate found
        }
    }
    return 0;
}

void addSymbol(const char* LABEL, unsigned short int address)
{
    if (isDuplicateSymbol(LABEL))
    {
        printf("Error: Pass 1, Line %d: Duplicate symbol '%s'\n", lineNumber, LABEL);
        exit(EXIT_FAILURE);
    }
    strcpy_s(symbolTable[symbolCount].name, sizeof(symbolTable[symbolCount].name), LABEL);
    symbolTable[symbolCount].address = address;
    symbolCount++;
}

//int main(int argc, char* argv[])
int main()
{
    /*if (argc != 2)
    {
        printf("Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    char* file_path = argv[1];
    FILE* file = fopen(file_path, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }*/

    FILE* file = fopen("C:\\Users\\Hannah\\Dropbox\\2024 Fall\\CIS335 Language Processors\\Assignments\\Assignment 4\\SIC_PROG.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    printf("Author Info: Hannah Simon & Charlie Strickland\n\n");

    char line[256];
    char* LABEL = NULL;
    char* OPCODE = NULL;
    char* OPERAND = NULL;
    char* context = NULL;
    int LOCCTR = NULL;
    FILE* outputFile1 = fopen("ListingFile.txt", "w");
    FILE* outputFile2 = fopen("ObjectCode.txt", "w");

    fprintf(outputFile1, "LINE\tLOCCTR\tLABEL\tOPCODE\tOPERAND\t\tOBJ_CODE\n");

    while (fgets(line, sizeof(line), file))
    {
        char lineCopy[256];
        strcpy_s(lineCopy, sizeof(lineCopy), line);
        lineNumber++;

        if (line[0] == '.')
        {
            fprintf(outputFile1, "%d\t%s", lineNumber, lineCopy);
            continue;
        }
        else if (line[0] != ' ')
        {
            LABEL = strtok_s(line, " \n", &context);
            addSymbol(LABEL, LOCCTR);
            OPCODE = strtok_s(NULL, " \n", &context);
            OPERAND = strtok_s(NULL, " \n", &context);
        }
        else
        {
            LABEL = NULL;
            OPCODE = strtok_s(line, " \n", &context);
            OPERAND = strtok_s(NULL, " \n", &context);
        }

        bool isDirective = false;
        bool isOpcode = false;
        if (isValidDirective(OPCODE))
        {
            isDirective = true;
        }
        else if (isValidOpcode(OPCODE))
        {
            isOpcode = true;
        }
        if (!isDirective && !isOpcode)
        {
            printf("Error: Pass 1, Line %d: Invalid operation '%s'\n", lineNumber, OPCODE);
            exit(EXIT_FAILURE);
        }

        /*printf("Token 1: %s\n", LABEL);
        printf("Token 2: %s\n", OPCODE);
        printf("Token 3: %s\n\n", OPERAND);*/

        if (strcmp(OPCODE, "START") == 0)
        {
            LOCCTR = atoi(OPERAND);
            fprintf(outputFile1, "%d\t%d\t%s\t%s\t%s\n", lineNumber, LOCCTR, LABEL, OPCODE, OPERAND);
        }
        else
        {
            LOCCTR += 3;
            
            char OBJCODE[5];
            if (isOpcode)
            {
                char* hexString = getObjectCode(OPCODE);
                strncpy_s(OBJCODE, sizeof(OBJCODE), hexString, 4);
                free(hexString);
            }
            else
            {
                OBJCODE[0] = '\0';
            }

            if (OPERAND != NULL && strlen(OPERAND) >= 8) {
                fprintf(outputFile1, "%d\t%X\t%s\t%s\t%s\t%s\n",
                    lineNumber,
                    LOCCTR,
                    (LABEL != NULL) ? LABEL : "",
                    (OPCODE != NULL) ? OPCODE : "",
                    (OPERAND != NULL) ? OPERAND : "",
                    OBJCODE);
            }
            else
            {
                fprintf(outputFile1, "%d\t%X\t%s\t%s\t%s\t\t%s\n",
                    lineNumber,
                    LOCCTR,
                    (LABEL != NULL) ? LABEL : "",
                    (OPCODE != NULL) ? OPCODE : "",
                    (OPERAND != NULL) ? OPERAND : "",
                    OBJCODE);
            }
            if (strcmp(OPCODE, "END") != 0)
            {

            }
            else if (strcmp(OPCODE, "END") == 0)
            {
                break;
            }
        }
    }

    fprintf(outputFile1, "\nSYMBOL\tADDRESS\n");
    for (int i = 0; i < symbolCount; i++)
    {
        fprintf(outputFile1, "%s\t%X\n", symbolTable[i].name, symbolTable[i].address);
    }
    fclose(outputFile1);

    //loop, read from outputfile 1, after closing for writing, reopen it for reading
    //search table for menumonic, find corresponding opcode

    
    fclose(outputFile2);
    fclose(file);
    return 0;
}
