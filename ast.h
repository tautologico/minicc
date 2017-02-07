//
// Created by Andrei Formiga on 2/7/17.
//

#ifndef MINICC_AST_H
#define MINICC_AST_H

#include <string>
#include <vector>
#include "lexer.h"

class ASTNode {

};

class Programa : public ASTNode {
public:
    Programa() = default;

    void adicionaFuncao(Funcao *f) {  funcoes.push_back(f); }

private:
    std::vector<Funcao*> funcoes;
};


class Funcao : public ASTNode {
public:
    Funcao() = default;

    std::string nome;
    std::vector<Comando*> comandos;
    std::vector<std::string> argumentos;
    Exp* expRetorno;
};

class Comando : public ASTNode {

};

class BlocoComandos : public Comando {
public:
    BlocoComandos() = default;
    BlocoComandos(std::vector<Comando*> comandos) : comandos(comandos) {}

    std::vector<Comando*> comandos;
};

class PrintfCom : public Comando {
public:
    PrintfCom() = default;

    PrintfCom(Exp *e) : e(e) {}

    Exp* e;
};

class Exp : public ASTNode {

};

class VarExp : public Exp {
public:
    VarExp(const std::string &nome) : nome(nome) {}
    std::string nome;
};

class LiteralExp : public Exp {
public:
    LiteralExp::LiteralExp(int valor) : valor(valor) {}
    int valor;
};

class Chamada : public Exp {
public:
    std::string nome;
    std::vector<Exp*> parametros;

    Chamada(const std::string &nome, std::vector<Exp*> params) : nome(nome), parametros(params) {}
};

class ExpBin : public Exp {
public:
    Op op;
    Exp* e1;
    Exp* e2;

    ExpBin(Op op, Exp* e1, Exp* e2) : op(op), e1(e1), e2(e2) { }
};

#endif //MINICC_AST_H
