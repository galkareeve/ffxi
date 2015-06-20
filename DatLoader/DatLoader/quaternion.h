#pragma once

#include <math.h>
#include "Tmatrix4.h"

class quaternion
{
public:
	quaternion(float x, float y, float z, float w) : W(w), X(x), Y(y), Z(z) {}
	~quaternion(void){}

	void normalized() {
		float n = 1.0f/sqrt(W*W + X*X + Y*Y + Z*Z);
		W *=n;
		X *=n;
		Y *=n;
		Z *=n;
	}

	void getMatrix(TMatrix44 &out) {
		out(0,0) = 1.0f - 2.0f*Y*Y - 2.0f*Z*Z;
		out(0,1) = 2.0f*X*Y + 2.0f*Z*W;
		out(0,2) = 2.0f*X*Z - 2.0f*Y*W;
		out(0,3) = 0.0f;

		out(1,0) = 2.0f*X*Y - 2.0f*Z*W;
		out(1,1) = 1.0f - 2.0f*X*X - 2.0f*Z*Z;
		out(1,2) = 2.0f*Z*Y + 2.0f*X*W;
		out(1,3) = 0.0f;

		out(2,0) = 2.0f*X*Z + 2.0f*Y*W;
		out(2,1) = 2.0f*Z*Y - 2.0f*X*W;
		out(2,2) = 1.0f - 2.0f*X*X - 2.0f*Y*Y;
		out(2,3) = 0.0f;

		out(3,0) = 0.f;
		out(3,1) = 0.f;
		out(3,2) = 0.f;
		out(3,3) = 1.f;
	}

	void toAxix(float &angle, float &x, float &y, float &z) {
		if (W > 1) normalized(); // if w>1 acos and sqrt will produce errors, this cant happen if quaternion is normalised
		angle = 2 * 180.f * acos(W) / 3.14159265;
		double s = sqrt(1.0f-W*W); // assuming quaternion normalised then w is less than 1, so term always positive.
		if (s < 0.001) {		// test to avoid divide by zero, s is always positive due to sqrt
								// if s close to zero then direction of axis not important
			x = X;				// if it is important that axis is normalised then replace with x=1; y=z=0;
			y = Y;
			z = Z;
		} 
		else {
			x = X / s; // normalise axis
			y = Y / s;
			z = Z / s;
		}
	}

	float W;
	float X;
	float Y;
	float Z;
};