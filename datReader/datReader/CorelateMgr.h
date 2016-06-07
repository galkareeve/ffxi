#pragma once
#include <vector>
#include "myStruct.h"

class CCorelateMgr
{
public:
	CCorelateMgr(void);
	~CCorelateMgr(void);

	void corelate(std::ofstream &ofs, int startIndices, std::vector<myFace> &vF, std::vector<myVector> &vV);
	int findMaxVertexIndices(std::vector<myFace> &in);
	myFace* getFaceContainVertexIndices(int desiredIndices, std::vector<myFace> &vF);

	bool checkNormal(myVector *first, myVector *second);
	bool findFirstVertexPos(int firstPos, myFace *f, std::vector<myVector> &vV, int &outPos);

private:
	int m_startIndices;
	int m_maxVertexIndices;
	int m_firstVertexPos;
	int m_maxSequence;
	//first pos with the best sequence match
	int m_bestfirstVertexPos;
};

