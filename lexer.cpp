//
// Created by Andrei Formiga on 2/3/17.
//

#include <string>
#include <cstdlib>
#include "lexer.h"

Token tok;

int buffer;
bool bufferValid = false;

int getNextChar() {
    if (!bufferValid)
        buffer = getchar();
    else
        bufferValid = false;
    return buffer;
}

void returnChar(int c) {
    buffer = c;
    bufferValid = true;
}

void testLexer() {
    // TODO: testar o analisador lexico
}

char *palavrasChave[] = { "int", "printf", "printint", "return"};

int identificaPalavraChave(const char *s) {
    for (int i = 0; i < 4; ++i) {
        if (strcmp(s, palavrasChave[i]) == 0)
            return i;
    }

    return -1;
}

void lexerError(const char *err) {
    fprintf(stderr, "Erro lexico: %s\n", err);
    exit(1);
}

const char* printTokenType(Token &tok) {
    switch (tok.type) {
        case TokType::PalavraChave:
            return "PCHAVE";
        case TokType::Identificador:
            return "ID";
        case TokType::LiteralNum:
            return "NUM";
        case TokType::Operador:
            return "OP";
        case TokType::Eof:
            return "EOF";
        case TokType::Pontuacao:
            return "PONT";
    }
}

Token *getNextToken() {
    int c = getNextChar();

    // pula espaco em branco
    while (isspace(c))
        c = getNextChar();

    bool done = false;
    while (!done) {
        switch (c) {
            case '/':
                c = getNextChar();
                if (c != '/') {
                    lexerError("comentario esperado");
                    return nullptr;
                }
                while (c != '\n')
                    c = getNextChar();
                c = getNextChar();
                break;

            case '+':
                tok.type = TokType::Operador;
                tok.val = OpSoma;
                done = true;
                break;

            case '*':
                tok.type = TokType::Operador;
                tok.val = OpMult;
                done = true;
                break;

            case '<':
                tok.type = TokType::Operador;
                tok.val = OpLt;
                done = true;
                break;

            case '=':
                tok.type = TokType::Operador;
                c = getNextChar();
                if (c == '=')
                    tok.val = OpEq;
                else {
                    tok.val = OpAssign;
                    returnChar(c);
                }
                done = true;
                break;

            case '(':
                tok.type = TokType::Pontuacao;
                tok.val = LParen;
                done = true;
                break;

            case ')':
                tok.type = TokType::Pontuacao;
                tok.val = RParen;
                done = true;
                break;

            case '{':
                tok.type = TokType::Pontuacao;
                tok.val = LBrace;
                done = true;
                break;

            case '}':
                tok.type = TokType::Pontuacao;
                tok.val = RBrace;
                done = true;
                break;

            case ',':
                tok.type = TokType::Pontuacao;
                tok.val = Comma;
                done = true;
                break;

            case ';':
                tok.type = TokType::Pontuacao;
                tok.val = Semicolon;
                done = true;
                break;

            case -1:
                tok.type = TokType::Eof;
                done = true;
                break;

            default:
                if (isdigit(c)) {
                    std::string num;

                    while (isdigit(c)) {
                        num += c;
                        c = getNextChar();
                    }
                    returnChar(c);
                    tok.type = TokType::LiteralNum;
                    tok.val = atoi(num.c_str());
                    done = true;
                } else if (isalpha(c)) {
                    std::string s;

                    while (isalnum(c)) {
                        s += c;
                        c = getNextChar();
                    }
                    returnChar(c);

                    int ix = identificaPalavraChave(s.c_str());
                    if (ix == -1) {
                        tok.type = TokType::Identificador;
                        tok.nome = s;
                    } else {
                        tok.type = TokType::PalavraChave;
                        tok.val = ix;
                    }
                    done = true;
                } else {
                    std::string error = "Caractere nao esperado: " + static_cast<char>(c);
                    lexerError(error.c_str());
                    return nullptr;
                }
                break;
        }
    }

    return &tok;
}
