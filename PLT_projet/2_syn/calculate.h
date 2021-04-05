//
// Created by 田沛林 on 2020/12/3.
//

#ifndef PLT_PROJET_CALCULATE_H
#define PLT_PROJET_CALCULATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tools/OpTree.h"

#define MAX_LEX_LEN 50
#define ERROR_LEX 3
#define ERROR_SYN 4
#define ERROR_CAL 5

typedef struct lexemeCal {
    char type;  // N(Natural), L(Left parenthesis), R(Right parenthesis), O(Operator)
    char *content;
} lexemeCal;

char* exprConvert(char *expr);

int matchNatural(const char *expr, int i);
int anaLexCal(char *expr, lexemeCal **pLexemes);
void printLexemesCal(lexemeCal **pLexemes, int lexemesLen);
void freeLexemesCal(lexemeCal **pLexemes, int lexemesLen);

int isNatural(lexemeCal **pLexemes, int i);
int isFactor(lexemeCal **pLexemes, int b, int f);
int isSum(lexemeCal **pLexemes, int b, int f);
int anaSynCal(lexemeCal **pLexemes, int lexemesLen);

pOpTreeNode formOpTree(lexemeCal **pLexemes, int lexemesLen);
int calculateOpTree(pOpTreeNode root);

int calculateC(char *expr);

#endif //PLT_PROJET_CALCULATE_H