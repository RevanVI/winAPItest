#include "helpFunc.h"

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
	return true;
}

void create2DGrid(HDC hdc, int l, int t, int r, int b, int cx, int cy, int x0, int y0)
{
	drawLine(hdc, l, t, r, t);
	drawLine(hdc, r, t, r, b);
	drawLine(hdc, r, b, l, b);
	drawLine(hdc, l, b, l, t);

	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN oPen = (HPEN)SelectObject(hdc, hPen);

	drawLine(hdc, l, y0, r, y0); //ось ќх
	drawLine(hdc, x0, t, x0, b); //ось ќу

	SelectObject(hdc, oPen);
	DeleteObject(hPen);
int num = (r - x0) / cx; //количество делений справа
for (int i = 0; i < num; ++i)
	drawLine(hdc, x0 + cx * i, y0 - 2, x0 + cx * i, y0 + 2);
num = (x0 - l) / cx; //количество делений слева
for (int i = 0; i < num; ++i)
	drawLine(hdc, x0 - cx * i, y0 - 2, x0 - cx * i, y0 + 2);
num = (y0 - t) / cy; //количество делений сверху
for (int i = 0; i < num; ++i)
	drawLine(hdc, x0 - 2, y0 - cy * i, x0 + 2, y0 - cy * i);
num = (b - y0) / cy; //количество делений снизу
for (int i = 0; i < num; ++i)
	drawLine(hdc, x0 - 2, y0 + cy * i, x0 + 2, y0 + cy * i);
}



matrix::matrix(int m, int n)
{
	coef = new int*[m];
	for (int i = 0; i < m; ++i)
		coef[i] = new int[n];
	this->m = m;
	this->n = n;
}

matrix::matrix(const matrix& matr)
{
	matr.getDimens(m, n);
	coef = new int*[m];
	for (int i = 0; i < m; ++i)
		coef[i] = new int[n];
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			coef[i][j] = matr.getElem(i, j);
}

matrix::~matrix()
{
	for (int i = 0; i < m; ++i)
		delete coef[i];
	delete coef;
}

void matrix::getDimens(int& m, int& n) const
{
	m = this->m;
	n = this->n;
}

int matrix::getElem(int i, int j) const
{
	return coef[i][j];
}

int& matrix::operator()(int i, int j)
{
	return coef[i][j];
}

matrix& matrix::operator=(const matrix & obj)
{
	if (this == &obj)
		return *this;
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			coef[i][j] = obj.getElem(i, j);
	return *this;
}

matrix operator*(const matrix& left, const matrix& right)
{
	int mL, nL, mR, nR;
	left.getDimens(mL, nL);
	right.getDimens(mR, nR);
	int m = mL,
		n = nR;
	matrix res(m, n);
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			res(i, j) = 0;
	int strPos = 0;
	for (int i = 0; i < m; ++i, strPos = 0)
	{
		for (int strPos = 0; strPos < n; ++strPos) //
			for (int j = 0; j < nL; ++j)
				res(i, strPos) += left.getElem(i, j) * right.getElem(j, strPos);
	}
	return res;
}

matrix operator*(int left, const matrix& right)
{
	int m, n;
	right.getDimens(m, n);
	matrix res(m, n);
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			res(i, j) = left * right.getElem(i, j);
	return res;
}


int* matrix::getStr(int m)
{
	return coef[m];
}