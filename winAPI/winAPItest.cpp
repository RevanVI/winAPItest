// winAPItest.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "winAPItest.h"
#include <windowsx.h>
#include <cmath>
#include "helpFunc.h"

#define ID_EDITCX 1000
#define ID_EDITCY 1001
#define ID_EDITCZ 1002
#define ID_EDITX 2000
#define ID_EDITY 2001
#define ID_MBUT0 3000
#define ID_MBUT1 3001
#define ID_MBUT2 3002
#define ID_EDITXF 4000
#define ID_EDITYF 4001
#define ID_EDITZF 4002
#define ID_EDITH 4003
#define ID_EDITW 4004
#define ID_EDITD 4005

#define PI 3.14
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

	static HWND titleLbl, 
		stepLbl, cxEdit, cyEdit, czEdit, 
		coordLbl, xEdit, yEdit, 
		modeLbl, mBut0, mBut1, mBut2,
		figLbl, xfEdit, yfEdit, zfEdit, hEdit, wEdit, dEdit,
		but1;
    switch (message)
    {
	case WM_CREATE:
		{
		double y = 0.05 * rcWnd.bottom;
		double x = 0.65 * rcWnd.right;
		titleLbl = CreateWindow(L"static", L"Настройки", WS_CHILD | WS_VISIBLE | SS_CENTER,
			x, y, 0.3 * rcWnd.right, 0.025 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		y += 0.025 * rcWnd.bottom + 0.025 * rcWnd.bottom;

		stepLbl = CreateWindow(L"static", L"Масштабы осей:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		cxEdit = CreateWindow(L"edit", L"40", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.1 * rcWnd.right, 0.1 * rcWnd.bottom, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITCX, hInst, NULL);
		cyEdit = CreateWindow(L"edit", L"40", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.125 * rcWnd.right, 0.1 * rcWnd.bottom, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITCY, hInst, NULL);
		czEdit = CreateWindow(L"edit", L"40", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.15 * rcWnd.right, 0.1 * rcWnd.bottom, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITCZ, hInst, NULL);

		y += 0.04 * rcWnd.bottom + 0.01 * rcWnd.bottom;
		coordLbl = CreateWindow(L"static", L"Начало координат:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		xEdit = CreateWindow(L"edit", L"0,5", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.1 * rcWnd.right, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITX, hInst, NULL);
		yEdit = CreateWindow(L"edit", L"0,5", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.125 * rcWnd.right, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITY, hInst, NULL);

		y += 0.04 * rcWnd.bottom + 0.01 * rcWnd.bottom;
		modeLbl = CreateWindow(L"static", L"Режим:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		mBut0 = CreateWindow(L"button", NULL, WS_CHILD | WS_BORDER | BS_GROUPBOX,
			x + 0.1 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_MBUT0, hInst, NULL);
		mBut1 = CreateWindow(L"button", L"Изометр.", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTORADIOBUTTON,
			x + 0.1 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_MBUT1, hInst, NULL);
		mBut1 = CreateWindow(L"button", L"Диметр.", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTORADIOBUTTON,
			x + 0.2 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_MBUT2, hInst, NULL);

		y += 0.04 * rcWnd.bottom + 0.04 * rcWnd.bottom;
		figLbl = CreateWindow(L"static", L"Параметры фигуры", WS_CHILD | WS_VISIBLE | SS_CENTER,
			x, y, 0.3 * rcWnd.right, 0.025 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		y += 0.04 * rcWnd.bottom;
		figLbl = CreateWindow(L"static", L"Координаты: ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		xfEdit = CreateWindow(L"edit", L"40", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.1 * rcWnd.right, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITXF, hInst, NULL);
		yfEdit = CreateWindow(L"edit", L"40", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.125 * rcWnd.right, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITYF, hInst, NULL);
		zfEdit = CreateWindow(L"edit", L"40", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.15 * rcWnd.right, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITZF, hInst, NULL);


		//but1 = CreateWindow(L"button", L"Open", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		//	0.9 * xClient, 0.9* yClient, 100, 100, hWnd, (HMENU)ID_BUT1, hInst, NULL);
		}
		break;
    case WM_COMMAND:
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			coordDescr d;
			d.l = xClient / 20,
			d.r = 0.6 * xClient,
			d.t = yClient / 20,
			d.b = yClient - d.t,
			d.cy = 20,
			d.cx = 20,
			d.x0 = (d.r - d.l) / 2 + d.l,
			d.y0 = (d.b - d.t) / 2 + d.t;

			bool mode = false;

			matrix dim(4, 4);
			dim(0, 0) = 0.925;  dim(0, 1) = -0.134;  dim(0, 2) = 0; dim(0, 3) = 0;
			dim(1, 0) = 0;      dim(1, 1) = 0.935;   dim(1, 2) = 0; dim(1, 3) = 0;
			dim(2, 0) = -0.378; dim(2, 1) = -0.327;  dim(2, 2) = 0; dim(2, 3) = 0;
			dim(3, 0) = 0;      dim(3, 1) = 0;       dim(3, 2) = 0; dim(3, 3) = 1;

			matrix iso(4, 4);
			iso(0, 0) = 0.707;  iso(0, 1) = -0.408;  iso(0, 2) = 0; iso(0, 3) = 0;
			iso(1, 0) = 0;      iso(1, 1) = 0.816;   iso(1, 2) = 0; iso(1, 3) = 0;
			iso(2, 0) = -0.707; iso(2, 1) = -0.408;  iso(2, 2) = 0; iso(2, 3) = 0;
			iso(3, 0) = 0;      iso(3, 1) = 0;       iso(3, 2) = 0; iso(3, 3) = 1;

			create3DGrid(hdc, d, mode);

			int x = 2, y = 2, z = 2;
			matrix fig(8, 4);
			fig(0, 0) = 4 + x; fig(0, 1) = 0 + y;  fig(0, 2) = 0 + z; fig(0, 3) = 1;
			fig(1, 0) = 8 + x; fig(1, 1) = 0 + y;  fig(1, 2) = 0 + z; fig(1, 3) = 1;
			fig(2, 0) = 8 + x; fig(2, 1) = 0 + y;  fig(2, 2) = 4 + z; fig(2, 3) = 1;
			fig(3, 0) = 4 + x; fig(3, 1) = 0 + y;  fig(3, 2) = 4 + z; fig(3, 3) = 1;
			fig(4, 0) = 4 + x; fig(4, 1) = 4 + y;  fig(4, 2) = 4 + z; fig(4, 3) = 1;
			fig(5, 0) = 8 + x; fig(5, 1) = 4 + y;  fig(5, 2) = 4 + z; fig(5, 3) = 1;
			fig(6, 0) = 8 + x; fig(6, 1) = 4 + y;  fig(6, 2) = 0 + z; fig(6, 3) = 1;
			fig(7, 0) = 4 + x; fig(7, 1) = 4 + y;  fig(7, 2) = 0 + z; fig(7, 3) = 1;

			matrix axis(2, 4); //ось поворота
			axis(0, 0) = 0; axis(0, 1) = 0; axis(0, 2) = 0; axis(0, 3) = 1; 
			axis(1, 0) = 12; axis(1, 1) = 0; axis(1, 2) = 0; axis(1, 3) = 1;
			double angle = 180 * PI / 180; //угол поворота

			fig = rotateFig(fig, axis, angle);
			if (mode)
			{
				fig = fig * iso;
				axis = axis * iso;
			}
			else
			{
				fig = fig * dim;
				axis = axis * dim;
			}
			drawLine(hdc, axis(0, 0), axis(0, 1), axis(1, 0), axis(1, 1), d);
			drawBrickDim(hdc, fig, d, RGB(0, 0, 0));
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