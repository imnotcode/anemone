﻿// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#define OEMRESOURCE
#include "targetver.h"

#pragma warning(disable: 4244) // wstring-string 경고
#pragma warning(disable: 4996) // wcscpy

// Windows 헤더 파일:
#include <windows.h>
#include <process.h>

// C 런타임 헤더 파일입니다.
#include <ios>
#include <tchar.h>
#include <vector>
#include <regex>

#include "Library.h"

#include "TransEngine.h"
#include "TextRenderer.h"
#include "TextProcess.h"
#include "Hotkey.h"
#include "Config.h"
#include "FileWatch.h"
#include "Remocon.h"

#include "Resource.h"

// GDIPlus
#include <GdiPlus.h>
#pragma comment(lib,"GdiPlus.lib")
using namespace Gdiplus;

// WinInet
#include <WinInet.h>
#pragma comment(lib, "WinInet.lib")

// STL
#include <string>
#include <sstream>
#include <hash_map>

// control
#include <commctrl.h>
#pragma comment(linker,"/manifestdependency:\" type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0'  processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"") 

// MinTrackSize
#define WND_MINTRACKSIZE 90

extern HINSTANCE hInst;

// hWnd 모음
struct _hWnds
{
	HWND About;
	HWND Main;
	HWND Parent;
	HWND Setting;
	HWND Remocon;
	HWND View;
	HWND Clip;
	HWND Trans;
	HWND FileTrans;
};
extern _hWnds hWnds;
extern HANDLE AneHeap;
extern int IsActive;

// 자석 모드
struct _MagnetWnd
{
	HANDLE hThread;
	bool IsMagnet = false;
	bool IsMinimize = false;
	HWND hWnd;
	int res_x;
	int res_y;
	int res_c;
	int diff_x;
	int diff_y;
	int rect_x;
	int rect_y;
	bool IsTopMost;
};
extern _MagnetWnd MagnetWnd;

// 클래스 모음
struct _Class
{
	CTextRenderer *TextRenderer;
	CTransEngine *TransEngine;
	CTextProcess *TextProcess;
	CHotkey *Hotkey;
	CConfig *Config;
	CFileWatch *FileWatch;
	CRemocon *Remocon;
};
extern _Class Cl;

struct render_attr
{
	FontFamily *Font;
	int Size_A;
	int Size_B;
	int Size_C;
	int Size_S;
};

struct _key_map
{
	bool Ctrl = false;
	bool Alt = false;
	bool Shift = false;
	DWORD Code;
	DWORD func;
	bool Always = false;
};

extern std::vector<_key_map> key_map;

static render_attr *rdName;
static render_attr *rdOriginal;
static render_attr *rdTrans;

INT IsWritableMemory(LPVOID pMemoryAddr);
INT IsReadableMemory(LPVOID pMemoryAddr);

typedef struct {
	DWORD pid;
	HWND hwnd;
} WINDOWPROCESSINFO;

UINT CALLBACK CCHookProc(HWND, UINT, WPARAM, LPARAM);
UINT CALLBACK CFHookProc(HWND, UINT, WPARAM, LPARAM);

extern int Elapsed_Prepare;
extern int Elapsed_Translate;

// WININET
typedef struct
{
	HINTERNET hInternet;
	HINTERNET hURL;
	HINTERNET hRequest;
	char *strHeaders;
	char *szPostData;
} PARM;

typedef struct
{
	wchar_t lpszInputFileName[260];
	wchar_t lpszOutputFileName[260];
	int WriteType;
	int StartTickCount;
} FILETRANS;
/*
struct dicStruct
{
	wchar_t *lpwszJpn;
	wchar_t *lpwszKor;
	char *lpszJpn;
	char *lpszKor;
	char *lpszPart;
	char *lpszAttr;
	bool bHidden;
};
extern std::vector<dicStruct> DICT;
*/
struct aneDicStruct
{
	int line;
	int type;
	wchar_t wjpn[31];
	wchar_t wkor[31];
	bool uc_vaild;
	char hidden;
	char jpn[31];
	char kor[31];
	char part[5];
	char attr[42];
	int operator<(aneDicStruct rhs) { return ((wcscmp(wjpn, rhs.wjpn) > 0) || (wcscmp(wjpn, rhs.wjpn) == 0) && (line < rhs.line)); }
};
extern std::vector<aneDicStruct> AneDic;

typedef struct
{
	int filesize;
	void *m_loc;
} OLDFILEINFO;

extern NOTIFYICONDATA niData;
extern UINT WM_TASKBARCHANGED;

struct _viewLog
{
	std::wstring wName;
	std::wstring wNameT;
	std::wstring wText;
	std::wstring wTextT;
};

extern std::vector<_viewLog> viewLog;
extern int viewLogNum;