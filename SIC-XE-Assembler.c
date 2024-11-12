#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char* DIRECTIVES[] = {"BYTE", "WORD", "RESB", "RESW", "END", "BASE", "NOBASE"};
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
    char Mnemonic[6];
    char Format;
    unsigned short int MachineCode;
    unsigned short int NumberOperands;
}SIC_OPTAB;

static SIC_OPTAB OPTAB[] =
{
    {   "ADD",  '3',  0x18, 1},
    {  "ADDR",  '2',  0x90, 2},
    { "CLEAR",  '2',  0xB4, 1},
    {  "COMP",  '3',  0x28, 1},
    { "COMPR",  '2',  0xA0, 2},
    {   "DIV",  '3',  0x24, 1},
    {     "J",  '3',  0x3C, 1},
    {   "JEQ",  '3',  0x30, 1},
    {   "JGT",  '3',  0x34, 1},
    {   "JLT",  '3',  0x38, 1},
    {  "JSUB",  '3',  0x48, 1},
    {   "LDA",  '3',  0x00, 1},
    {   "LDB",  '3',  0x68, 1},
    {  "LDCH",  '3',  0x50, 1},
    {   "LDL",  '3',  0x08, 1},
    {   "LDT",  '3',  0x74, 1},
    {   "LDX",  '3',  0x04, 1},
    {   "MUL",  '3',  0x20, 1},
    {    "RD",  '3',  0xD8, 1},
    {  "RSUB",  '3',  0x4C, 0},
    {   "STA",  '3',  0x0C, 1},
    {   "STB",  '3',  0x78, 1},
    {  "STCH",  '3',  0x54, 1},
    {   "STL",  '3',  0x14, 1},
    {  "STSW",  '3',  0xE8, 1},
    {   "STX",  '3',  0x10, 1},
    {   "SUB",  '3',  0x1C, 1},
    {   "SUBR", '2',  0x94, 2},
    {    "TD",  '3',  0xE0, 1},
    {   "TIX",  '3',  0x2C, 1},
    {   "TIXR", '2',  0xB8, 1},
    {    "WD",  '3',  0xDC, 1},
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

char getFormat(const char* OPCODE)
{
    if (OPCODE[0] == '+')
    {
        OPCODE = OPCODE + 1;
    }
    for (int i = 0; i < OPTAB_SIZE; i++)
    {
        if (strcmp(OPTAB[i].Mnemonic, OPCODE) == 0)
        {
            return OPTAB[i].Format;
        }
    }
    return 0; // Return 0 if opcode not found
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

int getSymbolAddress(char* name)
{
    if (strlen(name) >= 2 && name[strlen(name) - 2] == ',' && name[strlen(name) - 1] == 'X')
    {
        name[strlen(name) - 2] = '\0';
    }
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].name, name) == 0)
        {
            return symbolTable[i].address; // Return the address if found
        }
    }
    return NULL;
}

const char* RegisterCodes[] = {
    "0000", // A
    "0001", // X
    "0010", // L
    "0011", // B
    "0100", // S
    "0101", // T
    "0110", // F
    "1000", // PC
    "1001"  // SW
};

const char* getRegisterCode(char letter) {
    switch (letter) {
    case 'A': return RegisterCodes[0];
    case 'X': return RegisterCodes[1];
    case 'L': return RegisterCodes[2];
    case 'B': return RegisterCodes[3];
    case 'S': return RegisterCodes[4];
    case 'T': return RegisterCodes[5];
    case 'F': return RegisterCodes[6];
    case 'P': return RegisterCodes[7];
    case 'W': return RegisterCodes[8];
    default: return NULL;
    }
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

writeToObjectFile(FILE* ObjectFile, char* buffer)
{
    int result = (strlen(buffer) - 9 + 1) / 2;  // +1 for rounding up
    char resultChars[3];
    sprintf_s(resultChars, sizeof(resultChars), "%02d", result); //use X here instead for hex
    buffer[7] = resultChars[0];
    buffer[8] = resultChars[1];
    fprintf(ObjectFile, "%s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
}

char* intToBinary(int n)
{
    char* binaryString = malloc(8);
    binaryString[8] = '\0';

    for (int i = 7; i >= 0; i--)
    {
        binaryString[i] = (n & 1) ? '1' : '0';
        n >>= 1;
    }
    return binaryString;
}

int binaryToInt(const char* binaryString)
{
    int result = 0;
    int length = strlen(binaryString);

    for (int i = 0; i < length; i++)
    {
        result <<= 1; // Shift left by 1 (equivalent to multiplying by 2)
        if (binaryString[i] == '1')
        {
            result |= 1; // Set the least significant bit if the character is '1'
        }
    }
    return result;
}

char* binaryToHex(char* binary)
{
    int len = strlen(binary);
    int hexLen = (len + 3) / 4;
    static char hexString[32];
    hexString[hexLen] = '\0';

    int j = 0;
    for (int i = len - 4; i >= 0; i -= 4)
    {
        int value = 0;
        // Get the 4-bit binary chunk and convert it to a decimal value
        for (int k = 0; k < 4; k++)
        {
            value += (binary[i + k] - '0') * pow(2, 3 - k);
        }
        // Store the hex character
        if (value < 10)
        {
            hexString[j++] = value + '0';
        }
        else
        {
            hexString[j++] = value - 10 + 'A';
        }
    }

    // Handle the remaining bits (if any)
    int remainingBits = len % 4;
    if (remainingBits > 0)
    {
        int value = 0;
        for (int i = 0; i < remainingBits; i++)
        {
            value += (binary[i] - '0') * pow(2, remainingBits - i - 1);
        }
        hexString[j++] = (value < 10) ? value + '0' : value - 10 + 'A';
    }

    return hexString;
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

    FILE* file = fopen("C:\\Users\\Hannah\\Dropbox\\2024 Fall\\CIS335 Language Processors\\Assignments\\Assignment 4\\SIC_XE_PROG.txt", "r");
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
            if (OPCODE[0] == '+')
            {
                memmove(OPCODE, OPCODE + 1, strlen(OPCODE));    // Shift the string to the left by one to remove the '+'
            }
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
                        printf("Error: Pass 1, Line %d: Invalid BYTE format for operand %s\n", lineNumber, OPERAND);
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
                char format = getFormat(OPCODE);
                if (format == '1')
                {
                    LOCCTR += 1;
                }
                else if (format == '2')
                {
                    LOCCTR += 2;
                }
                else if (format == '3' && OPCODE[0] != '+')
                {
                    LOCCTR += 3;
                }
                else if (format == '3' && OPCODE[0] == '+')
                {
                    LOCCTR += 4;
                }
                else
                {
                    printf("Error: Pass 1, Line %d: Invalid format for opcode '%s'\n", lineNumber, OPCODE);
                }
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
    firstLine = true; char* LINE2 = NULL, * ADDRESS2 = NULL; bool baseSet = false; int baseAddress = NULL;
    char nextLine[256] = { 0 };
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
        char objectCode[33];
        char format = getFormat(OPCODE);

        if (strcmp(OPERAND, "BUFFER,X") == 0)
        {
            int y = 3;
        }
        
        if (strcmp(OPCODE, "BASE") == 0)
        {
            baseSet = true;
            long int currentPos = ftell(IntermediateFile);  // Save the current file position
            if (fgets(nextLine, sizeof(nextLine), IntermediateFile))
            {
                LINE2 = strtok_s(nextLine, "\t\n", &context);
                ADDRESS2 = strtok_s(NULL, " \t\n", &context);
            }
            baseAddress = atoi(ADDRESS2);
            fseek(IntermediateFile, currentPos, SEEK_SET);
            continue;
        }
        else if (strcmp(OPCODE, "NOBASE") == 0)
        {
            baseSet = false;
            continue;
        }

        else if (strcmp(OPCODE, "RESW") == 0 || strcmp(OPCODE, "RESB") == 0)
        {
            fprintf(ListingFile, "%s\n", lineCopy);
            if (strlen(buffer) > 0)
            {
                writeToObjectFile(ObjectFile, buffer);
            }
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
                printf("Error: Pass 1, Line %d: Invalid BYTE format for operand %s\n", lineNumber, OPERAND);
                exit(EXIT_FAILURE);
            }
        }
  
        else if (format == '1')
        {
            OPCODEINT = getMachineCode(OPCODE);
            snprintf(objectCode, sizeof(objectCode), "%d", OPCODEINT);
        }
        else if (format == '2')
        {
            OPCODEINT = getMachineCode(OPCODE);
            if (strlen(OPERAND) == 1)
            {
                int code = binaryToInt(getRegisterCode(OPERAND[0]));
                snprintf(objectCode, sizeof(objectCode), "%08d%04d", OPCODEINT, code); // 8 bits for OPCODEINT and 4 for RegisterCode
            }
            else if (strlen(OPERAND) == 3)
                {
                int code1 = binaryToInt(getRegisterCode(OPERAND[0]));
                int code2 = binaryToInt(getRegisterCode(OPERAND[2]));
                snprintf(objectCode, sizeof(objectCode), "%08d%04d%04d", OPCODEINT, code1, code2);
            }
        }
        else if (format == '3')
        {
            char binaryString[13];
            printf("%d\n", (getMachineCode(OPCODE)));
            char* OPCODECHAR = intToBinary(getMachineCode(OPCODE));
            printf("%s\n", OPCODECHAR);
            OPCODECHAR[strlen(OPCODECHAR) - 2] = '\0';
            printf("%s\n", OPCODECHAR);
            break;
           
            if (OPERAND != NULL && OPERAND[0] == '#')
            {
                int number = atoi(OPERAND + 1);
                
                if (0 <= number <= 4095)
                {
                    snprintf(binaryString, sizeof(binaryString), "%s010000", OPCODECHAR);
                    char* hexString = binaryToHex(binaryString);
                    snprintf(objectCode, sizeof(objectCode), "%s%d", hexString, number);
                }
                else if (4096 <= number <= 1048575 && OPCODE[0] == '+')
                {
                    snprintf(binaryString, sizeof(binaryString), "%s010001", OPCODECHAR);
                    char* hexString = binaryToHex(binaryString);
                    snprintf(objectCode, sizeof(objectCode), "%s%d", hexString, number);

                }
                else
                {
                    printf("Error: Pass 2, Line %s: Immediate number out of range %s\n", LINE, OPERAND);
                    //add exit statements for errors
                }
            }
            else if (OPERAND != NULL)
            {
                if (OPERAND[0] == '@')
                {
                    ADDR = getSymbolAddress(&OPERAND[1]);
                }
                else
                {
                    ADDR = getSymbolAddress(OPERAND);
                }
                if (ADDR == NULL)
                {
                    printf("Error: Pass 2, Line %s: Symbol not found %s\n", LINE, OPERAND);
                }
                long int currentPos = ftell(IntermediateFile);  // Save the current file position
                if (fgets(nextLine, sizeof(nextLine), IntermediateFile))
                {
                    LINE2 = strtok_s(nextLine, "\t\n", &context);
                    ADDRESS2 = strtok_s(NULL, " \t\n", &context);
                }
                fseek(IntermediateFile, currentPos, SEEK_SET);

                if (OPCODE[0] == '+')
                {
                    snprintf(binaryString, sizeof(binaryString), "%s110001", OPCODECHAR);
                    char* hexString = binaryToHex(binaryString);
                    snprintf(objectCode, sizeof(objectCode), "%s%d", hexString, ADDR);
                }

                else if (-2048 <= ADDR - atoi(ADDRESS2) <= 2047)
                {
                    snprintf(binaryString, sizeof(binaryString), "%s110010", OPCODECHAR);
                    //printf("%s\n", binaryString);
                    //break;
                    char* hexString = binaryToHex(binaryString);
                    int displacement = ADDR - atoi(ADDRESS2);
                    snprintf(objectCode, sizeof(objectCode), "%s%d", hexString, displacement);
                }
                else if (baseSet && 0 <= ADDR - baseAddress <= 4095)
                {
                    snprintf(binaryString, sizeof(binaryString), "%s110100", OPCODECHAR);
                    char* hexString = binaryToHex(binaryString);
                    int displacement = ADDR - baseAddress;
                    snprintf(objectCode, sizeof(objectCode), "%s%d", hexString, displacement);
                }
                else
                {
                    printf("Error: Pass 2, Line %s: Instruction addressing error\n", LINE);
                }

                if (OPERAND[0] == '#')
                {
                    objectCode[7] = 0;
                }
                else if (OPERAND[0] == '@')
                {
                    objectCode[8] = 0;
                }
                else if (strlen(OPERAND) >= 2 && OPERAND[strlen(OPERAND) - 2] == ',' && OPERAND[strlen(OPERAND) - 1] == 'X')
                {
                    objectCode[9] = 1;
                }
            }
            else
            {
                snprintf(objectCode, sizeof(objectCode), "%d%s", OPCODEINT, binaryToHex("110000000000000000"));
            }
            printf("%s\t%s\n", lineCopy, objectCode);
        }
        else
        {
            printf("error %s", LINE);
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
