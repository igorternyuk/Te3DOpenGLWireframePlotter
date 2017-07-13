#include <string>
#include <vector>
#include <cmath>
#include <ctype.h>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include "parser.h"
Parser::Parser(std::string inputString)
{
    input = inputString.c_str();
}
Parser::Parser(std::string inputString, std::vector<std::pair<char, double>> variable)
{
    //Добавляем в вектор с парами = имя переменной - значение две константы
    //число ПИ и число Непера
    std::pair<char, double> Pi('P', M_PI);
    std::pair<char, double> E('E', M_E);
    std::pair<char, double> G('G', (1 + sqrt(5)) / 2);
    variable.push_back(Pi);
    variable.push_back(E);
    variable.push_back(G);
    inputExprWithVariables = inputString;
    // Во внешнем циклке перебираем все элементы вектора переменных
    for(auto &v : variable)
    {
        // Во внутреннем циклке сравниваем каждый символ входной строки с текущим элементом в векторе переменных

        for(int i = 0; i < inputExprWithVariables.length(); ++i)
           {
            if(inputExprWithVariables[i] == v.first)
            {
                //Скопировать часть строки с начала до х
                std::string buf1 = inputExprWithVariables.substr(0,i);
                //Скопировать часть строки с х + 1 до конца
                std::string buf2 = inputExprWithVariables.substr(i + 1,inputExprWithVariables.length() - i);
                //Конвертировать значение переменной в строку обернув в скобки
                std::ostringstream sstream;
                //sstream << "(" << variable.second << ")";
                sstream << v.second;
                std::string res = sstream.str();
                // добавляем к первой части значение икса
                buf1 += res;
                //Конкатинируем строки
                inputExprWithVariables = buf1 + buf2;
            }
          }
    }
    input = inputExprWithVariables.c_str();
}
Parser::~Parser()
{}
std::string Parser::parseToken()
{
    //std::cout << "input= " << input << std::endl;
    while (isspace(*input)) ++input;
    // Проверка является токен числом
    if (isdigit(*input))
    {
        std::string number;
        while (isdigit(*input) || *input == '.') number.push_back(*input++);
        return number;
    }
    // Проверка является токен оператором или функцией
    for (auto& t : tokens)
    {
        if (strncmp(input, t.c_str(), t.size()) == 0)
        {
            input += t.size();
            return t;
        }
    }
    // Если совпадений нет возвращаем пустую строку
    return "";
}
Expression Parser::parseUnaryExpression() {
    auto token = parseToken();
    if (token.empty()) throw std::runtime_error("Invalid input");
    if (token == "(") {
        auto result = parse();
        if (parseToken() != ")") throw std::runtime_error("Expected ')'");
        return result;
    }
    if (isdigit(token[0]))
        return Expression(token);
    return Expression(token, parseUnaryExpression());
}
int getPriority(const std::string& binaryOperation) {
    if (binaryOperation == "+" || binaryOperation == "-") return 1;
    if (binaryOperation == "*" || binaryOperation == "/" || binaryOperation == "mod") return 2;
    if (binaryOperation == "&" || binaryOperation == "|" || binaryOperation == "^" ) return 3;
    if (binaryOperation == "<=" || binaryOperation == ">=" || binaryOperation == "<" ||
        binaryOperation == ">" || binaryOperation == "==" || binaryOperation == "!=") return 4;
    if (binaryOperation == "**") return 5;
    return 0;
}
Expression Parser::parseBinaryExpression(int minPriority) {
    auto leftExpression = parseUnaryExpression();
    for (;;) {
        auto op = parseToken();
        auto priority = getPriority(op);
        if (priority <= minPriority) {
            input -= op.size();
            return leftExpression;
        }
        auto rightExpression = parseBinaryExpression(priority);
        leftExpression = Expression(op, leftExpression, rightExpression);
    }
}
Expression Parser::parse() {
    return parseBinaryExpression(0);
}

double Parser::evaluateExpression(const Expression &e)
{
    switch (e.args.size()) {
    case 2: {
        auto a = evaluateExpression(e.args[0]);
        auto b = evaluateExpression(e.args[1]);
        if (e.token == "+") return a + b;
        if (e.token == "-") return a - b;
        if (e.token == "*") return a * b;
        if (e.token == "/")
        {
            if(b != 0)
            return a / b;
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "**")
        {
            return pow(a, b);
        }
        if (e.token == "mod")
        {
            if(b != 0)
                 return (int)a % (int)b;
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "<=") return a <= b ? 1 : 0;
        if (e.token == ">=") return a >= b ? 1 : 0;
        if (e.token == "<") return a < b ? 1 : 0;
        if (e.token == ">") return a > b ? 1 : 0;
        if (e.token == "==") return a == b ? 1 : 0;
        if (e.token == "!=") return a != b ? 1 : 0;
        if (e.token == "&") return (a != 0 && b != 0) ? 1 : 0;
        if (e.token == "|") return (a != 0 || b != 0) ? 1 : 0;
        if (e.token == "^") return ((a != 0 && b == 0) || (a == 0 && b != 0)) ? 1 : 0;
        throw std::runtime_error("Unknown binary operator");
    }
    case 1: {
        auto a = evaluateExpression(e.args[0]);
        if (e.token == "+") return +a;
        if (e.token == "-") return -a;
        if (e.token == "!") return (a != 0) ? 0 : 1;
        if (e.token == "sign") return (a >= 0) ? 1 : -1;
        if (e.token == "abs") return fabs(a);
                if (e.token == "cbrt") return pow(a,pow(3,-1));
        if (e.token == "sqr") return pow(a,2);
        if (e.token == "cube") return pow(a,3);
        if (e.token == "gradtorad") return M_PI * a / 180;
        if (e.token == "radtograd") return 180 * a / M_PI;
        if (e.token == "exp") return exp(a);
        if (e.token == "ln")
        {
            if(a >= 0)
                 return log(a);
            else throw  std::runtime_error("Argument is out of the function domen");
        }
        if (e.token == "log8")
        {
            if(a >= 0)
                 return log10(a)/log10(8);
            else throw  std::runtime_error("Argument is out of the function domen");
        }
        if (e.token == "log10")
        {
            if(a >= 0)
                 return log10(a);
            else throw  std::runtime_error("Argument is out of the function domen");
        }
        if (e.token == "log16")
        {
            if(a >= 0)
                 return log10(a)/log10(16);
            else throw  std::runtime_error("Argument is out of the function domen");
        }
        if (e.token == "log2")
        {
            if(a >= 0)
                 return log2(a);
            else throw  std::runtime_error("Argument is out of the function domen");
        }
        if (e.token == "arcsh") return asinh(a);
        if (e.token == "arcch")
        {
            if(a >= 1)
             return acosh(a);
            else throw std::runtime_error("Argument is out of the function domen");
        }
        if (e.token == "arcth")
        {
            if(fabs(a) < 1)
                 return atanh(a);
            else throw std::runtime_error("Argument is out of the function domen");
        }
        if (e.token == "arccth")
        {
            if(a == 0)
                throw std::runtime_error("Division by zero!");
            if(fabs(a) > 1)
                return atanh(pow(a,-1));
            else throw std::runtime_error("Argument is out of the function domen");
        }
        if (e.token == "arcsech")
        {
            if(a != 0)
                 return asinh(pow(a,-1));
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "arccsech")
        {
            if(a == 0)
               throw std::runtime_error("Division by zero!");
            if(a <= 1)
             return acosh(pow(a,-1));
            else throw std::runtime_error("Argument is out of the function domen");
        }
        if (e.token == "sqrt")
        {
            if(a >= 0)
                 return sqrt(a);
            else throw std::runtime_error("Negative radicand!");
        }
        if (e.token == "sin") return sin(a);
        if (e.token == "cos") return cos(a);
        if (e.token == "tg") return tan(a);
        if (e.token == "ctg")
        {
            if(tan(a) != 0)
                 return 1 / tan(a);
            else throw std::runtime_error("Division by zero!");
        }

        if (e.token == "csecans")
        {
            if(cos(a) != 0)
                   return 1 / cos(a);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "secans")
        {
            if(sin(a) != 0)
                  return 1 / sin(a);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "arcsin")
        {
            if(fabs(a) <= 1)
                   return asin(a);
            else throw std::runtime_error("Argument is out of the function domen");
        }
        if (e.token == "arccos")
        {
            if(fabs(a) <= 1)
                    return acos(a);
            else throw std::runtime_error("Argument is out of the function domen");
        }
        if (e.token == "arctg") return atan(a);
        if (e.token == "arcctg")
        {
            if(a != 0)
                    return atan(1 / a);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "arcsecans")
        {
            if(a != 0)
                    return asin(1 / a);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "arccsecans")
        {
            if(a != 0)
                     return acos(1 / a);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "sh") return sinh(a);
        if (e.token == "ch") return cosh(a);
        if (e.token == "th") return tanh(a);
        if (e.token == "cth")
        {
            if(tanh(a) != 0)
                      return pow(tanh(a),-1);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "sech")
        {
            if(sinh(a) != 0)
                       return pow(sinh(a), -1);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "csech") return 1 / cosh(a);
        {
            if(cosh(a) != 0)
                return pow(cosh(a), -1);
            else throw std::runtime_error("Division by zero!");
        }
        throw std::runtime_error("Unknown unary operator");
    }
    case 0:
        return strtod(e.token.c_str(), nullptr);
    }
    throw std::runtime_error("Unknown expression type");
}
double Parser::calculateExpression()
{
    Expression e = parse();
    return evaluateExpression(e);
}
