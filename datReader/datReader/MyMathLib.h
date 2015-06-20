#pragma once

#define PI 3.14159265

class CMyMathLib {
public:
	~CMyMathLib(void);

	static float dotProd(myVector *first, myVector *second);
	static float mag(myVector *in);
	static float angleBtwVector(myVector *first, myVector *second);
	static myVector bezierCubic(float u, myVector p0, myVector p1, myVector p2, myVector p3);
	static myVector straightLine(float t, myVector p0, myVector p1);

private:
	CMyMathLib(void);
	static float bezierInternal(float u, float u1, float x0, float x1, float x2, float x3);
};