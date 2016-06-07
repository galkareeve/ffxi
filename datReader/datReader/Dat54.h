#pragma once
#include "IDatBase.h"
class CDat54 : public IDatBase
{
public:
	CDat54(void);
	~CDat54(void);
	int getOriginalTotalDDS();
	int getTotalDDS();
	char* getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap);

	void parseFrame();
	void parseMesh();
	void corelateNormal();
};

