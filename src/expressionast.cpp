#include "tokenizer.h"
#include "ast.h"
#include <stack>

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
// 辅助函数：递归解析嵌套函数调用
std::vector<std::unique_ptr<ExprAST>> ParseNestedCall() {
    std::vector<std::unique_ptr<ExprAST>> args;
    int nextFlag;
    while ((nextFlag = getNextToken()) != ')') {
        if (nextFlag == tok_number) {
            args.push_back(std::make_unique<NumberExprAST>(NumVal));
        } else if (nextFlag == tok_identifier) {
            std::string nestedCallee = IdentifierStr;
            int nestedFlag = getNextToken();
            if (nestedFlag == '(') {
                args.push_back(std::make_unique<CallExprAST>(nestedCallee, ParseNestedCall()));
            } else {
                args.push_back(std::make_unique<VariableExprAST>(nestedCallee));
            }
        } else {
            throw std::runtime_error("Unexpected token in nested call");
        }
    }
    return args;
}
Status CreateTree() {
    std::stack<std::unique_ptr<ExprAST>> operators;
    std::stack<std::unique_ptr<ExprAST>> operands;
    int ch_flag;
    int flag = getNextToken();
    if (flag == tok_identifier) {
        // 检查标识符后是否跟随 '('
        std::string callee = IdentifierStr;
        int nextFlag = getNextToken();
        ch_flag = nextFlag;
        if (nextFlag == '(') {
            // 解析函数调用（包括嵌套调用）
            std::vector<std::unique_ptr<ExprAST>> args;

            while ((nextFlag = getNextToken()) != ')') {
                if (nextFlag == tok_number) {
                    args.push_back(std::make_unique<NumberExprAST>(NumVal));
                } else if (nextFlag == tok_identifier) {
                    // 递归解析嵌套函数调用
                    std::string nestedCallee = IdentifierStr;
                    int nestedFlag = getNextToken();
                    if (nestedFlag == '(') {
                        // 嵌套函数调用
                        args.push_back(std::make_unique<CallExprAST>(nestedCallee, ParseNestedCall()));
                    } else {
                        args.push_back(std::make_unique<VariableExprAST>(nestedCallee));
                    }
                } else {
                    return ERROR; // 非法标记
                }
            }

            // 将函数调用节点压入操作数栈
            operands.push(std::make_unique<CallExprAST>(callee, std::move(args)));
        } else {
            // 普通标识符
            operands.push(std::make_unique<VariableExprAST>(callee));
        }
    } else if (flag == tok_number) {
        operands.push(std::make_unique<NumberExprAST>(NumVal));
        ch_flag = getNextToken(); // 获取下一个标记
    } else {
        return ERROR;
    }

    while (ch_flag != ';') {
        if (ch_flag == tok_number) {
            operands.push(std::make_unique<NumberExprAST>(NumVal));
            ch_flag = getNextToken();
        } else if (ch_flag == tok_identifier) {
            std::string callee = IdentifierStr;
            int nextFlag = getNextToken();
            ch_flag = nextFlag;
            if (nextFlag == '(') {
                // 函数调用
                std::vector<std::unique_ptr<ExprAST>> args;
                while ((nextFlag = getNextToken()) != ')') {
                    if (nextFlag == tok_number) {
                        args.push_back(std::make_unique<NumberExprAST>(NumVal));
                    } else if (nextFlag == tok_identifier) {
                        std::string nestedCallee = IdentifierStr;
                        int nestedFlag = getNextToken();
                        if (nestedFlag == '(') {
                            args.push_back(std::make_unique<CallExprAST>(nestedCallee, ParseNestedCall()));
                        } else {
                            args.push_back(std::make_unique<VariableExprAST>(nestedCallee));
                        }
                    } else {
                        return ERROR;
                    }
                }
                operands.push(std::make_unique<CallExprAST>(callee, std::move(args)));
            } else {
                operands.push(std::make_unique<VariableExprAST>(callee));
            }
        } else {
            switch (ch_flag) {
                case '(':
                    operators.push(std::make_unique<VariableExprAST>("("));
                    ch_flag = getNextToken();
                    break;
                case ')':
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
                    }
                    ch_flag = getNextToken();
                    break;
                default:
                    while (!operators.empty() && Precede(dynamic_cast<VariableExprAST*>(operators.top().get())->getName()[0], ch_flag) == '>') {
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
                    operators.push(std::make_unique<VariableExprAST>(std::string(1, ch_flag)));
                    ch_flag = getNextToken();
                    break;
            }
        }
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

    // 最终的 AST 根节点
    if (!operands.empty()) {
        auto root = std::move(operands.top());
        operands.pop();
        // 此处可以返回或存储根节点
    }
    return OK;
}


