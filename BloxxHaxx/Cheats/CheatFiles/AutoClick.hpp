#pragma once
#include <Windows.h>
#include <string>
#include <iostream>


bool AutoClick_Enabled = false;



void AutoClick_Cheat() {
	for (;;)
	{
		if (AutoClick_Enabled != true) { ExitThread(0); }

		Sleep(200);

		INPUT Input = { 0 };

		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, &Input, sizeof(INPUT));

		ZeroMemory(&Input, sizeof(INPUT));
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1, &Input, sizeof(INPUT));
	}
}
