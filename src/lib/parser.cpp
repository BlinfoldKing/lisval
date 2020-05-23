#include "../include/parser.h"
#include <iostream>
#include<algorithm>

using namespace std;

string Token::debug() {
    string res = "";
    if (this->type == TokenType::ATOM) {
        res += this->atom;
    } else {
        res += "( ";
        for (size_t i = 0; i < this->list.size(); i++) {
            res += this->list[i]->debug() + " ";
        }
        res += ")";
    }

    return res;
}

Parser::Parser(const string& str) {
    this->str = str;
}

Token* Parser::parse() {
    vector<State> state;
    Token* res = NULL;

    vector<Token*> list_stack;
    for (size_t i = 0; i < this->str.length(); i++) {
        Token* new_token_list = new Token;
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
                        Token* new_token = new Token;
                        new_token->type = TokenType::ATOM;
                        new_token->atom = newItem;
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
                Token* new_token = new Token;
                new_token->type = TokenType::ATOM;
                new_token->atom = newItem;
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

