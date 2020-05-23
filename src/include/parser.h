#include<map>
#include<vector>

using namespace std;

enum TokenType {
    ATOM,
    LIST,
};

enum State {
    CHAR,
    LIST_START,
    LIST_TOKEN,
    SPACE,
};

struct Token {
    TokenType type;
    string atom;
    vector<Token*> list;
    string debug();
};

class Parser {
    private:
        vector<char> parse_stack;
        string str;
    public:
        Parser(const string& str);
        Token* parse();
};
