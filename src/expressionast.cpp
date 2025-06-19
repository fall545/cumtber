#include "tokenizer.h"
#include "ast.h"
#include <stack>

typedef int Status;
enum {
    ERROR,
    OK
};

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

Status CreateTree() {
    std::stack<std::unique_ptr<ExprAST>> operators;
    std::stack<std::unique_ptr<ExprAST>> operands;

    int flag = getNextToken();
    if (flag == tok_identifier) {
        // 检查标识符后是否跟随数字
        std::string callee = IdentifierStr;
        int nextFlag = getNextToken();
        if (nextFlag == tok_number) {
            // 解析函数调用
            std::vector<std::unique_ptr<ExprAST>> args;
            args.push_back(std::make_unique<NumberExprAST>(NumVal));

            // 继续解析后续参数
            while ((nextFlag = getNextToken()) == tok_number) {
                args.push_back(std::make_unique<NumberExprAST>(NumVal));
            }

            // 将函数调用节点压入操作数栈
            operands.push(std::make_unique<CallExprAST>(callee, std::move(args)));
        } else {
            // 普通标识符
            operands.push(std::make_unique<VariableExprAST>(callee));
        }
    } else if (flag == tok_number) {
        operands.push(std::make_unique<NumberExprAST>(NumVal));
    } else {
        return ERROR;
    }

    int ch_flag = getNextToken(); // 获取下一个标记
    while (ch_flag != ';') {
        if (ch_flag == tok_number) {
            operands.push(std::make_unique<NumberExprAST>(NumVal));
        } else if (ch_flag == tok_identifier) {
            std::string callee = IdentifierStr;
            int nextFlag = getNextToken();
            if (nextFlag == tok_number) {
                // 解析函数调用
                std::vector<std::unique_ptr<ExprAST>> args;
                args.push_back(std::make_unique<NumberExprAST>(NumVal));

                while ((nextFlag = getNextToken()) == tok_number) {
                    args.push_back(std::make_unique<NumberExprAST>(NumVal));
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
