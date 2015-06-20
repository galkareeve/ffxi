#ifndef matrix4_h
#define matrix4_h

#include <iostream>
#include "Mathex.h"

class TVector;
using namespace std;

class TMatrix44
{
	private:
		float _Mx[4][4];

		// Input output
		ostream &write(ostream &out) const;
		istream &read(istream &in);

	public:
		// Constructors
		TMatrix44();
		TMatrix44(float Phi, float Theta, float Psi);
		TMatrix44(float mx00, float mx01, float mx02, float mx03, float mx10, float mx11, float mx12, float mx13, float mx20, float mx21, float mx22, float mx23,
			float mx30, float mx31, float mx32, float mx33);

		// Selectors
		float operator()(int Row, int Column) const { return _Mx[Row][Column]; }
		float &operator()(int Row, int Column) { return _Mx[Row][Column]; }

		// Optimised artimetric methods
		static TMatrix44 &add(const TMatrix44 &m1, const TMatrix44 &m2, TMatrix44 &result);
		static TMatrix44 &subtract(const TMatrix44 &m1, const TMatrix44 &m2, TMatrix44 &result);
		static TMatrix44 &multiply(const TMatrix44 &m1, const TMatrix44 &m2, TMatrix44 &result);
		static TMatrix44 &multiply(const TMatrix44 &m1, const float &scale, TMatrix44 &result);
		static TVector &multiply(const TMatrix44 &m1, const TVector &v, TVector &result);

		// Matrix arithmetic
		TMatrix44 &operator+=(const TMatrix44 &m) { return add(*this, m, *this); }
		TMatrix44 &operator-=(const TMatrix44 &m) { return subtract(*this, m, *this); }
		TMatrix44 &operator*=(const TMatrix44 &m) { TMatrix44 tm(*this); return multiply(tm, m, *this); }
		TMatrix44 &operator*=(const float &scale) { return multiply(*this, scale, *this); }
		TMatrix44 operator+(const TMatrix44 &m) const { TMatrix44 tm; return add(*this, m, tm); }
		TMatrix44 operator-(const TMatrix44 &m) const { TMatrix44 tm; return subtract(*this, m, tm); }
		TMatrix44 operator*(const TMatrix44 &m) const { TMatrix44 tm; return multiply(*this, m, tm); }
		TMatrix44 operator*(const float &scale) const { TMatrix44 tm; return multiply(*this, scale, tm); }
		TVector operator*(const TVector &v) const;

		// Determinants
		float determinant() const;
		static float determinant(const TMatrix44 &m) { return m.determinant(); }

		// Transpose
		TMatrix44 &transpose();
		static TMatrix44 &transpose(const TMatrix44 &m, TMatrix44 &result) { result = m; return result.transpose(); }
		static TMatrix44 transpose(const TMatrix44 &m) { return TMatrix44(m).transpose(); }

		// Inverse
		static TMatrix44 &TMatrix44::inverse(const TMatrix44 &m1, TMatrix44 &result);
		static TMatrix44 TMatrix44::inverse(const TMatrix44 &m1) { TMatrix44 tm; return inverse(m1, tm); }
		TMatrix44 inverse() const { TMatrix44 tm; return inverse(*this, tm); }

		void TMatrix44::setTranslation(float x, float y, float z) { _Mx[3][0]=x; _Mx[3][1]=y; _Mx[3][2]=z; }
		void TMatrix44::multiplyByVector3(float &x, float &y, float &z, float w);

		void setM( float arr[] );

		// Stream
		friend ostream &operator<<(ostream &out, const TMatrix44 &o) { return o.write(out); }
		friend istream &operator>>(istream &in, TMatrix44 &o) { return o.read(in); }
};

#endif
