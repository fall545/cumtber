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

//parse FuncBody
static std::unique_ptr<BlockExprAST> ParseBody() {
    if (CurTok != '{') return nullptr;
    getNextToken();    
    std::vector<std::unique_ptr<ExprAST>> Stmts;
    while (CurTok != '}') {
        std::unique_ptr<ExprAST> Stmt;
        
        if (CurTok == tok_if) {
            Stmt = ParseIfExpr();
        } else if (CurTok == tok_while) {
            Stmt = ParseWhileExpr();
        } else {
            Stmt = ParseExpression();
        }

        if (!Stmt) return nullptr;
        
        Stmts.push_back(std::move(Stmt));
    }
    getNextToken(); 
    return std::make_unique<BlockExprAST>(std::move(Stmts));
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
    std::unique_ptr<BlockExprAST> Body = nullptr;
    if (CurTok == '{') {
        Body = ParseBody(); //deg f a,b {}
    } else {
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
    if (Body) {
        if (auto* block = dynamic_cast<BlockExprAST*>(Body.get())) {
            for (auto& stmt : block->getStmts()) {
                info += "    " + stmt->codegen();
                if (!dynamic_cast<IfExprAST*>(stmt.get()) && 
                    !dynamic_cast<WhileExprAST*>(stmt.get())) {
                    info += ";";
                }
                info += "\n";
            }
        }
    }
    info += "}\n";
    return info;
}