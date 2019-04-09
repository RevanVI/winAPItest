// winAPItest.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "winAPItest.h"
#include <windowsx.h>
#include <cmath>
#include <string>
#include <vector>
#include "helpFunc.h"

#define ID_EDITCX 1000
#define ID_EDITCY 1001
#define ID_EDITX 2000
#define ID_EDITY 2001
#define ID_MBUT0 3000

#define ID_BUT1 10000
#define PI 3.14

using namespace std;
// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
TCHAR szMainClass[] = L"MainClass";

ATOM                RegisterClass(HINSTANCE hInstance, WNDPROC Proc, LPCTSTR szName);
LRESULT CALLBACK    MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	PopupWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ChildWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
WNDPROC OldWndProc;

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

	static HWND titleLbl, 
		stepLbl, cxEdit, cyEdit, 
		coordLbl, xEdit, yEdit, 
		but1;

	static double x0mod, y0mod;
	static coordDescr d;

	int minC = 10, maxC = 60, defC = 40;
	double minPos = 0.25, maxPos = 0.75, defPos = 0.5;

	static matrix win(4, 2); //win coords
	static vector<matrix> p; //vector of segments
	static matrix newPoint;
	static int pNum, iNum;
	static bool isLBDown, isLBDblClk;
	switch (message)
	{
	case WM_CREATE:
	{
		//инициализация данных
		x0mod = 0.5;
		y0mod = 0.5;
		isLBDown = false;
		pNum = -1;
		d.cy = 40;
		d.cx = 40;
		d.cz = 40;
		win(0, 0) = 1;		win(0, 1) = 5;
		win(1, 0) = 5;		win(1, 1) = 5;
		win(2, 0) = 5;		win(2, 1) = 1;
		win(3, 0) = 1;		win(3, 1) = 1;
		newPoint = matrix(2, 2);

		double y = 0.05 * rcWnd.bottom;
		double x = 0.65 * rcWnd.right;
		titleLbl = CreateWindow(L"static", L"Настройки", WS_CHILD | WS_VISIBLE | SS_CENTER,
			x, y, 0.3 * rcWnd.right, 0.025 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		y += 0.025 * rcWnd.bottom + 0.01 * rcWnd.bottom;

		stepLbl = CreateWindow(L"static", L"Масштабы осей:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		cxEdit = CreateWindow(L"edit", L"40", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.1 * rcWnd.right, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITCX, hInst, NULL);
		cyEdit = CreateWindow(L"edit", L"40", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.125 * rcWnd.right, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITCY, hInst, NULL);

		y += 0.04 * rcWnd.bottom;
		coordLbl = CreateWindow(L"static", L"Начало координат:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		xEdit = CreateWindow(L"edit", L"0.5", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.1 * rcWnd.right, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITX, hInst, NULL);
		yEdit = CreateWindow(L"edit", L"0.5", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.125 * rcWnd.right, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITY, hInst, NULL);
		SendMessage(xEdit, EM_LIMITTEXT, 5, 0);
		SendMessage(yEdit, EM_LIMITTEXT, 5, 0);
		OldWndProc = (WNDPROC)SetWindowLong(xEdit, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(yEdit, GWL_WNDPROC, (LONG)EditWndProc);

		y += 0.05 * rcWnd.bottom;
		but1 = CreateWindow(L"button", L"Выполнить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			x + 0.1 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_BUT1, hInst, NULL);
	}
	break;
	case WM_COMMAND:
	{
		TCHAR buf[10];
		int size;
		if (LOWORD(wParam) >= ID_EDITCX && LOWORD(wParam) <= ID_EDITCY)
		{
			HWND* edit;
			if (HIWORD(wParam) == EN_KILLFOCUS)
			{
				if (LOWORD(wParam) == ID_EDITCX)
					edit = &cxEdit;
				if (LOWORD(wParam) == ID_EDITCY)
					edit = &cyEdit;
				size = SendMessage(*edit, EM_GETLINE, 0, (LPARAM)buf);
				long val = wcstol(buf, NULL, 10);
				if (size == 0 || val < minC || val > maxC)
				{
					if (val < minC)
						val = minC;
					else
						val = maxC;
					_ltow(val, buf, 10);
					SetWindowText(*edit, buf);
				}
			}
		}
		else if (LOWORD(wParam) == ID_EDITX || LOWORD(wParam) == ID_EDITY)
		{
			HWND* edit;
			if (HIWORD(wParam) == EN_KILLFOCUS)
			{
				if (LOWORD(wParam) == ID_EDITX)
					edit = &xEdit;
				if (LOWORD(wParam) == ID_EDITY)
					edit = &yEdit;
				size = SendMessage(*edit, EM_GETLINE, 0, (LPARAM)buf);
				double val = wcstod(buf, NULL);
				if (size == 0 || val < minPos || val > maxPos)
				{
					if (val < minPos)
						val = minPos;
					else
						val = maxPos;
					val = round(val * pow(10, 3)) / pow(10, 3);
					wstring str = to_wstring(val).substr(0, 4);
					SetWindowText(*edit, str.c_str());
				}
			}
		}
		else if (LOWORD(wParam) == ID_BUT1)
		{
			SendMessage(cxEdit, EM_GETLINE, 0, (LPARAM)buf);
			d.cx = wcstol(buf, NULL, 10);
			SendMessage(cyEdit, EM_GETLINE, 0, (LPARAM)buf);
			d.cy = wcstol(buf, NULL, 10);
			SendMessage(xEdit, EM_GETLINE, 0, (LPARAM)buf);
			x0mod = wcstod(buf, NULL);
			SendMessage(yEdit, EM_GETLINE, 0, (LPARAM)buf);
			y0mod = wcstod(buf, NULL);

			RECT r;
			r.bottom = d.b;
			r.top = d.t;
			r.right = d.r;
			r.left = d.l;
			RedrawWindow(hWnd, &r, NULL, RDW_ERASE | RDW_INVALIDATE);
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		if ((x > d.l) && (x < d.r) && (y < d.b) && (y > d.t))
		{
			int count = p.size();
			for (int i = 0; i < count && pNum == -1; ++i)
			{
				pNum = definePoint(p[i], x, y, d);
				if (pNum != -1) //if point exist
				{
					iNum = i;
					isLBDown = true;
				}
			}
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		if ((x > d.l) && (x < d.r) && (y < d.b) && (y > d.t))
		{
			for (int i = 0; i < p.size() && pNum == -1; ++i)
			{
				pNum = definePoint(p[i], x, y, d);
				if (pNum != -1) //if point exist
					p.erase(p.begin() + i);		
			}
		}
		if (pNum != -1)
		{
			pNum = -1;
			RECT r;
			r.bottom = d.b;
			r.top = d.t;
			r.right = d.r;
			r.left = d.l;
			RedrawWindow(hWnd, &r, NULL, RDW_ERASE | RDW_INVALIDATE);
		}
	}
	break;
	case WM_LBUTTONDBLCLK:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		if ((x > d.l) && (x < d.r) && (y < d.b) && (y > d.t))
		{
			if (!isLBDblClk)
			{
				newPoint(0, 0) = double(x - d.x0) / d.cx;
				newPoint(0, 1) = double(y - d.y0) / -d.cy;
				isLBDblClk = true;
			}
			else
			{
				newPoint(1, 0) = double(x - d.x0) / d.cx;
				newPoint(1, 1) = double(y - d.y0) / -d.cy;
				isLBDblClk = false;
				p.push_back(newPoint);

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
	case WM_LBUTTONUP:
	{
		isLBDown = false;
		pNum = -1;
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (isLBDown && (pNum >= 0))
		{
			int newX = LOWORD(lParam);
			int newY = HIWORD(lParam);

			HWND* edit;
			if (isLBDown)
			{
				p[iNum](pNum, 0) = double(newX - d.x0) / d.cx;
				p[iNum](pNum, 1) = double(newY - d.y0) / -d.cy;
			}
			RECT r;
			r.bottom = d.b;
			r.top = d.t;
			r.right = d.r;
			r.left = d.l;
			RedrawWindow(hWnd, &r, NULL, RDW_ERASE | RDW_INVALIDATE);
		}
	}
	break;
	case WM_PAINT:
		{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		d.l = xClient / 20;
		d.r = 0.6 * xClient;
		d.t = yClient / 20;
		d.b = yClient - d.t;
		d.x0 = (d.r - d.l) * x0mod + d.l;
		d.y0 = (d.b - d.t) * y0mod + d.t;

		create2DGrid(hdc, d);
		drawPol2Dim(hdc, win, RGB(0, 0, 0), d, true);

		for (int i = 0; i < p.size(); ++i)
		{
			drawPol2Dim(hdc, p[i], RGB(0, 0, 0), d, false);
			matrix visSeg = defineVisibleSegment(p[i], win);
			int m, n;
			visSeg.getDimens(m, n);
			if (m != 0)
				drawPol2Dim(hdc, visSeg, RGB(200, 0, 0), d, false);
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

LRESULT CALLBACK EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int count = 0;
	switch (message)
	{
	case WM_CHAR: 
	{
		TCHAR str[10];
		int size = SendMessage(hWnd, EM_GETLINE, 0, (LPARAM)str);
		
		if (LOWORD(wParam) == '-' && size != 0)
			return 0;
		else if (LOWORD(wParam) == '.' && wcschr(str, '.') != 0)
			return 0;
		else if (LOWORD(wParam) != VK_BACK && LOWORD(wParam) != '-' && LOWORD(wParam) != '.' && (LOWORD(wParam) < '0' || LOWORD(wParam) > '9'))
			return 0;
	}
		break;
	default: 
		break;
	}
	return CallWindowProc(OldWndProc, hWnd, message,
		wParam, lParam);
}
