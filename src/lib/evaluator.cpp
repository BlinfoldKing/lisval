#include "../include/evaluator.h"
#include "../include/token.h"

#include <algorithm>
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

void clrscr () {
    system (CLEAR);
}

pair<Token*, function<Token*(Token*)>> Evaluator::dec() {
    Token* op = new UnaryOperator(UnaryOperatorType::DEC);
    Token* var1 = new Variable("Pattern");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(var1);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* list = static_cast<List*>(input);

            Token* new_signature = this->eval(list->list[1]);

            auto new_rule = make_pair(new_signature, [](Token* input) {
                        Token* res = new Boolean(true);
                        return res;
            });

            this->state.push_back(new_rule);

            Token* res = new Boolean(true);
            return res;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::not_() {
    Token* op = new UnaryOperator(UnaryOperatorType::NOT);
    Token* var1 = new Variable("Pattern");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(var1);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* list = static_cast<List*>(input);

            Token* pattern_token = this->eval(list->list[1]);
            if (pattern_token->type != TokenType::BOOLEAN) {
                return input;
            }

            Boolean* pattern = static_cast<Boolean*>(pattern_token);


            Token* res = new Boolean(!pattern->value);
            return res;
    });

    return res;
}


pair<Token*, function<Token*(Token*)>> Evaluator::plus() {
    Token* op = new BinaryOperator(BinaryOperatorType::PLUS);
    Token* var1 = new Variable("L");
    Token* var2 = new Variable("R");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(var1);
    signature->list.push_back(var2);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* list = static_cast<List*>(input);

            Token* a_token = this->eval(list->list[1]);
            Token* b_token = this->eval(list->list[2]);

            if (a_token->type != TokenType::NUMBER) {
                return input;
            }

            if (b_token->type != TokenType::NUMBER) {
                return input;
            }

            Number* a = static_cast<Number*>(a_token);
            Number* b = static_cast<Number*>(b_token);

            Token* res = new Number(a->value + b->value);
            return res;
    });

    return res;
}

Token* subtitute_variable(Token* result_template, vector<pair<Variable*, Token*>> arguments) {
    Token* token = result_template;
    if (token->type == TokenType::VARIABLE) {
        Variable* var = static_cast<Variable*>(token);
        auto it = find_if(
                arguments.begin(), arguments.end(), [var](pair<Variable*, Token*> curr) {
                return curr.first->str == var->str;
        });

        if (it == arguments.end()) {
            return token;
        }

        return it->second;
    } else if (token->type == TokenType::LIST) {
        List* l = static_cast<List*>(token);
        List* result_list = new List();
        for (size_t i = 0; i < l->list.size(); i++) {
            auto curr = l->list[i];
            if (curr->type == TokenType::VARIABLE || curr->type == TokenType::LIST) {
                Token* t = subtitute_variable(curr, arguments);
                result_list->list.push_back(t);
            } else {
                result_list->list.push_back(curr);
            }
        }

        return result_list;
    }

    return token;

}

pair<Token*, function<Token*(Token*)>> Evaluator::def() {
    Token* op = new BinaryOperator(BinaryOperatorType::DEF);
    Token* condition = new Variable("CONDITION");
    Token* result = new Variable("RESULT");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(condition);
    signature->list.push_back(result);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* list = static_cast<List*>(input);

            Token* condition_token = list->list[1];
            Token* result_token = list->list[2];

            auto new_rule = make_pair(condition_token, [condition_token, result_token, this](Token* input) {
                    vector<pair<Variable*, Token*>> arguments;
                    if (condition_token->type == TokenType::VARIABLE) {
                        Variable* var = static_cast<Variable*>(condition_token);
                        arguments.push_back(make_pair(var, input));
                    } else if (condition_token->type == TokenType::LIST) {
                        List* params = static_cast<List*>(condition_token);
                        List* args = static_cast<List*>(input);

                        for (size_t i = 0; i < params->list.size(); i++) {
                            if (params->list[i]->type == TokenType::VARIABLE) {
                                Variable* var = static_cast<Variable*>(params->list[i]);
                                arguments.push_back(make_pair(var, args->list[i]));
                            }
                        }
                    } else {
                        // if there are no variable at all then there's nothing
                        // to subtitute
                        return result_token;
                    }

                    Token* result_list = subtitute_variable(result_token, arguments);
                    cout << result_list->debug() << '\n';
                    Token* res = this->eval(result_list);
                    return res;
            });

            this->state.push_back(new_rule);

            Token* res = new Boolean(true);
            return res;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::times() {
    Token* op = new BinaryOperator(BinaryOperatorType::TIMES);
    Token* var1 = new Variable("L");
    Token* var2 = new Variable("R");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(var1);
    signature->list.push_back(var2);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* list = static_cast<List*>(input);

            Token* a_token = this->eval(list->list[1]);
            Token* b_token = this->eval(list->list[2]);

            if (a_token->type != TokenType::NUMBER) {
                return input;
            }

            if (b_token->type != TokenType::NUMBER) {
                return input;
            }

            Number* a = static_cast<Number*>(a_token);
            Number* b = static_cast<Number*>(b_token);

            Token* res = new Number(a->value * b->value);
            return res;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::div() {
    Token* op = new BinaryOperator(BinaryOperatorType::DIV);
    Token* var1 = new Variable("L");
    Token* var2 = new Variable("R");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(var1);
    signature->list.push_back(var2);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* list = static_cast<List*>(input);

            Token* a_token = this->eval(list->list[1]);
            Token* b_token = this->eval(list->list[2]);

            if (a_token->type != TokenType::NUMBER) {
                return input;
            }

            if (b_token->type != TokenType::NUMBER) {
                return input;
            }

            Number* a = static_cast<Number*>(a_token);
            Number* b = static_cast<Number*>(b_token);

            Token* res = new Number(a->value / b->value);
            return res;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::minus() {
    Token* op = new BinaryOperator(BinaryOperatorType::MINUS);
    Token* var1 = new Variable("L");
    Token* var2 = new Variable("R");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(var1);
    signature->list.push_back(var2);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* list = static_cast<List*>(input);

            cout << input->debug() << '\n';
            Token* a_token = this->eval(list->list[1]);
            Token* b_token = this->eval(list->list[2]);

            cout << a_token->debug() << '\n';
            if (a_token->type != TokenType::NUMBER) {
                return input;
            }

            if (b_token->type != TokenType::NUMBER) {
                return input;
            }

            Number* a = static_cast<Number*>(a_token);
            Number* b = static_cast<Number*>(b_token);

            Token* res = new Number(a->value - b->value);
            return res;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::clear() {
    Token* op = new Atom("clear");
    List* signature = new List();
    signature->list.push_back(op);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            clrscr();
           return nullptr;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::or_() {
    Token* op = new BinaryOperator(BinaryOperatorType::OR);
    Token* var1 = new Variable("L");
    Token* var2 = new Variable("R");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(var1);
    signature->list.push_back(var2);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* list = static_cast<List*>(input);

            Token* a_token = this->eval(list->list[1]);
            Token* b_token = this->eval(list->list[2]);

            if (a_token->type != TokenType::BOOLEAN) {
                return input;
            }

            if (b_token->type != TokenType::BOOLEAN) {
                return input;
            }

            Boolean* a = static_cast<Boolean*>(a_token);
            Boolean* b = static_cast<Boolean*>(b_token);

            Token* res = new Boolean(a->value || b->value);
            return res;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::and_() {
    Token* op = new BinaryOperator(BinaryOperatorType::AND);
    Token* var1 = new Variable("L");
    Token* var2 = new Variable("R");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(var1);
    signature->list.push_back(var2);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* list = static_cast<List*>(input);

            Token* a_token = this->eval(list->list[1]);
            Token* b_token = this->eval(list->list[2]);

            if (a_token->type != TokenType::BOOLEAN) {
                return input;
            }

            if (b_token->type != TokenType::BOOLEAN) {
                return input;
            }

            Boolean* a = static_cast<Boolean*>(a_token);
            Boolean* b = static_cast<Boolean*>(b_token);

            Token* res = new Boolean(a->value && b->value);
            return res;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::definitions() {
    Token* op = new Atom("definitions");
    List* signature = new List();
    signature->list.push_back(op);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            for (size_t i = 0; i < this->state.size(); i++) {
                cout << this->state[i].first->debug() << '\n';
            }
            Token* res = new Boolean(true);
            return res;
    });

    return res;
}

Evaluator::Evaluator() {
    vector<pair<Token*, function<Token*(Token*)>>> state;

    state.push_back(dec());
    state.push_back(not_());

    state.push_back(plus());
    state.push_back(minus());
    state.push_back(times());
    state.push_back(div());
    state.push_back(or_());
    state.push_back(and_());
    state.push_back(def());

    state.push_back(definitions());
    state.push_back(clear());

    this->state = state;
}

bool compareList(List* input, List* signature) {
    if (input->list.size() != signature->list.size()) {
        return false;
    }

    for (size_t i = 0; i < input->list.size(); i++) {
        if (input->list[i]->type != signature->list[i]->type) {
            if (signature->list[i]->type != TokenType::VARIABLE) {
               return false;
            }

            continue;
        } else {
            if (input->list[i]->type == TokenType::NUMBER) {
                Number* inp = static_cast<Number*>(input->list[i]);
                Number* sig = static_cast<Number*>(signature->list[i]);

                if (inp->value != sig->value) {
                    return false;
                }
            }

            if (input->list[i]->type == TokenType::BOOLEAN) {
                Boolean* inp = static_cast<Boolean*>(input->list[i]);
                Boolean* sig = static_cast<Boolean*>(signature->list[i]);

                if (inp->value != sig->value) {
                    return false;
                }
            }

            if (input->list[i]->type == TokenType::ATOM) {
                Atom* inp = static_cast<Atom*>(input->list[i]);
                Atom* sig = static_cast<Atom*>(signature->list[i]);

                if (inp->str != sig->str) {
                    return false;
                }
            }

            if (input->list[i]->type == TokenType::LIST) {
                List* inp = static_cast<List*>(input->list[i]);
                List* sig = static_cast<List*>(signature->list[i]);

                if (!compareList(inp, sig)) {
                    return false;
                }
            }

            if (input->list[i]->type == TokenType::UNARY_OPERATOR) {
                UnaryOperator* inp = static_cast<UnaryOperator*>(input->list[i]);
                UnaryOperator* sig = static_cast<UnaryOperator*>(signature->list[i]);

                if (inp->operator_type != sig->operator_type) {
                    return false;
                }
            }

            if (input->list[i]->type == TokenType::BINARY_OPERATOR) {
                BinaryOperator* inp = static_cast<BinaryOperator*>(input->list[i]);
                BinaryOperator* sig = static_cast<BinaryOperator*>(signature->list[i]);

                if (inp->operator_type != sig->operator_type) {
                    return false;
                }
            }
        }
    }


    return true;
}

Token* Evaluator::eval(Token* input) {
    auto it = find_if(state.begin(), state.end(), [input](pair<Token*, function<Token*(Token*)>> handler) {
        auto signature = handler.first;

        if (signature->type == TokenType::VARIABLE) {
            return input->type != TokenType::VARIABLE;
        }

        if (input->type != signature->type) {
            return false;
        }

        if (input->type == TokenType::LIST) {
            List* input_list = static_cast<List*>(input);
            List* signature_list = static_cast<List*>(signature);

            return compareList(input_list, signature_list);
        }

        if (input->type == TokenType::ATOM) {
            Atom* i = static_cast<Atom*>(input);
            Atom* s = static_cast<Atom*>(signature);
            return i->str == s->str;
        }

        if (input->type == TokenType::VARIABLE) {
            Variable* i = static_cast<Variable*>(input);
            Variable* s = static_cast<Variable*>(signature);
            return i->str == s->str;
        }

        if (input->type == TokenType::NUMBER) {
            Number* i = static_cast<Number*>(input);
            Number* s = static_cast<Number*>(signature);
            return i->value == s->value;
        }

        if (input->type == TokenType::BOOLEAN) {
            Boolean* i = static_cast<Boolean*>(input);
            Boolean* s = static_cast<Boolean*>(signature);
            return i->value == s->value;
        }

        return false;
    });

    if (it == state.end()) {
        if (input->type == TokenType::LIST) {
            List* input_list = static_cast<List*>(input);
            List* result_list = new List();

            for (size_t i = 0; i < input_list->list.size(); i++) {
                Token* newItem = this->eval(input_list->list[i]);
                result_list->list.push_back(newItem);
            }

            return result_list;
        }
        return input;
    }

    return it->second(input);
}
