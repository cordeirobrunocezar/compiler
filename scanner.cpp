#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <queue>

#include "scanner.hpp"

std::set<std::string> KEYWORDS = {
  "and", "break", "do", "else", "elseif", "end", "false", "for", "function",
  "if","in", "local", "nil", "not", "or", "repeat", "return", "then",
  "true", "until", "while", "number"
};

void ferr(char c, std::ifstream &file) {
  switch (c) {
    default:
      std::cout << "err\n";
      break;
  }
}

int bracketCounter = 0;

Token generateToken(char c, std::ifstream &file) {
  Token token;
  switch (c) {
    case '-':
      bracketCounter = 0;
      if (file.peek() == '-') {
        file.get(c);
        token.name = "commentary";
        if (file.peek() == '[') {
          file.get(c);
          bracketCounter++;
          if (file.peek() == '[') {
            file.get(c);
            bracketCounter++;
            token.name = "long_commentary";
            //if (c == std::char_traits<char>::eof()) ferr(' ', file);
            while (file.get(c)) {
              if (bracketCounter == 0) break;
              if (c == ']') bracketCounter--;
              else if (c == '[') bracketCounter++;
            }
            if (bracketCounter != 0) {
              ferr(' ', file);
              break;
            }
            file.putback(c);
          }
        }
        else {
          while (file.get(c))
            if (c == '\n')
              break;
        }
      }
      else {
        token.name = "-";
        token.attribute = "operator";
      }
      break;

    case '"':
      token.name = "string";
      while (file.get(c)) {
        if (c == '\\') {
          switch (file.peek()) {
            case 'a':
              token.attribute += '\a';
              file.get(c);
              break;

            case 'b':
              token.attribute += '\b';
              file.get(c);
              break;

            case 'f':
              token.attribute += '\f';
              file.get(c);
              break;

            case 'n':
              token.attribute += '\n';
              file.get(c);
              break;

            case 'r':
              token.attribute += '\r';
              file.get(c);
              break;

            case 't':
              token.attribute += '\t';
              file.get(c);
              break;

            case 'v':
              token.attribute += '\v';
              file.get(c);
              break;

            case '\\':
              token.attribute += '\\';
              file.get(c);
              break;

            case '\"':
              token.attribute += '\"';
              file.get(c);
              break;

            default:
              break;
          }
        }
        else {
          if (c == '"') break;
          else token.attribute += c;
        }
      }
      file.putback(c);
      if (file.peek() == std::char_traits<char>::eof()) ferr(' ', file);
      else file.get(c);
      break;

    case '+':
      token.name = "+";
      token.attribute = "operator";
      break;

    case '*':
      token.name = "*";
      token.attribute = "operator";
      break;

    case '/':
      token.name = "/";
      token.attribute = "operator";
      break;

    case '^':
      token.name = "^";
      token.attribute = "operator";
      break;

    case '<':
      switch (file.peek()) {
        case '=':
          file.get(c);
          token.name = "<=";
          token.attribute = "LE";
          break;

        default:
          token.name = "<";
          token.attribute = "LT";
          break;
      }
      break;

    case '~':
      if (file.peek() == '=') {
        file.get(c);
        token.name = "~=";
        token.attribute = "NE";
      }
      break;

    case '=':
      switch (file.peek()) {
        case '=':
          file.get(c);
          token.name = "==";
          token.attribute = "EQ";
          break;
        default:
          token.name = "=";
          token.attribute = "operator";
          break;
      }
      break;

    case '>':
      switch (file.peek()) {
        case '=':
          file.get(c);
          token.name = ">=";
          token.attribute = "GE";
          break;
        default:
          token.name = ">";
          token.attribute = "GT";
          break;
      }
      break;

    default:
      if (std::ispunct(c) && c != '_' && c != '\\') {
        token.name = c;
        break;
      }

      else if (std::isalpha(c) || c == '_') {
        token.name = "identifier";
        token.attribute += c;
        while (file.get(c)) {
          if ((std::isalpha(c) || std::isdigit(c) || c == '_'))
            token.attribute += c;
          else {
            file.putback(c);
            break;
          }
        }
      }

      else if (std::isdigit(c)) {
        token.name = "number";
        token.attribute += c;
        while (file.get(c)) {
          if (std::isdigit(c))
            token.attribute = token.attribute + c;
          else {
            file.putback(c);
            break;
          }
        }
      }
      break;
  }
  return token;
}

void verifyKeyword(Token *token) {
  if (KEYWORDS.find(token->attribute) != KEYWORDS.end()) {
    token->name = token->attribute;
    token->attribute = "";
  }
}

Token getToken(std::queue<Token> &q) {
  if (q.empty()) exit(-1);
  Token token = q.front();
  q.pop();
  std::cout << '(' << token.name << ',' << token.attribute << ')' << std::endl;
  return token;
}

std::queue<Token> scanner(char const *s[]) {
  std::ifstream file;
  char c;
  std::queue<Token> q;
  file.open(s[1]);
  if (file.is_open()) {
    while (file.get(c)) {
      // std::cout << c << std::endl;
      Token token = generateToken(c, file);
      if (token.name != "string")
        verifyKeyword(&token);
      if (
        token.name != "" &&
        token.name != "commentary" &&
        token.name != "long_commentary"
      ) {
        q.push(token);
      }
    }
  }
  file.close();
  return q;
}