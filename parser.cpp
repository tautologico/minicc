//
// Created by Andrei Formiga on 2/7/17.
//

#include <cstdlib>
#include "lexer.h"
#include "parser.h"

static Token *tok;

void syntaxError(std::string msg) {
    fprintf(stderr, "Erro de sintaxe: %s\n", msg.c_str());
    exit(2);
}

void matchTokenType(TokType type) {
    if (tok->type != type) {
        printToken(type, 0, "");
        syntaxError("Token inesperado");
    }

    tok = getNextToken();
}

/// Verifica se o token atual eh o esperado, e obtem o proximo
void matchToken(TokType type, int val, std::string nome) {
    if (tok->type != type or tok->val != val or tok->nome != nome) {
        printToken(type, val, nome);
        syntaxError("Token inesperado");
    }

    tok = getNextToken();
}

std::vector<std::string> parseListaArgumentos() {
    matchToken(TokType::PalavraChave, Int, "");
    std::string nome = tok->nome;
    matchTokenType(TokType::Identificador);

    std::vector<std::string> result;
    result.push_back(nome);

    while (not (tok->type == TokType::Pontuacao and tok->val == RParen)) {
        matchToken(TokType::Pontuacao, Comma, "");
        matchToken(TokType::PalavraChave, Int, "");
        nome = tok->nome;
        matchTokenType(TokType::Identificador);
        result.push_back(nome);
    }

    return result;
}

std::vector<Comando*> parseComandos();
Exp* parseExpressao();

Comando* parseComando() {
    if (tok->type == TokType::Pontuacao and tok->val == LBrace) {
        std::vector<Comando*> bloco = parseComandos();
        BlocoComandos *b = new BlocoComandos(bloco);
        return b;
    } else if (tok->type == TokType::PalavraChave and tok->val == Printf) {
        matchToken(TokType::Pontuacao, LParen, "");
        Exp *e = parseExpressao();
        matchToken(TokType::Pontuacao, RParen, "");
        matchToken(TokType::Pontuacao, Semicolon, "");
        PrintfCom *p = new PrintfCom(e);
        return p;
    }

    syntaxError("comando esperado");
    return nullptr;
}

std::vector<Comando*> parseComandos() {
    std::vector<Comando*> result;

    while (not ((tok->type == TokType::Pontuacao and tok->val == RBrace) or
                (tok->type == TokType::PalavraChave and tok->val == Return))) {
        Comando *c = parseComando();
        result.push_back(c);
    }

    return result;
}

std::vector<Exp*> parseListaExpressoes() {
    std::vector<Exp*> result;

    // lista vazia
    if (tok->type == TokType::Pontuacao and tok->val == RParen)
        return result;

    Exp *e1 = parseExpressao();
    result.push_back(e1);

    while (not (tok->type == TokType::Pontuacao and tok->val == RParen)) {
        matchToken(TokType::Pontuacao, Comma, "");
        e1 = parseExpressao();
        result.push_back(e1);
    }

    return result;
}

void initParse() {
    tok = getNextToken();
}

Exp* parseAtom() {
    if (tok->type == TokType::Identificador) {   // variavel ou chamada
        std::string nome = tok->nome;
        tok = getNextToken();

        if (tok->type == TokType::Pontuacao and tok->val == LParen) { // chamada
            tok = getNextToken();
            std::vector<Exp*> params = parseListaExpressoes();
            Chamada *c = new Chamada(nome, params);
            return c;
        }

        // variavel
        VarExp *v = new VarExp(nome);
        return v;
    } else if (tok->type == TokType::LiteralNum) {
        LiteralExp *l = new LiteralExp(tok->val);
        tok = getNextToken();  // consume o literal
        return l;
    }

    syntaxError("Expressao atomica esperada");
    return nullptr;
}

Exp* parseAditiva() {
    Exp* atom = parseAtom();

    if (tok->type == TokType::Operador and tok->val == OpSoma) {
        tok = getNextToken();
        Exp *e2 = parseExpressao();
        ExpBin *res = new ExpBin(OpSoma, atom, e2);
        return res;
    }

    return atom;
}

Exp* parseExpressao() {
    return parseAtom(); // TODO alterar para implementacao real
}

Funcao* parseFuncao() {
    matchToken(TokType::PalavraChave, Int, "");

    std::string nome = tok->nome;
    matchTokenType(TokType::Identificador);

    matchToken(TokType::Pontuacao, LParen, "");
    std::vector<std::string> args = parseListaArgumentos();
    matchToken(TokType::Pontuacao, RParen, "");

    matchToken(TokType::Pontuacao, LBrace, "");
    std::vector<Comando*> comandos = parseComandos();
    matchToken(TokType::PalavraChave, Return, "");
    Exp* e = parseExpressao();
    matchToken(TokType::Pontuacao, Semicolon, "");
    matchToken(TokType::Pontuacao, RBrace, "");

    Funcao *f = new Funcao();
    f->nome = nome;
    f->argumentos = args;
    f->comandos = comandos;
    f->expRetorno = e;

    return f;
}

Programa* parsePrograma() {

    tok = getNextToken();

    Programa *p = new Programa();

    while (tok->type != TokType::Eof) {
        Funcao *f = parseFuncao();
        p->adicionaFuncao(f);
    }

    return p;
}
