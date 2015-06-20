#pragma once
#include "IDatBase.h"

class CDat29 : public IDatBase
{
public:
	CDat29(std::string FN);
	~CDat29(void);

	bool loadDat();
	bool extractDat();

	int getOriginalTotalDDS();
	int getTotalDDS();
	bool getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap, char *name, bool &isDDS, char *& pStorage);
	bool isDDSImage(char *p);

	void parseFrame(){};
	void parseFrameMesh(){};
	void parseBaseMesh(){};
	void parseMesh(){};

	void makeDDS( char *p, char *name, unsigned int width, unsigned int height );
	void makeImage( char *p, char *name, unsigned int width, unsigned int height );

	bool nextDatFile();
	bool prevDatFile();
	void skipNext();
	int getCurDatFile() { return m_rem; };

	FFXIFile f;
	int totalDDSinDat;
	int m_dir;		//rom dir
	int m_rem;		//folder(/1000) and file(%1000)
	std::string m_FN;
};

