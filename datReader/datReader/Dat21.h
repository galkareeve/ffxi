#pragma once
#include "IDatBase.h"

//worm
class CDat21 : public IDatBase
{
public:
	CDat21(void);
	~CDat21(void);
	//base method
	int getOriginalTotalDDS();
	int getTotalDDS();
	char* getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap);

	void parseFrame();
	void parseMesh();
	void corelateNormal();
};

