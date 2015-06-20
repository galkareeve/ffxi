#pragma once
#include "TDWAnalysis.h"
#include "Tmatrix4.h"

using namespace std;

class CDatLoader
{
public:
	CDatLoader(void);
	~CDatLoader(void);

	bool loadDat(int fno);
	void extractImage(char *p, unsigned int len);
	void extractVertex(char *p, unsigned int len);
	void extractAnimation(char *p, unsigned int len);
	void extractBone(char *p, unsigned int len);
	void extractMZB(char *p, unsigned int len, unsigned int count, bool isValid);
	void extractMMB(char *p, unsigned int len, unsigned int count);
	void decodeMesh(char *p, unsigned int offsetB112, unsigned int offsetB92);
	void multiplyByMatrix( SMZBBlock112 *mat, float &x, float &y, float &z);
	unsigned int limit( unsigned int max, unsigned short val);
	SMZBBlock100* findMMBTransform( char *name);
	void MMBTransform(SMZBBlock100 *objinfo, SMZBVector &v);
	void pushMMBIndices(SMZBFace f);
	void write_MMB();

	MODELVERTEX1* transformVertexMV1(int k, MODELVERTEX1*, WORD flip, bool isIndirect);
	MODELVERTEX2* transformVertexMV2(int k, MODELVERTEX2*, WORD flip, bool isIndirect);

	CLOTHVERTEX1* transformVertexC1(int k, CLOTHVERTEX1 *c1, WORD flip, bool isIndirect);
	CLOTHVERTEX2* transformVertexC2(int k, CLOTHVERTEX2 *c2, WORD flip, bool isIndirect);

	ofstream ofs;
	int cur_mesh;
	int m_fileNo;

	TMatrix44 matrixMirrorX;
	TMatrix44 matrixMirrorY;
	TMatrix44 matrixMirrorZ;

	vector<TMatrix44> m_vecBoneMatrix;
	vector<TEXLIST> m_vecFace;
	//vector<int> vecBoneIndexL;
	//vector<int> vecBoneIndexH;
	//vector<BONE4> vecBone;
	vector<int> m_vecBoneIndexTbl;
	vector<BONE3> m_vecBone3;

	unsigned int m_lastIndices;
	vector<SMZBVector> m_vecVector;
	vector<SMZBFace> m_vecIndices;

	vector<SMZBBlock100> m_vecOBJInfo;
};

