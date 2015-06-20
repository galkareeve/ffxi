#pragma once
#include "glGlobal.h"
#include "myStruct.h"

class IDatBase
{
public:

	IDatBase(void)
	{
		m_writeBMP =false;
	}

	virtual ~IDatBase(void)
	{
	}
	virtual int getOriginalTotalDDS()=0;
	virtual int getTotalDDS()=0;
	virtual bool getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap, char *name, bool &isDDS, char *& pStorage)=0;
	virtual void parseMesh()=0;
	virtual void parseFrame()=0;

	virtual bool loadDat(){return false;};
	virtual	bool nextDatFile()=0;
	virtual bool prevDatFile()=0;
	virtual void skipNext()=0;
	virtual int getCurDatFile()=0;

	void setWriteBMP() {m_writeBMP=true;};
	bool isWriteBMP() { return m_writeBMP; };

	char* createData(std::ifstream &in, unsigned int size) {
		char *pM = new char[size];
		memset(pM, 0, size);	
		in.read(pM, size);
		if( in.fail() ) {
			std::cout << "error walk read: " << in.gcount() << std::endl;
			delete[] pM;
			return nullptr;
		}
		return pM;	
	};

	void addDDSTextureID(GLuint texID, std::string name) {
		m_vecDDS.push_back(texID);
		m_vecID.push_back(name);
		++m_totalDDS;
	};

	std::string nextDDS() {
		if( ++m_curDDS >= m_totalDDS )
			m_curDDS=0;
		g_resources.textures[0] = m_vecDDS[m_curDDS];
		return m_vecID[m_curDDS];
	};

	std::string prevDDS() {
		if( --m_curDDS <0 )
			m_curDDS=m_totalDDS-1;
		g_resources.textures[0] = m_vecDDS[m_curDDS];
		return m_vecID[m_curDDS];
	};

	std::string firstImage() {
		g_resources.textures[0] = m_vecDDS[0];
		return m_vecID[0];
	};

	void clear() {
		m_totalDDS=0;
		m_curDDS=0;
		for(auto it=m_vecDDS.begin(); it!=m_vecDDS.end(); ++it) {
			glDeleteTextures(1, &(*it));
		}
		m_vecDDS.clear();
		m_vecID.clear();
	};


protected:
	bool m_writeBMP;
	int m_totalDDS;
	int m_curDDS;
	std::vector<GLuint> m_vecDDS;
	std::vector<std::string> m_vecID;
};

extern char ffxidir[512];
class FFXIFile
{
	LPSTR pData;
	char filename[256];
	BOOL Load(void)
	{
		Free();
		HANDLE hFile = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_ARCHIVE,NULL);
		if( hFile!=INVALID_HANDLE_VALUE ){
			DWORD dmy;
			dwSize = GetFileSize(hFile,NULL);
			pdat = new char[dwSize];
			ReadFile(hFile,pdat,dwSize,&dmy,NULL);
			CloseHandle(hFile);
			return TRUE;
		}
		return FALSE;
	};

public:
	char *pdat;
	DWORD dwSize;
	int vnum;
	int fnum;

	FFXIFile() 
	{ 
		pdat = NULL; 
		pData = NULL; 
		dwSize = 0; 
	}
   ~FFXIFile() 
   { 
	   Free(); 
   }

	DWORD GetSize(){return dwSize;}
	BOOL IsNull(){
		return (!pdat || dwSize==0);
	}
	BOOL IsFFXIFormat()
	{   
		if( IsNull() ) return FALSE;
		DATHEAD *phd = (DATHEAD *)pdat;
		if( phd->next<=0 || dwSize<(DWORD)phd->next*16 ) return FALSE;
		//if( phd->type!=0x01) return FALSE;
		return TRUE;
	}
	LPSTR FistData(DATHEAD *phd)
	{
		if( !pdat ) return NULL;
		*phd = *(DATHEAD *)pdat;
		pData = pdat;
		return pData;
	}
	LPSTR NextData(DATHEAD *phd)
	{
		if(!pData) return NULL;
		*phd = *(DATHEAD *)pData;
		int next = phd->next;
		if( next<=0 ) return NULL;
		if( pdat+dwSize<=pData+next*16 ) return NULL;
		pData += next*16;
		*phd = *(DATHEAD *)pData;
		return pData;
	}
	BOOL Free(void)
	{
		dwSize = 0;
		if( pdat )
		{
			delete []pdat;
			pdat = NULL;
		}
		pData=NULL;
		return TRUE;
	}
	LPSTR GetShortFilename()
	{
		return &filename[lstrlen(ffxidir)];
	}

	BOOL LoadF(int flg, int no) 
	{
		Free();
		if(flg==0) wsprintf(filename,"%sROM\\%d\\%d.dat",ffxidir,no/1000,no%1000);  
		else       wsprintf(filename,"%sROM%d\\%d\\%d.dat",ffxidir,flg,no/1000,no%1000);

		return Load();
	}
		

};
