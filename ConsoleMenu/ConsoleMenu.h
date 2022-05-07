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
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND consoleWindow = GetConsoleWindow();
	CONSOLE_CURSOR_INFO structCursorInfo;
	COORD point;

	string selectText;
	string exitText;
	int currentPosition;
	int amountOfOptions;
	vector<pair<string, void (*)(string)>> Options;
	void drawMenu();
	void onCursor();
	void offCursor();
public:
	consoleMenu();
	consoleMenu(string, vector<string>&, vector<void (*)(string)>&, string);
	void updateOptions(vector<string>&, vector<void (*)(string)>&);
	void select();
};

consoleMenu::consoleMenu()
{
	GetConsoleCursorInfo(consoleHandle, &structCursorInfo);
	point.X = 0;
	point.Y = 0;
	selectText = "";
	exitText = "";
	currentPosition = 0;
	amountOfOptions = 1;
}

consoleMenu::consoleMenu(string selectText, vector<string>& optionNames, vector<void (*)(string)>& optionFunctions, string exitText)
{
	GetConsoleCursorInfo(consoleHandle, &structCursorInfo);
	point.X = 0;
	point.Y = 0;
	this->selectText = selectText;
	this->exitText = exitText;
	currentPosition = 0;
	amountOfOptions = optionNames.size();

	if (amountOfOptions != optionFunctions.size())
	{
		cout << "FATAL ERROR: MENU CAN'T BE CREATED" << endl;
		system("pause");
		exit(-1);
	}

	for (int i = 0; i < amountOfOptions; i++)
		Options.push_back(pair<string, void (*)(string)>(optionNames[i], optionFunctions[i]));
	Options.push_back(pair<string, void (*)(string)>(exitText, [](string) {}));
	amountOfOptions++;
}

void consoleMenu::onCursor()
{
	structCursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(consoleHandle, &structCursorInfo);
}

void consoleMenu::offCursor()
{
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &structCursorInfo);
}

void consoleMenu::updateOptions(vector<string>& optionNames, vector<void (*)(string)>& optionFunctions)
{
	Options.clear();
	amountOfOptions = optionNames.size();

	if (amountOfOptions != optionFunctions.size())
	{
		cout << "FATAL ERROR: MENU CAN'T BE CREATED" << endl;
		system("pause");
		exit(-1);
	}

	for (int i = 0; i < amountOfOptions; i++)
		Options.push_back(pair<string, void (*)(string)>(optionNames[i], optionFunctions[i]));
	Options.push_back(pair<string, void (*)(string)>(exitText, [](string) {}));
	amountOfOptions++;
}

void consoleMenu::drawMenu()
{
	system("cls");

	point.X = 0; point.Y = 0;
	cout << selectText;
	point.X = 2;

	for (int i = 0; i < amountOfOptions; i++)
	{
		point.Y = i + 1;
		SetConsoleCursorPosition(consoleHandle, point);

		if (i != currentPosition)
		{
			cout << Options[i].first;
		}
		else
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_WHITE);
			cout << Options[i].first;
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_BLACK);
		}
	}
}

void consoleMenu::select()
{
	while (currentPosition != amountOfOptions - 1)
	{
		offCursor();

		drawMenu();
		while (GetAsyncKeyState(VK_RETURN)) {}

		for (;;)
		{
			if (!IsIconic(consoleWindow))
			{
				if (GetAsyncKeyState(VK_LBUTTON))
				{
					POINT cursorPosition;
					RECT windowRect;
					GetCursorPos(&cursorPosition);
					GetWindowRect(consoleWindow, &windowRect);
					if (cursorPosition.x < windowRect.left || cursorPosition.x > windowRect.right || cursorPosition.y < windowRect.top || cursorPosition.y > windowRect.bottom)
						ShowWindow(consoleWindow, SW_MINIMIZE);
				}

				if (GetAsyncKeyState(VK_UP))
				{
					if (currentPosition != 0) currentPosition--;
					else currentPosition = amountOfOptions - 1;
					drawMenu();
				}

				if (GetAsyncKeyState(VK_DOWN))
				{
					if (currentPosition != amountOfOptions - 1) currentPosition++;
					else currentPosition = 0;
					drawMenu();
				}

				if (GetAsyncKeyState(VK_RETURN))
				{
					wcin.ignore(LLONG_MAX, '\n');
					while (_kbhit()) wcin.get();
					break;
				}
			}

			Sleep(175);
		}
		system("cls");

		onCursor();

		Options[currentPosition].second(Options[currentPosition].first);
		Sleep(25);
	}

	currentPosition = 0;
}

class wconsoleMenu
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND consoleWindow = GetConsoleWindow();
	CONSOLE_CURSOR_INFO structCursorInfo;
	COORD point;

	wstring selectText;
	wstring exitText;
	int currentPosition;
	int amountOfOptions;
	vector<pair<wstring, void (*)(wstring)>> Options;
	void drawMenu();
	void onCursor();
	void offCursor();
public:
	wconsoleMenu();
	wconsoleMenu(wstring, vector<wstring>&, vector<void (*)(wstring)>&, wstring);
	void updateOptions(vector<wstring>&, vector<void (*)(wstring)>&);
	void select();
};

wconsoleMenu::wconsoleMenu()
{
	GetConsoleCursorInfo(consoleHandle, &structCursorInfo);
	point.X = 0;
	point.Y = 0;
	selectText = L"";
	exitText = L"";
	currentPosition = 0;
	amountOfOptions = 1;
}

wconsoleMenu::wconsoleMenu(wstring selectText, vector<wstring>& optionNames, vector<void (*)(wstring)>& optionFunctions, wstring exitText)
{
	GetConsoleCursorInfo(consoleHandle, &structCursorInfo);
	point.X = 0;
	point.Y = 0;
	this->selectText = selectText;
	this->exitText = exitText;
	currentPosition = 0;
	amountOfOptions = optionNames.size();

	if (amountOfOptions != optionFunctions.size())
	{
		wcout << "FATAL ERROR: MENU CAN'T BE CREATED" << endl;
		_wsystem(L"pause");
		exit(-1);
	}

	for (int i = 0; i < amountOfOptions; i++)
		Options.push_back(pair<wstring, void (*)(wstring)>(optionNames[i], optionFunctions[i]));
	Options.push_back(pair<wstring, void (*)(wstring)>(exitText, [](wstring) {}));
	amountOfOptions++;
}

void wconsoleMenu::onCursor()
{
	structCursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(consoleHandle, &structCursorInfo);
}

void wconsoleMenu::offCursor()
{
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &structCursorInfo);
}

void wconsoleMenu::updateOptions(vector<wstring>& optionNames, vector<void (*)(wstring)>& optionFunctions)
{
	Options.clear();
	amountOfOptions = optionNames.size();

	if (amountOfOptions != optionFunctions.size())
	{
		wcout << "FATAL ERROR: MENU CAN'T BE CREATED" << endl;
		_wsystem(L"pause");
		exit(-1);
	}

	for (int i = 0; i < amountOfOptions; i++)
		Options.push_back(pair<wstring, void (*)(wstring)>(optionNames[i], optionFunctions[i]));
	Options.push_back(pair<wstring, void (*)(wstring)>(exitText, [](wstring) {}));
	amountOfOptions++;
}

void wconsoleMenu::drawMenu()
{
	_wsystem(L"cls");

	point.X = 0; point.Y = 0;
	wcout << selectText;
	point.X = 2;

	for (int i = 0; i < amountOfOptions; i++)
	{
		point.Y = i + 1;
		SetConsoleCursorPosition(consoleHandle, point);

		if (i != currentPosition)
		{
			wcout << Options[i].first;
		}
		else
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_WHITE);
			wcout << Options[i].first;
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_BLACK);
		}
	}
}

void wconsoleMenu::select()
{
	while (currentPosition != amountOfOptions - 1)
	{
		FlushConsoleInputBuffer(consoleHandle);
		offCursor();

		drawMenu();
		while (GetAsyncKeyState(VK_RETURN)) {}

		for (;;)
		{
			if (!IsIconic(consoleWindow))
			{
				if (GetAsyncKeyState(VK_LBUTTON))
				{
					POINT cursorPosition;
					RECT windowRect;
					GetCursorPos(&cursorPosition);
					GetWindowRect(consoleWindow, &windowRect);
					if (cursorPosition.x < windowRect.left || cursorPosition.x > windowRect.right || cursorPosition.y < windowRect.top || cursorPosition.y > windowRect.bottom)
						ShowWindow(consoleWindow, SW_MINIMIZE);
				}

				if (GetAsyncKeyState(VK_UP))
				{
					if (currentPosition != 0) currentPosition--;
					else currentPosition = amountOfOptions - 1;
					drawMenu();
				}

				if (GetAsyncKeyState(VK_DOWN))
				{
					if (currentPosition != amountOfOptions - 1) currentPosition++;
					else currentPosition = 0;
					drawMenu();
				}

				if (GetAsyncKeyState(VK_RETURN))
				{
					wcin.ignore(LLONG_MAX, '\n');
					while (_kbhit()) wcin.get();
					break;
				}
			}

			Sleep(175);
		}
		_wsystem(L"cls");

		onCursor();

		Options[currentPosition].second(Options[currentPosition].first);
		Sleep(25);
	}

	currentPosition = 0;
}