# SIC/XE Assembler

## Overview
- SIC/XE is a hypothetical architecture introduced in *System Software: An Introduction to Systems Programming*, by Leland Beck to explain the concepts of assemblers, compilers, and operating systems [1; 2].
- The `sicasm.c` program implements a two-pass assembler for the SIC (Simplified Instructional Computer) machine architecture.
- The `sicxeasm.c` program implements a two-pass assembler for the SIC/XE (Simplified Instructional Computer with Extra Equipment) machine architecture.
- Both programs read an assembly language program for the SIC or SIC/XE machines respectively, process it, and produce three output files:
  - Intermediate File: A temporary file that can be safely deleted. It contains the source code and is utilized to generate the listing and object code files.
  - Listing File: This file contains the source code along with the corresponding object code (in hexadecimal) generated for each statement. It also includes a symbol table that lists all symbols and their corresponding addresses after the assembly process.
  - Object Code File: This file contains the final object code generated by the assembler, formatted according to SIC/XE standards. It includes a Header record, Text records, and an End record.
- The program handles basic directives and opcodes in the SIC/XE instruction set and performs error checking during both passes of the assembly process.

## Features
- Pass 1:
  - Parses the assembly source code to generate a symbol table and track location counters.
  - Checks for duplicate symbols and invalid opcodes.
- Pass 2:
  - Generates object code based on the symbol table and processes each statement.
  - Checks for undefined symbols and errors related to operand formats.
- Supported Directives: `START`, `BYTE`, `WORD`, `RESB`, `RESW`, `END`, `BASE`, `NOBASE`
- Supported Opcodes: A range of SIC/XE machine opcodes such as `ADD`, `SUB`, `LDA`, `STA`, `JSUB`, `RD`, `TD`, `RSUB`, and more.
- Input Format: The source file is a text file containing assembly instructions, comments, labels, opcodes, and operands formatted according to SIC/XE conventions.
- Output Format:
  1. An intermediate file (temporary file that can be safely deleted) containing:
      - Line numbers
      - Location counter values
      - Source statements
  2. A listing file containing:
      - Line numbers
      - Location counter values
      - Source statements
      - Object code
      - Symbol table
  3. An object file containing the final assembled object code

## Setup & Usage
1. Ensure the following prerequisites are installed:
    - C Compiler (e.g., GCC, MSVC)
    - Command-line interface to run the assembler program
    - Standard C libraries: `stdio.h`, `stdlib.h`, `string.h`, `stdbool.h`
2. Compile the source code:
    ```bash
    gcc sicasm.c -o sicasm
    ```
    ```bash
    gcc sicxeasm.c -o sicasm
    ```
3. Run the assembler with the input assembly source code file:
    ```bash
    ./sicasm SIC_PROG.txt
    ```
    ```bash
    ./sicxeasm SIC_PROG.txt
    ```
4. Each program will generate three output files:
    - `sic_intermediate.txt`, `sic_listing.txt`, and `sic_object.txt`
    - `sicxe_intermediate.txt`, `sicxe_listing.txt`, and `sicxe_object.txt`

## Sample Program Inputs & Outputs
- Sample input and output files are included in the repository for reference in the `SIC sample_io` and `SIC_XE sample_io` folders.
- `sicasm.c` Program:
  - Input: `SIC_PROG.txt`
  - Outputs: `sic_immediate.txt`, `sic_listing.txt`, `sic_object.txt`
- `sicxeasm.c` Program:
  - Input: `SIC_XE_PROG.txt`
  - Outputs: `sicxe_immediate.txt`, `sicxe_listing.txt`, `sicxe_object.txt`

## References
[1] Beck, L. L. (1997). *System Software: An Introduction to Systems Programming* (3rd ed.). Addison-Wesley.  
[2] Wikimedia Foundation. (2024, December 16). *Simplified Instructional Computer*. Wikipedia. https://en.wikipedia.org/wiki/Simplified_Instructional_Computer

## Authors
This project was collaboratively developed by Hannah G. Simon and Charlie Strickland.
