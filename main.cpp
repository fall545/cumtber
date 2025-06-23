#include "tokenizer.h"
#include "parser.h"
static void HandleTopLevelExpression() {
  // Evaluate a top-level expression into an anonymous function.
  if (auto FnAST = ParseTopLevelExpr()) {
    fprintf(stderr, "Read ParseTopLevelExpr");
      fprintf(stderr, "\n");
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}
static void HandleDefinition() {
  if (auto FnAST = ParseDefinition()) {
      fprintf(stderr, "Read function definition:");
      fprintf(stderr, "\n");
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
    BinopPrecedence['*'] = 40; // highest.

    fprintf(stderr, "ready> ");
    getNextToken();

    // Run the main "interpreter loop" now.
    MainLoop();
}