#pragma once
#include "IDatBase.h"
class CDat49 : public IDatBase
{
public:
	CDat49(void);
	~CDat49(void);
	int getOriginalTotalDDS();
	int getTotalDDS();
	char* getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap);
	void parseFrame();
	void parseMesh();

	void parseWalkMesh();
};

