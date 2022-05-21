#pragma once
#define NOMINMAX
#include <windows.h>

class consoleParameters
{
protected:
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND consoleWindow = GetConsoleWindow();
	CONSOLE_CURSOR_INFO structCursorInfo{};
	COORD point{};
	void onCursor();
	void offCursor();
};

void consoleParameters::onCursor()
{
	structCursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(consoleHandle, &structCursorInfo);
}

void consoleParameters::offCursor()
{
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &structCursorInfo);
}