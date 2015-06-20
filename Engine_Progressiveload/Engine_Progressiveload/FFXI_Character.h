#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "TDWAnalysis.h"
#include "myEnum.h"

#define PARTS 10

struct SVertexNormal {
	glm::vec3 vertex;
	glm::vec3 normal;
};

struct SVertexNormal2 {
	glm::vec2 x;
	glm::vec2 y;
	glm::vec2 z;
	glm::vec2 w;
	glm::vec2 hx;
	glm::vec2 hy;
	glm::vec2 hz;
};

struct SBindPosVertex {
	glm::vec4 x1;
	glm::vec4 x2;
	glm::vec4 h1;
	glm::vec4 h2;
};

struct SCloth {
	glm::vec3 vertex;
};

struct SCloth2 {
	glm::vec2 x;
	glm::vec2 y;
	glm::vec2 z;
	glm::vec2 w;
};

#pragma pack(push,1)
struct SFace {
	glm::u16 indices;
	glm::vec2 uv;
};

struct SFace3 {
	glm::u16vec3 indices;
	glm::vec2 uv1;
	glm::vec2 uv2;
	glm::vec2 uv3;
};

#pragma pack(pop)

struct SFaceMB {
	GL_DRAWTYPE type;
	std::string m_textureName;
	std::vector<SFace> vecface;
};

struct SAnimInfo {
	unsigned int frameStart;
	unsigned int frameEnd;
	int dat2BIndex;
	float speed;
};

//parts is the equivalent of meshBuffer
//FFXI_Character->parts,  CFFXIMesh->meshBuffer
struct SPartsInfo {
	char name[17];
	int parts_index;
};

struct SBoneMatrix {
	char parent;
	char term;
	glm::fmat4x4 jointLocalMat;			//unused
	glm::fmat4x4 skinLocalMat;			//after multi with animation matrix
	glm::f32quat jointLocalRotation;	//relative to immediate parent	(from dat)
	glm::vec3 jointLocalTranslation;	//relative to immediate parent	(from dat)
	glm::f32quat animationRotation;		//animation (from dat)
	glm::vec3 animationTranslation;	//animation (from dat)
	glm::f32quat skinLocalRotation;		//bindpos multiply with animationRotation
	glm::vec3 skinLocalTranslation;	//bindpos multiply with animationTranslation
	std::vector<int> child;
};


/*
Every FFXIPart is created when encounter addVertex (twice if needflip, mirror side)
each part can have multiple face. (on encounter TriangleList / TriangleStrip), which require its own meshBuffer
each face is an array of indices & uv
each face can reference different texture
each face generate a meshBuffer (TRIANGLE_STRIP / TRIANGLE_LIST)
each part ONLY have 1 storage for vertices/normal, and is overwritten after skinMesh
each part contain an array of vertices (bindPosVertex), it is converted to its final pos thru skinMesh
*/
class FFXIParts
{
public:
	FFXIParts(bool f){needflip=f;m_gldrawType=E_TRIANGLE_LIST;m_meshBufferStartIndex=0;};
	~FFXIParts(void){};

	//ffxi vertices contain 4 member
	int getBindPosVertexCount() { return m_vecBindPosVertices.size(); }
	void addBindPosVertex(SBindPosVertex bv) {
		m_vecBindPosVertices.push_back(bv);
	}
	void updateBindPosVertex(int i, SBindPosVertex bv) {
		m_vecBindPosVertices[i] = bv;
	}
	SBindPosVertex getBindPosVertex(int i) {
		return m_vecBindPosVertices[i];
	}

	//gl vertices
	void clearVertices() { m_vecVertices.clear(); m_vecNormal.clear(); }
	int getVertexCount() {
		return m_vecVertices.size();
	}
	void addVertex(glm::vec3 v) {
		m_vecVertices.push_back(v);
	}
	void updateVertex(int i, glm::vec3 v) {
		m_vecVertices[i] = v;
	}
	glm::vec3 getVertex(int i) {
		return m_vecVertices[i];
	}

	void addNormal(glm::vec3 n) {
		m_vecNormal.push_back(n);
	}
	void updateNormal(int i, glm::vec3 n) {
		m_vecNormal[i] = n;
	}
	glm::vec3 getNormal(int i) {
		return m_vecNormal[i];
	}

	BONE4 getBone4(int i) {
		return m_vecBone4[i];
	}
	void addBone4(BONE4 b4) {
		m_vecBone4.push_back(b4);
	}

	//int getIndicesCount() {
	//	return m_vecIndices.size();
	//}
	//void addIndices(glm::u16vec3 tl) {
	//	m_vecIndices.push_back(tl);
	//}
	//glm::u16vec3 getIndices(int i) {
	//	return m_vecIndices[i];
	//}

	bool needFlip() { return needflip; }

	void addFace( SFaceMB *in ) {
		m_vecFace.push_back(in);
	}
	unsigned int getFaceCount() { return m_vecFace.size(); }
	SFaceMB* getFace(unsigned int i) {
		if(i>=m_vecFace.size())
			return NULL;
		return m_vecFace[i];
	}

	void setMeshBufferStartIndex(unsigned int ind) { m_meshBufferStartIndex=ind; }
	unsigned int getMeshBufferStartIndex() { return m_meshBufferStartIndex; }

protected:
	bool needflip;
	unsigned int m_meshBufferStartIndex;				//startIndex + m_vecFace.size() == all the meshBuffer index for this part
	GL_DRAWTYPE m_gldrawType;
	
	std::vector<BONE4> m_vecBone4;
	std::vector<SBindPosVertex> m_vecBindPosVertices;		//ffxi vert contain 4 member
	std::vector<SFaceMB*> m_vecFace;						//can be TRIANGLE_LIST or TRIANGLE_STRIP
	//does not change
	std::vector<glm::u16vec3> m_vecIndices;
	std::vector<glm::vec2> m_vecUV;
	//update after skinMesh
	std::vector<glm::vec3> m_vecVertices;
	std::vector<glm::vec3> m_vecNormal;
};

class IMeshBuffer;
class CFFXI_Character
{
public:
	CFFXI_Character(void);
	~CFFXI_Character(void);

	bool m_useQuaternion;
	BONE *dat29;
	//Every call to addVertex will create a FFXIParts which contain many Face
	std::vector<FFXIParts*> m_parts;

	//BONE2 dat29_2[128]; //Ç±ÇÍÇÕ128åèîzóÒÇ™Ç†ÇËÇ‹Ç∑
	//float *dat29_3;
	int  dat29Suu;								//total joint count
	std::vector<SBoneMatrix*> matBones;
	std::vector<DAT2BHeader*> dat2Bs;
	int curAnimFrame;							//total Frame count
	std::vector<SAnimInfo> arrAnimInfo;
	int m_curAnimeDat2BIndex;					//point to current Animation to be play

	void nextAnimation(int &frameStart, int &frameEnd, float &speed);
	void setCurrentAnimation(int i, int &frameStart, int &frameEnd, float &speed);
	int getAnimIndex(int frameStart, int frameEnd);
	int getTotalAnimationFrame() { return curAnimFrame; }
	int getNumAnimation() { return arrAnimInfo.size(); }
	SAnimInfo getAnimationInfo(int i) { return arrAnimInfo[i]; }

	bool animate(int frame);
	int getJointAnimation(int i, DAT2BHeader *p2b, int frame);
	int skinJoint(int frame);
	void skinMesh(FFXIParts*part);
	void updateMeshBuffer(unsigned int mIndex, unsigned int frame, unsigned int pn, IMeshBuffer *inout);

	std::string extractImageName(char *p, glm::u32 &width, glm::u32 &height, glm::u8 *& ppImage);
	int AddVertex(char *p, int sz);
	//extract vertices/normal
	void buildBindPosVertex(DAT2A *dat, FFXIParts *pMB, bool flip);
	//extract face (indices/uv)
	void buildBindPosPartFace(DAT2A *dat, FFXIParts *pMB, bool flip);

	void resetBoneMatrix() {		
		for(unsigned int i=0;i<matBones.size();i++) {
			SBoneMatrix *pbm = matBones[i];
			pbm->skinLocalRotation = pbm->jointLocalRotation;
			pbm->skinLocalTranslation = pbm->jointLocalTranslation;
			pbm->animationRotation = glm::f32quat(0,0,0,0);
			pbm->animationTranslation = glm::vec3(0,0,0);
		}
	}

	int  GetBoneNo(DAT2A *d2a, glm::u16 no, bool flgFlip, int &flg, int c);
	bool SetBone(char *p,int sz) {
		dat29Suu = (int)*(short*)(p+0x2);
		if( dat29 ) {
			delete []dat29; 
			dat29 = NULL;
		}
		dat29 = new BONE[dat29Suu];
		memcpy(dat29,p+0x4,sizeof(BONE)*dat29Suu);

		//CopyMemory(&dat29_2,p+0x14+dat29Suu*sizeof(BONE)+4,sizeof(BONE2)*128);
		//dat29_3 = (float*)(&dat29_2[128]);
		
		for( int ii=0; ii<dat29Suu; ii++ ){
			SBoneMatrix *pbm = new SBoneMatrix;
			glm::fmat4x4 m1,m2;
			BONE &bn = dat29[ii];
			glm::f32quat q(bn.w, bn.i,bn.j,bn.k);
			pbm->jointLocalRotation = q;
			glm::vec3 v(bn.x,bn.y,bn.z);
			pbm->jointLocalTranslation=v;
			pbm->jointLocalMat = glm::mat4_cast(q);
			pbm->jointLocalMat[3] = glm::vec4(v,1);
			pbm->term=bn.term;
			if( ii>0 ) {
				pbm->parent=bn.parent;
				//add self to parent
				matBones[pbm->parent]->child.push_back(ii);
			}
			else {
				pbm->parent=255;
			}
			matBones.push_back(pbm);
		}
		return true;
	}

	glm::vec3 getRotatedTranslation( glm::f32quat q, glm::vec3 v) {
		//(w,x,y,z)
		glm::f32quat tq(0, v.x, v.y, v.z);
//		glm::normalize(q);
		//when q is normalize, qCong == qInverse
		//actual formula is q * v * qInverse
		glm::f32quat qCong = glm::conjugate(q);
		// q * v * qConjugate
		glm::f32quat res = q * tq * qCong;
		glm::vec3 ret(res.x, res.y, res.z);
		return ret;
	}

	bool AddAnimation(char *p,int sz)
	{
		//ensure 4 byte align
		int size = sz/4;
		size *=4;
		char *pp = new char[size];
		memcpy(pp,p,size);
		SAnimInfo ai;
		DAT2BHeader *d = (DAT2BHeader*)pp;
		//record animInfo using frameStart/frameEnd
		//since all animation frame start from 1, we need to combine all frame
		ai.frameStart=curAnimFrame;
		ai.frameEnd = curAnimFrame + d->frame-1;
		ai.dat2BIndex = dat2Bs.size();
		ai.speed = d->speed;
		//An animInfo block contain the frame start/end/speed and the actual dat2B block (animationRotation, Translation)
		arrAnimInfo.push_back(ai);
		dat2Bs.push_back((DAT2BHeader*)pp);

		curAnimFrame += d->frame;
		return true;
	}

	int getPartsCount() {
		return m_parts.size();
	}
	FFXIParts* getParts(int i) {
		return m_parts[i];
	}

	bool dat2Bsclear(void)
	{
		std::vector<DAT2BHeader*>::iterator it;
		for(it=dat2Bs.begin();it!=dat2Bs.end();it++){
			delete [](char*)(*it);
		}
		dat2Bs.clear();
		return true;
	}
	bool Clear(void){
		curAnimFrame=1;
		dat2Bsclear();
		matBones.clear();
		if( dat29 ) delete []dat29; dat29 = NULL;
		dat29Suu = 0;
		return true;
	}
};

