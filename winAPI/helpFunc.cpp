#include "helpFunc.h"
#include <cmath>

double roundTo(double val, int prec)
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

		matrix tM(4, 4);

		matrix tM1 = matrix(4, 4); //матрица перемещени€
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
		if (vec(0, 0) > 0.001 || vec(0, 0) < -0.001) //если составл€юща€ cx не равна нулю
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
		if (vec(0, 2) > 0.001 || vec(0, 2) < -0.001) //если составл€юща€ cz не равна нулю
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

		//матрица перемещени€ (возврат)
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
	drawLine(hdc, d.x0, d.y0, d.x0, d.y0 - d.cy * num); //ось ќy
	drawLine(hdc, oX(0, 0) * d.cx + d.x0, -oX(0, 1) * d.cy + d.y0, oX(1, 0) * d.cx + d.x0, -oX(1, 1) * d.cy + d.y0); //ось ќx
	drawLine(hdc, oZ(0, 0) * d.cx + d.x0, -oZ(0, 1) * d.cy + d.y0, oZ(1, 0) * d.cx + d.x0, -oZ(1, 1) * d.cy + d.y0); //ось ќz

	SelectObject(hdc, oPen);
	DeleteObject(hPen);

	//ось ќх
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

double* matrix::getStr(int m)
{
	return coef[m];
}

void matrix::round(int prec)
{
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			coef[i][j] = roundPrec(coef[i][j], 3);
}

void matrix::reverse()
{

}

double matrix::determ()
{
	if (m != n)
		return 0;

	int detVal = 0;   //временна€ переменна€ дл€ хранени€ определител€
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