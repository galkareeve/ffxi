#include "StdAfx.h"
#include "DatLoader.h"
#include "quaternion.h"
#include <iomanip>

#define WRITE_RAW_DECODE_MZB	0
#define WRITE_RAW_DECODE_MMB	0
#define PARTIAL_MMB	1
#define COUNT_W		25

#define PARSE_MZB	1
#define PARSE_MMB	1
#define PARSE_BONE	1
#define PARSE_ANIM	1
#define PARSE_VERTEX	1
#define PARSE_IMG	1

#define PARSE_UNK 0
#define PARSE_UNK_BONE			0

#define OUTPUT_CHAR_MESH_BLENDER	0
#define OUTPUT_COLLISION_BLENDER	0
#define OUTPUT_MMB_BLENDER		0

#define	WRITE_MMB_RGB		1

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

	if (WRITE_MMB_RGB) {
		sprintf_s(buf, 50, "%d_MMBRGB.txt", fno);
		str.assign(buf);
		ofs2.open(str.c_str(), std::ofstream::out);
		if (!ofs) {
			cout << "unable to open MMB_RGB" << endl;
			return false;
		}
	}
	bool isValidMZB=false;
	unsigned int len,mzb=0,mmb=0,Z5=0,Z7=0,Z61=0,Z25=0,Z31=0,ZUNK=0;
	DATHEAD hd;
	ofstream datout;
		char *p, *start=f.FistData(&hd);
		char iname[5];
		iname[4]=0x00;
		for( p=f.FistData(&hd); p; p=f.NextData(&hd) )
		{
			int type = (int)hd.type;
			memcpy(iname,p,4);
			
			len = (hd.next&0x7ffff)*16;
			switch (type)
			{
				case 0x1c:  //28 MZB
						ofs << mzb << ")  offset: " << p-start << " MZB " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
						if(PARSE_MZB) {
							cout << "parsing MZB" << endl;
							isValidMZB = decode_mzb((BYTE*)(p+16), len);
							if(!isValidMZB)
								ofs << " Unable to decode mzb: " << fno << endl;

							extractMZB(p+sizeof(DATHEAD), len-sizeof(DATHEAD), mzb, isValidMZB);						
	
						mzb++;
					}
				break;
				case 0x2e:  //46 MMB
	
						ofs << mmb << ")  offset: " << p-start << " MMB " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
						if (WRITE_MMB_RGB)
							ofs2 << mmb << ") ";

						if(PARSE_MMB) {
							cout << "parsing MMB" << endl;
							if(decode_mmb((BYTE*)(p+16), len))
								extractMMB(p+sizeof(DATHEAD), len-sizeof(DATHEAD), mmb);
							else
								ofs << "  Unable to decode mmb: " << mmb << endl;

						mmb++;
					}
				break;
				case 0x20:  //32 IMG

						ofs << "offset: " << p-start << " IMG " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
						if(PARSE_IMG) {
							cout << "parsing Image" << endl;
							extractImage(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
						}
				break;
				case 0x29:	//41 Bone
						ofs << "offset: " << p-start << " Bone " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
						if(PARSE_BONE) {
							cout << "parsing Bone" << endl;
							extractBone(p+sizeof(DATHEAD),len-sizeof(DATHEAD));
						}
				break;
				case 0x2B:	//43 animation
						ofs << "offset: " << p-start << " Animation " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
						if(PARSE_ANIM) {
							cout << "parsing Animation" << endl;
							extractAnimation(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
						}
				break;
				case 0x2a:	//42 vertex
						ofs << "offset: " << p-start << " Vertex " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
						if(PARSE_VERTEX) {
							cout << "parsing Vertex" << endl;
							extractVertex(p+sizeof(DATHEAD),len-sizeof(DATHEAD));
						}
				break;
				case 0x5:		//5
						ofs << "offset: " << p-start << " unknown " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
						if(PARSE_UNK && COUNT_W==5) {
							sprintf_s(buf,50,"ZDat5_%d_%s", Z5++,iname);
							datout.open(buf, std::ofstream::out| std::ofstream::binary);
							datout.write(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
							datout.close();
						}
					break;
				case 0x7:		//7 schedule animation??
						ofs << "offset: " << p-start << " schedule " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
						if(PARSE_UNK && COUNT_W==7) {
							sprintf_s(buf,50,"ZDat7_%d_%s", Z7++,iname);
							datout.open(buf, std::ofstream::out| std::ofstream::binary);
							datout.write(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
							datout.close();
						}
					break;
				case 0x19:	//25 
						ofs << "offset: " << p-start << " unknown " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
						if(PARSE_UNK && COUNT_W==25) {
							sprintf_s(buf,50,"ZDat25_%d_%s", Z25++,iname);
							datout.open(buf, std::ofstream::out| std::ofstream::binary);
							datout.write(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
							datout.close();
						}
					break;
				case 0x1F:	//31 effect data
					ofs << "offset: " << p - start << " unknown " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
					if (PARSE_UNK && COUNT_W == 31) {
						sprintf_s(buf, 50, "ZDat31_%d_%s", Z31++, iname);
						datout.open(buf, std::ofstream::out | std::ofstream::binary);
						datout.write(p + sizeof(DATHEAD), len - sizeof(DATHEAD));
						datout.close();
					}
					break;
				case 0x3D:	//61 animation subcategory???
						ofs << "offset: " << p-start << " category " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
						if(PARSE_UNK && COUNT_W==61) {
							sprintf_s(buf,50,"ZDat61_%d_%s", Z61++,iname);
							datout.open(buf, std::ofstream::out| std::ofstream::binary);
							datout.write(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
							datout.close();
						}
					break;

				default:
						ofs << "offset: " << p-start << " unknown " << iname << " type: " << hd.type << " shadow: " << hd.is_shadow << " extract: " << hd.is_extracted << " virtual: " << hd.is_virtual << " ver: " << hd.ver_num << " size: " << len << endl;
						if(PARSE_UNK && COUNT_W==type) {
							sprintf_s(buf,50,"ZDatUnk_%d_%s", ZUNK++,iname);
							datout.open(buf, std::ofstream::out| std::ofstream::binary);
							datout.write(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
							datout.close();
						}
				break;
			}
			ofs << "offset segment: " << ((p - start) + len) << std::endl;
		}


	if( OUTPUT_MMB_BLENDER) {
		write_MMB();
	}

	cout << "Done parsing dat" << endl;
	f.Free();
	ofs.close();
	if (WRITE_MMB_RGB)
		ofs2.close();
	return true;
}

void CDatLoader::extractMZB(char *p, unsigned int len, unsigned int count, bool isValid)
{
	if(WRITE_RAW_DECODE_MZB) {
		char *buf = new char[len];
		memcpy(buf,p,len);

		char fileName[30];
		sprintf_s(fileName,30,"MZB%d_%d", count, m_fileNo);
		ofstream output(fileName, std::ofstream::out | std::ofstream::binary);
		for(int i=0; i<len; ++i) {
			output << hex << buf[i] ;
		}
		output.close();

		delete []buf;
	}

	if(!isValid)
		return;

	char buf[17];
	buf[16]=0x00;
	unsigned int totalsize=0,num,val,size;
	SMZBHeader *mzbh1 = (SMZBHeader*)p;
//	mzbh1->totalRecord100 &=0xffffff;
	memcpy(buf,mzbh1->id,4);
	buf[4]=0x00;
//	ofs << "id: " << mzbh1->id[0] << mzbh1->id[1] << mzbh1->id[2] << mzbh1->id[3] << " Total100: " << mzbh1->totalRecord100 << " offset Collision: " << mzbh1->offsetHeader2 << " unk1: " << mzbh1->unk1 << " unk2: " << mzbh1->unk2 << " unk3: " << mzbh1->unk3 << " offsetEnd: " << mzbh1->offsetEnd << " unk4: " << mzbh1->unk4 << " unk5: " << mzbh1->unk5 << endl;
	ofs << "id: " << buf << " Total100: " << mzbh1->totalRecord100 << " R100Flg: " << mzbh1->R100Flag << " offsetCollision: " << mzbh1->offsetHeader2 << " d: " << mzbh1->d1 << "," << mzbh1->d2 << "," << mzbh1->d3 << "," << mzbh1->d4 << " offsetCubetree: " << mzbh1->offsetCubetree << " offsetEndRecord100: " << mzbh1->offsetEndRecord100 << " offsetEndCubetree: " << mzbh1->offsetEndCubetree << " unk5: " << mzbh1->unk5 << endl;
	totalsize += sizeof(SMZBHeader);

	SMZBBlock100 *oj=nullptr, ob;
	SMZBBlock84 *b84=nullptr;
	SMZBBlock92b *b92=nullptr;
//	int noj = mzbh1->totalRecord100 & 0xffffff;
	int noj = mzbh1->totalRecord100;

	if( (noj*sizeof(SMZBBlock84) + sizeof(SMZBHeader))==mzbh1->offsetEndRecord100 ) {
		//for MMB object
		b84 = (SMZBBlock84*)(p+totalsize);
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
			memcpy(buf, b84->id,16);
			ofs << "   " << i << ")  id: " << buf << " Trans: (" << b84->fTransX << ", " << b84->fTransY << ", " << b84->fTransZ << ")  Rot: (" << b84->fRotX << ", " << b84->fRotY << ", " << b84->fRotZ << ")" << endl;
			ofs << "    Scale: (" << b84->fScaleX << ", " << b84->fScaleY << ", " << b84->fScaleZ << ")  [" << b84->fa << ", " << b84->fb << ", " << b84->fc << ", " << b84->fd << "]  [" << b84->i1 << ", " << b84->i2 << ", " << b84->i3 << ", " << b84->i4 << "]" << endl;
			b84++;
		}
		totalsize += (noj*sizeof(SMZBBlock84));
	}
	else if((noj*sizeof(SMZBBlock100) + sizeof(SMZBHeader))==mzbh1->offsetEndRecord100) {
		//block100, for the MZB object
		oj =  (SMZBBlock100*)(p+totalsize);
		noj = mzbh1->totalRecord100;
		ofs << "Block100: " << noj << endl;
		for(int i=0; i<noj; ++i) {
			m_vecOBJInfo.push_back(*oj);
			memcpy(buf, oj->id, 16);
			ofs << "   " << i << ")  id: " << buf << " Trans: (" << oj->fTransX << ", " << oj->fTransY << ", " << oj->fTransZ << ")  Rot: (" << oj->fRotX << ", " << oj->fRotY << ", " << oj->fRotZ << ")" << endl;
		//	ofs << "     Scale: (" << oj->fScaleX << ", " << oj->fScaleY << ", " << oj->fScaleZ << ")  [" << hex << uppercase << oj->fa << ", " << oj->fb << ", " << oj->fc << ", " << oj->fd << ", " << oj->fe << ", " << oj->ff << ", " << oj->fg << ", " << oj->fh << ", " << oj->fi << ", " << oj->fj << ", " << oj->fk << ", " << oj->fl << "]" << nouppercase << dec << endl;
			ofs << "     Scale: (" << oj->fScaleX << ", " << oj->fScaleY << ", " << oj->fScaleZ << ")  [" << oj->fa << ", " << oj->fb << ", " << oj->fc << ", " << oj->fd << ", " << hex << uppercase << oj->fe << ", " << oj->ff << ", " << oj->fg << ", " << oj->fh << ", " << nouppercase << dec << oj->fi << ", " << oj->fj << ", " << oj->fk << ", " << oj->fl << "]" << endl;
		//ofs << "     Scale: (" << oj->fScaleX << ", " << oj->fScaleY << ", " << oj->fScaleZ << ")  [" << oj->fa << ", " << oj->fb << ", " << oj->fc << ", " << oj->fd << ", " << oj->fe << ", " << oj->ff << ", " << oj->fg << ", " << oj->fh << ", " << oj->fi << ", " << oj->fj << ", " << oj->fk << ", " << oj->fl << "]" << endl;
			oj++;
		}
		totalsize += (noj*sizeof(SMZBBlock100));
	}
	else if((noj*sizeof(SMZBBlock92b) + sizeof(SMZBHeader))==mzbh1->offsetEndRecord100) {
		b92 = (SMZBBlock92b*)(p+totalsize);
		ofs << "Block92: " << noj << endl;
		for(int i=0; i<noj; ++i ) {
			memcpy(&ob.id, b92->id, 16);
			ob.fTransX=b92->fTransX;
			ob.fTransY=b92->fTransY;
			ob.fTransZ=b92->fTransZ;
			ob.fRotX=b92->fRotX;
			ob.fRotY=b92->fRotY;
			ob.fRotZ=b92->fRotZ;
			ob.fScaleX=b92->fScaleX;
			ob.fScaleY=b92->fScaleY;
			ob.fScaleZ=b92->fScaleZ;
			ob.fa=b92->fa;
			ob.fb=b92->fb;
			ob.fc=b92->fc;
			ob.fd=b92->fd;
			ob.fe=b92->i1;
			ob.ff=b92->i2;
			ob.fg=b92->i3;
			ob.fh=b92->i4;
			ob.fi=b92->i5;
			ob.fj=b92->i6;
			ob.fk=0;
			ob.fl=0;
			m_vecOBJInfo.push_back(ob);
			memcpy(buf, b92->id,16);
			ofs << "   " << i << ")  id: " << buf << " Trans: (" << b92->fTransX << ", " << b92->fTransY << ", " << b92->fTransZ << ")  Rot: (" << b92->fRotX << ", " << b92->fRotY << ", " << b92->fRotZ << ")" << endl;
			ofs << "    Scale: (" << b92->fScaleX << ", " << b92->fScaleY << ", " << b92->fScaleZ << ")  [" << b92->fa << ", " << b92->fb << ", " << b92->fc << ", " << b92->fd << "]  [" << b92->i1 << ", " << b92->i2 << ", " << b92->i3 << ", " << b92->i4 << ", " << b92->i5 << ", " << b92->i6 << "]" << endl;
			b92++;
		}
		totalsize += (noj*sizeof(SMZBBlock92));
	}
	else {
		ofs << "Invalid SMZBBlock size" << endl;
		return;
	}

	ofs << "local offset B84,B100,B92: " << totalsize << endl;

	int sizeSpecial=0,index;
	bool haveCubeTree=false, haveSpecial=false, haveDatRef=false;
	haveCubeTree = mzbh1->offsetCubetree > mzbh1->offsetEndRecord100 && mzbh1->offsetEndCubetree < mzbh1->offsetHeader2;
	if(haveCubeTree) {
		if(mzbh1->offsetCubetree-mzbh1->offsetEndRecord100 > 16) {
			haveSpecial=true;
		}

		if(mzbh1->offsetHeader2 - mzbh1->offsetEndCubetree > 16) {
			haveDatRef=true;
		}
	}
	else {
		if(mzbh1->offsetCubetree>0) {
			sizeSpecial=mzbh1->offsetCubetree;
			haveSpecial=true;
		}
	}
	//unknown data Between B100 - cubeTree
	if( haveSpecial ) {
		ofs << "offset: " << totalsize << ") Special" << endl;
		if(sizeSpecial==0) {
			sizeSpecial = *(unsigned int*)(p+totalsize);
				totalsize+=4;
		}
		ofs << "total record: " << sizeSpecial << endl;
		//some special encoding observed at Dat 116
		//format: numRecord followed by 4Byte (num), followed by numRecord....etc
		for(index=1; index<=sizeSpecial; ++index) {		
			num = *(unsigned int*)(p+totalsize);
			totalsize+=4;
			if(totalsize >= mzbh1->offsetHeader2 )
				break;

			ofs << index << ") num MZB ref: " << num << endl;
			for(int i=1; i<=num; ++i) {
				val = *(unsigned int*)(p+totalsize);
				totalsize += 4;
			
				ofs << val << " ";
				if( i % 10 == 0 )
					ofs << endl;

				if(totalsize >= mzbh1->offsetHeader2 ) {
					ofs << endl;
					ofs << "num is invalid";
					break;
				}
			}
			ofs << endl;		
		}
		ofs << "local offset unknown special: " << totalsize << endl;
	}

	//if mzbh1->Cubetree !=0, it point to an Cubetree???
	if(haveCubeTree) {
		totalsize = mzbh1->offsetCubetree;
		ofs << "offset: " << totalsize << ")  Cubetree" << endl;
		//24 float (min/max boundingRect - 8point), 8 int (MZBref + num + 6 face)
		unsigned int totalMZB=0;
		noj=1;		
		SMZBBlock128 *pB128=nullptr;
		do {
			pB128 = (SMZBBlock128*)(p+totalsize);
			ofs << noj << ") offset: " << totalsize << " (" << pB128->x1 << "," << pB128->y1 << "," << pB128->z1 << "), (" << pB128->x2 << "," << pB128->y2 << "," << pB128->z2 << "), ("<< pB128->x3 << "," << pB128->y3 << "," << pB128->z3 << "), (" << pB128->x4 << "," << pB128->y4 << "," << pB128->z4 << "), (" << pB128->x5 << "," << pB128->y5 << "," << pB128->z5 << "), (" << pB128->x6 << "," << pB128->y6 << "," << pB128->z6 << "), (" << pB128->x7 << "," << pB128->y7 << "," << pB128->z7 << "), (" << pB128->x8 << "," << pB128->y8 << "," << pB128->z8 << ")" << endl;
			ofs << "offsetMZB: " << pB128->offsetMZB << " numMZBref: " << pB128->numMZB << " nextNode: [" << pB128->offset3 << ", " << pB128->offset4 << ", " << pB128->offset5 << ", " << pB128->offset6 << ", " << pB128->offset7 << ", " << pB128->offset8 << "]" << endl;
			noj++;

			if(pB128->numMZB!=0) {
				totalMZB+=pB128->numMZB;
				//MZB index
				ofs << "MZB ref: [";
				for(int j=0; j<pB128->numMZB; ++j) {
					ofs << *(unsigned int*)(p+pB128->offsetMZB+j*4) << " ";
				}
				ofs << "]" << endl;
			}
			totalsize += sizeof(SMZBBlock128);
			if(totalsize > mzbh1->offsetEndCubetree)
				break;
		} while(totalMZB < mzbh1->totalRecord100);

		ofs << "Local offset cubeTree: " << totalsize << "  noRecord: " << (noj-1) << " total MZB ref: " << totalMZB << " total Rec100: " << mzbh1->totalRecord100 << endl;

		//mzbReference
		if(totalsize < mzbh1->offsetEndCubetree ) {
			sizeSpecial = (mzbh1->offsetEndCubetree-totalsize)/4;
			for(index=0; index<sizeSpecial; ++index) {
				ofs << *(unsigned int*)(p+totalsize+index*4) << " ";
				if( index!=0 && index % 10 == 0 )
					ofs << endl;
			}
			ofs << endl;

			ofs << "Local offset MZBref: " << (totalsize + index * 4) << endl;
		}
	}

	if(haveDatRef) {
		buf[4]=0x00;
		totalsize = mzbh1->offsetEndCubetree;
		SMZBBlock76 *pB76=nullptr;
		ofs << "offset: " << totalsize << ") DatRef" << endl;
		do {
			pB76 = (SMZBBlock76*)(p+totalsize);
			if(pB76->datno[0]==0 && pB76->datno[1]==0 && pB76->datno[2]==0 && pB76->datno[3]==0)
				break;

			memcpy(buf, pB76->datno, 4);
			ofs << buf << endl;
			totalsize += sizeof(SMZBBlock76);
		} while(totalsize < mzbh1->offsetHeader2);
	}

	ofs << "Local offset DatRef: " << totalsize << endl;

	//unknown data betweeen endCubeTree to Collision [seems to be dat no]
	if( mzbh1->offsetHeader2==0 )
		return;

	//32Byte, block92...variable length
	SMZBHeader2 *mzbh2 = (SMZBHeader2*)(p+mzbh1->offsetHeader2);
	ofs << "offset: " << mzbh1->offsetHeader2 << ") Header2 - totalB92: " << mzbh2->totalRecord92 << " R92Flg: " << mzbh2->R92Flag << ", offsetB92: " << mzbh2->offsetBlock92 << ", totalB16: " << mzbh2->totalBlock16 << ", offsetB16: " << mzbh2->offsetBlock16 << ", offsetB4: " << mzbh2->offsetVertex << ", offsetB112: "  << mzbh2->offsetBlock112 << ", totalB112: " << mzbh2->totalRecord112 << ", unk: " << mzbh2->unk1 << endl;

//	mzbh2->totalRecord92 &=0xffffff;
	ofs << "  " << endl;
	ofs << "Block92: " << mzbh2->totalRecord92 << endl;

	totalsize=mzbh2->offsetBlock92;

	unsigned int r1, r2, r3, sizeB, sizeV, sizeN, flgB;
	float x,y,z;
	char s1[10], s2[10], s3[10], s4[10];
//	SMZBBlock92 *pB92 = (SMZBBlock92*)(p+totalsize);
	for(int i=0; i<mzbh2->totalRecord92; ++i) {
		r1 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
		r2 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
		r3 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
		sizeB = (*(unsigned int*)(p+totalsize)); totalsize +=4;
		flgB = sizeB & 0xffff0000;
		sizeB &= 0xffff;
		
		sizeV = (r2-r1)/12;
		sizeN = (r3-r2)/12;
		ofs << r1 << " - " << r2 << " - " << r3 << "  sizeV: " << sizeV << "  sizeN: " << sizeN << "  sizeB8: " << sizeB << " flgB: " << hex << uppercase << flgB << nouppercase << dec << endl;
		
		for(int j=0; j<sizeV; ++j) {
			x = (*(float*)(p+totalsize)); totalsize +=4;
			y = (*(float*)(p+totalsize)); totalsize +=4;
			z = (*(float*)(p+totalsize)); totalsize +=4;
			ofs << j << ") Vertex xyz: " << x << ", " << y << ", " << z << endl;
		}
	
		for(int j=0; j<sizeN; ++j) {
			x = (*(float*)(p+totalsize)); totalsize +=4;
			y = (*(float*)(p+totalsize)); totalsize +=4;
			z = (*(float*)(p+totalsize)); totalsize +=4;
			ofs << j << ") Normal xyz: " << x << ", " << y << ", " << z << endl;
		}
		//block92 is not used, because the footer is variable length, it depend on sizeB
		SMZBBlock8 *pB8=(SMZBBlock8*)(p+totalsize);
		for(int j=0; j<sizeB; ++j) {
			//x,y,z, normal
			ofs << j << ") " << limit(sizeV, pB8->s1) << ", " << limit(sizeV, pB8->s2) << ", " << limit(sizeV, pB8->s3) << ", " << limit(sizeV, pB8->s4) << endl;
			//sprintf_s(s1,10,"%x",pB8->s1);
			//sprintf_s(s2,10,"%x",pB8->s2);
			//sprintf_s(s3,10,"%x",pB8->s3);
			//sprintf_s(s4,10,"%x",pB8->s4);
			////x,y,z, normal
			//ofs << s1 << "," << s2 << ","  << s3 << "," << s4 << std::endl;
			pB8++;
		}
		totalsize += sizeB*8;			//sizeB*4*2(byte)

		//ofs << i << ") " << pB92->range1 << ", " << pB92->range2 << ", " << pB92->range3 << endl;
		//ofs << pB92->f1 << ", " << pB92->f2 << ", " << pB92->f3 << ", " << pB92->f4 << ", " << pB92->f5 << ", " << pB92->f6 << ", " << pB92->f7 << ", " << pB92->f8 << ", " << pB92->f9 << ", " << pB92->f10 << ", " << pB92->f11 << ", " << pB92->f12 << ", " << pB92->f13 << ", " << pB92->f14 << ", " << pB92->f15 << endl;
		//ofs << pB92->unk3 << "  " << pB92->footer << endl;
		//pB92++;
	}

	ofs << "Local offset: " << totalsize << endl;
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
	ofs << "Local offset B112: " << mzbh2->offsetBlock112 + mzbh2->totalRecord112*sizeof(SMZBBlock112) << endl;

	//block16...variable length
	ofs << "  " << endl;
	sizeV = mzbh2->totalBlock16;
	ofs << "Block16: " << sizeV << endl;
	totalsize = mzbh2->offsetBlock16;
	SMZBBlock16 *pB16;
	int ix = 5;
	sizeN=0;
	for(int i=0; i<sizeV; ++i) {
		pB16 = (SMZBBlock16*)(p+totalsize);
		if( pB16->i4==0 ) {
			ofs << i << ") addr: " << totalsize << " diff: " << (pB16->i1-sizeN) << "  - i1) " << pB16->i1 << ", i2) " << pB16->i2 << ", i3) " << pB16->i3 << ", i4) " << pB16->i4 << endl;
			totalsize += 16;
			sizeN = pB16->i1;
		}
		else {
			ofs << i << ") addr: " << totalsize << " diff: " << (pB16->i1-sizeN) << "  - i1) " << pB16->i1 << ", i2) " << pB16->i2 << ", i3) " << pB16->i3 << ", i4) " << pB16->i4;
			totalsize += 16;
			sizeN=pB16->i1;
			ix = 5;
			do {
				r1 = (*(unsigned int*)(p+totalsize));
				totalsize +=4;
				ofs << ", " << ix++ << ") " << r1;
			} while(r1!=0);
			ofs << endl;
		}

		//pB16++;
	}
	ofs << "Local offset B16: " << totalsize << endl;

	//block4, scan till end of MZB
	//every value is an addr to Block16
	ofs << "Block4" << endl;
	totalsize = mzbh2->offsetVertex;
	unsigned int last=len-16;
	int i, j;
	for(i=totalsize,j=0; i<len; ++j) {
		if(i>last) {
			if ((i - last) < 5) {
				ofs << j << ") addr: " << i << "  " << (*(unsigned int*)(p + i)) << ", " << (*(unsigned int*)(p + i + 4)) << ", " << (*(unsigned int*)(p + i + 8)) << endl;
				i += 12;
			}
			else if ((i - last) < 9) {
				ofs << j << ") addr: " << i << "  " << (*(unsigned int*)(p + i)) << ", " << (*(unsigned int*)(p + i + 4)) << endl;
				i += 8;
			}
			else if ((i - last) < 13) {
				ofs << j << ") addr: " << i << "  " << (*(unsigned int*)(p + i)) << endl;
				i += 4;
			}
			break;
		}
		else
			ofs << j << ") addr: " << i << "  " << (*(unsigned int*)(p+i)) << ", " << (*(unsigned int*)(p+i+4)) << ", " << (*(unsigned int*)(p+i+8)) << ", " << (*(unsigned int*)(p+i+12)) << endl;

		i+=16;
	}
	ofs << "Local offset B4: " << i << endl;

	//output collision mesh for blender
	if(OUTPUT_COLLISION_BLENDER) {
		char buf[50];
		sprintf_s(buf,50,"mapMesh_collision%d_%d.txt", count, m_fileNo);
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
	unsigned int r1, r2, r3, sizeB, flgB, sizeV, sizeN,totalsize=offsetB92;
//	float x,y,z;
	SMZBVector v;
	SMZBFace f;
	SMZBBlock112 *pB112 = (SMZBBlock112*)(p+offsetB112);

	r1 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	r2 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	r3 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	sizeB = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	flgB = sizeB & 0xffff0000;
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
/*
void CDatLoader::extractMMB(char *p, unsigned int len, unsigned int count)
{
	//char *mmbbuf = new char[len];
	//memcpy(mmbbuf,p,len);

	//char fileName[30];
	//sprintf_s(fileName,30,"MMB%d_%d", count, m_fileNo);
	//ofstream output(fileName, std::ofstream::binary);
	//for(int i=0; i<len; ++i) {
	//	output << hex << mmbbuf[i] ;
	//}
	//
	//output.close();
	//delete []mmbbuf;
//return;

	unsigned int totalsize=0, indexS, offsetStartMMB, offset, incr, rangeOffset, maxRange, in;
	SMZBVector v;
	SMZBFace f;
	char buf[40], buf1[3], bufid[5];
	buf[16]=0x00; buf1[2]=0x00; bufid[3]=0x00;
	offsetStartMMB = totalsize;

	SMMBHEAD *pMMB = (SMMBHEAD*)(p+totalsize);
	SMMBHEAD2 *pMMB2 = (SMMBHEAD2*)(p+totalsize);
	totalsize += sizeof(SMMBHEAD);

	if( (pMMB->id[0]=='M') && (pMMB->id[1]=='M') && (pMMB->id[2]=='B') ) {
		memcpy(bufid, pMMB->id, 3);
		ofs << "  MMB Head: id: " << bufid << " type: " << pMMB->type << " size: " << pMMB->next*16 << endl;
	}
	else {
		memcpy(buf, pMMB2->name, 16);
		memcpy(buf1, pMMB2->unk2, 2);
		ofs << "  MMB Head2: size: " << pMMB2->MMBSize << " unk1: " << pMMB2->unk1 << " unk2: " << buf1 << " name: " << buf << endl;
	}
	SMMBHeader *pMMBH = (SMMBHeader*)(p+totalsize);
	totalsize += sizeof(SMMBHeader);
	memcpy(buf, pMMBH->imgID, 16);
	ofs << "  Header1:  imgID: " << buf << " pieces: " << pMMBH->pieces << "  BoundingRect Combine xyz (min,max): " <<  pMMBH->x1 << ", " << pMMBH->x2 << ", " << pMMBH->y1 << ", " << pMMBH->y2 << ", " << pMMBH->z1 << ", " << pMMBH->z2 << endl;
	//get the global transformation from MZB
	OBJINFO *objinfo = findMMBTransform(pMMBH->imgID);

	//NOTE: range is not always 9 float...it is dependent on the start of SMMBBlockHeader
	//SMMBRange *pMMBR = (SMMBRange*)(p+totalsize);
	//ofs << " r1: " << pMMBR->r1 << " numModel: (" << pMMBR->numModel << ")  BoundingRect xyz (min,max): [" << pMMBR->x1 << "," << pMMBR->x2 << ", " << pMMBR->y1 << ", " << pMMBR->y2 << "," << pMMBR->z1 << "," << pMMBR->z2 << "] unk: " << pMMBR->unk1 << endl;

	////find the first range offset
	//maxRange=0;
	//if( pMMBR->r1!=0 )
	//	maxRange = (pMMBR->r1 - totalsize)/4;
	//else if( pMMBR->r2!=0 )
	//	maxRange = (pMMBR->r2 - totalsize)/4;
	//else if( pMMBR->r3!=0 )
	//	maxRange = (pMMBR->r3 - totalsize)/4;
	incr=0; rangeOffset=totalsize;
	SMMBBlockHeader *pMMBBH = nullptr;
	//range are spread among the 9 field
//	for(int BH=0; BH<maxRange; ++BH) {
	for(int BH=0; BH<1; ++BH) {
		//read an unsigned int which is the offset
		offset = (*(unsigned int*)(p+rangeOffset+BH*4));
		ofs << "r1: " << offset << endl;
		if( offset==0 )
			continue;

		incr++;
		if( incr > pMMBH->pieces ) {
			ofs << "invalid MMB size: " << pMMBH->pieces << endl;
			return;
		}
//		offset += offsetStartMMB;
		pMMBBH = (SMMBBlockHeader*)(p+offset);
		offset += sizeof(SMMBBlockHeader);		
		ofs << "   BoundingRect numModel: (" << pMMBBH->numModel << ") xyz (min,max): [" << pMMBBH->x1 << ", " << pMMBBH->x2 << ", " << pMMBBH->y1 << ", " << pMMBBH->y2 << ", " << pMMBBH->z1 << ", " << pMMBBH->z2 << "] mumFace: " << pMMBBH->numFace << endl;

		SMMBModelHeader *pMMBMH = (SMMBModelHeader*)(p+offset);
		offset += sizeof(SMMBModelHeader);
		pMMBMH->vertexsize &= 0xffff;
		ofs <<  " ModelHeader) TextureName: " << pMMBMH->textureName << " BlockVertex size: " << pMMBMH->vertexsize << endl;

		for(int i=0; i<pMMBMH->vertexsize; ++i) {
			SMMBBlockVertex *pMMBBV = (SMMBBlockVertex*)(p+offset);
			offset += sizeof(SMMBBlockVertex);
//			ofs << setw(8) << i << ")  " << setw(6) << "xyz: " << setw(9) << pMMBBV->x << ", " << setw(9) << pMMBBV->y << ", " << setw(9) << pMMBBV->z << "  normal: " << setw(11) << pMMBBV->hx << ", " << setw(11) << pMMBBV->hy << ", " << setw(11) << pMMBBV->hz << "  " << setw(10) << pMMBBV->zero << ", " << pMMBBV->one << endl;
			sprintf_s(buf,40,"%02x %02x %02x %02x",pMMBBV->marker[0], pMMBBV->marker[1], pMMBBV->marker[2], pMMBBV->marker[3]);
			ofs << setw(8) << i << ")  " << setw(6) << "xyz: " << setw(9) << pMMBBV->x << ", " << setw(9) << pMMBBV->y << ", " << setw(9) << pMMBBV->z << "  normal: " << setw(11) << pMMBBV->hx << ", " << setw(11) << pMMBBV->hy << ", " << setw(11) << pMMBBV->hz << "  marker: " << buf << " uv: " << setw(10) << pMMBBV->zero << ", " << pMMBBV->one << endl;

			if( OUTPUT_MMB_BLENDER ) {
				if( (PARTIAL_MMB==1) && (count!=COUNT_W))
					continue;
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

		if( OUTPUT_MMB_BLENDER ) {
			if( (PARTIAL_MMB==1) && (count!=COUNT_W))
				goto P2;

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
				if( f.i1==f.i2 ) {
					f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
					++j;
				}
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
						f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
						++j;
						//if( f.i1==f.i2) {
						//	//yes, 2 similiar
						//	f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
						//	++j;
						//}
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
P2:
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

	bufid[4]=0x00;
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
			ofs << "   offset: " << offset << " textureName: " << pMMBMH->textureName << " BlockVertex size: " << pMMBMH->vertexsize << endl;

			for(int i=0; i<pMMBMH->vertexsize; ++i) {
				SMMBBlockVertex *pMMBBV = (SMMBBlockVertex*)(p+offset);
				offset += sizeof(SMMBBlockVertex);
//				memcpy(bufid, pMMBBV->marker, 4);
				sprintf_s(buf,40,"%02x %02x %02x %02x",pMMBBV->marker[0], pMMBBV->marker[1], pMMBBV->marker[2], pMMBBV->marker[3]);
				ofs << setw(8) << i << ")  " << setw(6) << "xyz: " << setw(9) << pMMBBV->x << ", " << setw(9) << pMMBBV->y << ", " << setw(9) << pMMBBV->z << "  normal: " << setw(9) << pMMBBV->hx << ", " << setw(9) << pMMBBV->hy << ", " << setw(9) << pMMBBV->hz << "  marker: " << buf << " uv: " << pMMBBV->zero << ", " << pMMBBV->one << endl;

				if( OUTPUT_MMB_BLENDER ) {
					if( (PARTIAL_MMB==1) && (count!=COUNT_W))
						continue;

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

			if( OUTPUT_MMB_BLENDER ) {
				if( (PARTIAL_MMB==1) && (count!=COUNT_W))
					goto P3;

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
					if( f.i1==f.i2 ) {
						f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
						++j;
					}
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
							f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
							++j;
							//if( f.i1==f.i2) {
							//	//yes, 2 similiar
							//	f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
							//	++j;
							//}
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
P3:
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
*/
void CDatLoader::extractMMB(char *p, unsigned int len, unsigned int count)
{
	if(WRITE_RAW_DECODE_MMB && count==COUNT_W) {
		char *mmbbuf = new char[len];
		memcpy(mmbbuf,p,len);

		char fileName[30];
		sprintf_s(fileName,30,"MMB_RAW_%d_%d", count, m_fileNo);
		ofstream output(fileName, std::ofstream::binary);
		for(int i=0; i<len; ++i) {
			output << hex << mmbbuf[i] ;
		}
	
		output.close();
		delete []mmbbuf;
	}
	int indexS, indexFlag;
	unsigned int totalsize=0, offsetStartMMB, offset, incr, rangeOffset, maxRange, in, maxSize;
	SMZBVector v;
	SMZBFace f;
	char buf[40], buf1[3], buf2[100], bufid[5];
	buf[16]=0x00; buf1[2]=0x00; bufid[3]=0x00;
	offsetStartMMB = totalsize;

	//16Byte Header
	SMMBHEAD *pMMB = (SMMBHEAD*)(p+totalsize);
	SMMBHEAD2 *pMMB2 = (SMMBHEAD2*)(p+totalsize);
	totalsize += sizeof(SMMBHEAD);

	if( (pMMB->id[0]=='M') && (pMMB->id[1]=='M') && (pMMB->id[2]=='B') ) {
		memcpy(bufid, pMMB->id, 3);
		maxSize=pMMB->next*16;
		ofs << "  MMB Head: id: " << bufid << " type: " << pMMB->type << " size: " << pMMB->next*16 << "  shadow: " << pMMB->is_shadow << " extract: " << pMMB->is_extracted << " ver: " << pMMB->ver_num << " virtual: " << pMMB->is_virtual << endl;
	}
	else {
		memcpy(buf, pMMB2->name, 8);
		buf[8] = 0x00;
		sprintf_s(buf2,100,"d1: %d, d3: %d, [d4: %d, d5: %d, d6:%d] ", pMMB2->d1, pMMB2->d3, pMMB2->d4, pMMB2->d5, pMMB2->d6);
		maxSize=pMMB2->MMBSize;
		ofs << "  MMB Head2: size: " << pMMB2->MMBSize << " data: " << buf2 << " name: " << buf << endl;
		if (WRITE_MMB_RGB)
			ofs2 << buf2;
	}
	//48Byte, Define no of pieces (SMMBBLockHeader)
	SMMBHeader *pMMBH = (SMMBHeader*)(p+totalsize);
	totalsize += sizeof(SMMBHeader);
	memcpy(buf, pMMBH->imgID, 16);
	buf[16] = 0x00;
	ofs << "  Header1:  imgID: " << buf << " pieces: " << pMMBH->pieces << "  BoundingRect Combine xyz (min,max): " <<  pMMBH->x1 << ", " << pMMBH->x2 << ", " << pMMBH->y1 << ", " << pMMBH->y2 << ", " << pMMBH->z1 << ", " << pMMBH->z2 << "  offsetBlockHeader: " << pMMBH->offsetBlockHeader << endl;

	if (WRITE_MMB_RGB)
		ofs2 << "  imgID: " << buf << endl;
	
	incr=0; rangeOffset=totalsize;
	
	if(pMMBH->offsetBlockHeader==0 ) {
		if(pMMBH->pieces!=0 ) {
			//32Byte
			ofs << "next SMMBBlockHeader offset:";
			for(int i=0; i<8; ++i) {
				ofs << "  " << (*(unsigned int*)(p+totalsize));
				totalsize += 4;
			}
			ofs << endl;
		}
	}
	else {
		maxRange = pMMBH->offsetBlockHeader - totalsize;
		if(maxRange>0) {
			ofs << "next SMMBBlockHeader offset:";
			for(int i=0; i<maxRange; i+=4) {
				ofs << "  " << (*(unsigned int*)(p+totalsize));
				totalsize += 4;
			}
			ofs << endl;
		}
	}

	for(int piece=1; piece<=pMMBH->pieces; piece++) {
		ofs << "   Piece: " << piece << endl;
		//32Byte define num of Model in each Piece
		SMMBBlockHeader *pMMBBH = (SMMBBlockHeader*)(p+totalsize);
		totalsize += sizeof(SMMBBlockHeader);		
		ofs << "   BoundingRect numModel: (" << pMMBBH->numModel << ") xyz (min,max): [" << pMMBBH->x1 << ", " << pMMBBH->x2 << ", " << pMMBBH->y1 << ", " << pMMBBH->y2 << ", " << pMMBBH->z1 << ", " << pMMBBH->z2 << "] numFace: " << pMMBBH->numFace << endl;
		
		if(pMMBBH->numModel > 50) {
			//not possible ==> corrupt
			ofs << "   numModel: " << pMMBBH->numModel << " is corrupt,  totalsize: " << totalsize << "  padding: " << (len-totalsize) << endl;
			return;
		}
		bufid[4]=0x00;
		offset = totalsize;
		//NOTE if didn't end, then it is follow by another SMMBModelHeader
		//TBC...this block might not be at the end, it might need to depend on the RANGE too....
		//the RANGE determine num of separate piece of each model
		for(int k=1; k<=pMMBBH->numModel; ++k) {		
			if( offset + sizeof(SMMBModelHeader) > len ) {
				ofs << "   Invalid numModel: " << pMMBBH->numModel << "  was " << k << endl;
				break;
			}
			//20Byte Model Info, TextureName and Vertex Info
			SMMBModelHeader *pMMBMH = (SMMBModelHeader*)(p+offset);
			offset += sizeof(SMMBModelHeader);
//			pMMBMH->vertexsize &= 0xffff;
			memcpy(buf,pMMBMH->textureName,16);
			buf[16]=0x00;
			ofs << "   Local offset: " << offset << "  Model: " << k << ")  textureName: " << buf << " BlockVertex size: " << pMMBMH->vertexsize << " Blending: 0x" << hex << uppercase << pMMBMH->blending << dec << nouppercase << endl;

			for(int i=0; i<pMMBMH->vertexsize; ++i) {
				//2 different encoding, unknown condition
				if(pMMB2->d3==2) {
					//48Byte, Vertex Info
					SMMBBlockVertex2 *pMMBBV = (SMMBBlockVertex2*)(p+offset);
					offset += sizeof(SMMBBlockVertex2);
					sprintf_s(buf,40,"R:%d G:%d B:%d A:%d",pMMBBV->R, pMMBBV->G, pMMBBV->B, pMMBBV->A);
					ofs << setw(8) << i << ")  " << setw(6) << "xyz: " << setw(9) << pMMBBV->x << ", " << setw(9) << pMMBBV->y << ", " << setw(9) << pMMBBV->z << "  delta: " << setw(9) << pMMBBV->dx << ", " << setw(9) << pMMBBV->dy << ", " << setw(9) << pMMBBV->dz << "  normal: " << setw(9) << pMMBBV->hx << ", " << setw(9) << pMMBBV->hy << ", " << setw(9) << pMMBBV->hz << "  vertexColor: " << buf << " uv: " << pMMBBV->u << ", " << pMMBBV->v << endl;

					if( OUTPUT_MMB_BLENDER ) {
						if( (PARTIAL_MMB==1) && (count!=COUNT_W))
							continue;
						v.x = pMMBBV->x; v.y = pMMBBV->y; v.z = pMMBBV->z;
						m_vecVector.push_back(v);
					}
				}
				else {
					//36Byte, Vertex Info
					SMMBBlockVertex *pMMBBV = (SMMBBlockVertex*)(p+offset);
					offset += sizeof(SMMBBlockVertex);				
					sprintf_s(buf,40,"R:%d G:%d B:%d A:%d",pMMBBV->R, pMMBBV->G, pMMBBV->B, pMMBBV->A);
					ofs << setw(8) << i << ")  " << setw(6) << "xyz: " << setw(9) << pMMBBV->x << ", " << setw(9) << pMMBBV->y << ", " << setw(9) << pMMBBV->z << "  normal: " << setw(9) << pMMBBV->hx << ", " << setw(9) << pMMBBV->hy << ", " << setw(9) << pMMBBV->hz << "  vertexColor: " << buf << " uv: " << pMMBBV->u << ", " << pMMBBV->v << endl;
					if( OUTPUT_MMB_BLENDER ) {
						if( (PARTIAL_MMB==1) && (count!=COUNT_W))
							continue;
						v.x = pMMBBV->x; v.y = pMMBBV->y; v.z = pMMBBV->z;
						m_vecVector.push_back(v);
					}
				}
			}
			//Num of Indices
			indexS = (*(unsigned int*)(p+offset)) &0xffff;
			indexFlag = (*(unsigned int*)(p + offset)) & 0xffff0000;

			ofs << "   Indices size: " << indexS << " Flg: 0x" << hex << uppercase << indexFlag << dec << nouppercase << endl;
			offset += 4;
			if( OUTPUT_MMB_BLENDER ) {
				//COUNT_W is used to pinpoint the MMB Block to write to file, else there r too many block
				if( (PARTIAL_MMB==1) && (count!=COUNT_W))
					goto P3;

				if((pMMB->id[0]=='M' && pMMB->type==0) || (pMMB->id[0]!='M' && pMMB2->d3==2)) {
					//simple 3 indices triangle
					pMMBBH->numFace = indexS/3;
					for(int i=0,j=0; i<pMMBBH->numFace; i++) {
						f.i1 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
						j++;
						f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
						j++;
						f.i3 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
						j++;
						pushMMBIndices(f);
					}
				}
				else {
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
						if( f.i1==f.i2 ) {
							f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
							++j;
							}
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
								f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
								++j;
								//if( f.i1==f.i2) {
								//	//yes, 2 similiar
								//	f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
								//	++j;
								//}
							}
							else {
								//might need to reset the rotation???
								f.i1=f.i2;
								f.i2=f.i3;
								++j;
							}
						}
					}
				}
				m_lastIndices += pMMBMH->vertexsize;
			}
P3:
			//2Byte per index
			int last = indexS-4;
			for(int j=0; j<indexS;) {
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

			if( indexS % 2 > 0 )
				offset += 2;	//padding 2 byte

			totalsize = offset;
		}
		ofs << "   Local offset piece: " << piece << " @" << totalsize << endl;
	}
	maxSize = (len-totalsize);
	ofs << "   totalsize: " << totalsize << "  padding: " << maxSize << endl;
	if(maxSize>16)
		ofs << "   missing data" << endl;

}

SMZBBlock100* CDatLoader::findMMBTransform( char *name)
{
	for(auto it=m_vecOBJInfo.begin(); it!=m_vecOBJInfo.end(); ++it) {
		if( memcmp(it->id, name, 16)==0 )
			return &(*it);
	}
	return nullptr;
}

void CDatLoader::MMBTransform(SMZBBlock100 *b84, SMZBVector &v)
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

	if( (floor(v.x)==-1983 ) && (floor(v.y)==-3) && (floor(v.z)==1182) ) {
//	if( (v.x==-1982.31) && (v.y==-2.323) && (v.z==1182.4) ) {
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
	sprintf_s(buf,50,"mapMeshMMB%d_%d.txt", COUNT_W, m_fileNo);
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
	unsigned int width,height, size, offset, rem;
	IMGINFOA1 *ii = nullptr;

	ii = (IMGINFOA1*)(p);
	memcpy(buf, ii->id, 16);
	buf[16]=0x00;
	imgname.assign(buf);
	width = ii->imgx;
	height = ii->imgy;
	size = width*height;

	if (ii->flg == 0xA1) {
		ofs << "Flg: Ox" << hex << uppercase << (0x0000ff & ii->flg) << nouppercase << dec << " id: " << imgname << " x,y: " << ii->imgx << " " << ii->imgy << " bpp: " << ii->widthbyte << " size: " << ii->size << " block: " << ii->noBlock << endl;
		ofs << "Flg: 0x" << hex << uppercase << (0xFFFF00 & ii->flg) << " unk2: 0x" << ii->dwnazo2[0] << " 0x" << ii->dwnazo2[1] << " 0x" << ii->dwnazo2[2] << " 0x" << ii->dwnazo2[3] << " 0x" << ii->dwnazo2[4] << " 0x" << ii->dwnazo2[5] << nouppercase << dec << endl;
	}
	else 
		ofs << "Flg: Ox" << hex << uppercase << (0x0000ff & ii->flg) << nouppercase << dec << " id: " << imgname << " x,y: " << ii->imgx << " " << ii->imgy << " width: " << ii->widthbyte << endl;

	switch( ii->flg )
	{
	case 0xA1:	offset = sizeof(IMGINFOA1); sprintf_s(buf,500,"DATHEAD:16, IMGINFO Header:%d, DXT%c, Data:%d, Padding:%d", sizeof(IMGINFOA1), ii->ddsType[0], ii->size, len-sizeof(IMGINFOA1)-size);
		break;
	case 0x01:	offset = sizeof(IMGINFO); sprintf_s(buf,500,"DATHEAD:16, IMGINFO Header:%d, Data:%d, Padding:%d", sizeof(IMGINFO), size, len-sizeof(IMGINFO)-size);
		break;
	case 0x81:	offset = sizeof(IMGINFO); sprintf_s(buf,500,"DATHEAD:16, IMGINFO Header:%d, Data:%d, Padding:%d", sizeof(IMGINFO), size, len-sizeof(IMGINFO)-size);
		break;
	case 0xB1:	offset = sizeof(IMGINFOB1); sprintf_s(buf,500,"DATHEAD:16, IMGINFO Header:%d, Data:%d, Padding:%d", sizeof(IMGINFOB1), size, len-sizeof(IMGINFOB1)-size);
		break;
	case 0x05:	offset = sizeof(IMGINFO05); sprintf_s(buf,500,"DATHEAD:16, IMGINFO Header:%d, Data:%d, Padding:%d", sizeof(IMGINFO05), size, len-sizeof(IMGINFO05)-size);
		break;
	case 0x91:	offset = sizeof(IMGINFO); sprintf_s(buf,500,"DATHEAD:16, IMGINFO Header:%d, Data:%d, Padding:%d", sizeof(IMGINFO), size, len-sizeof(IMGINFO)-size);
		break;
	default:	ofs << "Unknown image" << endl; offset=0;
	}

	str.assign(buf);
	ofs << str << endl << endl;
	//print the last 11byte padding

	//rem=len-offset-size;
	//char *ch = (p+offset+ii->size);
	//ofs << "padding byte : ";
	//for(int i=0; i<rem; ++i) {
	//	ofs << hex << uppercase << *ch << " ";
	//	ch++;
	//}
	//ofs << endl;
}

void CDatLoader::extractVertex(char *p, unsigned int len)
{
	char buf[500];
	string str;
	unsigned int totalsize=0;
	DAT2AHeader *pcp=nullptr;

	pcp=(DAT2AHeader *)(p+totalsize);
	sprintf_s(buf,500,"    ver:%d,  nazo:%d,  type:%d,  flip:%d", pcp->ver, pcp->nazo, (pcp->type &0x7f), pcp->flip);
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

	bool isWrite2Blender = !m_vecBoneMatrix.empty() && OUTPUT_CHAR_MESH_BLENDER;
	ofstream blender;
	if( isWrite2Blender ) {
		sprintf_s(buf,500,"charMesh%d_%d.txt", m_fileNo, cur_mesh);
		str.assign(buf);
		blender.open(str.c_str(), std::ofstream::out);
		if( !blender ) {
			cout << "unable to open outfile" << endl;
			return;
		}
	}
	//whether bone index is access indirectly from boneTable
	bool isIndirect=(pcp->type&0x80);
	
	m_vecFace.clear();
	//extract poly
	TEXLIST *uvf=nullptr;
	TEXLIST2 *uvf2=nullptr;
	int num8010=0,num8000=0,num5453=0,num4345=0,num0054=0,num0043=0,numUNK=0;
	totalsize = pcp->offsetPoly*2;
	while(1) {
		int wf = (int)*(WORD*)(p+totalsize  );
		int ws = (int)*(WORD*)(p+totalsize+2);
        if( 0x8010 == (wf&0x80F0) ) { 
            dat8000 *dat = (dat8000*)(p+totalsize+2);		//+2 for wf, there is no ws
//            p+=0x2e;		//46 byte
			totalsize += 46;
			num8010++;
            continue;  //不明
		} 
		else if ( 0x8000 == (wf&0x80F0) ) { //テクスチャの変更
			num8000++;
			memcpy(buf,p+totalsize+2,16);		//+2 for wf, there is no ws
			buf[16]=0x00;
			str.assign(buf);
			ofs << "textureID: " << str << endl;
//			p+=0x12;	//18 byte
			totalsize += 18;					//16 + 2
		} 
		else {
			if( 0x5453 == wf ) {/*ST*/  //StripTriangle
				num5453++;
				ofs << "[StripTriangle] Total face: " << ws << endl;
				totalsize +=4;			//this is wf+ws (4byte)
				//first 3 vertex
				uvf = (TEXLIST*)(p+totalsize);
				sprintf_s(buf,500,"i1:%d i2:%d i3:%d  uv1:(%f,%f)  uv2:(%f,%f)  uv3:(%f,%f)",uvf->i1,uvf->i2,uvf->i3,uvf->u1,uvf->v1,uvf->u2,uvf->v2,uvf->u3,uvf->v3);
				str.assign(buf);
				ofs << str << endl;
				totalsize += 30;
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
			else if( 0xffff==wf )
				break;
			else {
				numUNK++;
				sprintf_s(buf,500,"unknown wf: %x", wf);
				str.assign(buf);
				ofs << str << endl;
	//			break;
			}
		}
	}

	ofs << "num8010: " << num8010 << " num8000: " << num8000 << " num5453: " << num5453 << " num4345: " << num4345 << " num0054: " << num0054 << " num0043: " << num0043 << " numUNK: " << numUNK << endl;
	ofs << "local offset Face: " << totalsize << endl;

	if( isWrite2Blender ) {
		blender << m_vecFace.size() << endl;
		for(auto it=m_vecFace.begin(); it!=m_vecFace.end(); ++it) {
			blender << it->i1 << "," << it->i2 << "," << it->i3 << endl;
		}
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

	ofs << "local offset BoneTbl: " << totalsize << endl;

	//extract Bone weight
	BONE3 *b3 = nullptr;
	int noWeight = pcp->WeightSuu;
	totalsize = pcp->offsetWeight*2;
	b3 = (BONE3*)(p+totalsize);
	int noB1 = (int)b3->low;
	int noB2 = (int)b3->high;
	ofs << "noBoneWeight:" << noWeight << " noB1:" << noB1 << " noB2:" << noB2 << endl;
	totalsize += sizeof(BONE3);
	ofs << "local offset BoneWeight: " << totalsize << endl;

	//extract bone
	int noB = pcp->BoneSuu/2;		//bone is 4byte, therefore need to divide 2

//	int tblidxL, flgL, tblidxH, flgH;
//	BONE4 b4;
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

	ofs << "local offset Bone: " << totalsize << endl;

	//IMPORTANT if (pcp->flip) then need to draw twice, HOW?????
	WORD doflip=0;

	//extract vertex
	MODELVERTEX1 *mv1=nullptr;
	MODELVERTEX2 *mv2=nullptr;
	CLOTHVERTEX1 *cv1=nullptr;
	CLOTHVERTEX2 *cv2=nullptr;

	blender << noB << endl;
	if(pcp->PolyLod2Suu==0)
		ofs << "ModelVertex: " << noB << endl;
	else
		ofs << "ClothVertex: " << noB << endl;

	totalsize = pcp->offsetVertex*2;
	for(int k=0; k<noB1; ++k) {
		if(pcp->PolyLod2Suu==0) {
			mv1 = (MODELVERTEX1*)(p+totalsize);
			sprintf_s(buf,500,"%d) xyz: %f %f %f,  hxhyhz: %f %f %f",k, mv1->x, mv1->y, mv1->z, mv1->hx, mv1->hy, mv1->hz);
			str.assign(buf);
			ofs << str << endl;

			if( isWrite2Blender ) {
				mv1=transformVertexMV1(k, mv1, doflip, isIndirect);
				blender << mv1->x << "," << mv1->y << "," << mv1->z << endl;
			}
			totalsize += sizeof(MODELVERTEX1);
		}
		else {
			cv1 = (CLOTHVERTEX1*)(p+totalsize);
			sprintf_s(buf,500,"%d) xyz: %f %f %f", k, cv1->x, cv1->y, cv1->z);
			str.assign(buf);
			ofs << str << endl;

			if( isWrite2Blender ) {
				cv1 = transformVertexC1(k, cv1, doflip, isIndirect);
				blender << cv1->x << "," << cv1->y << "," << cv1->z << endl;
			}
			totalsize += sizeof(CLOTHVERTEX1);
		}
	}

	for(int k=0; k<noB2; ++k) {
		if(pcp->PolyLod2Suu==0) {
			mv2 = (MODELVERTEX2*)(p+totalsize);
			sprintf_s(buf,500,"%d) xyzw1: %f %f %f %f,  hxhyhz: %f %f %f,  xyzw2: %f %f %f %f,  hxhyhz2: %f %f %f", k+noB1,
				mv2->x1,mv2->y1,mv2->z1,mv2->w1, mv2->hx1,mv2->hy1,mv2->hz1, mv2->x2,mv2->y2,mv2->z2,mv2->w2, mv2->hx2,mv2->hy2,mv2->hz2);
			str.assign(buf);
			ofs << str << endl;

			if( isWrite2Blender ) {
				mv2=transformVertexMV2(k+noB1, mv2, doflip, isIndirect);
				blender << (mv2->x1+mv2->x2) << "," << (mv2->y1+mv2->y2) << "," << (mv2->z1+mv2->z2) << endl;
			}
			totalsize += sizeof(MODELVERTEX2);
		}
		else {
			cv2 = (CLOTHVERTEX2*)(p+totalsize);
			sprintf_s(buf,500,"%d) xyzw1: %f %f %f %f,  xyzw2: %f %f %f %f", k, cv2->x1, cv2->y1, cv2->z1, cv2->w1, cv2->x2, cv2->y2, cv2->z2, cv2->w2);
			str.assign(buf);
			ofs << str << endl;

			if( isWrite2Blender ) {
				cv2 = transformVertexC2(k, cv2, doflip, isIndirect);
				blender << (cv2->x1+cv2->x2) << "," << (cv2->y1+cv2->y2) << "," << (cv2->z1+cv2->z2) << endl;
			}
			totalsize += sizeof(CLOTHVERTEX2);
		}
	}

	ofs << "local offset Vertex: " << totalsize << endl;

	//extract extra, i1/i2 value exactly equal to num of clothVertex!! To make the cloth soft?? HOW
	myExtra *ext=nullptr;
	if(pcp->PolyLod2Suu!=0) {
		ofs << "Extra: " << pcp->PolyLod2Suu << endl;
		totalsize = pcp->offsetPolyLod2*2;
		int size = pcp->PolyLod2Suu*2/sizeof(myExtra);
		for(int i=0; i<size; ++i) {
			ext = (myExtra*)(p+totalsize);
			sprintf_s(buf,500,"%d) i1:%d  [flg1:%d],  i2:%d  [flg2:%d], [%f]", i, ext->i1, ext->flg1, ext->i2, ext->flg2, ext->f);
			str.assign(buf);
			ofs << str << endl;

			totalsize += sizeof(myExtra);
		}

		ofs << "local offset Extra: " << totalsize << endl;
	}

	m_vecFace.clear();
	m_vecBoneIndexTbl.clear();
	m_vecBone3.clear();
	if( isWrite2Blender )
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
	TMatrix44 M = m_vecBoneMatrix[index];

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

CLOTHVERTEX1* CDatLoader::transformVertexC1(int k, CLOTHVERTEX1 *c1, WORD flip, bool isIndirect)
{
	//get the bone matrix
	int index, flg;
	BONE3 b3 = m_vecBone3[k];
	if( isIndirect )
		index = ((flip==0)? m_vecBoneIndexTbl[b3.leftL] : m_vecBoneIndexTbl[b3.rightL]);
	else
		index = ((flip==0)? b3.leftL : b3.rightL);

	flg = b3.flgL;
	TMatrix44 M = m_vecBoneMatrix[index];

	if( flip!=0 ) {
		if( flg==1 ) M *= matrixMirrorX;
		if( flg==2 ) M *= matrixMirrorY; 
		if( flg==3 ) M *= matrixMirrorZ;
	}
	//multiply with vertex
	M.multiplyByVector3(c1->x, c1->y, c1->z, 1.0f);
	return c1;
}

CLOTHVERTEX2* CDatLoader::transformVertexC2(int k, CLOTHVERTEX2 *c2, WORD flip, bool isIndirect)
{
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

	if( flip!=0 ) {
		if( flgL==1 ) ML *= matrixMirrorX;
		if( flgL==2 ) ML *= matrixMirrorY; 
		if( flgL==3 ) ML *= matrixMirrorZ;
		if( flgH==1 ) MH *= matrixMirrorX;
		if( flgH==2 ) MH *= matrixMirrorY;
		if( flgH==3 ) MH *= matrixMirrorZ;
	}
	ML.multiplyByVector3(c2->x1, c2->y1, c2->z1, c2->w1);
	MH.multiplyByVector3(c2->x2, c2->y2, c2->z2, c2->w2);
	return c2;
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
	sprintf_s(buf,500,"local offset:%d, sizeFloat:%d, arrFloat:%d", totalsize,rem,rem/4);
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
	sprintf_s(buf,500,"    unk: %d,  noBone:%d ",unk, noBone);
	str.assign(buf);
	ofs << str << endl;
	
	ofstream boneStream;
	if(PARSE_UNK_BONE) {
		sprintf_s(buf,50,"bone%d.txt", m_fileNo);
		boneStream.open(buf,std::ofstream::out);
		boneStream << str << endl;
	}
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
		if(PARSE_UNK_BONE)
			boneStream << i <<") parent:" << (short)bn->parent << "   x: " << M(3,0) << " y: " << M(3,1) << " z: " << M(3,2) << " (xyz): [" << bn->x << ", " << bn->y << ", " << bn->z << "]" << endl;
//		boneStream << M(3,0) << "," << M(3,1) << "," << M(3,2) << endl;
		totalsize +=sizeof(BONE);
	}
	sprintf_s(buf,500,"local offset:%d", totalsize);
	str.assign(buf);
	ofs << str << endl << endl;

	if(PARSE_UNK_BONE) {
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
}
