#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <queue>

#include "scanner.hpp"

void err(std::string);
void sync(std::set<std::string>);
void Block();
void Statement();
void L();
void F();
void I();
void O();
void Expressions();
void e();
void Expression();
void E();
void Prefix();
void P();
void Field();
void Operator();
void Variables();
void Variable();
void Function();
void FunctionBody();
void Parameters();
void Identifiers();

Token token;
std::queue<Token> tokenQueue;
// First
std::set<std::string> FirstBlock = {
  "identifier","(","function","do","while","if","return","break","for","local"
};
std::set<std::string> FirstVariables = {
  "identifier", "("
};
std::set<std::string> FirstExpressions = {
  "not","-","identifier","(","function","{","nil","true","false","number","string"
};
std::set<std::string> FirstIdentifiers = {
  "identifier"
};
std::set<std::string> FirstParameters = {
  "identifier"
};
std::set<std::string> FirstFunctionBody = {
  "identifier"
};
std::set<std::string> FirstPrefix = {
  "identifier","("
};
std::set<std::string> FirstFunction = {
  "function"
};
std::set<std::string> FirstField = {
  "[","identifier"
};
std::set<std::string> FirstOperator = {
  "or","and","<",">","<=","~=","==","+","-","*","/","^"
};

void err(std::string e) {
  std::cout << "expected " << e << std::endl << "received " << token.name << std::endl;
}

void sync(std::set<std::string> expected) {
  std::cout << "sync" << std::endl;
  do if (!(tokenQueue.empty())) getToken(tokenQueue); while (!(expected.find(token.name) != expected.end()));
  std::cout << "#" << token.name << std::endl;
}

void Block() {
  std::cout << "Block" << std::endl;
  if (FirstBlock.find(token.name) != FirstBlock.end()) {
    Statement();
    if (token.name == ";") token = getToken(tokenQueue);
    else err(";");
    Block();
  }
}

void Statement() {
  std::cout << "Statement" << std::endl;
  // Variables = Expressions
  if (FirstVariables.find(token.name) != FirstVariables.end()) {
    Variables();
    if (token.name == "=") token = getToken(tokenQueue);
    else {
      err("=");
      sync(FirstExpressions);
    }
    Expressions();
  }
  // function F
  else if (token.name == "function") {
    token = getToken(tokenQueue);
    F();
  }
  // do Block end
  else if (token.name == "do") {
    token = getToken(tokenQueue);
    Block();
    if (token.name == "end") token = getToken(tokenQueue);
    else err("end");
  }
  // while Expression do Block end
  else if (token.name == "while") {
    token = getToken(tokenQueue);
    Expression();
    if (token.name == "do") token = getToken(tokenQueue);
    else {
      err("do");
      sync(FirstBlock);
    }
    Block();
    if (token.name == "end") token = getToken(tokenQueue);
    else err("end");
  }
  // if Expression then Block (elseif Expression then Block)* (else Block)’ end
  else if (token.name == "if") {
    token = getToken(tokenQueue);
    Expression();
    if (token.name == "then") token = getToken(tokenQueue);
    else {
      err("then");
      sync(FirstBlock);
    }
    Block();
    if (token.name == "elseif") {
      token = getToken(tokenQueue);
      Expression();
      if (token.name == "then") token = getToken(tokenQueue);
      Block();
      Statement();
    }
    if (token.name == "else") {
      token = getToken(tokenQueue);
      Block();
    }
    if (token.name == "end") token = getToken(tokenQueue);
    else err("end");
  }
  // return Expressions’
  else if (token.name == "return") {
    token = getToken(tokenQueue);
    if (FirstExpressions.find(token.name) != FirstExpressions.end()) {
      Expressions();
    }
  }
  // break
  else if (token.name == "break") {
    token = getToken(tokenQueue);
  }
  // for O
  else if (token.name == "for") {
    token = getToken(tokenQueue);
    O();
  }
  // local L
  else if (token.name == "local") {
    token = getToken(tokenQueue);
    L();
  }
  else {
    err("identifier|(|function|do|while|if|return|break|for|local");
    sync(std::set<std::string> {";"}); // ; 
  }
}

void L() {
  std::cout << "L" << std::endl;
  // function identifier FunctionBody
  if (token.name == "function") {
    token = getToken(tokenQueue);
    if (token.name == "identifier") token = getToken(tokenQueue);
    else {
      err("identifier");
      sync(std::set<std::string> {"identifier","(","=","[",","}); // FunctionBody, I, "=", P, "(", ","
    }
    FunctionBody();
  }
  // Identifiers = Expressions
  else if (FirstIdentifiers.find(token.name) != FirstIdentifiers.end()) {
    Identifiers();
    if (token.name == "=") token = getToken(tokenQueue);
    else {
      err("=");
      sync(FirstExpressions);
    }
    Expressions();
  }
  // else
}

void F() {
  std::cout << "F" << std::endl;
  // identifier I
  if (token.name == "identifier") {
    token = getToken(tokenQueue);
    I();
  }
  else {
    err("identifier");
    sync(std::set<std::string> {"identifier","(","=","[",","}); // FunctionBody, I, "=", P, "(", ","
  }
  // else
}

void I() {
  std::cout << "I" << std::endl;
  // (Parameters’) Block end
  if (token.name == "(") {
    token = getToken(tokenQueue);
    if (FirstParameters.find(token.name) != FirstParameters.end()) Parameters();
    if (token.name == ")") token = getToken(tokenQueue);
    else {
      err(")");
      sync(std::set<std::string> {"identifier","(","function","do","while",
      "if","return","break","for","local","["}); // Block, P, 
    }
    Block();
    if (token.name == "end") token = getToken(tokenQueue);
    else err("end");
  }
  // FunctionBody
  else if (FirstFunctionBody.find(token.name) != FirstFunctionBody.end()) {
    FunctionBody();
  }
  // else
}

void O() {
  std::cout << "O" << std::endl;
  // = Expression do Block end
  if (token.name == "=") {
    token = getToken(tokenQueue);
    Expression();
    if (token.name == "do") token = getToken(tokenQueue);
    else {
      err("do");
      sync(FirstBlock);
    }
    Block();
    if (token.name == "end") token = getToken(tokenQueue);
    else err("end");
  }
  // identifier = Expressions do Block end
  else if (token.name == "identifier") {
    token = getToken(tokenQueue);
    if (token.name == "=") token = getToken(tokenQueue);
    else {
      err("=");
      sync(FirstExpressions);
    }
    Expressions();
    if (token.name == "do") token = getToken(tokenQueue);
    else {
      err("do");
      sync(FirstBlock);
    }
    Block();
    if (token.name == "end") token = getToken(tokenQueue);
    else err("end");
  }
  // else
}

void Expressions() {
  std::cout << "Expressions" << std::endl;
  // Expression (, Expression)*
  if (FirstExpressions.find(token.name) != FirstExpressions.end()) {
    Expression();
    if (token.name == ",") {
      token = getToken(tokenQueue);
      Expression();
      e();
    }
  }
  else {
    err("not|-|identifier|(|function|{|nil|true|false|number|string");
    sync(std::set<std::string> {"do"});  // "do",
  }
}

void e() {
  std::cout << "e" << std::endl;
  if (token.name == ",") {
    token = getToken(tokenQueue);
    Expression();
    e();
  }
}

void Expression() {
  std::cout << "Expression" << std::endl;
  // not Expression E
  if (token.name == "not") {
    token = getToken(tokenQueue);
    Expression();
    E();
  }
  // - Expression E
  else if (token.name == "-") {
    token = getToken(tokenQueue);
    Expression();
    E();
  }
  // Prefix E
  else if (FirstPrefix.find(token.name) != FirstPrefix.end()) {
    Prefix();
    E();
  }
  // Function E
  else if (FirstFunction.find(token.name) != FirstFunction.end()) {
    Function();
    E();
  }
  // {(Field(, Field)*)’} E
  else if (token.name == "{") {
    token = getToken(tokenQueue);
    if (FirstField.find(token.name) != FirstField.end()) {
      Field();
      if (token.name == ",") {
        token = getToken(tokenQueue);
        Field();
        Expression(); // check
      }
    }
    E();
  }
  // nil E
  else if (token.name == "nil") {
    token = getToken(tokenQueue);
    E();
  }
  // true E
  else if (token.name == "true") {
    token = getToken(tokenQueue);
    E();
  }
  // false E
  else if (token.name == "false") {
    token = getToken(tokenQueue);
    E();
  }
  // number E
  else if (token.name == "number") {
    token = getToken(tokenQueue);
    E();
  }
  // string E
  else if (token.name == "string") {
    token = getToken(tokenQueue);
    E();
  }
  else {
    err(""); // expression
    sync(std::set<std::string> {"or","and","<",">","<=","~=","==","+","-",
    "*","/","^",")","]"}); // E, ")", "]", 
  }
}

void E() {
  std::cout << "E" << std::endl;
  // Operator Expression E
  if (FirstOperator.find(token.name) != FirstOperator.end()) {
    Operator();
    Expression();
    E();
  }
  // else token = getToken(tokenQueue);
}

void Prefix() {
  std::cout << "Prefix" << std::endl;
  // identifier P
  if (token.name == "identifier") {
    token = getToken(tokenQueue);
    P();
  }
  // (Expression) P
  else if (token.name == "(") {
    token = getToken(tokenQueue);
    Expression();
    if (token.name == ")") token = getToken(tokenQueue);
    else {
      err(")");
      sync(std::set<std::string> {"identifier","(","function","do","while",
      "if","return","break","for","local","["}); // Block, P,
    }
    P();
  }
  else {
    err("identifier|("); //
    sync(std::set<std::string> {"or","and","<",">","<=","~=","==","+","-",
    "*","/","^","["}); // E, "["
  }
}

void P() {
  std::cout << "P" << std::endl;
  // [Expression] P
  if (token.name == "[") {
    token = getToken(tokenQueue);
    Expression();
    if (token.name == "]") token = getToken(tokenQueue);
    else {
      err("]");
      sync(std::set<std::string> {"=", "["}); // "=", P, 
    }
    P();
  }
  // else token = getToken(tokenQueue);
}

void Field() {
  std::cout << "Field" << std::endl;
  // [Expression] = Expression
  if (token.name == "[") {
    token = getToken(tokenQueue);
    Expression();
    if (token.name == "]") token = getToken(tokenQueue);
    else {
      err("[");
      sync(FirstExpressions); // Expression
    }
    if (token.name == "=") token = getToken(tokenQueue);
    else {
      err("=");
      sync(FirstExpressions); // 
    }
    Expression();
  }
  // identifier = Expression
  else if (token.name == "=") {
    if (token.name == "=") token = getToken(tokenQueue);
    else {
      err("=");
      sync(FirstExpressions);
    }
    Expression();
  }
  else {
    err("[|identifier"); //
    sync(std::set<std::string> {"}",","}); // "}", ","
  }
}

void Operator() {
  std::cout << "Operator" << std::endl;
  // or | and | < | > | <= | >= | ~= | == | + | - | * | / | ^
  if (FirstOperator.find(token.name) != FirstOperator.end()) 
    token = getToken(tokenQueue);
  else {
    err(""); //
    sync(std::set<std::string> {"not","-","identifier","(","function","{",
    "nil","true","false","number","string"}); // Expression
  }
}
void Variables() {
  std::cout << "Variables" << std::endl;
  // Variable (, Variable)*
  if (FirstVariables.find(token.name) != FirstVariables.end()) {
    Variable();
    if (token.name == ",") {
      token = getToken(tokenQueue);
      Variable();
      Variables();
    }
  }
  else {
    err("identifier");
    sync(std::set<std::string> {"="}); // "="
  }
}

void Variable() {
  std::cout << "Variable" << std::endl;
  if (token.name == "identifier") token = getToken(tokenQueue);
  else if (FirstPrefix.find(token.name) != FirstPrefix.end()) {
    Prefix();
    if (token.name == "[") token = getToken(tokenQueue);
    else {
      err("[");
      sync(FirstExpressions); //
    }
    Expressions();
    if (token.name == "]") token = getToken(tokenQueue);
    else err("]");
  }
  else {
    err("identifier|("); //
    sync(std::set<std::string> {","}); // ","
  }
}

void Function() {
  std::cout << "Function" << std::endl;
  // function FunctionBody
  if (token.name == "function") {
    FunctionBody();
  }
  else {
    err("function");
    sync(std::set<std::string> {"or","and","<",">","<=","~=","==","+","-",
    "*","/","^"}); // E
  }
}

void FunctionBody() {
  std::cout << "FunctionBody" << std::endl;
  // identifier (Parameters’) Block end
  if (token.name == "identifier") {
    if (token.name == "(") {
      token = getToken(tokenQueue);
      Parameters();
    }
    else err("(");
    if (token.name == ")") token = getToken(tokenQueue);
    else {
      err(")");
      sync(std::set<std::string> {"identifier","(","function","do","while",
      "if","return","break","for","local","["}); // Block, P,
    }
    Block();
    if (token.name == "end") token = getToken(tokenQueue);
    else {
      err("end");
    }
  }
  else {
    err("identifier");
  }
}

void Parameters() {
  std::cout << "Parameters" << std::endl;
  // Identifiers
  if (token.name == "identifier") {
    Identifiers();
  }
  else {
    err("identifier");
    sync(std::set<std::string> {")"}); // ")" 
  }
}

void Identifiers() {
  std::cout << "Identifiers" << std::endl;
  if (token.name == "identifier") {
    token = getToken(tokenQueue);
    if (token.name == ",") {
      token = getToken(tokenQueue);
      if (token.name == "identifier") token = getToken(tokenQueue);
      else err("identifier");
    }
  }
  else {
    err("identifier");
    sync(std::set<std::string> {"="}); // "=" 
  }
}

void printQueue(std::queue<Token> q) {
  while (!q.empty()) {
    std::cout << '(' << q.front().name << ',' << q.front().attribute << ')' << std::endl;
    q.pop();
  }
}

int main(int argc, char const *argv[]) {
  tokenQueue = scanner(argv);
  //printQueue(tokenQueue);
  token = getToken(tokenQueue);
  Block();
  return 0;
}