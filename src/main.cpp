#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <editline/readline.h>
#include <editline/history.h>

#include "include/parser.h"
#include "include/evaluator.h"

using namespace std;

int main() {
    /* Print Version and Exit Information */
    puts("Press Ctrl+c to Exit\n");

    string str = "";
    Evaluator E;
    while (1) {

        /* Output our prompt and get input */
        char* input = readline("(lisval) ");

        /* Add input to history */
        add_history(input);

        string input_str(input);
        // trim input;
        while (input_str.size() > 0 && input_str.back() == ' ') {
            str.pop_back();
        }

        str += input_str;
        if (str.back() == '\\') {
            str.pop_back();
            continue;
        }

        if (str.length() > 0) {
            Parser p(str);
            auto res = p.parse();
            if (res) {
                auto ret = E.eval(res);
                if (ret != NULL) {
                    cout << ret->debug() << '\n';
                    /* cout << res->debug() << '\n'; */
                    /* cout << res->debugToken() << '\n'; */
                }
            } else
                cerr << "expression must be in a list format\n";
        }
        /* Free retrieved input */
        free(input);

    }

    return 0;
}
