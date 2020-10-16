#include "CKeyHook.h"

bool CKeyHook::isKeyDown(unsigned char iKey)
{
	return this->bKeyTable[iKey];
}

bool CKeyHook::isKeyReleased(unsigned char iKey)
{
	static bool bPressed[0xFF];
	if (!this->bKeyTable[iKey]) {
		if (bPressed[iKey])
			return !(bPressed[iKey] = false);
	}
	else bPressed[iKey] = true;

	return false;
}

bool CKeyHook::isKeyPressed(unsigned char iKey)
{
	static bool bPressed[0xFF];
	if (this->bKeyTable[iKey]) {
		if (!bPressed[iKey])
			return bPressed[iKey] = true;
	}
	else bPressed[iKey] = false;

	return false;
}

auto __stdcall CKeyHook::ClickHandler(UINT msg, WPARAM wParam, LPARAM lParam)->void
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		this->bKeyTable[VK_LBUTTON] = (msg == WM_LBUTTONDOWN);
		break;

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		this->bKeyTable[VK_RBUTTON] = (msg == WM_RBUTTONDOWN);
		break;

	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		this->bKeyTable[VK_MBUTTON] = (msg == WM_MBUTTONDOWN);
		break;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		bool bDown = (msg == WM_SYSKEYDOWN || msg == WM_KEYDOWN);
		int	iKey = (int)wParam;
		unsigned __int32 ScanCode = LOBYTE(HIWORD(lParam));

		this->bKeyTable[iKey] = bDown;

		switch (iKey)
		{
		case VK_SHIFT:
			if (ScanCode == MapVirtualKey(VK_LSHIFT, 0)) this->bKeyTable[VK_LSHIFT] = bDown;
			if (ScanCode == MapVirtualKey(VK_RSHIFT, 0)) this->bKeyTable[VK_RSHIFT] = bDown;
			break;

		case VK_CONTROL:
			if (ScanCode == MapVirtualKey(VK_LCONTROL, 0)) this->bKeyTable[VK_LCONTROL] = bDown;
			if (ScanCode == MapVirtualKey(VK_RCONTROL, 0)) this->bKeyTable[VK_RCONTROL] = bDown;
			break;

		case VK_MENU:
			if (ScanCode == MapVirtualKey(VK_LMENU, 0)) this->bKeyTable[VK_LMENU] = bDown;
			if (ScanCode == MapVirtualKey(VK_RMENU, 0)) this->bKeyTable[VK_RMENU] = bDown;
			break;
		}
		break;
	}
	}
}