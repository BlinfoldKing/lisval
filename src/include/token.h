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
	ERROR,
	TYPE,
};

enum class TypeType {
	ATOM,
	LIST,
	NUMBER,
	BOOLEAN,
	ERR,
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
	EQ,
};

class Token {
	public:
		TokenType type;
		string debug();
		string debugToken();
		Token(TokenType t): type(t) {};
};

class Type: public Token {
	public:
		TypeType type_type;
		Token* value;
		Type(TypeType t, Token* v):
			Token(TokenType::TYPE),
			value(v) {};
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
		float value;
		Number(float v):
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

class Error: public Token {
    public:
        string message;
        Token* input;
        Error(string m, Token* i):
            Token(TokenType::ERROR),
            message(m), input(i) {};
		Token* unwrap() {return input;};
};

#endif
