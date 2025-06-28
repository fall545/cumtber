#include "tokenizer.h"
#include "parser.h"
extern std::string IdentifierStr;
extern int CurTok;
extern std::unique_ptr<ExprAST> ParseExpression();
extern std::unique_ptr<ExprAST> ParseIfExpr() ;
extern std::unique_ptr<ExprAST> ParseWhileExpr() ;
//变量为tok_identifier,解析Args
std::vector<std::string> ParseArgs() {
    std::vector<std::string> Args;
    while (CurTok == tok_identifier) {
        Args.push_back(IdentifierStr);
        getNextToken();
        if (CurTok != ',') break;
        getNextToken();
    }
    return Args;
}

//parse FuncBody
std::unique_ptr<ExprAST> ParseBody() {
    std::unique_ptr<ExprAST> Stmt;
    if (CurTok != '{') {
        syntaxerror(std::string("function definition error , missing \"{\""));
        return nullptr;
    }
    getNextToken();
    if (CurTok != '}') {
        if (CurTok == tok_identifier && IdentifierStr == "if") {
            Stmt = ParseIfExpr();
        } else if (CurTok == tok_identifier && IdentifierStr == "while") {
            Stmt = ParseWhileExpr();
        } else {
            Stmt = ParseExpression();
        }
    }
    getNextToken(); // 跳过}
    return Stmt;
}

std::unique_ptr<PrototypeAST> ParsePrototype(){
    if (CurTok != tok_def) {
        syntaxerror(std::string("function definition error , missing \"def\""));
        return nullptr;
    }
    getNextToken();
    if (CurTok != tok_identifier) {
        syntaxerror(std::string("function name error , using identifier"));
        return nullptr;
    }
    std::string FnName = IdentifierStr;
    getNextToken();
    auto Args = ParseArgs();
    if (CurTok == ';') {
        getNextToken(); //def f a,b;
    }else{
        syntaxerror(std::string("function prototype error , not using \";\""));
        return nullptr;
    }
    auto Proto = std::make_unique<PrototypeAST>(FnName, std::move(Args));
    return Proto;
}

std::unique_ptr<FunctionAST> ParseDefinition(){
    if (CurTok != tok_def) {
        syntaxerror(std::string("function definition error , missing \"def\""));
        return nullptr;
    }
    getNextToken();
    if (CurTok != tok_identifier) {
        syntaxerror(std::string("function name error , using identifier"));
        return nullptr;
    }
    std::string FnName = IdentifierStr;
    getNextToken();
    auto Args = ParseArgs();
    std::unique_ptr<ExprAST> Body;
    if (CurTok == '{') {
        Body = ParseBody(); //deg f a,b {}
    } else {
        syntaxerror(std::string("function definition error , missing \"{\""));
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
    info += "    " + Body->codegen();
    if (!dynamic_cast<IfExprAST*>(Body.get()) && 
                    !dynamic_cast<WhileExprAST*>(Body.get())) {
                    info += ";";
                }
    info += "\n";
    return info;
}
