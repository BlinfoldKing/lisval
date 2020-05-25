#include <iostream>

#include "../include/parser.h"
#include "../include/repl.h"

void Repl::run() {

	/* linenoiseHistoryLoad(file); */
	cout << "Press Ctrl+C to exit\n\n";

	string prompt = "(\x1b[1;32mlisval\x1b[0m) \u03bb ";

	string input = "";
	while (1) {
        cout << prompt;

        string result;
        getline(cin, result);
        cout << result << '\n';

		if (input == "(history)") {

		} else if (result == "") {
			continue;
		}

		string str_result(result);
		while (str_result.back() == ' ') str_result.pop_back();
		if (str_result.back() == '\\') {
			str_result.pop_back();
			input += str_result + ' ';
			continue;
		}

		input += str_result;

		Parser p(input);
		Token *ast = p.parse();
		if (ast)
		{
			Token *eval_result = this->evaluator.eval(ast);
			if (eval_result)
				cout << eval_result->debug() << '\n';
		}
		else
		{
			cout << "invalid pattern: " << result << '\n';
		}

		input = "";
	}

}
