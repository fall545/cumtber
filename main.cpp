#include "tokenizer.h"
#include "parser.h"
#include <iostream>
std::string IdentifierStr;
int CurTok;
extern std::unique_ptr<FunctionAST> ParseDefinition();
std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
  if (auto E = ParseExpression()) {
    // Make an anonymous proto.
    auto Proto = std::make_unique<PrototypeAST>("__anon_expr",
                                                 std::vector<std::string>());
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  }
  return nullptr;
}
static void HandleTopLevelExpression() {
  // Evaluate a top-level expression into an anonymous function.
  if (auto FnAST = ParseTopLevelExpr()) {
    auto info = FnAST->codegen();
    fprintf(stderr, "Read ParseTopLevelExpr\n");
    std::cout<<info<<"\nParseTopLevelExpr end\n";
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}
static void HandleDefinition() {
  if (auto FnAST = ParseDefinition()) {
      auto info = FnAST->codegen();
      fprintf(stderr, "Read function definition:\n");
      std::cout<<info<<"\nfunction definition end\n";
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}
/// top ::= definition | expression | ';'
static void MainLoop() {
  while (true) {
    fprintf(stderr, "ready> ");
    switch (CurTok) {
    case tok_eof:
      return;
    case ';': // ignore top-level semicolons.
      getNextToken();
      break;
    case tok_def:
      HandleDefinition();
      break;
    default:
      HandleTopLevelExpression();
      break;
    }
  }
}
int main(){
    BinopPrecedence['<'] = 10;
    BinopPrecedence['+'] = 20;
    BinopPrecedence['-'] = 20;
    BinopPrecedence['='] = 50;
    BinopPrecedence['*'] = 40; // highest.

    fprintf(stderr, "ready> ");
    getNextToken();

    // Run the main "interpreter loop" now.
    MainLoop();
}