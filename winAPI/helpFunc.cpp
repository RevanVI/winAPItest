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


matrix::matrix(int m = 2, int n = 2)
{
	coef = new int*[m];
	for (int i = 0; i < m; ++i)
		coef[i] = new int[n];
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

matrix operator*(matrix, matrix);
matrix operator*(int, matrix);
