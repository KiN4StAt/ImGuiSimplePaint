#pragma once

#include <Windows.h>

class CKeyHook
{
public:
	CKeyHook()
	{
		memset(bKeyTable, 0, 256);
	}

	bool isKeyDown(unsigned char iKey);
	bool isKeyReleased(unsigned char iKey);
	bool isKeyPressed(unsigned char iKey);

	auto __stdcall ClickHandler(UINT msg, WPARAM wParam, LPARAM lParam)->void;
private:
	bool bKeyTable[256];
	
	WNDPROC m_pWindowProc;
};