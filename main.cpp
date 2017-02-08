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
    FILE *f = fopen("exp1.mc", "r");
    initLexer(f);

    std::cout << "Analisando..." << std::endl;
    initParse();
    Exp *e = parseAtom();
    std::cout << e->toString() << std::endl;

    fclose(f);

    return 0;
}
