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

class wconsoleMenu : consoleParameters
{
	wstring selectText{};
	wstring exitText{};
	bool isSelectTextEmpty{};
	bool isExitTextEmpty{};
	short currentPosition{};
	vector<pair<wstring, void (*)(wstring)>> options{};

	void menuCreator(vector<wstring>&, wstring, wstring); //Manual initializer
	void drawMenu(vector<pair<wstring, void (*)(wstring)>>&, wstring); //Draw all options
	void redrawMenu(vector<pair<wstring, void (*)(wstring)>>&, short); //Redraw changed options
	bool selectController(vector<pair<wstring, void (*)(wstring)>>&, wstring); //Controller for menu
	void displayFilteredOptions(wstring&, vector<pair<wstring, void (*)(wstring)>>&); //If use select with filter, this is necessary to display the sutable options
	vector<pair<wstring, void (*)(wstring)>> optionsFilterer(wstring&); //If use select with filter, this is necessary to filter options by characters

public:
	//Initializers
	wconsoleMenu() {}; //Empty console menu
	wconsoleMenu(vector<wstring>&, vector<void (*)(wstring)>&, wstring, wstring); //Console menu with different functions
	wconsoleMenu(vector<wstring>&, void (*function)(wstring), wstring, wstring); //Console menu with one function

//Other public functions
	short singleSelect();
	short singleSelectWithFilter();
	void cyclicSelect();
	void cyclicSelectWithFilter();
};

void wconsoleMenu::menuCreator(vector<wstring>& optionNames, wstring selectText, wstring exitText)
{
	ignore = _setmode(_fileno(stdout), _O_U16TEXT);
	ignore = _setmode(_fileno(stdin), _O_U16TEXT);
	ignore = _setmode(_fileno(stderr), _O_U16TEXT);

	if (optionNames.size() > SHRT_MAX)
	{
		wcout << L"FATAL ERROR. MENU CAN'T BE CREATED. TOO MANY OPTIONS." << endl;
		_wsystem(L"pause");
		exit(-1);
	}

	GetConsoleCursorInfo(consoleHandle, &structCursorInfo);
	point.X = 0;
	point.Y = 0;

	this->selectText = selectText;
	isSelectTextEmpty = (selectText == L"");

	this->exitText = exitText;
	isExitTextEmpty = (exitText == L"");

	currentPosition = 0;
}

wconsoleMenu::wconsoleMenu(vector<wstring>& optionNames, vector<void (*)(wstring)>& optionFunctions, wstring selectText = L"", wstring exitText = L"")
{
	menuCreator(optionNames, selectText, exitText);

	if (optionNames.size() != optionFunctions.size())
	{
		wcout << L"FATAL ERROR. MENU CAN'T BE CREATED. SIZES OF OPTIONS' NAMES ARRAY AND FUNCTIONS' ARRAY ARE DIFFERENT." << endl;
		_wsystem(L"pause");
		exit(-2);
	}

	for (unsigned short i = 0; i < optionNames.size(); i++)
		options.push_back(pair<wstring, void (*)(wstring)>(optionNames[i], optionFunctions[i]));
}

wconsoleMenu::wconsoleMenu(vector<wstring>& optionNames, void(*function)(wstring), wstring selectText = L"", wstring exitText = L"")
{
	menuCreator(optionNames, selectText, exitText);

	for (unsigned short i = 0; i < optionNames.size(); i++)
		options.push_back(pair<wstring, void (*)(wstring)>(optionNames[i], function));
}

void wconsoleMenu::drawMenu(vector<pair<wstring, void (*)(wstring)>>& options, wstring filterText)
{
	_wsystem(L"cls");

	if (!isSelectTextEmpty)
	{
		point.X = point.Y = 0;
		wcout << selectText << filterText;
	}

	point.X = 2;
	for (unsigned short i = 0; i < options.size(); i++)
	{
		point.Y = (short)!isSelectTextEmpty + i;
		SetConsoleCursorPosition(consoleHandle, point);

		if (i != currentPosition)
			wcout << options[i].first;
	}

	point.Y = (short)!isSelectTextEmpty + currentPosition;
	SetConsoleCursorPosition(consoleHandle, point);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_WHITE);
	wcout << options[currentPosition].first;
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_BLACK);
}

void wconsoleMenu::redrawMenu(vector<pair<wstring, void (*)(wstring)>>& options, short action)
{
	short previousPosition = static_cast<short>((options.size() + currentPosition - action) % options.size()); //Find previous position number

	point.X = 2; //Set space from border
	point.Y = (short)!isSelectTextEmpty + previousPosition; //Set position of the cursor on previous option
	SetConsoleCursorPosition(consoleHandle, point);
	wcout << options[previousPosition].first; //Draw previous option with black background

	point.Y = (short)!isSelectTextEmpty + currentPosition; //Set position of the cursor on current option
	SetConsoleCursorPosition(consoleHandle, point);

	//Draw previous option with white background
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_WHITE);
	wcout << options[currentPosition].first;
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_BLACK);
}

bool wconsoleMenu::selectController(vector<pair<wstring, void (*)(wstring)>>& options, wstring filterText = L"")
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
			wcin.ignore(LLONG_MAX, '\n');
			while (_kbhit()) ignore = _getch();

			onCursor();
			_wsystem(L"cls");

			return true;
		}

		if (GetAsyncKeyState(VK_BACK))
		{
			while (_kbhit()) ignore = _getch();

			currentPosition = 0;
			onCursor();
			_wsystem(L"cls");

			return false;
		}
	}
}

short wconsoleMenu::singleSelect()
{
	if (options.size() == 0) return -1;
	currentPosition = 0;

	if (!isExitTextEmpty) options.push_back(pair<wstring, void (*)(wstring)>(exitText, [](wstring) {}));
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

void wconsoleMenu::cyclicSelect()
{
	if (options.size() == 0) return;

	if (!isExitTextEmpty) options.push_back(pair<wstring, void (*)(wstring)>(exitText, [](wstring) {}));
	do
	{
		if (selectController(options))
			options[currentPosition].second(options[currentPosition].first);
		else
			return;
	} while (isExitTextEmpty || currentPosition != options.size() - 1);
	if (!isExitTextEmpty) options.pop_back();

	currentPosition = 0;
}

vector<pair<wstring, void (*)(wstring)>> wconsoleMenu::optionsFilterer(wstring& filterText)
{
	vector<pair<wstring, void (*)(wstring)>> filteredOptions;

	for (int i = 0; i < options.size(); i++)
	{
		if (filterText.length() <= options[i].first.length() && filterText == options[i].first.substr(0, filterText.length()))
			filteredOptions.push_back(options[i]);
	}
	if (!isExitTextEmpty) filteredOptions.push_back(pair<wstring, void (*)(wstring)>(exitText, [](wstring) {}));

	return filteredOptions;
}

void wconsoleMenu::displayFilteredOptions(wstring& filterText, vector<pair<wstring, void (*)(wstring)>>& filteredOptions)
{
	_wsystem(L"cls");
	wcout << selectText << filterText;

	point.X = 2;
	if (filteredOptions.size() > 0)
	{
		for (int i = 0; i < filteredOptions.size(); i++)
		{
			point.Y = 1 + i;
			SetConsoleCursorPosition(consoleHandle, point);
			wcout << filteredOptions[i].first;
		}
	}
	else
	{
		point.Y = 1;
		SetConsoleCursorPosition(consoleHandle, point);
		wcout << L"Nothing not found! Try something else.";
	}

	point.X = selectText.length() + filterText.length();
	point.Y = 0;
	SetConsoleCursorPosition(consoleHandle, point);
}

short wconsoleMenu::singleSelectWithFilter()
{
	if (options.size() == 0) return -1;
	if (isSelectTextEmpty) isSelectTextEmpty = false;
	currentPosition = 0;

	wstring filterText = L"";
	vector<pair<wstring, void (*)(wstring)>> filteredOptions = optionsFilterer(filterText);

	displayFilteredOptions(filterText, filteredOptions);
	for (;; Sleep(10))
	{
		wchar_t wch;

		if (wch = _getwch())
		{
			if (wch == 27)
			{
				_wsystem(L"cls");
				isSelectTextEmpty = (selectText == L"");
				return -1;
			}
			else if (wch == 13)
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
				if (wch == 8)
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
					if ((char)wch == -32)
					{
						ignore = _getwch();
						continue;
					}

					filterText += wch;

					filteredOptions = optionsFilterer(filterText);
					displayFilteredOptions(filterText, filteredOptions);
				}
			}
		}
	}
}

void wconsoleMenu::cyclicSelectWithFilter()
{
	if (options.size() == 0) return;
	if (isSelectTextEmpty) isSelectTextEmpty = false;

	wstring filterText = L"";
	vector<pair<wstring, void (*)(wstring)>> filteredOptions = optionsFilterer(filterText);

	displayFilteredOptions(filterText, filteredOptions);
	for (;; Sleep(10))
	{
		wchar_t wch;

		if (wch = _getwch())
		{
			if (wch == 27)
			{
				_wsystem(L"cls");
				isSelectTextEmpty = (selectText == L"");
				return;
			}
			else if (wch == 13)
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
				if (wch == 8)
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
					if ((char)wch == -32)
					{
						ignore = _getwch();
						continue;
					}

					filterText += wch;

					filteredOptions = optionsFilterer(filterText);
					displayFilteredOptions(filterText, filteredOptions);
				}
			}
		}
	}
}