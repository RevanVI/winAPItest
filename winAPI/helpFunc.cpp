#include "helpFunc.h"
#include <cmath>


bool drawLine(HDC hdc, double x0, double y0, double x, double y, coordDescr d)
{
	POINT pt;
	MoveToEx(hdc, x0 * d.cx + d.x0, -y0 * d.cy + d.y0, &pt);
	return LineTo(hdc, x * d.cx + d.x0, -y * d.cy + d.y0);
}

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

bool drawBrickDim(HDC hdc, matrix fig, coordDescr d, COLORREF color)
{
	drawLine(hdc, fig(0, 0), fig(0, 1), fig(1, 0), fig(1, 1), d); //01
	drawLine(hdc, fig(0, 0), fig(0, 1), fig(3, 0), fig(3, 1), d); //03
	drawLine(hdc, fig(0, 0), fig(0, 1), fig(7, 0), fig(7, 1), d); //07
	drawLine(hdc, fig(1, 0), fig(1, 1), fig(2, 0), fig(2, 1), d); //12
	drawLine(hdc, fig(1, 0), fig(1, 1), fig(6, 0), fig(6, 1), d); //16
	drawLine(hdc, fig(2, 0), fig(2, 1), fig(3, 0), fig(3, 1), d); //23
	drawLine(hdc, fig(2, 0), fig(2, 1), fig(5, 0), fig(5, 1), d); //25
	drawLine(hdc, fig(3, 0), fig(3, 1), fig(4, 0), fig(4, 1), d); //34
	drawLine(hdc, fig(4, 0), fig(4, 1), fig(5, 0), fig(5, 1), d); //45 
	drawLine(hdc, fig(4, 0), fig(4, 1), fig(7, 0), fig(7, 1), d); //47
	drawLine(hdc, fig(5, 0), fig(5, 1), fig(6, 0), fig(6, 1), d); //56
	drawLine(hdc, fig(6, 0), fig(6, 1), fig(7, 0), fig(7, 1), d); //67
	return true;
}

matrix rotateFig(matrix fig, matrix axis, double angle)
{
	if (angle > 0.01 || angle < -0.01)
	{
		double norm = sqrt(pow(axis(1, 0) - axis(0, 0), 2) + pow(axis(1, 1) - axis(0, 1), 2) + pow(axis(1, 2) - axis(0, 2), 2));
		matrix vec(1, 4);
		vec(0, 0) = (axis(1, 0) - axis(0, 0)) / norm;
		vec(0, 1) = (axis(1, 1) - axis(0, 1)) / norm;
		vec(0, 2) = (axis(1, 2) - axis(0, 2)) / norm;
		double k = sqrt(pow(vec(0, 0), 2) + pow(vec(0, 1), 2)); // k = sqrt(cx^2 + cy^2)

		matrix tM1 = matrix(4, 4); //матрица перемещения
		tM1(0, 0) = 1;				tM1(0, 1) = 0;				tM1(0, 2) = 0;				tM1(0, 3) = 0;
		tM1(1, 0) = 0;				tM1(1, 1) = 1;				tM1(1, 2) = 0;				tM1(1, 3) = 0;
		tM1(2, 0) = 0;				tM1(2, 1) = 0;				tM1(2, 2) = 1;				tM1(2, 3) = 0;
		tM1(3, 0) = -axis(0, 0);	tM1(3, 1) = -axis(0, 1);	tM1(3, 2) = -axis(0, 2);	tM1(3, 3) = 1;

		matrix tM2 = matrix(4, 4); //матрица поворота отн-но Oz
		tM2(0, 0) = vec(0, 1) / k;		tM2(0, 1) = vec(0, 0) / k;		tM2(0, 2) = 0;		tM2(0, 3) = 0;
		tM2(1, 0) = -vec(0, 0) / k;		tM2(1, 1) = vec(0, 1) / k;		tM2(1, 2) = 0;		tM2(1, 3) = 0;
		tM2(2, 0) = 0;					tM2(2, 1) = 0;					tM2(2, 2) = 1;		tM2(2, 3) = 0;
		tM2(3, 0) = 0;					tM2(3, 1) = 0;					tM2(3, 2) = 0;		tM2(3, 3) = 1;

		matrix tM3 = matrix(4, 4); //матрица поворота отн-но Ox
		tM3(0, 0) = 1;			tM3(0, 1) = 0;				tM3(0, 2) = 0;				tM3(0, 3) = 0;
		tM3(1, 0) = 0;			tM3(1, 1) = k;				tM3(1, 2) = -vec(0, 2);		tM3(1, 3) = 0;
		tM3(2, 0) = 0;			tM3(2, 1) = vec(0, 2);		tM3(2, 2) = k;				tM3(2, 3) = 0;
		tM3(3, 0) = 0;			tM3(3, 1) = 0;				tM3(3, 2) = 0;				tM3(3, 3) = 1;

		matrix tM4 = matrix(4, 4); //матрица поворота отн-но Oy (содерж)
		tM4(0, 0) = cos(angle);		tM4(0, 1) = 0;			tM4(0, 2) = -sin(angle);	tM4(0, 3) = 0;
		tM4(1, 0) = 0;				tM4(1, 1) = 1;			tM4(1, 2) = 0;				tM4(1, 3) = 0;
		tM4(2, 0) = sin(angle);		tM4(2, 1) = 0;			tM4(2, 2) = cos(angle);		tM4(2, 3) = 0;
		tM4(3, 0) = 0;				tM4(3, 1) = 0;			tM4(3, 2) = 0;				tM4(3, 3) = 1;

		matrix tM5 = matrix(4, 4); //матрица поворота отн-но Ox (возврат)
		tM5(0, 0) = 1;			tM5(0, 1) = 0;				tM5(0, 2) = 0;				tM5(0, 3) = 0;
		tM5(1, 0) = 0;			tM5(1, 1) = k;				tM5(1, 2) = vec(0, 2);		tM5(1, 3) = 0;
		tM5(2, 0) = 0;			tM5(2, 1) = -vec(0, 2);		tM5(2, 2) = k;				tM5(2, 3) = 0;
		tM5(3, 0) = 0;			tM5(3, 1) = 0;				tM5(3, 2) = 0;				tM5(3, 3) = 1;

		matrix tM6 = matrix(4, 4); //матрица поворота отн-но Oz (возврат)
		tM6(0, 0) = vec(0, 1) / k;		tM6(0, 1) = -vec(0, 0) / k;		tM6(0, 2) = 0; tM6(0, 3) = 0;
		tM6(1, 0) = vec(0, 0) / k;		tM6(1, 1) = vec(0, 1) / k;		tM6(1, 2) = 0; tM6(1, 3) = 0;
		tM6(2, 0) = 0;					tM6(2, 1) = 0;					tM6(2, 2) = 1; tM6(2, 3) = 0;
		tM6(3, 0) = 0;					tM6(3, 1) = 0;					tM6(3, 2) = 0; tM6(3, 3) = 1;

		matrix tM7 = matrix(4, 4); //матрица перемещения (возврат)
		tM7(0, 0) = 1;				tM7(0, 1) = 0;				tM7(0, 2) = 0;				tM7(0, 3) = 0;
		tM7(1, 0) = 0;				tM7(1, 1) = 1;				tM7(1, 2) = 0;				tM7(1, 3) = 0;
		tM7(2, 0) = 0;				tM7(2, 1) = 0;				tM7(2, 2) = 1;				tM7(2, 3) = 0;
		tM7(3, 0) = axis(0, 0);		tM7(3, 1) = axis(0, 1);		tM7(3, 2) = axis(0, 2);		tM7(3, 3) = 1;

		return fig * tM1 * tM2 * tM3 * tM4 * tM5 * tM6;
	}
	else
		return fig;
}

void create2DGrid(HDC hdc, coordDescr d)
{
	drawLine(hdc, d.l, d.t, d.r, d.t);
	drawLine(hdc, d.r, d.t, d.r, d.b);
	drawLine(hdc, d.r, d.b, d.l, d.b);
	drawLine(hdc, d.l, d.b, d.l, d.t);

	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN oPen = (HPEN)SelectObject(hdc, hPen);
	drawLine(hdc, d.l, d.y0, d.r, d.y0); //ось Ох
	drawLine(hdc, d.x0, d.t, d.x0, d.b); //ось Оу

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

void create3DGrid(HDC hdc, coordDescr d, bool mode)
{
	drawLine(hdc, d.l, d.t, d.r, d.t);
	drawLine(hdc, d.r, d.t, d.r, d.b);
	drawLine(hdc, d.r, d.b, d.l, d.b);
	drawLine(hdc, d.l, d.b, d.l, d.t);

	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN oPen = (HPEN)SelectObject(hdc, hPen);

	matrix dim(4, 4);

	if (mode == false) //dim
	{
		dim(0, 0) = 0.925;  dim(0, 1) = -0.134;  dim(0, 2) = 0; dim(0, 3) = 0;
		dim(1, 0) = 0;      dim(1, 1) = 0.935;   dim(1, 2) = 0; dim(1, 3) = 0;
		dim(2, 0) = -0.378; dim(2, 1) = -0.327;  dim(2, 2) = 0; dim(2, 3) = 0;
		dim(3, 0) = 0;      dim(3, 1) = 0;       dim(3, 2) = 0; dim(3, 3) = 1;
	}
	else //iso
	{
		dim(0, 0) = 0.707;  dim(0, 1) = -0.408;  dim(0, 2) = 0; dim(0, 3) = 0;
		dim(1, 0) = 0;      dim(1, 1) = 0.816;   dim(1, 2) = 0; dim(1, 3) = 0;
		dim(2, 0) = -0.707; dim(2, 1) = -0.408;  dim(2, 2) = 0; dim(2, 3) = 0;
		dim(3, 0) = 0;      dim(3, 1) = 0;       dim(3, 2) = 0; dim(3, 3) = 1;
	}

	int num = (d.r - d.x0) / d.cx;
	matrix oX(2, 4);
	oX(0, 0) = 0; oX(0, 1) = 0; oX(0, 2) = 0; oX(0, 3) = 1;
	oX(1, 0) = num; oX(1, 1) = 0; oX(1, 2) = 0; oX(1, 3) = 1;
	oX = oX * dim;

	matrix oZ(2, 4);
	oZ(0, 0) = 0; oZ(0, 1) = 0; oZ(0, 2) = 0; oZ(0, 3) = 1;
	oZ(1, 0) = 0; oZ(1, 1) = 0; oZ(1, 2) = num; oZ(1, 3) = 1;
	oZ = oZ * dim;

	num = (d.y0 - d.t) / d.cy;
	drawLine(hdc, d.x0, d.y0, d.x0, d.y0 - d.cy * num); //ось Оy
	drawLine(hdc, oX(0, 0) * d.cx + d.x0, -oX(0, 1) * d.cy + d.y0, oX(1, 0) * d.cx + d.x0, -oX(1, 1) * d.cy + d.y0); //ось Оx
	drawLine(hdc, oZ(0, 0) * d.cx + d.x0, -oZ(0, 1) * d.cy + d.y0, oZ(1, 0) * d.cx + d.x0, -oZ(1, 1) * d.cy + d.y0); //ось Оz

	SelectObject(hdc, oPen);
	DeleteObject(hPen);

	//ось Ох
	num = (d.r - d.x0) / d.cx; 
	matrix buf(2, 4);
	matrix step(1, 4);
	for (int j = 0; j < 3; ++j)
	{
		if (j == 0) //Ox
		{
			num = (d.r - d.x0) / d.cx;
			buf(0, 0) = 1; buf(0, 1) = -0.3; buf(0, 2) = 0; buf(0, 3) = 1;
			buf(1, 0) = 1; buf(1, 1) = 0.3;  buf(1, 2) = 0; buf(0, 3) = 1;
			step(0, 0) = 1; step(0, 1) = 0; step(0, 2) = 0; step(0, 3) = 1;
		}
		else if (j == 1) //Oz
		{
			buf(0, 0) = 0; buf(0, 1) = -0.3; buf(0, 2) = 1; buf(0, 3) = 1;
			buf(1, 0) = 0; buf(1, 1) = 0.3;  buf(1, 2) = 1; buf(0, 3) = 1;
			step(0, 0) = 0; step(0, 1) = 0; step(0, 2) = 1; step(0, 3) = 1;
		}
		else //Oy
		{
			buf(0, 0) = -0.3; buf(0, 1) = 1; buf(0, 2) = 0; buf(0, 3) = 1;
			buf(1, 0) = 0.3;  buf(1, 1) = 1;  buf(1, 2) = 0; buf(0, 3) = 1;
			step(0, 0) = 0; step(0, 1) = 1; step(0, 2) = 0; step(0, 3) = 1;
		}

		buf = buf * dim;
		step = step * dim;
		for (int i = 1; i < num - 1; ++i)
		{
			drawLine(hdc, buf(0, 0), buf(0, 1), buf(1, 0), buf(1, 1), d);
			buf(0, 0) += step(0, 0);
			buf(1, 0) += step(0, 0);
			buf(0, 1) += step(0, 1);
			buf(1, 1) += step(0, 1);
		}
	}
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