//
// Created by Andrei Formiga on 2/3/17.
//

#include <string>
#include <cstdlib>
#include "lexer.h"

/// O token atual
Token tok;

/// O arquivo/stream de onde o lexer le caracteres
FILE *stream;

/// buffer de caracteres para o lexer
int buffer;
bool bufferValid = false;

int getNextChar() {
    if (!bufferValid)
        buffer = getc(stream);
    else
        bufferValid = false;
    return buffer;
}

void returnChar(int c) {
    buffer = c;
    bufferValid = true;
}

/// Inicializa o lexer
void initLexer(FILE *f) {
    bufferValid = false;
    stream = f;
}


std::string palavrasChave[] = { "int", "printf", "printint", "return"};

int identificaPalavraChave(std::string &s) {
    for (int i = 0; i < 4; ++i) {
        if (s == palavrasChave[i])
            return i;
    }

    return -1;
}

void lexerError(const char *err) {
    fprintf(stderr, "Erro lexico: %s\n", err);
    exit(1);
}

const char* printTokenType(TokType &type) {
    switch (type) {
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

    // valores iniciais
    tok.nome = "";
    tok.val = 0;

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

            case EOF:
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

                    int ix = identificaPalavraChave(s);
                    if (ix == -1) {
                        tok.type = TokType::Identificador;
                        tok.nome = s;
                    } else {
                        tok.type = TokType::PalavraChave;
                        tok.val = ix;
                    }
                    done = true;
                } else {
                    std::string error = "Caractere nao esperado: ";
                    error += static_cast<char>(c);
                    lexerError(error.c_str());
                    return nullptr;
                }
                break;
        }
    }

    return &tok;
}


//--- Testes do analisador lexico --------------------

int errors = 0;

void printToken(Token *t) {
    fprintf(stderr, "tipo: %s | val = %d | nome = %s", printTokenType(t->type), t->val, t->nome.c_str());
}

void printToken(TokType typ, int val, std::string nome) {
    fprintf(stderr, "tipo: %s | val = %d | nome = %s", printTokenType(typ), val, nome.c_str());
}


static void testToken(Token *t1, TokType typ, int val, std::string nome) {
    if (t1->type == typ and t1->nome == nome and t1->val == val) {
        return;
    } else {
        fprintf(stderr, "Erro no teste: \n");
        fprintf(stderr, "Esperado: ");
        printToken(t1);
        fprintf(stderr, "\n");
        fprintf(stderr, "Analise : ");
        printToken(typ, val, nome);
        fprintf(stderr, "\n");
        errors++;
    }
}

static void test1() {
    FILE *f = fopen("print.mc", "r");
    initLexer(f);

    testToken(getNextToken(), TokType::PalavraChave, Int, "");
    testToken(getNextToken(), TokType::Identificador, 0, "main");
    testToken(getNextToken(), TokType::Pontuacao, LParen, "");
    testToken(getNextToken(), TokType::Pontuacao, RParen, "");
    testToken(getNextToken(), TokType::Pontuacao, LBrace, "");
    testToken(getNextToken(), TokType::PalavraChave, Printint, "");
    testToken(getNextToken(), TokType::Pontuacao, LParen, "");
    testToken(getNextToken(), TokType::LiteralNum, 4712, "");
    testToken(getNextToken(), TokType::Pontuacao, RParen, "");
    testToken(getNextToken(), TokType::Pontuacao, Semicolon, "");
    testToken(getNextToken(), TokType::Pontuacao, RBrace, "");
    testToken(getNextToken(), TokType::Eof, 0, "");

    fclose(f);
}

static void test2() {
    FILE *f = fopen("cond.mc", "r");
    initLexer(f);

    testToken(getNextToken(), TokType::PalavraChave, Int, "");
    testToken(getNextToken(), TokType::Identificador, 0, "main");
    testToken(getNextToken(), TokType::Pontuacao, LParen, "");
    testToken(getNextToken(), TokType::Pontuacao, RParen, "");
    testToken(getNextToken(), TokType::Pontuacao, LBrace, "");
    testToken(getNextToken(), TokType::Identificador, 0, "if");
    testToken(getNextToken(), TokType::Pontuacao, LParen, "");
    testToken(getNextToken(), TokType::Identificador, 0, "x");
    testToken(getNextToken(), TokType::Operador, OpLt, "");
    testToken(getNextToken(), TokType::LiteralNum, 0, "");
    testToken(getNextToken(), TokType::Pontuacao, RParen, "");
    testToken(getNextToken(), TokType::Identificador, 0, "x");
    testToken(getNextToken(), TokType::Operador, OpAssign, "");
    testToken(getNextToken(), TokType::LiteralNum, 47, "");
    testToken(getNextToken(), TokType::Operador, OpSoma, "");
    testToken(getNextToken(), TokType::LiteralNum, 3, "");
    testToken(getNextToken(), TokType::Operador, OpMult, "");
    testToken(getNextToken(), TokType::LiteralNum, 2, "");
    testToken(getNextToken(), TokType::Pontuacao, Semicolon, "");
    testToken(getNextToken(), TokType::Identificador, 0, "else");
    testToken(getNextToken(), TokType::Identificador, 0, "y");
    testToken(getNextToken(), TokType::Operador, OpAssign, "");
    testToken(getNextToken(), TokType::LiteralNum, 8, "");
    testToken(getNextToken(), TokType::Pontuacao, Semicolon, "");
    testToken(getNextToken(), TokType::PalavraChave, Return, "");
    testToken(getNextToken(), TokType::LiteralNum, 0, "");
    testToken(getNextToken(), TokType::Pontuacao, Semicolon, "");
    testToken(getNextToken(), TokType::Pontuacao, RBrace, "");
    testToken(getNextToken(), TokType::Eof, 0, "");

    fclose(f);
}

void testLexer() {
    errors = 0;

    fprintf(stderr, "Teste 1...\n");
    test1();
    fprintf(stderr, "Teste 2...\n");
    test2();

    if (errors == 0)
        fprintf(stderr, "Todos os testes corretos\n");
    else
        fprintf(stderr, "Foram encontrados %d erros\n", errors);
}
