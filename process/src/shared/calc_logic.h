#ifndef CALC_LOGIG_H_
#define CALC_LOGIG_H_


#include <string>
#include <queue>

class Token {
    private:
        std::string value;
        int type;
    public:
        enum  { NUMBER_TYPE,
            	VARIABLE_TYPE,
                OPERATION_TYPE,
                OPEN_BRACKET_TYPE,
                CLOSE_BRACKET_TYPE,
                UNKNOWN_TYPE };

        Token(const std::string& source);

        bool isNumber();
        bool isVariable();
        bool isOperation();
        bool isOpenBracket();
        bool isCloseBracket();

        int getPriority();

        std::string getValue();
};

std::string i2p(const std::string& exp);
std::string replaceUnaryMinus(const std::string& exp);
std::queue<Token> divideExp(const std::string& exp);

double p2r(const std::string& exp) throw (std::string);
std::queue<Token> replaceVaraiable(std::queue<Token> tokens) throw (std::string);
std::queue<Token> parse(const std::string& exp);
double calc(Token operation, double opr1, double opr2 = 0) throw (std::string);


#endif /* CALC_LOGIG_H_ */
