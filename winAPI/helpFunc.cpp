#include "helpFunc.h"
#include <cmath>

double roundTo(double val, int prec)
{
	return round(val * pow(10, prec)) / pow(10, prec);
}

double roundPrec(double val, int prec)
{
	return round(val * pow(10, prec)) / pow(10, prec);
}

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

bool drawPol2Dim(HDC hdc, matrix fig, COLORREF color, coordDescr d, bool closed)
{
	int m, n;
	fig.getDimens(m, n);
	HPEN newPen = CreatePen(PS_SOLID, 1, color);
	HPEN oldPen = (HPEN)SelectObject(hdc, newPen);
	for (int i = 0; i < m - 1; ++i)
		drawLine(hdc, fig(i, 0), fig(i, 1), fig(i + 1, 0), fig(i + 1, 1), d);
	if (closed)
		drawLine(hdc, fig(m - 1, 0) * d.cx + d.x0, -fig(m - 1, 1) * d.cy + d.y0, fig(0, 0) * d.cx + d.x0, -fig(0, 1) * d.cy + d.y0);
	SelectObject(hdc, oldPen);
	DeleteObject(newPen);
	return true;
}

bool drawPol3Dim(HDC hdc, matrix fig, COLORREF color, coordDescr d)
{
	int m, n;
	fig.getDimens(m, n);
	HPEN newPen = CreatePen(PS_SOLID, 1, color);
	HPEN oldPen = (HPEN)SelectObject(hdc, newPen);
	for (int i = 0; i < m - 1; ++i)
		drawLine(hdc, fig(i, 0), fig(i, 1), fig(i + 1, 0), fig(i + 1, 1), d);
	SelectObject(hdc, oldPen);
	DeleteObject(newPen);
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

void createFrame(HDC hdc, coordDescr d)
{
	drawLine(hdc, d.l, d.t, d.r, d.t);
	drawLine(hdc, d.r, d.t, d.r, d.b);
	drawLine(hdc, d.r, d.b, d.l, d.b);
	drawLine(hdc, d.l, d.b, d.l, d.t);
}

void create2DGrid(HDC hdc, coordDescr d)
{
	createFrame(hdc, d);
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

matrix findCenter(matrix l, matrix r)
{
	int m, n;
	l.getDimens(m, n);
	matrix res(m, n);
	for (int i = 0; i < n - 1; ++i)
		res(0, i) = (l(0, i) + r(0, i)) / 2;
	res(0, n - 1) = 1;
	return res;
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

		matrix tM(4, 4);

		matrix tM1 = matrix(4, 4); //матрица перемещения
		tM1(0, 0) = 1;				tM1(0, 1) = 0;				tM1(0, 2) = 0;				tM1(0, 3) = 0;
		tM1(1, 0) = 0;				tM1(1, 1) = 1;				tM1(1, 2) = 0;				tM1(1, 3) = 0;
		tM1(2, 0) = 0;				tM1(2, 1) = 0;				tM1(2, 2) = 1;				tM1(2, 3) = 0;
		tM1(3, 0) = -axis(0, 0);	tM1(3, 1) = -axis(0, 1);	tM1(3, 2) = -axis(0, 2);	tM1(3, 3) = 1;

		double dArr[8][4];
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
				dArr[i][j] = tM1(i, j);
		}
		tM = tM1;

		//матрица поворота отн-но Oz
		if (vec(0, 0) > 0.001 || vec(0, 0) < -0.001) //если составляющая cx не равна нулю
		{
			tM1(0, 0) = vec(0, 1) / k;		tM1(0, 1) = vec(0, 0) / k;		tM1(0, 2) = 0;		tM1(0, 3) = 0;
			tM1(1, 0) = -vec(0, 0) / k;		tM1(1, 1) = vec(0, 1) / k;		tM1(1, 2) = 0;		tM1(1, 3) = 0;
			tM1(2, 0) = 0;					tM1(2, 1) = 0;					tM1(2, 2) = 1;		tM1(2, 3) = 0;
			tM1(3, 0) = 0;					tM1(3, 1) = 0;					tM1(3, 2) = 0;		tM1(3, 3) = 1;

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
					dArr[i][j] = tM1(i, j);
			}
			tM = tM * tM1;
		}

		//матрица поворота отн-но Ox
		if (vec(0, 2) > 0.001 || vec(0, 2) < -0.001) //если составляющая cz не равна нулю
		{
			tM1(0, 0) = 1;			tM1(0, 1) = 0;				tM1(0, 2) = 0;				tM1(0, 3) = 0;
			tM1(1, 0) = 0;			tM1(1, 1) = k;				tM1(1, 2) = -vec(0, 2);		tM1(1, 3) = 0;
			tM1(2, 0) = 0;			tM1(2, 1) = vec(0, 2);		tM1(2, 2) = k;				tM1(2, 3) = 0;
			tM1(3, 0) = 0;			tM1(3, 1) = 0;				tM1(3, 2) = 0;				tM1(3, 3) = 1;

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
					dArr[i][j] = tM1(i, j);
			}
			tM = tM * tM1;
		}

		//матрица поворота отн-но Oy (содерж)
		tM1(0, 0) = cos(angle);		tM1(0, 1) = 0;			tM1(0, 2) = -sin(angle);	tM1(0, 3) = 0;
		tM1(1, 0) = 0;				tM1(1, 1) = 1;			tM1(1, 2) = 0;				tM1(1, 3) = 0;
		tM1(2, 0) = sin(angle);		tM1(2, 1) = 0;			tM1(2, 2) = cos(angle);		tM1(2, 3) = 0;
		tM1(3, 0) = 0;				tM1(3, 1) = 0;			tM1(3, 2) = 0;				tM1(3, 3) = 1;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
				dArr[i][j] = tM1(i, j);
		}
		tM = tM * tM1;

		//матрица поворота отн-но Ox (возврат)
		if (vec(0, 2) > 0.001 || vec(0, 2) < -0.001)
		{
			tM1(0, 0) = 1;			tM1(0, 1) = 0;				tM1(0, 2) = 0;				tM1(0, 3) = 0;
			tM1(1, 0) = 0;			tM1(1, 1) = k;				tM1(1, 2) = vec(0, 2);		tM1(1, 3) = 0;
			tM1(2, 0) = 0;			tM1(2, 1) = -vec(0, 2);		tM1(2, 2) = k;				tM1(2, 3) = 0;
			tM1(3, 0) = 0;			tM1(3, 1) = 0;				tM1(3, 2) = 0;				tM1(3, 3) = 1;

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
					dArr[i][j] = tM1(i, j);
			}
			tM = tM * tM1;
		}

		//матрица поворота отн-но Oz (возврат)
		if (vec(0, 0) > 0.001 || vec(0, 0) < -0.001)
		{
			tM1(0, 0) = vec(0, 1) / k;		tM1(0, 1) = -vec(0, 0) / k;		tM1(0, 2) = 0; tM1(0, 3) = 0;
			tM1(1, 0) = vec(0, 0) / k;		tM1(1, 1) = vec(0, 1) / k;		tM1(1, 2) = 0; tM1(1, 3) = 0;
			tM1(2, 0) = 0;					tM1(2, 1) = 0;					tM1(2, 2) = 1; tM1(2, 3) = 0;
			tM1(3, 0) = 0;					tM1(3, 1) = 0;					tM1(3, 2) = 0; tM1(3, 3) = 1;

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
					dArr[i][j] = tM1(i, j);
			}
			tM = tM * tM1;
		}

		//матрица перемещения (возврат)
		tM1(0, 0) = 1;				tM1(0, 1) = 0;				tM1(0, 2) = 0;				tM1(0, 3) = 0;
		tM1(1, 0) = 0;				tM1(1, 1) = 1;				tM1(1, 2) = 0;				tM1(1, 3) = 0;
		tM1(2, 0) = 0;				tM1(2, 1) = 0;				tM1(2, 2) = 1;				tM1(2, 3) = 0;
		tM1(3, 0) = axis(0, 0);		tM1(3, 1) = axis(0, 1);		tM1(3, 2) = axis(0, 2);		tM1(3, 3) = 1;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
				dArr[i][j] = tM1(i, j);
		}
		tM = tM * tM1;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
				dArr[i][j] = tM(i, j);
		}
		return fig * tM;
	}
	else
		return fig;
}



//Lab2
matrix chordAppr(matrix points)
{
	int m, n;
	points.getDimens(m, n);

	matrix res(1, m - 1);
	res.fill(0);
	for (int i = 1; i < m; ++i)
	{
		for (int j = 0; j < n; ++j)//n - 1 тк однородные координаты
			res(0, i - 1) = res(0, i - 1) + pow(points(i, j) - points(i - 1, j), 2);
		res(0, i - 1) = sqrt(res(0, i - 1));
	}
	return res;
}
matrix* calculatePoints(matrix& p0, int& num)
{
	int m, n;
	p0.getDimens(m, n);
	if (m % 2 != 0)
		num = m + 1;
	else
		num = m;
	num = num / 2 - 1;
	matrix* p = new matrix[num];
	for (int i = 0; i < num; ++i)
		p[i] = matrix(4, n);

	matrix centerPoint(1, n);
	int k = 0;
	int pos = 0;
	while (k < m - 1)
	{
		if (k == 0)
			for (int j = 0; j < n; ++j)
				p[pos](0, j) = p0(0, j);
		else
			for (int j = 0; j < n; ++j)
				p[pos](0, j) = centerPoint(0, j);

		for (int i = 1; i < 3; ++i)
			for (int j = 0; j < n; ++j)
				p[pos](i, j) = p0(k + i, j);

		if ((m - 1) - (k + 2) >= 2) //останется не менее 2х точек
		{
			k += 2;
			centerPoint = findCenter(p0.getStr(k), p0.getStr(k + 1));
			for (int j = 0; j < n; ++j)
				p[pos](3, j) = centerPoint(0, j);
		}
		else //if ((k + 2 == m - 1) || ((m - 1) - (k + 2) == 1)) //k + 2 - последняя точка  либо k + 3 - последняя точка
		{
			k = m - 1;
			for (int j = 0; j < n; ++j)
				p[pos](3, j) = p0(k, j);
		}
		++pos;
	}
	return p;
}


//Matrix operations

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

matrix matrix::subMatrix(int str, int c) const
{
	if (str + c > m)
		return matrix(0, 0);
	matrix res(c, n);
	for (int i = str, strPos = 0; i < (str + c); ++i, ++strPos)
		for (int j = 0; j < n; ++j)
			res(strPos, j) = coef[i][j];
	return res;
}

matrix matrix::toDefCoords()
{
	matrix res(m, n + 1);
	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < n; ++j)
			res(i, j) = coef[i][j];
		res(i, n) = 1;
	}
	return res;
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
		delete coef[i];
	delete coef;
	obj.getDimens(m, n);
	coef = new double*[m];
	for (int i = 0; i < m; ++i)
		coef[i] = new double[n];
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
	res.fill(0);
	
	for (int i = 0; i < m; ++i)
	{
		for (int strPos = 0; strPos < n; ++strPos)
			for (int j = 0; j < nL; ++j)
				res(i, strPos) += left.getElem(i, j) * right.getElem(j, strPos);
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
			res(i, j) = roundTo(left * right.getElem(i, j), 3);
	return res;
}

matrix matrix::getStr(int m)
{
	matrix res(1, n);
	for (int i = 0; i < n; ++i)
		res(0, i) = coef[m][i];
	return res;
}

void matrix::round(int prec)
{
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			coef[i][j] = roundPrec(coef[i][j], 3);
}

void matrix::fill(double val)
{
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			coef[i][j] = val;
}

matrix matrix::reverse()
{
	if (m != n)
		return matrix(0, 0);
	double det = roundTo(this->determ(), 3);
	matrix rev_matr(m, n);

	for (int i = 0; i < n; i++) 
	{
		for (int j = 0; j < n; j++) 
		{
			matrix temp_matr = this->strikeout(i , j);
			double tempDet = temp_matr.determ();
			double val = temp_matr.determ() / det;
			rev_matr(i, j) = pow(-1.0, i + j + 2) * roundTo(val, 3);
		}


	}
	rev_matr.transport();
	return rev_matr;
}

void matrix::transport()
{
	if (m != n)
		return;
	double buf;
	for (int i = 0; i < m; ++i)
		for (int j = i + 1; j < n; ++j)
		{
			buf = coef[i][j];
			coef[i][j] = coef[j][i];
			coef[j][i] = buf;
		}
}

double matrix::determ()
{
	if (m != n)
		return 0;

	double detVal = 0;   //временная переменная для хранения определителя
	int k = 1;      //степень

	if (n == 1)
		detVal = coef[0][0];
	else if (n == 2)
		detVal = coef[0][0] * coef[1][1] - coef[1][0] * coef[0][1];
	else {
		for (int i = 0; i < n; i++) {
			int m = n - 1;
			matrix temp_matr = this->strikeout(0, i);
			detVal = detVal + k * coef[0][i] * temp_matr.determ();
			k = -k;
		}
	}
	return detVal;
}

matrix matrix::strikeout(int r, int c)
{
	matrix res(m - 1, n - 1);
	int ki = 0;
	for (int i = 0; i < m; ++i) 
	{
		if (i != r) 
		{
			for (int j = 0, kj = 0; j < n; ++j) 
				if (j != c) 
				{
					res(ki, kj) = coef[i][j];
					kj++;
				}
			ki++;
		}
	}
	return res;
}

matrix operator-(matrix& l, matrix& r)
{
	int ml, nl, mr, nr;
	l.getDimens(ml, nl);
	r.getDimens(mr, nr);

	if (ml != mr || nl != nr)
		return matrix(0, 0);
	matrix res(ml, nl);
	for (int i = 0; i < ml; ++i)
		for (int j = 0; j < nl; ++j)
			res(i, j) = l(i, j) - r(i, j);
	return res;
}
matrix operator+(matrix& l, matrix& r)
{
	int ml, nl, mr, nr;
	l.getDimens(ml, nl);
	r.getDimens(mr, nr);

	if (ml != mr || nl != nr)
		return matrix(0, 0);
	matrix res(ml, nl);
	for (int i = 0; i < ml; ++i)
		for (int j = 0; j < nl; ++j)
			res(i, j) = l(i, j) + r(i, j);
	return res;
}

int definePoint(matrix& p0, int x, int y, bool mode, coordDescr d)
{
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
	matrix cp0 = p0 * dim;
	int s = 3;
	int m, n;
	p0.getDimens(m, n);
	n = -1;
	for (int i = 0; i < m; ++i)
	{
		int xp = cp0(i, 0) * d.cx + d.x0;
		int yp = -cp0(i, 1) * d.cy + d.y0;
		if ((x > (xp - s)) && (x < (xp  + s)) && (y > (yp - s)) && (y < (yp + s)))
			n = i;
	}
	return n;
}

int definePoint(matrix& p0, int x, int y, coordDescr d)
{
	int s = 3;
	int m, n;
	p0.getDimens(m, n);
	n = -1;
	for (int i = 0; i < m; ++i)
	{
		int xp = p0(i, 0) * d.cx + d.x0;
		int yp = -p0(i, 1) * d.cy + d.y0;
		if ((x >(xp - s)) && (x < (xp + s)) && (y >(yp - s)) && (y < (yp + s)))
			n = i;
	}
	return n;
}

std::bitset<4> defineCodes(const matrix& seg, const matrix& win)
{
	std::bitset<4> code;
	for (int i = 0; i < 4; ++i)
		code[i] = 0;
	if (seg.getElem(0, 0) < win.getElem(0, 0)) //left
		code[0] = 1;
	if (seg.getElem(0, 0) > win.getElem(1, 0)) //right
		code[1] = 1;
	if (seg.getElem(0, 1) < win.getElem(3, 1)) //bottom
		code[2] = 1;
	if (seg.getElem(0, 1) > win.getElem(0, 1)) //top
		code[3] = 1;
	return code;
}

matrix defineVisibleSegment(const matrix& seg, const matrix& win)
{
	int pNum = 0;
	matrix visSeg = seg;
	matrix ps = seg.subMatrix(0, 1);
	matrix pe = seg.subMatrix(1, 1);
	std::bitset<4> cps = defineCodes(ps, win);
	std::bitset<4> cpe = defineCodes(pe, win);
	if ((cps == 0) && (cpe == 0))
		return visSeg;
	while ((cps != 0) || (cpe != 0))
	{
		if ((cps & cpe) != 0)
		{
			visSeg = matrix(0, 0);
			return visSeg;
		}

		if (cps == 0)
		{
			++pNum;
			pe = ps;
			ps = seg.subMatrix(1, 1);
			std::bitset<4> buf = cps;
			cps = cpe;
			cpe = buf;
		}

		if (cps[0] != 0 || cps[1] != 0)
		{
			if (cps[0] != 0) //left
				visSeg(pNum, 0) = win.getElem(0, 0);
			else			 //right
				visSeg(pNum, 0) = win.getElem(1, 0);
			double t = (visSeg(pNum, 0) - ps(0, 0)) / (pe(0, 0) - ps(0, 0));
			visSeg(pNum, 1) = ps(0, 1) + (pe(0, 1) - ps(0, 1)) * t;
		}
		else if (cps[2] != 0 || cps[3] != 0)
		{
			if (cps[2] != 0) //bottom
				visSeg(pNum, 1) = win.getElem(3, 1);
			else			 //top
				visSeg(pNum, 1) = win.getElem(0, 1);
			double t = (visSeg(pNum, 1) - ps(0, 1)) / (pe(0, 1) - ps(0, 1));
			visSeg(pNum, 0) = ps(0, 0) + (pe(0, 0) - ps(0, 0)) * t;
		}
		ps(0, 0) = visSeg(pNum, 0);
		ps(0, 1) = visSeg(pNum, 1);
		cps = defineCodes(ps, win);
	}
	return visSeg;
}

matrix calcBodyMatrix(const matrix& fig, const std::vector<std::vector<int> >& edges)
{
	int m, n;
	m = edges.size();
	n = edges[0].size();
	matrix planes(4, m);

	for (int k = 0; k < m; ++k)//for all edges
	{
		double a, b, c, d;
		a = b = c = d = 0;
		for (int i = 0; i < 4; ++i)//through all dots
		{
			int e = edges[k][i]; //edge  num
			int ne = edges[k][(i + 1) % 4];//next edge num
			a += (fig.getElem(e, 1) - fig.getElem(ne, 1))*(fig.getElem(e, 2) + fig.getElem(ne, 2));
			b += (fig.getElem(e, 2) - fig.getElem(ne, 2))*(fig.getElem(e, 0) + fig.getElem(ne, 0));
			c += (fig.getElem(e, 0) - fig.getElem(ne, 0))*(fig.getElem(e, 1) + fig.getElem(ne, 1));
		}
		d = -(a * fig.getElem(edges[k][0], 0) + b * fig.getElem(edges[k][0], 1) + c * fig.getElem(edges[k][0], 2));
		planes(0, k) = a;
		planes(1, k) = b;
		planes(2, k) = c;
		planes(3, k) = d;
	}

	//check signs
	matrix point(1, 4);
	point.fill(0);
	int m1, n1;
	fig.getDimens(m1, n1);
	for (int k = 0; k < 4; ++k)
	{
		for (int i = 0; i < m1; ++i)
			point(0, k) += fig.getElem(i, k);
		point(0, k) = point(0, k) / m1;
	}
	matrix check = point * planes;
	for (int i = 0; i < m; ++i)
	{
		if (check(0, i) < 0)
			for (int j = 0; j < 4; ++j)
				planes(j, i) *= -1;
	}
	return planes;
}