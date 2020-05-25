#include <iostream>
#include <algorithm>
#include <map>

#include "../include/parser.h"
#include "../include/token.h"

using namespace std;



bool isLowercase(char x) {
	return x >= 'a' && x <= 'z';
}

bool isUppercase(char x) {
	return x >= 'A' && x <= 'Z';
}

bool isWhitespace(char x) {
	return x == '\t' || x == '\n' || x == ' ';
}

string unaryOperatorToString(UnaryOperatorType type) {
	map<UnaryOperatorType, string> valid_operator;
	valid_operator[UnaryOperatorType::NOT] = "not";
	valid_operator[UnaryOperatorType::DEC] = "dec";

	return valid_operator[type];
}

UnaryOperatorType stringToUnaryOperator(string s) {
	map<string, UnaryOperatorType> valid_operator;
	valid_operator["not"] = UnaryOperatorType::NOT;
	valid_operator["dec"] = UnaryOperatorType::DEC;

	return valid_operator[s];
}

bool isUnaryOperator(string s) {
	map<string, bool> valid_operator;
	valid_operator["not"] = true;
	valid_operator["dec"] = true;

	auto it = valid_operator.find(s);
	if (it == valid_operator.end()) {
		return false;
	}

	return valid_operator[s];
}

string binaryOperatorToString(BinaryOperatorType type) {
	map<BinaryOperatorType, string> valid_operator;
	valid_operator[BinaryOperatorType::PLUS] = "+";
	valid_operator[BinaryOperatorType::MINUS] = "-";
	valid_operator[BinaryOperatorType::DIV] = "/";
	valid_operator[BinaryOperatorType::TIMES] = "*";
	valid_operator[BinaryOperatorType::AND] = "and";
	valid_operator[BinaryOperatorType::OR] = "or";
	valid_operator[BinaryOperatorType::DEF] = "def";
	valid_operator[BinaryOperatorType::EQ] = "=";

	return valid_operator[type];
}

BinaryOperatorType stringToBinaryOperator(string s) {
	map<string, BinaryOperatorType> valid_operator;
	valid_operator["+"] = BinaryOperatorType::PLUS;
	valid_operator["-"] = BinaryOperatorType::MINUS;
	valid_operator["/"] = BinaryOperatorType::DIV;
	valid_operator["*"] = BinaryOperatorType::TIMES;
	valid_operator["and"] = BinaryOperatorType::AND;
	valid_operator["or"] = BinaryOperatorType::OR;
	valid_operator["def"] = BinaryOperatorType::DEF;
	valid_operator["="] = BinaryOperatorType::EQ;

	return valid_operator[s];
}

bool isBinaryOperator(string s) {
	map<string, bool> valid_operator;
	valid_operator["+"] = true;
	valid_operator["-"] = true;
	valid_operator["/"] = true;
	valid_operator["*"] = true;
	valid_operator["and"] = true;
	valid_operator["or"] = true;
	valid_operator["def"] = true;
	valid_operator["="] = true;

	auto it = valid_operator.find(s);

	if (it == valid_operator.end()) {
		return false;
	}

	return valid_operator[s];
}

bool isNumber(char s) {
	return s >= '0' && s <= '9';
}

bool stringToBool(string s) {
	return s == "true";
}

string boolToString(bool s) {
	if (s) return "true";
	return "false";
}

Token* createSingularToken(string s) {
	Token* t = NULL;
	if (s == "true" || s == "false") {
		t = new Boolean(stringToBool(s));
	} else if (isUnaryOperator(s)) {
		t = new UnaryOperator(stringToUnaryOperator(s));
	} else if (isBinaryOperator(s)) {
		t = new BinaryOperator(stringToBinaryOperator(s));
	} else if (isLowercase(s[0])) {
		t = new Atom(s);
	} else if (isUppercase(s[0])) {
		t = new Variable(s);
	} else if (isNumber(s[0])) {
		int x = (int) s[0] - '0';
		for (size_t i = 1; i < s.length() && isNumber(s[i]); i++)  {
			x *= 10;
			x += (int) s[i] - '0';
		}

		t = new Number(x);
	}

	return t;
}

void debugState(vector<State> state) {
    for (size_t i = 0; i < state.size(); i++) {
        string res = "";
        switch (state[i]) {
            case State::LIST_START:
                res = "B";
                break;
            case State::CHAR:
                res = "C";
                break;
            case State::LIST_TOKEN:
                res = "L";
                break;
            case State::SPACE:
                res = "S";
                break;
            case State::COMMENT:
                res = "#";
                break;
        }

        cout << res;
    }

    cout << '\n';
}

Token* Parser::parse() {
	vector<State> state;
	Token* res = NULL;

	vector<Token*> list_stack;
	for (size_t i = 0; i < this->str.length(); i++) {
        /* debugState(state); */
		List* new_token_list = new List();
		new_token_list->type = TokenType::LIST;

        if (state.size() > 0 && state.back() == State::COMMENT) {
            if (this->str[i] == '\n') state.pop_back();
        } else if(this->str[i] == '#') {
            state.push_back(State::COMMENT);
        } else if (this->str[i] == '(') {
			state.push_back(State::LIST_START);
		} else if (this->str[i] == ')') {
			string newItem = "";
			while(state.size() && state.back() != State::LIST_START) {
				if (state.back() == State::CHAR) {
					newItem = this->parse_stack.back() + newItem;
					this->parse_stack.pop_back();
				} else {
					if (newItem != "") {
						Token* new_token = createSingularToken(newItem);
						new_token_list->list.push_back(new_token);

						newItem = "";
					}

					if (state.back() == State::LIST_TOKEN) {
						new_token_list->list.push_back(list_stack.back());
						list_stack.pop_back();
					} else {
						this->parse_stack.pop_back();
					}
				}

				state.pop_back();
			}

			if (newItem != "") {
				Token* new_token = createSingularToken(newItem);
				new_token_list->list.push_back(new_token);
			}

			reverse(new_token_list->list.begin(), new_token_list->list.end());

			state.pop_back();
			state.push_back(State::LIST_TOKEN);
			list_stack.push_back(new_token_list);
		} else if (this->str[i] == ' ' || this->str[i] == '\t') {
            if (state.size() < 1 || state.back() != State::SPACE) {
				this->parse_stack.push_back(this->str[i]);
				state.push_back(State::SPACE);
			}
		} else if (this->str[i] == '\n') {
            continue;
        } else {
			state.push_back(State::CHAR);
			this->parse_stack.push_back(this->str[i]);
		}

	}

	if (state.size() > 0 && state.back() == State::LIST_TOKEN) {
		res = list_stack.back();
		list_stack.pop_back();
		state.pop_back();
	}

	return res;
}

string Token::debugToken() {
	string res = "";
	switch (this->type) {
		case TokenType::ATOM:
			res += "ATOM";
			break;
		case TokenType::LIST:
			{
				res += "LIST{ ";
				List* l = static_cast<List*>(this);
				for (size_t i = 0; i < l->list.size(); i++) {
					res += l->list[i]->debugToken() + " ";
				}
				res += "}";
				break;
			}
		case TokenType::NUMBER:
			res += "NUMBER";
			break;
		case TokenType::VARIABLE:
			res += "VARIABLE";
			break;
		case TokenType::UNARY_OPERATOR:
			res += "UNARY_OPERATOR";
			break;
		case TokenType::BINARY_OPERATOR:
			res += "BINARY_OPERATOR";
			break;
		case TokenType::BOOLEAN:
			res += "BOOLEAN";
			break;
		default:
			res += "UNKNOWN";
	}

	return res;
}

string Token::debug() {
	string res = "";
	switch (this->type) {
		case TokenType::ATOM:
		{
			Atom* a = static_cast<Atom*>(this);
			res += a->str;
			break;
		}
		case TokenType::LIST:
			{
				res += "(";
				List* l = static_cast<List*>(this);
				for (size_t i = 0; i < l->list.size(); i++) {
					res += l->list[i]->debug();
					if (i < l->list.size() - 1) res += ' ';
				}
				res += ")";
				break;
			}
		case TokenType::NUMBER:
			{
				Number* a = static_cast<Number*>(this);
				res.push_back(a->value + '0');
				break;
			}
		case TokenType::VARIABLE:
		{
			Variable* a = static_cast<Variable*>(this);
			res += a->str;
			break;
		}
		case TokenType::UNARY_OPERATOR:
		{
			UnaryOperator* a = static_cast<UnaryOperator*>(this);
			res += unaryOperatorToString(a->operator_type);
			break;
		}
		case TokenType::BINARY_OPERATOR:
		{
			BinaryOperator* a = static_cast<BinaryOperator*>(this);
			res += binaryOperatorToString(a->operator_type);
			break;
		}
		case TokenType::BOOLEAN:
		{
			Boolean* a = static_cast<Boolean*>(this);
			res += boolToString(a->value);
			break;
		}
		default:
			res += "UNKNOWN";
	}

	return res;

}
