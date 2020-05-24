#include <iostream>

#include "linenoise.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/parser.h"
#include "../include/repl.h"

void Repl::run() {
	linenoiseInstallWindowChangeHandler();
	linenoiseSetMultiLine(1);

	const char *file = "~/lisval_history";

	linenoiseHistoryLoad(file);
	cout << "Press Ctrl+C to exit\n\n";

	char const *prompt = "(\x1b[1;32mlisval\x1b[0m) \u03bb ";

	string input = "";
	while (1) {
		char *result = linenoise(prompt);

		if (result == NULL)
		{
			break;
		}
		else if (!strncmp(result, "(history)", 8))
		{
			/* Display the current history. */
			for (int index = 0;; ++index)
			{
				char *hist = linenoiseHistoryLine(index);
				if (hist == NULL)
					break;
				printf("%4d: %s\n", index, hist);
				delete hist;
			}
		}
		if (*result == '\0')
		{
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
		linenoiseHistoryAdd(result);
		delete result;	
	}

	linenoiseHistorySave(file);
	linenoiseHistoryFree();
}
