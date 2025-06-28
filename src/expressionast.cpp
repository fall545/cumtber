#include "tokenizer.h"
#include "ast.h"
#include <stack>
#include "parser.h"
#include <iostream>
typedef int Status;
enum {
    ERROR,
    OK
};

extern std::unique_ptr<ExprAST> ParseIfExpr();
extern std::unique_ptr<ExprAST> ParseWhileExpr();

char Precede(const char& a, const char& b) {
    if (a == '#' && b != '#') return '<';
    else if (a != '#' && b == '#') return '>';
    else if (a == '#' && b == '#') return '=';

    // Special case for parentheses
    if (a == '(' && b == ')') return '=';
    if (a == '(') return '<';
    if (b == '(') return '<';
    if (b == ')') return '>';

    // Compare precedence using BinopPrecedence map
    int precA = BinopPrecedence.count(a) ? BinopPrecedence[a] : -1;
    int precB = BinopPrecedence.count(b) ? BinopPrecedence[b] : -1;

    if (precA > precB) return '>';
    else if (precA < precB) return '<';
    else return '='; // Equal precedence
}
std::vector<int> funcFlag = {0};
std::unique_ptr<ExprAST> ParseExpression() {
    std::stack<std::unique_ptr<ExprAST>> operators;
    std::stack<std::unique_ptr<ExprAST>> operands;
    int currentToken;
    if(funcFlag.back()) {
        currentToken = CurTok;
    } else {
        currentToken = getNextToken();
    }
    while (currentToken != ';') { 
        if (funcFlag.back() && currentToken == ',') {
            funcFlag.push_back(0);
            break;
        }
        if (funcFlag.back() && currentToken == ')') {
            break;
        }
        if (currentToken == tok_identifier) {
            // 检查标识符后是否跟随 '('
            std::string callee = IdentifierStr;
            currentToken = getNextToken(); // 消耗当前 token  (
            if (currentToken == '(') {
                // 函数调用
                funcFlag.back() ++;
                std::vector<std::unique_ptr<ExprAST>> args;
                currentToken = getNextToken(); // 消耗 '('   1
                while (currentToken != ')') {
                    if (currentToken == tok_number || currentToken == tok_identifier) {
                        auto nestedExpr = ParseExpression();
                        currentToken = CurTok;
                        if (!nestedExpr) {
                            syntaxerror(std::string("function call error,Failed to parse nested function call ") + std::to_string(funcFlag.back()));
                            return nullptr;
                        }
                        args.push_back(std::move(nestedExpr));
                    } else if (currentToken == ')') {
                        break;
                    } else {
                        syntaxerror(std::string("function call error,Unexpected token in function call ") + std::to_string(funcFlag.back()));
                        return nullptr;
                    }
                    // currentToken = getNextToken(); // 消耗当前 token
                }
                // 将函数调用节点压入操作数栈
                operands.push(std::make_unique<CallExprAST>(callee, std::move(args)));
                std::cout << (char) currentToken;
                currentToken = getNextToken(); // 消耗 ')'
            } else {
                // 普通标识符
                operands.push(std::make_unique<VariableExprAST>(callee));
            }
        } else if (currentToken == tok_number) {
            // 数字节点
            operands.push(std::make_unique<NumberExprAST>(NumVal));
            currentToken = getNextToken(); // 消耗当前 token
            // std::cout << (char)currentToken;
            // std::cout << funcFlag;
        } else if (currentToken == '(') {
            operators.push(std::make_unique<VariableExprAST>("("));
            currentToken = getNextToken(); // 消耗 '('
        } else if (currentToken == ')') {
            while (!operators.empty() && dynamic_cast<VariableExprAST*>(operators.top().get())->getName() != "(") {
                auto op = std::move(operators.top());
                operators.pop();

                auto rhs = std::move(operands.top());
                operands.pop();

                auto lhs = std::move(operands.top());
                operands.pop();

                operands.push(std::make_unique<BinaryExprAST>(
                    dynamic_cast<VariableExprAST*>(op.get())->getName()[0],
                    std::move(lhs),
                    std::move(rhs)
                ));
            }
            if (!operators.empty()) {
                operators.pop(); // 弹出 '('
                currentToken = getNextToken(); // 消耗 ')'
            } else {
                // std::cout << (char)currentToken;
                currentToken = getNextToken();
                // std::cout << (char)currentToken;
            }
        } else {
            // 处理操作符
            while (!operators.empty() && Precede(dynamic_cast<VariableExprAST*>(operators.top().get())->getName()[0], currentToken) == '>') {
                auto op = std::move(operators.top());
                operators.pop();

                auto rhs = std::move(operands.top());
                operands.pop();

                auto lhs = std::move(operands.top());
                operands.pop();

                operands.push(std::make_unique<BinaryExprAST>(
                    dynamic_cast<VariableExprAST*>(op.get())->getName()[0],
                    std::move(lhs),
                    std::move(rhs)
                ));
            }
            operators.push(std::make_unique<VariableExprAST>(std::string(1, currentToken)));
            currentToken = getNextToken(); // 消耗操作符
        }
    }
    if (operators.empty() && operands.size() == 1) {
        // std::cout << "123";
        auto it = funcFlag.end();
        *(it - 2) --; 
        return std::move(operands.top());
    }

    while (!operators.empty()) {
        auto op = std::move(operators.top());
        operators.pop();

        auto rhs = std::move(operands.top());
        operands.pop();

        auto lhs = std::move(operands.top());
        operands.pop();

        operands.push(std::make_unique<BinaryExprAST>(
            dynamic_cast<VariableExprAST*>(op.get())->getName()[0],
            std::move(lhs),
            std::move(rhs)
        ));
    }

    // 返回最终的 AST 根节点
    if (!operands.empty()) {
        funcFlag.back() --;
        return std::move(operands.top());
    }
    syntaxerror(std::string("expression error,Failed to parse expression"));
    return nullptr; // Return nullptr to indicate failure
}

std::string NumberExprAST::codegen() {
    return "Number: " + std::to_string(Val);
}

std::string VariableExprAST::codegen() {
    return "Variable: " + Name;
}

std::string BinaryExprAST::codegen() {
    std::string result = "Binary Operation: " + std::string(1, Op) + "\n";
    result += "Left-hand side: " + LHS->codegen() + "\n";
    result += "Right-hand side: " + RHS->codegen() + "\n";
    return result;
}

std::string CallExprAST::codegen() {
    std::string result = "Function Call: " + Callee + "\nArguments: ";
    for (const auto& arg : Args) {
        result += arg->codegen() + ", ";
    }
    if (!Args.empty()) {
        result.pop_back(); // Remove trailing comma
        result.pop_back(); // Remove trailing space
    }
    return result;
}

std::string ExprAST::codegen() {
    return "";
}
