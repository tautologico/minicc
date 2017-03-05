//
// Created by Andrei Formiga on 2/7/17.
//

#include <cstdlib>
#include <map>
#include <algorithm>
#include "lexer.h"
#include "parser.h"

static Token *tok;

static std::map<Op, int> precMap = { {OpLt, 10}, {OpEq, 10}, {OpSoma, 20}, {OpMult, 30} };

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
    std::vector<std::string> result;

    if (tok->type == TokType::Pontuacao and tok->val == RParen)
        return result;

    matchToken(TokType::PalavraChave, Int, "");
    std::string nome = tok->nome;
    matchTokenType(TokType::Identificador);

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
        tok = getNextToken();
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


// analise de expressoes
Exp* parseAtom() {
    if (tok->type == TokType::Identificador) {   // variavel ou chamada
        std::string nome = tok->nome;
        tok = getNextToken();

        if (tok->type == TokType::Pontuacao and tok->val == LParen) { // chamada
            tok = getNextToken();
            std::vector<Exp*> params = parseListaExpressoes();
            Chamada *c = new Chamada(nome, params);
            tok = getNextToken();
            return c;
        }

        // variavel
        VarExp *v = new VarExp(nome);
        return v;
    } else if (tok->type == TokType::LiteralNum) {
        LiteralExp *l = new LiteralExp(tok->val);
        tok = getNextToken();  // consome o literal
        return l;
    }

    syntaxError("Expressao atomica esperada");
    return nullptr;
}

ExpBin* searchExpBin(ExpBin* expBin) {
    ExpBin* exp1 = dynamic_cast<ExpBin*>(expBin->e1);
    if (exp1==nullptr)
        return expBin;
    else
        searchExpBin(exp1);
}

Exp* parseBinOp() {
    Exp* opEsq = parseAtom();

    if (tok->type == TokType::Operador) {
        Op op1 = static_cast<Op>(tok->val);
        tok = getNextToken();
        Exp* opDir = parseExpressao();

        ExpBin* expBin = dynamic_cast<ExpBin*>(opDir);
        if (expBin != nullptr) {
            ExpBin* exp1 = searchExpBin(expBin);
            // verifica se operador do resto tem precedencia maior
            if (precMap[op1] >= precMap[exp1->op]) { // >= assume associatividade a esquerda
                ExpBin *e1 = new ExpBin(op1, opEsq, exp1->e1);
                exp1->e1 = e1;
            } else {
                ExpBin* cpExp = new ExpBin(exp1->op, exp1->e1, exp1->e2);
                exp1->op = op1;
                exp1->e1 = opEsq;
                exp1->e2 = cpExp;
            }
            return expBin;
        } else {   // operando direito nao era expressao binaria
            ExpBin *res = new ExpBin(op1, opEsq, opDir);
            return res;
        }
    }
    return opEsq;
}

Exp* parseExpressao() {
    return parseBinOp();
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


// --- Testes -----------------------------------
bool assertExp(const std::string& sexp1, std::string sexp2){
    int len = sexp1.length();
    int index = 0;
    // removendo espaço e quebra de linha
    sexp2.erase(std::remove(sexp2.begin(), sexp2.end(), ' '), sexp2.end());
    sexp2.erase(std::remove(sexp2.begin(), sexp2.end(), '\n'), sexp2.end());

    if(sexp2==sexp1)
      return true;
    fprintf(stderr, "\nexpressao esperada =>\t %s", sexp1.c_str());

    for(int i = 0 ; i < len ; ++i){
      if(sexp1[i]!=sexp2[i]){
        index = i;
        break;
      }
    }
    fprintf(stderr, "\nbreakpoint => \t\t %s\n", sexp2.substr(0, index).c_str());
    fprintf(stderr, "expressao econtrada =>\t %s\n", sexp2.c_str());
    return false;
}

void test_exp(const char* file, const char* exp) {
    FILE *f = fopen(file, "r");
    initLexer(f);
    initParse();
    Exp* e = parseExpressao();

    fprintf(stderr, "arquivo: %s ....... \n", file);

    if(assertExp(exp, e->toString()))
      fprintf(stderr, "........................ ok\n");
    else
      fprintf(stderr, "\n........................ fail\n");
}

void testParseExp() {
    fprintf(stderr, "Testando analise de expressoes\n\n");

    test_exp("exp1.mc", "+(*(Lit(33)Var(xy))Var(z))");
    test_exp("exp2.mc", "+(Lit(33)*(Var(xy)Var(z)))");
    test_exp("exp3.mc", "=(Var(y)+(Var(x)*(Lit(3)Cham(funcVar(x)))))");
    test_exp("exp4.mc", "=(Var(a)Cham(funcCham(f*(Lit(3)Var(x)))))");
    test_exp("exp5.mc", "+(+(Var(x)*(Lit(3)Lit(3)))Var(a))");
    test_exp("exp6.mc", "=(Var(a)+(Cham(funcVar(x))Lit(3)))");
    test_exp("exp7.mc", "=(Var(a)+(Lit(3)Cham(funcVar(x))))");
    test_exp("exp8.mc", "<(Cham(funcVar(x))+(Cham(funcVar(y))Lit(3)))");
    test_exp("exp9.mc", "+(+(*(Var(y)Var(x))Lit(3))Lit(10))");
    test_exp("exp10.mc", "+(+(*(Var(x)Lit(2))Lit(2))*(Lit(5)Lit(4)))");
    test_exp("exp11.mc", "+(+(+(+(Lit(9)Lit(8))Lit(7))Lit(6))*(Lit(5)Var(x)))");
}
