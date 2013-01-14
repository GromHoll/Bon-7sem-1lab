#include "calc_logic.h"

#include <cstdlib>
#include <math.h>
#include <queue>
#include <stack>

using namespace std;

const char OPEN_BRACKET = '(';
const char CLOSE_BRACKET = ')';
const char UNARY_MINUS = '~';
string OPERATIONS_LIST = "~^/*+-";

Token::Token(const string& source) {
    value = source;

    if(value.length() == 0) {
        type = UNKNOWN_TYPE;
    } else if(value.length() == 1) {
        char ch = value[0];
        if (ch == OPEN_BRACKET)
            type = OPEN_BRACKET_TYPE;
        else if (ch == CLOSE_BRACKET)
            type = CLOSE_BRACKET_TYPE;
        else if (OPERATIONS_LIST.find(ch) != string::npos)
            type = OPERATION_TYPE;
        else if (ch >= '0' && ch <= '9')
            type = NUMBER_TYPE;
        else if(ch >= 'a' && ch <= 'z')
        	type = VARIABLE_TYPE;
        else
            type = UNKNOWN_TYPE;
    } else {
        bool isVariable = false;
        bool isNumber = false;
        bool isReal = false;

        if(value[0] >= '0' && value[0] <= '9')
            isNumber = true;
        else if(value[0] >= 'a' && value[0] <= 'z')
            isVariable = true;

        for(int i = 1; i < value.length(); i++) {
            if(!isVariable && !isNumber)
                break;

            if(isNumber) {
                if(value[i] == '.')
                    if(isReal)
                        isNumber = false;
                    else
                        isReal = true;
                else if(value[i] < '0' || value[i] > '9')
                    isNumber = false;
            } else {
                if(value[i] < 'a' || value[i] > 'z')
                    isVariable = false;
            }
        }

        if(isNumber)
            type = NUMBER_TYPE;
        else if(isVariable)
            type = VARIABLE_TYPE;
        else
            type = UNKNOWN_TYPE;
    }
}

bool Token::isNumber() {
    return type == NUMBER_TYPE || type == VARIABLE_TYPE;
}
bool Token::isVariable() {
    return type == VARIABLE_TYPE;
}
bool Token::isOperation() {
    return type == OPERATION_TYPE;
}
bool Token::isOpenBracket() {
    return type == OPEN_BRACKET_TYPE;
}
bool Token::isCloseBracket() {
    return type == CLOSE_BRACKET_TYPE;
}
int Token::getPriority() {
    if(isOperation()) {
        switch(value[0]) {
            case '~':   return 4;
            case '^':   return 3;
            case '*':   case '/':   return 2;
            case '-':   case '+':   return 1;
        }
    } else {
        return 0;
    }
}
string Token::getValue() {
    return value;
}

string i2p(const string& exp) {
    string res;

    queue<Token> tokens = divideExp(replaceUnaryMinus(exp));
    stack<Token> operations;

    while(!tokens.empty()) {
        Token t = tokens.front();
        tokens.pop();

        if(t.isNumber()) {
            res += t.getValue() + " ";
        } else if(t.isOpenBracket()) {
            operations.push(t);
        } else if(t.isCloseBracket()) {
            while(!operations.top().isOpenBracket()) {
                t = operations.top();
                operations.pop();

                res += t.getValue() + " ";

                if(operations.empty())
                    return string();
            }
            operations.pop();
        } else if(t.isOperation()) {
            while(!operations.empty()) {
                if(t.getPriority() <= operations.top().getPriority()) {
                    res += operations.top().getValue() + " ";
                    operations.pop();
                } else {
                    break;
                }
            }
            operations.push(t);
        } else
            return string();
    }

    while(!operations.empty()) {
        if(operations.top().isOperation()) {
            res += operations.top().getValue() + " ";
            operations.pop();
        } else
            return string();
    }

    return res;
}
string replaceUnaryMinus(const string& exp) {
    string res = exp;

    if(res.empty())
        return res;

    if(res[0] == '-')
        res[0] = UNARY_MINUS;

    for(int i = 1; i < res.length()-1; i++) {
        if(res[i] == '-' && (OPERATIONS_LIST.find(res[i-1]) != string::npos || res[i-1] == OPEN_BRACKET))
            res[i] = UNARY_MINUS;
    }

    return res;
}
queue<Token> divideExp(const string& exp) {
    queue<Token> res;
    string token;

    for(int i = 0; i < exp.length(); i++) {
        token.clear();
        token += exp[i];

        if(token[0] == OPEN_BRACKET || token[0] == CLOSE_BRACKET
                    || OPERATIONS_LIST.find(token[0]) != string::npos) {
            res.push(Token(token));
        } else {
            while(true) {
                if(i+1 < exp.length()) {
                    if(exp[i+1] == OPEN_BRACKET || exp[i+1] == CLOSE_BRACKET
                                || OPERATIONS_LIST.find(exp[i+1]) != string::npos) {
                        res.push(Token(token));
                        break;
                    } else {
                        i++;
                        token += exp[i];
                    }
                } else {
                    res.push(Token(token));
                    break;
                }
            }
        }
    }

    return res;
}

double getOperand(stack<double> & operands) throw(std::string) {
	double res;

	if(operands.empty())
		throw string("Error in expression: not found operand.");

	res = operands.top();
	operands.pop();

	return res;
}

double p2r(const std::string& exp) throw (std::string) {

	queue<Token> tokens = replaceVaraiable(parse(exp));
	stack<double> operands;
	double opr1, opr2;

	while(!tokens.empty()) {
		if(tokens.front().isNumber()) {
			operands.push(atof(tokens.front().getValue().c_str()));
		} else if(tokens.front().isOperation()) {
			if(tokens.front().getValue().compare("~") != string::npos) {
				opr1 = getOperand(operands);
				operands.push(calc(tokens.front(), opr1));
			} else {
				opr2 = getOperand(operands);
				opr1 = getOperand(operands);
				operands.push(calc(tokens.front(), opr1, opr2));
			}
		} else {
			throw string("Error in expression: unknown token.");
		}
		tokens.pop();
	}

	if(operands.size() != 1)
    	throw string("Error in expression: too much operand.");
    return operands.top();
}
queue<Token> replaceVaraiable(queue<Token> tokens) throw (std::string) {
	queue<Token> newTokens;
	char * pVariable;

	while(!tokens.empty()) {
		if(tokens.front().isVariable()) {
			pVariable = getenv(tokens.front().getValue().c_str());
			if(pVariable != NULL) {
				Token t(pVariable);
				if(t.isNumber())
					newTokens.push(t);
				else {
					throw string("Error in variable: ")
						+ tokens.front().getValue()
						+ string(" is not number.");
				}
			} else {
				throw string("Error in variable: ")
					 + tokens.front().getValue()
					 + string(" not found.");
			}
		} else {
			newTokens.push(tokens.front());
		}
		tokens.pop();
	}

	return newTokens;
}
queue<Token> parse(const std::string& exp) {
	queue<Token> tokens;

	size_t first(0), second(0);

	while( (second = exp.find(' ', first)) != string::npos) {
		tokens.push(Token(exp.substr(first, second - first)));
		first = second + 1;
	}
	if(first < exp.size())
		tokens.push(Token(exp.substr(first, exp.size()- first)));

	return tokens;
}
double calc(Token operation, double opr1, double opr2) throw (std::string) {
	if(!operation.isOperation())
		throw string("Calc error: Token not operation.");

	double res(0);
	switch(operation.getValue()[0]) {
		case '~':
			res = -opr1;
			break;
		case '+':
			res = opr1 + opr2;
			break;
		case '-':
			res = opr1 - opr2;
			break;
		case '*':
			res = opr1 * opr2;
			break;
		case '/':
			if(opr2 == 0)
				throw string("Calc error: divide by zero.");
			res = opr1 / opr2;
			break;
		case '^':
			res = pow(opr1, opr2);
			break;
		default:
			break;
	}
	return res;
}
