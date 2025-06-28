#ifndef PARSER_H
#define PARSER_H
#include <ast.h>
//如果解析失败就return nullptr;
/// definition ::= 'def' prototype expression

//TODO:恺哥
// static std::unique_ptr<FunctionAST> ParseDefinition();
// static std::unique_ptr<PrototypeAST> ParsePrototype();

// //TODO 振宇
extern std::unique_ptr<ExprAST> ParseExpression();

// // TODO 玥鑫
// static std::unique_ptr<ExprAST> ParseIfExpr();
// static std::unique_ptr<ExprAST> ParseWhileExpr();


//把顶层表达式用匿名表函数封装
/// toplevelexpr ::= expression



#endif