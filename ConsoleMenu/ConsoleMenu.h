#pragma once
#include <iostream>
#include <vector>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include "consoleParameters.h"
using namespace std;
static const int BACKGROUND_WHITE = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
static const int BACKGROUND_BLACK = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

class consoleMenu : consoleParameters
{
	string selectText{};
	string exitText{};
	bool isSelectTextEmpty{};
	bool isExitTextEmpty{};
	short currentPosition{};
	vector<pair<string, void (*)(string)>> options{};

	void drawMenu(vector<pair<string, void (*)(string)>>&, string); //Draw all options
	void redrawMenu(vector<pair<string, void (*)(string)>>& , short); //Redraw changed options
	bool selectController(vector<pair<string, void (*)(string)>>&, string); //Controller for menu
	void displayFilteredOptions(string&, vector<pair<string, void (*)(string)>>&); //If use select with filter, this is necessary to display the sutable options
	vector<pair<string, void (*)(string)>> optionsFilterer(string&); //If use select with filter, this is necessary to filter options by characters

public:
//Initializers
	consoleMenu() {}; //Empty console menu
	consoleMenu(vector<string>&, vector<void (*)(string)>&, string, string); //Console menu with different functions

//Other public functions
	short singleSelect();
	short singleSelectWithFilter();
	void cyclicSelect();
	void cyclicSelectWithFilter();
};

consoleMenu::consoleMenu(vector<string>& optionNames, vector<void (*)(string)>& optionFunctions, string selectText = "", string exitText = "")
{
	if (optionNames.size() > SHRT_MAX)
	{
		cout << "FATAL ERROR. MENU CAN'T BE CREATED. TOO MANY OPTIONS." << endl;
		system("pause");
		exit(-1);
	}

	GetConsoleCursorInfo(consoleHandle, &structCursorInfo);
	point.X = 0;
	point.Y = 0;

	this->selectText = selectText;
	isSelectTextEmpty = (selectText == "");

	this->exitText = exitText;
	isExitTextEmpty = (exitText == "");

	currentPosition = 0;

	if (optionNames.size() != optionFunctions.size())
	{
		cout << "FATAL ERROR. MENU CAN'T BE CREATED. SIZES OF OPTIONS' NAMES ARRAY AND FUNCTIONS' ARRAY ARE DIFFERENT." << endl;
		system("pause");
		exit(-2);
	}

	for (unsigned short i = 0; i < optionNames.size(); i++)
		options.push_back(pair<string, void (*)(string)>(optionNames[i], optionFunctions[i]));
}

void consoleMenu::drawMenu(vector<pair<string, void (*)(string)>>& options, string filterText)
{
	system("cls");

	if (!isSelectTextEmpty)
	{
		point.X = point.Y = 0;
		cout << selectText << filterText;
	}

	point.X = 2;
	for (unsigned short i = 0; i < options.size(); i++)
	{
		point.Y = (short)!isSelectTextEmpty + i;
		SetConsoleCursorPosition(consoleHandle, point);

		if (i != currentPosition)
			cout << options[i].first;
	}

	point.Y = (short)!isSelectTextEmpty + currentPosition;
	SetConsoleCursorPosition(consoleHandle, point);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_WHITE);
	cout << options[currentPosition].first;
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_BLACK);
}

void consoleMenu::redrawMenu(vector<pair<string, void (*)(string)>>& options, short action)
{
	short previousPosition = static_cast<short>((options.size() + currentPosition - action) % options.size()); //Find previous position number

	point.X = 2; //Set space from border
	point.Y = (short)!isSelectTextEmpty + previousPosition; //Set position of the cursor on previous option
	SetConsoleCursorPosition(consoleHandle, point);
	cout << options[previousPosition].first; //Draw previous option with black background

	point.Y = (short)!isSelectTextEmpty + currentPosition; //Set position of the cursor on current option
	SetConsoleCursorPosition(consoleHandle, point);

	//Draw previous option with white background
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_WHITE);
	cout << options[currentPosition].first;
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_BLACK);
}

bool consoleMenu::selectController(vector<pair<string, void (*)(string)>>& options, string filterText = "")
{
	FlushConsoleInputBuffer(consoleHandle);
	offCursor();

	drawMenu(options, filterText);
	while (GetAsyncKeyState(VK_UP)) { Sleep(10); }
	while (GetAsyncKeyState(VK_DOWN)) { Sleep(10); }
	while (GetAsyncKeyState(VK_RETURN)) { Sleep(10); }
	while (GetAsyncKeyState(VK_BACK)) { Sleep(10); }

	for (;; Sleep(10))
	{
		if (IsIconic(consoleWindow)) continue;

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

		for (int i = 0; GetAsyncKeyState(VK_UP); Sleep(200 - 10 * i))
		{
			currentPosition = static_cast<short>((options.size() + currentPosition - 1) % options.size());
			redrawMenu(options, -1);
			if (i < 19) i++;
		}

		for (int i = 0; GetAsyncKeyState(VK_DOWN); Sleep(200 - 10 * i))
		{
			currentPosition = (currentPosition + 1) % options.size();
			redrawMenu(options, 1);
			if (i < 19) i++;
		}

		if (GetAsyncKeyState(VK_RETURN))
		{
			cin.ignore(LLONG_MAX, '\n');
			while (_kbhit()) ignore = _getch();

			onCursor();
			system("cls");

			return true;
		}

		if (GetAsyncKeyState(VK_BACK))
		{
			while (_kbhit()) ignore = _getch();

			currentPosition = 0;
			onCursor();
			system("cls");

			return false;
		}
	}
}

short consoleMenu::singleSelect()
{
	if (options.size() == 0) return -1;
	currentPosition = 0;

	if (!isExitTextEmpty) options.push_back(pair<string, void (*)(string)>(exitText, [](string) {}));
	if (selectController(options))
	{
		options[currentPosition].second(options[currentPosition].first);

		if (!isExitTextEmpty)
		{
			options.pop_back();
			return ((currentPosition != options.size()) ? currentPosition : -1);
		}
		else
			return currentPosition;
	}
	else
		return -1;
}

void consoleMenu::cyclicSelect()
{
	if (options.size() == 0) return;

	if (!isExitTextEmpty) options.push_back(pair<string, void (*)(string)>(exitText, [](string) {}));
	do
	{
		if (selectController(options))
			options[currentPosition].second(options[currentPosition].first);
		else
			return;
	} 	while (isExitTextEmpty || currentPosition != options.size() - 1);
	if (!isExitTextEmpty) options.pop_back();

	currentPosition = 0;
}

vector<pair<string, void (*)(string)>> consoleMenu::optionsFilterer(string& filterText)
{
	vector<pair<string, void (*)(string)>> filteredOptions;

	for (int i = 0; i < options.size(); i++)
	{
		if (filterText.length() <= options[i].first.length() && filterText == options[i].first.substr(0, filterText.length()))
			filteredOptions.push_back(options[i]);
	}
	if (!isExitTextEmpty) filteredOptions.push_back(pair<string, void (*)(string)>(exitText, [](string) {}));

	return filteredOptions;
}

void consoleMenu::displayFilteredOptions(string& filterText, vector<pair<string, void (*)(string)>>& filteredOptions)
{
	system("cls");
	cout << selectText << filterText;

	point.X = 2;
	if (filteredOptions.size() > 0)
	{
		for (int i = 0; i < filteredOptions.size(); i++)
		{
			point.Y = 1 + i;
			SetConsoleCursorPosition(consoleHandle, point);
			cout << filteredOptions[i].first;
		}
	}
	else
	{
		point.Y = 1;
		SetConsoleCursorPosition(consoleHandle, point);
		cout << "Nothing not found! Try something else.";
	}

	point.X = selectText.length() + filterText.length();
	point.Y = 0;
	SetConsoleCursorPosition(consoleHandle, point);
}

short consoleMenu::singleSelectWithFilter()
{
	if (options.size() == 0) return -1;
	if (isSelectTextEmpty) isSelectTextEmpty = false;
	currentPosition = 0;

	string filterText = "";
	vector<pair<string, void (*)(string)>> filteredOptions = optionsFilterer(filterText);

	displayFilteredOptions(filterText, filteredOptions);
	for (;;Sleep(10))
	{
		char ch;

		if (ch = _getch())
		{
			if (ch == 27)
			{
				system("cls");
				isSelectTextEmpty = (selectText == "");
				return -1;
			}
			else if (ch == 13)
			{
				if (filteredOptions.size() == 0) continue;

				if (selectController(filteredOptions))
				{
					filteredOptions[currentPosition].second(filteredOptions[currentPosition].first);

					if (!isExitTextEmpty)
						filteredOptions.pop_back();

					if (currentPosition == filteredOptions.size())
						return -1;
					else
					{
						short i = 0;

						for (short j = 0; j <= currentPosition;)
						{
							if (options[i].first == filteredOptions[j].first)
								j++;

							i++;
						}

						return (i - 1);
					}
				}
				else
					return -1;
			}
			else
			{
				if (ch == 8)
				{
					if (filterText.length() > 0)
					{
						filterText.pop_back();

						filteredOptions = optionsFilterer(filterText);
						displayFilteredOptions(filterText, filteredOptions);
					}
				}
				else
				{
					if (ch == -32)
					{
						ignore = _getch();
						continue;
					}

					filterText += ch;

					filteredOptions = optionsFilterer(filterText);
					displayFilteredOptions(filterText, filteredOptions);
				}
			}
		}
	}
}

void consoleMenu::cyclicSelectWithFilter()
{
	if (options.size() == 0) return;
	if (isSelectTextEmpty) isSelectTextEmpty = false;

	string filterText = "";
	vector<pair<string, void (*)(string)>> filteredOptions = optionsFilterer(filterText);

	displayFilteredOptions(filterText, filteredOptions);
	for (;;Sleep(10))
	{
		char ch;

		if (ch = _getch())
		{
			if (ch == 27)
			{
				system("cls");
				isSelectTextEmpty = (selectText == "");
				return;
			}
			else if (ch == 13)
			{
				if (filteredOptions.size() == 0) continue;

				do
				{
					if (selectController(filteredOptions, filterText))
						filteredOptions[currentPosition].second(filteredOptions[currentPosition].first);
					else
						break;
				} while (isExitTextEmpty || currentPosition != filteredOptions.size() - 1);

				currentPosition = 0;
				displayFilteredOptions(filterText, filteredOptions);
			}
			else
			{
				if (ch == 8)
				{
					if (filterText.length() > 0)
					{
						filterText.pop_back();

						filteredOptions = optionsFilterer(filterText);
						displayFilteredOptions(filterText, filteredOptions);
					}
				}
				else
				{
					if (ch == -32)
					{
						ignore = _getch();
						continue;
					}

					filterText += ch;

					filteredOptions = optionsFilterer(filterText);
					displayFilteredOptions(filterText, filteredOptions);
				}
			}
		}
	}
}