//
// Created by Andrei Formiga on 2/7/17.
//

#ifndef MINICC_PARSER_CPP_H
#define MINICC_PARSER_CPP_H

#include "ast.h"

Exp* parseAtom();
void initParse();
Exp* parseBinOp();
void testParseExp();
Programa* parsePrograma();

#endif //MINICC_PARSER_CPP_H
