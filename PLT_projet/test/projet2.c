//
// Created by 田沛林 on 2020/12/3.
//

#include <stdio.h>
#include "../2_syn/calculate.h"
#include "../tools/Stack.h"


int main() {
    char *expr = "-(1-2)+(3-4)x(-5)";
    int res = calculateC(expr);
    printf("Calculate: %s = %d\n", expr, res);

    return 0;
}