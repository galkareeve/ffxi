#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>

#include "TDWAnalysis.h"
#include "DDS2Bmp.h"
#include "IMeshBuffer.h"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

#include "FFXI_Character.h"

using namespace glm;

CFFXI_Character::CFFXI_Character(void)
{
	m_useQuaternion=false;
	dat29=NULL;
	curAnimFrame=1;
	m_curAnimeDat2BIndex=0;	//index to dat2Bs

	//create a BindPos Frame
	SAnimInfo ai;
	
	//record animInfo using frameStart/frameEnd
	ai.frameStart=0;
	ai.frameEnd =0;
	ai.dat2BIndex = -1;
	ai.speed = 0;
	arrAnimInfo.push_back(ai);
}


CFFXI_Character::~CFFXI_Character(void)
{
	Clear();
}

std::string CFFXI_Character::extractImageName(char *p, u32 &width, u32 &height, u8 *& ppImage)
{
	char buf[50];
	std::string imgname;
	IMGINFO1 *ii = nullptr;

	ii = (IMGINFO1*)(p);
	memcpy(buf, ii->ddsType, 4);
	buf[4]=0x00;

	if( strcmp(buf, "3TXD")!=0 )
		return nullptr;

	width = ii->imgx;
	height = ii->imgy;
	//size of DDS imageBlock = width/4 * height/4 * 16byte == ii->size
//	ppImage = new u8[ii->size];
//	memcpy(ppImage, p+sizeof(IMGINFO1), ii->size);
	
	//convert to bitmap
	u8 *pDDSBlock = new u8[ii->size];
	memcpy(pDDSBlock, p+sizeof(IMGINFO1), ii->size);
	CDDS2Bmp ddsbmp;
	ddsbmp.convert2BMP(pDDSBlock, width, height, ii->size, ii->widthbyte, ii->ddsType[0], NULL, ppImage);

	memcpy(buf,ii->id,16);
	buf[16]=0x00;
	imgname.assign(buf);
	return imgname;
}

int CFFXI_Character::AddVertex(char *p, int sz) {
	DAT2A tmp; 
		
	memset(&tmp,0,sizeof(tmp));
	char *pp = new char [sz];
	memcpy(pp,p,sz);
	tmp.dat2A = pp;
	DAT2AHeader *pcp=(DAT2AHeader *)tmp.dat2A;
	tmp.dat2ahead = pcp;
	if((pcp->type&0x7f)==1) {
		pcp->type=pcp->type;
	}
	tmp.weight1 =  ((short*)(tmp.dat2A+pcp->offsetWeight*2))[0];
	tmp.weight2 =  ((short*)(tmp.dat2A+pcp->offsetWeight*2))[1];
	tmp.pBone   =  (u16*)(tmp.dat2A+pcp->offsetBone*2);
	tmp.pBoneTbl=  (u16*)(tmp.dat2A+pcp->offsetBoneTbl*2);
	tmp.pVertex   = (MODELVERTEX1*)(tmp.dat2A+pcp->offsetVertex*2);
	tmp.pVertex2  = (MODELVERTEX2*)(((char*)tmp.pVertex)+ tmp.weight1*0x18    );
	tmp.pVertexC  = (CLOTHVERTEX1*)(tmp.dat2A+pcp->offsetVertex*2);
	tmp.pVertexC2 = (CLOTHVERTEX2*)(((char*)tmp.pVertex)+ tmp.weight1*0xC    );
	p += 2 * (*(short*)(p+0x6));		//p+6 is offsetPoly, there move p to start of uvFace ptr
	tmp.pPoly   = (char*)(tmp.dat2A + pcp->offsetPoly*2);

	
	//extract bone tbl
	std::vector<int> vecBoneIndexTbl;
	vecBoneIndexTbl.clear();
	int totalsize = pcp->offsetBoneTbl*2;
	u16 *tblindex;
	for(int k=0; k<pcp->BoneTblSuu; ++k) {
		tblindex = (u16*)(pp+totalsize);
		vecBoneIndexTbl.push_back(*tblindex);
		totalsize +=2;
	}

	//extract bone
	BONE3 *b3 = nullptr;
	totalsize = pcp->offsetWeight*2;
	b3 = (BONE3*)(pp+totalsize);
	int noB1 = (int)b3->low;
	int noB2 = (int)b3->high;
	
	bool isIndirect=(pcp->type&0x80)? true:false;
	BONE4 b4;

	//create FFXIParts
	FFXIParts *pMB = new FFXIParts(false);
	m_parts.push_back(pMB);

	//extract Bone4 and store in meshBuffer
	totalsize = pcp->offsetBone*2;
	for(int k=0; k<noB1; ++k) {
		b3 = (BONE3*)(pp+totalsize);
		if( isIndirect )
			b4.low = vecBoneIndexTbl[b3->bitLow.left];
		else
			b4.low = b3->bitLow.left;
		b4.lflg = b3->bitLow.flg;
		b4.hflg=b4.high=0;
		pMB->addBone4(b4);
		totalsize +=4;
	}
	for(int k=0; k<noB2; ++k) {
		b3 = (BONE3*)(pp+totalsize);
		if( isIndirect ) {
			b4.low = vecBoneIndexTbl[b3->bitLow.left];
			b4.high = vecBoneIndexTbl[b3->bitHigh.left];
		}
		else {
			b4.low = b3->bitLow.left;
			b4.high = b3->bitHigh.left;
		}
		b4.lflg = b3->bitLow.flg;
		b4.hflg = b3->bitHigh.flg;
		pMB->addBone4(b4);
		totalsize +=4;
	}
	//extract vertex/normal
	buildBindPosVertex(&tmp, pMB, false);
	//extract face (indices/uv)
	buildBindPosPartFace(&tmp, pMB, false);

	if( pcp->flip ) {
		FFXIParts *pMB = new FFXIParts(true);
		m_parts.push_back(pMB);

		//extract Bone4 and store in meshBuffer
		totalsize = pcp->offsetBone*2;
		for(int k=0; k<noB1; ++k) {
			b3 = (BONE3*)(pp+totalsize);
			if( isIndirect )
				b4.low = vecBoneIndexTbl[b3->bitLow.right];
			else
				b4.low = b3->bitLow.right;
			b4.lflg = b3->bitLow.flg;
			b4.hflg=b4.high=0;
			pMB->addBone4(b4);
			totalsize +=4;
		}
		for(int k=0; k<noB2; ++k) {
			b3 = (BONE3*)(pp+totalsize);
			if( isIndirect ) {
				b4.low = vecBoneIndexTbl[b3->bitLow.right];
				b4.high = vecBoneIndexTbl[b3->bitHigh.right];
			}
			else {
				b4.low = b3->bitLow.right;
				b4.high = b3->bitHigh.right;
			}
			b4.lflg = b3->bitLow.flg;
			b4.hflg = b3->bitHigh.flg;
			pMB->addBone4(b4);
			totalsize +=4;
		}

		buildBindPosVertex(&tmp, pMB, true);
		buildBindPosPartFace(&tmp, pMB, true);
	}

	delete []pp;
	return tmp.weight1+tmp.weight2;
}

void CFFXI_Character::buildBindPosVertex(DAT2A *dat, FFXIParts *pMB, bool needflip)
{
	//get its meshBuffer index to parts no
	float minF=0.0001f;
	DAT2AHeader *pcp=nullptr;
	pcp=(DAT2AHeader *)dat->dat2ahead;

	char *p = dat->dat2A;
	SBoneMatrix bm;

	SVertexNormal *vn1;
	SVertexNormal2 *vn2;
	SCloth	*cv1;
	SCloth2	*cv2;

	vec4 dummy(0,0,0,0);
	vec4 v4,h4;
	SBindPosVertex bpv;

	for(int i=0; i<dat->weight1; ++i) {
		if(pcp->PolyLod2Suu==0) {
			vn1 = (SVertexNormal*)(&dat->pVertex[i]);			
			v4[0] = vn1->vertex.x;
			v4[1] = vn1->vertex.y;
			v4[2] = vn1->vertex.z;
			v4[3] = 0;
			bpv.x1 = v4;
			h4[0] = vn1->normal.x;
			h4[1] = vn1->normal.y;
			h4[2] = vn1->normal.z;
			h4[3] = 0;
			bpv.h1 = h4;			
		}
		else {
			cv1 = (SCloth*)(&dat->pVertexC[i]);
			v4[0] = cv1->vertex.x;
			v4[1] = cv1->vertex.y;
			v4[2] = cv1->vertex.z;
			v4[3] = 0;
			bpv.x1 = v4;
			bpv.h1 = dummy;
		}
		pMB->addBindPosVertex(bpv);
	}

	for(int i=0; i<dat->weight2; ++i) {
		if(pcp->PolyLod2Suu==0) {
			vn2 = (SVertexNormal2*)(&dat->pVertex2[i]);
			v4[0] = vn2->x[0];
			v4[1] = vn2->y[0];
			v4[2] = vn2->z[0];
			v4[3] = vn2->w[0];
			bpv.x1 = v4;

			h4[0] = vn2->hx[0];
			h4[1] = vn2->hy[0];
			h4[2] = vn2->hz[0];
			h4[3] = 0;
			bpv.h1 = h4;

			v4[0] = vn2->x[1];
			v4[1] = vn2->y[1];
			v4[2] = vn2->z[1];
			v4[3] = vn2->w[1];
			bpv.x2 = v4;
			h4[0] = vn2->hx[1];
			h4[1] = vn2->hy[1];
			h4[2] = vn2->hz[1];
			h4[3] = 0;
			bpv.h2 = h4;
		}
		else {
			cv2 = (SCloth2*)(&dat->pVertexC2[i]);
			v4[0] = cv2->x[0];
			v4[1] = cv2->y[0];
			v4[2] = cv2->z[0];
			v4[3] = cv2->w[0];
			bpv.x1 = v4;
			bpv.h1 = dummy;

			v4[0] = cv2->x[1];
			v4[1] = cv2->y[1];
			v4[2] = cv2->z[1];
			v4[3] = cv2->w[1];
			bpv.x2 = v4;
			bpv.h2 = dummy;
		}
		pMB->addBindPosVertex(bpv);
	}
}

void CFFXI_Character::buildBindPosPartFace(DAT2A *dat, FFXIParts *pMB, bool needflip)
{
	int wf, ws=0, totalIndices=0, tws=0;
	char buf[20];
	std::string str;
	SFaceMB *faceMB=nullptr;
	SFace3 *psf3;
	SFace *psf;
	SFace sf;
	std::string textureName;

	char *p = dat->pPoly;
	while(1) {
		wf = (int)*(u16*)(p );
		ws = (int)*(u16*)(p+2);
		if( 0x8010 == (wf&0x80F0) ) { 
			p+=0x2e;		//46 byte
			continue;
		} 
		else if ( 0x8000 == (wf&0x80F0) ) {
			memcpy(buf,p+2,16);
			buf[16]=0x00;
			textureName.assign(buf);
			//every Triangle (list/strip) can point to different texture
//			pMB->setTextureName(str);
			p+=0x12;	//18 byte
		} 
		else {
			if( 0x5453 == wf ) {/*ST*/  //StripTriangle
				p+=4;
				psf3 = (SFace3*)p;
				faceMB = new SFaceMB;
				faceMB->type = E_TRIANGLE_STRIP;
				faceMB->m_textureName = textureName;

				//there r 3 index for first record
				if(!needflip) {
					//add a dummy 0 to reflect
					sf.indices = psf3->indices[0];
					sf.uv = psf3->uv1;
					faceMB->vecface.push_back(sf);

					sf.indices = psf3->indices[0];
					sf.uv = psf3->uv1;
					faceMB->vecface.push_back(sf);

					sf.indices = psf3->indices[1];
					sf.uv = psf3->uv2;
					faceMB->vecface.push_back(sf);

					sf.indices = psf3->indices[2];
					sf.uv = psf3->uv3;
					faceMB->vecface.push_back(sf);
				}
				else {
					sf.indices = psf3->indices[0];
					sf.uv = psf3->uv1;
					faceMB->vecface.push_back(sf);

					sf.indices = psf3->indices[1];
					sf.uv = psf3->uv2;
					faceMB->vecface.push_back(sf);

					sf.indices = psf3->indices[2];
					sf.uv = psf3->uv3;
					faceMB->vecface.push_back(sf);
				}
				p+=30;

				//subsequence add 1 vertex
				for(int k=0; k<ws-1; ++k) {
					psf = (SFace*)p;
					faceMB->vecface.push_back(*psf);					
					p += 10;
				}

				pMB->addFace(faceMB);
				continue;
			} 
			else if( 0x4353 == wf ) { /*SC*/
				p+=ws*20  + 0x0C;
				continue; 
			} 
			else if( 0x0043 == wf ) { /*C*/
				p+=ws*10 + 0x4;
				continue; 
			} 
			else if( 0x0054 == wf ) {  /*T*/ //TriangleList
				p+=4;
				faceMB = new SFaceMB;
				faceMB->type = E_TRIANGLE_LIST;
				faceMB->m_textureName = textureName;

				for( int k=0; k<ws; k++ ) {
					psf3 = (SFace3*)p;
					if(!needflip) {
						sf.indices = psf3->indices[2];
						sf.uv = psf3->uv3;
						faceMB->vecface.push_back(sf);

						sf.indices = psf3->indices[1];
						sf.uv = psf3->uv2;
						faceMB->vecface.push_back(sf);

						sf.indices = psf3->indices[0];
						sf.uv = psf3->uv1;
						faceMB->vecface.push_back(sf);
					}
					else {
						sf.indices = psf3->indices[0];
						sf.uv = psf3->uv1;
						faceMB->vecface.push_back(sf);

						sf.indices = psf3->indices[1];
						sf.uv = psf3->uv2;
						faceMB->vecface.push_back(sf);

						sf.indices = psf3->indices[2];
						sf.uv = psf3->uv3;
						faceMB->vecface.push_back(sf);
					}
					p+=30;				//sizeof uvface
				}
				pMB->addFace(faceMB);
				continue;
			} 
			else
				break;
		}
	}
}

void CFFXI_Character::nextAnimation(int &frameStart, int &frameEnd, float &speed)
{
	m_curAnimeDat2BIndex++;
	if( m_curAnimeDat2BIndex>=arrAnimInfo.size() )
		m_curAnimeDat2BIndex=0;

	setCurrentAnimation(m_curAnimeDat2BIndex, frameStart, frameEnd, speed);
}

void CFFXI_Character::setCurrentAnimation(int i, int &frameStart, int &frameEnd, f32 &speed)
{ 
	m_curAnimeDat2BIndex=i;

	SAnimInfo info = arrAnimInfo[m_curAnimeDat2BIndex];
	frameStart = info.frameStart;
	frameEnd = info.frameEnd;
	speed = (30*info.speed);
}

int CFFXI_Character::getAnimIndex(int frameStart, int frameEnd)
{
	for(auto it=arrAnimInfo.begin(); it!=arrAnimInfo.end(); ++it) {
		if( frameStart>=it->frameStart && frameEnd<=it->frameEnd)
			return it->dat2BIndex;
	}
	return -1;
}

int CFFXI_Character::getJointAnimation(int no, DAT2BHeader *p2b, int frame)
{
	if(frame<0 ) 
		frame=0;

	if(p2b && p2b->element>no ) {
		DAT2B &dat = p2b->dat[no];
		float *f = p2b->f;
		f32quat qt,qt2;
		f32vec3 tr,sc;

		SBoneMatrix *pbm = matBones[dat.no];
		if((dat.idx_qtx|dat.idx_qty|dat.idx_qtz|dat.idx_qtw)&0x80000000) {
			return -1;
		}
		if(dat.idx_qtx){ qt.x = f[dat.idx_qtx+frame]; } else{ qt.x = dat.qtx; }
		if(dat.idx_qty){ qt.y = f[dat.idx_qty+frame]; } else{ qt.y = dat.qty; }
		if(dat.idx_qtz){ qt.z = f[dat.idx_qtz+frame]; } else{ qt.z = dat.qtz; }
		if(dat.idx_qtw){ qt.w = f[dat.idx_qtw+frame]; } else{ qt.w = dat.qtw; }
		pbm->animationRotation = qt;

		if(!dat.idx_tx) tr.x = dat.tx; else tr.x = f[dat.idx_tx+frame];
		if(!dat.idx_ty) tr.y = dat.ty; else tr.y = f[dat.idx_ty+frame];
		if(!dat.idx_tz) tr.z = dat.tz; else tr.z = f[dat.idx_tz+frame];
		pbm->animationTranslation = tr;
		return dat.no;
	}
	return -1;
}

int CFFXI_Character::skinJoint(int frame)
{
	int i;
	int fr=0;
	fmat4x4 Wanim, Binv;
	SBoneMatrix *pbm=nullptr, *parentJ=nullptr;

	SAnimInfo info = arrAnimInfo[m_curAnimeDat2BIndex];
	if( info.dat2BIndex>=0 ) {
		//all animation frame are concatenated, therefore need to subtract frameStart
		//because the actual animation frame start from 0
		frame -= info.frameStart;
		DAT2BHeader *p2b = dat2Bs[info.dat2BIndex];
		if(p2b) {
			for(i=0; i<p2b->element; i++) {
				int bno = getJointAnimation(i,p2b,frame);
				if( bno>=0 ) {
					pbm = matBones[bno];
					if( bno==0 ) {
						pbm->skinLocalRotation = pbm->animationRotation * pbm->jointLocalRotation;
						pbm->skinLocalTranslation = pbm->jointLocalTranslation + pbm->animationTranslation;
					}
					else {
						pbm->skinLocalRotation = pbm->animationRotation * pbm->jointLocalRotation;
						pbm->skinLocalTranslation = pbm->jointLocalTranslation + getRotatedTranslation( pbm->jointLocalRotation, pbm->animationTranslation);
					}
//					pbm->skinLocalRotation.normalize();
				}
			}
		}
	}
	
	//create the skinGlobalMat
	for(i=0; i<dat29Suu; ++i) {
		pbm = matBones[i];
		if( i==0 ) {
			//create matrix
			pbm->skinLocalMat = glm::mat4_cast(pbm->jointLocalRotation);
			pbm->skinLocalMat[3] = glm::vec4(pbm->jointLocalTranslation,1);
		}
		else {
			parentJ = matBones[pbm->parent];			
			//add to parent
			pbm->skinLocalRotation = parentJ->skinLocalRotation * pbm->skinLocalRotation;
			pbm->skinLocalTranslation = parentJ->skinLocalTranslation + getRotatedTranslation( parentJ->skinLocalRotation, pbm->skinLocalTranslation);
			//create matrix
			pbm->skinLocalMat = glm::mat4_cast(pbm->skinLocalRotation);
			pbm->skinLocalMat[3] = glm::vec4(pbm->skinLocalTranslation,1);
		}
	}
	return fr;
}

//animate the boneMatrix
bool CFFXI_Character::animate(int frame)
{
	//Binv * B = Identity ==> Vnew = Vold	
	resetBoneMatrix();
	//animate the bone to set its matrix, will be used when calling GetVertex
	skinJoint(frame);

	//for all parts, skin mesh
	auto it =m_parts.begin();
	for(; it!=m_parts.end(); ++it) {
		skinMesh(*it);
	}
	return true;
}


//invoke after boneMatrix have been animated
void CFFXI_Character::updateMeshBuffer(unsigned int mIndex, unsigned int frame, unsigned int pn, IMeshBuffer *inout)
{
	//need a mapping from meshBuffer index to partsNo and partFace
	//no of meshBuffer = partsNo * partFace
	std::vector<f32vec3> vecVertices;
	std::vector<f32vec3> vecNormal;

	FFXIParts *part = m_parts[pn];
	unsigned int faceIndex = mIndex - part->getMeshBufferStartIndex();
	int maxFace = part->getFaceCount();
	if(faceIndex >= maxFace)
		return;
	//retrieve the meshBuffer for this face [TRIANGLE_LIST/TRIANGLE_STRIP]
	SFaceMB *pfmb = part->getFace(faceIndex);
	for(auto it=pfmb->vecface.begin(); it!=pfmb->vecface.end(); ++it) {
		vecVertices.push_back(part->getVertex((*it).indices));
		vecNormal.push_back(part->getNormal((*it).indices));
	}
	inout->updateVertexBuffer(frame, vecVertices);
	inout->updateNormalBuffer(frame, vecNormal);
	
}

void CFFXI_Character::skinMesh(FFXIParts *pMB)
{
	SBoneMatrix *pbm=nullptr;
	BONE4 b4;
	fmat4x4 B;
	vec3 v1, v2, h1, h2, tr, dummy(0,0,0);
	SBindPosVertex bpv;
	vec3 v3;
	//clear existing vertices/normal
	pMB->clearVertices();

	int size = pMB->getBindPosVertexCount();
	for(int i=0; i<size; ++i) {
		bpv = pMB->getBindPosVertex(i);
		
		if( bpv.x2.x==0 ) {
			v1.x = bpv.x1.x; v1.y = bpv.x1.y; v1.z = bpv.x1.z;
			h1.x = bpv.h1.x; h1.y = bpv.h1.y; h1.z = bpv.h1.z;

			b4 = pMB->getBone4(i);
			pbm = matBones[b4.low];
			B = pbm->skinLocalMat;

			if(pMB->needFlip()) { 
				if(b4.lflg==1) v1.x = -v1.x;
				if(b4.lflg==2) v1.y = -v1.y;
				if(b4.lflg==3) v1.z = -v1.z;
			}
			if(!m_useQuaternion) {
				//matrix multiply
				v1 = vec3(B*vec4(v1,1));

				//normal does not need translation
				B[3] = glm::vec4(dummy,0);
				h1 = vec3(B*vec4(h1,0));
			}
			else {
				v1 = getRotatedTranslation(pbm->skinLocalRotation, v1);
				v1 += pbm->skinLocalTranslation;

				//normal does not need translation
				h1 = getRotatedTranslation(pbm->skinLocalRotation, h1);
			}
			glm::normalize(h1);
			//vertex
			v3.x = v1.x;
			v3.y = v1.y;
			v3.z = v1.z;
			pMB->addVertex(v3);
			
			//normal
			v3.x = h1.x;
			v3.y = h1.y;
			v3.z = h1.z;
			pMB->addNormal(v3);
		}
		else {
			v1.x = bpv.x1.x; v1.y = bpv.x1.y; v1.z = bpv.x1.z;
			v2.x = bpv.x2.x; v2.y = bpv.x2.y; v2.z = bpv.x2.z;
			h1.x = bpv.h1.x; h1.y = bpv.h1.y; h1.z = bpv.h1.z;
			h2.x = bpv.h2.x; h2.y = bpv.h2.y; h2.z = bpv.h2.z;

			b4 = pMB->getBone4(i);
			pbm = matBones[b4.low];
			B = pbm->skinLocalMat;
			if(pMB->needFlip()) { 
				if(b4.lflg==1) v1.x = -v1.x;
				if(b4.lflg==2) v1.y = -v1.y;
				if(b4.lflg==3) v1.z = -v1.z;
			}
			if(!m_useQuaternion) {
				tr = vec3(glm::column(B,3));
				tr *= bpv.x1.w;		//modulate by w1
				B[3] = glm::vec4(tr,1);
				v1 = vec3(B*vec4(v1,1));

				B[3] = glm::vec4(dummy,0);
				h1 = vec3(B*vec4(h1,0));
				h1 *= bpv.x1.w;
			}
			else {
				tr = pbm->skinLocalTranslation*bpv.x1.w;
				v1 = getRotatedTranslation(pbm->skinLocalRotation, v1);
				v1 += tr;

				h1 = getRotatedTranslation(pbm->skinLocalRotation,h1);
				h1 *= bpv.x1.w;
			}
			glm::normalize(h1);

			pbm = matBones[b4.high];
			B = pbm->skinLocalMat;
			if(pMB->needFlip()) { 
				if(b4.hflg==1) v2.x = -v2.x;
				if(b4.hflg==2) v2.y = -v2.y;
				if(b4.hflg==3) v2.z = -v2.z;
			}
			if(!m_useQuaternion) {
				tr = vec3(glm::column(B,3));
				tr *= bpv.x2.w;		//modulate by w2
				B[3] = glm::vec4(tr,1);
				v2 = vec3(B*vec4(v2,1));

				B[3] = glm::vec4(dummy,0);
				h2 = vec3(B*vec4(h2,0));
				h2 *= bpv.x2.w;
			}
			else {
				tr = pbm->skinLocalTranslation*bpv.x2.w;
				v2 = getRotatedTranslation(pbm->skinLocalRotation,v2);
				v2 +=tr;

				h2 = getRotatedTranslation(pbm->skinLocalRotation, h2);
				h2 *= bpv.x2.w;
			}
			glm::normalize(h2);

			//combine normal
			h1.x = h1.x + h2.x;
			h1.y = h1.y + h2.y;
			h1.z = h1.z + h2.z;

			glm::normalize(h1);
			//vertex
			v3.x = v1.x + v2.x;
			v3.y = v1.y + v2.y;
			v3.z = v1.z + v2.z;
			pMB->addVertex(v3);
			//normal
			v3.x = h1.x;
			v3.y = h1.y;
			v3.z = h1.z;
			pMB->addNormal(v3);
		}
	}
}