// winAPItest.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "winAPItest.h"
#include <windowsx.h>
#include <cmath>

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

bool drawLine(HDC hdc, int x0, int y0, int x, int y)
{
	POINT pt;
	MoveToEx(hdc, x0, y0, &pt);
	return LineTo(hdc, x, y);
}

bool drawBrickDim(HDC hdc, int x, int y, int w, int h, int d, COLORREF color)
{
	int ** arr = new int*[7];
	for (int i = 0; i < 7; ++i)
		arr[i] = new int[4];

	for (int i = 0; i < 7; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (j == 3)
				arr[i][j] = 1;
			else if (i > 2 && j == ) //передний план
				arr[i][j] += 
		}
	}

}

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
	GetClientRect(hWnd, &rcWorkArea);
	static int xClient, yClient;
	//xClient = rcWnd.left;
	//yClient = rcWnd.bottom - rcWorkArea.bottom;

	GetClientRect(hWnd, &rcWorkArea);
	static int width = rcWorkArea.right;
	static int height = rcWorkArea.bottom;

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
			int l = xClient / 10,
				r = xClient - l,
				t = yClient / 10,
				b = yClient - t,
				yc = yClient / 2,
				xc = xClient / 2;
			drawLine(hdc, l, t, r, t);
			drawLine(hdc, r, t, r, b);
			drawLine(hdc, r, b, l, b);
			drawLine(hdc, l, b, l, t);

			int x0 = xc + 0.5 * (r - l),
				y0 = yc,
				x = xc + 0.5 * (r - l) * cos(2 * 3.14 * 0.25),
				y = yc - 0.5 * (b - t) * sin(2 * 3.14 * 0.25);
			drawLine(hdc, xc, yc, x0, y0);
			HBRUSH newBr = CreateSolidBrush(RGB(255, 0, 0));
			HBRUSH  oldBr = (HBRUSH)SelectObject(hdc, newBr);
			Pie(hdc, l, t, r, b, x0, y0, x, y);
			x0 = x;
			y0 = y;
			x = xc + 0.4 * xClient * cos(2 * 3.14 * 0.125);
			y = yc - 0.4 * yClient * sin(2 * 3.14 * 0.125);
			TextOut(hdc, x, y, L"25%", 3);

			x = xc + 0.5 * (r - l) * cos(2 * 3.14 * 0.90);
			y = yc - 0.5 * (b - t) * sin(2 * 3.14 * 0.90);
			drawLine(hdc, xc, yc, x0, y0);
			newBr = CreateSolidBrush(RGB(0, 255, 0));
			SelectObject(hdc, newBr);
			Pie(hdc, l, t, r, b, x0, y0, x, y);
			x0 = x;
			y0 = y;
			x = xc + 0.4 * xClient * cos(2 * 3.14 * 0.575);
			y = yc - 0.4 * yClient * sin(2 * 3.14 * 0.575);
			TextOut(hdc, x, y, L"65%", 3);

			x = xc + 0.5 * (r - l) * cos(2 * 3.14 * 1);
			y = yc - 0.5 * (b - t) * sin(2 * 3.14 * 1);
			drawLine(hdc, xc, yc, x0, y0);
			newBr = CreateSolidBrush(RGB(0, 0, 255));
			SelectObject(hdc, newBr);
			Pie(hdc, l, t, r, b, x0, y0, x, y);
			x = xc + 0.4 * xClient * cos(2 * 3.14 * 0.95);
			y = yc - 0.4 * yClient * sin(2 * 3.14 * 0.95);
			TextOut(hdc, x, y, L"10%", 3);

			SelectObject(hdc, oldBr);
			DeleteObject(newBr);
			/*
			LOGFONT lf;
			lf.lfHeight = curH;
			lf.lfWidth = curW;
			lf.lfCharSet = DEFAULT_CHARSET;
			lf.lfPitchAndFamily = DEFAULT_PITCH;
			lf.lfItalic = 0;
			lf.lfStrikeOut = 0;
			lf.lfWeight = FW_NORMAL;
			lf.lfEscapement = 0;
			lf.lfOrientation = 0;
			HFONT oldFont, nFont;
			nFont = CreateFontIndirect(&lf);
			oldFont = (HFONT)SelectObject(hdc, nFont);
			SelectObject(hdc, oldFont);
			DeleteObject(nFont);
			*/
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_SIZE:
	{
		/*
		GetClientRect(hWnd, &rcWorkArea);
		width = rcWorkArea.right - 40;
		height = rcWorkArea.bottom - 40;
		int oldH = curH;
		if (height / curStr > curH)
		{
			if (height / curStr < defH)
			{
				curStr = defStr;
				curH = height / curStr;
			}
			else
			{
				curH = defH;
				curStr = height / curH;
			}
		}
		else
		{
			int posStr = height / defH;
			if (posStr < defStr)
			{
				curStr = defStr;
				curH = height / curStr;
			}
			else
				curStr = height / defH;
		}
		if (width / charNum < defW)
			curW = width / 8;
		else
			curW = (double(curH) / defH) * defW;
		*/
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