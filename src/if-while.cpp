#include "../include/ast.h"
#include "../include/tokenizer.h"
#include "../include/parser.h"
#include <iostream>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>

// 补充static声明，保证与头文件一致，解决链接问题
typedef std::unique_ptr<ExprAST> ExprASTPtr;
ExprASTPtr ParseExpression();
ExprASTPtr ParseIfExpr();
ExprASTPtr ParseWhileExpr();

// ========== ExprAST vtable/typeinfo fix ==========
void ExprAST::codegen() {}


// ========== ast.cpp 部分 ==========
void NumberExprAST::codegen() {
    std::cout << "NumberExprAST::codegen() - Value: " << getValue() << std::endl;
}
void VariableExprAST::codegen() {
    std::cout << "VariableExprAST::codegen() - Name: " << getName() << std::endl;
}
void CallExprAST::codegen() {
    std::cout << "CallExprAST::codegen() - Function: " << getCallee() << std::endl;
    for (const auto& arg : getArgs()) {
        if (arg) {
            arg->codegen();
        }
    }
}
void IfExprAST::codegen() {
    std::cout << "IfExprAST::codegen()" << std::endl;
}
void WhileExprAST::codegen() {
    std::cout << "WhileExprAST::codegen()" << std::endl;
}
void PrototypeAST::codegen() {
    std::cout << "PrototypeAST::codegen() - Function: " << Name << std::endl;
}
void FunctionAST::codegen() {
    std::cout << "FunctionAST::codegen()" << std::endl;
    if (Proto) {
        Proto->codegen();
    }
    if (Body) {
        Body->codegen();
    }
}

// ========== loopast.cpp (parser) 部分 ==========
std::unique_ptr<ExprAST> LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}
std::unique_ptr<ExprAST> ParseNumberExpr() {
    auto Result = std::unique_ptr<NumberExprAST>(new NumberExprAST(NumVal));
    getNextToken();
    return std::move(Result);
}
std::unique_ptr<ExprAST> ParseParenExpr() {
    getNextToken();
    auto V = ParseExpression();
    if (!V)
        return nullptr;
    if (CurTok != ')')
        return LogError("expected ')'");
    getNextToken();
    return V;
}
std::unique_ptr<ExprAST> ParseIdentifierExpr() {
    std::string IdName = IdentifierStr;
    getNextToken();
    return std::unique_ptr<VariableExprAST>(new VariableExprAST(IdName));
}
std::unique_ptr<ExprAST> ParseIfExpr() {
    getNextToken();
    auto Cond = ParseExpression();
    if (!Cond) return nullptr;
    auto Then = ParseExpression();
    if (!Then) return nullptr;
    if (CurTok != tok_else) return LogError("expected 'else'");
    getNextToken();
    auto Else = ParseExpression();
    if (!Else) return nullptr;
    return std::unique_ptr<IfExprAST>(new IfExprAST(std::move(Cond), std::move(Then), std::move(Else)));
}
std::unique_ptr<ExprAST> ParseBlockExpr() {
    if (CurTok != '{')
        return LogError("expected '{' at start of block");
    getNextToken();
    auto Body = ParseExpression();
    if (!Body) return nullptr;
    if (CurTok == ';')
        getNextToken();
    if (CurTok != '}')
        return LogError("expected '}' at end of block");
    getNextToken();
    return Body;
}
std::unique_ptr<ExprAST> ParseWhileExpr() {
    getNextToken();
    auto Cond = ParseExpression();
    if (!Cond) return nullptr;
    auto Body = ParseBlockExpr();
    if (!Body) return nullptr;
    return std::unique_ptr<WhileExprAST>(new WhileExprAST(std::move(Cond), std::move(Body)));
}
std::unique_ptr<ExprAST> ParsePrimary() {
    switch (CurTok) {
    case tok_identifier:
        return ParseIdentifierExpr();
    case tok_number:
        return ParseNumberExpr();
    case '(': 
        return ParseParenExpr();
    case tok_if:
        return ParseIfExpr();
    case tok_while:
        return ParseWhileExpr();
    default:
        return LogError("unknown token when expecting an expression");
    }
}

// 保留static版本在文件末尾
static std::unique_ptr<FunctionAST> ParseDefinition() { return nullptr; }
static std::unique_ptr<ExprAST> ParseExpression() { return ParsePrimary(); }