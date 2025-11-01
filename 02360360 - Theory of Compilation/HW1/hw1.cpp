#include "tokens.hpp"

void InvalidCharacter(char* invalid){
  std::cout << "Error " << invalid << std::endl;
  exit(0);
}

void UnclosedString(){
  std::cout << "Error unclosed string" << std::endl;
  exit(0);
}

void BadSequence(char* invalid){
  std::cout << "Error undefined escape sequence " << invalid << std::endl;
  exit(0);
}

int main() {
  std::vector<char> inputString;
  bool inString = false;

  int token;
  while ((token = yylex())) {
    switch (token) {
      case VOID:
        std::cout << lineNumber << " VOID " << yytext << std::endl;
        break;
      case INT:
        std::cout << lineNumber << " INT " << yytext << std::endl;
        break;
      case BYTE:
        std::cout << lineNumber << " BYTE " << yytext << std::endl;
        break;
      case B:
        std::cout << lineNumber << " B " << yytext << std::endl;
        break;
      case BOOL:
        std::cout << lineNumber << " BOOL " << yytext << std::endl;
        break;
      case AND:
        std::cout << lineNumber << " AND " << yytext << std::endl;
        break;
      case OR:
        std::cout << lineNumber << " OR " << yytext << std::endl;
        break;
      case NOT:
        std::cout << lineNumber << " NOT " << yytext << std::endl;
        break;
      case TRUE:
        std::cout << lineNumber << " TRUE " << yytext << std::endl;
        break;
      case FALSE:
        std::cout << lineNumber << " FALSE " << yytext << std::endl;
        break;
      case RETURN:
        std::cout << lineNumber << " RETURN " << yytext << std::endl;
        break;
      case IF:
        std::cout << lineNumber << " IF " << yytext << std::endl;
        break;
      case ELSE:
        std::cout << lineNumber << " ELSE " << yytext << std::endl;
        break;
      case WHILE:
        std::cout << lineNumber << " WHILE " << yytext << std::endl;
        break;
      case BREAK:
        std::cout << lineNumber << " BREAK " << yytext << std::endl;
        break;
      case CONTINUE:
        std::cout << lineNumber << " CONTINUE " << yytext << std::endl;
        break;
      case SC:
        std::cout << lineNumber << " SC " << yytext << std::endl;
        break;
      case COMMA:
        std::cout << lineNumber << " COMMA " << yytext << std::endl;
        break;
      case LPAREN:
        std::cout << lineNumber << " LPAREN " << yytext << std::endl;
        break;
      case RPAREN:
        std::cout << lineNumber << " RPAREN " << yytext << std::endl;
        break;
      case LBRACE:
        std::cout << lineNumber << " LBRACE " << yytext << std::endl;
        break;
      case RBRACE:
        std::cout << lineNumber << " RBRACE " << yytext << std::endl;
        break;
      case ASSIGN:
        std::cout << lineNumber << " ASSIGN " << yytext << std::endl;
        break;
      case RELOP:
        std::cout << lineNumber << " RELOP " << yytext << std::endl;
        break;
      case BINOP:
        std::cout << lineNumber << " BINOP " << yytext << std::endl;
        break;
      // ========== ========== ========== ==========
      case COMMENT:
        std::cout << lineNumber - 1 << " COMMENT //" << std::endl;
        break;
      case ID:
        printf("%d ID %s\n", lineNumber, yytext);
        break;
      case NUM:
        std::cout << lineNumber << " NUM " << yytext << std::endl;
        break;
      case STRING: {
        if (stringStart) {
          inputString.clear();
          stringStart = false;
          inString = true;
        }
        else if (stringEnd) {
          std::cout << lineNumber << " STRING ";

          for (char c : inputString) std::cout << c;

          std::cout << std::endl;

          stringEnd = false;
          inString = false;
        } 
        else {
          if (yytext[0] == '\n' || yytext[0] == '\r') UnclosedString();
          if (yytext[0] == '\\') {
            switch (yytext[1]) {
              case '\\': inputString.push_back('\\');
                break;
              case 'n': inputString.push_back('\n');
                break;
              case 'r': inputString.push_back('\r');
                break;
              case 't': inputString.push_back('\t');
                break;
              case '0': inputString.push_back('\0');
                break;
              case '"': inputString.push_back('"');
                break;
              case 'x':{
                if (((yytext[2] >= '0' && yytext[2] <= '7')) && \
                    ((yytext[3] >= '0' && yytext[3] <= '9') || (yytext[3] >= 'A' && yytext[3] <= 'F') || (yytext[3] >= 'a' && yytext[3] <= 'f')) ) {
                      
                      int a,b;
                      
                      a = yytext[2] - '0';

                      if (yytext[3] >= '0' && yytext[3] <= '9')b = yytext[3] - '0';
                      if (yytext[3] >= 'A' && yytext[3] <= 'F')b = yytext[3] - 'A' + 10;
                      if (yytext[3] >= 'a' && yytext[3] <= 'f')b = yytext[3] - 'a' + 10;

                      inputString.push_back(a*16+b);
                    }
                    else {
                      BadSequence(yytext+1);
                    }
              }break;
              default:BadSequence(yytext+1);
            };
          }
          else 
          inputString.push_back(*yytext);
        }
      } break;
      case INVALIDCHAR:InvalidCharacter(yytext);
    };
  }

  if (inString) UnclosedString();

  return 0;
}