#include <vector>
#include <utility>
#include <functional>

#include "token.h"

using namespace std;

class Evaluator {
	private:
        unsigned int max_stack_call = 256;
		vector<Token*> stack_trace;

		vector<pair<Token*, function<Token*(Token*)>>> state;
		// unary
		pair<Token*, function<Token*(Token*)>> dec();
		pair<Token*, function<Token*(Token*)>> not_();

		// binary
		pair<Token*, function<Token*(Token*)>> plus();
		pair<Token*, function<Token*(Token*)>> minus();
		pair<Token*, function<Token*(Token*)>> div();
		pair<Token*, function<Token*(Token*)>> times();
		pair<Token*, function<Token*(Token*)>> and_();
		pair<Token*, function<Token*(Token*)>> or_();
		pair<Token*, function<Token*(Token*)>> def();
		pair<Token*, function<Token*(Token*)>> eq();
		pair<Token*, function<Token*(Token*)>> range();

        // Hof
		pair<Token*, function<Token*(Token*)>> map();
		pair<Token*, function<Token*(Token*)>> filter();
		pair<Token*, function<Token*(Token*)>> reduce();

		// misc
		pair<Token*, function<Token*(Token*)>> definitions();
		pair<Token*, function<Token*(Token*)>> clear();
		pair<Token*, function<Token*(Token*)>> print();

        Token* subtitute_variable(Token* result_pattern, vector<pair<Variable*, Token*>> arguments);

        vector<pair<Variable*, Token*>>
            generateVariableArgument(Token* condition, Token* input) ;
	public:
		Evaluator();
		Token* eval(Token* token);
};
