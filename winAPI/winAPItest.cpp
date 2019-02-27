// winAPItest.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "winAPItest.h"
#include <windowsx.h>
#include <cmath>
#include "helpFunc.h"

#define ID_EDIT 1000
#define ID_LIST 2000
#define ID_COMBOBOX 3000
#define ID_BUT1 4001
#define ID_BUT2 4002
#define ID_BUT3 4003
#define ID_BUT4 4004

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
TCHAR szMainClass[] = L"MainClass";
TCHAR szPopupClass[] = L"PopupClass";
TCHAR szChildClass[] = L"ChildClass";

ATOM                RegisterClass(HINSTANCE hInstance, WNDPROC Proc, LPCTSTR szName);
LRESULT CALLBACK    MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	PopupWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ChildWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK CountWndProc(HWND hwnd, LPARAM lParam);
int AmountWindows();
int count = 0;



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	RegisterClass(hInstance, MainWndProc, szMainClass);
	RegisterClass(hInstance, PopupWndProc, szPopupClass);
	RegisterClass(hInstance, ChildWndProc, szChildClass);
	// Выполнить инициализацию приложения:
	hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной
	HWND hWnd;

	hWnd = CreateWindowW(szMainClass, L"winAPITest", WS_OVERLAPPEDWINDOW,
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
		return FALSE;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPITEST));
    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM RegisterClass(HINSTANCE hInstance, WNDPROC Proc, LPCTSTR szName)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = Proc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAPITEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	if (szName == L"ChildClass")
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOWTEXT);
	else
		wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
    wcex.lpszClassName  = szName;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rcWnd, rcWorkArea;
	GetWindowRect(hWnd, &rcWnd);
	static int xClient, yClient;

	GetClientRect(hWnd, &rcWorkArea);
	static int width, height; 
	width = rcWorkArea.right;
	height = rcWorkArea.bottom;

	static int cxChar;
	static TCHAR* str = L"Привет!";
	static int charNum = wcslen(str + 1);
	static int defW = 40;
	static int defH = 60;
	static int defStr = 5; 
	static int curW = defW;
	static int curH = defH;
	static int curStr = defStr;
    switch (message)
    {
	case WM_CREATE:
		{

		}
		break;
    case WM_COMMAND:
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			int l = xClient / 20,
				r = 0.6 * xClient,
				t = yClient / 20,
				b = yClient - t,
				yc = 10,
				xc = 10,
				x0 = (r - l) / 2 + l,
				y0 = (b - t) / 2 + t;
			create2DGrid(hdc, l, t, r, b, xc, yc, x0, y0);

			matrix line = matrix(2, 3);
			line(0, 0) = 0;
			line(0, 1) = 0;
			line(0, 2) = 1;
			line(1, 0) = 8;
			line(1, 1) = 8;
			line(1, 2) = 1;

			matrix tM = matrix(3, 3);
			tM(0, 0) = 1;
			tM(0, 1) = 0;
			tM(0, 2) = 0;
			tM(1, 0) = 0;
			tM(1, 1) = 1;
			tM(1, 2) = 0;
			tM(2, 0) = 2;
			tM(2, 1) = 0;
			tM(2, 2) = 1;
			drawLine(hdc, line(0, 0) * xc + x0, -1 * line(0, 1) * yc + y0, line(1, 0) * xc + x0, -1 * line(1, 1) * yc + y0);
			line = line * tM;
			drawLine(hdc, line(0, 0) * xc + x0, -1 * line(0, 1) * yc + y0, line(1, 0) * xc + x0, -1 * line(1, 1) * yc + y0);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_SIZE:
	{
		xClient = LOWORD(lParam);
		yClient = HIWORD(lParam);
	}
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK PopupWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetWindowRect(hwnd, &rc);
	switch (msg)
	{
	case WM_LBUTTONUP:
		break;
	default: 
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetWindowRect(hwnd, &rc);
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		//SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_HIDEWINDOW | SWP_NOMOVE);
		break;
	case WM_SYSCOMMAND:
		if (wParam == SC_RESTORE)
		{
			MessageBeep(MB_ICONASTERISK);
			WINDOWPLACEMENT pl;
			pl.length = sizeof(WINDOWPLACEMENT);
			SetRect(&pl.rcNormalPosition, 0, 0, rc.right - rc.left, 500);
			SetWindowPlacement(hwnd, &pl); 
		}
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

BOOL CALLBACK CountWndProc(HWND hwnd, LPARAM lParam)
{
	TCHAR winClass[50] = {0};
	GetClassName(hwnd, winClass, 50);
	if (!wcscmp(winClass, szMainClass)) ++count;
	return true;
}

int AmountWindows()
{
	EnumWindows(CountWndProc, 0L);
	return count;
}