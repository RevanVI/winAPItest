#pragma once
#include <Windows.h>
#include <vector>
#include <bitset>

struct coordDescr
{
	int l, t, r, b; //Координаты границ области рисования
	int cx, cy, cz; //масштаб осей
	int x0, y0, z0; //начало координат (в пикслелях отн-но рабочей области)
};

class matrix
{
private:
	double** coef;
	int m, n;
public: 
	matrix(int m = 2, int n = 2);
	matrix(const matrix& matr);
	~matrix();
	void getDimens(int& m, int& n) const;
	double getElem(int i, int j) const;
	matrix subMatrix(int str, int c) const;
	matrix getStr(int m);
	void round(int prec);
	matrix reverse();
	double determ();
	matrix strikeout(int m, int n);
	void transport();
	void fill(double val);
	matrix toDefCoords();

	double& operator()(int, int);
	matrix& operator=(const matrix& obj);
	friend matrix operator*(const matrix&, const matrix&);
	friend matrix operator*(double, const matrix&);
	friend matrix operator-(matrix&, matrix&);
	friend matrix operator+(matrix&, matrix&);
};

bool drawLine(HDC hdc, double x0, double y0, double x, double y, coordDescr d);
bool drawLine(HDC hdc, int x0, int y0, int x, int y);
bool drawPol2Dim(HDC hdc, matrix fig, COLORREF color, coordDescr descr, bool closed);
bool drawPol3Dim(HDC hdc, matrix fig, COLORREF color, coordDescr descr);
bool drawBrickDim(HDC hdc, matrix fig, coordDescr d, COLORREF color);
matrix rotateFig(matrix fig, matrix axis, double angle);
void create2DGrid(HDC hdc, coordDescr d);
void create3DGrid(HDC hdc, coordDescr d, bool mode); // mode = true - isometric, mode = false - dim
matrix chordAppr(matrix points);
matrix findCenter(matrix l, matrix r);
matrix* calculatePoints(matrix& p0, int& num); //рассчитывает точки для построения составной кривой Безье 4-го порядка

int definePoint(matrix& p0, int x, int y, bool mode, coordDescr d);
int definePoint(matrix& p0, int x, int y, coordDescr d);

std::bitset<4> defineCodes(const matrix& seg, const matrix& win);

matrix defineVisibleSegment(const matrix& seg, const matrix& win);


matrix calcBodyMatrix(const matrix& fig, const std::vector<std::vector<int>>& edges);