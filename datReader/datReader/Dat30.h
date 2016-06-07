#pragma once
#include "IDatBase.h"

//elevator
class CDat30 : public IDatBase
{
public:
	CDat30(void);
	~CDat30(void);
	int getOriginalTotalDDS();
	int getTotalDDS();
	char* getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap);
	void parseFrame();
	void parseMesh();
};

