#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>

const char* DIRECTIVES[] = {"BYTE", "WORD", "RESB", "RESW", "END", "BASE", "NOBASE" };
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
    //{  "ADDR",  '2',  0x90},
    //{ "CLEAR",  '2',  0xB4},
    {  "COMP",  '3',  0x28},
    //{ "COMPR",  '2',  0xA0},
    {   "DIV",  '3',  0x24},
    {     "J",  '3',  0x3C},
    {   "JEQ",  '3',  0x30},
    {   "JGT",  '3',  0x34},
    {   "JLT",  '3',  0x38},
    {  "JSUB",  '3',  0x48},
    {   "LDA",  '3',  0x00},
    //{   "LDB",  '3',  0x68},
    {  "LDCH",  '3',  0x50},
    {   "LDL",  '3',  0x08},
    //{   "LDT",  '3',  0x74},
    {   "LDX",  '3',  0x04},
    {   "MUL",  '3',  0x20},
    {    "RD",  '3',  0xD8},
    {  "RSUB",  '3',  0x4C},
    {   "STA",  '3',  0x0C},
    //{   "STB",  '3',  0x78},
    {  "STCH",  '3',  0x54},
    {   "STL",  '3',  0x14},
    {  "STSW",  '3',  0xE8},
    {   "STX",  '3',  0x10},
    {   "SUB",  '3',  0x1C},
    //{   "SUBR",  '2', 0x94},
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

unsigned short int getMachineCode(const char* OPCODE)
{
    for (int i = 0; i < OPTAB_SIZE; i++)
    {
        if (strcmp(OPTAB[i].Mnemonic, OPCODE) == 0)
        {
            return OPTAB[i].MachineCode;
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
int symbolCount = 0;
int lineNumber = 0;

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

int getSymbolAddress(const char* name)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].name, name) == 0)
        {
            return symbolTable[i].address; // Return the address if found
        }
    }
    return 0;
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

    char line[256], lineCopy[256];
    char* LABEL = NULL, * OPCODE = NULL, * OPERAND = NULL, * context = NULL;
    int LOCCTR = 0;
    bool firstLine = true;
    FILE* IntermediateFile = fopen("IntermediateFile.txt", "w");
    FILE* ListingFile = fopen("ListingFile.txt", "w");
    FILE* ObjectFile = fopen("ObjectCode.txt", "w");
    
    fprintf(IntermediateFile, "LINE\tLOCCTR\tSOURCE_STATEMENT\n");

    //Pass 1
    while (fgets(line, sizeof(line), file))
    {
        lineNumber += 5;

        if (line[0] == '.')
        {
            strcpy_s(lineCopy, sizeof(lineCopy), line);
            fprintf(IntermediateFile, "%d\t%s", lineNumber, lineCopy);
            continue;
        }
        else if (line[0] != ' ')
        {
            LABEL = strtok_s(line, " \n", &context);
            OPCODE = strtok_s(NULL, " \n", &context);
            OPERAND = strtok_s(NULL, " \n", &context);
        }
        else
        {
            LABEL = NULL;
            OPCODE = strtok_s(line, " \n", &context);
            OPERAND = strtok_s(NULL, " \n", &context);
        }

        if (firstLine)
        {
            if (strcmp(OPCODE, "START") == 0)
            {
                LOCCTR = atoi(OPERAND);
            }
            fprintf(IntermediateFile, "%d\t%d\t%s\t%s\t%s\n", lineNumber, LOCCTR, LABEL, OPCODE, OPERAND);
            if (LABEL != NULL)
            {
                addSymbol(LABEL, LOCCTR);
            }
            firstLine = false;
            continue;
        }
        
        if (LABEL != NULL)
        {
            addSymbol(LABEL, LOCCTR);
        }

        if (strcmp(OPCODE, "START") != 0)
        {
            bool isDirective = false;
            bool isOpcode = false;
            if (isValidDirective(OPCODE))
            {
                isDirective = true;
                writeToIntermediateFile(IntermediateFile, LOCCTR, LABEL, OPCODE, OPERAND, isOpcode);
                if (strcmp(OPCODE, "END") == 0)
                {
                    break;
                }
                else if (strcmp(OPCODE, "RESW") == 0)
                {
                    LOCCTR += 3 * atoi(OPERAND);
                }
                else if (strcmp(OPCODE, "RESB") == 0)
                {
                    LOCCTR += atoi(OPERAND);
                }
                else if (strcmp(OPCODE, "BYTE") == 0)
                {
                    if (OPERAND[0] == 'C')
                    {
                        int length = strlen(OPERAND) - 3;
                        LOCCTR += length;
                    }
                    else if (OPERAND[0] == 'X')
                    {
                        int hexLength = (strlen(OPERAND) - 3 + 1) / 2; // The +1 ensures rounding for odd numbers, Divide by 2 since 2 hex digits represent 1 byte
                        LOCCTR += hexLength;
                    }
                    else
                    {
                        printf("Error: Invalid BYTE format for operand: %s\n", OPERAND);
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    LOCCTR += 3;
                }
                fprintf(IntermediateFile, "\n");
            }
            else if (isValidOpcode(OPCODE))
            {
                isOpcode = true;
                writeToIntermediateFile(IntermediateFile, LOCCTR, LABEL, OPCODE, OPERAND, isOpcode);
                fprintf(IntermediateFile, "\n");
                LOCCTR += 3;
            }
            else
            {
                printf("Error: Pass 1, Line %d: Invalid operation '%s'\n", lineNumber, OPCODE);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(IntermediateFile);
    fopen_s(IntermediateFile, "IntermediateFile.txt", "r");
    char* LINE = NULL, * ADDRESS = NULL;
    firstLine = true;
    char buffer[70] = { 0 };

    // PASS 2
    while (fgets(line, sizeof(line), IntermediateFile))
    {
        strcpy_s(lineCopy, sizeof(lineCopy), line);
        
        size_t len = strlen(lineCopy);
        if (len > 0 && lineCopy[len - 1] == '\n')
{
            lineCopy[len - 1] = '\0';
        }

        if (firstLine && line[0] != '.')
        {
            fprintf(ListingFile, "%s\tOBJ_CODE\n", lineCopy);
            firstLine = false;
            continue;
        }

        LINE = strtok_s(line, "\t\n", &context);
        ADDRESS = strtok_s(NULL, " \t\n", &context);
        LABEL = strtok_s(NULL, " \t\n", &context);
        OPCODE = strtok_s(NULL, " \t\n", &context);
        OPERAND = strtok_s(NULL, " \t\n", &context);

        if (LINE == NULL)
        {
            break;
        }
        if (OPCODE == NULL)
        {
            OPCODE = LABEL;
            LABEL = NULL;
        }
        else if (OPERAND == NULL)
        {
            OPERAND = OPCODE;
            OPCODE = LABEL;
            LABEL = NULL;
        }

        if (strcmp(ADDRESS, ".") == 0)
        {
            fprintf(ListingFile, "%s\n", lineCopy);
            continue;
        }
        if (strcmp(OPCODE, "START") == 0)
        {
            fprintf(ListingFile, "%s\n", lineCopy);
            fprintf(ObjectFile, "H%s\t%06s%06d\n", LABEL, ADDRESS, LOCCTR - atoi(ADDRESS));
            continue;
        }

        unsigned short int OPCODEINT = NULL;
        int ADDR = NULL;
        char objectCode[8];

        if (OPERAND != NULL && strcmp(OPERAND, "BUFFER,X") == 0)
        {
            OPCODEINT = getMachineCode(OPCODE);
            ADDR = getSymbolAddress("BUFFER");
            char addrStr[5];
            snprintf(addrStr, sizeof(addrStr), "%04d", ADDR);
            int firstDigit = addrStr[0] - '0';
            firstDigit &= 0x07;
            firstDigit |= 0x08;
            addrStr[0] = firstDigit + '0';
            ADDR = atoi(addrStr);

            sprintf_s(objectCode, sizeof(objectCode), "%02d%04d", OPCODEINT, ADDR);
        }
        else if (strcmp(OPCODE, "RESW") == 0 || strcmp(OPCODE, "RESB") == 0)
        {
            fprintf(ListingFile, "%s\n", lineCopy);
            //if buffer has contents, write it to objectfile and clear objectfile
            continue;
        }
        else if (strcmp(OPCODE, "WORD") == 0)
        {
            sprintf_s(objectCode, sizeof(objectCode), "%06d", atoi(OPERAND));
            //later do X to convert to hex
            continue;
        }
        else if (strcmp(OPCODE, "BYTE") == 0)
        {
            if (OPERAND[0] == 'C')
            {
                int i = 2; // Start after C'
                int hexIndex = 0;

                while (OPERAND[i] != '\'')
                {
                    hexIndex += snprintf(objectCode + hexIndex, sizeof(objectCode) - hexIndex, "%02X", OPERAND[i]);
                    i++;
                }
            }
            else if (OPERAND[0] == 'X')
            {
                char* startQuote = &OPERAND[2];
                char* endQuote = strchr(startQuote, '\'');
                size_t hexLength = endQuote - startQuote;
                strncpy_s(objectCode, hexLength + 1, startQuote, hexLength);
                objectCode[hexLength] = '\0';
            }
            else
            {
                printf("Error: Invalid BYTE format for operand: %s\n", OPERAND);
                exit(EXIT_FAILURE);
            }
        }
        else if (OPERAND != NULL)
        {
            OPCODEINT = getMachineCode(OPCODE);
            ADDR = getSymbolAddress(OPERAND);
            sprintf_s(objectCode, sizeof(objectCode), "%02d%04d", OPCODEINT, ADDR);
        }
        else if (OPERAND == NULL)
        {
            OPCODEINT = getMachineCode(OPCODE);
            ADDR = 0000;
            sprintf_s(objectCode, sizeof(objectCode), "%02d%04d", OPCODEINT, ADDR);
        }
        fprintf(ListingFile, "%s\t%s\n", lineCopy, objectCode);

        if (strcmp(OPCODE, "END") == 0)
        {
            if (strlen(buffer) > 0)
            {
                writeToObjectFile(ObjectFile, buffer);
            }
            fprintf(ObjectFile, "E%s", objectCode);
            break;
        }
        if (buffer[0] == '\0')
        {
            buffer[0] = 'T';
            char paddedAddress[7];
            snprintf(paddedAddress, sizeof(paddedAddress), "%06s", ADDRESS);
            strncat_s(buffer, sizeof(buffer), paddedAddress, sizeof(buffer) - strlen(buffer) - 1);
            strncat_s(buffer, sizeof(buffer), "1E", sizeof(buffer) - strlen(buffer) - 1);   // Placeholder
        }
        if (strlen(buffer) + strlen(objectCode) >= 69)
        {
            writeToObjectFile(ObjectFile, buffer);
        }
        else
        {
            strcat_s(buffer, sizeof(buffer), objectCode);
        }
    }

    fprintf(ListingFile, "\nSYMBOL\tADDRESS\n");
    for (int i = 0; i < symbolCount; i++)
    {
        fprintf(ListingFile, "%s\t%d", symbolTable[i].name, symbolTable[i].address);
        if (i < symbolCount - 1)
        {
            fprintf(ListingFile, "\n");
        }
    }

    fclose(IntermediateFile);
    fclose(ListingFile);
    fclose(ObjectFile);
    fclose(file);
    return 0;
}

writeToIntermediateFile(FILE* IntermediateFile, int LOCCTR, char* LABEL, char* OPCODE, char* OPERAND, bool isOpcode)
{
    fprintf(IntermediateFile, "%d\t%d\t%s\t%s\t%s",
        lineNumber,
        LOCCTR,
        (LABEL != NULL) ? LABEL : "",
        (OPCODE != NULL) ? OPCODE : "",
        (OPERAND != NULL) ? OPERAND : "");
}

writeToObjectFile(FILE* ObjectFile, char *buffer)
{
    int result = (strlen(buffer) - 9 + 1) / 2;  // +1 for rounding up
    char resultChars[3];
    sprintf_s(resultChars, sizeof(resultChars), "%02d", result); //use X here instead for hex
    buffer[7] = resultChars[0];
    buffer[8] = resultChars[1];
    fprintf(ObjectFile, "%s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
}
