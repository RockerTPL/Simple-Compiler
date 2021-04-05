
#include <stdio.h>
#include "../1_lex/analex.h"

int main() {
    /// read file
    char *fileName = "factorielle.lac";
    char *fileStr;
    long fileSize = readFile(fileName, &fileStr);
    if (fileSize <= 0) {
        printf("Read file error!\n");
        exit(ERROR_READ);
    }

    /// lexical analysis
    pLEX tableLEX = anaLex(fileStr);
    printLexemes(tableLEX);

    /// free
    free(fileStr);
    lexemesFree(tableLEX);
    return 0;
}