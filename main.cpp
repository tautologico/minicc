//
// Created by Andrei Formiga on 2/3/17.
//

#include <iostream>
#include "lexer.h"
#include "ast.h"
#include "parser.h"

using namespace std;

int main() {
    //testLexer();
    //testParseExp();

    FILE *f = fopen("print.mc", "r");
    initLexer(f);
    Programa *p = parsePrograma();
    
    testParseExp();

    return 0;
}
