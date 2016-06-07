#pragma once
#include "IDatBase.h"
class CDat96 : public IDatBase
{
public:
	CDat96(void);
	~CDat96(void);
	int getOriginalTotalDDS();
	int getTotalDDS();
	char* getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap);

	void parseFrame();
	void parseMesh();
	void corelateNormal();
};

