//------------------------------------------------------------------------------
//  File: TDWCharacter.cpp
//  Desc: ffxi的角色单元     
//  Revision history:
//      * 2007/08/10 Created by Muzisoft05@163.com.
//
//  Todo:
//
//------------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <irrArray.h>

#include "TDWAnalysis.h"
#include "TDWCharacter.h"

FFXICharacter::FFXICharacter()
{
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

FFXICharacter::~FFXICharacter(void)
{
	Clear();
}

////从dat2a中得到第no个骨骼(Memo中指出bone混合在dat2a中)
//int FFXICharacter::GetBoneNo(DAT2A *d2a, u16 no, bool flgFlip, int &flg, int c)
//{
//	int ret;
//	int tblidx;                        
//	u16 bone;                        
//
//	if( c==0 ) bone = d2a->pBone[no*2  ];
//	else       bone = d2a->pBone[no*2+1];
//
//	tblidx=(bone)&0x7f;
//	flg = (bone>>14)&0x3;
//	if(flgFlip) bone>>=7;
//	tblidx = bone&0x7f;
//
//	if( flg==0 ){
//		flg=0;
//	}
//
//	if(d2a->dat2ahead->type&0x80) ret = d2a->pBoneTbl[tblidx];
//	else                          ret = tblidx;
//
//	if(dat29Suu && ret>=dat29Suu){
//		//ret%=dat29Suu;
//	}
//	return ret;
//}


//求得Bone间的Matrixs
//int FFXICharacter::GetBoneMatrix(core::matrix4 *matrix, u16 bno)
//{
//	*matrix = mat[bno];
//	return  0;
//}

//求得最终各顶点坐标
//void FFXICharacter::GetVertex(DAT2A *d2a, u32 i, D3DTEXVERTEX *ppp, bool flip)
//{
//  int bno1,bno2;
//  int flg;
//  f32 arr[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//  core::matrix4 p1,h1,p2,h2;
//  //p1.setM(&arr[0]);
//  //h1.setM(&arr[0]);
//  //p2.setM(&arr[0]);
//  //h2.setM(&arr[0]);
//  p1.makeIdentity();
//  h1.makeIdentity();
//  p2.makeIdentity();
//  h2.makeIdentity();
//
//  core::matrix4 m1,m2;
//  core::matrix4 mp1,mp2;
//  core::matrix4 ms1,ms2;
//  ppp->color = 0xffffffff;
//  if(i<d2a->weight1){
//    bno1 = GetBoneNo1(d2a,i,flip,flg);
//    GetBoneMatrix(&m1,bno1);
//    //if(flip){ 
//    //  if(flg==3)  m1 = matrixMirrorZ * m1;
//    //  if(flg==2)  m1 = matrixMirrorY * m1;
//    //  if(flg==1)  m1 = matrixMirrorX * m1;
//    //}
//    if((d2a->dat2ahead->type&0x7f)==0){
//      p1[0] = d2a->pVertex[i].x;
//      p1[1] = d2a->pVertex[i].y;
//      p1[2] = d2a->pVertex[i].z;
//      p1[3] = 1.0f;
//      h1[0] = d2a->pVertex[i].hx;
//      h1[1] = d2a->pVertex[i].hy;
//      h1[2] = d2a->pVertex[i].hz;
//      h1[3] = 0.0f;
//    }else{
//      p1[0] = d2a->pVertexC[i].x;
//      p1[1] = d2a->pVertexC[i].y;
//      p1[2] = d2a->pVertexC[i].z;
//      p1[3] = 1.0f;
//    }
////    D3DXVec4Transform(&p1,&p1,&m1);  D3DXVec4Transform(&h1,&h1,&m1);
//	p1 = p1*m1;
//	h1 = h1*m1;
//    ppp->x= p1[0]; ppp->y= p1[1]; ppp->z =p1[2];
//    ppp->hx=h1[0]; ppp->hy=h1[1]; ppp->hz=h1[2];
//  }else{
//    bno1 = GetBoneNo1(d2a,i,flip,flg);
//    GetBoneMatrix(&m1,bno1);
//    //if(flip){ 
//    //  if(flg==3) m1 = matrixMirrorZ * m1;
//    //  if(flg==2) m1 = matrixMirrorY * m1;
//    //  if(flg==1) m1 = matrixMirrorX * m1;
//    //}
//    bno2 = GetBoneNo2(d2a,i,flip,flg);
//    GetBoneMatrix(&m2,bno2);
//    //if(flip){ 
//    //  if(flg==3) m2 = matrixMirrorZ * m2;
//    //  if(flg==2) m2 = matrixMirrorY * m2;
//    //  if(flg==1) m2 = matrixMirrorX * m2;
//    //}
//
//    i -= d2a->weight1;
//    if((d2a->dat2ahead->type&0x7f)==0){
//      p1[0] = d2a->pVertex2[i].x1;    p2[0] = d2a->pVertex2[i].x2;  
//      p1[1] = d2a->pVertex2[i].y1;    p2[1] = d2a->pVertex2[i].y2;  
//      p1[2] = d2a->pVertex2[i].z1;    p2[2] = d2a->pVertex2[i].z2;  
//      p1[3] = d2a->pVertex2[i].w1;    p2[3] = d2a->pVertex2[i].w2;  
//      h1[0] = d2a->pVertex2[i].hx1;   h2[0] = d2a->pVertex2[i].hx2; 
//      h1[1] = d2a->pVertex2[i].hy1;   h2[1] = d2a->pVertex2[i].hy2; 
//      h1[2] = d2a->pVertex2[i].hz1;   h2[2] = d2a->pVertex2[i].hz2; 
//      h1[3] = 0.0f;                   h2[3] = 0.0f;  
//    }else{
//      p1[0] = d2a->pVertexC2[i].x1;    p2[0] = d2a->pVertexC2[i].x2;  
//      p1[1] = d2a->pVertexC2[i].y1;    p2[1] = d2a->pVertexC2[i].y2;  
//      p1[2] = d2a->pVertexC2[i].z1;    p2[2] = d2a->pVertexC2[i].z2;  
//      p1[3] = d2a->pVertexC2[i].w1;    p2[3] = d2a->pVertexC2[i].w2;  
//    }
// 
//	p1 = p1*m1;
//	p2 = p2*m2;
//	h1 = h1*m1;
//	h2 = h2*m2;
// 
//    h1=h1*d2a->pVertex2[i].w1;  h2=h2*d2a->pVertex2[i].w2; 
//    ppp->x  = p1[0]+p2[0]; ppp->y =p1[1]+p2[1]; ppp->z =p1[2]+p2[2];
//    ppp->hx = h1[0]+h2[0]; ppp->hy=h1[1]+h2[1]; ppp->hz=h1[2]+h2[2];
//  }
//}

std::string FFXICharacter::extractTextureName(char *p, core::array<u16> &arrIndices)
{
	DAT2AHeader *pcp=nullptr;
	pcp=(DAT2AHeader *)p;

	char buf[500];
	std::string str;
	str.clear();
	TEXLIST *uvf;
	TEXLIST2 *uvf2;
	//incr to first block of uvFace data
	p += pcp->offsetPoly*2;
	while(1) {
		int wf = (int)*(u16*)(p );
		int ws = (int)*(u16*)(p+2);
        if( 0x8010 == (wf&0x80F0) ) { 
            p+=0x2e;		//46 byte
            continue;  //晄柧
		} 
		else if ( 0x8000 == (wf&0x80F0) ) { 
			memcpy(buf,p+2,16);
			buf[16]=0x00;
			str.assign(buf);
//			return str;

			p+=0x12;	//18 byte
		} 
		else {
			if( 0x5453 == wf ) {/*ST*/  //StripTriangle
				p+=4;
				uvf = (TEXLIST*)p;
				arrIndices.push_back(uvf->i1);
				arrIndices.push_back(uvf->i2);
				arrIndices.push_back(uvf->i3);
				p+=30;
				//subsequence add 1 vertex
				for(int k=0; k<ws-1; ++k) {
					uvf2 = (TEXLIST2*)p;
					arrIndices.push_back(uvf2->i);
					p += 10;
				}
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
				for( int k=0; k<ws; k++ ) {
					uvf = (TEXLIST*)p;
					arrIndices.push_back(uvf->i1);
					arrIndices.push_back(uvf->i2);
					arrIndices.push_back(uvf->i3);
					p+=30;				//sizeof uvface
				}					
				continue;
			} 
			else
				break;
		}
	}
	return str;
}

std::string FFXICharacter::extractImageName(char *p, u32 &width, u32 &height, u8 *& ppImage)
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
	ppImage = new u8[ii->size];
	memcpy(ppImage, p+sizeof(IMGINFO1), ii->size);

	//convert texture to char*
//	GetBMPImage(ii, ppImage);
	memcpy(buf,ii->id,16);
	buf[16]=0x00;
	imgname.assign(buf);
	return imgname;
}

//求得Frame间的Matrixs
int FFXICharacter::GetMotionMatrix(core::matrix4 *matrix, s32 no, DAT2BHeader *p2b, u32 frame )
{
	if(frame<0 ) 
		frame=0;

	if(p2b && p2b->element>no ) {
		float n = (float)frame/p2b->frame;
		if(n>1.0f) n=1.0f;

		if(p2b->frame<=1) frame = 0;
		else              frame %= (p2b->frame-1);

		DAT2B &dat = p2b->dat[no];
		float *f = p2b->f;
		core::quaternion qt,qt2;
		core::vector3df tr;
		core::vector3df sc;
		core::matrix4 m1, m2, m3;
		m2.makeIdentity();
		m3.makeIdentity();

		if((dat.idx_qtx|dat.idx_qty|dat.idx_qtz|dat.idx_qtw)&0x80000000) {
			matrix->makeIdentity();
			return dat.no;
		}
		if(dat.idx_qtx){ qt.X = f[dat.idx_qtx+frame];  qt2.X = f[dat.idx_qtx+frame+1]; } else{ qt.X = dat.qtx; qt2.X = dat.qtx; }
		if(dat.idx_qty){ qt.Y = f[dat.idx_qty+frame];  qt2.Y = f[dat.idx_qty+frame+1]; } else{ qt.Y = dat.qty; qt2.Y = dat.qty; }
		if(dat.idx_qtz){ qt.Z = f[dat.idx_qtz+frame];  qt2.Z = f[dat.idx_qtz+frame+1]; } else{ qt.Z = dat.qtz; qt2.Z = dat.qtz; }
		if(dat.idx_qtw){ qt.W = f[dat.idx_qtw+frame];  qt2.W = f[dat.idx_qtw+frame+1]; } else{ qt.W = dat.qtw; qt2.W = dat.qtw; }
		qt = qt.slerp(qt,qt2,n);
		qt.getMatrix(m1);

		if(!dat.idx_tx) tr.X = dat.tx; else tr.X = f[dat.idx_tx+frame]*(1.0f-n) + f[dat.idx_tx+frame+1]*n;
		if(!dat.idx_ty) tr.Y = dat.ty; else tr.Y = f[dat.idx_ty+frame]*(1.0f-n) + f[dat.idx_ty+frame+1]*n;
		if(!dat.idx_tz) tr.Z = dat.tz; else tr.Z = f[dat.idx_tz+frame]*(1.0f-n) + f[dat.idx_tz+frame+1]*n;

	/*	if(!dat.idx_tx) tr.X = dat.tx; else tr.X = f[dat.idx_tx+frame];
		if(!dat.idx_ty) tr.Y = dat.ty; else tr.Y = f[dat.idx_ty+frame];
		if(!dat.idx_tz) tr.Z = dat.tz; else tr.Z = f[dat.idx_tz+frame];*/
//		m2.setTranslation(core::vector3df(tr.X, tr.Y, tr.Z));

		m1.setTranslation(core::vector3df(tr.X, tr.Y, tr.Z));
		*matrix = m1;

		//if(!dat.idx_sx) sc.X = dat.sx; else sc.X = f[dat.idx_sx+frame]*(1.0f-n) + f[dat.idx_sx+frame+1]*n;
		//if(!dat.idx_sy) sc.Y = dat.sy; else sc.Y = f[dat.idx_sy+frame]*(1.0f-n) + f[dat.idx_sy+frame+1]*n;
		//if(!dat.idx_sz) sc.Z = dat.sz; else sc.Z = f[dat.idx_sz+frame]*(1.0f-n) + f[dat.idx_sz+frame+1]*n;
		//m3.setScale(core::vector3df(sc.X, sc.Y, sc.Z));

		//if( dat.idx_sx || dat.idx_sy || dat.idx_sz ) {
		//	*matrix = m3 * m2 * m1;			//should be rotation first then translate, scale for (column major)==> S * T * R, but....irr::matrix is reversed...
		//}
		//else {
		//	*matrix = m2 * m1;
		//}
		return dat.no;
	}
	matrix->makeIdentity();
	return -1;
}

int FFXICharacter::getJointAnimation(s32 no, DAT2BHeader *p2b, s32 frame)
{
	if(frame<0 ) 
		frame=0;

	if(p2b && p2b->element>no ) {
		DAT2B &dat = p2b->dat[no];
		float *f = p2b->f;
		core::quaternion qt,qt2;
		core::vector3df tr;
		core::vector3df sc;

		SBoneMatrix *pbm = matBones[dat.no];
		if((dat.idx_qtx|dat.idx_qty|dat.idx_qtz|dat.idx_qtw)&0x80000000) {
			return -1;
		}
		if(dat.idx_qtx){ qt.X = f[dat.idx_qtx+frame]; } else{ qt.X = dat.qtx; }
		if(dat.idx_qty){ qt.Y = f[dat.idx_qty+frame]; } else{ qt.Y = dat.qty; }
		if(dat.idx_qtz){ qt.Z = f[dat.idx_qtz+frame]; } else{ qt.Z = dat.qtz; }
		if(dat.idx_qtw){ qt.W = f[dat.idx_qtw+frame]; } else{ qt.W = dat.qtw; }
		pbm->animationRotation = qt;

		if(!dat.idx_tx) tr.X = dat.tx; else tr.X = f[dat.idx_tx+frame];
		if(!dat.idx_ty) tr.Y = dat.ty; else tr.Y = f[dat.idx_ty+frame];
		if(!dat.idx_tz) tr.Z = dat.tz; else tr.Z = f[dat.idx_tz+frame];
		pbm->animationTranslation = tr;

		//if(!dat.idx_sx) sc.X = dat.sx; else sc.X = f[dat.idx_sx+frame]*(1.0f-n) + f[dat.idx_sx+frame+1]*n;
		//if(!dat.idx_sy) sc.Y = dat.sy; else sc.Y = f[dat.idx_sy+frame]*(1.0f-n) + f[dat.idx_sy+frame+1]*n;
		//if(!dat.idx_sz) sc.Z = dat.sz; else sc.Z = f[dat.idx_sz+frame]*(1.0f-n) + f[dat.idx_sz+frame+1]*n;
		//m3.setScale(core::vector3df(sc.X, sc.Y, sc.Z));
		return dat.no;
	}
	return -1;
}

int FFXICharacter::skinJoint(s32 frame)
{
	int i;
	int fr=0;
	core::matrix4 Wanim, Binv;
	SBoneMatrix *pbm=nullptr, *parentJ=nullptr;

	SAnimInfo info = arrAnimInfo[m_curAnimeDat2BIndex];
	if( info.dat2BIndex>=0 ) {
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
						parentJ = matBones[pbm->parent];
						pbm->skinLocalRotation = pbm->animationRotation * pbm->jointLocalRotation;
						pbm->skinLocalTranslation = pbm->jointLocalTranslation + getRotatedTranslation( pbm->jointLocalRotation, pbm->animationTranslation);
					}
				}
			}
		}
	}
	
	//create the skinGlobalMat
	for(i=0; i<dat29Suu; ++i) {
		pbm = matBones[i];
		if( i==0 ) {
			//create matrix
			pbm->skinLocalMat = pbm->jointLocalRotation.getMatrix();
			pbm->skinLocalMat.setTranslation(pbm->jointLocalTranslation);
		}
		else {
			parentJ = matBones[pbm->parent];			
			//add to parent
			pbm->skinLocalRotation = parentJ->skinLocalRotation * pbm->skinLocalRotation;
			pbm->skinLocalTranslation = parentJ->skinLocalTranslation + getRotatedTranslation( parentJ->skinLocalRotation, pbm->skinLocalTranslation);
			//create matrix
			pbm->skinLocalMat = pbm->skinLocalRotation.getMatrix();
			pbm->skinLocalMat.setTranslation(pbm->skinLocalTranslation);
		}
	}

	return fr;
}

//animate the boneMatrix
bool FFXICharacter::animate(s32 frame)
{
	//Binv * B = Identity ==> Vnew = Vold	
	resetBoneMatrix();
	//animate the bone to set its matrix, will be used when calling GetVertex
	skinJoint(frame);

	return true;
}


//invoke after boneMatrix have been animated
int FFXICharacter::getMesh(u32 pn, std::vector<D3DTEXVERTEX> &outVertex)
{
	if(pn>=m_meshBuffer.size()) return 0;

	FFXIMeshBuffer *pMB = m_meshBuffer[pn];
	skinMesh(pn, outVertex);
	
	return outVertex.size();
}

int FFXICharacter::AddVertex(char *p, int sz) {
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

	//create meshBuffer
	FFXIMeshBuffer *pMB = new FFXIMeshBuffer(false);
	m_meshBuffer.push_back(pMB);

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
	buildBindPosVertex(&tmp, pMB, 0);

	if( pcp->flip ) {
		FFXIMeshBuffer *pMB = new FFXIMeshBuffer(true);
		m_meshBuffer.push_back(pMB);

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

		buildBindPosVertex(&tmp, pMB, 1);
	}

	delete []pp;
	return tmp.weight1+tmp.weight2;
}

void FFXICharacter::buildBindPosVertex(DAT2A *dat, FFXIMeshBuffer *pMB, bool needflip)
{
	//get its meshBuffer index to parts no
	float minF=0.0001f;
	DAT2AHeader *pcp=nullptr;
	pcp=(DAT2AHeader *)dat->dat2ahead;

	char *p = dat->dat2A;
	MODELVERTEX1 *mv1;
	MODELVERTEX2 *mv2;
	MODELVERTEX3 msv3;
	SBoneMatrix bm;
	core::matrix4 m1, B;
	core::vector3df v1, v2, h1, h2, tr;
//	BONE4 b4;

	for(int i=0; i<dat->weight1; ++i) {
		mv1 = (MODELVERTEX1*)(&dat->pVertex[i]);
		v1.set(mv1->x, mv1->y, mv1->z);
		h1.set(mv1->hx, mv1->hy, mv1->hz);

		//b4 = pMB->getBone4(i);
		//bm = matBones[b4.low];
		//B = bm.boneBindPosMat;

		//if(needflip){ 
		//	if(b4.lflg==3) B =  B*matrixMirrorZ;
		//	if(b4.lflg==2) B =  B*matrixMirrorY;
		//	if(b4.lflg==1) B =  B*matrixMirrorX;
		//}
		//B.transformVect(v1);
		////normal does not need translation
		//B.setTranslation(core::vector3df(0,0,0));
		//B.transformVect(h1);

		msv3.x1 = v1.X;
		msv3.y1 = v1.Y;
		msv3.z1 = v1.Z;
		msv3.w1 = 0;

		msv3.hx1 = h1.X;
		msv3.hy1 = h1.Y;
		msv3.hy1 = h1.Z;

		msv3.x2 = 0;
		msv3.y2 = 0;
		msv3.z2 = 0;
		msv3.w2 = 0;
		msv3.u = 0;
		msv3.v = 0;

		pMB->addBindPosVertex(msv3);
	}

	for(int i=0; i<dat->weight2; ++i) {
		mv2 = (MODELVERTEX2*)(&dat->pVertex2[i]);
		v1.set(mv2->x1, mv2->y1, mv2->z1);
		h1.set(mv2->hx1, mv2->hy1, mv2->hz1);
		v2.set(mv2->x2, mv2->y2, mv2->z2);
		h2.set(mv2->hx2, mv2->hy2, mv2->hz2);

		//b4 = pMB->getBone4(i+dat->weight1);
		//bm = matBones[b4.low];
		//B = bm.boneBindPosMat;
		//if(needflip){ 
		//	if(b4.lflg==3) B =  B*matrixMirrorZ;
		//	if(b4.lflg==2) B =  B*matrixMirrorY;
		//	if(b4.lflg==1) B =  B*matrixMirrorX;
		//}
		////workaround for matrix4 inability to multiply translation
		//tr=B.getTranslation();
		//tr *= mv2->w1;
		//B.setTranslation(tr);
		//B.transformVect(v1);

		//B.setTranslation(core::vector3df(0,0,0));
		//B.transformVect(h1);
		//h1 *= mv2->w1;
		//h1.normalize();

		msv3.x1 = v1.X;
		msv3.y1 = v1.Y;
		msv3.z1 = v1.Z;
		msv3.w1 = mv2->w1;

		msv3.hx1 = h1.X;
		msv3.hy1 = h1.Y;
		msv3.hz1 = h1.Z;

		//bm = matBones[b4.high];
		//B = bm.boneBindPosMat;
		//if(needflip){ 
		//	if(b4.hflg==3) B =  B*matrixMirrorZ;
		//	if(b4.hflg==2) B =  B*matrixMirrorY;
		//	if(b4.hflg==1) B =  B*matrixMirrorX;
		//}
		//tr=B.getTranslation();
		//tr *= mv2->w2;
		//B.setTranslation(tr);
		//B.transformVect(v2);

		//B.setTranslation(core::vector3df(0,0,0));
		//B.transformVect(h2);
		//h2 *= mv2->w2;
		//h2.normalize();

		msv3.x2 = v2.X;
		msv3.y2 = v2.Y;
		msv3.z2 = v2.Z;
		msv3.w2 = mv2->w2;

		msv3.hx2 = h2.X;
		msv3.hy2 = h2.Y;
		msv3.hz2 = h2.Z;

		msv3.u = 0;
		msv3.v = 0;

		pMB->addBindPosVertex(msv3);
	}

	//get the uv
	std::vector<int> vecTriangleStrip;
	int wf, ws=0, totalIndices=0, tws=0;
	TEXLIST *uvf;
	TEXLIST2 *uvf2;
	char buf[20];
	std::string str;

	//incr to first block of uvFace data
	p = dat->pPoly;
	while(1) {
		//convert triangleStrip to triangleList
		if( vecTriangleStrip.size() > 0 ) {
			if(!needflip ) {
				//reverse
				pMB->addFace(vecTriangleStrip[2]);
				pMB->addFace(vecTriangleStrip[1]);
				pMB->addFace(vecTriangleStrip[0]);
			}
			else {
				pMB->addFace(vecTriangleStrip[0]);
				pMB->addFace(vecTriangleStrip[1]);
				pMB->addFace(vecTriangleStrip[2]);
			}
			for(u32 i=3; i<vecTriangleStrip.size(); ++i) {
				if( (i%2)==0 ) {
					if( !needflip ) {
						pMB->addFace(vecTriangleStrip[i]);
						pMB->addFace(vecTriangleStrip[i-1]);
						pMB->addFace(vecTriangleStrip[i-2]);
					}
					else {
						//012 ... 234....345....
						pMB->addFace(vecTriangleStrip[i-2]);
						pMB->addFace(vecTriangleStrip[i-1]);
						pMB->addFace(vecTriangleStrip[i]);
					}
				}
				else {
					if( !needflip ) {
						pMB->addFace(vecTriangleStrip[i]);
						pMB->addFace(vecTriangleStrip[i-2]);
						pMB->addFace(vecTriangleStrip[i-1]);
					}
					else {
						//213....324.....435
						pMB->addFace(vecTriangleStrip[i-1]);
						pMB->addFace(vecTriangleStrip[i-2]);
						pMB->addFace(vecTriangleStrip[i]);
					}
				}
			}
			vecTriangleStrip.clear();
		}
		wf = (int)*(u16*)(p );
		ws = (int)*(u16*)(p+2);
		if( 0x8010 == (wf&0x80F0) ) { 
			p+=0x2e;		//46 byte
			continue;  //晄柧
		} 
		else if ( 0x8000 == (wf&0x80F0) ) {
			memcpy(buf,p+2,16);
			buf[16]=0x00;
			str.assign(buf);
			pMB->setTextureName(str);
			p+=0x12;	//18 byte
		} 
		else {
			if( 0x5453 == wf ) {/*ST*/  //StripTriangle
				p+=4;
				uvf = (TEXLIST*)p;
				msv3 = pMB->getBindPosVertex(uvf->i1);
				msv3.u = uvf->u1;
				msv3.v = uvf->v1;
				pMB->updateBindPosVertex(uvf->i1, msv3);
				vecTriangleStrip.push_back(uvf->i1);
				
				msv3 = pMB->getBindPosVertex(uvf->i2);
				msv3.u = uvf->u2;
				msv3.v = uvf->v2;
				pMB->updateBindPosVertex(uvf->i2, msv3);
				vecTriangleStrip.push_back(uvf->i2);

				msv3 = pMB->getBindPosVertex(uvf->i3);
				msv3.u = uvf->u3;
				msv3.v = uvf->v3;
				pMB->updateBindPosVertex(uvf->i3, msv3);
				vecTriangleStrip.push_back(uvf->i3);

				p+=30;
				//subsequence add 1 vertex
				for(int k=0; k<ws-1; ++k) {
					uvf2 = (TEXLIST2*)p;
					msv3 = pMB->getBindPosVertex(uvf2->i);
					msv3.u = uvf2->u;
					msv3.v = uvf2->v;
					pMB->updateBindPosVertex(uvf2->i, msv3);
					vecTriangleStrip.push_back(uvf2->i);

					p += 10;
				}
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
				for( int k=0; k<ws; k++ ) {
					uvf = (TEXLIST*)p;
					msv3 = pMB->getBindPosVertex(uvf->i1);
					msv3.u = uvf->u1;
					msv3.v = uvf->v1;
					pMB->updateBindPosVertex(uvf->i1, msv3);
					
					msv3 = pMB->getBindPosVertex(uvf->i2);
					msv3.u = uvf->u2;
					msv3.v = uvf->v2;
					pMB->updateBindPosVertex(uvf->i2, msv3);
					
					msv3 = pMB->getBindPosVertex(uvf->i3);
					msv3.u = uvf->u3;
					msv3.v = uvf->v3;
					pMB->updateBindPosVertex(uvf->i3, msv3);
					if( !needflip ) {
						pMB->addFace(uvf->i3);
						pMB->addFace(uvf->i2);
						pMB->addFace(uvf->i1);
					}
					else {
						pMB->addFace(uvf->i1);
						pMB->addFace(uvf->i2);
						pMB->addFace(uvf->i3);					
					}
					p+=30;				//sizeof uvface
				}					
				continue;
			} 
			else
				break;
		}
	}
}


void FFXICharacter::getBindPosMesh(u32 pn, std::vector<D3DTEXVERTEX> &outVertex, std::string &textureName, std::vector<u16> &outFace )
{
	FFXIMeshBuffer *pMB = m_meshBuffer[pn];
	D3DTEXVERTEX v;
	core::vector3df v1;
	MODELVERTEX3 mv3;

	int size = pMB->getVertexCount();
	for(int i=0; i<size; ++i) {
		mv3 = pMB->getBindPosVertex(i);
		if( mv3.x2==0 ) {
			v1.set(mv3.hx1, mv3.hy1, mv3.hz1);
			v1.normalize();

			v.x = mv3.x1;
			v.y = mv3.y1;
			v.z = mv3.z1;
			v.hx = v1.X;
			v.hy = v1.Y;
			v.hz = v1.Z;
			v.tu = mv3.u;
			v.tv = mv3.v;
		}
		else {
			v1.set(mv3.hx1 + mv3.hx2, mv3.hy1 + mv3.hy2, mv3.hz1 + mv3.hz2);
			v1.normalize();

			v.x = mv3.x1 + mv3.x2;
			v.y = mv3.y1 + mv3.y2;
			v.z = mv3.z1 + mv3.z2;
			v.hx = v1.X;
			v.hy = v1.Y;
			v.hz = v1.Z;
			v.tu = mv3.u;
			v.tv = mv3.v;
		}
		outVertex.push_back(v);
	}

	textureName = pMB->getTextureName();

	size = pMB->getFaceCount();
	u16 tl;
	for(int i=0; i<size; ++i) {
		tl = pMB->getFace(i);
		outFace.push_back(tl);
	}
}

void FFXICharacter::nextAnimation(s32 &frameStart, s32 &frameEnd, f32 &speed)
{
	m_curAnimeDat2BIndex++;
	if( m_curAnimeDat2BIndex>=arrAnimInfo.size() )
		m_curAnimeDat2BIndex=0;

	setCurrentAnimation(m_curAnimeDat2BIndex, frameStart, frameEnd, speed);
}

void FFXICharacter::setCurrentAnimation(int i, s32 &frameStart, s32 &frameEnd, f32 &speed)
{ 
	m_curAnimeDat2BIndex=i;

	//create the AnimBindPosMat
	core::quaternion q;
	core::matrix4 A;
	SAnimInfo info = arrAnimInfo[m_curAnimeDat2BIndex];
	frameStart = info.frameStart;
	frameEnd = info.frameEnd;
	speed = (30*info.speed);
}

s32 FFXICharacter::getAnimIndex(s32 frameStart, s32 frameEnd)
{
	for(auto it=arrAnimInfo.begin(); it!=arrAnimInfo.end(); ++it) {
		if( frameStart>=it->frameStart && frameEnd<=it->frameEnd)
			return it->dat2BIndex;
	}
	return -1;
}

void FFXICharacter::skinMesh(u32 pn, std::vector<D3DTEXVERTEX> &outVertex)
{
	FFXIMeshBuffer *pMB = m_meshBuffer[pn];
	D3DTEXVERTEX v;
	MODELVERTEX3 mv3;
	SBoneMatrix *pbm=nullptr;
	BONE4 b4;
	core::matrix4 B;
	core::vector3df v1, v2, h1, h2, tr;

	int size = pMB->getVertexCount();
	for(int i=0; i<size; ++i) {
		mv3 = pMB->getBindPosVertex(i);
		
		if( mv3.x2==0 ) {
			v1.set(mv3.x1, mv3.y1, mv3.z1);
			h1.set(mv3.hx1, mv3.hy1, mv3.hz1);

			b4 = pMB->getBone4(i);
			pbm = matBones[b4.low];
			B = pbm->skinLocalMat;

			if(pMB->needFlip()) { 
				if(b4.lflg==1) v1.X = -v1.X;
				if(b4.lflg==2) v1.Y = -v1.Y;
				if(b4.lflg==3) v1.Z = -v1.Z;
			}
			B.transformVect(v1);
			//normal does not need translation
			B.setTranslation(core::vector3df(0,0,0));
			B.transformVect(h1);
			h1.normalize();

			v.x = v1.X;
			v.y = v1.Y;
			v.z = v1.Z;
			v.hx = h1.X;
			v.hy = h1.Y;
			v.hz = h1.Z;
			v.tu = mv3.u;
			v.tv = mv3.v;
			outVertex.push_back(v);
		}
		else {
			v1.set(mv3.x1, mv3.y1, mv3.z1);
			v2.set(mv3.x2, mv3.y2, mv3.z2);
			h1.set(mv3.hx1, mv3.hy1, mv3.hz1);
			h2.set(mv3.hx2, mv3.hy2, mv3.hz2);

			b4 = pMB->getBone4(i);
			pbm = matBones[b4.low];
			B = pbm->skinLocalMat;
			if(pMB->needFlip()) { 
				if(b4.lflg==1) v1.X = -v1.X;
				if(b4.lflg==2) v1.Y = -v1.Y;
				if(b4.lflg==3) v1.Z = -v1.Z;
			}
			//workaround for matrix4 inability to multiply translation
			tr=B.getTranslation();
			tr *= mv3.w1;
			B.setTranslation(tr);
			B.transformVect(v1);

			B.setTranslation(core::vector3df(0,0,0));
			B.transformVect(h1);
			h1 *= mv3.w1;
			h1.normalize();

			pbm = matBones[b4.high];
			B = pbm->skinLocalMat;
			if(pMB->needFlip()) { 
				if(b4.hflg==1) v2.X = -v2.X;
				if(b4.hflg==2) v2.Y = -v2.Y;
				if(b4.hflg==3) v2.Z = -v2.Z;
			}
			tr=B.getTranslation();
			tr *= mv3.w2;
			B.setTranslation(tr);
			B.transformVect(v2);

			B.setTranslation(core::vector3df(0,0,0));
			B.transformVect(h2);
			h2 *= mv3.w2;
			h2.normalize();
		
			h1.set(h1.X + h2.X, h1.Y + h2.Y, h1.Z + h2.Z);
			h1.normalize();

			v.x = v1.X + v2.X;
			v.y = v1.Y + v2.Y;
			v.z = v1.Z + v2.Z;
			v.hx = h1.X;
			v.hy = h1.Y;
			v.hz = h1.Z;
			v.tu = mv3.u;
			v.tv = mv3.v;
			outVertex.push_back(v);
		}
	}
}