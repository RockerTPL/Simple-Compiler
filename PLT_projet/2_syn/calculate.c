//
// Created by 田沛林 on 2020/12/3.
//

#include "calculate.h"

char* exprConvert(char *expr) {
    /* add zeros in front of negative terms in the expression,
     * return the new expression */
    int exprLen = (int)strlen(expr);
    char *newExpr = (char *)calloc(exprLen * 2, sizeof(char));
    int i = 1, j;
    if (*expr == '-') {
        // expression begins with a negative term
        newExpr[0] = '0';
        newExpr[1] = '-';
        j = 2;
    } else if (*expr == ' ' || *expr == '\n') {
        j = 0;
    } else {
        newExpr[0] = expr[0];
        j = 1;
    }
    while (i < exprLen) {
        if (expr[i] == '-' && expr[i-1] == '(') {
            newExpr[j] = '0';
            newExpr[++j] = '-';
        } else if (expr[i] == ' ' || expr[i] == '\n') {
            i++;
            continue;
        }
        else {
            newExpr[j] = expr[i];
        }
        i++;
        j++;
    }
    return newExpr;
}

int matchNatural(const char *expr, int i) {
    /* last index of a number in an expression */
    if (!(expr[i]>='0' && expr[i]<='9')) {
        return i;
    }
    while (expr[i+1]>='0' && expr[i+1]<='9') {
        i++;
    }
    return i;
}

int anaLexCal(char *expr, lexemeCal **pLexemes) {
    /* lexical analysis,
     * return the length of pLexemes */
    int exprLen = (int)strlen(expr);
    int i = 0;
    int lexemesLen = 0;
    /// match and add lexemes
    while (i < exprLen) {
        lexemeCal *pLexemeCal;
        pLexemeCal = (lexemeCal *)malloc(sizeof(lexemeCal));
        /// match natural
        int j = matchNatural(expr, i);
        if (i < j || (expr[i]>='0' && expr[i]<='9')) {
            if (i < j && expr[i] == '0') {
                printf("Bad expression: %s\n", expr);
                exit(ERROR_LEX);
            }
            char *content = (char *)calloc(j - i + 2, sizeof(char));
            strncpy(content, expr + i,  sizeof(char) * (j - i + 1));
            pLexemeCal->content = content;
            pLexemeCal->type = 'N';
            pLexemes[lexemesLen] = pLexemeCal;
            lexemesLen += 1;
            i = j + 1;
            continue;
        }
        /// match parenthesis and operators
        if (expr[i] == '(') {
            pLexemeCal->type = 'L';
            pLexemeCal->content = "(";
        } else if (expr[i] == ')') {
            pLexemeCal->type = 'R';
            pLexemeCal->content = ")";
        } else {
            switch (expr[i]) {
                case '+':
                    pLexemeCal->content = "+";
                    break;
                case '-':
                    pLexemeCal->content = "-";
                    break;
                case 'x':
                    pLexemeCal->content = "x";
                    break;
                default:
                    printf("Lexical error in expression: %s\n", expr);
                    exit(ERROR_LEX);
            }
            pLexemeCal->type = 'O';
        }
        pLexemes[lexemesLen] = pLexemeCal;
        lexemesLen += 1;
        i++;
    }
    return lexemesLen;
}

void printLexemesCal(lexemeCal **pLexemes, int lexemesLen) {
    /* print elements in array of lexeme */
    for (int i = 0; i < lexemesLen - 1; i++) {
        printf("%c(\"%s\")->", pLexemes[i]->type, pLexemes[i]->content);
    }
    printf("%c(\"%s\")", pLexemes[lexemesLen - 1]->type, pLexemes[lexemesLen - 1]->content);
    printf("\n");
}

void freeLexemesCal(lexemeCal **pLexemes, int lexemesLen) {
    for (long i = 0; i < lexemesLen; i++) {
        if (pLexemes[i]->type == 'N') {
            free(pLexemes[i]->content);
        }
        free(pLexemes[i]);
    }
    free(pLexemes);
}


int isNatural(lexemeCal **pLexemes, int i) {
    /* whether the lexeme at index i is a natural number */
    return pLexemes[i]->type == 'N';
}

int isFactor(lexemeCal **pLexemes, int b, int f) {
    /* whether the lexemes between index b and f is a factor */
    return (isNatural(pLexemes, b) && b==f) ||
            (pLexemes[b]->type=='L' && pLexemes[f]->type=='R' && isSum(pLexemes, b+1, f-1));
}

int isSum(lexemeCal **pLexemes, int b, int f) {
    int i = f - 1;
    int r = isFactor(pLexemes, b, f);
    while (r == 0 && i > b) {
        r = (pLexemes[i]->type == 'O') && isFactor(pLexemes, i + 1, f) && isSum(pLexemes, b, i - 1);
        i--;
    }
    return r;
}

int anaSynCal(lexemeCal **pLexemes, int lexemesLen) {
    /* syntactic analysis */
    return isSum(pLexemes, 0, lexemesLen-1);
}


pOpTreeNode formOpTree(lexemeCal **pLexemes, int lexemesLen) {
    /* form a tree of operation based on the array of lexeme,
     * return the root of the tree */
    int maxPriority = 10;  // priority for parenthesis
    int opPriority = 0;    // priority of operators
    pOpTreeNode pNode = opTreeNodeNew();
    pOpTreeNode root = pNode;

    for (int i = 0; i < lexemesLen; i++) {
        if (pLexemes[i]->type == 'L') {
            opPriority += maxPriority;
        } else if (pLexemes[i]->type == 'R') {
            opPriority -= maxPriority;
        } else if (pLexemes[i]->type == 'N') {
            char *endPtr;
            pNode->value = (int)strtol(pLexemes[i]->content, &endPtr, 10);
        } else {
            // operator
            pOpTreeNode newNode = opTreeNodeNew();
            switch (*(pLexemes[i]->content)) {
                case '+':
                    newNode->priority = opPriority + 1;
                    newNode->op = '+';
                    break;
                case '-':
                    newNode->priority = opPriority + 1;
                    newNode->op = '-';
                    break;
                case 'x':
                    newNode->priority = opPriority + 2;
                    newNode->op = 'x';
                    break;
            }
            while (pNode->father && pNode->father->priority >= newNode->priority) {
                pNode = pNode->father;
            }
            if (!pNode->father) {
                pNode->father = root = newNode;
                root->lChild = pNode;
                pNode = root->rChild = opTreeNodeNew();
                root->rChild->father = root;
            } else {
                if (pNode == pNode->father->lChild) {
                    pNode->father->lChild = newNode;
                } else {
                    pNode->father->rChild = newNode;
                }
                newNode->father = pNode->father;
                newNode->lChild = pNode;
                pNode->father = newNode;
                pNode = newNode->rChild = opTreeNodeNew();
                pNode->father = newNode;
            }
        }
    }
    return root;
}

int calculateOpTree(pOpTreeNode root) {
    /* calculate an operation tree,
     * return the result */
    if(!root->lChild && !root->rChild) {
        return root->value;
    }
    switch(root->op) {
        case '+':
            return calculateOpTree(root->lChild) + calculateOpTree(root->rChild);
        case '-':
            return calculateOpTree(root->lChild) - calculateOpTree(root->rChild);
        case 'x':
            return calculateOpTree(root->lChild) * calculateOpTree(root->rChild);
        default:
            printf("Calculation error!\n");
            exit(ERROR_CAL);
    }
}


int calculateC(char *expr) {
    /* calculate the input expression,
     * return the result */
    /// convert expression
    char *exp = exprConvert(expr);
//    printf("new expression: %s\n", exp);

    /// lexical analysis
    lexemeCal **pLexemesCal;
    pLexemesCal = (lexemeCal **)malloc(MAX_LEX_LEN * sizeof(lexemeCal *));
    int lexemesLen = anaLexCal(exp, pLexemesCal);
//    printf("lexemesLen = %d\n", lexemesLen);
//    printLexemesCal(pLexemes, lexemesLen);

    /// syntactic analysis
    if (!anaSynCal(pLexemesCal, lexemesLen)) {
        printf("Syntactic error in expression: %s\n", expr);
        exit(ERROR_SYN);
    }

    /// form OpTree
    pOpTreeNode root = formOpTree(pLexemesCal, lexemesLen);

    /// calculate OpTree
    int res = calculateOpTree(root);

    /// free
    free(exp);
    freeLexemesCal(pLexemesCal, lexemesLen);
    opTreeFree(root);

    return res;
}