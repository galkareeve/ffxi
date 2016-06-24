#include "FFXILandscapeMesh.h"
#include "TDWAnalysis.h"
#include "myEnum.h"

#include "IMeshBuffer.h"
#include "OpenGLDriver.h"
#include "myEnum.h"
#include "DDS2Bmp.h"
#include "SceneManager.h"
#include "Utility.h"
#include "MeshBufferGroup.h"

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <glm/gtx/string_cast.hpp>

//#include <Windows.h>

char ffxidir[512]="E:\\Program Files (x86)\\PlayOnline2\\SquareEnix\\FINAL FANTASY XI\\";

using namespace std;

//void LocateFFXIDataFolder()
//{
//	HKEY hKey;
//	memset(ffxidir,0,512);
//
//	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\PlayOnlineUS\\InstallFolder",0,KEY_READ,&hKey))
//	{
//		DWORD dwData = sizeof(ffxidir);
//		DWORD dwType = REG_SZ;
//		RegQueryValueEx(hKey,"0001",NULL,&dwType,(LPBYTE)ffxidir,&dwData);
//		RegCloseKey( hKey );
//	}
//	if (lstrlen(ffxidir) == 0)
//	{
//		MessageBox(NULL,"FINAL FANTASY XI","Err",MB_OK);
//		GetCurrentDirectory(sizeof(ffxidir),ffxidir);
//	}
//	if (lstrlen(ffxidir) > 0)
//	{
//		if (ffxidir[lstrlen(ffxidir)-1] != '\\')
//		{
//			lstrcat(ffxidir,"\\");
//		}
//	}
//}

static unsigned char key_table[0x100] =
{
	0xE2, 0xE5, 0x06, 0xA9, 0xED, 0x26, 0xF4, 0x42, 0x15, 0xF4, 0x81, 0x7F, 0xDE, 0x9A, 0xDE, 0xD0,
	0x1A, 0x98, 0x20, 0x91, 0x39, 0x49, 0x48, 0xA4, 0x0A, 0x9F, 0x40, 0x69, 0xEC, 0xBD, 0x81, 0x81,
	0x8D, 0xAD, 0x10, 0xB8, 0xC1, 0x88, 0x15, 0x05, 0x11, 0xB1, 0xAA, 0xF0, 0x0F, 0x1E, 0x34, 0xE6,
	0x81, 0xAA, 0xCD, 0xAC, 0x02, 0x84, 0x33, 0x0A, 0x19, 0x38, 0x9E, 0xE6, 0x73, 0x4A, 0x11, 0x5D,
	0xBF, 0x85, 0x77, 0x08, 0xCD, 0xD9, 0x96, 0x0D, 0x79, 0x78, 0xCC, 0x35, 0x06, 0x8E, 0xF9, 0xFE,
	0x66, 0xB9, 0x21, 0x03, 0x20, 0x29, 0x1E, 0x27, 0xCA, 0x86, 0x82, 0xE6, 0x45, 0x07, 0xDD, 0xA9,
	0xB6, 0xD5, 0xA2, 0x03, 0xEC, 0xAD, 0x62, 0x45, 0x2D, 0xCE, 0x79, 0xBD, 0x8F, 0x2D, 0x10, 0x18,
	0xE6, 0x0A, 0x6F, 0xAA, 0x6F, 0x46, 0x84, 0x32, 0x9F, 0x29, 0x2C, 0xC2, 0xF0, 0xEB, 0x18, 0x6F,
	0xF2, 0x3A, 0xDC, 0xEA, 0x7B, 0x0C, 0x81, 0x2D, 0xCC, 0xEB, 0xA1, 0x51, 0x77, 0x2C, 0xFB, 0x49,
	0xE8, 0x90, 0xF7, 0x90, 0xCE, 0x5C, 0x01, 0xF3, 0x5C, 0xF4, 0x41, 0xAB, 0x04, 0xE7, 0x16, 0xCC,
	0x3A, 0x05, 0x54, 0x55, 0xDC, 0xED, 0xA4, 0xD6, 0xBF, 0x3F, 0x9E, 0x08, 0x93, 0xB5, 0x63, 0x38,
	0x90, 0xF7, 0x5A, 0xF0, 0xA2, 0x5F, 0x56, 0xC8, 0x08, 0x70, 0xCB, 0x24, 0x16, 0xDD, 0xD2, 0x74,
	0x95, 0x3A, 0x1A, 0x2A, 0x74, 0xC4, 0x9D, 0xEB, 0xAF, 0x69, 0xAA, 0x51, 0x39, 0x65, 0x94, 0xA2,
	0x4B, 0x1F, 0x1A, 0x60, 0x52, 0x39, 0xE8, 0x23, 0xEE, 0x58, 0x39, 0x06, 0x3D, 0x22, 0x6A, 0x2D,
	0xD2, 0x91, 0x25, 0xA5, 0x2E, 0x71, 0x62, 0xA5, 0x0B, 0xC1, 0xE5, 0x6E, 0x43, 0x49, 0x7C, 0x58,
	0x46, 0x19, 0x9F, 0x45, 0x49, 0xC6, 0x40, 0x09, 0xA2, 0x99, 0x5B, 0x7B, 0x98, 0x7F, 0xA0, 0xD0,
};

/************************************************************************
*																		*
*																		*
*																		*
************************************************************************/

static unsigned char key_table2[0x100] =
{
	0xB8, 0xC5, 0xF7, 0x84, 0xE4, 0x5A, 0x23, 0x7B, 0xC8, 0x90, 0x1D, 0xF6, 0x5D, 0x09, 0x51, 0xC1,
	0x07, 0x24, 0xEF, 0x5B, 0x1D, 0x73, 0x90, 0x08, 0xA5, 0x70, 0x1C, 0x22, 0x5F, 0x6B, 0xEB, 0xB0,
	0x06, 0xC7, 0x2A, 0x3A, 0xD2, 0x66, 0x81, 0xDB, 0x41, 0x62, 0xF2, 0x97, 0x17, 0xFE, 0x05, 0xEF,
	0xA3, 0xDC, 0x22, 0xB3, 0x45, 0x70, 0x3E, 0x18, 0x2D, 0xB4, 0xBA, 0x0A, 0x65, 0x1D, 0x87, 0xC3,
	0x12, 0xCE, 0x8F, 0x9D, 0xF7, 0x0D, 0x50, 0x24, 0x3A, 0xF3, 0xCA, 0x70, 0x6B, 0x67, 0x9C, 0xB2,
	0xC2, 0x4D, 0x6A, 0x0C, 0xA8, 0xFA, 0x81, 0xA6, 0x79, 0xEB, 0xBE, 0xFE, 0x89, 0xB7, 0xAC, 0x7F,
	0x65, 0x43, 0xEC, 0x56, 0x5B, 0x35, 0xDA, 0x81, 0x3C, 0xAB, 0x6D, 0x28, 0x60, 0x2C, 0x5F, 0x31,
	0xEB, 0xDF, 0x8E, 0x0F, 0x4F, 0xFA, 0xA3, 0xDA, 0x12, 0x7E, 0xF1, 0xA5, 0xD2, 0x22, 0xA0, 0x0C,
	0x86, 0x8C, 0x0A, 0x0C, 0x06, 0xC7, 0x65, 0x18, 0xCE, 0xF2, 0xA3, 0x68, 0xFE, 0x35, 0x96, 0x95,
	0xA6, 0xFA, 0x58, 0x63, 0x41, 0x59, 0xEA, 0xDD, 0x7F, 0xD3, 0x1B, 0xA8, 0x48, 0x44, 0xAB, 0x91,
	0xFD, 0x13, 0xB1, 0x68, 0x01, 0xAC, 0x3A, 0x11, 0x78, 0x30, 0x33, 0xD8, 0x4E, 0x6A, 0x89, 0x05,
	0x7B, 0x06, 0x8E, 0xB0, 0x86, 0xFD, 0x9F, 0xD7, 0x48, 0x54, 0x04, 0xAE, 0xF3, 0x06, 0x17, 0x36,
	0x53, 0x3F, 0xA8, 0x11, 0x53, 0xCA, 0xA1, 0x95, 0xC2, 0xCD, 0xE6, 0x1F, 0x57, 0xB4, 0x7F, 0xAA,
	0xF3, 0x6B, 0xF9, 0xA0, 0x27, 0xD0, 0x09, 0xEF, 0xF6, 0x68, 0x73, 0x60, 0xDC, 0x50, 0x2A, 0x25,
	0x0F, 0x77, 0xB9, 0xB0, 0x04, 0x0B, 0xE1, 0xCC, 0x35, 0x31, 0x84, 0xE6, 0x22, 0xF9, 0xC2, 0xAB,
	0x95, 0x91, 0x61, 0xD9, 0x2B, 0xB9, 0x72, 0x4E, 0x10, 0x76, 0x31, 0x66, 0x0A, 0x0B, 0x2E, 0x83,
};

CFFXILandscapeMesh::CFFXILandscapeMesh(IDriver *in, unsigned int tid)
{
	p_driver=in;
	m_DefaultTextureID=tid;
	m_isTransform=true;
}


CFFXILandscapeMesh::~CFFXILandscapeMesh(void)
{
	for(auto it=m_meshBufferGroup.begin(); it!=m_meshBufferGroup.end(); ++it)
		delete *it;

	for(auto it=m_vectextureInfo.begin(); it!=m_vectextureInfo.end(); ++it) {
		p_driver->deleteTexture(it->id);
		it->name.clear();
	}

	m_vecMZB.clear();
	for(auto it=m_vecMMB.begin(); it!=m_vecMMB.end(); ++it) {
		for(auto nit=(*it)->m_SMMBVertexIndices.begin(); nit!=(*it)->m_SMMBVertexIndices.end(); ++nit) {
			nit->vecIndices.clear();
			nit->vecVertex.clear();
		}
		delete *it;
	}
	m_vecMMB.clear();

	for (auto mit = m_mapLT_Node.begin(); mit != m_mapLT_Node.end(); ++mit) {
		delete mit->second;
	}
	m_mapLT_Node.clear();
}


bool CFFXILandscapeMesh::animate(int frame)
{
	return true;
}

void CFFXILandscapeMesh::recalculateBoundingBox()
{
}

void CFFXILandscapeMesh::loadDependency( std::string dependStr)
{
	//std::list<std::string> listStr;
	//CUtility::explode(",", dependStr, listStr);

	//char filename[256];
	//std::string FN;
	//unsigned int dir=0,rem,fno;
	//for(auto it=listStr.begin(); it!=listStr.end(); ++it) {
	//	fno = atoi(it->c_str());
	//	rem = fno;
	//	if(fno>=1000000) {
	//		dir=fno/1000000;
	//		rem -= dir*1000000;
	//	}
	//	else {
	//		dir = 0;
	//		rem = fno;
	//	}
	//	if(dir==0) sprintf_s(filename,255,"%sROM\\%d\\%d.dat",ffxidir,rem/1000,rem%1000);  
	//	else       sprintf_s(filename,255,"%sROM%d\\%d\\%d.dat",ffxidir,dir,rem/1000,rem%1000);
	//	FN.assign(filename);

	//	ifstream ifs;
	//	ifs.open(FN.c_str(), ifstream::in | ifstream::binary);
	//	if(ifs.fail() ) {
	//		return;
	//	}
	//
	//	ifs.seekg (0, ifs.end);
	//	int length = ifs.tellg();
	//	ifs.seekg (0, ifs.beg);

	//	// allocate memory:
	//	char * buffer = new char [length];

	//	// read data as a block:
	//	ifs.read (buffer,length);
	//	ifs.close();

	//	FFXIFile *f = new FFXIFile(buffer,length);

	//	//extract file
	//	string textureName;

	//	m_vectextureInfo.clear();
	//	glm::u8 *ppImage=NULL;
	//	unsigned int width,height;
	//	unsigned int len;
	//	DATHEAD hd;
	//	SLandscapeTextureInfo tf;

	//	char *p, *start=f->FistData(&hd);
	//	for( p=f->FistData(&hd); p; p=f->NextData(&hd) )
	//	{
	//		int type = (int)hd.type;
	//		len = (hd.next&0x7ffff)*16;

	//		switch (type)
	//		{
	//			case 0x2e:  //MMB
	//				decode_mmb((unsigned char*)(p+16));
	//				extractMMB(p+sizeof(DATHEAD), len-sizeof(DATHEAD), true);							
	//			break;
	//			case 0x20:  //IMG
	//				tf.name = extractImageName(p+sizeof(DATHEAD), width, height, ppImage);
	//				tf.id=p_driver->createTexture(width,height,1,ppImage);
	//				m_vectextureInfo.push_back(tf);
	//				delete ppImage;
	//			break;
	//		}
	//	}
	//	delete f;
	//}
}

bool CFFXILandscapeMesh::loadModelFile(std::string FN, CSceneManager *mgr)
{
//	LocateFFXIDataFolder();
	unsigned int fno = atoi(FN.c_str());

	m_meshBufferGroup.clear();
	m_vectextureInfo.clear();

	//check dependency
	string dependStr;
//	if(mgr->checkDependency(fno, dependStr)) {
//		loadDependency(dependStr);
//	}

	int dir=0,rem=fno;
	if(fno>=1000000) {
		dir=fno/1000000;
		rem -= dir*1000000;
	}
	char filename[256];
	if(dir==0) sprintf_s(filename,255,"%sROM\\%d\\%d.dat",ffxidir,rem/1000,rem%1000);  
	else       sprintf_s(filename,255,"%sROM%d\\%d\\%d.dat",ffxidir,dir,rem/1000,rem%1000);
	FN.assign(filename);

	ifstream ifs;
	ifs.open(FN.c_str(), ifstream::in | ifstream::binary);
	if(ifs.fail() ) {
		return false;
	}
	
	ifs.seekg (0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg (0, ifs.beg);

    // allocate memory:
    char * buffer = new char [length];

    // read data as a block:
    ifs.read (buffer,length);
    ifs.close();

	FFXIFile *f = new FFXIFile(buffer,length);
	//extract file
	string textureName;

	glm::u8 *ppImage=NULL;
	unsigned int width,height;
	unsigned int len;
	int MZBc=0,MMBc=0,IMGc=0,BONEc=0,ANIMc=0,VERTc=0;
	DATHEAD hd;
	SLandscapeTextureInfo tf;

	char *p, *start=f->FistData(&hd);
	for( p=f->FistData(&hd); p; p=f->NextData(&hd) )
	{
		int type = (int)hd.type;
		len = (hd.next&0x7ffff)*16;

		switch (type)
		{
			case 0x1c:  //MZB
					if(decode_mzb((unsigned char*)(p+16),len))
						extractMZB(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
					else
						cout << "Unable to decode MZB: " << MZBc << endl;
					MZBc++;
			break;
			case 0x2e:  //MMB
					decode_mmb((unsigned char*)(p+16));
					extractMMB(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
					MMBc++;
			break;
			case 0x20:  //IMG	
					tf = extractImageName(p+sizeof(DATHEAD), width, height, ppImage);
//					tf.id=p_driver->createTexture(width,height,1,ppImage);
					m_vectextureInfo.push_back(tf);
//					delete ppImage;
					IMGc++;
			break;
			case 0x29:	//Bone
				//	p_mFFChar->SetBone(p+sizeof(DATHEAD),len-sizeof(DATHEAD));
				//	p_mFFChar->resetBoneMatrix();
					BONEc++;
			break;
			case 0x2B:	//animation
			//		p_mFFChar->AddAnimation(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
					ANIMc++;
			break;
			case 0x2a:	//vertex, create a meshBuffer for this part
				//	p_mFFChar->AddVertex(p+sizeof(DATHEAD),len-sizeof(DATHEAD));
					VERTc++;
			break;
		}
	}

	int tot_v = 0, tot_nv = 0;
	char buf[255];
	sprintf_s(buf,255,"MZB:%d MMB:%d IMG:%d BONE:%d ANIM:%d VERT:%d", MZBc,MMBc,IMGc,BONEc,ANIMc,VERTc);
	std::cout << buf << std::endl;

	if(!m_vecMZB.empty()) {
		int mzbIndex=0;
		std::cout << "Total B100: " << m_vecMZB.size() << std::endl;	
		for(auto it=m_vecMZB.begin(); it!=m_vecMZB.end(); ++it, ++mzbIndex) {
			length = findMMBIndex(&(*it));
			if (!lookupMMB(mzbIndex, length, &(*it))) {
				memcpy(buf, (*it).id, 16);
				buf[16] = 0x00;
				std::cout << "MMB not valid " << buf << std::endl;
				tot_nv++;
			}
			else
				tot_v++;
		}
	}
	std::cout << "Total Valid: " << tot_v << "  Total Invalid: " << tot_nv << std::endl;
	recalculateBoundingBox();
	return true;
}


CMeshBufferGroup* CFFXILandscapeMesh::getMeshBufferGroup(unsigned int i)
{
	if ( m_meshBufferGroup.empty() || (i >= m_meshBufferGroup.size()))
		return nullptr;
	return m_meshBufferGroup[i];
}

void CFFXILandscapeMesh::refreshMeshBufferGroup(int i, bool isMZB)
{
	char buf[100];
	int mmbIndex=0, mzbIndex=0, size=0;
	SMZBBlock100 *b100;
	for(auto it=m_meshBufferGroup.begin(); it!=m_meshBufferGroup.end(); ++it)
		delete *it;

	m_meshBufferGroup.clear();
	if(isMZB) {
		if(i!=-1) {
			//only refresh ith record MZB, but each B100 record can have many meshBuffer (numModel)
			b100 = &m_vecMZB[i];
			mmbIndex = findMMBIndex(b100);
			if(!lookupMMB( i, mmbIndex, b100 )) {
				memcpy(buf, b100->id, 16);
				buf[16]=0x00;
				std::cout << "MZB not valid (" << i << ")   id: " << buf << std::endl;
			}
		}
		else {
			//refresh all MZB
			size=m_vecMZB.size();
			for(int j=0; j<size; ++j) {
				b100 = &m_vecMZB[j];
				mmbIndex = findMMBIndex(b100);
				if(!lookupMMB( j, mmbIndex, b100 )) {
					memcpy(buf, b100->id, 16);
					buf[16]=0x00;
					std::cout << "MZB not valid (" << j << ")   id: " << buf << std::endl;
				}
			}
		}
	}
	else {
		//refresh MMB
//		mzbIndex = findMZBIndex(m_vecMMB[i]->m_SMMBHeader.imgID);
//		if (mzbIndex >= 0)
//			b100 = &m_vecMZB[mzbIndex];
//		else
//			b100 = nullptr;
//		if(!lookupMMB(-1, i, b100)) {
		if(!lookupMMB(-1, i, nullptr)) {
			std::cout << "MMB not valid (" << i << ")  " << std::endl;
		}
	}
}

void CFFXILandscapeMesh::refreshSpecialMeshBufferGroup(int i)
{
	char buf[100];
	int mmbIndex = 0, size = 0;
	SMZBBlock100 *b100;
	SSpecial sp;

	for (auto it = m_meshBufferGroup.begin(); it != m_meshBufferGroup.end(); ++it)
		delete *it;

	m_meshBufferGroup.clear();
	//refresh all MZB in Special
	sp = m_vecPVS[i];
	size = sp.vecMZBref.size();
	for (int j = 0; j<size; ++j) {
		b100 = &m_vecMZB[sp.vecMZBref[j]];
		mmbIndex = findMMBIndex(b100);
		if (!lookupMMB(sp.vecMZBref[j], mmbIndex, b100)) {
			memcpy(buf, b100->id, 16);
			buf[16] = 0x00;
			std::cout << "MZB not valid (" << sp.vecMZBref[j] << ")   id: " << buf << std::endl;
		}
	}
}

void CFFXILandscapeMesh::toggleMMBTransform()
{
	m_isTransform=!m_isTransform;
}

SLandscapeTextureInfo CFFXILandscapeMesh::extractImageName(char *p, glm::u32 &width, glm::u32 &height, glm::u8 *& ppImage)
{
	char buf[50];
	int len;
//	std::string imgname;
	char *bytePtr;
	unsigned char *pPalett;
	IMGINFOB1 *ib = nullptr;
	IMGINFO *i1 = nullptr;
	IMGINFO05 *i5 = nullptr;
	IMGINFO81_DDS *i81DDS = nullptr;
	u32 *pPal=nullptr;
	CDDS2Bmp ddsbmp;

	SLandscapeTextureInfo tf;

	IMGINFOA1 *ii = (IMGINFOA1*)(p);
	width = ii->imgx;
	height = ii->imgy;
	glm::u8 *pDDSBlock = nullptr;
	//copy name
	memcpy(buf, ii->id, 16);
	buf[16]=0x00;
//	imgname.assign(buf);
	tf.name.assign(buf);

	switch( ii->flg )
	{
	case 0xA1:	pDDSBlock = new u8[ii->size];
				memset(pDDSBlock, 0, ii->size);
				memcpy(pDDSBlock, p+sizeof(IMGINFOA1), ii->size);
//				ddsbmp.convert2BMP(pDDSBlock, width, height, ii->size, ii->widthbyte, ii->ddsType[0], NULL, ppImage);
				tf.id = p_driver->createDDSTexture(atoi(&ii->ddsType[0]), width, height, 1, pDDSBlock);
			
				delete[] pDDSBlock;
	break;
			
	case 0x01:
				//1088 + width*height....==> single byte index to palette
				//DATHEAD + 57 + (width*height+1024)
				i1 = (IMGINFO*)(p);
				len = width * height;
				ppImage = new glm::u8[len*4];
				bytePtr = p + sizeof(IMGINFO);
				for(int i=height-1; i>=0; --i) {
					for(int j=0; j<width; ++j) {
						memcpy(&ppImage[(i*width+j)*4], &i1->palet[(unsigned char)(*bytePtr)],4);
						bytePtr++;
					}
				}
				tf.id = p_driver->createTexture(width, height, 1, ppImage);
				delete[] ppImage;
	break;

	case 0x81:
				//contain a normal 0x01 image + DDS image
				i1 = (IMGINFO*)(p);
				len = width * height;
					
				//DDS, does not always contain a DDS image
				i81DDS = (IMGINFO81_DDS*)(p + sizeof(IMGINFO) + len);
				if( i81DDS->ddsType[0]=='3' && i81DDS->ddsType[1]=='T' && i81DDS->ddsType[2]=='X' && i81DDS->ddsType[3]=='D') {
					if(i81DDS->size!=len) {
						if(i81DDS->size==65536) {
							width=256; height=256;
						}
						else if(i81DDS->size==16384) {
							width=128; height=128;
						}
						else if(i81DDS->size==4096) {
							width=64; height=64;
						}
						else if(i81DDS->size==1024) {
							width=32; height=32;
						}
						else if(i81DDS->size==256) {
							width=16; height=16;
						}	
					}
					pDDSBlock = new glm::u8[i81DDS->size];
					memset(pDDSBlock, 0, i81DDS->size);
					//len is palette index (1byte)
					memcpy(pDDSBlock, (p+sizeof(IMGINFO)+len+sizeof(IMGINFO81_DDS)), i81DDS->size);
//					ddsbmp.convert2BMP(pDDSBlock, width, height, i81DDS->size, 32, i81DDS->ddsType[0], NULL, ppImage);
					tf.id = p_driver->createDDSTexture(atoi(&i81DDS->ddsType[0]), width, height, 1, pDDSBlock);

					delete[] pDDSBlock;
				}
				else {
					ppImage = new glm::u8[len*4];
					bytePtr = p + sizeof(IMGINFO);
					for(int i=height-1; i>=0; --i) {
						for(int j=0; j<width; ++j) {
							memcpy(&ppImage[(i*width+j)*4], &i1->palet[(unsigned char)(*bytePtr)],4);
							bytePtr++;
						}
					}
					tf.id = p_driver->createTexture(width, height, 1, ppImage);
					delete[] ppImage;
				}
	break;

	case 0xB1:		//0xB1
				ib = (IMGINFOB1*)(p);
				len = width * height;
				ppImage = new glm::u8[len*4];
				//extract color from palette 0xB1
				bytePtr = p + sizeof(IMGINFOB1);
				for(int i=height-1; i>=0; --i) {
					for(int j=0; j<width; ++j) {
						memcpy(&ppImage[(i*width+j)*4], &ib->palet[(unsigned char)(*bytePtr)],4);
						bytePtr++;
					}
				}
				tf.id = p_driver->createTexture(width, height, 1, ppImage);
				delete[] ppImage;
	break;

	case 0x05:
				//same as 0x01 but with 5 image
				//DATHEAD + 57 + (width*height+1024)*5
				i5 = (IMGINFO05*)(p);
				len = width * height;
				ppImage = new glm::u8[len*4];
				//extract color from palette
				len = sizeof(IMGINFO05);
					
				pPal = (u32*)(p+len);
				pPalett = (unsigned char*)(p+len+1024);
				for(int index=0; index<len; ++index) {
					memcpy(&ppImage[index*4], &pPal[pPalett[index]], 4);
				}
				tf.id = p_driver->createTexture(width, height, 1, ppImage);
				delete[] ppImage;
	break;

	case 0x91:
				//same as 0x01
				i1 = (IMGINFO*)(p);
				len = width * height;
				ppImage = new glm::u8[len*4];
				//extract color from palette
				bytePtr = p + sizeof(IMGINFO);
				for(int i=height-1; i>=0; --i) {
					for(int j=0; j<width; ++j) {
						memcpy(&ppImage[(i*width+j)*4], &i1->palet[(unsigned char)(*bytePtr)],4);
						bytePtr++;
					}
				}
				tf.id = p_driver->createTexture(width, height, 1, ppImage);
				delete[] ppImage;
	break;

	default:
			cout << "Unknown IMG type: " << ii->flg << endl;
	}

	//return imgname;
	return tf;
}


/************************************************************************
*																		*
*																		*
*																		*
************************************************************************/

int CFFXILandscapeMesh::BitCount(unsigned char x)
{
	int n = 0;
	n += ((x >> 7) & 1);
	n += ((x >> 6) & 1);
	n += ((x >> 5) & 1);
	n += ((x >> 4) & 1);
	n += ((x >> 3) & 1);
	n += ((x >> 2) & 1);
	n += ((x >> 1) & 1);
	n += ((x >> 0) & 1);
	return n;
}

/************************************************************************
*																		*
*																		*
*																		*
************************************************************************/

void CFFXILandscapeMesh::decode_mmb2(unsigned char *p)
{
	if(p[6] == 0xFF && p[7] == 0xFF)	{
		int decode_length = (p[0] << 0) | (p[1] << 8) | (p[2] << 16);
		unsigned int key1 = p[5] ^ 0xF0;
		unsigned int key2 = key_table2[key1] ;
		int key_counter = 0;

		unsigned int decode_count = ((decode_length - 8) & ~0xf) / 2;

		unsigned int *data1 = (unsigned int *)(p + 8 + 0);
		unsigned int *data2 = (unsigned int *)(p + 8 + decode_count);
		for(unsigned int pos = 0; pos < decode_count; pos += 8)
		{
			if(key2 & 1)
			{
				unsigned int tmp;

				tmp = data1[0];
				data1[0] = data2[0];
				data2[0] = tmp;

				tmp = data1[1];
				data1[1] = data2[1];
				data2[1] = tmp;
			}
			key1 += 9;
			key2 += key1;
			data1 += 2;
			data2 += 2;
		}
	}
}

/************************************************************************
*																		*
*																		*
*																		*
************************************************************************/

void CFFXILandscapeMesh::decode_mmb(unsigned char*p)
{
	if(p[3] >= 5)
	{
		int decode_length = (p[0] << 0) | (p[1] << 8) | (p[2] << 16);
		unsigned int key = key_table[p[5] ^ 0xF0];
		int key_counter = 0;

		for(int pos = 8; pos < decode_length; pos++)
		{
			unsigned int x = ((key & 0xFF) << 8) | (key & 0xFF);
			key += ++key_counter;

			p[pos] ^= (x >> (key & 7));
			key += ++key_counter;
		}
	}
	decode_mmb2(p);
}

/************************************************************************
*																		*
*																		*
*																		*
************************************************************************/
//
//void CFFXILandscapeMesh::decode_mzb(unsigned char* p)
//{
//	if (p[3] >= 0x1B)
//	{
//		int decode_length = (p[0] << 0) | (p[1] << 8) | (p[2] << 16);
//		unsigned int key = key_table[p[7] ^ 0xFF];
//		int key_counter = 0;
//
//		for (int pos = 8; pos < decode_length; )
//		{
//			int xor_length = ((key >> 4) & 7) + 16;
//
//			if ((key & 1) && (pos + xor_length < decode_length))
//			{
//				for (int i = 0; i < xor_length; i++)
//	 			{
//					p[pos+i] ^= 0xFF;
//				}
//			}
//			key += ++key_counter;
//			pos += xor_length;
//		}
//		int node_count = (p[4] << 0) | (p[5] << 8) | (p[6] << 16);
//		OBJINFO *node = (OBJINFO *)(p+32);
//		for(int i = 0; i < node_count; i++)
//		{
//			for(int i = 0; i < 16; i++)
//			{
//				node->id[i] ^= 0x55;
//			}
//			node++;
//		}
//	}
//}

bool CFFXILandscapeMesh::decode_mzb(unsigned char* p, unsigned int maxLen)
{
	if (p[3] >= 0x1B)
	{
		unsigned int decode_length = (p[0] << 0) | (p[1] << 8) | (p[2] << 16);
		if(decode_length > maxLen)
			return false;

		unsigned int key = key_table[p[7] ^ 0xFF];
		int key_counter = 0;

		for (unsigned int pos = 8; pos < decode_length; )
		{
			unsigned int xor_length = ((key >> 4) & 7) + 16;

			if ((key & 1) && (pos + xor_length < decode_length))
			{
				for (unsigned int i = 0; i < xor_length; i++)
	 			{
					p[pos+i] ^= 0xFF;
				}
			}
			key += ++key_counter;
			pos += xor_length;
		}

		int node_count = (p[4] << 0) | (p[5] << 8) | (p[6] << 16);
		SMZBBlock100 *node = (SMZBBlock100 *)(p+32);
		for(int i = 0; i < node_count; i++)
		{
			for(int j = 0; j < 16; j++)
			{
				node->id[j] ^= 0x55;
			}
			node++;
		}
	}
	return true;
}

void CFFXILandscapeMesh::extractMMB(char *p, unsigned int len, bool isDepend)
{
	unsigned int totalsize=0,offset,maxRange;

	CMMB *pmmb = new CMMB;
	pmmb->m_Size=0;
	pmmb->m_SMMBVertexIndices.clear();
	pmmb->m_SMMBHeader.pieces=0;

	SMMBHEAD *pMMB = (SMMBHEAD*)(p+totalsize);
	SMMBHEAD2 *pMMB2 = (SMMBHEAD2*)(p+totalsize);
	totalsize += sizeof(SMMBHEAD);

	if ((pMMB->id[0] == 'M') && (pMMB->id[1] == 'M') && (pMMB->id[2] == 'B')) {
		pmmb->m_Size = pMMB->next * 16;
		pmmb->m_type = 1;
	}
	else {
		pmmb->m_Size = pMMB2->MMBSize;
		pmmb->m_type = 2;
	}
	
	//SMMBHeader (imgID reference by MZB)
	SMMBHeader *pMMBH = (SMMBHeader*)(p+totalsize);
	trimSpace(pMMBH->imgID, pMMBH->imgID, 16);
	totalsize += sizeof(SMMBHeader);
	pmmb->m_SMMBHeader = *pMMBH;

	SMMBBlockHeader *pMMBBH = nullptr;
	SMMBModelHeader *pMMBMH = nullptr;
	SMMBBlockVertex *pMMBBV = nullptr;
	SMMBBlockVertex2 *pMMBBV2 = nullptr;
	SMMBBlockVertex2 MMBBV2;
	SMMBVertexIndices mmblock;

	list<unsigned int> listOffset;
	if(pMMBH->offsetBlockHeader==0 ) {
		if(pMMBH->pieces!=0 ) {
			//32byte, assuming, even if it exceed the limit, it doesnt matter since the first offset is recorded
			for(int i=0; i<8; ++i) {
				offset = (*(unsigned int*)(p+totalsize));
				if(offset!=0)
					listOffset.push_back(offset);
				totalsize += 4;
			}
		}
		else
			listOffset.push_back(totalsize);
	}
	else {
		listOffset.push_back(pMMBH->offsetBlockHeader);
		maxRange = pMMBH->offsetBlockHeader - totalsize;
		if(maxRange>0) {			
			for(unsigned int i=0; i<maxRange; i+=4) {
				offset = (*(unsigned int*)(p+totalsize));
				if(offset!=0)
					listOffset.push_back(offset);
				totalsize += 4;
			}
			if( listOffset.size() != pMMBH->pieces )
				cout << "mismatch offset" << endl;
		}
		totalsize = pMMBH->offsetBlockHeader;
	}

	glm::u16 i1, i2, i3;
	for(int piece=1; piece<=pMMBH->pieces; piece++) {
		if(!listOffset.empty()) {
			totalsize = listOffset.front();
			listOffset.pop_front();
		}
		//SMMBBLockHeader
		pMMBBH = (SMMBBlockHeader*)(p+totalsize);
		totalsize += sizeof(SMMBBlockHeader);
		
		if(pMMBBH->numModel > 50) {
			cout << "  courrupt MMB, numModel: " << pMMBBH->numModel << endl;
			return;
		}
		for(int k=0; k<pMMBBH->numModel; ++k) {			
			if( totalsize + sizeof(SMMBModelHeader) > len )
				break;
	
			//SMMBModelHeader
			pMMBMH = (SMMBModelHeader*)(p+totalsize);
			totalsize += sizeof(SMMBModelHeader);
			memcpy(mmblock.textureName, pMMBMH->textureName, 16);

			//Vertex/Normal Info for each model
//			pMMBMH->vertexsize &= 0xffff;
			mmblock.numVertex = pMMBMH->vertexsize;
			mmblock.blending = pMMBMH->blending;
//debug
			switch (pMMBMH->blending)
			{
			case 0x00:
			case 0x1000:
			case 0x2000:
			case 0x4000:
			case 0x8000:
			case 0xA000:
				break;
			default : std::cout << " unknown blendflag: 0x" << std::hex << std::uppercase << pMMBMH->blending << std::dec << std::nouppercase << std::endl;
			}

			mmblock.vecVertex.clear();	
			for(int i=0; i<pMMBMH->vertexsize; ++i) {
				//SMMBBlockVertex / SMMBBlockVertex2
				if(pMMB2->d3==2) {
					pMMBBV2 = (SMMBBlockVertex2*)(p+totalsize);
					MMBBV2.x = pMMBBV2->x;
					MMBBV2.y = pMMBBV2->y;
					MMBBV2.z = pMMBBV2->z;
					MMBBV2.dx = pMMBBV2->dx;
					MMBBV2.dy = pMMBBV2->dy;
					MMBBV2.dz = pMMBBV2->dz;
					MMBBV2.hx = pMMBBV2->hx;
					MMBBV2.hy = pMMBBV2->hy;
					MMBBV2.hz = pMMBBV2->hz;
					MMBBV2.color = pMMBBV2->color;
					MMBBV2.u = pMMBBV2->u;
					MMBBV2.v = pMMBBV2->v;
					totalsize += sizeof(SMMBBlockVertex2);
				}
				else {
					pMMBBV = (SMMBBlockVertex*)(p+totalsize);
					MMBBV2.x = pMMBBV->x;
					MMBBV2.y = pMMBBV->y;
					MMBBV2.z = pMMBBV->z;
					MMBBV2.dx =0;
					MMBBV2.dy =0;
					MMBBV2.dz =0;
					MMBBV2.hx = pMMBBV->hx;
					MMBBV2.hy = pMMBBV->hy;
					MMBBV2.hz = pMMBBV->hz;
					MMBBV2.color = pMMBBV->color;
					MMBBV2.u = pMMBBV->u;
					MMBBV2.v = pMMBBV->v;
					totalsize += sizeof(SMMBBlockVertex);
				}
				mmblock.vecVertex.push_back(MMBBV2);
			}

			mmblock.numIndices = (*(unsigned int*)(p+totalsize))&0xffff;
			totalsize += 4;
			mmblock.vecIndices.clear();

			if( (pMMB->id[0]=='M' && pMMB->type==0) || (pMMB->id[0]!='M' && pMMB2->d3==2) ) {
				//simple 3 indices triangle
				pMMBBH->numFace = mmblock.numIndices/3;
				mmblock.drawType = E_TRIANGLE_LIST;
				for(int i=0,j=0; i<pMMBBH->numFace; i++) {
					i1 = ((*(glm::u16*)(p+totalsize+j*2))&0xffff);
					mmblock.vecIndices.push_back(i1);
					j++;
					i2 = ((*(glm::u16*)(p+totalsize+j*2))&0xffff);
					mmblock.vecIndices.push_back(i2);
					j++;
					i3 = ((*(glm::u16*)(p+totalsize+j*2))&0xffff);
					mmblock.vecIndices.push_back(i3);
					j++;
				}
			}
			else {
				i2 = 0, i3 = 1;
				mmblock.drawType = E_TRIANGLE_STRIP;
				for(int j=0; j<mmblock.numIndices; ++j) {
					if (j > 2) {
						i3 = i2;
						i2 = i1;
					}
					i1 = ((*(glm::u16*)(p+totalsize+j*2))&0xffff);
					//if there r already 3 similiar, remove the last one
//					if (i1 == i2 && i2 == i3)
//						continue;

					mmblock.vecIndices.push_back(i1);
				}


				////triangleStrip with special encoding
				////3 similiar index==STOP, start new triangleStrip with new index
				////2 similiar index==END triangleStrip, start new triangleStrip with last index
				//i1 = ((*(glm::u16*)(p+totalsize))&0xffff);
				//mmblock.vecIndices.push_back(i1);
				//i2 = ((*(glm::u16*)(p+totalsize+2))&0xffff);
				//mmblock.vecIndices.push_back(i2);
				//i3 = ((*(glm::u16*)(p+totalsize+4))&0xffff);
				//mmblock.vecIndices.push_back(i3);
				//i1 = i2;
				//i2 = i3;

				//for(unsigned int j=3; j<mmblock.numIndices;) {
				//	if( i1==i2 ) {
				//		i2 = ((*(glm::u16*)(p+totalsize+j*2))&0xffff);
				//		++j;
				//	}
				//	i3 = ((*(glm::u16*)(p+totalsize+j*2))&0xffff);
				//	if( i2!=i3 ) {
				//		if(j%2) {
				//			//make clockwise
				//			i4 = i1;
				//			i1 = i2;
				//			i2 = i4;
				//			mmblock.vecIndices.push_back(i1);
				//			mmblock.vecIndices.push_back(i2);
				//			mmblock.vecIndices.push_back(i3);
				//			//reverse back, i1 is already i2
				//			i2 = i3;
				//		}
				//		else {
				//			mmblock.vecIndices.push_back(i1);
				//			mmblock.vecIndices.push_back(i2);
				//			mmblock.vecIndices.push_back(i3);
				//			i1 = i2;
				//			i2 = i3;
				//		}
				//		++j;
				//	}			
				//	else {
				//		i1 = i2;
				//		i2 = i3;
				//		++j;
				//		//check if 2 similiar or 3 similiar
				//		i3 = ((*(glm::u16*)(p+totalsize+j*2))&0xffff);
				//		if( i2==i3 ) {
				//			//3 similiar, is mostly follow by 2 similiar
				//			++j;
				//			i1 = ((*(glm::u16*)(p+totalsize+j*2))&0xffff);
				//			++j;
				//			i2 = ((*(glm::u16*)(p+totalsize+j*2))&0xffff);
				//			++j;
				//			//if( f.i1==f.i2) {
				//			//	//yes, 2 similiar
				//			//	f.i2 = ((*(unsigned int*)(p+offset+j*2))&0xffff);
				//			//	++j;
				//			//}
				//		}
				//		else {
				//			//might need to reset the rotation???
				//			i1=i2;
				//			i2=i3;
				//			++j;
				//		}
				//	}
				//}
			}
			totalsize += mmblock.numIndices*2;
			if( mmblock.numIndices % 2 > 0 )
				totalsize += 2;	//padding 2 byte

			pmmb->m_SMMBVertexIndices.push_back(mmblock);
		}
	}
	if(!isDepend)
		m_vecMMB.push_back(pmmb);
	else
		m_vecDependMMB.push_back(pmmb);
}

//replace by getMZB100Matrix()
//void CFFXILandscapeMesh::MMBTransform(SMZBBlock100 *b84, glm::vec3 &v)
//{
//	if( b84==nullptr )
//		return;
//
//	float x = v.x;
//	float y = v.y;
//	float z = v.z;
//
//	//v.x = x + b84->fTransX;
//	//v.y = y + b84->fTransY;
//	//v.z = z + b84->fTransZ;
//
//	//rotation...assuming rotate order x->y->z
//	//matrix: 
//	//|cosYcosZ,    cosZsinXsinY-cosXsinZ,     cosXcosZsinY+sinXsinZ,    Tx|
//	//|cosYsinZ,    cosXcosZ+sinXsinYsinZ,     -cosZsinX+cosXsinYsinZ,   Ty|
//	//|-sinY,       cosYsinX,                  cosXcosY,                 Tz|
//	//|0,           0,                         0,                        1 |
//	//v.x = cos(b84->fRotY)*cos(b84->fRotZ)*x + (cos(b84->fRotZ)*sin(b84->fRotX)*sin(b84->fRotY)-cos(b84->fRotX)*sin(b84->fRotZ))*y + (cos(b84->fRotX)*cos(b84->fRotZ)*sin(b84->fRotY) + sin(b84->fRotX)*sin(b84->fRotZ))*z + b84->fTransX;
//	//v.y = cos(b84->fRotY)*sin(b84->fRotZ)*x + (cos(b84->fRotX)*cos(b84->fRotZ)+sin(b84->fRotX)*sin(b84->fRotY)*sin(b84->fRotZ))*y + (-cos(b84->fRotZ)*sin(b84->fRotX)+cos(b84->fRotX)*sin(b84->fRotY)*sin(b84->fRotZ))*z + b84->fTransY;
//	//v.z = -sin(b84->fRotY)*x + (cos(b84->fRotY)*sin(b84->fRotX))*y + (cos(b84->fRotX)*cos(b84->fRotY))*z + b84->fTransZ;
//
//	v.x = cos(b84->fRotY)*cos(b84->fRotZ)*b84->fScaleX*x + (cos(b84->fRotZ)*sin(b84->fRotX)*sin(b84->fRotY)-cos(b84->fRotX)*sin(b84->fRotZ))*y + (cos(b84->fRotX)*cos(b84->fRotZ)*sin(b84->fRotY) + sin(b84->fRotX)*sin(b84->fRotZ))*z + b84->fTransX;
//	v.y = cos(b84->fRotY)*sin(b84->fRotZ)*x + (cos(b84->fRotX)*cos(b84->fRotZ)+sin(b84->fRotX)*sin(b84->fRotY)*sin(b84->fRotZ))*b84->fScaleY*y + (-cos(b84->fRotZ)*sin(b84->fRotX)+cos(b84->fRotX)*sin(b84->fRotY)*sin(b84->fRotZ))*z + b84->fTransY;
//	v.z = -sin(b84->fRotY)*x + (cos(b84->fRotY)*sin(b84->fRotX))*y + (cos(b84->fRotX)*cos(b84->fRotY))*b84->fScaleZ*z + b84->fTransZ;
//
//
//	if( (floor(v.x)==-1983 ) && (floor(v.y)==-3) && (floor(v.z)==1182) ) {
////	if( (v.x==-1982.31) && (v.y==-2.323) && (v.z==1182.4) ) {
//		x = 1;
//	}
//}

void CFFXILandscapeMesh::extractMZB(char *p, unsigned int len)
{
	SMZBHeader *mzbh1 = (SMZBHeader*)p;
//	mzbh1->totalRecord100 &=0xffffff;

	char buf[17];
	buf[16]=0x00;
	SMZBBlock100 *oj, ob;
	SMZBBlock84 *b84;
	SMZBBlock92b *b92;
	int noj = mzbh1->totalRecord100;

	//offsetHeader2==0 ==> no collision detection
	if( (noj*sizeof(SMZBBlock84)+sizeof(SMZBHeader))==mzbh1->offsetEndRecord100 ) {
		//for MMB object
		b84 = (SMZBBlock84*)(p+32);

		for(int i=0; i<noj; ++i ) {
//			memcpy(&ob.id, b84->id, 16);
			trimSpace(ob.id, b84->id, 16);
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
			//place it in m_vecMZB, lookupMMB at later stage
			m_vecMZB.push_back(ob);
			b84++;
		}
	}
	else if( (noj*sizeof(SMZBBlock100)+sizeof(SMZBHeader))==mzbh1->offsetEndRecord100 ) {
		//block100, for the MZB object
		oj =  (SMZBBlock100 *)(p+32);
		for(int i=0; i<noj; ++i) {
			trimSpace(oj->id, oj->id, 16);
			m_vecMZB.push_back(*oj);
			oj++;
		}
	}
	else if( (noj*sizeof(SMZBBlock92b)+sizeof(SMZBHeader))==mzbh1->offsetEndRecord100 ) {
		b92 = (SMZBBlock92b*)(p+32);
		for(int i=0; i<noj; ++i ) {
//			memcpy(&ob.id, b92->id, 16);
			trimSpace(ob.id, b92->id, 16);
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
			//place it in m_vecMZB, lookupMMB at later stage
			m_vecMZB.push_back(ob);
			b92++;
		}
	}
	else {
		std::cout << "Unknown MZB " << std::endl;
	}

	//extract PVS
	SSpecial sp;
	int sizePVS = 0, index;
	bool havelooseTree = false, havePVS = false;
	havelooseTree = mzbh1->offsetlooseTree > mzbh1->offsetEndRecord100 && mzbh1->offsetEndlooseTree < mzbh1->offsetHeader2;
	unsigned int totalsize,val;

	if (havelooseTree) {
		if (mzbh1->offsetlooseTree - mzbh1->offsetEndRecord100 > 16) {
			havePVS = true;
		}
	}
	else {
		if (mzbh1->offsetlooseTree>0) {
			sizePVS = mzbh1->offsetlooseTree;
			havePVS = true;
		}
	}
	if (havePVS) {
		totalsize = mzbh1->offsetEndRecord100;
		m_vecPVS.clear();
		if (sizePVS == 0) {
			sizePVS = *(unsigned int*)(p + totalsize);
			totalsize += 4;
		}

		for (index = 1; index <= sizePVS; ++index) {
			sp.vecMZBref.clear();
			sp.size = *(unsigned int*)(p + totalsize);
			totalsize += 4;
			if (totalsize >= mzbh1->offsetHeader2)
				break;

			for (int i = 1; i <= sp.size; ++i) {
				val = *(unsigned int*)(p + totalsize);
				totalsize += 4;

				sp.vecMZBref.push_back(val);
				if (totalsize >= mzbh1->offsetHeader2)
					break;
			}
			m_vecPVS.push_back(sp);
		}
	}

	if (havelooseTree) {
		totalsize = mzbh1->offsetlooseTree;
		//24 float (min/max boundingRect - 8point), 8 int (MZBref + num + 6 nextNode)
		unsigned int totalMZB = 0;
		SMZBBlock128 *pB128 = nullptr;
		do {
			pB128 = (SMZBBlock128*)(p + totalsize);
			pLT_Node pLT = new LT_Node;
			pLT->addrID = totalsize;

			//min
			pLT->bbox.x1 = pB128->x1;
			if (pB128->x2 < pLT->bbox.x1) pLT->bbox.x1 = pB128->x2;
			if (pB128->x3 < pLT->bbox.x1) pLT->bbox.x1 = pB128->x3;
			if (pB128->x4 < pLT->bbox.x1) pLT->bbox.x1 = pB128->x4;
			if (pB128->x5 < pLT->bbox.x1) pLT->bbox.x1 = pB128->x5;
			if (pB128->x6 < pLT->bbox.x1) pLT->bbox.x1 = pB128->x6;
			if (pB128->x7 < pLT->bbox.x1) pLT->bbox.x1 = pB128->x7;
			if (pB128->x8 < pLT->bbox.x1) pLT->bbox.x1 = pB128->x8;

			pLT->bbox.y1 = pB128->y1;
			if (pB128->y2 < pLT->bbox.y1) pLT->bbox.y1 = pB128->y2;
			if (pB128->y3 < pLT->bbox.y1) pLT->bbox.y1 = pB128->y3;
			if (pB128->y4 < pLT->bbox.y1) pLT->bbox.y1 = pB128->y4;
			if (pB128->y5 < pLT->bbox.y1) pLT->bbox.y1 = pB128->y5;
			if (pB128->y6 < pLT->bbox.y1) pLT->bbox.y1 = pB128->y6;
			if (pB128->y7 < pLT->bbox.y1) pLT->bbox.y1 = pB128->y7;
			if (pB128->y8 < pLT->bbox.y1) pLT->bbox.y1 = pB128->y8;

			pLT->bbox.z1 = pB128->z1;
			if (pB128->z2 < pLT->bbox.z1) pLT->bbox.z1 = pB128->z2;
			if (pB128->z3 < pLT->bbox.z1) pLT->bbox.z1 = pB128->z3;
			if (pB128->z4 < pLT->bbox.z1) pLT->bbox.z1 = pB128->z4;
			if (pB128->z5 < pLT->bbox.z1) pLT->bbox.z1 = pB128->z5;
			if (pB128->z6 < pLT->bbox.z1) pLT->bbox.z1 = pB128->z6;
			if (pB128->z7 < pLT->bbox.z1) pLT->bbox.z1 = pB128->z7;
			if (pB128->z8 < pLT->bbox.z1) pLT->bbox.z1 = pB128->z8;

			//max
			pLT->bbox.x2 = pB128->x1;
			if (pB128->x2 > pLT->bbox.x2) pLT->bbox.x2 = pB128->x2;
			if (pB128->x3 > pLT->bbox.x2) pLT->bbox.x2 = pB128->x3;
			if (pB128->x4 > pLT->bbox.x2) pLT->bbox.x2 = pB128->x4;
			if (pB128->x5 > pLT->bbox.x2) pLT->bbox.x2 = pB128->x5;
			if (pB128->x6 > pLT->bbox.x2) pLT->bbox.x2 = pB128->x6;
			if (pB128->x7 > pLT->bbox.x2) pLT->bbox.x2 = pB128->x7;
			if (pB128->x8 > pLT->bbox.x2) pLT->bbox.x2 = pB128->x8;

			pLT->bbox.y2 = pB128->y1;
			if (pB128->y2 > pLT->bbox.y2) pLT->bbox.y2 = pB128->y2;
			if (pB128->y3 > pLT->bbox.y2) pLT->bbox.y2 = pB128->y3;
			if (pB128->y4 > pLT->bbox.y2) pLT->bbox.y2 = pB128->y4;
			if (pB128->y5 > pLT->bbox.y2) pLT->bbox.y2 = pB128->y5;
			if (pB128->y6 > pLT->bbox.y2) pLT->bbox.y2 = pB128->y6;
			if (pB128->y7 > pLT->bbox.y2) pLT->bbox.y2 = pB128->y7;
			if (pB128->y8 > pLT->bbox.y2) pLT->bbox.y2 = pB128->y8;

			pLT->bbox.z2 = pB128->z1;
			if (pB128->z2 > pLT->bbox.z2) pLT->bbox.z2 = pB128->z2;
			if (pB128->z3 > pLT->bbox.z2) pLT->bbox.z2 = pB128->z3;
			if (pB128->z4 > pLT->bbox.z2) pLT->bbox.z2 = pB128->z4;
			if (pB128->z5 > pLT->bbox.z2) pLT->bbox.z2 = pB128->z5;
			if (pB128->z6 > pLT->bbox.z2) pLT->bbox.z2 = pB128->z6;
			if (pB128->z7 > pLT->bbox.z2) pLT->bbox.z2 = pB128->z7;
			if (pB128->z8 > pLT->bbox.z2) pLT->bbox.z2 = pB128->z8;

			if (pB128->offset3 != 0)
				pLT->vecChild.push_back(pB128->offset3);

			if (pB128->offset4 != 0)
				pLT->vecChild.push_back(pB128->offset4);

			if (pB128->offset5 != 0)
				pLT->vecChild.push_back(pB128->offset5);

			if (pB128->offset6 != 0)
				pLT->vecChild.push_back(pB128->offset6);

			if (pB128->offset7 != 0)
				pLT->vecChild.push_back(pB128->offset7);

			if (pB128->offset8 != 0)
				pLT->vecChild.push_back(pB128->offset8);

			pLT->noChild = pLT->vecChild.size();
			pLT->noMZB = pB128->numMZB;

			if (pB128->numMZB != 0) {
				totalMZB += pB128->numMZB;
				//MZB index
				for (int j = 0; j<pB128->numMZB; ++j) {
					pLT->vecMZBref.push_back( *(unsigned int*)(p + pB128->offsetMZB + j * 4));
				}
			}
			//add record to map
			m_mapLT_Node.insert(pair<unsigned int, pLT_Node>(pLT->addrID, pLT));

			totalsize += sizeof(SMZBBlock128);
			if (totalsize > mzbh1->offsetEndlooseTree)
				break;
		} while (totalMZB < mzbh1->totalRecord100);
	}
return;

	//the following is for collision detection
	//SMZBHeader2
	unsigned int r1;
	SMZBHeader2 *mzbh2 = (SMZBHeader2*)(p+mzbh1->offsetHeader2);
	m_lastIndices=0;
	SLandscapeTextureInfo ltex;

	std::vector<IMeshBuffer*> vecMeshBuffer;
	CMeshBufferGroup *pMBG = new CMeshBufferGroup(0,0);
	pMBG->m_maxBoundRect= glm::vec3(1000, 1000, 1000);
	pMBG->m_minBoundRect= glm::vec3(-1000, -1000, -1000);

	//create a mb for each model
	IMeshBuffer *mb = new IMeshBuffer(E_TRIANGLE_LIST);
	vecMeshBuffer.push_back(mb);
	//set a checker texture as default, if none is available
//	mb->updateTextureID(m_DefaultTextureID);
	ltex=m_vectextureInfo[0];
	mb->updateTextureID(ltex.id);
	mb->generateFrameBuffer(1);

	int mbIndex=0;
	SMZBBlock16 *pB16=nullptr;
	int sizeV = mzbh2->totalBlock16;
	totalsize = mzbh2->offsetBlock16;
	unsigned int tmpN;
	for(int i=0; i<sizeV; ++i) {
		mbIndex=0;
		mb=vecMeshBuffer[0];
		pB16 = (SMZBBlock16*)(p+totalsize);
		if( pB16->i4==0 ) {
			//use i2, i3
			decodeMesh(p, pB16->i2, pB16->i3, mb);
			totalsize += 16;
		}
		else {
			decodeMesh(p, pB16->i2, pB16->i3, mb);
			totalsize += 16;
			tmpN=pB16->i4;
			do {
				r1 = (*(unsigned int*)(p+totalsize));
				totalsize +=4;
				if(tmpN==0)
					tmpN=r1;
				else {
					mbIndex++;
					if(mbIndex==vecMeshBuffer.size()) {
						mb = new IMeshBuffer(E_TRIANGLE_LIST);
						vecMeshBuffer.push_back(mb);
						ltex=m_vectextureInfo[mbIndex];
						mb->updateTextureID(ltex.id);
						mb->generateFrameBuffer(1);
					}
					else
						mb = vecMeshBuffer[mbIndex];

					decodeMesh(p, tmpN, r1, mb);
					tmpN=0;
				}
			} while(r1!=0);
		}
	}
	for(auto it=vecMeshBuffer.begin(); it!=vecMeshBuffer.end(); ++it)
		pMBG->addMeshBuffer(*it);

	addMeshBufferGroup(pMBG);
	m_lastIndices=0;
}

void CFFXILandscapeMesh::trimSpace(char *des, char *src, int len)
{
	int i, size = len, pos1=0;
	for(i=0; i<size; ++i) {
		//find middle space, if any
		if( *(src+i)==0x20 ) {
			if(pos1==0)
				pos1=i;
		}
		else if(pos1!=0) {
			//there is already a space, followed by a char
			break;
		}
	}
	if(i==size) {
		memcpy(des,src,16);
	}
	else {
		//pad trailing space, if any
		pos1=size-i;
		memcpy(des,src+i,pos1);
		for(i=pos1; i<size; ++i)
			des[i]=0x20;
	}
}

int CFFXILandscapeMesh::findMZBIndex(char *id)
{
	int index = 0;
	for (auto it = m_vecMZB.begin(); it != m_vecMZB.end(); ++it,++index) {
		if (memcmp(id, it->id, 16) == 0)
			return index;
	}
	return -1;
}

int CFFXILandscapeMesh::findMMBIndex(SMZBBlock100 *in)
{
	int i=0;
	for(auto it = m_vecMMB.begin(); it!=m_vecMMB.end(); ++it,++i) {
		//id/imgID is not null-terminated
		if(memcmp(in->id, (*it)->m_SMMBHeader.imgID,16)==0) {
			return i;
		}
	}

	//check dependent MMB
	i=0;
	for(auto it = m_vecDependMMB.begin(); it!=m_vecDependMMB.end(); ++it, ++i) {
		if(memcmp(in->id, (*it)->m_SMMBHeader.imgID,16)==0) {
			return i+1000;
		}
	}
	return -1;
}

void CFFXILandscapeMesh::getMZB100Matrix(SMZBBlock100 *b84, glm::mat4 &out)
{
	//rotation...assuming rotate order x->y->z
	//matrix: 
	//|cosYcosZ,    cosZsinXsinY-cosXsinZ,     cosXcosZsinY+sinXsinZ,    Tx|
	//|cosYsinZ,    cosXcosZ+sinXsinYsinZ,     -cosZsinX+cosXsinYsinZ,   Ty|
	//|-sinY,       cosYsinX,                  cosXcosY,                 Tz|
	//|0,           0,                         0,                        1 |
	glm::vec4 col1,col2,col3,col4;

//	col1.x = b84->fScaleX*(cos(b84->fRotY)*cos(b84->fRotZ));
	col1.x = (cos(b84->fRotY)*cos(b84->fRotZ));
	col1.y = cos(b84->fRotY)*sin(b84->fRotZ);
	col1.z = -sin(b84->fRotY);
	col1.w = 0;

	col2.x = (cos(b84->fRotZ)*sin(b84->fRotX)*sin(b84->fRotY)-cos(b84->fRotX)*sin(b84->fRotZ));
//	col2.y = b84->fScaleY*(cos(b84->fRotX)*cos(b84->fRotZ)+sin(b84->fRotX)*sin(b84->fRotY)*sin(b84->fRotZ));
	col2.y = (cos(b84->fRotX)*cos(b84->fRotZ)+sin(b84->fRotX)*sin(b84->fRotY)*sin(b84->fRotZ));
	col2.z = (cos(b84->fRotY)*sin(b84->fRotX));
	col2.w = 0;

	col3.x = (cos(b84->fRotX)*cos(b84->fRotZ)*sin(b84->fRotY) + sin(b84->fRotX)*sin(b84->fRotZ));
	col3.y = (-cos(b84->fRotZ)*sin(b84->fRotX)+cos(b84->fRotX)*sin(b84->fRotY)*sin(b84->fRotZ));
//	col3.z = b84->fScaleZ*(cos(b84->fRotX)*cos(b84->fRotY));
	col3.z = (cos(b84->fRotX)*cos(b84->fRotY));
	col3.w = 0;

	col4.x = b84->fTransX;
	col4.y = b84->fTransY;
	col4.z = b84->fTransZ;
	col4.w = 1;

	out[0]=col1;
	out[1]=col2;
	out[2]=col3;
	out[3]=col4;
}

bool CFFXILandscapeMesh::lookupMMB(int mzbIndex, int mmbIndex, SMZBBlock100 *in)
{
	glm::vec3 min,max,tmp, br1,br2,br3,br4,br5,br6;
	glm::vec3 v,n;
	glm::vec2 uv;
	glm::vec4 color;
	std::vector<glm::vec3> vecVertices;
	std::vector<glm::vec3> vecNormal;
	std::vector<glm::vec2> vecUV;
	std::vector<glm::vec4> vecColor;
	glm::mat4 m4(1.0f);

	SMMBBlockVertex2 smmbv;
	if(mmbIndex<0)
		return false;

	CMMB *pmmb = nullptr;
	if(mmbIndex < 1000 && mmbIndex<m_vecMMB.size())
		pmmb = m_vecMMB[mmbIndex];
	else if(mmbIndex >=1000 && (mmbIndex-1000) < m_vecDependMMB.size())
		pmmb = m_vecDependMMB[mmbIndex-1000];
	else
		return false;

	char buf[20];
	memcpy(buf, pmmb->m_SMMBHeader.imgID,16);
	buf[16]=0x00;
	std::cout << "id: " << buf << "  piece:" << pmmb->m_SMMBHeader.pieces << std::endl;

	//get the transform matrix
	if(m_isTransform && in!=nullptr) {
		getMZB100Matrix(in, m4);
	}

	//create a MeshBuffer group to hold the MeshBuffer, since the boundingRect is the same
	//it will minimize frustum culling time
	CMeshBufferGroup *pMBG = new CMeshBufferGroup(mzbIndex, mmbIndex);

	//update BoundingRect
	pMBG->m_origMinBoundRect=pMBG->m_minBoundRect=glm::vec3(pmmb->m_SMMBHeader.x1, pmmb->m_SMMBHeader.y1, pmmb->m_SMMBHeader.z1);
	pMBG->m_origMaxBoundRect=pMBG->m_maxBoundRect=glm::vec3(pmmb->m_SMMBHeader.x2, pmmb->m_SMMBHeader.y2, pmmb->m_SMMBHeader.z2);
	if(m_isTransform && in!=nullptr) {
		pMBG->m_minBoundRect.x *= in->fScaleX;
		pMBG->m_minBoundRect.y *= in->fScaleY;
		pMBG->m_minBoundRect.z *= in->fScaleZ;
		pMBG->m_maxBoundRect.x *= in->fScaleX;
		pMBG->m_maxBoundRect.y *= in->fScaleY;
		pMBG->m_maxBoundRect.z *= in->fScaleZ;

		//generate intermediate point min,001,010,011,100,101,110,max
		br1.x=pMBG->m_minBoundRect.x;
		br1.y=pMBG->m_minBoundRect.y;
		br1.z=pMBG->m_maxBoundRect.z;
		br2.x=pMBG->m_minBoundRect.x;
		br2.y=pMBG->m_maxBoundRect.y;
		br2.z=pMBG->m_minBoundRect.z;
		br3.x=pMBG->m_minBoundRect.x;
		br3.y=pMBG->m_maxBoundRect.y;
		br3.z=pMBG->m_maxBoundRect.z;
		br4.x=pMBG->m_maxBoundRect.x;
		br4.y=pMBG->m_minBoundRect.y;
		br4.z=pMBG->m_minBoundRect.z;
		br5.x=pMBG->m_maxBoundRect.x;
		br5.y=pMBG->m_minBoundRect.y;
		br5.z=pMBG->m_maxBoundRect.z;
		br6.x=pMBG->m_maxBoundRect.x;
		br6.y=pMBG->m_maxBoundRect.y;
		br6.z=pMBG->m_minBoundRect.z;

		//transform the boundingRect to worldCoord by MZB 
		pMBG->m_minBoundRect = glm::vec3(m4 * glm::vec4(pMBG->m_minBoundRect,1));
		pMBG->m_maxBoundRect = glm::vec3(m4 * glm::vec4(pMBG->m_maxBoundRect,1));

		br1 = glm::vec3(m4 * glm::vec4(br1,1));
		br2 = glm::vec3(m4 * glm::vec4(br2,1));
		br3 = glm::vec3(m4 * glm::vec4(br3,1));
		br4 = glm::vec3(m4 * glm::vec4(br4,1));
		br5 = glm::vec3(m4 * glm::vec4(br5,1));
		br6 = glm::vec3(m4 * glm::vec4(br6,1));

		//re-evaluate the min/max
		tmp=min=pMBG->m_minBoundRect;
		max=pMBG->m_maxBoundRect;
		if(br1.x<min.x)
			min.x=br1.x;
		if(br2.x<min.x)
			min.x=br2.x;
		if(br3.x<min.x)
			min.x=br3.x;
		if(br4.x<min.x)
			min.x=br4.x;
		if(br5.x<min.x)
			min.x=br5.x;
		if(br6.x<min.x)
			min.x=br6.x;
		if(max.x<min.x)
			min.x=max.x;

		if(br1.y<min.y)
			min.y=br1.y;
		if(br2.y<min.y)
			min.y=br2.y;
		if(br3.y<min.y)
			min.y=br3.y;
		if(br4.y<min.y)
			min.y=br4.y;
		if(br5.y<min.y)
			min.y=br5.y;
		if(br6.y<min.y)
			min.y=br6.y;
		if(max.y<min.y)
			min.y=max.y;
	
		if(br1.z<min.z)
			min.z=br1.z;
		if(br2.z<min.z)
			min.z=br2.z;
		if(br3.z<min.z)
			min.z=br3.z;
		if(br4.z<min.z)
			min.z=br4.z;
		if(br5.z<min.z)
			min.z=br5.z;
		if(br6.z<min.z)
			min.z=br6.z;
		if(max.z<min.z)
			min.z=max.z;

		//max
		if(tmp.x>max.x)
			max.x=tmp.x;
		if(br1.x>max.x)
			max.x=br1.x;
		if(br2.x>max.x)
			max.x=br2.x;
		if(br3.x>max.x)
			max.x=br3.x;
		if(br4.x>max.x)
			max.x=br4.x;
		if(br5.x>max.x)
			max.x=br5.x;
		if(br6.x>max.x)
			max.x=br6.x;

		if(tmp.y>max.y)
			max.y=tmp.y;
		if(br1.y>max.y)
			max.y=br1.y;
		if(br2.y>max.y)
			max.y=br2.y;
		if(br3.y>max.y)
			max.y=br3.y;
		if(br4.y>max.y)
			max.y=br4.y;
		if(br5.y>max.y)
			max.y=br5.y;
		if(br6.y>max.y)
			max.y=br6.y;

		if(tmp.z>max.z)
			max.z=tmp.z;
		if(br1.z>max.z)
			max.z=br1.z;
		if(br2.z>max.z)
			max.z=br2.z;
		if(br3.z>max.z)
			max.z=br3.z;
		if(br4.z>max.z)
			max.z=br4.z;
		if(br5.z>max.z)
			max.z=br5.z;
		if(br6.z>max.z)
			max.z=br6.z;
			
		pMBG->m_minBoundRect=min;
		pMBG->m_maxBoundRect=max;
	}

	//for each model, get its Vertex/Normal info
	unsigned int useAlpha=0, multipler=1;
	auto modelit=pmmb->m_SMMBVertexIndices.begin();
	for(; modelit!=pmmb->m_SMMBVertexIndices.end(); ++modelit) {
		//create a mb for each model
		IMeshBuffer *mb = new IMeshBuffer(modelit->drawType);
		//set a checker texture as default, if none is available
		mb->updateTextureID(m_DefaultTextureID);
		mb->generateFrameBuffer(1);
		useAlpha = 0;

		//find the textureID
		for(auto tit=m_vectextureInfo.begin(); tit!=m_vectextureInfo.end(); ++tit) {
			if( memcmp((*modelit).textureName, (*tit).name.c_str(), 16)==0) {
				mb->updateTextureID((*tit).id);
				break;
			}
		}
		if (pmmb->m_SMMBHeader.imgID[0] == '_' || modelit->blending & 0x8000)
			useAlpha = 1;

		multipler = (modelit->blending & 0xF000) >> 12;
		if (multipler == 0)
			multipler = 1;

		mb->setBlendFlag(multipler, useAlpha);
		vecVertices.clear();
		vecNormal.clear();
		vecUV.clear();
		vecColor.clear();

		////use the indice to lookup MMB vertex and transform MMB vertices - use DrawArrays()
		//for(auto iit=(*modelit).vecIndices.begin(); iit!=(*modelit).vecIndices.end(); ++iit) {
		//	smmbv = (*modelit).vecVertex[(*iit)];
		//	v.x = smmbv.x;
		//	v.y = smmbv.y;
		//	v.z = smmbv.z;
		//	n.x = smmbv.hx;
		//	n.y = smmbv.hy;
		//	n.z = smmbv.hz;
		//	color.x = (smmbv.color & 0xFF) /255.0f;
		//	color.y = ((smmbv.color & 0xFF00)>>8) /255.0f;
		//	color.z = ((smmbv.color & 0xFF0000)>>16) /255.0f;
		//	color.w = ((smmbv.color & 0xFF000000)>>24) /255.0f;
		//	uv.x = smmbv.u;
		//	uv.y = smmbv.v;
		//	
		//	if(m_isTransform && in!=nullptr) {
		//		v.x = v.x * in->fScaleX;
		//		v.y = v.y * in->fScaleY;
		//		v.z = v.z * in->fScaleZ;
		//		v = glm::vec3(m4 * glm::vec4(v,1));
		//		n = glm::vec3(m4 * glm::vec4(n,0));
		//	}
		//	vecVertices.push_back(v);
		//	vecNormal.push_back(n);
		//	vecUV.push_back(uv);
		//	vecColor.push_back(color);
		//}

		//use index buffer VBO, since MMB does not use multiple uv - use DrawElements()
		for (auto vit = (*modelit).vecVertex.begin(); vit != (*modelit).vecVertex.end(); ++vit) {
			smmbv = *vit;
			v.x = smmbv.x;
			v.y = smmbv.y;
			v.z = smmbv.z;
			n.x = smmbv.hx;
			n.y = smmbv.hy;
			n.z = smmbv.hz;
			//BGRA
			color.z = (smmbv.color & 0xFF) /255.0f;
			color.y = ((smmbv.color & 0xFF00)>>8) /255.0f;
			color.x = ((smmbv.color & 0xFF0000)>>16) /255.0f;
			color.w = ((smmbv.color & 0xFF000000)>>24) /255.0f;
			uv.x = smmbv.u;
			uv.y = smmbv.v;
				
			if(m_isTransform && in!=nullptr) {
				v.x = v.x * in->fScaleX;
				v.y = v.y * in->fScaleY;
				v.z = v.z * in->fScaleZ;
				v = glm::vec3(m4 * glm::vec4(v,1));
				n = glm::vec3(m4 * glm::vec4(n,0));
//				n = glm::vec3( glm::vec4(n,0) * glm::transpose(glm::inverse(m4)));
				n = glm::normalize(n);
			}
			vecVertices.push_back(v);
			vecNormal.push_back(n);
			vecUV.push_back(uv);
			vecColor.push_back(color);
		}

		mb->updateVertexBuffer(0,vecVertices);
		mb->updateNormalBuffer(0,vecNormal);
		mb->updateUVBuffer(vecUV);
		mb->updateColorBuffer(vecColor);
		mb->updateIndicesBuffer((*modelit).vecIndices);
		pMBG->addMeshBuffer(mb);
	}
	//add the group
	addMeshBufferGroup(pMBG);
	return true;
}

void CFFXILandscapeMesh::decodeMesh(char *p, unsigned int offsetB112, unsigned int offsetB92, IMeshBuffer *mb)
{
	unsigned int r1, r2, r3, sizeB, sizeV, sizeN,totalsize=offsetB92;
	glm::vec3 v,n;
	glm::u16 indice,normal;
	glm::vec2 uv1(0,0),uv2(0,1),uv3(1,0), uv4(1,1);
	std::vector<glm::vec3> vecVertex;
	std::vector<glm::vec3> vecNormal;
	vecVertex.clear();
	vecNormal.clear();

	SMZBBlock112 *pB112 = (SMZBBlock112*)(p+offsetB112);

	r1 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	r2 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	r3 = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	sizeB = (*(unsigned int*)(p+totalsize)); totalsize +=4;
	sizeB &= 0xffff;

	sizeV = (r2-r1)/12;
	for(unsigned int j=0; j<sizeV; ++j) {
		v.x = (*(float*)(p+totalsize)); totalsize +=4;
		v.y = (*(float*)(p+totalsize)); totalsize +=4;
		v.z = (*(float*)(p+totalsize)); totalsize +=4;
		multiplyByMatrix(pB112, v.x, v.y, v.z);
		vecVertex.push_back(v);
	}

	sizeN = (r3-r2)/12;
	for(unsigned int j=0; j<sizeN; ++j) {
		n.x = (*(float*)(p+totalsize)); totalsize +=4;
		n.y = (*(float*)(p+totalsize)); totalsize +=4;
		n.z = (*(float*)(p+totalsize)); totalsize +=4;
		multiplyByMatrixNormal(pB112, n.x, n.y, n.z);
		vecNormal.push_back(n);
	}

	SMZBBlock8 *pB8=(SMZBBlock8*)(p+totalsize);
	for(unsigned int j=0; j<sizeB; ++j) {
		//use same normal for 3 indices
		normal = limit(sizeN, pB8->s4);

		indice = limit(sizeV, pB8->s1);
		mb->addIndicesBuffer(indice + m_lastIndices);
		mb->addVertexBuffer(0, vecVertex[indice]);
		mb->addNormalBuffer(0, vecNormal[normal]);
		if(j%2)
			mb->addUVBuffer(uv1);
		else
			mb->addUVBuffer(uv2);
		indice = limit(sizeV, pB8->s2);
		mb->addIndicesBuffer(indice + m_lastIndices);
		mb->addVertexBuffer(0, vecVertex[indice]);
		mb->addNormalBuffer(0, vecNormal[normal]);
		if(j%2)
			mb->addUVBuffer(uv2);
		else
			mb->addUVBuffer(uv4);

		indice = limit(sizeV, pB8->s3);
		mb->addIndicesBuffer(indice + m_lastIndices);
		mb->addVertexBuffer(0, vecVertex[indice]);
		mb->addNormalBuffer(0, vecNormal[normal]);
			mb->addUVBuffer(uv3);

		pB8++;
	}
	m_lastIndices+=sizeV;
}

void CFFXILandscapeMesh::multiplyByMatrix( SMZBBlock112 *mat, float &vx, float &vy, float &vz)
{
	float x = mat->f1 * vx + mat->f5 * vy + mat->f9 * vz + mat->f13;
	float y = mat->f2 * vx + mat->f6 * vy + mat->f10 * vz + mat->f14;
	float z = mat->f3 * vx + mat->f7 * vy + mat->f11 * vz + mat->f15;
	vx = x;
	vy = y;
	vz = z;
}

void CFFXILandscapeMesh::multiplyByMatrixNormal( SMZBBlock112 *mat, float &vx, float &vy, float &vz)
{
	float x = mat->f17 * vx + mat->f20 * vy + mat->f23 * vz;
	float y = mat->f18 * vx + mat->f21 * vy + mat->f24 * vz;
	float z = mat->f19 * vx + mat->f22 * vy + mat->f25 * vz;
	vx = x;
	vy = y;
	vz = z;
}

unsigned short CFFXILandscapeMesh::limit( unsigned int max, unsigned short val)
{
	if( val<max ) return val;

	unsigned int mask = 0x7fff;
	do {
		val &= mask;
		mask >>= 1;
	} while(val>max);
	return val;
}


void CFFXILandscapeMesh::writeMeshInfo(int frame, int mbIndex)
{
//	m_meshBuffers[mbIndex]->outputMeshInfo(frame, mbIndex);
	//write for blender
	//1) get CMMB
	//2) get its indices
	//3) get vertex
	char buf[100];
	std::string str;
	
	CMMB *pmmb = m_vecMMB[mbIndex];
	for(int i=0; i<pmmb->m_SMMBVertexIndices.size(); ++i) {
		sprintf_s(buf,100,"meshBuffer_%d_%d_%d.txt", frame, mbIndex,i);
		std::ofstream ofs( buf, std::ios::out);
		ofs << "numIndices: " << pmmb->m_SMMBVertexIndices[i].numIndices << "  actual: " << pmmb->m_SMMBVertexIndices[i].vecIndices.size()/3 << std::endl;
		for(auto it=pmmb->m_SMMBVertexIndices[i].vecIndices.begin(); it!=pmmb->m_SMMBVertexIndices[i].vecIndices.end(); ++it) {
			ofs << *it << ",";
			++it;
			if (it == pmmb->m_SMMBVertexIndices[i].vecIndices.end()) {
				ofs << std::endl;
				break;
			}
			ofs << *it << ",";
			++it;
			if (it == pmmb->m_SMMBVertexIndices[i].vecIndices.end()) {
				ofs << std::endl;
				break;
			}
			ofs << *it << std::endl;
		}
		ofs << "numVertex: " << pmmb->m_SMMBVertexIndices[i].vecVertex.size() << std::endl;
		for(auto it=pmmb->m_SMMBVertexIndices[i].vecVertex.begin(); it!=pmmb->m_SMMBVertexIndices[i].vecVertex.end(); ++it) {
			str = glm::to_string(glm::vec3((*it).x, (*it).y, (*it).z));
			str = str.substr(5, str.length()-6);
			ofs << str << std::endl;
		}
		ofs.close();
	}
}

//check a point vec3 within view frustum
//  -w < x < w
//  -w < y < w
//   0 < z < w
bool CFFXILandscapeMesh::in_frustum(mat4 M, vec3 p) {
        vec4 Pclip = M * vec4(p, 1.);
        return abs(Pclip.x) < Pclip.w && 
               abs(Pclip.y) < Pclip.w && 
               0 < Pclip.z && 
               Pclip.z < Pclip.w;
}

