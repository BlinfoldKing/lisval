#ifndef TOKEN_H
#define TOKEN_H

#include <vector>
#include <string>

using namespace std;

enum class TokenType {
	ATOM,
	LIST,
	NUMBER,
	VARIABLE,
	UNARY_OPERATOR,
	BINARY_OPERATOR,
	LIST_OPERATOR,
	BOOLEAN,
};

enum class UnaryOperatorType {
	NOT,
	DEC,
};

enum class BinaryOperatorType {
	MINUS,
	PLUS,
	TIMES,
	DIV,
	DEF,
	AND,
	OR,
};

class Token {
	public:
		TokenType type;
		string debug();
		string debugToken();
		Token(TokenType t): type(t) {};
};

class UnaryOperator: public Token {
	public:
		UnaryOperatorType operator_type;
		UnaryOperator(UnaryOperatorType t):
			Token(TokenType::UNARY_OPERATOR),
			operator_type(t) {};
};

class BinaryOperator: public Token {
	public:
		BinaryOperatorType operator_type;
		BinaryOperator(BinaryOperatorType t):
			Token(TokenType::BINARY_OPERATOR),
			operator_type(t) {};
};

class Atom: public Token {
	public:
		string str;
		Atom(string s):
			Token(TokenType::ATOM),
			str(s) {};
};

class Variable: public Token {
	public:
		string str;
		Variable(string s):
			Token(TokenType::VARIABLE),
			str(s) {};
};

class List: public Token {
	public:
		vector<Token*> list;
		List(): Token(TokenType::LIST) {};
		List(vector<Token*> l):
			Token(TokenType::LIST),
			list(l) {};
};

class Number: public Token {
	public:
		int value;
		Number(int v):
			Token(TokenType::NUMBER),
			value(v) {};
};

class Boolean: public Token {
	public:
		bool value;
		Boolean(bool v):
			Token(TokenType::BOOLEAN),
			value(v) {};
};

#endif
