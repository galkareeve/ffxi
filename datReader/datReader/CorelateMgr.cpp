#include "StdAfx.h"
#include "stdafx.h"
#include "CorelateMgr.h"
#include <iostream>
#include <fstream>
#include "MyMathLib.h"

using namespace std;
CCorelateMgr::CCorelateMgr(void)
{
}


CCorelateMgr::~CCorelateMgr(void)
{
}

void CCorelateMgr::corelate(ofstream &ofs, int startIndices, vector<myFace> &vF, vector<myVector> &vV)
{
	string str;
	char buf[500];


	//init
	m_firstVertexPos=startIndices;
	m_maxSequence=0;
	m_bestfirstVertexPos=0;
	m_startIndices = startIndices;

	myFace *f=nullptr;
	int tmp,count;
	m_maxVertexIndices = findMaxVertexIndices(vF);
	int max = vV.size();
	while(m_firstVertexPos<max) {
		f = getFaceContainVertexIndices(m_startIndices, vF);
		if( f==nullptr ) break;

		if( findFirstVertexPos(m_firstVertexPos,f,vV,tmp) ) {
			count=1;
			m_firstVertexPos=tmp;
			//no need to check further since reaching end
			if( (m_firstVertexPos+m_maxVertexIndices) >=max ) break;

			//firstVertex is indices 0
			for(int j=m_startIndices+1; j<m_maxVertexIndices; ++j ) {
				//find the face that contain the current vertexIndices
				f = getFaceContainVertexIndices(j, vF);
				if( f==nullptr ) break;
				if( f->v1!=j ) {
					if( !checkNormal(&vV[m_firstVertexPos+j], &vV[m_firstVertexPos+f->v1]))
						break;
				}
				if( f->v2!=j ) {
					if( !checkNormal(&vV[m_firstVertexPos+j], &vV[m_firstVertexPos+f->v2]))
						break;
				}
				if( f->v3!=j ) {
					if( !checkNormal(&vV[m_firstVertexPos+j], &vV[m_firstVertexPos+f->v3]))
						break;
				}
				++count;
			}
			if( count>m_maxSequence ) {
				m_maxSequence=count;
				m_bestfirstVertexPos=m_firstVertexPos;
			}
			if( count==m_maxVertexIndices ) {
				sprintf_s(buf,500,"Success! vertexPos for indices: %d (%d %d %d) @ %d\n", m_startIndices, f->v1,f->v2,f->v3,m_firstVertexPos);
				str.assign(buf);
				ofs.write(str.c_str(),str.length());
				break;
			}
			else if( count > 1 ) {
				sprintf_s(buf,500,"found partial match for indices: %d (%d %d %d) @ %d, sequence:%d\n", m_startIndices, f->v1,f->v2,f->v3,m_firstVertexPos, count );
				str.assign(buf);
				ofs.write(str.c_str(), str.length());
			}
		}
		else
			break;

		++m_firstVertexPos;
	}
	if( (m_maxSequence>0) && (m_maxSequence<m_maxVertexIndices) ) {
		sprintf_s(buf,500,"best first vertexPos for indices: %d (%d %d %d) @ %d, sequence:%d\n", m_startIndices, f->v1,f->v2,f->v3,m_bestfirstVertexPos,m_maxSequence);
		str.assign(buf);
		ofs.write(str.c_str(),str.length());
	}
}

int CCorelateMgr::findMaxVertexIndices(vector<myFace> &in)
{
	int max=0;
	myFace *f=nullptr;
	int size=in.size();
	for(int i=0; i<size; ++i) {
		f = &in[i];
		if( f->v1>max ) max=f->v1;
		if( f->v2>max ) max=f->v2;
		if( f->v3>max ) max=f->v3;
	}
	return max;
}

myFace* CCorelateMgr::getFaceContainVertexIndices(int desiredIndices, std::vector<myFace> &vF)
{
	myFace *f=nullptr;
	int size=vF.size();
	for(int i=0; i<size; ++i) {
		f = &vF[i];
		if( (f->v1==desiredIndices) || (f->v2==desiredIndices) || (f->v3==desiredIndices) )
			return f;
	}
	return nullptr;
}
//i is the indices of interest
//find out the possible position of i (outPos)
bool CCorelateMgr::checkNormal(myVector *first, myVector *second)
{
	float angle = CMyMathLib::angleBtwVector(first, second);
	return ( (angle<30.0)? true:false );
}

//f contian the face with indices=0
bool CCorelateMgr::findFirstVertexPos(int firstPos, myFace *f, std::vector<myVector> &vV, int &outPos)
{
	int max=vV.size();
	for(int i=firstPos; i<max; ++i) {
		if( (i+m_maxVertexIndices) >=max )
			return false;

		if( f->v1==m_startIndices ) {
			if( checkNormal(&vV[i], &vV[i+f->v2]) && checkNormal(&vV[i], &vV[i+f->v3])) {
				outPos=i;
				return true;
			}
		}
		if( f->v2==m_startIndices ) {
			if( checkNormal(&vV[i], &vV[i+f->v1]) && checkNormal(&vV[i], &vV[i+f->v3])) {
				outPos=i;
				return true;
			}
		}
		if( f->v3==m_startIndices ) {
			if( checkNormal(&vV[i], &vV[i+f->v1]) &&  checkNormal(&vV[i], &vV[i+f->v2])) {
				outPos=i;
				return true;
			}
		}
	}
	return false;
}
