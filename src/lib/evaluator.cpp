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
                Token* t = new Error("expect a boolean, got:" + pattern_token->debug(), input);
                return t;
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
                Token* t = new Error("expect a number, got: " + a_token->debug(), input);
                return t;
            }

            if (b_token->type != TokenType::NUMBER) {
                Token* t = new Error("expect a number, got: " + b_token->debug(), input);
                return t;
            }

            Number* a = static_cast<Number*>(a_token);
            Number* b = static_cast<Number*>(b_token);

            Token* res = new Number(a->value + b->value);
            return res;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::eq() {
    Token* op = new BinaryOperator(BinaryOperatorType::EQ);
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

            if (a_token->type != b_token->type) {
                if (a_token->type == TokenType::VARIABLE
                        || b_token->type == TokenType::VARIABLE) {
                    Token* t = new Error("cannot check equality of variable", input);
                    return t;
                }
            }

            if (a_token->type == TokenType::NUMBER) {
                Number* a = static_cast<Number*>(a_token);
                Number* b = static_cast<Number*>(b_token);

                Token* t = new Boolean(a->value == b->value);
                return t;
            }

            if (a_token->type == TokenType::ATOM) {
                Atom* a = static_cast<Atom*>(a_token);
                Atom* b = static_cast<Atom*>(b_token);

                Token* t = new Boolean(a->str == b->str);
                return t;
            }

            if (a_token->type == TokenType::BOOLEAN) {
                Boolean* a = static_cast<Boolean*>(a_token);
                Boolean* b = static_cast<Boolean*>(b_token);

                Token* t = new Boolean(a->value == b->value);
                return t;
            }

            Token* t = new Error("unsupported equality", input);
            return t;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::range() {
    Token* op = new Atom("range");
    Token* var1 = new Variable("MIN");
    Token* var2 = new Variable("MAX");
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
                Token* t = new Error("expect a number, got: " + a_token->debug(), input);
                return t;
            }

            if (b_token->type != TokenType::NUMBER) {
                Token* t = new Error("expect a number, got: " + b_token->debug(), input);
                return t;
            }

            Number* min = static_cast<Number*>(a_token);
            Number* max = static_cast<Number*>(b_token);

            List* result = new List();
            for (int i = min->value; i < max->value; i++) {
                result->list.push_back(new Number(i));
            }

            Token* t = result;
            return t;
    });

    return res;
}

Token* Evaluator::subtitute_variable(Token* result_template, vector<pair<Variable*, Token*>> arguments) {
    Token* token = result_template;
    if (token->type == TokenType::VARIABLE) {
        Variable* var = static_cast<Variable*>(token);
        auto it = find_if(
                arguments.begin(), arguments.end(), [var](pair<Variable*, Token*> curr) {
                return curr.first->str == var->str;
        });

        if (it == arguments.end()) {
            return this->eval(token);
        }

        return it->second;
    } else if (token->type == TokenType::LIST) {
        List* l = static_cast<List*>(token);
        List* result_list = new List();
        for (size_t i = 0; i < l->list.size(); i++) {
            auto curr = l->list[i];
            if (curr->type == TokenType::VARIABLE || curr->type == TokenType::LIST) {
                Token* t = subtitute_variable(curr, arguments);
                result_list->list.push_back(this->eval(t));
            } else {
                result_list->list.push_back(this->eval(curr));
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

            auto new_rule = make_pair(condition_token, [condition_token, result_token, this](Token* raw_input) {
                    vector<pair<Variable*, Token*>> arguments;

                    Token* input = NULL;
                    if (raw_input->type != TokenType::LIST) {
                        input = raw_input;
                    } else {
                        List* input_list = static_cast<List*>(raw_input);
                        List* res = new List();

                        for (size_t i = 0; i < input_list->list.size(); i++) {
                            res->list.push_back(this->eval(input_list->list[i]));
                        }

                        input = res;
                    }


                    if (condition_token->type == TokenType::VARIABLE) {
                        if (input->type == TokenType::VARIABLE) {
                            Token* t = new Error("cannot subtitute variable with variable", input);
                            return t;
                        }
                        Variable* var = static_cast<Variable*>(condition_token);
                        arguments.push_back(make_pair(var, input));
                    } else if (condition_token->type == TokenType::LIST) {
                        List* params = static_cast<List*>(condition_token);
                        List* args = static_cast<List*>(input);

                        for (size_t i = 0; i < params->list.size(); i++) {
                            if (params->list[i]->type == TokenType::VARIABLE) {
                                if (args->list[i]->type == TokenType::VARIABLE) {
                                    Token* t = new Error("cannot subtitute variable with variable", input);
                                    return t;
                                }
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
                Token* t = new Error("expect a number, got: " + a_token->debug(), input);
                return t;
            }

            if (b_token->type != TokenType::NUMBER) {
                Token* t = new Error("expect a number, got: " + b_token->debug(), input);
                return t;
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
                Token* t = new Error("expect a number, got: " + a_token->debug(), input);
                return t;
            }

            if (b_token->type != TokenType::NUMBER) {
                Token* t = new Error("expect a number, got: " + b_token->debug(), input);
                return t;
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

            Token* a_token = this->eval(list->list[1]);
            Token* b_token = this->eval(list->list[2]);

            if (a_token->type != TokenType::NUMBER) {
                Token* t = new Error("expect a number, got: " + a_token->debug(), input);
                return t;
            }

            if (b_token->type != TokenType::NUMBER) {
                Token* t = new Error("expect a number, got: " + b_token->debug(), input);
                return t;
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
                Token* t = new Error("expect a boolean, got: " + a_token->debug() , input);
                return t;
            }

            if (b_token->type != TokenType::BOOLEAN) {
                Token* t = new Error("expect a boolean, got: " + a_token->debug(), input);
                return t;
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
                Token* t = new Error("expect a boolean, got: " + a_token->debug(), input);
                return t;
            }

            if (b_token->type != TokenType::BOOLEAN) {
                Token* t = new Error("expect a boolean, got: " + b_token->debug(), input);
                return t;
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

            return nullptr;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::print() {
    Token* op = new Atom("print");
    Token* input = new Variable("INPUT");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(input);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* input_list = static_cast<List*>(input);
            Token* res = this->eval(input_list->list[1]);
            cout << res->debug() << '\n';

            return res;
    });

    return res;
}

vector<pair<Variable*, Token*>> Evaluator::generateVariableArgument(Token* condition, Token* input) {
    vector<pair<Variable*, Token*>> arguments;
    if (condition->type == TokenType::VARIABLE) {
        Variable* var = static_cast<Variable*>(condition);
        arguments.push_back(make_pair(var, input));
    } else if (condition->type == TokenType::LIST) {
        List* params = static_cast<List*>(condition);
        List* args = static_cast<List*>(input);

        for (size_t i = 0; i < params->list.size(); i++) {
            if (params->list[i]->type == TokenType::VARIABLE) {
                Variable* var = static_cast<Variable*>(params->list[i]);
                arguments.push_back(make_pair(var, args->list[i]));
            }
        }
    }

    return arguments;
}

pair<Token*, function<Token*(Token*)>> Evaluator::map() {
    Token* op = new Atom("map");
    Token* input_pattern = new Variable("INPUT_PATTERN");
    Token* result_pattern = new Variable("RESULT_PATTERN");
    Token* list_input = new Variable("LIST_INPUT");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(input_pattern);
    signature->list.push_back(result_pattern);
    signature->list.push_back(list_input);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* res = new List();
            Token* t = NULL;

            List* input_list = static_cast<List*>(input);

            Token* input_pattern = input_list->list[1];
            Token* result_pattern = input_list->list[2];
            if (result_pattern->type == TokenType::ERROR) {
                Error* err = static_cast<Error*>(input_list->list[2]);
                result_pattern = err->unwrap();
            }

            Token* list_token = this->eval(input_list->list[3]);
            if (list_token->type != TokenType::LIST) {
                t = new Error("expected a list, got:" + list_token->debug(), input);
                return t;
            }

            List* subtitute_list = static_cast<List*>(list_token);

            for (size_t i = 0; i < subtitute_list->list.size(); i++) {
                auto args = generateVariableArgument(input_pattern, subtitute_list->list[i]);
                Token* output = subtitute_variable(result_pattern, args);

                res->list.push_back(output);
            }

            t = this->eval(res);
            return t;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::filter() {
    Token* op = new Atom("filter");
    Token* input_pattern = new Variable("INPUT_PATTERN");
    Token* result_pattern = new Variable("RESULT_PATTERN");
    Token* list_input = new Variable("LIST_INPUT");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(input_pattern);
    signature->list.push_back(result_pattern);
    signature->list.push_back(list_input);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            List* res = new List();
            Token* t = NULL;

            List* input_list = static_cast<List*>(input);

            Token* input_pattern = input_list->list[1];
            Token* result_pattern = input_list->list[2];
            if (result_pattern->type == TokenType::ERROR) {
                Error* err = static_cast<Error*>(input_list->list[2]);
                result_pattern = err->unwrap();
            }

            Token* list_token = input_list->list[3];
            if (list_token->type != TokenType::LIST) {
                t = new Error("expected a list, got:" + list_token->debug(), input);
                return t;
            }

            List* subtitute_list = static_cast<List*>(list_token);

            for (size_t i = 0; i < subtitute_list->list.size(); i++) {
                auto args = generateVariableArgument(input_pattern, subtitute_list->list[i]);

                Token* output = subtitute_variable(result_pattern, args);
                Token* filter = this->eval(output);
                if (filter->type == TokenType::BOOLEAN) {
                    Boolean* b = static_cast<Boolean*>(filter);
                    if (b->value == true) {
                        res->list.push_back(subtitute_list->list[i]);
                    }
                }
            }

            t = this->eval(res);
            return t;
    });

    return res;
}

pair<Token*, function<Token*(Token*)>> Evaluator::reduce() {
    Token* op = new Atom("reduce");
    Token* accumulator = new Variable("ACCUMULATOR_PATTERN");
    Token* input_pattern = new Variable("INPUT_PATTERN");
    Token* result_pattern = new Variable("RESULT_PATTERN");
    Token* list_input = new Variable("LIST_INPUT");
    List* signature = new List();
    signature->list.push_back(op);
    signature->list.push_back(accumulator);
    signature->list.push_back(input_pattern);
    signature->list.push_back(result_pattern);
    signature->list.push_back(list_input);
    Token* token = signature;
    auto res = make_pair(token, [this](Token* input) {
            Token* t = NULL;

            List* input_list = static_cast<List*>(input);

            Token* accumulator_pattern = input_list->list[1];
            Token* input_pattern = input_list->list[2];
            Token* result_pattern = input_list->list[3];
            if (result_pattern->type == TokenType::ERROR) {
                Error* err = static_cast<Error*>(input_list->list[3]);
                result_pattern = err->unwrap();
            }
            Token* list_token = this->eval(input_list->list[4]);

            if (list_token->type != TokenType::LIST) {
                t = new Error("expected a list, got:" + list_token->debug(), input);
                return t;
            }

            List* subtitute_list = static_cast<List*>(list_token);

            if (subtitute_list->list.size() < 1) {
                t = new Error("insufficient list size", input);
                return t;
            }

            Token* accumulator = subtitute_list->list[0];
            Variable* accumulator_var = static_cast<Variable*>(accumulator_pattern);

            for (size_t i = 1; i < subtitute_list->list.size(); i++) {
                auto args = generateVariableArgument(input_pattern, subtitute_list->list[i]);
                args.push_back(make_pair(accumulator_var, accumulator));

                Token* output = subtitute_variable(result_pattern, args);
                accumulator = this->eval(output);
            }

            t = this->eval(accumulator);
            return t;
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
    state.push_back(and_());
    state.push_back(or_());
    state.push_back(map());
    state.push_back(filter());
    state.push_back(reduce());
    state.push_back(eq());
    state.push_back(range());
    state.push_back(def());
    state.push_back(definitions());
    state.push_back(print());
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

bool compareSignature(Token* input, Token* signature) {
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
}

Token* Evaluator::eval(Token* input) {
    if (input->type == TokenType::LIST) {
        List* l = static_cast<List*>(input);
        List* res = new List();
        for (size_t i = 0; i < l->list.size(); i++) {
            if (this->stack_trace.size() < this->max_stack_call) {
                this->stack_trace.push_back(l->list[i]);
                Token* output = this->eval(l->list[i]);
                this->stack_trace.pop_back();
                if (output && output->type != TokenType::ERROR) {
                    res->list.push_back(output);
                } else {
                    res->list.push_back(l->list[i]);
                }
            }

        }

        input = res;
    }


    vector<pair<Token*, function<Token*(Token*)>>> matches_handler;
    for (size_t i = 0; i < this->state.size(); i++) {
        bool compare = compareSignature(input, this->state[i].first);
        if (compare) {
            matches_handler.push_back(this->state[i]);
        }
    }

    Error* err = NULL;
    for (size_t i = 0; i < matches_handler.size(); i++) {
        if (this->stack_trace.size() < this->max_stack_call) {
            this->stack_trace.push_back(input);
            Token* res = matches_handler[i].second(input);
            this->stack_trace.pop_back();

            if (res) {
                if (res->type != TokenType::ERROR) {
                    return res;
                } else {
                    err = static_cast<Error*>(res);
                }
            }
        } else {
            return new Error("stack call limit exceeded", input);
        }
    }

    if (err) {
        return err;
    };

    return input;
}
