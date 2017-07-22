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
public:
    Parser(const std::string &inputString, std::vector<std::pair<char,double>> vars);
    Parser(const std::string &inputString, std::vector<std::pair<char,double>> vars, std::string m_angleUnit);
    double calculateExpression();
private:
    const char* m_input;
    std::string m_angleUnit;
    std::string m_inputWithVariables;
    std::vector<std::pair<char, double>> m_vctVariables;
    static const struct MyTokens
    {
        const std::vector<std::string> tokens =
        {
          "+", "-","e", "**", "*", "/","(", ")",  "<=", ">=",
          "<", ">", "==", "!=", "&", "|","^","!", "mod", "abs",
          "factorial","sign", "inv", "cbrt","sqrt", "sqr", "cube",
          "gradtorad", "radtograd", "_exp","ln", "log2", "log8",
          "log10","log16", "sin","cos","tg","ctg","secans","csecans",
          "arcsin","arccos","arctg","arcctg","arcsecans", "arccsecans",
          "sh","ch", "th", "cth","sech","csech", "arcsh","arcch",
          "arcth","arccth","arcsech","arccsech"
        };

    } m_tokens;

    std::string parseToken();
    Expression parseUnaryExpression();
    Expression parseBinaryExpression(int minPriority);
    Expression parse();
    double evaluateExpression(const Expression &e);
    long int factorial(int n);
};

#endif // PARSER_H
