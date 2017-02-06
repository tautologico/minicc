//
// Created by Andrei Formiga on 2/3/17.
//

#ifndef MINICC_LEXER_H
#define MINICC_LEXER_H

// tipos
enum class TokType {
    Identificador,
    LiteralNum,
    PalavraChave,
    Operador,
    Pontuacao,
    Eof
};

enum Op {
    OpSoma = 1,
    OpMult,
    OpLt,
    OpAssign,
    OpEq
};

enum PChave {
    Int = 0,
    Printf = 1,
    Printint = 2,
    Return = 3
};

enum Pont {
    LParen = 0,
    RParen,
    LBrace,
    RBrace,
    Comma,
    Semicolon
};

struct Token {
    TokType type;
    int val;
    std::string nome;
};

// funcoes
void testLexer();
Token* getNextToken();
const char* printTokenType(TokType &type);
void initLexer(FILE *f);



#endif //MINICC_LEXER_H
