// winAPItest.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "winAPItest.h"
#include <windowsx.h>
#include <cmath>
#include <string>
#include <vector>
#include "helpFunc.h"

#define PI 3.14

using namespace std;
// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
TCHAR szMainClass[] = L"MainClass";

ATOM                RegisterClass(HINSTANCE hInstance, WNDPROC Proc, LPCTSTR szName);
LRESULT CALLBACK    MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	PopupWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ChildWndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	RegisterClass(hInstance, MainWndProc, szMainClass);
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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

	static double x0mod, y0mod;
	static coordDescr d;

	int minC = 10, maxC = 60, defC = 40;
	double minPos = 0.25, maxPos = 0.75, defPos = 0.5;

	static matrix fig(8, 4);
	static vector<vector<int> > edges(6);
	for (size_t i = 0; i < 6; ++i)
		edges[i].resize(4);
	static matrix planes(4, 6);
	static vector<int> visibleEdges;
	static matrix testPoint (1, 4);
	static bool isLBDown, isRBDown;
	static int oldX, oldY;
	static double aX = 0, aY = 0, aZ = 0;
	switch (message)
	{
	case WM_CREATE:
	{
		//инициализация данных
		x0mod = 0.5;
		y0mod = 0.5;
		oldX = width * 0.5;
		oldY = height * 0.5;
		isLBDown = false;
		isRBDown = false;
		d.cy = 40;
		d.cx = 40;
		d.cz = 40;

		fig(0, 0) = -2;					fig(0, 1) = -2;					fig(0, 2) = -2;				 fig(0, 3) = 1;
		fig(6, 0) = 1;					fig(6, 1) = 2;					fig(6, 2) = 2;				 fig(6, 3) = 1;

		fig(1, 0) = fig(6, 0);			fig(1, 1) = fig(0, 1);			fig(1, 2) = fig(0, 2);		 fig(1, 3) = 1;
		fig(2, 0) = fig(6, 0);			fig(2, 1) = fig(0, 1);			fig(2, 2) = fig(6, 2);		 fig(2, 3) = 1;
		fig(3, 0) = fig(0, 0);			fig(3, 1) = fig(0, 1);			fig(3, 2) = fig(6, 2);		 fig(3, 3) = 1;
		fig(4, 0) = fig(0, 0);			fig(4, 1) = fig(6, 1);		    fig(4, 2) = fig(0, 2);		 fig(4, 3) = 1;
		fig(5, 0) = fig(6, 0);			fig(5, 1) = fig(6, 1);			fig(5, 2) = fig(0, 2);		 fig(5, 3) = 1;
		fig(7, 0) = fig(0, 0);			fig(7, 1) = fig(6, 1);			fig(7, 2) = fig(6, 2);		 fig(7, 3) = 1;

		testPoint(0, 0) = 0;			testPoint(0, 1) = 0;			testPoint(0, 2) = -1;			testPoint(0, 3) = 0;
		
		edges[0][0] = 0;			edges[0][1] = 1;			edges[0][2] = 2;		 edges[0][3] = 3;
		edges[1][0] = 2;			edges[1][1] = 3;			edges[1][2] = 7;		 edges[1][3] = 6;
		edges[2][0] = 0;			edges[2][1] = 3;			edges[2][2] = 7;		 edges[2][3] = 4;
		edges[3][0] = 0;			edges[3][1] = 4;		    edges[3][2] = 5;		 edges[3][3] = 1;
		edges[4][0] = 1;			edges[4][1] = 2;			edges[4][2] = 6;		 edges[4][3] = 5;
		edges[5][0] = 4;			edges[5][1] = 5;			edges[5][2] = 6;		 edges[5][3] = 7;

	}
	break;
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		if ((x > d.l) && (x < d.r) && (y < d.b) && (y > d.t))
		{
			isLBDown = true;
			oldX = x;
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		if ((x > d.l) && (x < d.r) && (y < d.b) && (y > d.t))
		{
			isRBDown = true;
			oldY = y;
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		isLBDown = false;
	}
	break;
	case WM_RBUTTONUP:
	{
		isRBDown = false;
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (isLBDown)
		{
			int newX = LOWORD(lParam);
			int newY = HIWORD(lParam);
			if (abs(oldY - newY) > 0 || abs(oldX - newX) > 0)
			{
				if (abs(oldY - newY) > 0)
				{
					aY = -(oldY - newY);
					oldY = newY;
				}
				if (abs(oldX - newX) > 0)
				{
					aX = -(oldX - newX);
					oldX = newX;
				}
				RECT r;
				r.bottom = d.b;
				r.top = d.t;
				r.right = d.r;
				r.left = d.l;
				RedrawWindow(hWnd, &r, NULL, RDW_ERASE | RDW_INVALIDATE);
			}
		}
		if (isRBDown)
		{
			int newX = LOWORD(lParam);
			if (!isLBDown && abs(oldX - newX) > 0)
			{
				aZ = (oldX - newX);
				oldX = newX;
				RECT r;
				r.bottom = d.b;
				r.top = d.t;
				r.right = d.r;
				r.left = d.l;
				RedrawWindow(hWnd, &r, NULL, RDW_ERASE | RDW_INVALIDATE);
			}
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		d.l = xClient / 20;
		d.r = 0.95 * xClient;
		d.t = yClient / 20;
		d.b = yClient - d.t;
		d.x0 = (d.r - d.l) * x0mod + d.l;
		d.y0 = (d.b - d.t) * y0mod + d.t;

		visibleEdges.clear();
		planes = calcBodyMatrix(fig, edges);
		double dArr[4][6];
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 6; ++j)
				dArr[i][j] = planes(i, j);
		}

		//changes
		matrix change(4, 4);
		change.fill(0);
		for (int i = 0; i < 4; ++i)
			change(i, i) = 1;
		if (isLBDown)
		{
			double a = aY / 180 * PI;
			matrix iso(4, 4);
			iso(0, 0) = 1;  iso(0, 1) = 0;		 iso(0, 2) = 0;		 iso(0, 3) = 0;
			iso(1, 0) = 0;  iso(1, 1) = cos(a);  iso(1, 2) = sin(a); iso(1, 3) = 0;
			iso(2, 0) = 0;  iso(2, 1) = -sin(a); iso(2, 2) = cos(a); iso(2, 3) = 0;
			iso(3, 0) = 0;  iso(3, 1) = 0;		 iso(3, 2) = 0;		 iso(3, 3) = 1;
			change = iso;
			a = aX / 180 * PI;
			iso(0, 0) = cos(a);  iso(0, 1) = 0;  iso(0, 2) = -sin(a); iso(0, 3) = 0;
			iso(1, 0) = 0;		 iso(1, 1) = 1;  iso(1, 2) = 0;		  iso(1, 3) = 0;
			iso(2, 0) = sin(a);  iso(2, 1) = 0;  iso(2, 2) = cos(a);  iso(2, 3) = 0;
			iso(3, 0) = 0;		 iso(3, 1) = 0;  iso(3, 2) = 0;		  iso(3, 3) = 1;
			change = change * iso;
		}
		else if (isRBDown)
		{
			double a = aZ / 180 * PI;
			matrix iso(4, 4);
			iso(0, 0) = cos(a);  iso(0, 1) = sin(a);  iso(0, 2) = 0;	iso(0, 3) = 0;
			iso(1, 0) = -sin(a); iso(1, 1) = cos(a);  iso(1, 2) = 0;	iso(1, 3) = 0;
			iso(2, 0) = 0;		 iso(2, 1) = 0;		  iso(2, 2) = 1;	iso(2, 3) = 0;
			iso(3, 0) = 0;		 iso(3, 1) = 0;		  iso(3, 2) = 0;	iso(3, 3) = 1;
			change = iso;
		}
		double dArr2[4][4];
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				dArr2[i][j] = change(i, j);

		fig = fig * change;
		planes = change.reverse() * planes;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 6; ++j)
				dArr[i][j] = planes(i, j);
		}


		//test fo visibility
		matrix check = testPoint * planes;
		for (int i = 0; i < 6; ++i)
			if (check(0, i) > 0)
				visibleEdges.push_back(i);

		double dArr1[1][6];
		for (int i = 0; i < 6; ++i)
			dArr1[0][i] = check(0, i);

		//print
		createFrame(hdc, d);
		matrix figDraw(8, 4);
		figDraw = fig;
		for (int v = 0; v < visibleEdges.size(); ++v)
		{
			for (int p = 0; p < 4; ++p)
			{
				int fP = edges[visibleEdges[v]][p];
				int sP = edges[visibleEdges[v]][(p + 1) % 4];
				drawLine(hdc, figDraw(fP, 0), figDraw(fP, 1), figDraw(sP, 0), figDraw(sP, 1), d);
			}
		}
		EndPaint(hWnd, &ps);
		}
		break;
	case WM_SIZE:
	{
		xClient = LOWORD(lParam);
		yClient = HIWORD(lParam);
	}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}