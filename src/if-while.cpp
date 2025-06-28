#include <vector>
#include "../include/ast.h"
#include "../include/tokenizer.h"
#include "../include/parser.h"
#include <iostream>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <string>

// ========== 简化的AST节点实现 ==========
std::string IfExprAST::codegen() {
    std::cout << "IfExprAST::codegen()" << std::endl;
    if (Cond && Then && Else) {
        std::string condResult = Cond->codegen();
        std::string thenResult = Then->codegen();
        std::string elseResult = Else->codegen();
        return "if(" + condResult + "){" + thenResult + "}else{" + elseResult + "}";
    }
    return "";
}

std::string WhileExprAST::codegen() {
    std::cout << "WhileExprAST::codegen()" << std::endl;
    if (Cond && Body) {
        std::string condResult = Cond->codegen();
        std::string bodyResult = Body->codegen();
        return "while(" + condResult + "){" + bodyResult + "}";
    }
    return "";
}

std::string BlockExprAST::codegen() {
    std::string result;
    for (const auto& stmt : getStmts()) {
        if (stmt) result += stmt->codegen() + ";\n";
    }
    return result;
}

// ParseTopLevelExpr实现
std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
    if (auto E = ParseExpression()) {
        std::unique_ptr<PrototypeAST> Proto(new PrototypeAST("__anon_expr", std::vector<std::string>()));
        std::vector<std::unique_ptr<ExprAST> > stmts;
        stmts.push_back(std::move(E));
        std::unique_ptr<BlockExprAST> Body(new BlockExprAST(std::move(stmts)));
        return std::unique_ptr<FunctionAST>(new FunctionAST(std::move(Proto), std::move(Body)));
    }
    return nullptr;
}

// ========== 简化的解析器 ==========
// 简化的数字解析 - 只支持简单数字
std::unique_ptr<ExprAST> ParseNumberExpr() {
    auto Result = std::unique_ptr<NumberExprAST>(new NumberExprAST(NumVal));
    getNextToken();
    return std::move(Result);
}

// 简化的变量解析 - 只支持简单变量名
std::unique_ptr<ExprAST> ParseIdentifierExpr() {
    std::string IdName = IdentifierStr;
    getNextToken();
    return std::unique_ptr<VariableExprAST>(new VariableExprAST(IdName));
}

// 简化的主解析函数
std::unique_ptr<ExprAST> ParsePrimary() {
    if (CurTok == tok_identifier) {
        if (IdentifierStr == "if") {
            getNextToken();
            return ParseIfExpr();
        } else if (IdentifierStr == "while") {
            getNextToken();
            return ParseWhileExpr();
        } else if (IdentifierStr == "else") {
            // else一般在if内部处理，这里直接报错
            syntaxerror("unexpected 'else' , 'else' should be inside if statement");
            return nullptr;
        } else {
            // 简单变量名
            return ParseIdentifierExpr();
        }
    } else if (CurTok == tok_number) {
        // 简单数字
        return ParseNumberExpr();
    } else {
        syntaxerror("unknown token , expected identifier or number");
        return nullptr;
    }
}

// 简化的if语句解析
std::unique_ptr<ExprAST> ParseIfExpr() {
    // 进入该函数时，CurTok和IdentifierStr已是"if"，且已getNextToken()
    // 解析条件（单个变量或数字）
    auto Cond = ParsePrimary();
    if (!Cond) return nullptr;
    
    // 解析then分支（单个变量或数字）
    auto Then = ParsePrimary();
    if (!Then) return nullptr;
    
    // 检查else
    if (!(CurTok == tok_identifier && IdentifierStr == "else")) {
        syntaxerror("if statement , missing 'else' keyword");
        return nullptr;
    }
    getNextToken(); // 跳过else
    
    // 解析else分支（单个变量或数字）
    auto Else = ParsePrimary();
    if (!Else) return nullptr;
    
    return std::unique_ptr<IfExprAST>(new IfExprAST(std::move(Cond), std::move(Then), std::move(Else)));
}

// 简化的while循环解析
std::unique_ptr<ExprAST> ParseWhileExpr() {
    // 进入该函数时，CurTok和IdentifierStr已是"while"，且已getNextToken()
    // 解析条件(单个变量或数字）
    auto Cond = ParsePrimary();
    if (!Cond) return nullptr;
    
    // 解析循环体（单个变量或数字）
    auto Body = ParsePrimary();
    if (!Body) return nullptr;
    
    return std::unique_ptr<WhileExprAST>(new WhileExprAST(std::move(Cond), std::move(Body)));
}

// 简化的表达式解析
// std::unique_ptr<ExprAST> ParseExpression() { 
//     return ParsePrimary(); 
// }

// // 简化的函数定义解析（返回空实现）
// std::unique_ptr<FunctionAST> ParseDefinition() { 
//     return nullptr; 
// }



