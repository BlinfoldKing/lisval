#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <editline/readline.h>
#include <editline/history.h>

#include "include/parser.h"

using namespace std;

int main() {
    /* Print Version and Exit Information */
    puts("Press Ctrl+c to Exit\n");

    while (1) {

        /* Output our prompt and get input */
        char* input = readline("bracke> ");

        /* Add input to history */
        add_history(input);

        string str(input);
        // trim input;
        while (str.size() > 0 && str.back() == ' ') {
            str.pop_back();
        }

        if (str.length() > 0) {
            Parser p(str);
            auto res = p.parse();
            if (res)
                cout << res->debug() << '\n';
            else
                cerr << "expression must be in a list format\n";
        }
        /* Free retrieved input */
        free(input);

    }

    return 0;
}
