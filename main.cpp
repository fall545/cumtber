#include "tokenizer.h"
#include "ast.h"

void TestCreateAndTraverseTree() {
    // 创建一个简单的 AST
    std::unique_ptr<ExprAST> root;
    Status status = CreateTree(root);

    if (status == OK) {
        std::cout << "Tree created successfully!" << std::endl;

        // 中序遍历 AST
        std::cout << "In-order traversal of the AST:" << std::endl;
        InOrderTraverseTree(root);
    } else {
        std::cerr << "Failed to create AST!" << std::endl;
    }
}

int main() {
    TestCreateAndTraverseTree();
    return 0;
}