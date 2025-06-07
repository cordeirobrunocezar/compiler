#pragma once

struct Token {
  std::string name;
  std::string attribute;
};

void ferr(char, std::ifstream &);
Token generateToken(char, std::ifstream &);
void verifyKeyword(Token *);
Token getToken(std::queue<Token> &);
std::queue<Token> scanner(char const *[]);