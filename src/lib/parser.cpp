#include <iostream>
#include <algorithm>
#include <map>

#include "../include/parser.h"
#include "../include/token.h"

using namespace std;

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
        default:
            res += "UNKNOWN";
    }

    return res;
}

string Token::debug() {
    string res = "";
    if (this->type == TokenType::LIST) {
        res += "( ";
        List* l = static_cast<List*>(this);
        for (size_t i = 0; i < l->list.size(); i++) {
            res += l->list[i]->debug() + " ";
        }
        res += ")";
    } else {
        Atom* a = static_cast<Atom*>(this);
        res += a->str;
    }

    return res;
}

bool isLowercase(char x) {
    return x >= 'a' && x <= 'z';
}

bool isUppercase(char x) {
    return x >= 'A' && x <= 'Z';
}

bool isWhitespace(char x) {
    return x == '\t' || x == '\n' || x == ' ';
}

UnaryOperatorType stringToUnaryOperator(string s) {
    map<string, UnaryOperatorType> valid_operator;
    valid_operator["not"] = UnaryOperatorType::NOT;
    valid_operator["def"] = UnaryOperatorType::DEC;

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

BinaryOperatorType stringToBinaryOperator(string s) {
    map<string, BinaryOperatorType> valid_operator;
    valid_operator["+"] = BinaryOperatorType::PLUS;
    valid_operator["-"] = BinaryOperatorType::MINUS;
    valid_operator["/"] = BinaryOperatorType::DIV;
    valid_operator["*"] = BinaryOperatorType::TIMES;
    valid_operator["and"] = BinaryOperatorType::AND;
    valid_operator["or"] = BinaryOperatorType::OR;
    valid_operator["def"] = BinaryOperatorType::DEF;

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

    auto it = valid_operator.find(s);

    if (it == valid_operator.end()) {
        return false;
    }

    return valid_operator[s];
}

bool isNumber(char s) {
    return s >= '0' && s <= '9';
}

Token* createSingularToken(string s) {
    Token* t = NULL;
    if (isUnaryOperator(s)) {
        t = new UnaryOperator(stringToUnaryOperator(s));
    } else if (isBinaryOperator(s)) {
        cout << s << '\n';
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

Token* Parser::parse() {
    vector<State> state;
    Token* res = NULL;

    vector<Token*> list_stack;
    for (size_t i = 0; i < this->str.length(); i++) {
        List* new_token_list = new List();
        new_token_list->type = TokenType::LIST;
        if (this->str[i] == '(') {
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
        } else if (this->str[i] == ' ') {
            if (state.size() < 1 || state.back() != State::SPACE) {
                this->parse_stack.push_back(this->str[i]);
                state.push_back(State::SPACE);
            }
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

