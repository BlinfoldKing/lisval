#include <iostream>

using namespace std;

int main() {
    string input;
    cout << "[bracke]> ";
    while (getline(cin, input)) {
        cout << "[bracke]> ";
        if (input == "exit") break;
    }

    return 0;
}
