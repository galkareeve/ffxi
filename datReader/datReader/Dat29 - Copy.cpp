#include "StdAfx.h"
#include "Dat29.h"
#include <iostream>
#include <fstream>
#include "myStruct.h"

using namespace std;

char ffxidir[512];
void LocateFFXIDataFolder()
{
	HKEY hKey;
	memset(ffxidir,0,512);

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\PlayOnlineUS\\InstallFolder",0,KEY_READ,&hKey))
	{
		DWORD dwData = sizeof(ffxidir);
		DWORD dwType = REG_SZ;
		RegQueryValueEx(hKey,"0001",NULL,&dwType,(LPBYTE)ffxidir,&dwData);
		RegCloseKey( hKey );
	}
	if (lstrlen(ffxidir) == 0)
	{
		MessageBox(NULL,"1.没有找到FF11安装的注册表项,请在FINAL FANTASY XI文件夹下执行此程序.","提示",MB_OK);
		GetCurrentDirectory(sizeof(ffxidir),ffxidir);
	}
	if (lstrlen(ffxidir) > 0)
	{
		if (ffxidir[lstrlen(ffxidir)-1] != '\\')
		{
			lstrcat(ffxidir,"\\");
		}
	}
}

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
	LocateFFXIDataFolder();
	int fno = atoi(m_FN.c_str());

	int dir=0,rem=fno;
	if(fno>=1000000) {
		dir=fno/1000000;
		rem -= dir*1000000;
	}

	if( !f.LoadF(dir, rem)) {
		cout << "unable to load file" << endl;
		return false;
	}

	DATHEAD hd;

	char *p, *start=f.FistData(&hd);
	for( p=f.FistData(&hd); p; p=f.NextData(&hd) )
	{
		int type = (int)hd.type;
		switch (type)
		{
			case 0x20:  //IMG
				//need to check for 3txd
//				if( isDDSImage(p+sizeof(DATHEAD)) )
					totalDDSinDat++;
			break;
		}
	}

	cout << "Done parsing dat" << endl;
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

	unsigned int len, offset, size;
	DATHEAD hd;
	int index=0;
	unsigned char *pPalett;
	IMGINFOB1 *ib = nullptr;
	IMGINFO *i1 = nullptr;
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
					break;

				case 0x81:
					//contain a normal 0x01 image + DDS image....HOW to include 2 image???
					isDDS=true;
					i1 = (IMGINFO*)(p+sizeof(DATHEAD));
					len = width * height;
					pStorage = new char[len];
					offset = sizeof(DATHEAD) + sizeof(IMGINFO) + 1024 + len + 12;
					memcpy(pStorage, (p+offset), len);
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
					break;
					}

				case 0x05:
					//same as 0x01 but with 5 image
					//DATHEAD + 57 + (width*height+1024)*5
					isDDS=false;
					i1 = (IMGINFO*)(p+sizeof(DATHEAD));
					len = width * height;
					pStorage = new char[len*4];
					//extract color from palette
					pPalett = (unsigned char*)(p+sizeof(IMGINFO));
					for(int j=0; j<len; ++j) {
						memcpy(&pStorage[j*4], &i1->palet[pPalett[j]], 4);
					}
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

