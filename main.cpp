//
// Created by Andrei Formiga on 2/3/17.
//

#include <iostream>
#include "lexer.h"

using namespace std;

int main() {
    initLexer(stdin);

    Token *tok = getNextToken();

    while (tok->type != TokType::Eof) {
        printf("Token encontrado: %s\n", printTokenType(*tok));
        tok = getNextToken();
    }

    return 0;
}
