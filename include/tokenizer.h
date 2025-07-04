#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

// std::string IdentifierStr; // Filled in if tok_identifier
extern std::string IdentifierStr;
extern int CurTok;
static double NumVal;             // Filled in if tok_number
enum{
  tok_eof = -1,//这个是结束的标志,不在你们的语法解析里,不用管

  tok_def = -2,//def
  tok_extern = -3,

  tok_identifier = -4,
  tok_number = -5
};
static unsigned long long linenum=0;

static void syntaxerror(std::string error){
  printf("at line %d , syntax error : \n%s\n\n",linenum,error.c_str());
}

static int gettok() {
  static int LastChar = ' ';

  // Skip any whitespace.
  while (isspace(LastChar)) {
  if (LastChar == '\n') linenum++;
  LastChar = getchar();
}
  if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
    IdentifierStr = LastChar;
    while (isalnum((LastChar = getchar())))
      IdentifierStr += LastChar;

    if (IdentifierStr == "def")
      return tok_def;
    if (IdentifierStr == "extern")
      return tok_extern;
    return tok_identifier;
  }

  if (isdigit(LastChar) || LastChar == '.') { // Number: [0-9.]+
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = getchar();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), nullptr);
    return tok_number;
  }

  if (LastChar == '#') {
  // Comment until end of line.
  do {
    if (LastChar == '\n') linenum++;
    LastChar = getchar();
  } while (LastChar != 4 && LastChar != '\n' && LastChar != '\r');
  if (LastChar != 4)
    return gettok();
}

  // Check for end of file.  Don't eat the EOF.
  if (LastChar == 4)
    return tok_eof;

  // Otherwise, just return the character as its ascii value.
  int ThisChar = LastChar;
  LastChar = getchar();
  return ThisChar;
}

// static int CurTok;
static int getNextToken() { return CurTok = gettok(); }


static std::map<char, int> BinopPrecedence;//   +-*/ = 10 10 20 20 

static int GetTokPrecedence() {
  if (!isascii(CurTok))
    return -1;

  // Make sure it's a declared binop.
  int TokPrec = BinopPrecedence[CurTok];
  if (TokPrec <= 0)
    return -1;
  return TokPrec;
}
#endif