#include <vector>

#include "token.h"

using namespace std;

enum State {
	CHAR,
	LIST_START,
	LIST_TOKEN,
	SPACE,
};

class Parser {
	private:
		vector<char> parse_stack;
		string str;
	public:
		Parser(const string& input): str(input) {};
		Token* parse();
};
