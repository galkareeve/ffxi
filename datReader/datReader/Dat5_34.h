#pragma once
#include "IDatBase.h"
class CDat5_34 : public IDatBase
{
public:
	CDat5_34(void);
	~CDat5_34(void);
	int getOriginalTotalDDS();
	int getTotalDDS();
	char* getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap);

	void parseFrame();
	void parseMesh();
};

