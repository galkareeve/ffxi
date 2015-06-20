/*******************************************************************************/
/*********************************21/09/200*************************************/
/**********************Programmer: Dimitrios Christopoulos**********************/
/**********************for the oglchallenge contest*****************************/
/**********************COLLISION CRAZY******************************************/
/*******************************************************************************/
#include "stdafx.h"
#include "tmatrix4.h"
#include "tvector.h"

TMatrix44::TMatrix44() {
	_Mx[0][0]=1.0; _Mx[0][1]=0.0; _Mx[0][2]=0.0;
	_Mx[1][0]=0.0; _Mx[1][1]=1.0; _Mx[1][2]=0.0;
	_Mx[2][0]=0.0; _Mx[2][1]=0.0; _Mx[2][2]=1.0;
}

TMatrix44::TMatrix44(float mx00, float mx01, float mx02, float mx03,
			float mx10, float mx11, float mx12, float mx13, 
			float mx20, float mx21, float mx22, float mx23,
			float mx30, float mx31, float mx32, float mx33) {
	_Mx[0][0]=mx00; _Mx[0][1]=mx01; _Mx[0][2]=mx02; _Mx[0][3]=mx03;
	_Mx[1][0]=mx10; _Mx[1][1]=mx11; _Mx[1][2]=mx12; _Mx[1][3]=mx13;
	_Mx[2][0]=mx20; _Mx[2][1]=mx21; _Mx[2][2]=mx22; _Mx[2][3]=mx23;
	_Mx[3][0]=mx30; _Mx[3][1]=mx31; _Mx[3][2]=mx32; _Mx[3][3]=mx33;
}


TMatrix44::TMatrix44(float Phi, float Theta, float Psi) {
	float c1=cos(Phi), s1=sin(Phi), c2=cos(Theta), s2=sin(Theta), c3=cos(Psi), s3=sin(Psi);
	_Mx[0][0]=c2*c3;
	_Mx[0][1]=-c2*s3;
	_Mx[0][2]=s2;
	_Mx[1][0]=s1*s2*c3+c1*s3;
	_Mx[1][1]=-s1*s2*s3+c1*c3;
	_Mx[1][2]=-s1*c2;
	_Mx[2][0]=-c1*s2*c3+s1*s3;
	_Mx[2][1]=c1*s2*s3+s1*c3;
	_Mx[2][2]=c1*c2;
}

TMatrix44 &TMatrix44::add(const TMatrix44 &m1, const TMatrix44 &m2, TMatrix44 &result) {
	result._Mx[0][0] = m1._Mx[0][0] + m2._Mx[0][0];
	result._Mx[0][1] = m1._Mx[0][1] + m2._Mx[0][1];
	result._Mx[0][2] = m1._Mx[0][2] + m2._Mx[0][2];
	result._Mx[0][3] = m1._Mx[0][3] + m2._Mx[0][3];
	result._Mx[1][0] = m1._Mx[1][0] + m2._Mx[1][0];
	result._Mx[1][1] = m1._Mx[1][1] + m2._Mx[1][1];
	result._Mx[1][2] = m1._Mx[1][2] + m2._Mx[1][2];
	result._Mx[1][3] = m1._Mx[1][3] + m2._Mx[1][3];
	result._Mx[2][0] = m1._Mx[2][0] + m2._Mx[2][0];
	result._Mx[2][1] = m1._Mx[2][1] + m2._Mx[2][1];
	result._Mx[2][2] = m1._Mx[2][2] + m2._Mx[2][2];
	result._Mx[2][3] = m1._Mx[2][3] + m2._Mx[2][3];
	result._Mx[3][0] = m1._Mx[3][0] + m2._Mx[3][0];
	result._Mx[3][1] = m1._Mx[3][1] + m2._Mx[3][1];
	result._Mx[3][2] = m1._Mx[3][2] + m2._Mx[3][2];
	result._Mx[3][3] = m1._Mx[3][3] + m2._Mx[3][3];
	return result;
}

TMatrix44 &TMatrix44::subtract(const TMatrix44 &m1, const TMatrix44 &m2, TMatrix44 &result) {
	result._Mx[0][0] = m1._Mx[0][0] - m2._Mx[0][0];
	result._Mx[0][1] = m1._Mx[0][1] - m2._Mx[0][1];
	result._Mx[0][2] = m1._Mx[0][2] - m2._Mx[0][2];
	result._Mx[0][3] = m1._Mx[0][3] - m2._Mx[0][3];
	result._Mx[1][0] = m1._Mx[1][0] - m2._Mx[1][0];
	result._Mx[1][1] = m1._Mx[1][1] - m2._Mx[1][1];
	result._Mx[1][2] = m1._Mx[1][2] - m2._Mx[1][2];
	result._Mx[1][3] = m1._Mx[1][3] - m2._Mx[1][3];
	result._Mx[2][0] = m1._Mx[2][0] - m2._Mx[2][0];
	result._Mx[2][1] = m1._Mx[2][1] - m2._Mx[2][1];
	result._Mx[2][2] = m1._Mx[2][2] - m2._Mx[2][2];
	result._Mx[2][3] = m1._Mx[2][3] - m2._Mx[2][3];
	result._Mx[3][0] = m1._Mx[3][0] - m2._Mx[3][0];
	result._Mx[3][1] = m1._Mx[3][1] - m2._Mx[3][1];
	result._Mx[3][2] = m1._Mx[3][2] - m2._Mx[3][2];
	result._Mx[3][3] = m1._Mx[3][3] - m2._Mx[3][3];
	return result;
}

TMatrix44 &TMatrix44::multiply(const TMatrix44 &m1, const TMatrix44 &m2, TMatrix44 &result) {
	result._Mx[0][0] = m1._Mx[0][0]*m2._Mx[0][0] + m1._Mx[0][1]*m2._Mx[1][0] + m1._Mx[0][2]*m2._Mx[2][0] + m1._Mx[0][3]*m2._Mx[3][0];
	result._Mx[1][0] = m1._Mx[1][0]*m2._Mx[0][0] + m1._Mx[1][1]*m2._Mx[1][0] + m1._Mx[1][2]*m2._Mx[2][0] + m1._Mx[1][3]*m2._Mx[3][0];
	result._Mx[2][0] = m1._Mx[2][0]*m2._Mx[0][0] + m1._Mx[2][1]*m2._Mx[1][0] + m1._Mx[2][2]*m2._Mx[2][0] + m1._Mx[2][3]*m2._Mx[3][0];
	result._Mx[3][0] = m1._Mx[3][0]*m2._Mx[0][0] + m1._Mx[3][1]*m2._Mx[1][0] + m1._Mx[3][2]*m2._Mx[2][0] + m1._Mx[3][3]*m2._Mx[3][0];
	result._Mx[0][1] = m1._Mx[0][0]*m2._Mx[0][1] + m1._Mx[0][1]*m2._Mx[1][1] + m1._Mx[0][2]*m2._Mx[2][1] + m1._Mx[0][3]*m2._Mx[3][1];
	result._Mx[1][1] = m1._Mx[1][0]*m2._Mx[0][1] + m1._Mx[1][1]*m2._Mx[1][1] + m1._Mx[1][2]*m2._Mx[2][1] + m1._Mx[1][3]*m2._Mx[3][1];
	result._Mx[2][1] = m1._Mx[2][0]*m2._Mx[0][1] + m1._Mx[2][1]*m2._Mx[1][1] + m1._Mx[2][2]*m2._Mx[2][1] + m1._Mx[2][3]*m2._Mx[3][1];
	result._Mx[3][1] = m1._Mx[3][0]*m2._Mx[0][1] + m1._Mx[3][1]*m2._Mx[1][1] + m1._Mx[3][2]*m2._Mx[2][1] + m1._Mx[3][3]*m2._Mx[3][1];
	result._Mx[0][2] = m1._Mx[0][0]*m2._Mx[0][2] + m1._Mx[0][1]*m2._Mx[1][2] + m1._Mx[0][2]*m2._Mx[2][2] + m1._Mx[0][3]*m2._Mx[3][2];
	result._Mx[1][2] = m1._Mx[1][0]*m2._Mx[0][2] + m1._Mx[1][1]*m2._Mx[1][2] + m1._Mx[1][2]*m2._Mx[2][2] + m1._Mx[1][3]*m2._Mx[3][2];
	result._Mx[2][2] = m1._Mx[2][0]*m2._Mx[0][2] + m1._Mx[2][1]*m2._Mx[1][2] + m1._Mx[2][2]*m2._Mx[2][2] + m1._Mx[2][3]*m2._Mx[3][2];
	result._Mx[3][2] = m1._Mx[3][0]*m2._Mx[0][2] + m1._Mx[3][1]*m2._Mx[1][2] + m1._Mx[3][2]*m2._Mx[2][2] + m1._Mx[3][3]*m2._Mx[3][2];
	result._Mx[0][3] = m1._Mx[0][0]*m2._Mx[0][3] + m1._Mx[0][1]*m2._Mx[1][3] + m1._Mx[0][2]*m2._Mx[2][3] + m1._Mx[0][3]*m2._Mx[3][3];
	result._Mx[1][3] = m1._Mx[1][0]*m2._Mx[0][3] + m1._Mx[1][1]*m2._Mx[1][3] + m1._Mx[1][2]*m2._Mx[2][3] + m1._Mx[1][3]*m2._Mx[3][3];
	result._Mx[2][3] = m1._Mx[2][0]*m2._Mx[0][3] + m1._Mx[2][1]*m2._Mx[1][3] + m1._Mx[2][2]*m2._Mx[2][3] + m1._Mx[2][3]*m2._Mx[3][3];
	result._Mx[3][3] = m1._Mx[3][0]*m2._Mx[0][3] + m1._Mx[3][1]*m2._Mx[1][3] + m1._Mx[3][2]*m2._Mx[2][3] + m1._Mx[3][3]*m2._Mx[3][3];
	return result;
}

TMatrix44 &TMatrix44::multiply(const TMatrix44 &m1, const float &scale, TMatrix44 &result) {
	result._Mx[0][0] = m1._Mx[0][0] * scale;
	result._Mx[0][1] = m1._Mx[0][1] * scale;
	result._Mx[0][2] = m1._Mx[0][2] * scale;
	result._Mx[0][3] = m1._Mx[0][3] * scale;
	result._Mx[1][0] = m1._Mx[1][0] * scale;
	result._Mx[1][1] = m1._Mx[1][1] * scale;
	result._Mx[1][2] = m1._Mx[1][2] * scale;
	result._Mx[1][3] = m1._Mx[1][3] * scale;
	result._Mx[2][0] = m1._Mx[2][0] * scale;
	result._Mx[2][1] = m1._Mx[2][1] * scale;
	result._Mx[2][2] = m1._Mx[2][2] * scale;
	result._Mx[2][3] = m1._Mx[2][3] * scale;
	result._Mx[3][0] = m1._Mx[3][0] * scale;
	result._Mx[3][1] = m1._Mx[3][1] * scale;
	result._Mx[3][2] = m1._Mx[3][2] * scale;
	result._Mx[3][3] = m1._Mx[3][3] * scale;
	return result;
}

TVector &TMatrix44::multiply(const TMatrix44 &m1, const TVector &v, TVector &result) {
	result = TVector(
   		m1._Mx[0][0]*v.X() + m1._Mx[0][1]*v.Y() + m1._Mx[0][2]*v.Z(),
		m1._Mx[1][0]*v.X() + m1._Mx[1][1]*v.Y() + m1._Mx[1][2]*v.Z(),
		m1._Mx[2][0]*v.X() + m1._Mx[2][1]*v.Y() + m1._Mx[2][2]*v.Z() );
	return result;
}

float TMatrix44::determinant() const {
	return _Mx[0][0]*(_Mx[1][1]*_Mx[2][2]-_Mx[1][2]*_Mx[2][1])
		- _Mx[0][1]*(_Mx[1][0]*_Mx[2][2]-_Mx[1][2]*_Mx[2][0])
		+ _Mx[0][2]*(_Mx[1][0]*_Mx[2][1]-_Mx[1][1]*_Mx[2][0]);
}

TMatrix44 &TMatrix44::transpose() {
	float 	t=_Mx[0][1]; _Mx[0][1]=_Mx[1][0]; _Mx[1][0]=t;
	t=_Mx[0][2]; _Mx[0][2]=_Mx[2][0]; _Mx[2][0]=t;
	t=_Mx[0][3]; _Mx[0][3]=_Mx[3][0]; _Mx[3][0]=t;
	t=_Mx[1][2]; _Mx[1][2]=_Mx[2][1]; _Mx[2][1]=t;
	t=_Mx[1][3]; _Mx[1][3]=_Mx[3][1]; _Mx[3][1]=t;
	t=_Mx[2][3]; _Mx[2][3]=_Mx[3][2]; _Mx[3][2]=t;
	return *this;
}

//TMatrix44 &TMatrix44::inverse(const TMatrix44 &m1, TMatrix44 &result) {
//	float det = m1.determinant();
//	if (fabs(det) < EPSILON) {
//		result = TMatrix44();
//		return result;
//	} else {
//		result._Mx[0][0] = m1._Mx[1][1]*m1._Mx[2][2] - m1._Mx[1][2]*m1._Mx[2][1];
//		result._Mx[0][1] = m1._Mx[2][1]*m1._Mx[0][2] - m1._Mx[2][2]*m1._Mx[0][1];
//		result._Mx[0][2] = m1._Mx[0][1]*m1._Mx[1][2] - m1._Mx[0][2]*m1._Mx[1][1];
//		result._Mx[1][0] = m1._Mx[1][2]*m1._Mx[2][0] - m1._Mx[1][0]*m1._Mx[2][2];
//		result._Mx[1][1] = m1._Mx[2][2]*m1._Mx[0][0] - m1._Mx[2][0]*m1._Mx[0][2];
//		result._Mx[1][2] = m1._Mx[0][2]*m1._Mx[1][0] - m1._Mx[0][0]*m1._Mx[1][2];
//		result._Mx[2][0] = m1._Mx[1][0]*m1._Mx[2][1] - m1._Mx[1][1]*m1._Mx[2][0];
//		result._Mx[2][1] = m1._Mx[2][0]*m1._Mx[0][1] - m1._Mx[2][1]*m1._Mx[0][0];
//		result._Mx[2][2] = m1._Mx[0][0]*m1._Mx[1][1] - m1._Mx[0][1]*m1._Mx[1][0];
//		return multiply(result, 1.0/det, result);
//	}
//}

TMatrix44 &TMatrix44::inverse(const TMatrix44 &m1, TMatrix44 &result)
{
	float d = (m1._Mx[0][0] *  m1._Mx[1][1] - m1._Mx[0][1] * m1._Mx[1][0]) * (m1._Mx[2][2] * m1._Mx[3][3] - m1._Mx[2][3] * m1._Mx[3][2]) -
			(m1._Mx[0][0] * m1._Mx[1][2] - m1._Mx[0][2] * m1._Mx[1][0]) * (m1._Mx[2][1] * m1._Mx[3][3] - m1._Mx[2][3] * m1._Mx[3][1]) +
			(m1._Mx[0][0] * m1._Mx[1][3] - m1._Mx[0][3] * m1._Mx[1][0]) * (m1._Mx[2][1] * m1._Mx[3][2] - m1._Mx[2][2] * m1._Mx[3][1]) +
			(m1._Mx[0][1] * m1._Mx[1][2] - m1._Mx[0][2] * m1._Mx[1][1]) * (m1._Mx[2][0] * m1._Mx[3][3] - m1._Mx[2][3] * m1._Mx[3][0]) -
			(m1._Mx[0][1] * m1._Mx[1][3] - m1._Mx[0][3] * m1._Mx[1][1]) * (m1._Mx[2][0] * m1._Mx[3][2] - m1._Mx[2][2] * m1._Mx[3][0]) +
			(m1._Mx[0][2] * m1._Mx[1][3] - m1._Mx[0][3] * m1._Mx[1][2]) * (m1._Mx[2][0] * m1._Mx[3][1] - m1._Mx[2][1] * m1._Mx[3][0]);

		if (fabs(d) < EPSILON) {
			result = TMatrix44();
			return result;
		}

		d = 1.0/d;

		result._Mx[0][0] = d * (m1._Mx[1][1] * (m1._Mx[2][2] * m1._Mx[3][3] - m1._Mx[2][3] * m1._Mx[3][2]) +
				m1._Mx[1][2] * (m1._Mx[2][3] * m1._Mx[3][1] - m1._Mx[2][1] * m1._Mx[3][3]) +
				m1._Mx[1][3] * (m1._Mx[2][1] * m1._Mx[3][2] - m1._Mx[2][2] * m1._Mx[3][1]));
		result._Mx[0][1] = d * (m1._Mx[2][1] * (m1._Mx[0][2] * m1._Mx[3][3] - m1._Mx[0][3] * m1._Mx[3][2]) +
				m1._Mx[2][2] * (m1._Mx[0][3] * m1._Mx[3][1] - m1._Mx[0][1] * m1._Mx[3][3]) +
				m1._Mx[2][3] * (m1._Mx[0][1] * m1._Mx[3][2] - m1._Mx[0][2] * m1._Mx[3][1]));
		result._Mx[0][2] = d * (m1._Mx[3][1] * (m1._Mx[0][2] * m1._Mx[1][3] - m1._Mx[0][3] * m1._Mx[1][2]) +
				m1._Mx[3][2] * (m1._Mx[0][3] * m1._Mx[1][1] - m1._Mx[0][1] * m1._Mx[1][3]) +
				m1._Mx[3][3] * (m1._Mx[0][1] * m1._Mx[1][2] - m1._Mx[0][2] * m1._Mx[1][1]));
		result._Mx[0][3] = d * (m1._Mx[0][1] * (m1._Mx[1][3] * m1._Mx[2][2] - m1._Mx[1][2] * m1._Mx[2][3]) +
				m1._Mx[0][2] * (m1._Mx[1][1] * m1._Mx[2][3] - m1._Mx[1][3] * m1._Mx[2][1]) +
				m1._Mx[0][3] * (m1._Mx[1][2] * m1._Mx[2][1] - m1._Mx[1][1] * m1._Mx[2][2]));
		result._Mx[1][0] = d * (m1._Mx[1][2] * (m1._Mx[2][0] * m1._Mx[3][3] - m1._Mx[2][3] * m1._Mx[3][0]) +
				m1._Mx[1][3] * (m1._Mx[2][2] * m1._Mx[3][0] - m1._Mx[2][0] * m1._Mx[3][2]) +
				m1._Mx[1][0] * (m1._Mx[2][3] * m1._Mx[3][2] - m1._Mx[2][2] * m1._Mx[3][3]));
		result._Mx[1][1] = d * (m1._Mx[2][2] * (m1._Mx[0][0] * m1._Mx[3][3] - m1._Mx[0][3] * m1._Mx[3][0]) +
				m1._Mx[2][3] * (m1._Mx[0][2] * m1._Mx[3][0] - m1._Mx[0][0] * m1._Mx[3][2]) +
				m1._Mx[2][0] * (m1._Mx[0][3] * m1._Mx[3][2] - m1._Mx[0][2] * m1._Mx[3][3]));
		result._Mx[1][2] = d * (m1._Mx[3][2] * (m1._Mx[0][0] * m1._Mx[1][3] - m1._Mx[0][3] * m1._Mx[1][0]) +
				m1._Mx[3][3] * (m1._Mx[0][2] * m1._Mx[1][0] - m1._Mx[0][0] * m1._Mx[1][2]) +
				m1._Mx[3][0] * (m1._Mx[0][3] * m1._Mx[1][2] - m1._Mx[0][2] * m1._Mx[1][3]));
		result._Mx[1][3] = d * (m1._Mx[0][2] * (m1._Mx[1][3] * m1._Mx[2][0] - m1._Mx[1][0] * m1._Mx[2][3]) +
				m1._Mx[0][3] * (m1._Mx[1][0] * m1._Mx[2][2] - m1._Mx[1][2] * m1._Mx[2][0]) +
				m1._Mx[0][0] * (m1._Mx[1][2] * m1._Mx[2][3] - m1._Mx[1][3] * m1._Mx[2][2]));
		result._Mx[2][0] = d * (m1._Mx[1][3] * (m1._Mx[2][0] * m1._Mx[3][1] - m1._Mx[2][1] * m1._Mx[3][0]) +
				m1._Mx[1][0] * (m1._Mx[2][1] * m1._Mx[3][3] - m1._Mx[2][3] * m1._Mx[3][1]) +
				m1._Mx[1][1] * (m1._Mx[2][3] * m1._Mx[3][0] - m1._Mx[2][0] * m1._Mx[3][3]));
		result._Mx[2][1] = d * (m1._Mx[2][3] * (m1._Mx[0][0] * m1._Mx[3][1] - m1._Mx[0][1] * m1._Mx[3][0]) +
				m1._Mx[2][0] * (m1._Mx[0][1] * m1._Mx[3][3] - m1._Mx[0][3] * m1._Mx[3][1]) +
				m1._Mx[2][1] * (m1._Mx[0][3] * m1._Mx[3][0] - m1._Mx[0][0] * m1._Mx[3][3]));
		result._Mx[2][2] = d * (m1._Mx[3][3] * (m1._Mx[0][0] * m1._Mx[1][1] - m1._Mx[0][1] * m1._Mx[1][0]) +
				m1._Mx[3][0] * (m1._Mx[0][1] * m1._Mx[1][3] - m1._Mx[0][3] * m1._Mx[1][1]) +
				m1._Mx[3][1] * (m1._Mx[0][3] * m1._Mx[1][0] - m1._Mx[0][0] * m1._Mx[1][3]));
		result._Mx[2][3] = d * (m1._Mx[0][3] * (m1._Mx[1][1] * m1._Mx[2][0] - m1._Mx[1][0] * m1._Mx[2][1]) +
				m1._Mx[0][0] * (m1._Mx[1][3] * m1._Mx[2][1] - m1._Mx[1][1] * m1._Mx[2][3]) +
				m1._Mx[0][1] * (m1._Mx[1][0] * m1._Mx[2][3] - m1._Mx[1][3] * m1._Mx[2][0]));
		result._Mx[3][0] = d * (m1._Mx[1][0] * (m1._Mx[2][2] * m1._Mx[3][1] - m1._Mx[2][1] * m1._Mx[3][2]) +
				m1._Mx[1][1] * (m1._Mx[2][0] * m1._Mx[3][2] - m1._Mx[2][2] * m1._Mx[3][0]) +
				m1._Mx[1][2] * (m1._Mx[2][1] * m1._Mx[3][0] - m1._Mx[2][0] * m1._Mx[3][1]));
		result._Mx[3][1] = d * (m1._Mx[2][0] * (m1._Mx[0][2] * m1._Mx[3][1] - m1._Mx[0][1] * m1._Mx[3][2]) +
				m1._Mx[2][1] * (m1._Mx[0][0] * m1._Mx[3][2] - m1._Mx[0][2] * m1._Mx[3][0]) +
				m1._Mx[2][2] * (m1._Mx[0][1] * m1._Mx[3][0] - m1._Mx[0][0] * m1._Mx[3][1]));
		result._Mx[3][2] = d * (m1._Mx[3][0] * (m1._Mx[0][2] * m1._Mx[1][1] - m1._Mx[0][1] * m1._Mx[1][2]) +
				m1._Mx[3][1] * (m1._Mx[0][0] * m1._Mx[1][2] - m1._Mx[0][2] * m1._Mx[1][0]) +
				m1._Mx[3][2] * (m1._Mx[0][1] * m1._Mx[1][0] - m1._Mx[0][0] * m1._Mx[1][1]));
		result._Mx[3][3] = d * (m1._Mx[0][0] * (m1._Mx[1][1] * m1._Mx[2][2] - m1._Mx[1][2] * m1._Mx[2][1]) +
				m1._Mx[0][1] * (m1._Mx[1][2] * m1._Mx[2][0] - m1._Mx[1][0] * m1._Mx[2][2]) +
				m1._Mx[0][2] * (m1._Mx[1][0] * m1._Mx[2][1] - m1._Mx[1][1] * m1._Mx[2][0]));

		return result;
}

TVector TMatrix44::operator*(const TVector &v) const {
	TVector tv;
	return multiply(*this, v, tv);
}

ostream &TMatrix44::write(ostream &out) const {
	return out<<"("<<_Mx[0][0]<<","<<_Mx[0][1]<<","<<_Mx[0][2]<<")"<<endl
		<<"("<<_Mx[1][0]<<","<<_Mx[1][1]<<","<<_Mx[1][2]<<")"<<endl
		<<"("<<_Mx[2][0]<<","<<_Mx[2][1]<<","<<_Mx[2][2]<<")"<<endl;
}

istream &TMatrix44::read(istream &in) {
	char ch;
	return in>>ch>>_Mx[0][0]>>ch>>_Mx[0][1]>>ch>>_Mx[0][2]>>ch
		>>ch>>_Mx[1][0]>>ch>>_Mx[1][1]>>ch>>_Mx[1][2]>>ch
		>>ch>>_Mx[2][0]>>ch>>_Mx[2][1]>>ch>>_Mx[2][2]>>ch;
}


void TMatrix44::multiplyByVector3(float &x, float &y, float &z, float w)
{
	float tx = _Mx[0][0]*x + _Mx[1][0]*y + _Mx[2][0]*z + _Mx[3][0]*w;
	float ty = _Mx[0][1]*x + _Mx[1][1]*y + _Mx[2][1]*z + _Mx[3][1]*w;
	float tz = _Mx[0][2]*x + _Mx[1][2]*y + _Mx[2][2]*z + _Mx[3][2]*w;

	x=tx;
	y=ty;
	z=tz;
}

void TMatrix44::setM( float arr[] )
{
	_Mx[0][0] = arr[0];
	_Mx[0][1] = arr[1];
	_Mx[0][2] = arr[2];
	_Mx[0][3] = arr[3];
	_Mx[1][0] = arr[4];
	_Mx[1][1] = arr[5];
	_Mx[1][2] = arr[6];
	_Mx[1][3] = arr[7];
	_Mx[2][0] = arr[8];
	_Mx[2][1] = arr[9];
	_Mx[2][2] = arr[10];
	_Mx[2][3] = arr[11];
	_Mx[3][0] = arr[12];
	_Mx[3][1] = arr[13];
	_Mx[3][2] = arr[14];
	_Mx[3][3] = arr[15];
}
