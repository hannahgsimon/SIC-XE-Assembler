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

// Struct for an opcode containing its name, format, hex code, and number of expected operands
typedef struct OperationCodeTable
{
    char Mnemonic[6];
    char Format;
    unsigned short int MachineCode;
    unsigned short int NumberOperands;
}SIC_OPTAB;

// Table of opcodes (struct defined above)
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

// Checks if the opcode name it is sent is in the opcode table
int isValidOpcode(char* OPCODE)
{
    // If opcode starts with + for format 4, ignore it
    if (OPCODE[0] == '+')
    {
        OPCODE = OPCODE + 1;
    }
    for (int i = 0; i < OPTAB_SIZE; i++)
    {
        if (strcmp(OPTAB[i].Mnemonic, OPCODE) == 0)
        {
            return 1; // Valid opcode
        }
    }
    return 0; // Invalid opcode
}

unsigned short int getMachineCode(const char* OPCODE)
{
    // If opcode starts with +, skip the + character
    if (OPCODE[0] == '+') 
    {
        OPCODE++;
    }
    
    for (int i = 0; i < OPTAB_SIZE; i++)
    {
        if (strcmp(OPTAB[i].Mnemonic, OPCODE) == 0)
        {
            return OPTAB[i].MachineCode;
        }
    }
    return 0; // Return 0 if opcode not found
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

int getSymbolAddress(char* nameInput)
{
    // Create a copy of the input name
    char name[20];
    strncpy(name, nameInput, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0'; // Ensure null termination

    if (strlen(name) >= 2 && name[strlen(name) - 2] == ',' && name[strlen(name) - 1] == 'X')
    {
        name[strlen(name) - 2] = '\0';
    }
    else if (name[0] == '@' || name[0] == '#')
    {
        memmove(name, name + 1, strlen(name));
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

int convertToTwosComplement(int displacement, int bits) 
{
    // If number is negative
    if (displacement < 0) 
    {
        // Calculate two's complement
        int mask = (1 << bits) - 1;  // Create mask for specified bits
        displacement = ((-displacement) ^ mask) + 1;
    }
    return displacement;
}

const char* RegisterCodes[] = 
{
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

const char* getRegisterCode(char letter) 
{
    switch (letter) 
    {
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

void writeToIntermediateFile(FILE* IntermediateFile, int LOCCTR, char* LABEL, char* OPCODE, char* OPERAND, bool isOpcode)
{
    fprintf(IntermediateFile, "%d\t%04X\t%s\t%s\t%s",
        lineNumber,
        LOCCTR,
        (LABEL != NULL) ? LABEL : "",
        (OPCODE != NULL) ? OPCODE : "",
        (OPERAND != NULL) ? OPERAND : "");
}

void writeToObjectFile(FILE* ObjectFile, char* buffer)
{
    int result = (strlen(buffer) - 9 + 1) / 2;  // +1 for rounding up
    char resultChars[3];
    sprintf_s(resultChars, sizeof(resultChars), "%02X", result);
    buffer[7] = resultChars[0];
    buffer[8] = resultChars[1];
    fprintf(ObjectFile, "%s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
}

char* intToBinary(int n) 
{
    // Allocate enough space for 8 bits (one byte) plus null terminator
    char* binaryString = malloc(9);
    if (binaryString == NULL) 
    {
        return NULL;
    }
    
    // Always generate 8 bits, filling from right to left
    for (int i = 7; i >= 0; i--) 
    {
        binaryString[i] = (n & 1) ? '1' : '0';
        n >>= 1;
    }
    binaryString[8] = '\0';
    
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
    char* hexString = malloc(hexLen + 1);
    hexString[hexLen] = '\0';

    int value = 0;
    int j = hexLen - 1;

    // Process binary string from right to left, 4 bits at a time
    for (int i = len - 1; i >= 0; i -= 4)
    {
        value = 0;
        // Convert each 4-bit chunk to a decimal value
        for (int k = 0; k < 4 && (i - k) >= 0; k++)
        {
            if (binary[i - k] == '1')
            {
                value += (1 << k);
            }
        }
        // Store the corresponding hex character
        hexString[j--] = (value < 10) ? (value + '0') : (value - 10 + 'A');
    }
    return hexString;
}

char* hexToBinary(const char* hex) {
    int len = strlen(hex);
    // Allocate enough space for the binary result (4 bits per hex digit)
    char* binary = malloc(len * 4 + 1);
    if (binary == NULL) {
        return NULL;  // Error handling if malloc fails
    }
    binary[0] = '\0';  // Initialize the binary string as an empty string

    // Map each hex character to its 4-bit binary equivalent
    for (int i = 0; i < len; i++) 
        {
        switch (hex[i]) 
        {
        case '0': strcat_s(binary, len * 4 + 1, "0000"); break;
        case '1': strcat_s(binary, len * 4 + 1, "0001"); break;
        case '2': strcat_s(binary, len * 4 + 1, "0010"); break;
        case '3': strcat_s(binary, len * 4 + 1, "0011"); break;
        case '4': strcat_s(binary, len * 4 + 1, "0100"); break;
        case '5': strcat_s(binary, len * 4 + 1, "0101"); break;
        case '6': strcat_s(binary, len * 4 + 1, "0110"); break;
        case '7': strcat_s(binary, len * 4 + 1, "0111"); break;
        case '8': strcat_s(binary, len * 4 + 1, "1000"); break;
        case '9': strcat_s(binary, len * 4 + 1, "1001"); break;
        case 'A': case 'a': strcat_s(binary, len * 4 + 1, "1010"); break;
        case 'B': case 'b': strcat_s(binary, len * 4 + 1, "1011"); break;
        case 'C': case 'c': strcat_s(binary, len * 4 + 1, "1100"); break;
        case 'D': case 'd': strcat_s(binary, len * 4 + 1, "1101"); break;
        case 'E': case 'e': strcat_s(binary, len * 4 + 1, "1110"); break;
        case 'F': case 'f': strcat_s(binary, len * 4 + 1, "1111"); break;
        default:
            free(binary);
            return NULL; // Return NULL if an invalid character is found
        }
    }

    return binary; // Return the constructed binary string
}

void startLineObjectFile(char* buffer, size_t bufferSize, char* ADDRESS)
{
    buffer[0] = 'T';
    char paddedAddress[7];
    snprintf(paddedAddress, sizeof(paddedAddress), "%06s", ADDRESS);
    strncat_s(buffer, bufferSize, paddedAddress, bufferSize - strlen(buffer) - 1);
    strncat_s(buffer, bufferSize, "@@", bufferSize - strlen(buffer) - 1);   // Placeholder line length characters
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("\nUsage: %s <file_name>\n", argv[0]);
        return 1;
    }

    char* file_path = argv[1];
    FILE* file = fopen(file_path, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    printf("\nAuthor Info: Hannah Simon & Charlie Strickland\n\n");

    char line[256], lineCopy[256];
    char * LABEL = NULL, * OPCODE = NULL, * OPERAND = NULL, * context = NULL;
    int LOCCTR = 0;
    bool firstLine = true;
    FILE* IntermediateFile = fopen("sicxe_intermediate.txt", "w");

    fprintf(IntermediateFile, "LINE\tLOCCTR\t   SOURCE_STATEMENT\n");

    // Pass 1
    while (fgets(line, sizeof(line), file))
    {
        // Increase line number by 5 each line
        lineNumber += 5;

        // If the line is a comment
        if (line[0] == '.')
        {
            strcpy_s(lineCopy, sizeof(lineCopy), line); 
            fprintf(IntermediateFile, "%d\t%s", lineNumber, lineCopy); // Copy the line directly to the intermediate file
            continue;
        }
        else if (line[0] != ' ') // Else if the first char in the line isn't empty (a label is present)
        {
            // Tokenize line and assign each variable appropriately
            LABEL = strtok_s(line, " \n", &context);
            OPCODE = strtok_s(NULL, " \n", &context);
            OPERAND = strtok_s(NULL, " \n", &context);
        }
        else // Else if the first char in the line is empty
        {
            LABEL = NULL; // Tokenize and assign each properly, but LABEl is NULL
            OPCODE = strtok_s(line, " \n", &context);
            OPERAND = strtok_s(NULL, " \n", &context);
        }

        // If first line of file
        if (firstLine) 
        {
            if (strcmp(OPCODE, "START") == 0)
            {
                LOCCTR = atoi(OPERAND); // Set LOCCTR to wherever START indicates
            }
            fprintf(IntermediateFile, "%d\t%04X\t%s\t%s\t%s\n", lineNumber, LOCCTR, LABEL, OPCODE, OPERAND); // Write line to file
            if (LABEL != NULL) // If theres a label, add it to symbol table
            {
                addSymbol(LABEL, LOCCTR);
            }
            firstLine = false; // No longer first line
            continue;
        }

        if (LABEL != NULL) // If there is a label, add it to the symbol tabel with its LOCCTR
        {
            addSymbol(LABEL, LOCCTR);
        }
        if (strcmp(OPCODE, "START") != 0) // If the opcode isn't START (since START should only appear once)
        {
            bool isDirective = false;
            bool isOpcode = false;
            if (isValidDirective(OPCODE)) // If the opcode is an allowed directive...
            {
                isDirective = true;
                writeToIntermediateFile(IntermediateFile, LOCCTR, LABEL, OPCODE, OPERAND, isOpcode); // Write the line to the file
                if (strcmp(OPCODE, "END") == 0) // If it's END, end of file
                {
                    break;
                }
                else if (strcmp(OPCODE, "BASE") == 0)
                {
                    LOCCTR = LOCCTR;
                }
                else if (strcmp(OPCODE, "RESW") == 0) // Increment LOCCTR by 3 bytes per reserved word
                {
                    LOCCTR += 3 * atoi(OPERAND);
                }
                else if (strcmp(OPCODE, "RESB") == 0) // Increment LOCCTR by 1 byte per reserved byte
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
                    else // Error
                    {
                        printf("Error: Pass 1, Line %d: Invalid BYTE format for operand %s\n", lineNumber, OPERAND);
                        exit(EXIT_FAILURE);
                    }
                }
                else // Valid directive, but nothing that manipulates the LOCCTR unnaturally
                {
                    LOCCTR += 3;
                }
                fprintf(IntermediateFile, "\n"); // New line after determining new LOCCTR
            }
            else if (isValidOpcode(OPCODE)) // Opcode is valid but NOT a directive
            {
                isOpcode = true;
                writeToIntermediateFile(IntermediateFile, LOCCTR, LABEL, OPCODE, OPERAND, isOpcode); // Write to file
                fprintf(IntermediateFile, "\n"); // New line
                char format = getFormat(OPCODE); // Increment LOCCTR appropriately per format
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
                else // Error if opcode not correct
                {
                    printf("Error: Pass 1, Line %d: Invalid format for opcode '%s'\n", lineNumber, OPCODE);
                }
            }
            else // Not an opcode or directive
            {
                printf("Error: Pass 1, Line %d: Invalid operation '%s'\n", lineNumber, OPCODE);
                exit(EXIT_FAILURE);
            }
        }
    }
    // End of pass 1, close intermediate for writing, open for reading
    fclose(IntermediateFile);

    // Start of pass 2
    fopen_s(IntermediateFile, "sicxe_intermediate.txt", "r");
    FILE* ListingFile = fopen("sicxe_listing.txt", "w");
    FILE* ObjectFile = fopen("sicxe_object.txt", "w");
    char* LINE = NULL, * ADDRESS = NULL;
    firstLine = true; char* LINE2 = NULL, * ADDRESS2 = NULL; bool baseSet = false; int baseAddress = NULL;
    char nextLine[256] = { 0 };
    char buffer[70] = { 0 };
    int startingAddress = 0;

    // Pass 2
    while (fgets(line, sizeof(line), IntermediateFile))
    {
        strcpy_s(lineCopy, sizeof(lineCopy), line); // Copy line in from intermediate
        
        size_t len = strlen(lineCopy); 
        if (len > 0 && lineCopy[len - 1] == '\n') // Making sure string is properly null terminated for manipulation later on3
        {
            lineCopy[len - 1] = '\0';
        }

        if (firstLine && line[0] != '.') // First line AND first line intermediate isn't a comment
        {
            fprintf(ListingFile, "%s\tOBJ_CODE\n", lineCopy); // Append OBJ_CODE to column identifiers
            firstLine = false;
            continue;
        }

        // Tokenize each column in
        LINE = strtok_s(line, "\t\n", &context);
        ADDRESS = strtok_s(NULL, " \t\n", &context);
        LABEL = strtok_s(NULL, " \t\n", &context);
        OPCODE = strtok_s(NULL, " \t\n", &context);
        OPERAND = strtok_s(NULL, " \t\n", &context);

        if (LINE == NULL)
        {
            break;
        }
        if (OPCODE == NULL) // Make sure variables are read in correctly, accounting for whitespace
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

        if (strcmp(ADDRESS, ".") == 0) // Comment, directly copy to listing
        {
            fprintf(ListingFile, "%s\n", lineCopy);
            continue;
        }
        if (strcmp(OPCODE, "START") == 0) // START directive, only appears once, copy line to listing and start object file
        {
            startingAddress = (int)strtol(ADDRESS, NULL, 16);
            fprintf(ListingFile, "%s\n", lineCopy);
            fprintf(ObjectFile, "H%s\t%06s%06X\n", LABEL, ADDRESS, LOCCTR - atoi(ADDRESS)); // H (1) + program name (2-7) + starting address in hex (8-13) + length of program in bytes, in hex (14-19)
            continue;
        }

        unsigned short int OPCODEINT = NULL;
        int ADDR = NULL;
        char objectCode[33];
        char format = getFormat(OPCODE);
        
        if (strcmp(OPCODE, "BASE") == 0) // Use base addressing if PC addressing not available. LOCCTR - B where B is the address of the symbol BASE indicates
        {
            baseSet = true;
            baseAddress = getSymbolAddress(OPERAND);
            fprintf(ListingFile, "%s\n", lineCopy);
            continue;
        }
        else if (strcmp(OPCODE, "NOBASE") == 0) // Turn off base addressing
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
            snprintf(objectCode, sizeof(objectCode), "%02X", OPCODEINT);
        }
        else if (format == '2')
        {
            OPCODEINT = getMachineCode(OPCODE);
            if (strlen(OPERAND) == 1)
            {
                int code = binaryToInt(getRegisterCode(OPERAND[0]));
                snprintf(objectCode, sizeof(objectCode), "%02X%01X0", OPCODEINT, code);
            }
            else if (strlen(OPERAND) == 3)
                {
                int code1 = binaryToInt(getRegisterCode(OPERAND[0]));
                int code2 = binaryToInt(getRegisterCode(OPERAND[2]));
                snprintf(objectCode, sizeof(objectCode), "%02X%01X%01X", OPCODEINT, code1, code2);
            }
        }
        else if (format == '3') // Else if format 3 / 4
        {
            char binaryString[13];
            char* OPCODECHAR = intToBinary(getMachineCode(OPCODE));
            OPCODECHAR[6] = '\0';
           
           // If operand is #number
            if (OPERAND != NULL && OPERAND[0] == '#' && isalpha(OPERAND[1]) == 0)
            {
                int number = atoi(OPERAND + 1);
                // If 0 <= number <= 4095
                if (number >= 0 && number <= 4095)
                {
                    snprintf(binaryString, sizeof(binaryString), "%s010000", OPCODECHAR); // opcode + flags 010000
                    char* hexString = binaryToHex(binaryString);
                    snprintf(objectCode, sizeof(objectCode), "%03s%03X", hexString, number); // + 12 bit immediate value
                }
                else if (number >= 0 && number <= 1048575 && OPCODE[0] == '+') // Else if 4096 <= number <= 1048575 AND + before opcode
                {
                    snprintf(binaryString, sizeof(binaryString), "%s010001", OPCODECHAR); // opcode + flags 010001
                    char* hexString = binaryToHex(binaryString);
                    snprintf(objectCode, sizeof(objectCode), "%03s%05X", hexString, number); // + 20 bit immediate value
                }
                else // Error
                {
                    printf("Error: Pass 2, Line %s: Immediate number out of range %s\n", LINE, OPERAND);
                    exit(EXIT_FAILURE);
                }
            }
            else if (OPERAND != NULL) // Else if operand is not blank
            {
                ADDR = getSymbolAddress(OPERAND);
                if (ADDR == NULL) // Confirms symbol existence
                {
                    printf("Error: Pass 2, Line %s: Symbol not found %s\n", LINE, OPERAND);
                    exit(EXIT_FAILURE);
                }

                // Get next instruction's address for PC-relative addressing
                long int currentPos = ftell(IntermediateFile);  // Save the current file position
                if (fgets(nextLine, sizeof(nextLine), IntermediateFile))
                {
                    LINE2 = strtok_s(nextLine, "\t\n", &context);
                    ADDRESS2 = strtok_s(NULL, " \t\n", &context);
                }
                fseek(IntermediateFile, currentPos, SEEK_SET);


                if (OPCODE[0] == '+') // Format 4
                {
                    snprintf(binaryString, sizeof(binaryString), "%s110001", OPCODECHAR);
                    char* hexString = binaryToHex(binaryString);
                    snprintf(objectCode, sizeof(objectCode), "%03s%05X", hexString, ADDR & 0xFFFFF);
                }
                else // Try PC-relative first
                {
                    int pc = (ADDRESS2 != NULL) ? (int)strtol(ADDRESS2, NULL, 16) : 0;
                    int displacement = ADDR - pc;
                    
                    if (displacement >= -2048 && displacement <= 2047) 
                    {
                        snprintf(binaryString, sizeof(binaryString), "%s110010", OPCODECHAR);
                        char* hexString = binaryToHex(binaryString);
                        if (displacement < 0) 
                        {
                            displacement = convertToTwosComplement(displacement, 12);
                        }
                        snprintf(objectCode, sizeof(objectCode), "%03s%03X", hexString, displacement & 0xFFF);
                    }
                    else if (baseSet && (ADDR - baseAddress >= 0) && (ADDR - baseAddress <= 4095)) // Try base-relative if PC-relative fails
                    {
                        snprintf(binaryString, sizeof(binaryString), "%s110100", OPCODECHAR);
                        char* hexString = binaryToHex(binaryString);
                        int displacement = ADDR - baseAddress;
                        snprintf(objectCode, sizeof(objectCode), "%03s%03X", hexString, displacement & 0xFFF);
                    }
                    else 
                    {
                        printf("Error: Pass 2, Line %s: Address out of range for format 3\n", LINE);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else // Else operand is blank (ex. RSUB)
            {
                snprintf(binaryString, sizeof(binaryString), "%s110000", OPCODECHAR); // opcode + flags 110000
                char* hexString = binaryToHex(binaryString);
                snprintf(objectCode, sizeof(objectCode), "%03s000", hexString); // + 12 bit displacement (000)
            }
            
            if (OPERAND != NULL)
            {
                if (OPERAND[0] == '#') // If # before operand
                {
                    char* objectCodeHex = hexToBinary(objectCode);
                    objectCodeHex[6] = '0'; // set n bit to 0
                    snprintf(objectCode, sizeof(objectCode), "%s", binaryToHex(objectCodeHex));
                }
                else if (OPERAND[0] == '@') // If @ before operand
                {
                    char* objectCodeHex = hexToBinary(objectCode);
                    objectCodeHex[7] = '0'; // set i bit to 0
                    snprintf(objectCode, sizeof(objectCode), "%s", binaryToHex(objectCodeHex));
                } 
                //else if (strlen(OPERAND) >= 2 && OPERAND[strlen(OPERAND) - 2] == ',' && OPERAND[strlen(OPERAND) - 1] == 'X') // If line contains unexpected ,X
                else if (strstr(OPERAND, ",X") != NULL)
                {
                    char* objectCodeHex = hexToBinary(objectCode);
                    objectCodeHex[8] = '1'; // set x bit to 1
                    snprintf(objectCode, sizeof(objectCode), "%s", binaryToHex(objectCodeHex));
                }
            }
        }
        else if (strcmp(OPCODE, "END") != 0)
        {
            printf("Error: Pass 2, Line %s: Invalid format for opcode '%s'\n", LINE, OPCODE);
            exit(EXIT_FAILURE);
        }
        
        if (strcmp(OPCODE, "END") == 0)
        {
            fprintf(ListingFile, "%s\n", lineCopy);
            if (strlen(buffer) > 0)
            {
                writeToObjectFile(ObjectFile, buffer);
            }
            fprintf(ObjectFile, "E%06X", startingAddress);
            break;
        }

        fprintf(ListingFile, "%s\t%s\n", lineCopy, objectCode);

        // Writing text (T) records to object file
        if (buffer[0] == '\0') // If buffer is empty, start a new line
        {
            startLineObjectFile(buffer, sizeof(buffer), ADDRESS);
        }
        if (strlen(buffer) + strlen(objectCode) > 69) // If the buffer would be over 69 characters, write the buffer into the file and start a new one
        {
            writeToObjectFile(ObjectFile, buffer);
            startLineObjectFile(buffer, sizeof(buffer), ADDRESS);
            strcat_s(buffer, sizeof(buffer), objectCode); // Once new line has started, add current object code
        }
        else // Add the object code to the buffer
        {
            strcat_s(buffer, sizeof(buffer), objectCode);
        }
    }

    // Prints symbol table to listing
    fprintf(ListingFile, "\nSYMBOL\tADDRESS\n");
    for (int i = 0; i < symbolCount; i++)
    {
        fprintf(ListingFile, "%s\t%04X", symbolTable[i].name, symbolTable[i].address);
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
