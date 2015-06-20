#pragma once

#include "imesh.h"
#include <vector>
#include <glm/glm.hpp>
#include "myEnum.h"

#pragma pack(push,1)

struct SMMBHEAD {
	char id[3];
	long   type:7;
	long   next:19;
	long   is_shadow:1;
	long   is_extracted:1;
	long   ver_num:3;
	long   is_virtual:1;
	char unk[9];
};

struct SMMBHEAD2 {
	unsigned int MMBSize:24;
	unsigned int d1:8;
	unsigned short d3:8;
	unsigned short d4:8;
	char unk2[2];		//FF FF
	char name[8];
};

//57 byte
typedef struct
{
	glm::u8 flg;
	char id[16];
	glm::u32 dwnazo1;			//nazo = unknown
	long  imgx, imgy;
	glm::u32 dwnazo2[6];
	glm::u32 widthbyte;
} IMGINFO05;

//57 Byte
//typedef struct
//{
//	glm::u8  flg;
//	char id[16];
//	glm::u32 dwnazo1;			//nazo = unknown
//	long  imgx, imgy;
//	glm::u32 dwnazo2[6];
//	glm::u32 widthbyte;
//	glm::u32 palet[0x100];
//} IMGINFO;
//61 byte
typedef struct
{
	glm::u8  flg;
	char id[16];
	glm::u32 dwnazo1;			//nazo = unknown
	long  imgx, imgy;
	glm::u32 dwnazo2[6];
	glm::u32 widthbyte;
	glm::u32 unk;				//B1-extra unk, 01-no unk
	glm::u32 palet[0x100];
} IMGINFOB1;
//69 byte
typedef struct
{
	glm::u8  flg;
	char id[16];
	glm::u32 dwnazo1;
	long  imgx, imgy;
	glm::u32 dwnazo2[6];
	glm::u32 widthbyte;
	char ddsType[4];
	unsigned int size;
	unsigned int noBlock;
} IMGINFOA1;

typedef struct
{
	char ddsType[4];
	unsigned int size;
	unsigned int noBlock;
} IMGINFO81_DDS;

#pragma pack(pop)


struct SLandscapeTextureInfo {
	unsigned int id;
	std::string name;
};

struct SMZBHeader {
	char id[4];
	unsigned int totalRecord100;
	unsigned int offsetHeader2;
	unsigned int d1:8;
	unsigned int d2:8;
	unsigned int d3:8;
	unsigned int d4:8;
	int offsetOctree;		//???
	unsigned int offsetEndRecord100;	
	unsigned int offsetEndOctree;
	int unk5;
};

struct SMZBHeader2 {
	unsigned int totalRecord92;
	unsigned int offsetBlock92;
	unsigned int totalBlock16;
	unsigned int offsetBlock16;
	unsigned int offsetVertex;
	unsigned int offsetBlock112;
	unsigned int totalRecord112;
	int unk1;
};

//used for MMB object
struct SMZBBlock84 {
	char id[16];
	float fTransX,fTransY,fTransZ;
	float fRotX,fRotY,fRotZ;
	float fScaleX,fScaleY,fScaleZ;
	float fa,fb,fc,fd;				//0, 10, 100, 1000
	unsigned int i1, i2, i3, i4;
};

//observed in dat 116
struct SMZBBlock92b {
	char id[16];
	float fTransX,fTransY,fTransZ;
	float fRotX,fRotY,fRotZ;
	float fScaleX,fScaleY,fScaleZ;
	float fa,fb,fc,fd;				//0, 10, 100, 1000
	unsigned int i1, i2, i3, i4, i5, i6;
};
//same as OBJINFO, used for MZB object
//no. of objects in map, each object can point to the same MMB model
struct SMZBBlock100 {
	char id[16];
	float fTransX,fTransY,fTransZ;
	float fRotX,fRotY,fRotZ;
	float fScaleX,fScaleY,fScaleZ;
	float fa,fb,fc,fd;				//0, 10, 100, 1000
	long  fe,ff,fg,fh,fi,fj,fk,fl;
};
//variable length, no of vertex/normal depend on range.
struct SMZBBlock92 {
	unsigned int range1;	//start of Vertex offset
	unsigned int range2;	//end of vertex, start of normal offset  ==> (range2 - range1 == no. of vertex)
	unsigned int range3;	//end of normal offset  ==> (range3 - range2 = no. of normal)
	char unk1[4];			//2, 1,
	float f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15;	//12 float (x,y,z) + 3 float (hx,hy,hz)
	char footer[16];		//0, 1, 2, 0, 1, 3, 2, 0
};
//f1-f16 = transform matrix(4X4) for vertex, f17-f28 transform matrix(4X3) for normal
struct SMZBBlock112 {
	float f1,f2,f3,f4;			//1, 0, 0, 0
	float f5,f6,f7,f8;			//0, 1, 0, 0
	float f9,f10,f11,f12;		//0, 0, 1, 0
	float f13,f14,f15,f16;		//-180, 0, -180, 1
	float f17,f18,f19;			//1, 0, 0
	float f20,f21,f22;			//0, 1, 0
	float f23,f24,f25;			//0, 0, 1
	float f26,f27,f28;			//0, 0, 0
};
//variable length, need to read until 0
struct SMZBBlock16 {
	unsigned int i1;	//unknown
	unsigned int i2;	//offset112
	unsigned int i3;	//offset92
	unsigned int i4;	//0
};

//s1, s2, s3 == vertex index, s4=normal index (index to SMZBBlock92)
struct SMZBBlock8 {
	unsigned short s1;
	unsigned short s2;
	unsigned short s3;
	unsigned short s4;
};

struct SMZBVector {
	float x;
	float y;
	float z;
};

struct SMZBFace {
	unsigned int i1;
	unsigned int i2;
	unsigned int i3;
};

struct SMMBHeader {
	char imgID[16];
	int pieces;			//No of BlockHeader
	float x1,x2;		//BoundingRec Combine all BlockHeader (min,max)
	float y1,y2;
	float z1,z2;
	unsigned int offsetBlockHeader;	//offset to first SMMBBlockHeader
};

struct SMMBBlockHeader {
	int numModel;		//no of model block
	float x1,x2;		//BoundingRec Single (min,max)
	float y1,y2;
	float z1,z2;
	int numFace;
};
//part of SMMBVertexIndices, used to get the vertexsize
struct SMMBModelHeader {
	char textureName[16];
	unsigned int vertexsize;			//No of SMMBBlockVertex
};

struct SMMBBlockVertex {
	float x,y,z;
	float hx,hy,hz;
	unsigned int color;
	float u, v;
};

struct SMMBBlockVertex2 {
	float x,y,z;
	float dx,dy,dz;		//displacement?? cloth
	float hx,hy,hz;
	unsigned int color;
	float u, v;
};

struct SMMBVertexIndices {
	GL_DRAWTYPE drawType;
	char textureName[16];
	unsigned int numVertex;
	std::vector<SMMBBlockVertex2> vecVertex;	//some model with [numFace==0 or d3=2], uses SMMBBlockVertex2 structure
	unsigned int numIndices;					//numIndices is the original num of index in the dat before extraction
	std::vector<glm::u16> vecIndices;
};

class CMMB
{
public:
	CMMB(void){ m_Size=0;};
	~CMMB(void){};

	unsigned int m_Size;				//total length
	SMMBHeader m_SMMBHeader;			//contain the imgID
	std::vector<SMMBVertexIndices> m_SMMBVertexIndices;
};

class IDriver;
class IMeshBuffer;
class CMeshBufferGroup;
class CSceneManager;
class CFFXILandscapeMesh :	public IMesh
{
public:
	CFFXILandscapeMesh(IDriver *in, unsigned int tid);
	virtual ~CFFXILandscapeMesh(void);
	//virtual based method
	bool animate(int frame);
	void recalculateBoundingBox();
	bool loadModelFile(std::string FN, CSceneManager *mgr);
	void loadDependency(std::string dependStr);
	void writeMeshInfo(int frame, int mbIndex);

	CMeshBufferGroup* getMeshBufferGroup(unsigned int i);
	void getMeshBufferGroup(std::vector<CMeshBufferGroup*> &out) {out=m_meshBufferGroup;}
	void addMeshBufferGroup(CMeshBufferGroup *in) {m_meshBufferGroup.push_back(in); };
	int getMeshBufferGroupCount() { return m_meshBufferGroup.size(); };
	void refreshMeshBufferGroup(unsigned int i, bool isMZB);
	
	int BitCount(unsigned char x);
	std::string extractImageName(char *p, glm::u32 &width, glm::u32 &height, glm::u8 *& ppImage);
	void toggleMMBTransform();
	bool isMMBTransform() { return m_isTransform; }
	int getB100count() { return m_vecMZB.size(); }
	int getMMBCount() { return m_vecMMB.size(); }
	void getMZB100Matrix(SMZBBlock100 *in, glm::mat4 &out);
	
	void trimSpace(char *des, char *src, int len);
	bool in_frustum(glm::mat4 M, glm::vec3 p);

private:
	IDriver *p_driver;

	bool m_isTransform;
	unsigned int m_DefaultTextureID;
	int m_numFrameLoaded;			//num of frame loaded
	std::vector<CMeshBufferGroup*> m_meshBufferGroup;
	std::vector<SLandscapeTextureInfo> m_vectextureInfo;

	std::vector<SMZBBlock100> m_vecMZB;
	std::vector<CMMB*> m_vecMMB;
	std::vector<CMMB*> m_vecDependMMB;

	//MZB extraction
	unsigned int m_lastIndices;
	void decode_mmb2(unsigned char *p);
	void decode_mmb(unsigned char*p);
	bool decode_mzb(unsigned char* p, unsigned int maxLen);

	void extractMMB(char *p, unsigned int len, bool isDepend=false);
	void extractMZB(char *p, unsigned int len);
	
	int findMMBIndex(SMZBBlock100 *in);
	bool lookupMMB(int mzbIndex, int mmbIndex, SMZBBlock100 *in);
//	bool lookupMMB(SMZBBlock100 *in);
//	void MMBTransform(SMZBBlock100 *b84, glm::vec3 &v);
	void decodeMesh(char *p, unsigned int offsetB112, unsigned int offsetB92, IMeshBuffer *mb);
	void multiplyByMatrix( SMZBBlock112 *mat, float &vx, float &vy, float &vz);
	void multiplyByMatrixNormal( SMZBBlock112 *mat, float &vx, float &vy, float &vz);
	unsigned short limit( unsigned int max, unsigned short val);
};

