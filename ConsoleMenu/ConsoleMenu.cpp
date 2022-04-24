#include <iostream>
#include <vector>
#include "ConsoleMenu.h"

using namespace std;

void emptyFunc() { system("pause"); }
void toBranch();

vector<string> options = { "1" , "2" };
vector<void (*)()> funcsRoot({ toBranch, emptyFunc }), funcsBranch = { emptyFunc, emptyFunc };

consoleMenu root(options, funcsRoot, true);
consoleMenu branch;

void toBranch() { branch.select(); }

int main()
{
	branch = consoleMenu(options, funcsBranch);
	root.select();
}