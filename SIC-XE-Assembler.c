// Necessary imports
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>

// The directives we were told the input would be limited to, along with a definition for the size of one
// START is not included here as it is only supposed to appear once. If it appears again, we want it to throw an error
const char* DIRECTIVES[] = {"BYTE", "WORD", "RESB", "RESW", "END", "BASE", "NOBASE" };
#define DIRECTIVES_SIZE (sizeof(DIRECTIVES) / sizeof(DIRECTIVES[0]))

// Checks if the opcode it gets sent is a directive
int isValidDirective(const char* OPCODE)
{
    for (int i = 0; i < DIRECTIVES_SIZE; i++)
    {
        if (strcmp(OPCODE, DIRECTIVES[i]) == 0)
        {
            return 1; // Match found, valid directive
        }
    }
    return 0; // Match not found, invalid directive
}

// Object holding an opcode mnemonic, its format, and the machine code
typedef struct OperationCodeTable
{
    char Mnemonic[5];
    char Format;
    unsigned short int MachineCode;
}SIC_OPTAB;

// An array containing all SIC_OPTAB structs, along with a definition for the size of one
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

// Checks if the opcode it gets sent matches an opcode mnemonic in the SIC_OPTAB array
int isValidOpcode(char* OPCODE)
{
    for (int i = 0; i < OPTAB_SIZE; i++)
    {
        if (strcmp(OPTAB[i].Mnemonic, OPCODE) == 0)
        {
            return 1; // Match found, valid opcode mnemonic
        }
    }
    return 0; // Match not found, invalid opcode mnemonic
}

// Checks if an opcode is valid using isValidOpcode and, if it is, returns the machine code paired with it in the SIC_OPTAB array
unsigned short int getMachineCode(const char* OPCODE)
{
    for (int i = 0; i < OPTAB_SIZE; i++)
    {
        if (strcmp(OPTAB[i].Mnemonic, OPCODE) == 0)
        {
            return OPTAB[i].MachineCode;
        }
    }
    return NULL; // Returns NULL if opcode not found
}

// A symbol in SIC is like a function in C. This object stores the name and address of each symbol
typedef struct Symbol
{
    char name[7]; // 6 + 1 for the null terminator
    unsigned short int address;
} Symbol;

// Initializes an array of symbols, and a count of how many symbols are in the array
Symbol symbolTable[100];
int symbolCount = 0;
// Initializes the current line number being read from the file
int lineNumber = 0;

// Checks if a symbol already exists in the symbolTable array
int isDuplicateSymbol(const char* label)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].name, label) == 0)
        {
            return 1; // Duplicate found
        }
    }
    return 0; // New symbol
}

//  Checks if a symbol already exists, throwing an error if it does / adding it to the symbolTable array if it does not
void addSymbol(const char* LABEL, unsigned short int address)
{
    // Prints the line number and repeated symbol if it is a duplicate, then throws an error and exits
    if (isDuplicateSymbol(LABEL))
    {
        printf("Error: Pass 1, Line %d: Duplicate symbol '%s'\n", lineNumber, LABEL);
        exit(EXIT_FAILURE);
    }
    strcpy_s(symbolTable[symbolCount].name, sizeof(symbolTable[symbolCount].name), LABEL);
    symbolTable[symbolCount].address = address;
    symbolCount++;
}

// Checks symbolTable for a symbol with a name matching the one it is sent. If a match is found, returns the address associated with the name
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

// Output of pass 1, each line containing a line number, location counter, label, opcode, and operand (unless they are empty)
writeToIntermediateFile(FILE* IntermediateFile, int LOCCTR, char* LABEL, char* OPCODE, char* OPERAND, bool isOpcode)
{
    fprintf(IntermediateFile, "%d\t%d\t%s\t%s\t%s",
        lineNumber,
        LOCCTR,
        (LABEL != NULL) ? LABEL : "",
        (OPCODE != NULL) ? OPCODE : "",
        (OPERAND != NULL) ? OPERAND : "");
}

// Output of pass 2, writing a line to the ObjectFile when a new line is meant to be written
writeToObjectFile(FILE* ObjectFile, char *buffer)
{
    // Calculates length of machine code in buffer, accounting for header data (-9), rounding issues (+1), and byte representation (/2)
    int result = (strlen(buffer) - 9 + 1) / 2;
    // Converts and stores the calculated length into a two-character string
    char resultChars[3];
    sprintf_s(resultChars, sizeof(resultChars), "%02d", result); // Use x instead of d here for hex
    // Replaces the placeholder characters in buffer with the actual length
    buffer[7] = resultChars[0];
    buffer[8] = resultChars[1];
    // Writes the updated buffer to the ObjectFile
    fprintf(ObjectFile, "%s\n", buffer);
    // Clears buffer
    memset(buffer, 0, sizeof(buffer));
}

// The main function
int main()
{
    /*///////////////////////////////////////////////////////////
    // Prompts the user for the original SIC/XE file
    if (argc != 2)
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
    }
    ///////////////////////////////////////////////////////////*/

    /////////////////////////////////// DELETE THIS AFTER TESTING ///////////////////////////////////
    FILE* file = fopen("C:\\Users\\Charlie\\Desktop\\Bachelors\\CIS335\\Assignment 4\\SIC_PROG.txt", "r");
    //FILE* file = fopen("C:\\Users\\Hannah\\Dropbox\\2024 Fall\\CIS335 Language Processors\\Assignments\\Assignment 4\\SIC_PROG.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    /////////////////////////////////// DELETE THIS AFTER TESTING ///////////////////////////////////

    printf("Author Info: Hannah Simon & Charlie Strickland\n\n");

    char line[256], lineCopy[256];
    char* LABEL = NULL, * OPCODE = NULL, * OPERAND = NULL, * context = NULL;
    int LOCCTR = 0;
    bool firstLine = true;
    // Output file of pass 1
    FILE* IntermediateFile = fopen("IntermediateFile.txt", "w");
    // Output files of pass 2
    FILE* ListingFile = fopen("ListingFile.txt", "w");
    FILE* ObjectFile = fopen("ObjectCode.txt", "w");
    
    fprintf(IntermediateFile, "LINE\tLOCCTR\tSOURCE_STATEMENT\n");

    // Pass 1, loops through every line in the original SIC/XE file
    while (fgets(line, sizeof(line), file))
    {
        // Increments the line number by 5 every loop. Line number recorded for ease of reading, incremented by 5 to allow for extra room between in case we need to add a line
        lineNumber += 5;

        // If the first character in a line is '.' (indicating a comment), then copy that whole line to output file
        if (line[0] == '.')
        {
            strcpy_s(lineCopy, sizeof(lineCopy), line);
            fprintf(IntermediateFile, "%d\t%s", lineNumber, lineCopy);
            continue;
        }
        else if (line[0] != ' ') // If the first character in a line isn't blank, then tokenize the line and store the label, opcode, and operand
        {
            LABEL = strtok_s(line, " \n", &context);
            OPCODE = strtok_s(NULL, " \n", &context);
            OPERAND = strtok_s(NULL, " \n", &context);
        }
        else // If the first character in a line is blank, then the label is NULL but the rest can be tokenized appropriately
        {
            LABEL = NULL;
            OPCODE = strtok_s(line, " \n", &context);
            OPERAND = strtok_s(NULL, " \n", &context);
        }

        // If this is the first line in the file
        if (firstLine)
        {
            // And if the opcode is START
            if (strcmp(OPCODE, "START") == 0)
            {
                LOCCTR = atoi(OPERAND); // Then the location counter is the operand
            }
            fprintf(IntermediateFile, "%d\t%d\t%s\t%s\t%s\n", lineNumber, LOCCTR, LABEL, OPCODE, OPERAND);
            // If there is a label, then add it to the symbolTable
            if (LABEL != NULL)
            {
                addSymbol(LABEL, LOCCTR);
            }
            // First line read, therefore no longer first line
            firstLine = false;
            continue;
        }
        
        // If there is a label, add it to the symbolTable
        if (LABEL != NULL)
        {
            addSymbol(LABEL, LOCCTR);
        }

        // If the opcode is not START (which it shouldn't be)
        if (strcmp(OPCODE, "START") != 0)
        {
            bool isDirective = false;
            bool isOpcode = false;
            // Check if the opcode is a valid directive
            if (isValidDirective(OPCODE))
            {
                isDirective = true;
                // Write the line to the intermediate file
                writeToIntermediateFile(IntermediateFile, LOCCTR, LABEL, OPCODE, OPERAND, isOpcode);
                // If the opcode is END (occurs only at end of file), then break
                if (strcmp(OPCODE, "END") == 0)
                {
                    break;
                }
                else if (strcmp(OPCODE, "RESW") == 0) // Else if the opcode is RESW, update the LOCCTR by 3 * operand (indicating 'operand' number of words being reserved)
                {
                    LOCCTR += 3 * atoi(OPERAND);
                }
                else if (strcmp(OPCODE, "RESB") == 0) // Else if the opcode is RESB, update the LOCCTR by operand (indicating 'operand' number of bytes being reserved)
                {
                    LOCCTR += atoi(OPERAND);
                }
                else if (strcmp(OPCODE, "BYTE") == 0) // Else if the opcode is BYTE
                {
                    if (OPERAND[0] == 'C') // Operand is C, indicating a character constant
                    {
                        int length = strlen(OPERAND) - 3; // Get length of character constant -3 to account for 3 being 'C'
                        LOCCTR += length;
                    }
                    else if (OPERAND[0] == 'X') // Operand is X, indicating a hexadecimal constant
                    {
                        int hexLength = (strlen(OPERAND) - 3 + 1) / 2; // The +1 ensures rounding for odd numbers, Divide by 2 since 2 hex digits represent 1 byte
                        LOCCTR += hexLength;
                    }
                    else // If the operand is something else while the opcode is byte, the original SIC code is wrong and an error is thrown
                    {
                        printf("Error: Invalid BYTE format for operand: %s\n", OPERAND);
                        exit(EXIT_FAILURE);
                    }
                }
                else // If the opcode is something other than END, RESW/B, OR BYTE, then simply increment the LOCCTR
                {
                    LOCCTR += 3;
                }
                // Print the line to the output file
                fprintf(IntermediateFile, "\n");
            }
            else if (isValidOpcode(OPCODE)) // If the OPCODE is a valid opcode, but NOT a directive
            {
                isOpcode = true;
                // Write the line to the output file, start a new line, and increment the LOCCTR
                writeToIntermediateFile(IntermediateFile, LOCCTR, LABEL, OPCODE, OPERAND, isOpcode);
                fprintf(IntermediateFile, "\n");
                LOCCTR += 3;
            }
            else // If the line contains an OPCODE that is not in the valid OPCODE or DIRECTIVE list, throw an error
            {
                printf("Error: Pass 1, Line %d: Invalid operation '%s'\n", lineNumber, OPCODE);
                exit(EXIT_FAILURE);
            }
        }
    }

    // Close the file created by pass 1 from writing, and open it for reading
    fclose(IntermediateFile);
    fopen_s(IntermediateFile, "IntermediateFile.txt", "r");

    char* LINE = NULL, * ADDRESS = NULL;
    firstLine = true;
    char buffer[70] = { 0 };

    // Pass 2
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
