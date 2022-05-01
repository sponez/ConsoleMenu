#include <iostream>
#include <vector>
#include <windows.h>
#include <conio.h>
using namespace std;
static const int BACKGROUND_WHITE = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
static const int BACKGROUND_BLACK = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

class consoleMenu
{
protected:
	HANDLE window = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO structCursorInfo;
	COORD point;

	int positionOfSelectingLine = 0;
	int amountOfActions;
	vector<pair<string, void (*)()>> actions;
	void drawMenu();
	void onCursor();
	void offCursor();
public:
	consoleMenu();
	consoleMenu(vector<string>&, vector<void (*)()>&, bool);
	void updateActions(vector<string>&, vector<void (*)()>&);
	void select();
};

consoleMenu::consoleMenu()
{
	GetConsoleCursorInfo(window, &structCursorInfo);
	point.X = 0;
	point.Y = 0;
	amountOfActions = 0;
}

consoleMenu::consoleMenu(vector<string>& actionNames, vector<void (*)()>& actionFunctions, bool isRoot = false)
{
	GetConsoleCursorInfo(window, &structCursorInfo);
	point.X = 0;
	point.Y = 0;

	amountOfActions = actionNames.size();

	if (amountOfActions != actionFunctions.size())
	{
		cout << "FATAL ERROR: MENU CAN'T BE CREATED" << endl;
		system("pause");
		exit(-1);
	}

	for (int i = 0; i < amountOfActions; i++)
		actions.push_back(pair<string, void (*)()>(actionNames[i], actionFunctions[i]));
	actions.push_back(pair<string, void (*)()>((isRoot) ? "Exit" : "Back", []() {}));
	amountOfActions++;
}

void consoleMenu::onCursor()
{
	structCursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(window, &structCursorInfo);
}

void consoleMenu::offCursor()
{
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(window, &structCursorInfo);
}

void consoleMenu::updateActions(vector<string>& actionNames, vector<void (*)()>& actionFunctions)
{
	actions.clear();
	amountOfActions = actionNames.size();

	if (amountOfActions != actionFunctions.size())
	{
		cout << "FATAL ERROR: MENU CAN'T BE CREATED" << endl;
		system("pause");
		exit(-1);
	}

	for (int i = 0; i < amountOfActions; i++)
		actions.push_back(pair<string, void (*)()>(actionNames[i], actionFunctions[i]));
}

void consoleMenu::drawMenu()
{
	system("cls");

	point.X = 0; point.Y = 0;
	cout << "Select:";
	point.X = 2;

	for (int i = 0; i < amountOfActions; i++)
	{
		point.Y = i + 1;
		SetConsoleCursorPosition(window, point);

		if (i != positionOfSelectingLine)
		{
			cout << actions[i].first;
		}
		else
		{
			SetConsoleTextAttribute(window, BACKGROUND_WHITE);
			cout << actions[i].first;
			SetConsoleTextAttribute(window, BACKGROUND_BLACK);
		}
	}
}

void consoleMenu::select()
{
	while (positionOfSelectingLine != amountOfActions - 1)
	{
		offCursor();

		drawMenu();
		while (GetAsyncKeyState(VK_RETURN)) {}

		for (;;)
		{
			if (GetAsyncKeyState(VK_UP))
			{
				if (positionOfSelectingLine != 0) positionOfSelectingLine--;
				else positionOfSelectingLine = amountOfActions - 1;
				drawMenu();
			}
			else if (GetAsyncKeyState(VK_DOWN))
			{
				if (positionOfSelectingLine != amountOfActions - 1) positionOfSelectingLine++;
				else positionOfSelectingLine = 0;
				drawMenu();
			}
			else if (GetAsyncKeyState(VK_RETURN))
			{
				cin.ignore(LLONG_MAX, '\n');
				while (_kbhit()) cin.get();
				break;
			}

			Sleep(100);
		}
		system("cls");

		onCursor();

		actions[positionOfSelectingLine].second();
		Sleep(25);
	}

	positionOfSelectingLine = 0;
}

class wconsoleMenu
{
	HANDLE window = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO structCursorInfo;
	COORD point;

	int positionOfSelectingLine = 0;
	int amountOfActions;
	vector<pair<wstring, void (*)()>> actions;
	void drawMenu();
	void onCursor();
	void offCursor();
public:
	wconsoleMenu();
	wconsoleMenu(vector<wstring>&, vector<void (*)()>&, bool);
	void updateActions(vector<wstring>&, vector<void (*)()>&);
	void select();
};

wconsoleMenu::wconsoleMenu()
{
	GetConsoleCursorInfo(window, &structCursorInfo);
	point.X = 0;
	point.Y = 0;
	amountOfActions = 0;
}

wconsoleMenu::wconsoleMenu(vector<wstring>& actionNames, vector<void (*)()>& actionFunctions, bool isRoot = false)
{
	GetConsoleCursorInfo(window, &structCursorInfo);
	point.X = 0;
	point.Y = 0;

	amountOfActions = actionNames.size();

	if (amountOfActions != actionFunctions.size())
	{
		wcout << "FATAL ERROR: MENU CAN'T BE CREATED" << endl;
		_wsystem(L"pause");
		exit(-1);
	}

	for (int i = 0; i < amountOfActions; i++)
		actions.push_back(pair<wstring, void (*)()>(actionNames[i], actionFunctions[i]));
	actions.push_back(pair<wstring, void (*)()>((isRoot) ? L"Exit" : L"Back", []() {}));
	amountOfActions++;
}

void wconsoleMenu::onCursor()
{
	structCursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(window, &structCursorInfo);
}

void wconsoleMenu::offCursor()
{
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(window, &structCursorInfo);
}

void wconsoleMenu::updateActions(vector<wstring>& actionNames, vector<void (*)()>& actionFunctions)
{
	actions.clear();
	amountOfActions = actionNames.size();

	if (amountOfActions != actionFunctions.size())
	{
		wcout << "FATAL ERROR: MENU CAN'T BE CREATED" << endl;
		_wsystem(L"pause");
		exit(-1);
	}

	for (int i = 0; i < amountOfActions; i++)
		actions.push_back(pair<wstring, void (*)()>(actionNames[i], actionFunctions[i]));
}

void wconsoleMenu::drawMenu()
{
	_wsystem(L"cls");

	point.X = 0; point.Y = 0;
	wcout << "Select:";
	point.X = 2;

	for (int i = 0; i < amountOfActions; i++)
	{
		point.Y = i + 1;
		SetConsoleCursorPosition(window, point);

		if (i != positionOfSelectingLine)
		{
			wcout << actions[i].first;
		}
		else
		{
			SetConsoleTextAttribute(window, BACKGROUND_WHITE);
			wcout << actions[i].first;
			SetConsoleTextAttribute(window, BACKGROUND_BLACK);
		}
	}
}

void wconsoleMenu::select()
{
	while (positionOfSelectingLine != amountOfActions - 1)
	{
		offCursor();

		drawMenu();
		while (GetAsyncKeyState(VK_RETURN)) {}

		for (;;)
		{
			if (GetAsyncKeyState(VK_UP))
			{
				if (positionOfSelectingLine != 0) positionOfSelectingLine--;
				else positionOfSelectingLine = amountOfActions - 1;
				drawMenu();
			}
			else if (GetAsyncKeyState(VK_DOWN))
			{
				if (positionOfSelectingLine != amountOfActions - 1) positionOfSelectingLine++;
				else positionOfSelectingLine = 0;
				drawMenu();
			}
			else if (GetAsyncKeyState(VK_RETURN))
			{
				wcin.ignore(LLONG_MAX, '\n');
				while (_kbhit()) wcin.get();
				break;
			}

			Sleep(200);
		}
		_wsystem(L"cls");

		onCursor();

		actions[positionOfSelectingLine].second();
		Sleep(25);
	}

	positionOfSelectingLine = 0;
}