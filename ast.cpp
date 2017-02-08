//
// Created by Andrei Formiga on 2/7/17.
//

#include <sstream>
#include "ast.h"


std::string VarExp::toString(int ident) {
    std::ostringstream ss;
    ss << "Var(" << this->nome << ")";
    return ss.str();
}

std::string LiteralExp::toString(int ident) {
    std::ostringstream ss;
    ss << "Lit(" << this->valor << ")";
    return ss.str();
}

std::string Chamada::toString(int ident) {
    std::ostringstream ss;
    ss << "Cham(" << this->nome << std::endl;
    for (auto& arg : this->parametros) {
        ss << arg->toString(ident + 2) << std::endl;
    }
    ss << ")";
    return ss.str();
}

std::string opString(Op op) {
    switch (op) {
        case OpSoma:
            return "+";
        case OpMult:
            return "*";
        case OpLt:
            return "<";
        case OpAssign:
            return "=";
        case OpEq:
            return "==";
    }

    return "";
}

std::string ExpBin::toString(int ident) {
    std::ostringstream ss;
    ss << opString(this->op) << "(";
    ss << this->e1->toString(ident + 2) << std::endl;
    ss << this->e2->toString(ident + 2) << std::endl;
    ss << ")";
    return ss.str();
}
