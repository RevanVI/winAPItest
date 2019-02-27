#pragma once
#include <Windows.h>

bool drawLine(HDC hdc, int x0, int y0, int x, int y);
bool drawBrickDim(HDC hdc, int x, int y, int w, int h, int d, COLORREF color);
void create2DGrid(HDC hdc, int l, int t, int r, int b, int cx, int cy, int x0, int y0);

class matrix
{
private:
	int** coef;
	int m, n;
public: 
	matrix(int m = 2, int n = 2);
	matrix(const matrix& matr);
	~matrix();
	void getDimens(int& m, int& n) const;
	int getElem(int i, int j) const;

	int& operator()(int, int);
	matrix& operator=(const matrix& obj);
	friend matrix operator*(const matrix&, const matrix&);
	friend matrix operator*(int, const matrix&);
	
};
