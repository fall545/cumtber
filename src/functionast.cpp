#include "tokenizer.h"
#include "parser.h"

//变量为tok_identifier,解析Args
static std::vector<std::string> ParseArgs() {
    std::vector<std::string> Args;
    while (CurTok == tok_identifier) {
        Args.push_back(IdentifierStr);
        getNextToken();
        if (CurTok != ',') break;
        getNextToken();
    }
    return Args;
}

//解析函数体
static std::unique_ptr<ExprAST> ParseBody() {
    if (CurTok != '{') {
        return nullptr;
    }
    getNextToken(); //舍掉{
    auto Body = ParseExpression();
    if (!Body) {
        return nullptr;
    }

    if (CurTok != ';') {
        return nullptr;
    }
    getNextToken();
    if (CurTok != '}') {
        return nullptr;
    }
    getNextToken();
    return Body;
}

static std::unique_ptr<PrototypeAST> ParsePrototype(){
    if (CurTok != tok_def) {
        return nullptr;
    }
    getNextToken();
    if (CurTok != tok_identifier) {
        return nullptr;
    }
    std::string FnName = IdentifierStr;
    getNextToken();
    auto Args = ParseArgs();
    if (CurTok == ';') {
        getNextToken(); //def f a,b;
    }else return nullptr;
    return std::make_unique<PrototypeAST>(FnName, move(Args));
}

static std::unique_ptr<FunctionAST> ParseDefinition(){
    if (CurTok != tok_def) {
        return nullptr;
    }
    getNextToken();
    if (CurTok != tok_identifier) {
        return nullptr;
    }
    std::string FnName = IdentifierStr;
    getNextToken();
    auto Args = ParseArgs();
    std::unique_ptr<ExprAST> Body = nullptr;
    if (CurTok == '{') {
        Body = ParseBody(); //deg f a,b {}
    } else {
        return nullptr;
    }
    auto Proto = std::make_unique<PrototypeAST>(FnName, std::move(Args));
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(Body));
}
