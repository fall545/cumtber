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
                // 嵌套函数调用
                args.push_back(std::make_unique<CallExprAST>(nestedCallee, ParseNestedCall()));
            } else {
                args.push_back(std::make_unique<VariableExprAST>(nestedCallee));
            }
        } else if (nextFlag == ',') {
            // 跳过逗号，继续解析下一个参数
            continue;
        } else {
            syntaxerror(std::string("function call error,Unexpected token in nested call"));
        }
    }
    return args;
}
/**
 * @brief Creates an abstract syntax tree (AST) from a sequence of tokens.
 * 
 * @param r Reference to an ExprAST object where the resulting AST will be stored.
 * @return Status OK if the tree is successfully created, ERROR if an invalid token is encountered.
 */
static std::unique_ptr<ExprAST> ParseExpression() {
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
            // 函数调用
            std::vector<std::unique_ptr<ExprAST>> args;
            while ((nextFlag = getNextToken()) != ')') {
                // std::cout<<NumVal;
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
                } else if (nextFlag == ',') {
                    // 跳过逗号，继续解析下一个参数
                    continue;
                } else {
                    syntaxerror(std::string("function call error,Unexpected token in function call"));
                }
            }
            // 将函数调用节点压入操作数栈
            operands.push(std::make_unique<CallExprAST>(callee, std::move(args)));
            ch_flag = getNextToken();
        } else {
            // 普通标识符
            operands.push(std::make_unique<VariableExprAST>(callee));
        }
    } else if (flag == tok_number) {
        operands.push(std::make_unique<NumberExprAST>(NumVal));
        ch_flag = getNextToken(); // 获取下一个标记
        syntaxerror(std::string("expression error,Unexpected token at the beginning of expression"));
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
                    } else if (nextFlag == ',') {
                        // 跳过逗号，继续解析下一个参数
                        continue;
                    } else {
                        syntaxerror(std::string("function call error,Unexpected token in function call"));
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
        return std::move(operands.top());
    }
    syntaxerror(std::string("expression error,Failed to parse expression"));
}

// Status InOrderTraverseTree(const std::unique_ptr<ExprAST>& node) {
//     if (!node) return OK; // 如果节点为空，直接返回

//     // 根据节点类型进行处理
//     if (const auto* binaryNode = dynamic_cast<const BinaryExprAST*>(node.get())) {
//         // 中序遍历左子树
//         InOrderTraverseTree(binaryNode->getLHS());

//         // 输出当前节点（操作符）
//         std::cout << binaryNode->getOp() << " ";

//         // 中序遍历右子树
//         InOrderTraverseTree(binaryNode->getRHS());
//     } else if (const auto* numberNode = dynamic_cast<const NumberExprAST*>(node.get())) {
//         // 输出数字节点的值
//         std::cout << numberNode->getValue() << " ";
//     } else if (const auto* variableNode = dynamic_cast<const VariableExprAST*>(node.get())) {
//         // 输出变量节点的名称
//         std::cout << variableNode->getName() << " ";
//     } else if (const auto* callNode = dynamic_cast<const CallExprAST*>(node.get())) {
//         // 输出函数调用节点
//         std::cout << callNode->getCallee() << "(";

//         // 遍历函数参数
//         const auto& args = callNode->getArgs();
//         for (size_t i = 0; i < args.size(); ++i) {
//             InOrderTraverseTree(args[i]);
//             if (i < args.size() - 1) {
//                 std::cout << ", ";
//             }
//         }

//         std::cout << ") ";
//     } else {
//         throw std::runtime_error("Unknown node type in AST traversal");
//     }

//     return OK;
// }

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
