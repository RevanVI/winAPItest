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

#define ID_EDITXF 4000
#define ID_EDITYF 4001
#define ID_EDITZF 4002
#define ID_EDITH 4003
#define ID_EDITW 4004
#define ID_EDITD 4005

#define ID_EDITXA1 5000
#define ID_EDITYA1 5001
#define ID_EDITZA1 5002
#define ID_EDITXA2 5003
#define ID_EDITYA2 5004
#define ID_EDITZA2 5005

#define ID_EDITANG 6000
#define ID_BUT1 7000
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
		figLbl0, figLbl1, xfEdit, yfEdit, zfEdit, figLbl2, hEdit, wEdit, dEdit,
		axLbl0, axLbl1, xaEdit1, yaEdit1, zaEdit1, axLbl2, xaEdit2, yaEdit2, zaEdit2,
		angLbl, angEdit,
		but1;

	static bool mode;
	static bool viewChange;
	static bool sizeChange;
	static bool shiftFig;
	static double x0mod, y0mod;
	static double hf, wf, df;
	static double ohf, owf, odf;
	static double shiftXf, shiftYf, shiftZf;
	static coordDescr d;

	int minC = 10, maxC = 60, defC = 40;
	double minPos = 0.25, maxPos = 0.75, defPos = 0.5;

	static matrix fig(8, 4); 
	static matrix fig1(8, 4);

	static matrix axis(2, 4); //ось поворота

	static matrix dots(4 * 4, 2);
	static double angle; //угол поворота
    switch (message)
    {
	case WM_CREATE:
		{
		//инициализация данных
		x0mod = 0.5;
		y0mod = 0.5;
		hf = wf = df = 4.0;
		ohf = owf = odf = hf;
		mode = true;
		viewChange = false;
		sizeChange = false;
		shiftFig = false;
		d.cy = 80;
		d.cx = 80;

		int m, n;
		m = 4;
		n = 2;
		matrix test(m, n);
		test(0, 0) = 0;		test(0, 1) = 0;		//test(0, 2) = 1;
		test(1, 0) = 1;		test(1, 1) = 1;		//test(1, 2) = 1;
		test(2, 0) = 2;		test(2, 1) = -1;	//test(2, 2) = 1;
		test(3, 0) = 3;		test(3, 1) = 0;		//test(3, 2) = 1; 

		matrix p1(m, n);
		p1.fill(1);

		matrix t = chordAppr(test);
		t.round(3);

		matrix M(m, m);
		M.fill(0);
		for (int i = 1; i < m - 1; ++i)
		{
			M(i, i - 1) = t(0, i);
			M(i, i) = 2 * (t(0, i - 1) + t(0, i));
			M(i, i + 1) = t(0, i - 1);
		}
		M(0, 0) = 1;
		M(m - 1, m - 1) = 1;

		M.round(3);

		M = M.reverse();

		matrix R(m, n);
		for (int i = 1; i < m - 1; ++i)
			for (int j = 0; j < n; ++j)
				R(i, j) = (3 / t(0, i - 1) / t(0, i)) * (pow(t(0, i - 1), 2) * (test(i + 1, j) - test(i, j)) + pow(t(0, i), 2) * (test(i , j) - test(i - 1, j)));
		R(0, 0) = p1(0, 0);		R(0, 1) = p1(0, 1);
		R(m - 1, 0) = p1(m - 1, 0); R(m - 1, 1) = p1(m - 1, 1);

		p1 = M * R;
		p1.round(3);

		int tauC = 4;
		matrix tau(1, tauC);
		tau(0, 0) = 1.0 / 5;
		tau(0, 1) = 2.0 / 5;
		tau(0, 2) = 3.0 / 5;
		tau(0, 3) = 4.0 / 5;

		for (int i = 1; i < (m ); ++i)
		{
			for (int tauNum = 0; tauNum < tauC; ++tauNum)
			{
				double val;
				matrix buf(1, 4);
				buf(0, 0) = 2 * pow(tau(0, tauNum), 3) - 3 * pow(tau(0, tauNum), 2) + 1;
				buf(0, 1) = -2 * pow(tau(0, tauNum), 3) + 3 * pow(tau(0, tauNum), 2);
				buf(0, 2) = tau(0, tauNum) * (pow(tau(0, tauNum), 2) - 2 * tau(0, tauNum) + 1) * t(0, i - 1);
				buf(0, 3) = tau(0, tauNum) * (pow(tau(0, tauNum), 2) - tau(0, tauNum)) * t(0, i - 1);

				matrix G(4, n);
				G(0, 0) = test(i - 1, 0);			G(0, 1) = test(i - 1, 1);
				G(1, 0) = test(i, 0);				G(1, 1) = test(i, 1);
				G(2, 0) = p1(i - 1, 0);				G(2, 1) = p1(i -1, 1);
				G(3, 0) = p1(i , 0);				G(3, 1) = p1(i, 1);

				matrix ps = buf * G;
				for (int j = 0; j < n; ++j)
					dots((i - 1) * tauC + 1 + tauNum, j) = ps(0, j);
			}
		}
		for (int i = 0; i < n; ++i)
		{
			dots(0, i) = test(0, i);
			dots(m * tauC - 1, i) = test(m - 1, i);
		}

		double dArr[8][2];
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 2; ++j)
				dArr[i][j] = dots(i, j);
		}
		int val= 0;
		++val;
		/*
		fig(0, 0) = 0;					fig(0, 1) = 0;					fig(0, 2) = 0;				 fig(0, 3) = 1;
		fig(5, 0) = 4;					fig(5, 1) = 4;					fig(5, 2) = 4;				 fig(5, 3) = 1;

		fig(1, 0) = fig(5, 0);			fig(1, 1) = fig(0, 1);			fig(1, 2) = fig(0, 2);		 fig(1, 3) = 1;
		fig(2, 0) = fig(5, 0);			fig(2, 1) = fig(0, 1);			fig(2, 2) = fig(5, 2);		 fig(2, 3) = 1;
		fig(3, 0) = fig(0, 0);			fig(3, 1) = fig(0, 1);			fig(3, 2) = fig(5, 0);		 fig(3, 3) = 1;
		fig(4, 0) = fig(0, 0);			fig(4, 1) = fig(5, 1);			fig(4, 2) = fig(5, 2);		 fig(4, 3) = 1;
		fig(6, 0) = fig(5, 0);			fig(6, 1) = fig(5, 1);			fig(6, 2) = fig(0, 2);		 fig(6, 3) = 1;
		fig(7, 0) = fig(0, 0);			fig(7, 1) = fig(5, 1);		    fig(7, 2) = fig(0, 2);		 fig(7, 3) = 1;

		fig1 = fig;

		axis(0, 0) = 0; axis(0, 1) = 0; axis(0, 2) = 0; axis(0, 3) = 1;
		axis(1, 0) = 10; axis(1, 1) = 10; axis(1, 2) = 10; axis(1, 3) = 1;
		angle = 0 * PI / 180;

		double y = 0.05 * rcWnd.bottom;
		double x = 0.65 * rcWnd.right;
		titleLbl = CreateWindow(L"static", L"Настройки", WS_CHILD | WS_VISIBLE | SS_CENTER,
			x, y, 0.3 * rcWnd.right, 0.025 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		y += 0.025 * rcWnd.bottom + 0.025 * rcWnd.bottom;

		stepLbl = CreateWindow(L"static", L"Масштабы осей:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		cxEdit = CreateWindow(L"edit", L"20", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.1 * rcWnd.right, 0.1 * rcWnd.bottom, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITCX, hInst, NULL);
		cyEdit = CreateWindow(L"edit", L"20", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.125 * rcWnd.right, 0.1 * rcWnd.bottom, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITCY, hInst, NULL);
		czEdit = CreateWindow(L"edit", L"20", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			x + 0.15 * rcWnd.right, 0.1 * rcWnd.bottom, 0.025 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITCZ, hInst, NULL);

		y += 0.04 * rcWnd.bottom + 0.01 * rcWnd.bottom;
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


		y += 0.04 * rcWnd.bottom + 0.01 * rcWnd.bottom;
		modeLbl = CreateWindow(L"static", L"Режим:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		mBut0 = CreateWindow(L"button", NULL, WS_CHILD | WS_BORDER | BS_GROUPBOX,
			x + 0.1 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_MBUT0, hInst, NULL);
		mBut1 = CreateWindow(L"button", L"Изометр.", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTORADIOBUTTON,
			x + 0.1 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_MBUT1, hInst, NULL);
		mBut2 = CreateWindow(L"button", L"Диметр.", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTORADIOBUTTON,
			x + 0.2 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_MBUT2, hInst, NULL);
		SendMessage(mBut1, BM_SETCHECK, 1, 0L);


		y += 0.04 * rcWnd.bottom + 0.04 * rcWnd.bottom;
		figLbl0 = CreateWindow(L"static", L"Фигура", WS_CHILD | WS_VISIBLE | SS_CENTER,
			x, y, 0.3 * rcWnd.right, 0.025 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		y += 0.04 * rcWnd.bottom;
		figLbl1 = CreateWindow(L"static", L"Координаты: ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		xfEdit = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.1 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITXF, hInst, NULL);
		yfEdit = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.13 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITYF, hInst, NULL);
		zfEdit = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.16 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITZF, hInst, NULL);
		SendMessage(xfEdit, EM_LIMITTEXT, 5, 0);
		SendMessage(yfEdit, EM_LIMITTEXT, 5, 0);
		SendMessage(zfEdit, EM_LIMITTEXT, 5, 0);
		SetWindowLong(xfEdit, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(yfEdit, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(zfEdit, GWL_WNDPROC, (LONG)EditWndProc);

		y += 0.04 * rcWnd.bottom;
		figLbl2 = CreateWindow(L"static", L"Параметры: ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		wEdit = CreateWindow(L"edit", L"4.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.1 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITW, hInst, NULL);
		hEdit = CreateWindow(L"edit", L"4.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.13 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITH, hInst, NULL);
		dEdit = CreateWindow(L"edit", L"4.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.16 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITD, hInst, NULL);
		SendMessage(hEdit, EM_LIMITTEXT, 5, 0);
		SendMessage(wEdit, EM_LIMITTEXT, 5, 0);
		SendMessage(dEdit, EM_LIMITTEXT, 5, 0);
		SetWindowLong(hEdit, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(wEdit, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(dEdit, GWL_WNDPROC, (LONG)EditWndProc);

		y += 0.04 * rcWnd.bottom + 0.04 * rcWnd.bottom;
		axLbl0 = CreateWindow(L"static", L"Ось вращения", WS_CHILD | WS_VISIBLE | SS_CENTER,
			x, y, 0.3 * rcWnd.right, 0.025 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		y += 0.04 * rcWnd.bottom;
		axLbl1 = CreateWindow(L"static", L"Первая точка: ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		xaEdit1 = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.1 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITXA1, hInst, NULL);
		yaEdit1 = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.13 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITYA1, hInst, NULL);
		zaEdit1 = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.16 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITZA1, hInst, NULL);
		SendMessage(xaEdit1, EM_LIMITTEXT, 5, 0);
		SendMessage(yaEdit1, EM_LIMITTEXT, 5, 0);
		SendMessage(zaEdit1, EM_LIMITTEXT, 5, 0);
		SetWindowLong(xaEdit1, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(yaEdit1, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(zaEdit1, GWL_WNDPROC, (LONG)EditWndProc);

		y += 0.04 * rcWnd.bottom;
		axLbl2 = CreateWindow(L"static", L"Вторая точка: ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		xaEdit2 = CreateWindow(L"edit", L"10.0", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.1 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITXA2, hInst, NULL);
		yaEdit2 = CreateWindow(L"edit", L"10.0", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.13 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITYA2, hInst, NULL);
		zaEdit2 = CreateWindow(L"edit", L"10.0", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.16 * rcWnd.right, y, 0.03 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITZA2, hInst, NULL);
		SendMessage(xaEdit2, EM_LIMITTEXT, 5, 0);
		SendMessage(yaEdit2, EM_LIMITTEXT, 5, 0);
		SendMessage(zaEdit2, EM_LIMITTEXT, 5, 0);
		SetWindowLong(xaEdit2, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(yaEdit2, GWL_WNDPROC, (LONG)EditWndProc);
		SetWindowLong(zaEdit2, GWL_WNDPROC, (LONG)EditWndProc);

		y += 0.04 * rcWnd.bottom;
		angLbl = CreateWindow(L"static", L"Угол поворота: ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			x, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, NULL, hInst, NULL);
		angEdit = CreateWindow(L"edit", L"0.00", WS_CHILD | WS_VISIBLE | WS_BORDER,
			x + 0.1 * rcWnd.right, y, 0.035 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_EDITANG, hInst, NULL);
		SendMessage(angEdit, EM_LIMITTEXT, 6, 0);
		SetWindowLong(angEdit, GWL_WNDPROC, (LONG)EditWndProc);

		y += 4 * 0.04 * rcWnd.bottom;
		but1 = CreateWindow(L"button", L"Выполнить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			x + 0.1 * rcWnd.right, y, 0.1 * rcWnd.right, 0.04 * rcWnd.bottom, hWnd, (HMENU)ID_BUT1, hInst, NULL);
		*/
		}
		break;
    case WM_COMMAND:
		{
		/*
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
				case ID_EDITXF:
				case ID_EDITYF:
				case ID_EDITZF:
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
						double coef;
						int num, numm;
						if (LOWORD(wParam) == ID_EDITXF)
							edit = &xfEdit;
						if (LOWORD(wParam) == ID_EDITYF)
							edit = &yfEdit;
						if (LOWORD(wParam) == ID_EDITZF)
							edit = &zfEdit;
						if (LOWORD(wParam) == ID_EDITXF || LOWORD(wParam) == ID_EDITYF || LOWORD(wParam) == ID_EDITZF)
							shiftFig = true;

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
							val = round(1 * pow(10, 3)) / pow(10, 3);
							wstring str = to_wstring(val).substr(0, 4);
							SetWindowText(*edit, str.c_str());
						}
					}
				}
				break;
				case ID_EDITH:
				case ID_EDITW:
				case ID_EDITD:
				{
					HWND* edit;
					HWND* check;
					if (HIWORD(wParam) == EN_KILLFOCUS)
					{
						
						if (LOWORD(wParam) == ID_EDITH)
							edit = &hEdit;
						if (LOWORD(wParam) == ID_EDITW)
							edit = &wEdit;
						if (LOWORD(wParam) == ID_EDITD)
							edit = &dEdit;
						size = SendMessage(*edit, EM_GETLINE, 0, (LPARAM)buf);
						double val = wcstod(buf, NULL);
						if (size == 0 || val <= 0.01)
						{
							val = round(4 * pow(10, 3)) / pow(10, 3);
							wstring str = to_wstring(val).substr(0, 4);
							SetWindowText(*edit, str.c_str());
						}
						sizeChange = true;
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

					SendMessage(xfEdit, EM_GETLINE, 0, (LPARAM)buf);
					shiftXf = wcstod(buf, NULL) - fig1(0, 0);
					SendMessage(yfEdit, EM_GETLINE, 0, (LPARAM)buf);
					shiftYf = wcstod(buf, NULL) - fig1(0, 1);
					SendMessage(zfEdit, EM_GETLINE, 0, (LPARAM)buf);
					shiftZf = wcstod(buf, NULL) - fig1(0, 2);

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

					SendMessage(hEdit, EM_GETLINE, 0, (LPARAM)buf);
					ohf = hf;
					hf = wcstod(buf, NULL);
					SendMessage(wEdit, EM_GETLINE, 0, (LPARAM)buf);
					owf = wf;
					wf = wcstod(buf, NULL);
					SendMessage(dEdit, EM_GETLINE, 0, (LPARAM)buf);
					odf = df;
					df = wcstod(buf, NULL);

					SendMessage(angEdit, EM_GETLINE, 0, (LPARAM)buf);
					angle = wcstod(buf, NULL) * PI / 180;

					RECT r;
					r.bottom = d.b;
					r.top = d.t;
					r.right = d.r;
					r.left = d.l;
					RedrawWindow(hWnd, &r, NULL, RDW_ERASE | RDW_INVALIDATE);
				}
				break;
			}
			*/
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
		drawPol2Dim(hdc, dots, RGB(0, 0, 0), d);
		/*
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			d.l = xClient / 20;
			d.r = 0.6 * xClient;
			d.t = yClient / 20;
			d.b = yClient - d.t;
			d.x0 = (d.r - d.l) * x0mod + d.l;
			d.y0 = (d.b - d.t) * y0mod + d.t;

			double dArr[8][4];
			for (int i = 0; i < 8; ++i)
			{
				for (int j = 0; j < 4; ++j)
					dArr[i][j] = fig1(i, j);
			}

			create3DGrid(hdc, d, mode);

			if (shiftFig)
			{
				for (int i = 0; i < 8; ++i)
				{
					fig1(i, 0) = fig1(i, 0) + shiftXf;
					fig1(i, 1) = fig1(i, 1) + shiftYf;
					fig1(i, 2) = fig1(i, 2) + shiftZf;
				}
				shiftXf = shiftYf = shiftZf = 0.0;
				shiftFig = false;
			}

			if (!viewChange)
			{
				if (sizeChange)
				{
					matrix sChange(4, 4);
					sChange(0, 0) = wf / owf;   sChange(0, 1) = 0;			sChange(0, 2) = 0;			sChange(0, 3) = 0;
					sChange(1, 0) = 0;		    sChange(1, 1) = hf / ohf;   sChange(1, 2) = 0;			sChange(1, 3) = 0;
					sChange(2, 0) = 0;		    sChange(2, 1) = 0;			sChange(2, 2) = df / odf;	sChange(2, 3) = 0;
					sChange(3, 0) = 0;			sChange(3, 1) = 0;			sChange(3, 2) = 0;			sChange(3, 3) = 1;

					fig1 = fig1 * sChange;
					sizeChange = false;
				}
				fig1 = rotateFig(fig1, axis, angle);
			}

			for (int i = 0; i < 8; ++i)
			{
				for (int j = 0; j < 4; ++j)
					dArr[i][j] = fig1(i, j);
			}

			matrix axis1;
			matrix figDraw(8, 4);
			if (mode)
			{
				matrix iso(4, 4);
				iso(0, 0) = 0.707;  iso(0, 1) = -0.408;  iso(0, 2) = 0; iso(0, 3) = 0;
				iso(1, 0) = 0;      iso(1, 1) = 0.816;   iso(1, 2) = 0; iso(1, 3) = 0;
				iso(2, 0) = -0.707; iso(2, 1) = -0.408;  iso(2, 2) = 0; iso(2, 3) = 0;
				iso(3, 0) = 0;      iso(3, 1) = 0;       iso(3, 2) = 0; iso(3, 3) = 1;

				figDraw = fig1 * iso;
				axis1 = axis * iso;
			}
			else
			{
				matrix dim(4, 4);
				dim(0, 0) = 0.925;  dim(0, 1) = -0.134;  dim(0, 2) = 0; dim(0, 3) = 0;
				dim(1, 0) = 0;      dim(1, 1) = 0.935;   dim(1, 2) = 0; dim(1, 3) = 0;
				dim(2, 0) = -0.378; dim(2, 1) = -0.327;  dim(2, 2) = 0; dim(2, 3) = 0;
				dim(3, 0) = 0;      dim(3, 1) = 0;       dim(3, 2) = 0; dim(3, 3) = 1;

				figDraw = fig1 * dim;
				axis1 = axis * dim;
			}

			for (int i = 0; i < 8; ++i)
			{
				for (int j = 0; j < 4; ++j)
					dArr[i][j] = figDraw(i, j);
			}
			drawLine(hdc, axis1(0, 0), axis1(0, 1), axis1(1, 0), axis1(1, 1), d);
			drawBrickDim(hdc, figDraw, d, RGB(0, 0, 0));
			EndPaint(hWnd, &ps);
			viewChange = false;
			//занесение изменненных координат в интерфейс
			int prec = 2;
			int len = 5;
			double val = fig1(0, 0);
			val = round(val * pow(10, prec)) / pow(10, prec);
			wstring str = to_wstring(val).substr(0, len);
			SetWindowText(xfEdit, str.c_str());

			val = fig1(0, 1);
			val = round(val * pow(10, prec)) / pow(10, prec);
			str = to_wstring(val).substr(0, len);
			SetWindowText(yfEdit, str.c_str());

			val = fig1(0, 2);
			val = round(val * pow(10, prec)) / pow(10, prec);
			str = to_wstring(val).substr(0, len);
			SetWindowText(zfEdit, str.c_str());
			*/
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
