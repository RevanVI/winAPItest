// winAPItest.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "winAPItest.h"
#include <windowsx.h>
#include <cmath>
#include <string>
#include "helpFunc.h"

#define ID_EDITCX 1000
#define ID_EDITCY 1001
#define ID_EDITCZ 1002

#define ID_EDITX 2000
#define ID_EDITY 2001

#define ID_MBUT0 3000
#define ID_MBUT1 3001
#define ID_MBUT2 3002

#define ID_EDITXA1 4000
#define ID_EDITYA1 4001
#define ID_EDITZA1 4002
#define ID_EDITXA2 4003
#define ID_EDITYA2 4004
#define ID_EDITZA2 4005

#define ID_EDITANG 5000
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
		angLbl, angEdit,
		axLbl0, axLbl1, xaEdit1, yaEdit1, zaEdit1,
		axLbl2, xaEdit2, yaEdit2, zaEdit2,
		but1;
	static HWND pLbl0;
	static HWND pLbl[11];
	static HWND pxEdit[11];
	static HWND pyEdit[11];
	static HWND pzEdit[11];

	static bool mode;
	static bool viewChange;
	static bool pointChange;
	static double x0mod, y0mod;
	static coordDescr d;

	int minC = 10, maxC = 60, defC = 40;
	double minPos = 0.25, maxPos = 0.75, defPos = 0.5;

	static int m = 4;
	static int n = 16;
	static matrix p0(m, n); //изначальные точки
	static matrix** p;
	static int num = 0;
	static matrix J(4, 4);
	static matrix axis(2, 4); //ось поворота
	static double angle; //угол поворота
	static int pNum;
	static bool isLBDown, isRBDown;
	switch (message)
	{
	case WM_CREATE:
	{
		//инициализация данных
		x0mod = 0.5;
		y0mod = 0.5;
		mode = true;
		viewChange = false;
		pointChange = true;
		isLBDown = isRBDown = false;

		pNum = -1;

		d.cy = 40;
		d.cx = 40;
		d.cz = 40;


		p0(0, 0) = -3;		p0(0, 1) = 0;		p0(0, 2) = 3;		p0(0, 3) = 1;
		p0(1, 0) = -1;		p0(1, 1) = 1;		p0(1, 2) = 3;		p0(1, 3) = 1;
		p0(2, 0) = 1;		p0(2, 1) = 1;		p0(2, 2) = 3;		p0(2, 3) = 1;
		p0(3, 0) = 3;		p0(3, 1) = 0;		p0(3, 2) = 3;		p0(3, 3) = 1;

		p0(0, 4) = -3;		p0(0, 5) = 1;		p0(0, 6) = 1;		p0(0, 7) = 1;
		p0(1, 4) = -1;		p0(1, 5) = 1;		p0(1, 6) = 1;		p0(1, 7) = 1;
		p0(2, 4) = 1;		p0(2, 5) = 1;		p0(2, 6) = 1;		p0(2, 7) = 1;
		p0(3, 4) = 3;		p0(3, 5) = 1;		p0(3, 6) = 1;		p0(3, 7) = 1;

		p0(0, 8) = -3;		p0(0, 9) = 1;		p0(0, 10) = -1;		p0(0, 11) = 1;
		p0(1, 8) = -1;		p0(1, 9) = 1;		p0(1, 10) = -1;		p0(1, 11) = 1;
		p0(2, 8) = 1;		p0(2, 9) = 1;		p0(2, 10) = -1;		p0(2, 11) = 1;
		p0(3, 8) = 3;		p0(3, 9) = 1;		p0(3, 10) = -1;		p0(3, 11) = 1;

		p0(0, 12) = -3;		p0(0, 13) = 0;		p0(0, 14) = -3;		p0(0, 15) = 1;
		p0(1, 12) = -1;		p0(1, 13) = 1;		p0(1, 14) = -3;		p0(1, 15) = 1;
		p0(2, 12) = 1;		p0(2, 13) = 1;		p0(2, 14) = -3;		p0(2, 15) = 1;
		p0(3, 12) = 3;		p0(3, 13) = 0;		p0(3, 14) = -3;		p0(3, 15) = 1;

		p = new matrix*[4];
		for (int i = 0; i < 4; ++i)
			p[i] = new matrix[4];

		double dt = 1 / (m * 4.0);
		int jC = int(1 / dt);
		double t = 0;
		//J = new matrix(4, 4);
		//J->fill(0);
		J(0, 0) = -1;		J(0, 1) = 3;		J(0, 2) = -3;		J(0, 3) = 1;
		J(1, 0) = 3;		J(1, 1) = -6;		J(1, 2) = 3;		J(1, 3) = 0;
		J(2, 0) = -3;		J(2, 1) = 3;		J(2, 2) = 0;		J(2, 3) = 0;
		J(3, 0) = 1;		J(3, 1) = 0;		J(3, 2) = 0;		J(3, 3) = 0;
		/*
		for (int i = 0; i < 4; ++i)
		{
			(*J)(i, 0) = pow((1 - t), 3);
			(*J)(i, 1) = 3 * t * pow((1 - t), 2);
			(*J)(i, 2) = 3 * t * t* (1 - t);
			(*J)(i, 3) = t * t * t;
			t += dt;
			if (i + 1 == jC - 1)
				t = 1;
		}
		*/
		axis(0, 0) = 0; axis(0, 1) = 0; axis(0, 2) = 0; axis(0, 3) = 1;
		axis(1, 0) = 0; axis(1, 1) = 10; axis(1, 2) = 0; axis(1, 3) = 1;
		angle = 0 * PI / 180;

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
		czEdit = CreateWindow(L"edit", L"40", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.15 * rcWnd.right, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITCZ, hInst, NULL);

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

		y += 0.04 * rcWnd.bottom;
		modeLbl = CreateWindow(L"static", L"Режим:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		mBut0 = CreateWindow(L"button", NULL, WS_CHILD | WS_BORDER | BS_GROUPBOX,
			x + 0.1 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_MBUT0, hInst, NULL);
		mBut1 = CreateWindow(L"button", L"Изометр.", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTORADIOBUTTON,
			x + 0.1 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_MBUT1, hInst, NULL);
		mBut2 = CreateWindow(L"button", L"Диметр.", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTORADIOBUTTON,
			x + 0.2 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_MBUT2, hInst, NULL);
		SendMessage(mBut1, BM_SETCHECK, 1, 0L);

		y += 0.04 * rcWnd.bottom;
		angLbl = CreateWindow(L"static", L"Угол поворота: ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		angEdit = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.1 * rcWnd.right, y, 0.035 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITANG, hInst, NULL);
		SendMessage(angEdit, EM_LIMITTEXT, 6, 0);
		SetWindowLong(angEdit, GWL_WNDPROC, (LONG)EditWndProc);

		y += 0.01 * rcWnd.bottom + 0.04 * rcWnd.bottom;
		axLbl0 = CreateWindow(L"static", L"Ось вращения", WS_CHILD | WS_VISIBLE | SS_CENTER,
			x, y, 0.3 * rcWnd.right, 0.025 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		y += 0.04 * rcWnd.bottom;
		axLbl1 = CreateWindow(L"static", L"Т1: ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		xaEdit1 = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.025 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITXA1, hInst, NULL);
		yaEdit1 = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.055 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITYA1, hInst, NULL);
		zaEdit1 = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.085 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITZA1, hInst, NULL);
		SendMessage(xaEdit1, EM_LIMITTEXT, 5, 0);
		SendMessage(yaEdit1, EM_LIMITTEXT, 5, 0);
		SendMessage(zaEdit1, EM_LIMITTEXT, 5, 0);
		SetWindowLong(xaEdit1, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(yaEdit1, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(zaEdit1, GWL_WNDPROC, (LONG)EditWndProc);

		axLbl2 = CreateWindow(L"static", L"Т2: ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x + 0.12 * rcWnd.right, y, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		xaEdit2 = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.145 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITXA2, hInst, NULL);
		yaEdit2 = CreateWindow(L"edit", L"10.0", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.175 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITYA2, hInst, NULL);
		zaEdit2 = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.205 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITZA2, hInst, NULL);
		SendMessage(xaEdit2, EM_LIMITTEXT, 5, 0);
		SendMessage(yaEdit2, EM_LIMITTEXT, 5, 0);
		SendMessage(zaEdit2, EM_LIMITTEXT, 5, 0);
		SetWindowLong(xaEdit2, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(yaEdit2, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(zaEdit2, GWL_WNDPROC, (LONG)EditWndProc);

		y += 0.04 * rcWnd.bottom + 0.02 * rcWnd.bottom;
		pLbl0 = CreateWindow(L"static", L"Точки", WS_CHILD | WS_VISIBLE | SS_CENTER,
			x, y, 0.3 * rcWnd.right, 0.025 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		for (int i = 0; i < m; ++i)
		{
			y += 0.04 * rcWnd.bottom;
			wchar_t buffer[10];
			wsprintfW(buffer, L"T%d", i + 1);
			pLbl[i] = CreateWindow(L"static", buffer, WS_CHILD | WS_VISIBLE | SS_LEFT,
				x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
			swprintf(buffer, L"%3.2f", p0(i, 0));
			pxEdit[i] = CreateWindow(L"edit", buffer, WS_CHILD | WS_VISIBLE | WS_BORDER,
				x + 0.1 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)(6000 + i), hInst, NULL);
			swprintf(buffer, L"%3.2f", p0(i, 1));
			pyEdit[i] = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
				x + 0.13 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)(7000 + i), hInst, NULL);
			swprintf(buffer, L"%3.2f", p0(i, 2));
			pzEdit[i] = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
				x + 0.16 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)(8000 + i), hInst, NULL);
			SendMessage(pxEdit[i], EM_LIMITTEXT, 5, 0);
			SendMessage(pyEdit[i], EM_LIMITTEXT, 5, 0);
			SendMessage(pzEdit[i], EM_LIMITTEXT, 5, 0);
			SetWindowLong(pxEdit[i], GWL_WNDPROC, (LONG)EditWndProc);
			SetWindowLong(pyEdit[i], GWL_WNDPROC, (LONG)EditWndProc);
			SetWindowLong(pzEdit[i], GWL_WNDPROC, (LONG)EditWndProc);
		}

		y += 0.05 * rcWnd.bottom;
		but1 = CreateWindow(L"button", L"Выполнить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			x + 0.1 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_BUT1, hInst, NULL);
	}
	break;
	/*
	case WM_COMMAND:
	{
		TCHAR buf[10];
		int size;
		switch (LOWORD(wParam))
		{
		case ID_MBUT1:
		{
			viewChange = true;
			mode = true;
			RECT r;
			r.bottom = d.b;
			r.top = d.t;
			r.right = d.r;
			r.left = d.l;
			RedrawWindow(hWnd, &r, NULL, RDW_ERASE | RDW_INVALIDATE);
		}
		break;
		case ID_MBUT2:
		{
			viewChange = true;
			mode = false;
			RECT r;
			r.bottom = d.b;
			r.top = d.t;
			r.right = d.r;
			r.left = d.l;
			RedrawWindow(hWnd, &r, NULL, RDW_ERASE | RDW_INVALIDATE);
		}
		break;
		case ID_EDITCX:
		case ID_EDITCY:
		case ID_EDITCZ:
		{
			HWND* edit;
			if (HIWORD(wParam) == EN_KILLFOCUS)
			{
				if (LOWORD(wParam) == ID_EDITCX)
					edit = &cxEdit;
				if (LOWORD(wParam) == ID_EDITCY)
					edit = &cyEdit;
				if (LOWORD(wParam) == ID_EDITCZ)
					edit = &czEdit;
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
		break;
		case ID_EDITX:
		case ID_EDITY:
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
		break;
		case 6000:
		case 6001:
		case 6002:
		case 6003:
		case 6004:
		case 6005:
		case 6006:
		case 6007:
		case 6008:
		case 6009:
		case 6010:
		case 7000:
		case 7001:
		case 7002:
		case 7003:
		case 7004:
		case 7005:
		case 7006:
		case 7007:
		case 7008:
		case 7009:
		case 7010:
		case 8000:
		case 8001:
		case 8002:
		case 8003:
		case 8004:
		case 8005:
		case 8006:
		case 8007:
		case 8008:
		case 8009:
		case 8010:
		{
			HWND* edit;
			if (HIWORD(wParam) == EN_KILLFOCUS)
			{
				if (LOWORD(wParam) / 1000 == 6)
					edit = &pxEdit[LOWORD(wParam) % 1000];
				if (LOWORD(wParam) / 1000 == 7)
					edit = &pyEdit[LOWORD(wParam) % 1000];
				if (LOWORD(wParam) / 1000 == 8)
					edit = &pzEdit[LOWORD(wParam) % 1000];

				pointChange = true;
				size = SendMessage(*edit, EM_GETLINE, 0, (LPARAM)buf);
				double val = wcstod(buf, NULL);
				if (size == 0)
				{
					val = round(0 * pow(10, 3)) / pow(10, 3);
					wstring str = to_wstring(val).substr(0, 4);
					SetWindowText(*edit, str.c_str());
				}
			}
		}
		break;
		case ID_EDITXA1:
		case ID_EDITXA2:
		case ID_EDITYA1:
		case ID_EDITYA2:
		case ID_EDITZA1:
		case ID_EDITZA2:
		{
			HWND* edit;
			if (HIWORD(wParam) == EN_KILLFOCUS)
			{
				if (LOWORD(wParam) == ID_EDITXA1)
					edit = &xaEdit1;
				if (LOWORD(wParam) == ID_EDITXA2)
					edit = &xaEdit2;
				if (LOWORD(wParam) == ID_EDITYA1)
					edit = &yaEdit1;
				if (LOWORD(wParam) == ID_EDITYA2)
					edit = &yaEdit2;
				if (LOWORD(wParam) == ID_EDITZA1)
					edit = &zaEdit1;
				if (LOWORD(wParam) == ID_EDITZA2)
					edit = &zaEdit2;
				size = SendMessage(*edit, EM_GETLINE, 0, (LPARAM)buf);
				double val = wcstod(buf, NULL);
				if (size == 0)
				{
					val = round(0 * pow(10, 3)) / pow(10, 3);
					wstring str = to_wstring(val).substr(0, 4);
					SetWindowText(*edit, str.c_str());
				}
			}
		}
		break;
		case ID_BUT1:
		{
			SendMessage(cxEdit, EM_GETLINE, 0, (LPARAM)buf);
			d.cx = wcstol(buf, NULL, 10);
			SendMessage(cyEdit, EM_GETLINE, 0, (LPARAM)buf);
			d.cy = wcstol(buf, NULL, 10);
			SendMessage(czEdit, EM_GETLINE, 0, (LPARAM)buf);
			d.cz = wcstol(buf, NULL, 10);

			SendMessage(xEdit, EM_GETLINE, 0, (LPARAM)buf);
			x0mod = wcstod(buf, NULL);
			SendMessage(yEdit, EM_GETLINE, 0, (LPARAM)buf);
			y0mod = wcstod(buf, NULL);

			SendMessage(xaEdit1, EM_GETLINE, 0, (LPARAM)buf);
			axis(0, 0) = wcstod(buf, NULL);
			SendMessage(yaEdit1, EM_GETLINE, 0, (LPARAM)buf);
			axis(0, 1) = wcstod(buf, NULL);
			SendMessage(zaEdit1, EM_GETLINE, 0, (LPARAM)buf);
			axis(0, 2) = wcstod(buf, NULL);

			SendMessage(xaEdit2, EM_GETLINE, 0, (LPARAM)buf);
			axis(1, 0) = wcstod(buf, NULL);
			SendMessage(yaEdit2, EM_GETLINE, 0, (LPARAM)buf);
			axis(1, 1) = wcstod(buf, NULL);
			SendMessage(zaEdit2, EM_GETLINE, 0, (LPARAM)buf);
			axis(1, 2) = wcstod(buf, NULL);

			SendMessage(angEdit, EM_GETLINE, 0, (LPARAM)buf);
			angle = wcstod(buf, NULL) * PI / 180;

			if (pointChange)
			{
				for (int i = 0; i < m; ++i)
				{
					SendMessage(pxEdit[i], EM_GETLINE, 0, (LPARAM)buf);
					p0(i, 0) = wcstod(buf, NULL);
					SendMessage(pyEdit[i], EM_GETLINE, 0, (LPARAM)buf);
					p0(i, 1) = wcstod(buf, NULL);
					SendMessage(pzEdit[i], EM_GETLINE, 0, (LPARAM)buf);
					p0(i, 2) = wcstod(buf, NULL);
				}
				delete[] p;
				p = calculatePoints(p0, num);
				pointChange = false;
			}

			RECT r;
			r.bottom = d.b;
			r.top = d.t;
			r.right = d.r;
			r.left = d.l;
			RedrawWindow(hWnd, &r, NULL, RDW_ERASE | RDW_INVALIDATE);
		}
		break;
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		if ((x > d.l) && (x < d.r) && (y < d.b) && (y > d.t))
		{
			pNum = definePoint(p0, x, y, mode, d);
			isLBDown = true;
			pointChange = true;
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		if ((x > d.l) && (x < d.r) && (y < d.b) && (y > d.t))
		{
			pNum = definePoint(p0, x, y, mode, d);
			isRBDown = true;
			pointChange = true;
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		isLBDown = false;
		pNum = -1;
	}
	break;
	case WM_RBUTTONUP:
	{
		isRBDown = false;
		pNum = -1;
	}
	break;
	case WM_MOUSEMOVE:
	{
		if ((isLBDown || isRBDown) && (pNum >= 0))
		{
			int newX = LOWORD(lParam);
			int newY = HIWORD(lParam);

			HWND* edit;
			double* valEdit;
			double mp;
			if (isLBDown)
			{
				edit = &pxEdit[pNum];
				valEdit = &p0(pNum, 0);
				mp = 1;
			}
			else
			{
				edit = &pzEdit[pNum];
				valEdit = &p0(pNum, 2);
				mp = -1;
			}

			int x0 = d.x0, y0 = d.y0;
			double X = mp * (newX - x0) / cos(30.0 * PI / 180) / (d.cx * 0.707);
			double Y = (y0 - (newY - mp * (newX - x0) * tan(30.0 * PI / 180))) / (d.cy * 0.816);

			int prec = 3;
			int len = 5;
			double val = round(X * pow(10, prec)) / pow(10, prec);

			wstring str = to_wstring(val).substr(0, len);
			SetWindowText(*edit, str.c_str());
			*valEdit = val;

			val = round(Y * pow(10, prec)) / pow(10, prec);
			str = to_wstring(val).substr(0, len);
			SetWindowText(pyEdit[pNum], str.c_str());
			p0(pNum, 1) = val;

			delete[] p;
			p = calculatePoints(p0, num);

			RECT r;
			r.bottom = d.b;
			r.top = d.t;
			r.right = d.r;
			r.left = d.l;
			RedrawWindow(hWnd, &r, NULL, RDW_ERASE | RDW_INVALIDATE);
		}
	}
	break;
	*/
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

		matrix dim(4, 4);
		if (mode)
		{
			dim(0, 0) = 0.707;  dim(0, 1) = -0.408;  dim(0, 2) = 0; dim(0, 3) = 0;
			dim(1, 0) = 0;      dim(1, 1) = 0.816;   dim(1, 2) = 0; dim(1, 3) = 0;
			dim(2, 0) = -0.707; dim(2, 1) = -0.408;  dim(2, 2) = 0; dim(2, 3) = 0;
			dim(3, 0) = 0;      dim(3, 1) = 0;       dim(3, 2) = 0; dim(3, 3) = 1;
		}
		else
		{
			dim(0, 0) = 0.925;  dim(0, 1) = -0.134;  dim(0, 2) = 0; dim(0, 3) = 0;
			dim(1, 0) = 0;      dim(1, 1) = 0.935;   dim(1, 2) = 0; dim(1, 3) = 0;
			dim(2, 0) = -0.378; dim(2, 1) = -0.327;  dim(2, 2) = 0; dim(2, 3) = 0;
			dim(3, 0) = 0;      dim(3, 1) = 0;       dim(3, 2) = 0; dim(3, 3) = 1;
		}

		create3DGrid(hdc, d, mode);
			//for (int i = 0; i < num; ++i)
				//p[i] = rotateFig(p[i], axis, angle);
			double du = 1 / (m * 4.0);
			int uC = int(1 / du);
			double u = 0.0;

			double dw = 1 / (m * 4.0);
			int wC = int(1 / dw);
			double w = 0.0;
			p0 = rotateFig(p0, axis, angle);

			matrix oldDraw(wC, 4);
			matrix draw(wC, 4);

			matrix vU(1, 4);
			matrix vW(4, 1);
			for (int i = 0; i < uC; ++i)
			{
				//if (i != 0)
				//	oldDraw = draw;
				vU(0, 0) = pow(u, 3);
				vU(0, 1) = pow(u, 2);
				vU(0, 2) = pow(u, 1);
				vU(0, 3) = 1;
				for (int j = 0; j < wC; ++j)
				{
					vW(0, 1) = pow(w, 3);
					vW(1, 1) = pow(w, 2);
					vW(2, 1) = pow(w, 1);
					vW(3, 1) = 1;
					for (int coord = 0; coord < 3; ++coord)
					{
						matrix subMP(4, 4);
						for (int k = 0; k < 4; ++k)
							for (int l = 0; l < 4; ++l)
								subMP(k, l) = p0(k, l * 4 + coord);
						draw(j, coord) = (vU * J * subMP * J * vW)(0, 0);
					}
					draw(j, 3) = 1;
					w += dw;
					if (j + 1 == wC - 1)
						w = 1;
				}
				draw = draw * dim;
				drawPol3Dim(hdc, draw, RGB(0, 0, 0), d);
				u += du;
				if (i + 1 == uC - 1)
					u = 1;
			}
		
		/*
		for (int i = 0; i < num; ++i)
		{
			matrix draw = J * p[i];
			draw = draw * dim;
			drawPol3Dim(hdc, draw, RGB(0, 0, 0), d);
		}
		matrix draw = p0 * dim;
		drawPol3Dim(hdc, draw, RGB(120, 120, 120), d);
		*/
		EndPaint(hWnd, &ps);
		viewChange = false;

		//занесение изменненных координат в интерфейс
		int prec = 2;
		int len = 5;
		double val;
		for (int i = 0; i < m; ++i)
		{
			val = round(p0(i, 0) * pow(10, prec)) / pow(10, prec);
			wstring str = to_wstring(val).substr(0, len);
			SetWindowText(pxEdit[i], str.c_str());
			
			val = round(p0(i, 1) * pow(10, prec)) / pow(10, prec);
			str = to_wstring(val).substr(0, len);
			SetWindowText(pyEdit[i], str.c_str());
			
			val = round(p0(i, 2) * pow(10, prec)) / pow(10, prec);
			str = to_wstring(val).substr(0, len);
			SetWindowText(pzEdit[i], str.c_str());
		}
		}
		break;
	case WM_SIZE:
	{
		xClient = LOWORD(lParam);
		yClient = HIWORD(lParam);
	}
		break;
    case WM_DESTROY:
		//delete J;
		//delete[] p;
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
