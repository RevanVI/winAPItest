#pragma once
#include <Windows.h>
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

	double* getStr(int m);

	double& operator()(int, int);
	matrix& operator=(const matrix& obj);
	friend matrix operator*(const matrix&, const matrix&);
	friend matrix operator*(double, const matrix&);
	
};


bool drawLine(HDC hdc, int x0, int y0, int x, int y);
bool drawPol2Dim(HDC hdc, matrix fig, COLORREF color, coordDescr descr);
bool drawBrickDim(HDC hdc, int x, int y, int w, int h, int d, COLORREF color);
void create2DGrid(HDC hdc, coordDescr d);