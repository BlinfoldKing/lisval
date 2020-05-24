#include <vector>
#include <utility>
#include <functional>

#include "token.h"

using namespace std;

class Evaluator {
	private:
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

		// misc
		pair<Token*, function<Token*(Token*)>> definitions();
		pair<Token*, function<Token*(Token*)>> clear();
	public:
		Evaluator();
		Token* eval(Token* token);
};
