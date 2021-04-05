
#ifndef ANALEX_H
#define ANALEX_H

#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>

#define LAC_REG_N "(0|[1-9][0-9]*)"             // (0|[1-9][0-9]*)
#define LAC_REG_W  "[^\xff \"\n]+"              // [^ "\n]+
#define LAC_REG_S "(^|\n| )\" [^\"]*\""         // (^|\n| )" [^"]*"
#define LAC_REG_C "(^|\n| )\\( [^\\)]*\\)|(^|\n| )\\\\ [^\n]*"  // (^|\n| )\( [^\)]*\)|(^|\n| )\\ [^\n]*

#define ERROR_READ 1
#define ERROR_COMPILE 2

typedef struct lexeme {
    char type;  // S(String), N(Natural), M(Word)
    long beg;
    long end;
    char *content;
} *pLexeme;

typedef struct LEX {
    long tableLen;
    pLexeme *pLexArr;
} *pLEX;

long readFile(char *fileName, char **fileStrPt);

int matchANumber(regex_t lacNumber, char *fileStrCpy, long so, long eo);
void addLex(char lexType, long so, long eo, char *fileStrCpy, pLEX tableLEX);

long partitionLex(pLEX tableLEX, long left, long right);
void quickSortLex(pLEX tableLEX, long beg, long end);

pLEX lexemesInit(long size);
void lexemesFree(pLEX tableLEX);

pLEX anaLex(char *fileStr);
void printLexemes(pLEX tableLEX);

#endif