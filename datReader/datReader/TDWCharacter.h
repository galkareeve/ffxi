//------------------------------------------------------------------------------
//  File: TDWCharacter.h
//  Desc:     
//  Revision history:
//      * 2007/08/10 Created by Muzisoft05@163.com.
//
//  Todo:
//
//------------------------------------------------------------------------------
#pragma once

#include <string.h>
#include <vector>
#include <matrix4.h>
#include "TDWAnalysis.h"
#include <quaternion.h>

using namespace irr;
/*------------------------------------------------------------------------------
Begin Class FFXIParts
------------------------------------------------------------------------------*/
//class FFXIParts
//{
//public:
//	FFXIParts(void){};
//	~FFXIParts(void){Clear();};
//
////	core::list<std::string> TexList;
//	std::vector<DAT2A> dat2a;
//	std::vector<BONE3> vecBone3;
//	std::vector<int> vecBoneIndexTbl;
//
//	void GetVertex(DAT2A *d2a,int i, D3DTEXVERTEX *ppp,bool flip);
//
//	int AddVertex(char *p,int sz) {
//		DAT2A tmp; 
//		
//		memset(&tmp,0,sizeof(tmp));
//		char *pp = new char [sz];
//		memcpy(pp,p,sz);
//		tmp.dat2A = pp;
//		DAT2AHeader *pcp=(DAT2AHeader *)tmp.dat2A;
//		tmp.dat2ahead = pcp;
//		if((pcp->type&0x7f)==1) {
//			pcp->type=pcp->type;
//		}
//		tmp.weight1 =  ((short*)(tmp.dat2A+pcp->offsetWeight*2))[0];
//		tmp.weight2 =  ((short*)(tmp.dat2A+pcp->offsetWeight*2))[1];
//		tmp.pBone   =  (u16*)(tmp.dat2A+pcp->offsetBone*2);
//		tmp.pBoneTbl=  (u16*)(tmp.dat2A+pcp->offsetBoneTbl*2);
//		tmp.pVertex   = (MODELVERTEX1*)(tmp.dat2A+pcp->offsetVertex*2);
//		tmp.pVertex2  = (MODELVERTEX2*)(((char*)tmp.pVertex)+ tmp.weight1*0x18    );
//		tmp.pVertexC  = (CLOTHVERTEX1*)(tmp.dat2A+pcp->offsetVertex*2);
//		tmp.pVertexC2 = (CLOTHVERTEX2*)(((char*)tmp.pVertex)+ tmp.weight1*0xC    );
//		p += 2 * (*(short*)(p+0x6));		//p+6 is offsetPoly, there move p to start of uvFace ptr
//		tmp.pPoly   = (char*)(tmp.dat2A + pcp->offsetPoly*2);
//
//		dat2a.push_back(tmp);
//
//		//extract bone tbl
//		vecBoneIndexTbl.clear();
//		int totalsize = pcp->offsetBoneTbl*2;
//		u16 *tblindex;
//		for(int k=0; k<pcp->BoneTblSuu; ++k) {
//			tblindex = (u16*)(pp+totalsize);
//			vecBoneIndexTbl.push_back(*tblindex);
//			totalsize +=2;
//		}
//
//		//extract bone
//		BONE3 *b3 = nullptr;
//		int noB = pcp->BoneSuu/2;		//bone is 4byte, therefore need to divide 2
//		totalsize = pcp->offsetWeight*2;
//		b3 = (BONE3*)(pp+totalsize);
//		int noB1 = (int)b3->low;
//		int noB2 = (int)b3->high;
//		int tblidxL, flgL, tblidxH, flgH;
//		BONE4 b4;
//
//		totalsize = pcp->offsetBone*2;
//		for(int k=0; k<noB1; ++k) {
//			b3 = (BONE3*)(pp+totalsize);
//			vecBone3.push_back(*b3);
//			totalsize +=4;
//		}
//		for(int k=0; k<noB2; ++k) {
//			b3 = (BONE3*)(pp+totalsize);
//			vecBone3.push_back(*b3);
//			totalsize +=4;
//		}
//		return tmp.weight1+tmp.weight2;
//	}
//
//	void addBindPosVertex(MODELVERTEX3 v) {
//		vertexBindPos.push_back(v);
//	}
//
//	void updateBindPosVertex(int i, MODELVERTEX3 v) {
//		vertexBindPos[i] = v;
//	}
//
//	MODELVERTEX3 getBindPosVertex(int i) {
//		return vertexBindPos[i];
//	}
//
//	BONE3 getBoneIndex(int i) {
//		return vecBone3[i];
//	}
//
//	int getBoneTblIndex( u16 i ) {
//		return vecBoneIndexTbl[i];
//	}
//
//	bool dat2aclear(void)
//	{
//		auto it = dat2a.begin();
//		for(;it!=dat2a.end();it++){
//		  delete [](char*)(it->dat2A);
//		}
//		dat2a.clear();
//		return true;
//	}
//
//	bool Clear(void){
//		dat2aclear();
//		return true;
//	}
//};

#define PARTS 10
struct SAnimInfo {
	u32 frameStart;
	u32 frameEnd;
	s32 dat2BIndex;
	f32 speed;
};

struct SMeshBufferInfo {
	char name[17];
	int meshBuffer_index;
};

struct SBoneMatrix {
	u8 parent;
	u8 term;
	core::matrix4 jointLocalMat;
	core::matrix4 skinLocalMat;			//after multi with animation matrix
	//core::matrix4 AnimBindPosMat;		//every animation has it own bindposMat
	core::quaternion jointLocalRotation;
	core::vector3df jointLocalTranslation;
	core::quaternion skinLocalRotation;
	core::vector3df skinLocalTranslation;
	core::quaternion animationRotation;
	core::vector3df animationTranslation;
	core::quaternion bindPosRotation;
	core::vector3df bindPosTranslation;
	std::vector<int> child;
};

class FFXIMeshBuffer
{
public:
	FFXIMeshBuffer(bool f){needflip=f;};
	~FFXIMeshBuffer(void){};

	int getVertexCount() {
		return m_vertexBindPos.size();
	}
	int getFaceCount() {
		return m_Face.size();
	}

	void addBindPosVertex(MODELVERTEX3 v) {
		m_vertexBindPos.push_back(v);
	}

	void updateBindPosVertex(int i, MODELVERTEX3 v) {
		m_vertexBindPos[i] = v;
	}

	MODELVERTEX3 getBindPosVertex(int i) {
		return m_vertexBindPos[i];
	}

	BONE4 getBone4(int i) {
		return m_vecBone4[i];
	}

	void addBone4(BONE4 b4) {
		m_vecBone4.push_back(b4);
	}

	void setTextureName( std::string tn ) {
		m_textureName=tn;
	}
	std::string getTextureName() {
		return m_textureName;
	}

	void addFace(u16 tl) {
		m_Face.push_back(tl);
	}

	u16 getFace(int i) {
		return m_Face[i];
	}

	bool needFlip() { return needflip; }

protected:
	bool needflip;
	std::string m_textureName;
	std::vector<u16> m_Face;
	std::vector<BONE4> m_vecBone4;
	std::vector<MODELVERTEX3> m_vertexBindPos;

};

/*------------------------------------------------------------------------------
Begin Class FFXICharacter
------------------------------------------------------------------------------*/
class FFXICharacter
{
public:
	FFXICharacter(void);
	virtual ~FFXICharacter(void);

	BONE *dat29;
//	std::vector<FFXIParts> parts;
	std::vector<FFXIMeshBuffer*> m_meshBuffer;

	//BONE2 dat29_2[128]; //Ç±ÇÍÇÕ128åèîzóÒÇ™Ç†ÇËÇ‹Ç∑
	//float *dat29_3;
	int  dat29Suu;								//total joint count
	std::vector<SBoneMatrix*> matBones;
	std::vector<DAT2BHeader*> dat2Bs;
	int curAnimFrame;							//total Frame count
	std::vector<SAnimInfo> arrAnimInfo;
	std::vector<SMeshBufferInfo> arrMeshBufferInfo;
	int m_curAnimeDat2BIndex;					//point to current Animation to be play

	void nextAnimation(s32 &frameStart, s32 &frameEnd, f32 &speed);
	void setCurrentAnimation(s32 i, s32 &frameStart, s32 &frameEnd, f32 &speed);
	s32 getAnimIndex(s32 frameStart, s32 frameEnd);
//	int  skinJoint(DAT2BHeader *p2b, u32 frame);
	int skinJoint(s32 frame);
	void skinMesh(u32 pn, std::vector<D3DTEXVERTEX> &outVertex);
	int getJointAnimation(int i, DAT2BHeader *p2b, s32 frame);

	void buildBindPosVertex(DAT2A *dat, FFXIMeshBuffer *pMB, bool flip);
	bool animate(s32 frame);
	int getMesh(u32 pn, std::vector<D3DTEXVERTEX> &outVertex);
//	int Draw2A(DAT2A*it, D3DTEXVERTEX *& ppp);
	
//	void GetVertex(DAT2A *d2a,u32 i, D3DTEXVERTEX *ppp,bool flip);	
//	int  GetBoneMatrix(core::matrix4 *matrix, u16 bno);
	int  GetMotionMatrix(core::matrix4 *matrix, s32 no, DAT2BHeader *p2b, u32 frame );
//	int  GetBoneNo1(DAT2A *d2a, u16 no, bool flgFlip, s32 &flg){ return GetBoneNo(d2a,no,flgFlip,flg,0);}
//	int  GetBoneNo2(DAT2A *d2a, u16 no, bool flgFlip, s32 &flg){ return GetBoneNo(d2a,no,flgFlip,flg,1);}
	std::string extractTextureName(char *p, core::array<u16> &arrIndices);
	std::string extractImageName(char *p, u32 &width, u32 &height, u8 *& ppImage);
	void getBindPosMesh(u32 pn, std::vector<D3DTEXVERTEX> &outVertex, std::string &textureName, std::vector<u16> &outFace );
	int AddVertex(char *p, int sz);

	void resetBoneMatrix() {		
		for(u32 i=0;i<matBones.size();i++) {
			SBoneMatrix *pbm = matBones[i];
			pbm->skinLocalRotation = pbm->jointLocalRotation;
			pbm->skinLocalTranslation = pbm->jointLocalTranslation;
			pbm->animationRotation = core::quaternion(0,0,0,1);
			pbm->animationRotation = core::vector3df(0,0,0);
		}
	}

	int  GetBoneNo(DAT2A *d2a, u16 no, bool flgFlip, s32 &flg, int c);
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
			core::matrix4 m1,m2;
			BONE &bn = dat29[ii];
			core::quaternion q(bn.i,bn.j,bn.k,bn.w);
			pbm->jointLocalRotation = q;
			core::vector3df v(bn.x,bn.y,bn.z);
			pbm->jointLocalTranslation =v;
			pbm->jointLocalMat = q.getMatrix();
			pbm->jointLocalMat.setTranslation(v);

			pbm->term=bn.term;
			if( ii>0 ) {
				pbm->parent=bn.parent;
				getbindPosMatrix(ii, pbm);
				//add self to parent
				matBones[pbm->parent]->child.push_back(ii);
			}
			else {
				pbm->parent=255;
				pbm->bindPosRotation = pbm->jointLocalRotation;
				pbm->bindPosTranslation = pbm->jointLocalTranslation;
				pbm->skinLocalMat = pbm->bindPosRotation.getMatrix();
				pbm->skinLocalMat.setTranslation(pbm->bindPosTranslation);
			}
			matBones.push_back(pbm);
		}
		return true;
	}

	void getbindPosMatrix( int i, SBoneMatrix *&in ) {
		SBoneMatrix *parentJ = matBones[in->parent];
		in->bindPosRotation =  parentJ->bindPosRotation*in->jointLocalRotation;
		//for translation, need to consider parent joint's orientation
		in->bindPosTranslation = parentJ->bindPosTranslation + getRotatedTranslation( parentJ->bindPosRotation, in->jointLocalTranslation);
	}

	core::vector3df getRotatedTranslation( core::quaternion q, core::vector3df v) {
		core::quaternion tq(v.X, v.Y, v.Z, 0);
//		q.normalize();
		core::quaternion qCong = q;
		qCong.makeInverse();
		// q * v * qConjugate
		core::quaternion res = q * tq * qCong;
//		q.normalize();
		core::vector3df ret(res.X, res.Y, res.Z);
		return ret;
	}

	int getMeshBufferIndex( std::string name ) {
		auto it = arrMeshBufferInfo.begin();
		for(; it!=arrMeshBufferInfo.end(); ++it) {
			if( name.compare(it->name)==0 )
				return it->meshBuffer_index;
		}
		return -1;
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
		ai.frameStart=curAnimFrame;
		ai.frameEnd = curAnimFrame + d->frame-1;
		ai.dat2BIndex = dat2Bs.size();
		ai.speed = d->speed;

		arrAnimInfo.push_back(ai);
		dat2Bs.push_back((DAT2BHeader*)pp);

		curAnimFrame += d->frame;
		return true;
	}

	int getMeshBufferCount() {
		return m_meshBuffer.size();
	}
	//bool partsclear(void)
	//{
	//	for( int i=0;i<parts.size(); i++ ){
	//		parts[i].Clear();
	//	}
	//	return true;
	//}
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
//		partsclear();
		dat2Bsclear();
		matBones.clear();
		if( dat29 ) delete []dat29; dat29 = NULL;
		dat29Suu = 0;
		return true;
	}
	//bool AddList(int pn,char *pp,int sz){
	//	return parts[pn].AddList(pp,sz);
	//}
	//bool IsEnable(){
	//	return (!parts[0].dat2a.empty())||(!parts[1].dat2a.empty());
	//}
};
/*------------------------------------------------------------------------------
End Class FFXICharacter
------------------------------------------------------------------------------*/