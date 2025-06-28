#ifndef AST_H
#define AST_H

#include <string>
#include <memory>



/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
  virtual ~ExprAST() = default;
  virtual std::string getName() const {return "";};
  virtual std::string codegen();
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
  double Val;

public:
  NumberExprAST(double Val) : Val(Val) {}
  double getValue() const {return Val;};
  std::string codegen();
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
  std::string Name;

public:
  VariableExprAST(const std::string &Name) : Name(Name) {}
  
  std::string getName() const {return Name;}
  std::string codegen();
};

/// UnaryExprAST - Expression class for a unary operator.
// class UnaryExprAST : public ExprAST {
//   char Opcode;
//   std::unique_ptr<ExprAST> Operand;

// public:
//   UnaryExprAST(char Opcode, std::unique_ptr<ExprAST> Operand)
//       : Opcode(Opcode), Operand(std::move(Operand)) {}

//   void codegen() override;
// };

// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
  
  std::string getName() const {return std::string(1, Op);};
  const std::unique_ptr<ExprAST>& getLHS() const { return LHS; };
  const std::unique_ptr<ExprAST>& getRHS() const { return RHS; };
  char getOp() const {return Op;};
  std::string codegen();
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
      : Callee(Callee), Args(std::move(Args)) {}
  std::string getCallee() const {return Callee;};
  const std::vector<std::unique_ptr<ExprAST>>& getArgs() const {return Args;};
  std::string codegen();
};

/// IfExprAST - Expression class for if/then/else.
class IfExprAST : public ExprAST {
  std::unique_ptr<ExprAST> Cond, Then, Else;

public:
  IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
            std::unique_ptr<ExprAST> Else)
      : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

  std::string codegen();
};

/// WhileExprAST - Expression class for for/in.
class WhileExprAST : public ExprAST {
  std::unique_ptr<ExprAST> Cond, Body;

public:
  WhileExprAST(std::unique_ptr<ExprAST> Cond,std::unique_ptr<ExprAST> Body)
      : Cond(std::move(Cond)) , Body(std::move(Body)) {}

  std::string codegen();
};

// PrototypeAST - This class represents the "prototype" for a function,
// which captures its name, and its argument names (thus implicitly the number
// of arguments the function takes), as well as if it is an operator.
class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;
  bool IsOperator;
  unsigned Precedence;

public:
  PrototypeAST(const std::string &Name, std::vector<std::string> Args)
      : Name(Name), Args(std::move(Args)){}

  std::string codegen();

};

// FunctionAST - This class represents a function definition itself.
class FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
      : Proto(std::move(Proto)), Body(std::move(Body)) {}

  std::string codegen();
};
#endif