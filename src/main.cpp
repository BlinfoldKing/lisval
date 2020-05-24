#include "include/parser.h"
#include "include/repl.h"
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	if (argc > 1)	{
		string input = "";
		string line;
		ifstream myfile(argv[1]);
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				input += line;
			}
			myfile.close();
		}

		else
			cout << "Unable to open file\n";

		Parser p(input);
		Token* t = p.parse();
		if (t) {
			Evaluator e;
			Token* result = e.eval(t);
			if (result) cout << result->debug() << '\n';
		} else {
			cout << "invalid pattern: " << input << '\n';
		}

		return 0;
	}

	Repl r;
	r.run();
	return 0;
}
