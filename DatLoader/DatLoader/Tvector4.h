#ifndef tvector4_h
#define tvector4_h


#include <iostream>
#include <math.h>
#include "mathex.h"

//class TRay;
using namespace std;

class TVector4 {
	public:
		enum TStatus { INVALID, DEFAULT, UNIT };

	private:
		double _w, _x, _y, _z;
		TStatus _Status;

		// Constructors
		TVector4(double w, double x, double y, double z, TStatus s) : _w(w), _x(x), _y(y), _z(z), _Status(s) {}

		// Input and output
		ostream &write(ostream &out) const;
		istream &read(istream &in);

	public:
		// Constructors
		TVector4() : _w(0.0), _x(0.0), _y(0.0), _z(0.0), _Status(INVALID) {}
		TVector4(double w, double x, double y, double z) : _w(w), _x(x), _y(y), _z(z), _Status(DEFAULT) {}

		// Mid point between two lines
//		TVector4(const TRay &line1, const TRay &line2);

		// Selectors
		double W() const { return _w; }
		double X() const { return _x; }
		double Y() const { return _y; }
		double Z() const { return _z; }
		int isUnit() const { return _Status==UNIT; }
		int isDefault() const { return _Status==DEFAULT; }
		int isValid() const { return _Status!=INVALID; }

		// Change the status of a vector
		TVector4 &unit();
		static TVector4 &unit(const TVector4 &v, TVector4 &result) { result = v; return result.unit(); }
		static TVector4 unit(const TVector4 &v) { return TVector4(v).unit(); }

		TVector4 &Default();
		static TVector4 Default(const TVector4 &v, TVector4 &result) { result = v; return result.Default(); }
		static TVector4 Default(const TVector4 &v) { return TVector4(v).Default(); }

		// Magnitude
		double mag() const { return (isValid() ? (isUnit() ? 1.0 : sqrt(sqr(W()) + sqr(X()) + sqr(Y()) + sqr(Z()))) : 0.0); }
		double magSqr() const { return (isValid() ? (isUnit() ? 1.0 : sqr(W()) + sqr(X()) + sqr(Y()) + sqr(Z())) : 0.0); }

		// Dot or scalar product
		double dot(const TVector4 &v) const { return ((isValid() && v.isValid()) ? (X()*v.X() + Y()*v.Y() + Z()*v.Z()) : 0.0); }
		static double dot(const TVector4 &v1, const TVector4 &v2) { return v1.dot(v2); }

		// Distance between two vectors
		double dist(const TVector4 &v) const { return (*this-v).mag(); }
		double distSqr(const TVector4 &v) const { return (*this-v).magSqr(); }

		// Optimised arithmetic methods
		static TVector4 &add(const TVector4 &v1, const TVector4 &v2, TVector4 &result);
		static TVector4 &subtract(const TVector4 &v1, const TVector4 &v2, TVector4 &result);
		static TVector4 &cross(const TVector4 &v1, const TVector4 &v2, TVector4 &result);
		static TVector4 &invert(const TVector4 &v1, TVector4 &result);
		static TVector4 &multiply(const TVector4 &v1, const double &scale, TVector4 &result);

		// Vector arithmetic, addition, subtraction and vector product
		TVector4 operator-() const { return invert(*this, TVector4()); }
		TVector4 &operator+=(const TVector4 &v) { return add(*this, v, *this); }
		TVector4 &operator-=(const TVector4 &v) { return subtract(*this, v, *this); }
		TVector4 &operator*=(const TVector4 &v) { TVector4 tv(*this); return cross(tv, v, *this); }
		TVector4 &operator*=(const double &scale) { return multiply(*this, scale, *this); }
		TVector4 operator+(const TVector4 &v) const { TVector4 tv; return add(*this, v, tv); }
		TVector4 operator-(const TVector4 &v) const { TVector4 tv; return subtract(*this, v, tv); }
		TVector4 operator*(const TVector4 &v) const { TVector4 tv; return cross(*this, v, tv); }
		TVector4 operator*(const double &scale) const { TVector4 tv; return multiply(*this, scale, tv); }

		// Streaming
		friend ostream &operator<<(ostream &out, const TVector4 &o) { return o.write(out); }
		friend istream &operator>>(istream &in, TVector4 &o) { return o.read(in); }
};

#endif

