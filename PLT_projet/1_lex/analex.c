
#include "analex.h"

long readFile(char *fileName, char **fileStrPt) {
    /* read a lac file in directory "test" into a string,
     * return the size of file */
    FILE *fp;
    fp = fopen(fileName, "r");
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    *fileStrPt = (char *)calloc(fileSize, sizeof(char) + 2);
    fread(*fileStrPt, sizeof(char), fileSize, fp);
    (*fileStrPt)[fileSize] = '\n';

    return fileSize;
}


int matchANumber(regex_t lacNumber, char *fileStrCpy, long so, long eo) {
    /* match a number */
    regmatch_t pMatch[10];
    if (regexec(&lacNumber, fileStrCpy, 1, pMatch, 0) == REG_NOMATCH
    || eo != pMatch[0].rm_so || so != pMatch[0].rm_eo) {
        return 0;
    }
    return 1;
}

void addLex(char lexType, long so, long eo, char *fileStrCpy, pLEX tableLEX) {
    /* add a lexeme into the array pLexArr in tableLEX */
    pLexeme pLex;
    pLex = (pLexeme)malloc(sizeof(struct lexeme));
    pLex->type = lexType;
    pLex->beg = so;
    pLex->end = eo;
    char *content;
    if (lexType == 'S') {
        if (fileStrCpy[so] == '\"') {
            content = (char *)calloc(eo - so - 2, sizeof(char));
            strncpy(content, fileStrCpy + so + 2,  sizeof(char) * (eo - so - 3));
        } else {
            content = (char *)calloc(eo - so - 3, sizeof(char));
            strncpy(content, fileStrCpy + so + 3,  sizeof(char) * (eo - so - 4));
        }
    } else {
        content = (char *)calloc(eo - so + 1, sizeof(char));
        strncpy(content, fileStrCpy+so,  sizeof(char) * (eo - so));
    }
    pLex->content = content;
    tableLEX->pLexArr[tableLEX->tableLen] = pLex;
    tableLEX->tableLen += 1;
    for (long i = so; i < eo; i++) {
        fileStrCpy[i] = -1;
    }
}


long partitionLex(pLEX tableLEX, long left, long right) {
    /* for quick sort */
    long pivot = tableLEX->pLexArr[(left + right) / 2]->beg;
    long i = left, j = right;
    while (1) {
        while (tableLEX->pLexArr[i]->beg < pivot) {
            i++;
        }
        while (tableLEX->pLexArr[j]->beg > pivot) {
            j--;
        }
        if (i < j) {
            pLexeme temp = tableLEX->pLexArr[i];
            tableLEX->pLexArr[i] = tableLEX->pLexArr[j];
            tableLEX->pLexArr[j] = temp;
        } else {
            break;
        }
    }
    return i;
}

void quickSortLex(pLEX tableLEX, long beg, long end) {
    /* quick sort an array of lexemes */
    long i;
    if (end > beg) {
        i = partitionLex(tableLEX, beg, end);
        quickSortLex(tableLEX, beg, i - 1);
        quickSortLex(tableLEX, i + 1, end);
    }
}


pLEX lexemesInit(long size) {
    /* init table of lexemes*/
    pLEX tableLEX = (pLEX)malloc(sizeof(struct LEX));
    tableLEX->tableLen = 0;
    tableLEX->pLexArr = (pLexeme *)malloc(size * sizeof(pLexeme));
    return tableLEX;
}

void lexemesFree(pLEX tableLEX) {
    /* free table of lexemes */
    for (long i = 0; i < tableLEX->tableLen; i++) {
        free(tableLEX->pLexArr[i]->content);
        free(tableLEX->pLexArr[i]);
    }
    free(tableLEX->pLexArr);
    free(tableLEX);
}


pLEX anaLex(char *fileStr) {
    /* lexical analysis,
     * return the tableLEX */
    /// copy file string
    unsigned long fileStrLen = strlen(fileStr);
    char *fileStrCpy;
    fileStrCpy = (char *)calloc(fileStrLen, sizeof(char));
    strcpy(fileStrCpy, fileStr);

    /// init tableLEX
    pLEX tableLEX = lexemesInit((long)strlen(fileStrCpy));

    /// compile
    regex_t lacNumber;
    regex_t lacWord;
    regex_t lacString;
    regex_t lacComment;
    if (regcomp(&lacNumber, LAC_REG_N, REG_EXTENDED)
    || regcomp(&lacWord, LAC_REG_W, REG_EXTENDED)
    || regcomp(&lacString, LAC_REG_S, REG_EXTENDED)
    || regcomp(&lacComment, LAC_REG_C, REG_EXTENDED)) {
        printf("Bad rational expression!\n");
        exit(ERROR_COMPILE);
//        return tableLEX;
    }

    /// match lac types
    regmatch_t pMatch[10];
    /// match comment
    while (1) {
        if (regexec(&lacComment, fileStrCpy, 1, pMatch, 0) == REG_NOMATCH) {
            break;
        } else {
            for (int i = pMatch[0].rm_so; i < pMatch[0].rm_eo; i++) {
                fileStrCpy[i] = -1;
            }
        }
    }
    /// match string
    while (1) {
        if (regexec(&lacString, fileStrCpy, 1, pMatch, 0) == REG_NOMATCH) {
            break;
        } else {
            addLex('S', pMatch[0].rm_so, pMatch[0].rm_eo, fileStrCpy, tableLEX);
        }
    }
    /// match word and number
    while (1) {
        if (regexec(&lacWord, fileStrCpy, 1, pMatch, 0) == REG_NOMATCH) {
            break;
        } else if (matchANumber(lacNumber, fileStrCpy, pMatch[0].rm_eo, pMatch[0].rm_so)) {
            addLex('N', pMatch[0].rm_so, pMatch[0].rm_eo, fileStrCpy, tableLEX);
        } else {
            addLex('M', pMatch[0].rm_so, pMatch[0].rm_eo, fileStrCpy, tableLEX);
        }
    }

    quickSortLex(tableLEX, 0, tableLEX->tableLen - 1);

    free(fileStrCpy);
    return tableLEX;
}

void printLexemes(pLEX tableLEX) {
    /* print the table of lexemes */
    for (long i = 0; i < tableLEX->tableLen - 1; i++) {
        printf("%c(\"%s\")->", tableLEX->pLexArr[i]->type, tableLEX->pLexArr[i]->content);
    }
    printf("%c(\"%s\")", tableLEX->pLexArr[tableLEX->tableLen - 1]->type, tableLEX->pLexArr[tableLEX->tableLen - 1]->content);
    printf("\n");
}
