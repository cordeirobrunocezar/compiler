#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept> // to do
#include <set>

std::set<std::string> KEYWORDS = {"and", "break", "do", "else",
"elseif", "end", "false", "for", "function", "if",
"in", "local", "nil", "not", "or", "repeat", "return",
"then", "true", "until", "while"};

struct Token {
  std::string name = "";
  std::string attribute = "";
};

int attributeSize(Token token) {
  int size = 0;
  for (char c : token.attribute) {
    size += 1;
  }
  return size;
}

bool openString = false;
char currentState;

void ferr(char c) {
  switch (c) {
    case '\"':
      std::cout << "string not closed error\"\n"; // in line ...
      break;
    case ']':
      std::cout << "long commentary error\n";
      break;
    default:
      std::cout << "err\n";
      break;
  }
}

int aux = 0;

// to do error handler
Token generateToken(char c, std::ifstream &file) {
  Token token;
  switch (c) { // fix to do nested commentary
    case '-':
      aux = 0;
      if (file.peek() == '-') {
        file.get(c);
        token.name = "commentary";
        if (file.peek() == '[') {
          file.get(c);
          aux++;
          if (file.peek() == '[') {
            file.get(c);
            aux++;
            token.name = "long_commentary";
            if (file.peek() == std::char_traits<wchar_t>::eof())
              ferr(' ');
            while (file.get(c)) {
              if (aux == 0) break;
              if (c == ']') aux--;
              else if (c == '[') aux++;
            }
            if (aux != 0) {
              ferr(']');
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
      

    case '"': // ?
      aux = 0;
      if (!openString) {
        token.name = "string";
        token.attribute += '"';
        openString = true;
        aux++;
      }
      while (file.get(currentState) && openString) {
        token.attribute += currentState;
        if (currentState == '"' && token.attribute[attributeSize(token) - 2] != '\\') {
          openString = false;
          aux--;
        }
      }
      if (aux != 0) ferr('"');
      file.putback(currentState);
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
          token.name = "relational_operator";
          token.attribute = "LE";
          break;
        /*
        case '>': // ~=
          file.get(c);
          token.name = "relational_operator";
          token.attribute = "NE";
          break;
        */
        default:
          token.name = "relational_operator";
          token.attribute = "LT";
          break;
      }
      break;

    case '~':
      if (file.peek() == '=') {
        file.get(c);
        token.name = "relational_operator";
        token.attribute = "NE";
      }
      else
        token.name = '~';
      break;

    case '=':
      switch (file.peek()) {
        case '=':
          file.get(c);
          token.name = "relational_operator";
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
          token.name = "relational_operator";
          token.attribute = "GE";
          break;
        default:
          token.name = "relational_operator";
          token.attribute = "GT";
          break;
      }
      break;

    default:
      if (std::ispunct(c) && c != '_' && c != '\\') {
        token.name = c;
        break;
      }

      if (std::isalpha(c) || c == '_') {
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

      if (std::isdigit(c)) {
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

int main(int argc, char const *argv[]) {
  std::ifstream file;
  char c;
  file.open(argv[1]);
  if (file.is_open()) {
    while (file.get(c)) {
      if (openString) {
        currentState = c;
        c = '"';
      }
      // std::cout << c << std::endl;
      Token token = generateToken(c, file);
      if (token.name != "string")
        verifyKeyword(&token);
      if (token.name != "")
        std::cout << '(' << token.name << ',' << token.attribute << ')' << std::endl;
    }
  }

  file.close();
  return 0;
}
