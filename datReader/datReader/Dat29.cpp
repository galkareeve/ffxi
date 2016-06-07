#include "StdAfx.h"
#include "Dat29.h"

#include <iostream>
#include <fstream>
#include "myStruct.h"
#include <GL/glew.h>
#include "DDSLoader.h"
#include "DDS2Bmp.h"

using namespace std;

extern bool isLoaded;
CDDSLoader loader;
char ffxidir[512]="e:\\Program Files (x86)\\PlayOnline2\\SquareEnix\\FINAL FANTASY XI\\";

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

CDat29::CDat29(string FN)
{
	m_totalDDS=0;
	totalDDSinDat=0;
	m_curDDS=0;
	m_FN = FN;
}


CDat29::~CDat29(void)
{
	f.Free();
}

bool CDat29::loadDat()
{
//	LocateFFXIDataFolder();
	int fno = atoi(m_FN.c_str());

	m_dir=0,m_rem=fno;
	if(fno>=1000000) {
		m_dir=fno/1000000;
		m_rem -= m_dir*1000000;
	}

	if( !f.LoadF(m_dir, m_rem)) {
		MessageBox(NULL, "unable to load file", "Err", MB_OK);
		return false;
	}

	return extractDat();
}

bool CDat29::extractDat()
{
	CDDS2Bmp ddsbmp;

	DATHEAD hd;
	char name[255],buf[20];
	unsigned int len, offset, width, height,size;
	
	int index=0,MZBc=0,MMBc=0,IMGc=0,BONEc=0,ANIMc=0,VERTc=0,tmp;
	char *pStorage, *bmpImg, *bytePtr;
	unsigned char *pPalett;
	IMGINFOA1 *ii = nullptr;
	IMGINFOB1 *ib = nullptr;
	IMGINFO *i1 = nullptr;
	IMGINFO05 *i5 = nullptr;
	IMGINFO81_DDS *i81DDS = nullptr;
	DWORD *pPal=nullptr;

//	std::ofstream ofs( "palette.txt");

	char *p, *start=f.FistData(&hd);
	for( p=f.FistData(&hd); p; p=f.NextData(&hd) )
	{
		int type = (int)hd.type;
//		len = hd.next*16;
		if(type==0x1c)
			MZBc++;
		else if(type==0x2e)
			MMBc++;
		else if(type==0x29)
			BONEc++;
		else if(type==0x2b)
			ANIMc++;
		else if(type==0x2a)
			VERTc++;
		else if(type==0x20)
		{
			IMGc++;
			ii = (IMGINFOA1*)(p+sizeof(DATHEAD));
			memcpy(name, ii->id, 16);
			name[16]=0x00;
			if (strcmp(name, "model   d_pai_01") == 0)
				ddsbmp.setOutputAlpha(true);
			else
				ddsbmp.setOutputAlpha(false);
			width = ii->imgx;
			height = ii->imgy;
			switch( ii->flg )
			{
			case 0xA1:
				pStorage = new char[ii->size];
				memset(pStorage, 0, ii->size);
				memcpy(pStorage, (p+sizeof(DATHEAD)+sizeof(IMGINFOA1)), ii->size);
				
				if(m_writeBMP)
					ddsbmp.convert2BMP(pStorage, width, height, ii->size, ii->widthbyte, ii->ddsType[0], name, bmpImg);
				else
					ddsbmp.convert2BMP(pStorage, width, height, ii->size, ii->widthbyte, ii->ddsType[0], NULL, bmpImg);
				makeImage(bmpImg, name, width, height);
				makeDDS( pStorage, name, width, height );
				delete[] pStorage;
				delete[] bmpImg;
			break;
			
			case 0x01:
				//1088 + width*height....==> single byte index to palette
				//DATHEAD + 57 + (width*height+1024)
				i1 = (IMGINFO*)(p+sizeof(DATHEAD));
				len = width * height;
				pStorage = new char[len*4];
				//extract color from palette
				//pPalett = (unsigned char*)(p+sizeof(DATHEAD)+sizeof(IMGINFO));
				//for(int j=0; j<len; ++j) {
				//	memcpy(&pStorage[j*4], &i1->palet[pPalett[j]], 4);
				//}
				bytePtr = p+sizeof(DATHEAD) + sizeof(IMGINFO);
	/*			for(int j=0; j<len; ++j) {
					memcpy(&pStorage[j*4], &i1->palet[(unsigned char)(*bytePtr)],4);
					bytePtr++;
				}*/
				for(int i=height-1; i>=0; --i) {
					for(int j=0; j<width; ++j) {
						memcpy(&pStorage[(i*width+j)*4], &i1->palet[(unsigned char)(*bytePtr)],4);
						bytePtr++;
					}
				}
				makeImage(pStorage, name, width, height);
				delete[] pStorage;
			break;

			case 0x81:
				//contain a normal 0x01 image + DDS image
				i1 = (IMGINFO*)(p+sizeof(DATHEAD));
				len = width * height;
				pStorage = new char[len*4];	//4byte (ARGB)
				bytePtr = p+sizeof(DATHEAD) + sizeof(IMGINFO);
				//need to reverse the image
				//for(int j=0; j<len; ++j) {
				//	memcpy(&pStorage[j*4], &i1->palet[(unsigned char)(*bytePtr)], 4);
				//	bytePtr++;
				//}
				for(int i=height-1; i>=0; --i) {
					for(int j=0; j<width; ++j) {
						memcpy(&pStorage[(i*width+j)*4], &i1->palet[(unsigned char)(*bytePtr)],4);
						bytePtr++;
					}
				}
				makeImage(pStorage, name, width, height);
				delete[] pStorage;

				//DDS, does not always contain a DDS image
				i81DDS = (IMGINFO81_DDS*)(p+sizeof(DATHEAD) + sizeof(IMGINFO) + len);
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
						else if(i81DDS->size==64) {
							width=8; height=8;
						}
						else if(i81DDS->size==16) {
							width=4; height=4;
						}
						else
							continue;
					}
					pStorage = new char[i81DDS->size];
					memset(pStorage, 0, i81DDS->size);
					//len is palette index (1byte)
					memcpy(pStorage, (p+sizeof(DATHEAD)+sizeof(IMGINFO)+len+sizeof(IMGINFO81_DDS)), i81DDS->size);
					name[0]='2';
					makeDDS( pStorage, name, width, height );
//					ddsbmp.convert2BMP(pStorage, width, height, NULL, bmpImg);
//					makeImage(bmpImg, name, width, height);
					delete[] pStorage;
//					delete[] bmpImg;
				}
			break;

			case 0xB1:		//0xB1
				ib = (IMGINFOB1*)(p+sizeof(DATHEAD));
				len = width * height;
				pStorage = new char[len*4];
				//extract color from palette 0xB1
				pPalett = (unsigned char*)(p+sizeof(IMGINFOB1));
				for(int j=0; j<len; ++j) {
					memcpy(&pStorage[j*4], &ib->palet[pPalett[j]], 4);
				}
				//if( ib->unk>0 ) {
				//	len = ib->unk*4;
				//	pPalett = new unsigned char[len];
				//	size = (ib->imgx-ib->unk)*4;
				//	for(int r=0; r<ib->imgy; ++r) {
				//		offset = r * ib->imgx * 4;
				//		//record the first N RGBA	
				//		memcpy(pPalett, &pStorage[offset], len);
				//		//shift RGBA by N
				//		memcpy(&pStorage[offset], &pStorage[offset+len], size);
				//		//add first N RGBA to end
				//		memcpy(&pStorage[offset+size], pPalett, len);
				//	}
				//	delete []pPalett;
				//}
				makeImage(pStorage, name, width, height);
				delete[] pStorage;
				break;

			case 0x05:
				//same as 0x01 but with 5 image
				//DATHEAD + 57 + (width*height+1024)*5
				i5 = (IMGINFO05*)(p+sizeof(DATHEAD));
				len = width * height;
				pStorage = new char[len*4];
				//extract color from palette
				offset = sizeof(DATHEAD)+sizeof(IMGINFO05);
				for(int k=0; k<5; ++k) {		
					pPal = (DWORD*)(p+offset);
					pPalett = (unsigned char*)(p+offset+1024);
					for(index=0; index<len; ++index) {
						memcpy(&pStorage[index*4], &pPal[pPalett[index]], 4);
					}
					if( k==0 ){
						//do nothing
						width=width; height=height;
					}
					else if( k==3 ) {
						width=16; height=16;
					}
					else {
						width=32; height=16;
					}
					makeImage(pStorage, name, width, height);
					if( k==0 )
						offset = offset + len + 1024;		//only first image is width*height
					else if( k==3 )
						offset = offset + 256 + 1024;		//fourth image is always 256
					else
						offset = offset + 512 + 1024;		//2, 3, 5 is always 512
				}
				delete[] pStorage;
				break;

			case 0x91:
				//same as 0x01
				i1 = (IMGINFO*)(p+sizeof(DATHEAD));
				len = width * height;
				pStorage = new char[len*4];
				//extract color from palette
				pPalett = (unsigned char*)(p+sizeof(IMGINFO));
				for(int j=0; j<len; ++j) {
					memcpy(&pStorage[j*4], &i1->palet[pPalett[j]], 4);
				}
				makeImage(pStorage, name, width, height);
				delete[] pStorage;
				break;

			default:
				MessageBox(NULL, "Unknown IMG type", "Err", MB_OK);
			}
		}
	}
	if(IMGc>0)
		isLoaded=true;
	sprintf_s(name,255,"Dat: %s, MZB:%d, MMB:%d, IMG:%d, Bone:%d, Anim:%d, Vert:%d", f.GetShortFilename(),MZBc,MMBc,IMGc,BONEc,ANIMc,VERTc);
	cout << name << endl;
	return true;
}

bool CDat29::isDDSImage(char *p)
{
	IMGINFO *ii = (IMGINFO*)p;
	if( ii->flg==0xA1 )
		return true;
	return false;
}

int CDat29::getOriginalTotalDDS()
{
	//this is no of IMG in dat
	return totalDDSinDat;
}

int CDat29::getTotalDDS()
{
	//this is no of textureID created, max 40
	return m_totalDDS;
}

bool CDat29::getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap, char *name, bool &isDDS, char *& pStorage)
{
	if( i>totalDDSinDat ) return nullptr;

	unsigned int len, offset;
	DATHEAD hd;
	int index=0;
	unsigned char *pPalett;
	IMGINFOB1 *ib = nullptr;
	IMGINFO *i1 = nullptr;
	IMGINFO05 *i5 = nullptr;
	DWORD *pPal=nullptr;

	char *p, *start=f.FistData(&hd);
	for( p=f.FistData(&hd); p; p=f.NextData(&hd) )
	{
		int type = (int)hd.type;
//		len = hd.next*16;
		if(type==0x20)
		{
			if( index==i ) {
				IMGINFOA1 *ii = (IMGINFOA1*)(p+sizeof(DATHEAD));
				memcpy(name, ii->id, 16);
				name[15]=0x00;
				width = ii->imgx;
				height = ii->imgy;
				mipmap=1;
				switch( ii->flg )
				{
				case 0xA1:
					isDDS=true;
					pStorage = new char[ii->size];
					memset(pStorage, 0, ii->size);
					memcpy(pStorage, (p+sizeof(DATHEAD)+sizeof(IMGINFOA1)), ii->size);
					makeDDS( pStorage, name, width, height );
					delete[] pStorage;
				break;
				
				case 0x01:
					//1088 + width*height....==> single byte index to palette
					//DATHEAD + 57 + (width*height+1024)
					isDDS=false;
					i1 = (IMGINFO*)(p+sizeof(DATHEAD));
					len = width * height;
					pStorage = new char[len*4];
					//extract color from palette
					pPalett = (unsigned char*)(p+sizeof(IMGINFO));
					for(int j=0; j<len; ++j) {
						memcpy(&pStorage[j*4], &i1->palet[pPalett[j]], 4);
					}
					makeImage(pStorage, name, width, height);
					delete[] pStorage;
					break;

				case 0x81:
					//contain a normal 0x01 image + DDS image....HOW to include 2 image???
					isDDS=true;
					i1 = (IMGINFO*)(p+sizeof(DATHEAD));
					len = width * height;
					pStorage = new char[len];
					offset = sizeof(DATHEAD) + sizeof(IMGINFO) + 1024 + len + 12;
					memcpy(pStorage, (p+offset), len);
					delete[] pStorage;
					break;

				case 0xB1:		//0xB1
					isDDS=false;
					ib = (IMGINFOB1*)(p+sizeof(DATHEAD));
					len = width * height;
					pStorage = new char[len*4];
					//extract color from palette 0xB1
					pPalett = (unsigned char*)(p+sizeof(IMGINFOB1));
					for(int j=0; j<len; ++j) {
						memcpy(&pStorage[j*4], &ib->palet[pPalett[j]], 4);
					}

					//if( ib->unk>0 ) {
					//	len = ib->unk*4;
					//	pPalett = new unsigned char[len];
					//	size = (ib->imgx-ib->unk)*4;
					//	for(int r=0; r<ib->imgy; ++r) {
					//		offset = r * ib->imgx * 4;
					//		//record the first N RGBA	
					//		memcpy(pPalett, &pStorage[offset], len);
					//		//shift RGBA by N
					//		memcpy(&pStorage[offset], &pStorage[offset+len], size);
					//		//add first N RGBA to end
					//		memcpy(&pStorage[offset+size], pPalett, len);
					//	}
					//	delete []pPalett;
					//}
					makeImage(pStorage, name, width, height);
					delete[] pStorage;
					break;

				case 0x05:
					//same as 0x01 but with 5 image
					//DATHEAD + 57 + (width*height+1024)*5
					isDDS=false;
					i5 = (IMGINFO05*)(p+sizeof(DATHEAD));
					len = width * height;
					pStorage = new char[len*4];
					//extract color from palette
					offset = sizeof(IMGINFO05);
					for(int k=0; k<5; ++k) {
						pPalett = (unsigned char*)(p+offset);
						pPal = (DWORD*)(p+offset+len);
						for(int j=0; j<len; ++j) {
							memcpy(&pStorage[j*4], &pPal[pPalett[j]], 4);
						}
						makeImage(pStorage, name, width, height);
						offset = offset + len + 1024;
					}
					delete[] pStorage;
					break;
				}
				return true;
			}
			else
				index++;
		}
	}
	return false;
}

void CDat29::makeDDS( char *p, char *name, unsigned int width, unsigned int height)
{
	GLuint texture;
    glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	loader.loadDDS(p, width, height, 1);
	addDDSTextureID(texture, name);
}

void CDat29::makeImage( char *p, char *name, unsigned int width, unsigned int height)
{
//	if(memcmp(name, "r_3ba63 hako_4",14)==0)
//		return;

	GLuint texture;
    glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	loader.loadImage(p, width, height);
	addDDSTextureID(texture, name);
}

bool CDat29::nextDatFile()
{
	//delete existing texture
	clear();

	if( !f.LoadF(m_dir, m_rem+1)) {
		MessageBox(NULL, "unable to load file", "Err", MB_OK);
		return false;
	}
	m_rem++;
	return extractDat();
}

bool CDat29::prevDatFile()
{	
	clear();

	if( !f.LoadF(m_dir, m_rem-1)) {
		MessageBox(NULL, "unable to load file", "Err", MB_OK);
		return false;
	}
	m_rem--;
	return extractDat();
}

void CDat29::skipNext()
{
	m_rem++;
}
