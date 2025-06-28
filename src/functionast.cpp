#include "tokenizer.h"
#include "parser.h"

//变量为tok_identifier,解析Args
static std::vector<std::string> ParseArgs() {
    std::vector<std::string> Args;
    while (CurTok == IdentifierStr) {
        Args.push_back(IdentifierStr);
        getNextToken();
        if (CurTok != ',') break;
        getNextToken();
    }
    return Args;
}

//parse FuncBody
static std::unique_ptr<BlockExprAST> ParseBody() {
    if (CurTok != '{') {
        syntaxerror(string(function body error , missing "{"));
        return nullptr;
    }
    getNextToken();    
    std::unique_ptr<ExprAST> Stmts;
    if (CurTok != '}') {
        Stmts = ParseExpression();
    }else {
        syntaxerror(string(function body error , missing "}"));
        return nullptr;
    }
    getNextToken(); 
    return std::make_unique<ExprAST>(std::move(Stmts));
}

static std::unique_ptr<PrototypeAST> ParsePrototype(){
    if (CurTok != tok_def) {
        syntaxerror(string(function definition error , missing "def"));
        return nullptr;
    }
    getNextToken();
    if (CurTok != tok_identifier) {
        syntaxerror(string(function name error , using identifier));
        return nullptr;
    }
    std::string FnName = IdentifierStr;
    getNextToken();
    auto Args = ParseArgs();
    if (CurTok == ';') {
        getNextToken(); //def f a,b;
    }else{
        syntaxerror(string(function prototype error , not using ";"));
        return nullptr;
    }
    return std::make_unique<PrototypeAST>(FnName, move(Args));
}

static std::unique_ptr<FunctionAST> ParseDefinition(){
    if (CurTok != tok_def) {
        syntaxerror(string(function definition error , missing "def"));
        return nullptr;
    }
    getNextToken();
    if (CurTok != tok_identifier) {
        syntaxerror(string(function name error , using identifier));
        return nullptr;
    }
    std::string FnName = IdentifierStr;
    getNextToken();
    auto Args = ParseArgs();
    std::unique_ptr<BlockExprAST> Body = nullptr;
    if (CurTok == '{') {
        Body = ParseBody(); //deg f a,b {}
    } else {
        syntaxerror(string(function definition error , missing "{"))
        return nullptr;
    }
    auto Proto = std::make_unique<PrototypeAST>(FnName, std::move(Args));
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(Body));
}

std::string PrototypeAST::codegen() {
    std::string info = "This is Func" + Name + "with Args:";
    for (size_t i = 0; i < Args.size(); ++i) {
        if (i > 0) {
            info += ", ";
        }
        info += Args[i];
    }
    info +="\n";
    return info;
}

std::string FunctionAST::codegen() {
    std::string info = Proto->codegen() + " {\n";
    if (Body){
            info += "    " + Body->codegen();
            info += "\n";
        }
    info += "}\n";
    return info;
}
