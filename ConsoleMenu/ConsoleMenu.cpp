#include <iostream>
#include <vector>
#include "ConsoleMenu.h"

using namespace std;

void emptyFunc(string str)
{
	cout << str << endl;
	system("pause");
}
void toBranch(string);

vector<string> options = { "One" , "Two" };
vector<void (*)(string)> funcsRoot({ toBranch, emptyFunc }), funcsBranch = { emptyFunc, emptyFunc };

consoleMenu root("Select", options, funcsRoot, "Exit");
consoleMenu branch;

void toBranch(string) { branch.select(); }

int main()
{
	branch = consoleMenu("Select", options, funcsBranch, "Back");
	root.select();
}