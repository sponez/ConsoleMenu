#pragma once
#include <iostream>
#include <vector>
#include <windows.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
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
	size_t currentPosition;
	size_t amountOfOptions;
	vector<pair<string, void (*)(string)>> Options;
	void drawMenu();
	void redrawMenu(short);
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
			cout << Options[i].first;
	}

	point.Y = currentPosition + 1;
	SetConsoleCursorPosition(consoleHandle, point);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_WHITE);
	cout << Options[currentPosition].first;
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_BLACK);
}

void consoleMenu::redrawMenu(short action)
{
	point.X = 2;

	if (action == -1)
	{
		if (currentPosition != amountOfOptions - 1)
		{
			point.Y = currentPosition + 2;
			SetConsoleCursorPosition(consoleHandle, point);
			cout << Options[currentPosition + 1].first;
		}
		else
		{
			point.Y = 1;
			SetConsoleCursorPosition(consoleHandle, point);
			cout << Options[0].first;
		}
	}

	if (action == 1)
	{
		if (currentPosition != 0)
		{
			point.Y = currentPosition;
			SetConsoleCursorPosition(consoleHandle, point);
			cout << Options[currentPosition - 1].first;
		}
		else
		{
			point.Y = amountOfOptions;
			SetConsoleCursorPosition(consoleHandle, point);
			cout << Options[amountOfOptions - 1].first;
		}
	}

	point.Y = currentPosition + 1;
	SetConsoleCursorPosition(consoleHandle, point);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_WHITE);
	cout << Options[currentPosition].first;
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_BLACK);
}

void consoleMenu::select()
{
	while (currentPosition != amountOfOptions - 1)
	{
		FlushConsoleInputBuffer(consoleHandle);
		offCursor();

		drawMenu();
		while (GetAsyncKeyState(VK_RETURN)) { Sleep(10); }

		for (;; Sleep(10))
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

					while (GetAsyncKeyState(VK_LBUTTON)) { Sleep(10); }
				}

				if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_TAB))
					ShowWindow(consoleWindow, SW_MINIMIZE);

				for (int i = 0; GetAsyncKeyState(VK_UP); Sleep(100 - 2 * i))
				{
					if (currentPosition != 0) currentPosition--;
					else currentPosition = amountOfOptions - 1;
					redrawMenu(-1);

					if (i < 40) i++;
				}

				for (int i = 0; GetAsyncKeyState(VK_DOWN); Sleep(100 - 2 * i))
				{
					if (currentPosition != amountOfOptions - 1) currentPosition++;
					else currentPosition = 0;
					redrawMenu(1);

					if (i < 40) i++;
				}

				if (GetAsyncKeyState(VK_RETURN))
				{
					cin.ignore(LLONG_MAX, '\n');
					while (_kbhit()) cin.get();
					break;
				}
			}
		}
		system("cls");

		onCursor();

		Options[currentPosition].second(Options[currentPosition].first);
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
	size_t currentPosition;
	size_t amountOfOptions;
	vector<pair<wstring, void (*)(wstring)>> Options;
	void drawMenu();
	void redrawMenu(short);
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
	ignore = _setmode(_fileno(stdout), _O_U16TEXT);
	ignore = _setmode(_fileno(stdin), _O_U16TEXT);
	ignore = _setmode(_fileno(stderr), _O_U16TEXT);

	GetConsoleCursorInfo(consoleHandle, &structCursorInfo);
	point.X = 0;
	point.Y = 0;
	this->selectText = selectText;
	this->exitText = exitText;
	currentPosition = 0;
	amountOfOptions = optionNames.size();

	if (amountOfOptions != optionFunctions.size())
	{
		wcout << L"FATAL ERROR: MENU CAN'T BE CREATED" << endl;
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
		wcout << L"FATAL ERROR: MENU CAN'T BE CREATED" << endl;
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
			wcout << Options[i].first;
	}

	point.Y = currentPosition + 1;
	SetConsoleCursorPosition(consoleHandle, point);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_WHITE);
	wcout << Options[currentPosition].first;
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_BLACK);
}

void wconsoleMenu::redrawMenu(short action)
{
	point.X = 2;

	if (action == -1)
	{
		if (currentPosition != amountOfOptions - 1)
		{
			point.Y = currentPosition + 2;
			SetConsoleCursorPosition(consoleHandle, point);
			wcout << Options[currentPosition + 1].first;
		}
		else
		{
			point.Y = 1;
			SetConsoleCursorPosition(consoleHandle, point);
			wcout << Options[0].first;
		}
	}

	if (action == 1)
	{
		if (currentPosition != 0)
		{
			point.Y = currentPosition;
			SetConsoleCursorPosition(consoleHandle, point);
			wcout << Options[currentPosition - 1].first;
		}
		else
		{
			point.Y = amountOfOptions;
			SetConsoleCursorPosition(consoleHandle, point);
			wcout << Options[amountOfOptions - 1].first;
		}
	}

	point.Y = currentPosition + 1;
	SetConsoleCursorPosition(consoleHandle, point);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_WHITE);
	wcout << Options[currentPosition].first;
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_BLACK);
}

void wconsoleMenu::select()
{
	while (currentPosition != amountOfOptions - 1)
	{
		FlushConsoleInputBuffer(consoleHandle);
		offCursor();

		drawMenu();
		while (GetAsyncKeyState(VK_RETURN)) { Sleep(10); }

		for (;;Sleep(10))
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

					while (GetAsyncKeyState(VK_LBUTTON)) { Sleep(10); }
				}

				if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_TAB))
					ShowWindow(consoleWindow, SW_MINIMIZE);

				for (int i = 0; GetAsyncKeyState(VK_UP); Sleep(100 - 2 * i))
				{
					if (currentPosition != 0) currentPosition--;
					else currentPosition = amountOfOptions - 1;
					redrawMenu(-1);

					if (i < 40) i++;
				}

				for (int i = 0; GetAsyncKeyState(VK_DOWN); Sleep(100 - 2 * i))
				{
					if (currentPosition != amountOfOptions - 1) currentPosition++;
					else currentPosition = 0;
					redrawMenu(1);

					if (i < 40) i++;
				}

				if (GetAsyncKeyState(VK_RETURN))
				{
					wcin.ignore(LLONG_MAX, '\n');
					while (_kbhit()) wcin.get();
					break;
				}
			}
		}
		_wsystem(L"cls");

		onCursor();

		Options[currentPosition].second(Options[currentPosition].first);
	}

	currentPosition = 0;
}