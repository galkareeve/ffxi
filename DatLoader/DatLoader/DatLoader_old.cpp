#include "StdAfx.h"
#include "DatLoader.h"
#include "quaternion.h"
#include <iomanip>

#define WRITE_MMB	1
#define COUNT_W		128

CDatLoader::CDatLoader(void)
{
	cur_mesh=1;
	float arrX[16]={-1.0f,0,0,0,  0, 1.0f,0,0,  0,0, 1.0f,0,  0,0,0,1.0f};
	float arrY[16]={ 1.0f,0,0,0,  0,-1.0f,0,0,  0,0, 1.0f,0,  0,0,0,1.0f};
	float arrZ[16]={ 1.0f,0,0,0,  0, 1.0f,0,0,  0,0,-1.0f,0,  0,0,0,1.0f};

	matrixMirrorX.setM(arrX);
	matrixMirrorY.setM(arrY);
	matrixMirrorZ.setM(arrZ);

	m_fileNo=0;
	m_lastIndices=0;
	m_vecVector.clear();
	m_vecIndices.clear();
}


CDatLoader::~CDatLoader(void)
{
	m_vecVector.clear();
	m_vecIndices.clear();
}

bool CDatLoader::loadDat(int fno)
{
	FFXIFile f;
	int dir=0,rem=fno;
	if(fno>=1000000) {
		dir=fno/1000000;
		rem -= dir*1000000;
	}

	if( !f.LoadF(dir, rem)) {
		cout << "unable to load file" << endl;
		return false;
	}
	m_fileNo=fno;
	string str;
	char buf[50];

	sprintf_s(buf,50,"%d.txt",fno);
	str.assign(buf);
	ofs.open(str.c_str(), std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		return false;
	}

	unsigned int len,mzb=0,mmb=0;
	DATHEAD hd;

	for(int i=0; i<2; ++i)
	{
		char *p, *start=f.FistData(&hd);
		LPSTR iname;
		for( p=f.FistData(&hd); p; p=f.NextData(&hd) )
		{
			int type = (int)hd.type;
			iname = p;
			len = (hd.next&0x7ffff)*16;
			switch (type)
			{
				case 0x1c:  //28 MZB
					if( i==0 ) {
						decode_mzb((BYTE*)(p+16));
						ofs << mzb << ")  offset: " << p-start << " MZB " << iname << " type: " << hd.type << " size: " << len << endl;
//						extractMZB(p+sizeof(DATHEAD), len-sizeof(DATHEAD), mzb);
						mzb++;
					}
				break;
				case 0x2e:  //46 MMB
					if( i==1 ) {
						decode_mmb((BYTE*)(p+16));		
						ofs << mmb << ")  offset: " << p-start << " MMB " << iname << " type: " << hd.type << " size: " << len << endl;
						extractMMB(p+sizeof(DATHEAD), len-sizeof(DATHEAD), mmb);
						mmb++;
					}
				break;
				case 0x20:  //32 IMG
					if( i==0 ) {
						ofs << "offset: " << p-start << " IMG " << iname << " type: " << hd.type << " size: " << len << endl;
						extractImage(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
					}
				break;
				case 0x29:	//41 Bone
					if( i==0 ) {
						ofs << "offset: " << p-start << " Bone " << iname << " type: " << hd.type << " size: " << len << endl;
						extractBone(p+sizeof(DATHEAD),len-sizeof(DATHEAD));
					}
				break;
				case 0x2B:	//43 animation
					if( i==0 ) {
						ofs << "offset: " << p-start << " Animation " << iname << " type: " << hd.type << " size: " << len << endl;
						extractAnimation(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
					}
				break;
				case 0x2a:	//42 vertex
					if( i==1 ) {
						ofs << "offset: " << p-start << " Vertex " << iname << " type: " << hd.type << " size: " << len << endl;
						extractVertex(p+sizeof(DATHEAD),len-sizeof(DATHEAD));
					}
				break;
				case 0x5:		//5
					if( i==0 ) {
						ofs << "offset: " << p-start << " unknown " << iname << " type: " << hd.type << " size: " << len << endl;
					}
					break;
				case 0x7:		//7 schedule animation
					if( i==0 ) {
						ofs << "offset: " << p-start << " schedule " << iname << " type: " << hd.type << " size: " << len << endl;
					}
					break;
				case 0x19:	//25 
					if( i==0 ) {
						ofs << "offset: " << p-start << " unknown " << iname << " type: " << hd.type << " size: " << len << endl;
					}
					break;
				case 0x3D:	//61 animation subcategory
					if( i==0 ) {
						ofs << "offset: " << p-start << " category " << iname << " type: " << hd.type << " size: " << len << endl;
					}
					break;

				default:
					if( i==0 ) {
						ofs << "offset: " << p-start << " unknown " << iname << " type: " << hd.type << " size: " << len << endl;
					}
				break;
			}
		}
	}

	if( WRITE_MMB) {
		write_MMB();
	}

	cout << "Done parsing dat" << endl;
	f.Free();
	ofs.close();
	return true;
}

void CDatLoader::extractMZB(char *p, unsigned int len, unsigned int count)
{
//	char *buf = new char[len];
//	memcpy(buf,p,len);
//
//	char fileName[30];
//	sprintf_s(fileName,30,"MZB%d_%d", count, m_fileNo);
//	ofstream output(fileName, std::ofstream::out | std::ofstream::binary);
//	for(int i=0; i<len; ++i) {
//		output << hex << buf[i] ;
//	}
//	output.close();
//
//	delete []buf;
//return;

	SMZBHeader *mzbh1 = (SMZBHeader*)p;
	mzbh1->totalRecord100 &=0xffffff;

	OBJINFO *oj, ob;
	SMZBBlock84 *b84;
	int noj;

	if( mzbh1->offsetHeader2==0 ) {
		//for MMB object
		b84 = (SMZBBlock84*)(p+32);
		noj = mzbh1->totalRecord100 & 0xffffff;
		ofs << "Block84: " << noj << endl;
		for(int i=0; i<noj; ++i ) {
			memcpy(&ob.id, b84->id, 16);
			ob.fTransX=b84->fTransX;
			ob.fTransY=b84->fTransY;
			ob.fTransZ=b84->fTransZ;
			ob.fRotX=b84->fRotX;
			ob.fRotY=b84->fRotY;
			ob.fRotZ=b84->fRotZ;
			ob.fScaleX=b84->fScaleX;
			ob.fScaleY=b84->fScaleY;
			ob.fScaleZ=b84->fScaleZ;
			ob.fa=b84->fa;
			ob.fb=b84->fb;
			ob.fc=b84->fc;
			ob.fd=b84->fd;
			ob.fe=b84->i1;
			ob.ff=b84->i2;
			ob.fg=b84->i3;
			ob.fh=b84->i4;
			ob.fi=0;
			ob.fj=0;
			ob.fk=0;
			ob.fl=0;
			m_vecOBJInfo.push_back(ob);
			ofs << "    id: " << b84->id << " Trans: (" << b84->fTransX << ", " << b84->fTransY << ", " << b84->fTransZ << ")  Rot: (" << b84->fRotX << ", " << b84->fRotY << ", " << b84->fRotZ << ")" << endl;
			ofs << "    Scale: (" << b84->fScaleX << ", " << b84->fScaleY << ", " << b84->fScaleZ << ")  [" << b84->fa << ", " << b84->fb << ", " << b84->fc << ", " << b84->fd << "]  [" << b84->i1 << ", " << b84->i2 << ", " << b84->i3 << ", " << b84->i4 << "]" << endl;
			b84++;
		}
		return;
	}

	//block100, for the MZB object
	oj =  (OBJINFO *)(p+32);
	noj = (*(int*)(p+4) )&0xffffff;
	ofs << "Block84: " << noj << endl;
	for(int i=0; i<noj; ++i) {
		m_vecOBJInfo.push_back(*oj);
		ofs << "     id: " << oj->id << " Trans: (" << oj->fTransX << ", " << oj->fTransY << ", " << oj->fTransZ << ")  Rot: (" << oj->fRotX << ", " << oj->fRotY << ", " << oj->fRotZ << ")" << endl;
		ofs << "     Scale: (" << oj->fScaleX << ", " << oj->fScaleY << ", " << oj->fScaleZ << ")  [" << oj->fa << ", " << oj->fb << ", " << oj->fc << ", " << oj->fd << ", " << oj->fe << ", " << oj->ff << ", " << oj->fg << ", " << oj->fh << ", " << oj->fi << ", " << oj->fj << ", " << oj->fk << ", " << oj->fl << "]" << endl;
		oj++;
	}

	//block92...variable length
	SMZBHeader2 *mzbh2 = (SMZBHeader2*)(p+mzbh1->offsetHeader2);
	ofs << "Header2 - " << mzbh2->totalRecord92 << ", " << mzbh2->offsetBlock92 << ", " << mzbh2->totalBlock16 << ", " << mzbh2->offsetBlock16 << ", " << mzbh2->offsetVertex << ", "  << mzbh2->offsetBlock112 << ", " << mzbh2->totalRecord112 << ", " << mzbh2->unk1 << endl;

	mzbh2->totalRecord92 &=0xffffff;
	ofs << "  " << endl;
	ofs << "Block92: " << mzbh2->totalRecord92 << endl;

	unsigned int totalsize=mzbh2->offsetBlock92, r1, r2, r3, sizeB, sizeV, sizeN;
	float x,y,z;
//	SMZBBlock92 *pB92 = (SMZBBlock92*)(p+totalsize);
	for(int i=0; i<mzbh2->totalRecord92; ++i) {
		r1 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
		r2 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
		r3 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
		sizeB = (*(unsigned int*)(p+totalsize)); totalsize +=4;
		sizeB &= 0xffff;
		ofs << r1 << " - " << r2 << " - " << r3 << "    size: " << sizeB << endl;

		sizeV = (r2-r1)/12;
		for(int j=0; j<sizeV; ++j) {
			x = (*(float*)(p+totalsize)); totalsize +=4;
			y = (*(float*)(p+totalsize)); totalsize +=4;
			z = (*(float*)(p+totalsize)); totalsize +=4;
			ofs << j << ") Vertex xyz: " << x << ", " << y << ", " << z << endl;
		}

		sizeN = (r3-r2)/12;
		for(int j=0; j<sizeN; ++j) {
			x = (*(float*)(p+totalsize)); totalsize +=4;
			y = (*(float*)(p+totalsize)); totalsize +=4;
			z = (*(float*)(p+totalsize)); totalsize +=4;
			ofs << j << ") Normal xyz: " << x << ", " << y << ", " << z << endl;
		}
		
		SMZBBlock8 *pB8=(SMZBBlock8*)(p+totalsize);
		for(int j=0; j<sizeB; ++j) {
			ofs << limit(sizeV, pB8->s1) << ", " << limit(sizeV, pB8->s2) << ", " << limit(sizeV, pB8->s3) << ", " << limit(sizeV, pB8->s4) << endl;
			pB8++;
		}
		totalsize += sizeB*8;			//sizeB*4*2(byte)

		//ofs << i << ") " << pB92->range1 << ", " << pB92->range2 << ", " << pB92->range3 << endl;
		//ofs << pB92->f1 << ", " << pB92->f2 << ", " << pB92->f3 << ", " << pB92->f4 << ", " << pB92->f5 << ", " << pB92->f6 << ", " << pB92->f7 << ", " << pB92->f8 << ", " << pB92->f9 << ", " << pB92->f10 << ", " << pB92->f11 << ", " << pB92->f12 << ", " << pB92->f13 << ", " << pB92->f14 << ", " << pB92->f15 << endl;
		//ofs << pB92->unk3 << "  " << pB92->footer << endl;
		//pB92++;
	}


	//block112
	ofs << "  " << endl;
	ofs << "Block112: " << mzbh2->totalRecord112 << endl;
	SMZBBlock112 *pB112;
	pB112 = (SMZBBlock112*)(p+mzbh2->offsetBlock112);
	for(int i=0; i<mzbh2->totalRecord112; ++i) {
		ofs << i << ") " << pB112->f1 << ", " << pB112->f2 << ", " << pB112->f3 << ", " << pB112->f4 << ", " << pB112->f5 << ", " << pB112->f6 << ", " << pB112->f7 << ", " << pB112->f8 << ", " << pB112->f9 << ", " << pB112->f10 << endl;
		ofs << pB112->f11 << ", " << pB112->f12 << ", " << pB112->f13 << ", " << pB112->f14 << ", " << pB112->f15 << ", " << pB112->f16 << endl;
		ofs << pB112->f17 << ", " << pB112->f18 << ", " << pB112->f19 << ", " << pB112->f20 << ", " << pB112->f21 << ", " << pB112->f22 << ", " << pB112->f23 << ", " << pB112->f24 << ", " << pB112->f25 << ", " << pB112->f26 << ", " << pB112->f27 << ", " << pB112->f28 << endl;
		pB112++;
	}
	//block16...variable length
	ofs << "  " << endl;
	sizeV = mzbh2->totalBlock16;
	ofs << "Block16: " << sizeV << endl;
	totalsize = mzbh2->offsetBlock16;
	SMZBBlock16 *pB16;
	sizeN=0;
	for(int i=0; i<sizeV; ++i) {
		pB16 = (SMZBBlock16*)(p+totalsize);
		if( pB16->i4==0 ) {
			ofs << i << ") " << (pB16->i1-sizeN) << "  - " << pB16->i1 << ", " << pB16->i2 << ", " << pB16->i3 << ", " << pB16->i4 << endl;
			totalsize += 16;
			sizeN = pB16->i1;
		}
		else {
			ofs << i << ") " << (pB16->i1-sizeN) << "  - " << pB16->i1 << ", " << pB16->i2 << ", " << pB16->i3 << ", " << pB16->i4;
			totalsize += 16;
			sizeN=pB16->i1;
			do {
				r1 = (*(unsigned int*)(p+totalsize));
				totalsize +=4;
				ofs << ", " << r1;
			} while(r1!=0);
			ofs << endl;
		}

		//pB16++;
	}

	//block4, scan till end of MZB
	totalsize = mzbh2->offsetVertex;
	unsigned int last=len-16;
	for(int i=totalsize; i<len; ) {
		if(i>last) {
			if((i-last) <5 )
				ofs << (*(unsigned int*)(p+i)) << ", " << (*(unsigned int*)(p+i+4)) << ", " << (*(unsigned int*)(p+i+8)) << endl;
			else if((i-last) < 9)
				ofs << (*(unsigned int*)(p+i)) << ", " << (*(unsigned int*)(p+i+4)) << endl;
			else if((i-last) < 13)
				ofs << (*(unsigned int*)(p+i)) << endl;

			break;
		}
		else
			ofs << (*(unsigned int*)(p+i)) << ", " << (*(unsigned int*)(p+i+4)) << ", " << (*(unsigned int*)(p+i+8)) << ", " << (*(unsigned int*)(p+i+12)) << endl;

		i+=16;
	}

	//output mesh for blender
	if(0) {
		char buf[50];
		sprintf_s(buf,50,"mapMesh%d_%d.txt", count, m_fileNo);
		ofstream mapStream(buf,std::ofstream::out);

		m_lastIndices=0;
		m_vecVector.clear();
		m_vecIndices.clear();
		totalsize = mzbh2->offsetBlock16;
		unsigned int tmpN;
		for(int i=0; i<sizeV; ++i) {
			pB16 = (SMZBBlock16*)(p+totalsize);
			if( pB16->i4==0 ) {
				//use i2, i3
				decodeMesh(p, pB16->i2, pB16->i3);
				totalsize += 16;
			}
			else {
				decodeMesh(p, pB16->i2, pB16->i3);
				totalsize += 16;
				tmpN=pB16->i4;
				do {
					r1 = (*(unsigned int*)(p+totalsize));
					totalsize +=4;
					if(tmpN==0)
						tmpN=r1;
					else {
						decodeMesh(p, tmpN, r1);
						tmpN=0;
					}
				} while(r1!=0);
				ofs << endl;
			}
		}

		mapStream << m_vecIndices.size() << endl;
		for(auto it=m_vecIndices.begin(); it!=m_vecIndices.end(); ++it) {
			mapStream << it->i1 << "," << it->i2 << "," << it->i3 << endl;
		}

		mapStream << m_vecVector.size() << endl;
		for(auto it=m_vecVector.begin(); it!=m_vecVector.end(); ++it) {
			mapStream << it->x << "," << it->y << "," << it->z << endl;
		}
		mapStream.close();

		m_lastIndices=0;
		m_vecIndices.clear();
		m_vecVector.clear();
	}
}

void CDatLoader::decodeMesh(char *p, unsigned int offsetB112, unsigned int offsetB92)
{
	unsigned int r1, r2, r3, sizeB, sizeV, sizeN,totalsize=offsetB92;
	float x,y,z;
	SMZBVector v;
	SMZBFace f;
	SMZBBlock112 *pB112 = (SMZBBlock112*)(p+offsetB112);

	r1 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	r2 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	r3 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	sizeB = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	sizeB &= 0xffff;

	sizeV = (r2-r1)/12;
	for(int j=0; j<sizeV; ++j) {
		v.x = (*(float*)(p+totalsize)); totalsize +=4;
		v.y = (*(float*)(p+totalsize)); totalsize +=4;
		v.z = (*(float*)(p+totalsize)); totalsize +=4;
		multiplyByMatrix(pB112, v.x, v.y, v.z);
		m_vecVector.push_back(v);
	}

	sizeN = (r3-r2)/12;
	//for(int j=0; j<sizeN; ++j) {
	//	x = (*(float*)(p+totalsize)); totalsize +=4;
	//	y = (*(float*)(p+totalsize)); totalsize +=4;
	//	z = (*(float*)(p+totalsize)); totalsize +=4;
	//	
	//}
	//skip normal
	totalsize += sizeN*12;
	SMZBBlock8 *pB8=(SMZBBlock8*)(p+totalsize);
	for(int j=0; j<sizeB; ++j) {
		f.i1 = limit(sizeV, pB8->s1) + m_lastIndices;
		f.i2 = limit(sizeV, pB8->s2) + m_lastIndices;
		f.i3 = limit(sizeV, pB8->s3) + m_lastIndices;
		m_vecIndices.push_back(f);
		pB8++;
	}
	m_lastIndices+=sizeV;
}

void CDatLoader::multiplyByMatrix( SMZBBlock112 *mat, float &vx, float &vy, float &vz)
{
	float x = mat->f1 * vx + mat->f5 * vy + mat->f9 * vz + mat->f13;
	float y = mat->f2 * vx + mat->f6 * vy + mat->f10 * vz + mat->f14;
	float z = mat->f3 * vx + mat->f7 * vy + mat->f11 * vz + mat->f15;
	vx = x;
	vy = y;
	vz = z;
}

unsigned int CDatLoader::limit( unsigned int max, unsigned short val)
{
	if( val<max ) return val;

	unsigned int mask = 0x7fff;
	do {
		val &= mask;
		mask >>= 1;
	} while(val>max);
	return val;
}

void CDatLoader::extractMMB(char *p, unsigned int len, unsigned int count)
{
	//char *buf = new char[len];
	//memcpy(buf,p,len);

	//char fileName[30];
	//sprintf_s(fileName,30,"MMB%d_%d", count, m_fileNo);
	//ofstream output(fileName, std::ofstream::binary);
	//for(int i=0; i<len; ++i) {
	//	output << hex << buf[i] ;
	//}
	//
	//output.close();
	//delete []buf;
//return;

	unsigned int totalsize=0, indexS, offsetStartMMB, offset, incr, rangeOffset, maxRange, in;
	SMZBVector v;
	SMZBFace f;

	offsetStartMMB = totalsize;

	SMMBHEAD *pMMB = (SMMBHEAD*)(p+totalsize);
	SMMBHEAD2 *pMMB2 = (SMMBHEAD2*)(p+totalsize);
	totalsize += sizeof(SMMBHEAD);

	if( (pMMB->id[0]=='M') && (pMMB->id[1]=='M') && (pMMB->id[2]=='B') )
		ofs << "  MMB Head: " << pMMB->id << " type: " << pMMB->type << " size: " << pMMB->next*16 << endl;
	else 
		ofs << "  MMB Head2: " << pMMB2->MMBSize << " unk1: " << pMMB2->unk1 << " unk2: " << pMMB2->unk2 << pMMB2->name << endl;
	
	SMMBHeader *pMMBH = (SMMBHeader*)(p+totalsize);
	totalsize += sizeof(SMMBHeader);
	ofs << "  Header1:  imgID: " << pMMBH->imgID << " pieces: " << pMMBH->pieces << "  BoundingRect Combine xyz (min,max): " <<  pMMBH->x1 << ", " << pMMBH->x2 << ", " << pMMBH->y1 << ", " << pMMBH->y2 << ", " << pMMBH->z1 << ", " << pMMBH->z2 << endl;
	//get the global transformation from MZB
	OBJINFO *objinfo = findMMBTransform(pMMBH->imgID);

	//NOTE: range is not always 9 float...it is dependent on the start of SMMBBlockHeader
	SMMBRange *pMMBR = (SMMBRange*)(p+totalsize);
	ofs << " r1: " << pMMBR->r1 << " :" << pMMBR->unk1 << "," << pMMBR->unk2 << "," << pMMBR->unk3 << " r2: " << pMMBR->r2 << ", " << pMMBR->unk4 << "," << pMMBR->unk5 << "," << pMMBR->unk6 << " r3: " << pMMBR->r3 << endl;

	//find the first range offset
	maxRange=0;
	if( pMMBR->r1!=0 )
		maxRange = (pMMBR->r1 - totalsize)/4;
	else if( pMMBR->r2!=0 ) 
		maxRange = (pMMBR->r2 - totalsize)/4;
	else if( pMMBR->r3!=0 )
		maxRange = (pMMBR->r3 - totalsize)/4;

	incr=0; rangeOffset=totalsize;
	SMMBBlockHeader *pMMBBH = nullptr;
	//range are spread among the 9 field
	for(int BH=0; BH<maxRange; ++BH) {
		offset = (*(unsigned int*)(p+rangeOffset+BH*4));
		if( offset==0 )
			continue;

		incr++;
		if( incr > pMMBH->pieces ) {
			ofs << "invalid MMB size: " << pMMBH->pieces << endl;
			return;
		}
		offset += offsetStartMMB;
		pMMBBH = (SMMBBlockHeader*)(p+offset);
		offset += sizeof(SMMBBlockHeader);		
		ofs << "   BoundingRect numModel: " << pMMBBH->numModel << " xyz (min,max): " << pMMBBH->x1 << ", " << pMMBBH->x2 << ", " << pMMBBH->y1 << ", " << pMMBBH->y2 << ", " << pMMBBH->z1 << ", " << pMMBBH->z2 << " u2: " << pMMBBH->unk1 << endl;

		SMMBModelHeader *pMMBMH = (SMMBModelHeader*)(p+offset);
		offset += sizeof(SMMBModelHeader);
		pMMBMH->vertexsize &= 0xffff;
		ofs <<  " ModelHeader: " << pMMBMH->model << " BlockVertex size: " << pMMBMH->vertexsize << endl;

		for(int i=0; i<pMMBMH->vertexsize; ++i) {
			SMMBBlockVertex *pMMBBV = (SMMBBlockVertex*)(p+offset);
			offset += sizeof(SMMBBlockVertex);
			ofs << setw(8) << i << ")  " << setw(6) << "xyz: " << setw(9) << pMMBBV->x << ", " << setw(9) << pMMBBV->y << ", " << setw(9) << pMMBBV->z << "  normal: " << setw(11) << pMMBBV->hx << ", " << setw(11) << pMMBBV->hy << ", " << setw(11) << pMMBBV->hz << "  " << setw(10) << pMMBBV->zero << ", " << pMMBBV->one << endl;

			if( WRITE_MMB && (count==COUNT_W)) {
//			if( WRITE_MMB ) {
				v.x = pMMBBV->x; v.y = pMMBBV->y; v.z = pMMBBV->z;
				//transform
				if( objinfo!=nullptr ) {
					MMBTransform(objinfo, v);
				}
				m_vecVector.push_back(v);
			}
		}

		indexS = (*(unsigned int*)(p+offset));
		ofs << "   Indices size: " << indexS << endl;
		offset += 4;

		if( WRITE_MMB && (count==COUNT_W)) {
//		if( WRITE_MMB ) {
			//triangleStrip with special encoding
			//3 similiar index==STOP, start new triangleStrip with new index
			//2 similiar index==END triangleStrip, start new triangleStrip with last index
			f.i1 = ((*(unsigned int*)(p+offset))&0xffff);
			f.i2 = ((*(unsigned int*)(p+offset+2))&0xffff);
			f.i3 = ((*(unsigned int*)(p+offset+4))&0xffff);
			pushMMBIndices(f);
			f.i1 = f.i2;
			f.i2 = f.i3;
			for(unsigned int j=3; j<indexS;) {
				f.i3 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
				if( f.i2!=f.i3 ) {
					if(j%2) {
						//make clockwise
						in = f.i1;
						f.i1 = f.i2;
						f.i2 = in;
						pushMMBIndices(f);
						//reverse back, i1 is already i2
						f.i2 = f.i3;
					}
					else {
						pushMMBIndices(f);
						f.i1 = f.i2;
						f.i2 = f.i3;
					}
					++j;
				}
				else {
					f.i1 = f.i2;
					f.i2 = f.i3;
					++j;
					//check if 2 similiar or 3 similiar
					f.i3 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
					if( f.i2==f.i3 ) {
						//3 similiar, is mostly follow by 2 similiar
						++j;
						f.i1 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
						++j;
						if( f.i1==f.i3 ) {
							//more than 3 silimiar, loop until this index is gone
							do {
								f.i1 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
								++j;
							} while (f.i1==f.i3);
						}
						f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
						++j;
						if( f.i1==f.i2 ) {
							//check really follow by 2 similiar
							f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
							++j;
						} //else do nothing
					}
					else {
						//2 similiar, might need to reset the rotation???
						f.i1=f.i2;
						f.i2=f.i3;
						++j;
					}
				}
			}
			m_lastIndices += pMMBMH->vertexsize;
		}

		unsigned int last = indexS-4;
		for(unsigned int j=0; j<indexS;) {
			if( j>last ) {
				if( (indexS-j)>2) {
					ofs << "    " << ((*(unsigned int*)(p+offset))&0xffff) << ", " << ((*(unsigned int*)(p+offset+2))&0xffff) << ", " << ((*(unsigned int*)(p+offset+4))&0xffff) << endl;
					offset +=6;
				}
				else if( (indexS-j)>1) {
					ofs << "    " << ((*(unsigned int*)(p+offset))&0xffff) << ", " << ((*(unsigned int*)(p+offset+2))&0xffff) << endl;
					offset +=4;
				}
				else {
					ofs << "    " << ((*(unsigned int*)(p+offset))&0xffff) << endl;
					offset +=2;
				}
				break;
			}
			ofs << "    " << ((*(unsigned int*)(p+offset))&0xffff) << ", " << ((*(unsigned int*)(p+offset+2))&0xffff) << ", " << ((*(unsigned int*)(p+offset+4))&0xffff) << ", " << ((*(unsigned int*)(p+offset+6))&0xffff) << ", " << endl;
			j+=4;
			offset += 8;
		}
		totalsize = offset;
	}

	//NOTE if didn't end, then it is follow by another SMMBModelHeader
	//TBC...this block might not be at the end, it might need to depend on the RANGE too....
	//the RANGE determine num of separate piece of each model
	if( (pMMBBH!=nullptr) && (pMMBBH->numModel>1) ) {
		for(int k=1; k<pMMBBH->numModel; ++k) {		
			if( offset + sizeof(SMMBModelHeader) > len ) {
				ofs << "   Invalid numModel: " << pMMBBH->numModel << "  was " << k << endl;
				break;
			}
			in = (*(unsigned int*)(p+offset))&0xffff;
			if( in==0 )
				offset += 2;	//2 byte padding

			SMMBModelHeader *pMMBMH = (SMMBModelHeader*)(p+offset);
			offset += sizeof(SMMBModelHeader);
			pMMBMH->vertexsize &= 0xffff;
			ofs << "   offset: " << offset << " model: " << pMMBMH->model << " BlockVertex size: " << pMMBMH->vertexsize << endl;

			for(int i=0; i<pMMBMH->vertexsize; ++i) {
				SMMBBlockVertex *pMMBBV = (SMMBBlockVertex*)(p+offset);
				offset += sizeof(SMMBBlockVertex);
				ofs << setw(8) << i << ")  " << setw(6) << "xyz: " << setw(9) << pMMBBV->x << ", " << setw(9) << pMMBBV->y << ", " << setw(9) << pMMBBV->z << "  normal: " << setw(9) << pMMBBV->hx << ", " << setw(9) << pMMBBV->hy << ", " << setw(9) << pMMBBV->hz << "  " << pMMBBV->zero << ", " << pMMBBV->one << endl;

				if( WRITE_MMB && (count==COUNT_W)) {
//				if( WRITE_MMB ) {
					v.x = pMMBBV->x; v.y = pMMBBV->y; v.z = pMMBBV->z;
					//transform
					if( objinfo!=nullptr ) {
						MMBTransform(objinfo, v);
					}
					m_vecVector.push_back(v);
				}
			}

			indexS = (*(unsigned int*)(p+offset));
			ofs << "   Indices size: " << indexS << endl;
			offset += 4;

			if( WRITE_MMB && (count==COUNT_W)) {
//			if( WRITE_MMB ) {
				//triangleStrip with special encoding
				//3 similiar index==STOP, start new triangleStrip with new index
				//2 similiar index==END triangleStrip, start new triangleStrip with last index
				f.i1 = ((*(unsigned int*)(p+offset))&0xffff);
				f.i2 = ((*(unsigned int*)(p+offset+2))&0xffff);
				f.i3 = ((*(unsigned int*)(p+offset+4))&0xffff);
				pushMMBIndices(f);
				f.i1 = f.i2;
				f.i2 = f.i3;
				for(unsigned int j=3; j<indexS;) {
					f.i3 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
					if( f.i2!=f.i3 ) {
						if(j%2) {
							//make clockwise
							in = f.i1;
							f.i1 = f.i2;
							f.i2 = in;
							pushMMBIndices(f);
							//reverse back, i1 is already i2
							f.i2 = f.i3;
						}
						else {
							pushMMBIndices(f);
							f.i1 = f.i2;
							f.i2 = f.i3;
						}
						++j;
					}
					else {
						f.i1 = f.i2;
						f.i2 = f.i3;
						++j;
						//check if 2 similiar or 3 similiar
						f.i3 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
						if( f.i2==f.i3 ) {
							//3 similiar, is mostly follow by 2 similiar
							++j;
							f.i1 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
							++j;
							if( f.i1==f.i3 ) {
								//more than 3 silimiar, loop until this index is gone
								do {
									f.i1 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
									++j;
								} while (f.i1==f.i3);
							}
							f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
							++j;
							if( f.i1==f.i2 ) {
								//check really follow by 2 similiar
								f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
								++j;
							} //else do nothing
						}
						else {
							//might need to reset the rotation???
							f.i1=f.i2;
							f.i2=f.i3;
							++j;
						}
					}
				}
				m_lastIndices += pMMBMH->vertexsize;
			}

			unsigned int last = indexS-4;
			for(unsigned int j=0; j<indexS;) {
				if( j>last ) {
					if( (indexS-j)>2) {
						ofs << "    " << ((*(unsigned int*)(p+offset))&0xffff) << ", " << ((*(unsigned int*)(p+offset+2))&0xffff) << ", " << ((*(unsigned int*)(p+offset+4))&0xffff) << endl;
						offset +=6;
					}
					else if( (indexS-j)>1) {
						ofs << "    " << ((*(unsigned int*)(p+offset))&0xffff) << ", " << ((*(unsigned int*)(p+offset+2))&0xffff) << endl;
						offset +=4;
					}
					else {
						ofs << "    " << ((*(unsigned int*)(p+offset))&0xffff) << endl;
						offset +=2;
					}
					break;
				}
				ofs << "    " << ((*(unsigned int*)(p+offset))&0xffff) << ", " << ((*(unsigned int*)(p+offset+2))&0xffff) << ", " << ((*(unsigned int*)(p+offset+4))&0xffff) << ", " << ((*(unsigned int*)(p+offset+6))&0xffff) << ", " << endl;
				j+=4;
				offset += 8;
			}
			totalsize = offset;
		}
	}

	ofs << "   totalsize: " << totalsize << "  padding: " << (len-totalsize) << endl;
	//padding 16Byte align, not needed, padding due to appending all MMB into same file
	//incr = offset % 16;
	//totalsize = offset + (16-incr);
}

OBJINFO* CDatLoader::findMMBTransform( char *name)
{
	for(auto it=m_vecOBJInfo.begin(); it!=m_vecOBJInfo.end(); ++it) {
		if( memcmp(it->id, name, 16)==0 )
			return &(*it);
	}
	return nullptr;
}

void CDatLoader::MMBTransform(OBJINFO *b84, SMZBVector &v)
{
	if( b84==nullptr )
		return;

	float x = v.x;
	float y = v.y;
	float z = v.z;

	//v.x = x + b84->fTransX;
	//v.y = y + b84->fTransY;
	//v.z = z + b84->fTransZ;

	//rotation...assuming rotate order x->y->z
	//matrix: 
	//|cosYcosZ,    cosZsinXsinY-cosXsinZ,     cosXcosZsinY+sinXsinZ,    Tx|
	//|cosYsinZ,    cosXcosZ+sinXsinYsinZ,     -cosZsinX+cosXsinYsinZ,   Ty|
	//|-sinY,       cosYsinX,                  cosXcosY,                 Tz|
	//|0,           0,                         0,                        1 |
	v.x = cos(b84->fRotY)*cos(b84->fRotZ)*x + (cos(b84->fRotZ)*sin(b84->fRotX)*sin(b84->fRotY)-cos(b84->fRotX)*sin(b84->fRotZ))*y + (cos(b84->fRotX)*cos(b84->fRotZ)*sin(b84->fRotY) + sin(b84->fRotX)*sin(b84->fRotZ))*z + b84->fTransX;
	v.y = cos(b84->fRotY)*sin(b84->fRotZ)*x + (cos(b84->fRotX)*cos(b84->fRotZ)+sin(b84->fRotX)*sin(b84->fRotY)*sin(b84->fRotZ))*y + (-cos(b84->fRotZ)*sin(b84->fRotX)+cos(b84->fRotX)*sin(b84->fRotY)*sin(b84->fRotZ))*z + b84->fTransY;
	v.z = -sin(b84->fRotY)*x + (cos(b84->fRotY)*sin(b84->fRotX))*y + (cos(b84->fRotX)*cos(b84->fRotY))*z + b84->fTransZ;

	//for debug purpose, to find out which MMB have this coordinate
//	if( (floor(v.x)==63 ) && (floor(v.y)==0) && (floor(v.z)==-720) ) {
	if( (v.x>-1889) && (v.x<-1887) && (v.y>-16) && (v.y<-15) && (v.z>1439) && (v.z<1441) ) {
		x = 1;
	}
}

void CDatLoader::pushMMBIndices(SMZBFace f)
{
	SMZBFace n;
	n.i1 = f.i1 + m_lastIndices;
	n.i2 = f.i2 + m_lastIndices;
	n.i3 = f.i3 + m_lastIndices;

	m_vecIndices.push_back(n);
}

void CDatLoader::write_MMB()
{
	char buf[50];
	sprintf_s(buf,50,"mapMeshMMB_%d.txt", m_fileNo);
	ofstream mapStream(buf,std::ofstream::out);
	mapStream << m_vecIndices.size() << endl;
	for(auto it=m_vecIndices.begin(); it!=m_vecIndices.end(); ++it) {
		mapStream << it->i1 << "," << it->i2 << "," << it->i3 << endl;
	}

	mapStream << m_vecVector.size() << endl;
	for(auto it=m_vecVector.begin(); it!=m_vecVector.end(); ++it) {
		mapStream << it->x << "," << it->y << "," << it->z << endl;
	}

	mapStream.close();
}

void CDatLoader::extractImage(char *p, unsigned int len)
{
	char buf[500];
	string str, imgname;
	unsigned int totalsize;
	IMGINFO1 *ii = nullptr;

	totalsize=0;

	ii = (IMGINFO1*)(p+totalsize);
	/*memcpy(buf, ii->ddsType, 4);
	buf[4]=0x00;*/

//	if( strcmp(buf, "3TXD")!=0 )
//		break;

	memcpy(buf,ii->id,16);
	buf[16]=0x00;
	imgname.assign(buf);

//	sprintf_s(buf,12,"0x%X",(0x0000FF & ii->flg));
//	str.assign(buf);
	ofs << "Flg: Ox" << hex << uppercase << (0x0000ff & ii->flg) << nouppercase << dec << " id: " << imgname << " x,y: " << ii->imgx << " " << ii->imgy << " bpp: " << ii->widthbyte << " size: " << ii->size << " block: " << ii->noBlock << endl;



	sprintf_s(buf,500,"DATHEAD:16, IMGINFO Header:%d, Data:%d, Padding:%d", sizeof(IMGINFO1), totalsize, len-sizeof(IMGINFO1)-totalsize);
	str.assign(buf);
	ofs << str << endl << endl;
}

void CDatLoader::extractVertex(char *p, unsigned int len)
{
	char buf[500];
	string str;
	unsigned int totalsize=0;
	DAT2AHeader *pcp=nullptr;

	pcp=(DAT2AHeader *)(p+totalsize);
	sprintf_s(buf,500,"    ver:%d,  nazo:%d,  type:%d,  flip:%d", pcp->ver, pcp->nazo, pcp->type, pcp->flip);
	str.assign(buf);
	ofs << str << endl;

	sprintf_s(buf,500,"    offsetPoly:%d,  PolySuu:%d,  offsetBoneTbl:%d,  BoneTblSuu:%d,  offsetWeight:%d,  WeightSuu:%d,  offsetBone:%d,  BoneSuu:%d",
		pcp->offsetPoly, pcp->PolySuu, pcp->offsetBoneTbl, pcp->BoneTblSuu, pcp->offsetWeight, pcp->WeightSuu, pcp->offsetBone, pcp->BoneSuu);
	str.assign(buf);
	ofs << str << endl;

	sprintf_s(buf,500,"    offsetVertex:%d,  VertexSuu:%d,  offsetPolyLoad:%d,  PolyLoadSuu:%d,  PolyLodVtx0Suu:%d,  PolyLodVtx1Suu:%d, offsetPolyLod2:%d,  PolyLod2Suu:%d",
		pcp->offsetVertex, pcp->VertexSuu, pcp->offsetPolyLoad, pcp->PolyLoadSuu, pcp->PolyLodVtx0Suu, pcp->PolyLodVtx1Suu, pcp->offsetPolyLod2, pcp->PolyLod2Suu);
	str.assign(buf);
	ofs << str << endl;

	sprintf_s(buf,500,"    nazo1:%d,  nazo2:%d,  nazo3:%d,  nazo4:%d", pcp->nazo1, pcp->nazo2, pcp->nazo3, pcp->nazo4);
	str.assign(buf);
	ofs << str << endl;

	if( pcp->offsetPolyLod2!=0 ) {
		totalsize += (pcp->offsetPolyLod2*2 + pcp->PolyLod2Suu*2);
	}
	else {
		totalsize += (pcp->offsetPolyLoad*2 + pcp->PolyLoadSuu*2);
	}
	
//NOTE: all offset is wrt DAT2AHeader (inclusive).  eg. DAT2AHeader + offsetWeight 
	sprintf_s(buf,500,"DATHEAD:16, DAT2AHeader:%d, Data:%d, Padding:%d",sizeof(DAT2AHeader), totalsize-64, len-totalsize);
	str.assign(buf);
	ofs << str << endl << endl;

	sprintf_s(buf,500,"mesh%d_%d.txt", m_fileNo, cur_mesh);
	str.assign(buf);
	ofstream blender(str.c_str(), std::ofstream::out);
	if( !blender ) {
		cout << "unable to open outfile" << endl;
		return;
	}
	//whether bone index is access indirectly from boneTable
	bool isIndirect=(pcp->type&0x80);

	m_vecFace.clear();
	//extract poly
	TEXLIST *uvf=nullptr;
	TEXLIST2 *uvf2=nullptr;
	int num8010=0,num8000=0,num5453=0,num4345=0,num0054=0,num0043=0;
	totalsize = pcp->offsetPoly*2;
	while(1) {
		int wf = (int)*(WORD*)(p+totalsize  );
		int ws = (int)*(WORD*)(p+totalsize+2);
        if( 0x8010 == (wf&0x80F0) ) { 
            dat8000 *dat = (dat8000*)(p+totalsize+2);
//            p+=0x2e;		//46 byte
			totalsize += 46;
			num8010++;
            continue;  //不明
		} 
		else if ( 0x8000 == (wf&0x80F0) ) { //テクスチャの変更
			num8000++;
			memcpy(buf,p+totalsize+2,16);
			buf[16]=0x00;
			str.assign(buf);
			ofs << "textureID: " << str << endl;
//			p+=0x12;	//18 byte
			totalsize += 18;
		} 
		else {
			if( 0x5453 == wf ) {/*ST*/  //StripTriangle
				num5453++;
				ofs << "[StripTriangle] Total face: " << ws << endl;
				//first 3 vertex
				uvf = (TEXLIST*)(p+totalsize);
				sprintf_s(buf,500,"i1:%d i2:%d i3:%d  uv1:(%f,%f)  uv2:(%f,%f)  uv3:(%f,%f)",uvf->i1,uvf->i2,uvf->i3,uvf->u1,uvf->v1,uvf->u2,uvf->v2,uvf->u3,uvf->v3);
				str.assign(buf);
				ofs << str << endl;
				totalsize += 34;
				//subsequence add 1 vertex
				for(int k=0; k<ws-1; ++k) {
					uvf2 = (TEXLIST2*)(p+totalsize);
					sprintf_s(buf,500,"i1:%d uv:(%f,%f)", uvf2->i, uvf2->u, uvf2->v);
					str.assign(buf);
					ofs << str << endl;
					totalsize += 10;
				}
//				p+=ws*10  + 0x18;	== (ws-1)*10 + 24 + 10 [30(uvf)+4(header)]
				continue;
			} 
			else if( 0x4353 == wf ) { /*SC*/
				totalsize+=ws*20  + 0x0C;
				num4345++;
				continue; 
			} 
			else if( 0x0043 == wf ) { /*C*/
				totalsize+=ws*10 + 0x4;
				num0043++;
				continue; 
			} 
			else if( 0x0054 == wf ) {  /*T*/ //TriangleList
				num0054++;
//				blender << ws << endl;
				ofs << "[Triangle List] Total face: " << ws << endl;
				totalsize += 0x4;				//5400 8000
				for( int k=0; k<ws; k++ ) {
					uvf = (TEXLIST*)(p+totalsize);
					sprintf_s(buf,500,"i1:%d i2:%d i3:%d  uv1:(%f,%f)  uv2:(%f,%f)  uv3:(%f,%f)",uvf->i1,uvf->i2,uvf->i3,uvf->u1,uvf->v1,uvf->u2,uvf->v2,uvf->u3,uvf->v3);
					str.assign(buf);
					m_vecFace.push_back(*uvf);
//					blender << uvf->i1 << "," << uvf->i2 << "," << uvf->i3 << endl;
					ofs << str << endl;
					totalsize+=30;				//sizeof uvface
				}				
//				p+=ws*30  + 0x4;				
				continue;
			} 
			else
				break;
		}
	}

	blender << m_vecFace.size() << endl;
	for(auto it=m_vecFace.begin(); it!=m_vecFace.end(); ++it) {
		blender << it->i1 << "," << it->i2 << "," << it->i3 << endl;
	}

	//extract bone tbl
	m_vecBoneIndexTbl.clear();
	ofs << "Bone Tbl:" << pcp->BoneTblSuu << endl;
	totalsize = pcp->offsetBoneTbl*2;
	WORD *tblindex;
	for(int k=0; k<pcp->BoneTblSuu; ++k) {
		tblindex = (WORD*)(p+totalsize);
		ofs << *tblindex << endl;
		m_vecBoneIndexTbl.push_back(*tblindex);
		totalsize +=2;
	}

	//extract bone
	BONE3 *b3 = nullptr;
	int noB = pcp->BoneSuu/2;		//bone is 4byte, therefore need to divide 2
	totalsize = pcp->offsetWeight*2;
	b3 = (BONE3*)(p+totalsize);
	int noB1 = (int)b3->low;
	int noB2 = (int)b3->high;
	int tblidxL, flgL, tblidxH, flgH;
	BONE4 b4;
	m_vecBone3.clear();

	//ofs << "Bone: " << noB << endl;
	//totalsize = pcp->offsetBone*2;
	//for(int k=0; k<noB1; ++k) {
	//	b3 = (BONE3*)(p+totalsize);

	//	tblidxL=(b3->low)&0x7f;
	//	flgL = (b3->low>>14)&0x3;
	//	if(pcp->flip) {
	//		b3->low>>=7;
	//		tblidxL=(b3->low)&0x7f;
	//	}
	//	tblidxL = (pcp->type&0x80)? vecBoneIndexTbl[tblidxL]:tblidxL;
	//	sprintf_s(buf,500,"%d) tblidxL:%d  flgL:%d", k, tblidxL, flgL);
	//	str.assign(buf);
	//	ofs << str << endl;

	//		b4.low = tblidxL;
	//		b4.lflg = flgL;
	//		b4.high = 0;
	//		b4.hflg = 0;
	//		vecBone.push_back(b4);

	//	vecBoneIndexL.push_back(tblidxL);
	//	vecBoneIndexH.push_back(0);

	//	totalsize +=4;
	//}
	//for(int k=0; k<noB2; ++k) {
	//	b3 = (BONE3*)(p+totalsize);
	//	tblidxL=(b3->low)&0x7f;
	//	flgL = (b3->low>>14)&0x3;
	//	if(pcp->flip) {
	//		b3->low>>=7;
	//		tblidxL=(b3->low)&0x7f;
	//	}
	//	tblidxL = (pcp->type&0x80)? vecBoneIndexTbl[tblidxL]:tblidxL;
	//	vecBoneIndexL.push_back(tblidxL);

	//	tblidxH=(b3->high)&0x7f;
	//	flgH = (b3->high>>14)&0x3;
	//	if(pcp->flip) {
	//		b3->high>>=7;
	//		tblidxH=(b3->high)&0x7f;
	//	}
	//	tblidxH = (pcp->type&0x80)? vecBoneIndexTbl[tblidxH]:tblidxH;
	//	vecBoneIndexH.push_back(tblidxH);

	//		b4.low = tblidxL;
	//		b4.lflg = flgL;
	//		b4.high = tblidxH;
	//		b4.hflg = flgH;
	//		vecBone.push_back(b4);

	//	sprintf_s(buf,500,"%d) tblidxL:%d  flgL:%d,  tblidxH:%d  flgH:%d", k+noB1, tblidxL, flgL, tblidxH, flgH );
	//	str.assign(buf);
	//	ofs << str << endl;

	//	totalsize +=4;
	//}

	ofs << "Bone: " << noB << endl;
	totalsize = pcp->offsetBone*2;
	for(int k=0; k<noB1; ++k) {
		b3 = (BONE3*)(p+totalsize);

		if(pcp->type&0x80) {
			sprintf_s(buf,500,"%d) leftL:%d  rightL:%d  flgL:%d", k, m_vecBoneIndexTbl[b3->leftL], ((pcp->flip)? m_vecBoneIndexTbl[b3->rightL]:0), b3->flgL);
		}
		else {
			sprintf_s(buf,500,"%d) leftL:%d  rightL:%d  flgL:%d", k, b3->leftL, ((pcp->flip)? b3->rightL:0), b3->flgL);
		}
		str.assign(buf);
		ofs << str << endl;

		m_vecBone3.push_back(*b3);
		totalsize +=4;
	}
	for(int k=0; k<noB2; ++k) {
		b3 = (BONE3*)(p+totalsize);
		if(pcp->type&0x80) {
			sprintf_s(buf,500,"%d) leftL:%d  rightL:%d  flgL:%d   leftH:%d  rightH:%d  flgH:%d", k+noB1, m_vecBoneIndexTbl[b3->leftL], ((pcp->flip)? m_vecBoneIndexTbl[b3->rightL]:0), b3->flgL,
				m_vecBoneIndexTbl[b3->leftH], ((pcp->flip)? m_vecBoneIndexTbl[b3->rightH]:0), b3->flgH);
		}
		else {
			sprintf_s(buf,500,"%d) leftL:%d  rightL:%d  flgL:%d   leftH:%d  rightH:%d  flgH:%d", k+noB1, b3->leftL, ((pcp->flip)? b3->rightL:0), b3->flgL,
				b3->leftH, ((pcp->flip)? b3->rightH:0), b3->flgH);
		}
		
		str.assign(buf);
		ofs << str << endl;

		m_vecBone3.push_back(*b3);
		totalsize +=4;
	}

	//IMPORTANT if (pcp->flip) then need to draw twice, HOW?????
	WORD doflip=0;

	//extract vertex
	MODELVERTEX1 *mv1=nullptr;
	MODELVERTEX2 *mv2=nullptr;
	blender << noB << endl;
	ofs << "Vertex: " << noB << endl;
	totalsize = pcp->offsetVertex*2;
	for(int k=0; k<noB1; ++k) {
		mv1 = (MODELVERTEX1*)(p+totalsize);
		sprintf_s(buf,500,"%d) xyz: %f %f %f,  hxhyhz: %f %f %f",k, mv1->x, mv1->y, mv1->z, mv1->hx, mv1->hy, mv1->hz);
		str.assign(buf);
		ofs << str << endl;

		mv1=transformVertexMV1(k, mv1, doflip, isIndirect);
		blender << mv1->x << "," << mv1->y << "," << mv1->z << endl;
		totalsize += sizeof(MODELVERTEX1);
	}

	for(int k=0; k<noB2; ++k) {
		mv2 = (MODELVERTEX2*)(p+totalsize);
		sprintf_s(buf,500,"%d) xyzw1: %f %f %f %f,  hxhyhz: %f %f %f,  xyzw2: %f %f %f %f,  hxhyhz2: %f %f %f", k+noB1,
			mv2->x1,mv2->y1,mv2->z1,mv2->w1, mv2->hx1,mv2->hy1,mv2->hz1, mv2->x2,mv2->y2,mv2->z2,mv2->w2, mv2->hx2,mv2->hy2,mv2->hz2);
		str.assign(buf);
		ofs << str << endl;

		mv2=transformVertexMV2(k+noB1, mv2, doflip, isIndirect);
//		blender << (mv2->x1*mv2->w1+mv2->x2*mv2->w2) << "," << (mv2->y1*mv2->w1+mv2->y2*mv2->w2) << "," << (mv2->z1*mv2->w1+mv2->z2*mv2->w2) << endl;
//		blender << (mv2->x1*mv2->w1-mv2->x2*mv2->w2) << "," << (mv2->y1*mv2->w1-mv2->y2*mv2->w2) << "," << (mv2->z1*mv2->w1-mv2->z2*mv2->w2) << endl;
		blender << (mv2->x1+mv2->x2) << "," << (mv2->y1+mv2->y2) << "," << (mv2->z1+mv2->z2) << endl;
//		blender << (mv2->x1/mv2->w1+mv2->x2/mv2->w2) << "," << (mv2->y1/mv2->w1+mv2->y2/mv2->w2) << "," << (mv2->z1/mv2->w1+mv2->z2/mv2->w2) << endl;
		totalsize += sizeof(MODELVERTEX2);
	}

	m_vecFace.clear();
	m_vecBoneIndexTbl.clear();
	m_vecBone3.clear();
	blender.close();
	cur_mesh++;
}

MODELVERTEX1* CDatLoader::transformVertexMV1(int k, MODELVERTEX1* mv1, WORD flip, bool isIndirect)
{
	//get the bone matrix
	int index, flg;
	BONE3 b3 = m_vecBone3[k];
	if( isIndirect )
		index = ((flip==0)? m_vecBoneIndexTbl[b3.leftL] : m_vecBoneIndexTbl[b3.rightL]);
	else
		index = ((flip==0)? b3.leftL : b3.rightL);

	flg = b3.flgL;
//	int index = vecBoneIndexL[k];
	TMatrix44 M = m_vecBoneMatrix[index];

//	b4 = vecBone[k];
	//if( b4.lflg==1 ) mv1->x = 0.0-mv1->x;
	//if( b4.lflg==2 ) mv1->y = 0.0-mv1->y;
	//if( b4.lflg==3 ) mv1->z = 0.0-mv1->z;
	if( flip!=0 ) {
		if( flg==1 ) M *= matrixMirrorX;
		if( flg==2 ) M *= matrixMirrorY; 
		if( flg==3 ) M *= matrixMirrorZ;
	}
	//multiply with vertex
	M.multiplyByVector3(mv1->x, mv1->y, mv1->z, 1.0f);
	return mv1;
}

MODELVERTEX2* CDatLoader::transformVertexMV2(int k, MODELVERTEX2* mv2, WORD flip, bool isIndirect)
{
//	BONE4 b4;
//	TMatrix44 ML = m_vecBoneMatrix[vecBoneIndexL[k]];
//	TMatrix44 MH = m_vecBoneMatrix[vecBoneIndexH[k]];
	int indexL, indexH, flgL, flgH;
	BONE3 b3 = m_vecBone3[k];

	if( isIndirect ) {
		indexL = ((flip==0)? m_vecBoneIndexTbl[b3.leftL] : m_vecBoneIndexTbl[b3.rightL]);
		indexH = ((flip==0)? m_vecBoneIndexTbl[b3.leftH] : m_vecBoneIndexTbl[b3.rightH]);
	}
	else {
		indexL = ((flip==0)? b3.leftL : b3.rightL);
		indexH = ((flip==0)? b3.leftH : b3.rightH);
	}

	flgL = b3.flgL;
	flgH = b3.flgH;
	TMatrix44 ML = m_vecBoneMatrix[indexL];
	TMatrix44 MH = m_vecBoneMatrix[indexH];

//	b4 = vecBone[k];
	//if( b4.lflg==1 ) mv2->x1 = 0.0-mv2->x1;
	//if( b4.lflg==2 ) mv2->y1 = 0.0-mv2->y1;
	//if( b4.lflg==3 ) mv2->z1 = 0.0-mv2->z1;
	//if( b4.hflg==1 ) mv2->x2 = 0.0-mv2->x2;
	//if( b4.hflg==2 ) mv2->y2 = 0.0-mv2->y2;
	//if( b4.hflg==3 ) mv2->z2 = 0.0-mv2->z2;

	if( flip!=0 ) {
		if( flgL==1 ) ML *= matrixMirrorX;
		if( flgL==2 ) ML *= matrixMirrorY; 
		if( flgL==3 ) ML *= matrixMirrorZ;
		if( flgH==1 ) MH *= matrixMirrorX;
		if( flgH==2 ) MH *= matrixMirrorY;
		if( flgH==3 ) MH *= matrixMirrorZ;
	}
	ML.multiplyByVector3(mv2->x1, mv2->y1, mv2->z1, mv2->w1);
	MH.multiplyByVector3(mv2->x2, mv2->y2, mv2->z2, mv2->w2);
	return mv2;
}

void CDatLoader::extractAnimation(char *p, unsigned int len)
{
	float angle, axis_x, axis_y, axis_z, tr_x, tr_y, tr_z;
	char buf[500];
	string str;
	unsigned int totalsize=0;
	DAT2BHeader2 *pcp=nullptr;
	DAT2B *p2b=nullptr;
	
	pcp = (DAT2BHeader2*)p;
	DAT2BHeader *dat2b = (DAT2BHeader*)p;

	sprintf_s(buf,500,"    ver:%d, unk:%d, noElem:%d, frame:%d, speed:%f", pcp->ver, pcp->nazo, pcp->element, pcp->frame, pcp->speed);
	str.assign(buf);
	ofs << str << endl;

	totalsize += sizeof(DAT2BHeader2);
	for(int i=0; i<pcp->element; ++i) {
		p2b = (DAT2B*)(p+totalsize);
		//sprintf_s(buf,500,"    BoneNo:%d, %d %d %d %d %f %f %f %f",p2b->no,p2b->idx_qtx,p2b->idx_qty,p2b->idx_qtz,p2b->idx_qtw,p2b->qtx,p2b->qty,p2b->qtz,p2b->qtw);
		//str.assign(buf);
		//ofs << str << endl;

		//sprintf_s(buf,500,"    (Trans:%d %d %d %f %f %f),  (Scale:%d %d %d %f %f %f)",p2b->idx_tx,p2b->idx_ty,p2b->idx_tz,p2b->tx,p2b->ty,p2b->tz,
		//	p2b->idx_sx,p2b->idx_sy,p2b->idx_sz,p2b->sx,p2b->sy,p2b->sz);
		//str.assign(buf);
		//ofs << str << endl;

		//display its content
		quaternion q(p2b->qtx,p2b->qty,p2b->qtz,p2b->qtw);
		q.toAxix(angle, axis_x, axis_y, axis_z);
		if( p2b->idx_qtx & 0x80000000 )
			sprintf_s(buf,500,"    BoneNo:%d,  NO TRANSFORM  angle:%f,   axis: (%f, %f, %f)", p2b->no, angle, axis_x, axis_y, axis_z);
		else
			sprintf_s(buf,500,"    BoneNo:%d,  [%d %d %d %d]  angle:%f,   axis: (%f, %f, %f)", p2b->no, p2b->idx_qtx,p2b->idx_qty,p2b->idx_qtz,p2b->idx_qtw, angle, axis_x, axis_y, axis_z);
		str.assign(buf);
		ofs << str << endl;

		sprintf_s(buf,500,"    (Trans:%d %d %d %f %f %f),  (Scale:%d %d %d %f %f %f)",p2b->idx_tx,p2b->idx_ty,p2b->idx_tz,p2b->tx,p2b->ty,p2b->tz,
			p2b->idx_sx,p2b->idx_sy,p2b->idx_sz,p2b->sx,p2b->sy,p2b->sz);
		str.assign(buf);
		ofs << str << endl;
		if( !(p2b->idx_qtx & 0x80000000) ) {
			//extra transform
			for(int j=0; j<pcp->frame; ++j) {
				q.X = (p2b->idx_qtx)? dat2b->f[p2b->idx_qtx+j] : p2b->qtx;
				q.Y = (p2b->idx_qty)? dat2b->f[p2b->idx_qty+j] : p2b->qty;
				q.Z = (p2b->idx_qtz)? dat2b->f[p2b->idx_qtz+j] : p2b->qtz;
				q.W = (p2b->idx_qtw)? dat2b->f[p2b->idx_qtw+j] : p2b->qtw;
				q.toAxix(angle, axis_x, axis_y, axis_z);
				//translation
				tr_x=tr_y=tr_z=0.0f;
				if( p2b->idx_tx >0 )
					tr_x = dat2b->f[p2b->idx_tx+j];
				if( p2b->idx_ty >0 )
					tr_y = dat2b->f[p2b->idx_ty+j];
				if( p2b->idx_tz >0 )
					tr_z = dat2b->f[p2b->idx_tz+j];

				sprintf_s(buf,500,"       frame:%d)  angle:%f,  axis: (%f, %f, %f),   translate: (%f, %f, %f)", j, angle, axis_x, axis_y, axis_z, tr_x, tr_y, tr_z);
				str.assign(buf);
				ofs << str << endl;
			}
		}
		totalsize += sizeof(DAT2B);
	}
	unsigned int rem = len-sizeof(DAT2BHeader2);
	sprintf_s(buf,500,"size:%d, sizeFloat:%d, arrFloat:%d", totalsize,rem,rem/4);
	str.assign(buf);
	ofs << str << endl << endl;
}

void CDatLoader::extractBone(char *p, unsigned int len)
{
	char buf[500];
	string str;
	unsigned int totalsize=0,parent=0;
	BONE *bn=nullptr;
	float angle, axis_x, axis_y, axis_z;
	int unk = (int)*(short*)p;
	int noBone = (int)*(short*)(p+0x2);
	m_vecBoneMatrix.clear();

	vector<BONE*> vecBone;
	sprintf_s(buf,50,"bone%d.txt", m_fileNo);
	ofstream boneStream(buf,std::ofstream::out);

	sprintf_s(buf,500,"    unk: %d,  noBone:%d ",unk, noBone);
	str.assign(buf);
	ofs << str << endl;
	boneStream << str << endl;

	totalsize +=4;
	for(int i=0;i<noBone; ++i) {
		bn = (BONE*)(p+totalsize);
		if( i==0 )
			bn->parent=255;

		sprintf_s(buf,500,"      BoneNo:%d) parent:%d,  term:%d,  q1:%f, q2:%f, q3:%f, q4:%f  x:%f,  y:%f,  z:%f",i,
			bn->parent, bn->term, bn->i, bn->j, bn->k, bn->w, bn->x, bn->y, bn->z);

		str.assign(buf);
		ofs << str << endl;

		quaternion q(bn->i, bn->j, bn->k, bn->w);
		q.toAxix(angle, axis_x, axis_y, axis_z);
		sprintf_s(buf,500,"      angle: %f   axis: (%f, %f, %f)", angle, axis_x, axis_y, axis_z);
		str.assign(buf);
		ofs << str << endl;

		TMatrix44 M, PM;
		q.getMatrix(M);
		M.setTranslation(bn->x, bn->y, bn->z);
		m_vecBoneMatrix.push_back(M);

		//calculate the absolute position of the bone wrt root
		vecBone.push_back(bn);
		
		if( bn->parent!=255 ) {			
			PM = m_vecBoneMatrix[ bn->parent];
			M *= PM;					//local * Parent
			m_vecBoneMatrix[i] = M;		//update
		}
		boneStream << i <<") parent:" << (short)bn->parent << "   x: " << M(3,0) << " y: " << M(3,1) << " z: " << M(3,2) << " (xyz): [" << bn->x << ", " << bn->y << ", " << bn->z << "]" << endl;
//		boneStream << M(3,0) << "," << M(3,1) << "," << M(3,2) << endl;
		totalsize +=sizeof(BONE);
	}
	sprintf_s(buf,500,"size:%d", totalsize);
	str.assign(buf);
	ofs << str << endl << endl;

	WORD *sizeb2 = (WORD*)(p+totalsize);
	boneStream << "Bone2: " << *sizeb2 << endl;
	totalsize += 4;		//size(2) + FFFF(2)
	BONE2 *b2 = nullptr;
	for(int i=0; i<*sizeb2; ++i) {
		b2 = (BONE2*)(p+totalsize);
		boneStream << i << ") no: " << b2->no << " " << b2->a << ", " << b2->b << ", " << b2->c << ", " << b2->d << ", " << b2->e << ", " << b2->f << endl;
		totalsize += sizeof(BONE2);
	}

	float *pI = nullptr;
	boneStream << "Bone3: " << endl;
	for(int i=0; i<27; ++i) {
		pI = (float*)(p+totalsize);
		boneStream << i << ") " << *pI << endl;
		totalsize +=4;
	}
	boneStream.close();
}
