#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
#include <cmath>
#include <ctype.h>
#include <cstring>

struct Expression {
    Expression(std::string token) : token(token) {}
    Expression(std::string token, Expression a) : token(token), args{ a } {}
    Expression(std::string token, Expression a, Expression b) : token(token), args{ a, b } {}
    std::string token;
    std::vector<Expression> args;
};

class Parser {
private:
    const char* input;
    std::string inputExprWithVariables;
    std::vector<std::string> tokens =
    { "+", "-", "**", "*", "/","(", ")",  "<=", ">=",
      "<", ">", "==", "!=", "&", "|","^","!", "mod",
      "abs", "sign", "cbrt","sqrt", "sqr", "cube", "gradtorad",
      "radtograd", "exp","ln", "log2", "log8","log10","log16",
      "sin","cos","tg","ctg","secans","csecans",
      "arcsin","arccos","arctg","arcctg","arcsecans", "arccsecans",
      "sh","ch", "th", "cth","sech","csech",
      "arcsh","arcch", "arcth","arccth","arcsech","arccsech"};
public:
    Parser(std::string inputExpr);
    Parser(std::string inputExpr, std::vector<std::pair<char, double> > variable);
    double calculateExpression();
    ~Parser();
private:
    std::string parseToken();
    Expression parseUnaryExpression();
    Expression parseBinaryExpression(int minPriority);
    Expression parse();
    double evaluateExpression(const Expression &e);
};

#endif // PARSER_H
