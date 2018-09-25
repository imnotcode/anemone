#include "stdafx.h"
#include "Hotkey.h"

CHotkey *CHotkey::m_pThis = NULL;

CHotkey::CHotkey()
{
	m_pThis = this;

	SECURITY_ATTRIBUTES ThreadAttributes;
	ThreadAttributes.bInheritHandle = false;
	ThreadAttributes.lpSecurityDescriptor = NULL;
	ThreadAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);

	hHotkeyThread = CreateThread(&ThreadAttributes, 0, HotkeyThread, NULL, 0, NULL);
	if (hHotkeyThread == NULL)
	{
		MessageBox(0, L"������ ���� �۾��� �����߽��ϴ�.", 0, MB_ICONERROR);
	}
}

CHotkey::~CHotkey()
{
	RemoveHook();

	TerminateProcess(hHotkeyThread, 0);
}

DWORD CHotkey::_HotkeyThread(LPVOID lpParam)
{
	MSG msg;
	HACCEL hAccelTable;

	hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_ANEMONE));

	InstallHook();
	LoadKeyMap();

	// �⺻ �޽��� �����Դϴ�.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

bool CHotkey::LoadKeyMap()
{
	key_map.erase(key_map.begin(), key_map.end());

	// �ӽ�
	RegKey(VK_F6, false, false, false, ID_TEMP_CLICK_THOUGH);

	RegKey(VK_F7, false, false, false, ID_WND_BORDER_MODE);
	RegKey(VK_F7, false, true, false, ID_BACKGROUND_SWITCH);

	RegKey(VK_F9, false, false, false, ID_WINDOW_BACKLOG);

	// ���� ����/�̸� ���
	RegKey(VK_F8, false, false, false, ID_PRINT_ORGTEXT);
	RegKey(VK_F8, false, true, false, ID_PRINT_ORGNAME);

	// �̸� �и� ǥ��
	RegKey(VK_F8, true, false, false, ID_SEPERATE_NAME);

	RegKey(VK_F10, false, false, false, ID_WINDOW_SETTING, true);

	// ����
	RegKey(VK_F12, false, true, false, ID_TERMINATE_ANEMONE);

	// ������ / �ֱٱ� ����
	RegKey(VK_NUMPAD9, false, false, false, ID_TEXT_PREV);
	RegKey(VK_NUMPAD3, false, false, false, ID_TEXT_NEXT);

	// ���� ũ�� UP/DOWN
	RegKey(VK_SUBTRACT, false, false, false, ID_TEXTSIZE_MINUS);
	RegKey(VK_ADD, false, false, false, ID_TEXTSIZE_PLUS);
	
	// â ��� ����� / â ���� �����
	RegKey(VK_MULTIPLY, false, false, false, ID_WINDOW_VISIBLE, true);
	RegKey(VK_DIVIDE, false, false, false, ID_TEMP_WINDOW_HIDE, true);

	// â ��ġ ����
	RegKey(VK_NUMPAD5, true, false, false, ID_WINRESET);

	// â �̵�
	RegKey(VK_NUMPAD8, false, false, false, ID_WNDMOVE_TOP);
	RegKey(VK_NUMPAD2, false, false, false, ID_WNDMOVE_BOTTOM);
	RegKey(VK_NUMPAD4, false, false, false, ID_WNDMOVE_LEFT);
	RegKey(VK_NUMPAD6, false, false, false, ID_WNDMOVE_RIGHT);

	// â ũ�� ����
	RegKey(VK_NUMPAD8, false, true, false, ID_WNDSIZE_TOP);
	RegKey(VK_NUMPAD2, false, true, false, ID_WNDSIZE_BOTTOM);
	RegKey(VK_NUMPAD4, false, true, false, ID_WNDSIZE_LEFT);
	RegKey(VK_NUMPAD6, false, true, false, ID_WNDSIZE_RIGHT);

	// �ܹ� ����/���� ����â
	RegKey(VK_F11, false, false, true, ID_WINDOW_TRANS);
	RegKey(VK_F11, false, true, true, ID_WINDOW_FILETRANS);

	// ���� ����Ű
	RegKey(VK_F12, true, false, true, ID_EXTERN_HOTKEY, true);

	// ���� ����
	RegKey(VK_INSERT, false, false, true, ID_OPENDIC, true);

	// �ڼ����
	RegKey(VK_OEM_3, false, false, false, ID_MAGNETIC_MODE, true);

	// ���콺 Ŭ��
	RegKey(VK_NUMPAD0, false, false, false, ID_MOUSE_LCLICK, true);
	RegKey(VK_DECIMAL, false, false, false, ID_MOUSE_RCLICK, true);

	// â ����
	RegKey(VK_F9, false, false, true, ID_TRANSTEXT_WNDMENU);
	RegKey(VK_F10, false, false, true, ID_TRANSTEXT_WNDTEXT);

	// Ŭ������ ����/��Ŀ ���� ����
	RegKey(VK_SCROLL, false, false, false, ID_CLIPBOARD_SWITCH, true);
	RegKey(VK_OEM_5, true, true, false, ID_HOOKER_MONITOR, true);
	return true;
}

inline void CHotkey::RegKey(int code, bool bCtrl, bool bAlt, bool bShift, int func, bool bAlways)
{
	struct _key_map kmap;
	kmap.Alt = bAlt;
	kmap.Shift = bShift;
	kmap.Ctrl = bCtrl;
	kmap.Code = code;
	kmap.func = func;
	kmap.Always = bAlways;
	key_map.push_back(kmap);
}

inline void CHotkey::UnregKey(int code)
{
	std::vector<_key_map>::iterator it = key_map.begin();
	for (; it != key_map.end(); it++)
	{
		if ((*it).Code == code)
		{
			key_map.erase(it);
			it = key_map.begin();
		}
	}
}

inline void CHotkey::UnregFuncKey(int func)
{
	std::vector<_key_map>::iterator it = key_map.begin();
	for (; it != key_map.end(); it++)
	{
		if ((*it).func == func)
		{
			key_map.erase(it);
			it = key_map.begin();
		}
	}
}

bool CHotkey::SaveKeyMap()
{
	//key_map.erase(key_map.begin(), key_map.end());
	return true;
}

bool CHotkey::InstallHook()
{
	m_pThis = this;
	m_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, _KeyboardProc, hInst, NULL);
	return true;
}

bool CHotkey::RemoveHook()
{
	if (m_hHook != NULL)
	{
		UnhookWindowsHookEx(m_hHook);
		m_hHook = NULL;
	}
	return true;
}

LRESULT CHotkey::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		PKBDLLHOOKSTRUCT pHookKey = (PKBDLLHOOKSTRUCT)lParam;
		std::vector<_key_map> key = key_map;

		switch(wParam)
		{
		case 256:	// WM_KEYDOWN
		case 260:	// WM_SYSKEYDOWN
			std::vector<_key_map>::iterator it = key.begin();
			for (; it != key.end(); it++)
			{
				if ((*it).Code == pHookKey->vkCode)
				{
					bool bCtrl = ((GetKeyState(VK_CONTROL) & 0x8000) ? true : false);
					bool bShift = ((GetKeyState(VK_SHIFT) & 0x8000) ? true : false);
					bool bAlt = ((GetKeyState(VK_MENU) & 0x8000) ? true : false);
					
					if ((*it).Alt != bAlt) continue;
					if ((*it).Shift != bShift) continue;
					if ((*it).Ctrl != bCtrl) continue;

					if ((*it).func == ID_EXTERN_HOTKEY ||
						Cl.Config->GetExternHotkey() &&
						(((*it).Always == true && Cl.Config->GetTempWinHide()) ||
						((*it).Always == true && Cl.Config->GetHideWinUnlockHotkey() != 2 ||
						(!Cl.Config->GetWindowVisible() && ((*it).func == ID_WINDOW_VISIBLE || (*it).func == ID_TEMP_WINDOW_HIDE)) ||
						Cl.Config->GetWindowVisible() ||
						Cl.Config->GetHideWinUnlockHotkey() == 0)))
					{
						SendMessage(hWnds.Main, WM_COMMAND, MAKELONG((*it).func, 0), 0);
						SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
						return -1;
					}
				}
			}

			break;
		}
		/*
		switch (wParam)
		{
			// pHookKey.flags = 0x20 ALT
			// pHookKey.flags = 0x00 CTRL
			// pHookKey.flags = 0x01 NONE


		case 256: // WM_KEYDOWN
			//case 257: // WM_KEYUP
		case 260: // WM_SYSKEYDOWN
			//case 261: // WM_SYSKEYUP
			if (g_UseHotKey == true)
			{
				if ((IsWindowVisible(hMainWnd) || !IsWindowVisible(hMainWnd) && f_hidewin_nohotkey == false))
				{
					if (pHookKey->vkCode == VK_F6)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_F6_WNDTRANSPARENT, 0);
						return 1;
					}
					if (pHookKey->vkCode == VK_F7)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_F7_BACKGROUNDTP, 0);
						return 1;
					}
					if (pHookKey->vkCode == VK_F8)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_F8_TOGGLEJPNSTR, pHookKey->flags);
						return 1;
					}
					if (pHookKey->vkCode == VK_F9)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_F9_LOGWND, pHookKey->flags);
						return 1;
					}
					if (pHookKey->vkCode == VK_F10)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_F10_SETTINGWND, pHookKey->flags);
						return 1;
					}

					if (pHookKey->vkCode == VK_NUMPAD9)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N9_REVIEWPREV, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_NUMPAD3)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N3_REVIEWNEXT, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_OEM_3 && !(GetKeyState(VK_SHIFT) & 0x8000))
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_MAGNETICMODE, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_SCROLL)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_CLIPBOARDWATCH, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_ADD)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_TEXTSIZEUP, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_SUBTRACT)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_TEXTSIZEDOWN, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_DIVIDE)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_REFRESHWND, 0);
						return 1;
					}

					// ����Ű â�̵� / ALT PRESS, ����Ű âũ�� ����
					if (pHookKey->vkCode == VK_NUMPAD4)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N4_LEFT, pHookKey->flags & 0x20);
						return 1;
					}

					if (pHookKey->vkCode == VK_NUMPAD6)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N6_RIGHT, pHookKey->flags & 0x20);
						return 1;
					}

					if (pHookKey->vkCode == VK_NUMPAD8)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N8_TOP, pHookKey->flags & 0x20);
						return 1;
					}

					if (pHookKey->vkCode == VK_NUMPAD2)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N2_BOTTOM, pHookKey->flags & 0x20);
						return 1;
					}
				}

				// â ����� ����Ű �Ͻ� ������ �۵����϶� NUM * Ű�� ���������� ���
				if (pHookKey->vkCode == VK_MULTIPLY)
				{
					PostMessage(hMainWnd, WM_HOTKEY, IDH_WNDSHOWTOGGLE, 0);
					return 1;
				}

			}

			if (pHookKey->vkCode == VK_F10 && (GetKeyState(VK_SHIFT) & 0x8000) && ((pHookKey->flags & 0x20) == 0x20))
			{
				PostMessage(hMainWnd, WM_HOTKEY, IDH_USE_HOTKEY, 0);
				return 1;
			}

			break;
			
		}*/
	}
	return CallNextHookEx(m_hHook, nCode, wParam, lParam);


}