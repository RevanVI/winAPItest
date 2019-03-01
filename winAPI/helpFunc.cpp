#include "helpFunc.h"
#include <cmath>



bool drawLine(HDC hdc, int x0, int y0, int x, int y)
{
	POINT pt;
	MoveToEx(hdc, x0, y0, &pt);
	return LineTo(hdc, x, y);
}

bool drawPol2Dim(HDC hdc, matrix fig, COLORREF color, coordDescr d)
{
	int m, n;
	fig.getDimens(m, n);
	for (int i = 0; i < m - 1; ++i)
		drawLine(hdc, fig(i, 0) * d.cx + d.x0, -fig(i, 1) * d.cy + d.y0, fig(i + 1, 0) * d.cx + d.x0, -fig(i + 1, 1) * d.cy + d.y0);
	drawLine(hdc, fig(m - 1, 0) * d.cx + d.x0, -fig(m - 1, 1) * d.cy + d.y0, fig(0, 0) * d.cx + d.x0, -fig(0, 1) * d.cy + d.y0);
	return true;
}

bool drawBrickDim(HDC hdc, int x, int y, int w, int h, int d, COLORREF color)
{
	int ** arr = new int*[7];
	for (int i = 0; i < 7; ++i)
		arr[i] = new int[4];
	return true;
}

void create2DGrid(HDC hdc, coordDescr d)
{
	drawLine(hdc, d.l, d.t, d.r, d.t);
	drawLine(hdc, d.r, d.t, d.r, d.b);
	drawLine(hdc, d.r, d.b, d.l, d.b);
	drawLine(hdc, d.l, d.b, d.l, d.t);

	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN oPen = (HPEN)SelectObject(hdc, hPen);

	drawLine(hdc, d.l, d.y0, d.r, d.y0); //ось ќх
	drawLine(hdc, d.x0, d.t, d.x0, d.b); //ось ќу

	SelectObject(hdc, oPen);
	DeleteObject(hPen);
int num = (d.r - d.x0) / d.cx; //количество делений справа
for (int i = 0; i < num; ++i)
	drawLine(hdc, d.x0 + d.cx * i, d.y0 - 2, d.x0 + d.cx * i, d.y0 + 2);
num = (d.x0 - d.l) / d.cx; //количество делений слева
for (int i = 0; i < num; ++i)
	drawLine(hdc, d.x0 - d.cx * i, d.y0 - 2, d.x0 - d.cx * i, d.y0 + 2);
num = (d.y0 - d.t) / d.cy; //количество делений сверху
for (int i = 0; i < num; ++i)
	drawLine(hdc, d.x0 - 2, d.y0 - d.cy * i, d.x0 + 2, d.y0 - d.cy * i);
num = (d.b - d.y0) / d.cy; //количество делений снизу
for (int i = 0; i < num; ++i)
	drawLine(hdc, d.x0 - 2, d.y0 + d.cy * i, d.x0 + 2, d.y0 + d.cy * i);
}



double roundPrec(double val, int prec)
{
	return round(val * pow(10, prec)) / pow(10, prec);
}

matrix::matrix(int m, int n)
{
	coef = new double*[m];
	for (int i = 0; i < m; ++i)
		coef[i] = new double[n];
	this->m = m;
	this->n = n;
}

matrix::matrix(const matrix& matr)
{
	matr.getDimens(m, n);
	coef = new double*[m];
	for (int i = 0; i < m; ++i)
		coef[i] = new double[n];
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

double matrix::getElem(int i, int j) const
{
	return coef[i][j];
}

double& matrix::operator()(int i, int j)
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
				res(i, strPos) += roundPrec(left.getElem(i, j) * right.getElem(j, strPos), 2);
	}
	return res;
}

matrix operator*(double left, const matrix& right)
{
	int m, n;
	right.getDimens(m, n);
	matrix res(m, n);
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			res(i, j) = roundPrec(left * right.getElem(i, j), 2);
	return res;
}


double* matrix::getStr(int m)
{
	return coef[m];
}