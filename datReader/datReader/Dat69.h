#pragma once
#include "IDatBase.h"
//lu shang rod
class CDat69 : public IDatBase
{
public:
	CDat69(void);
	~CDat69(void);
	int getOriginalTotalDDS();
	int getTotalDDS();
	char* getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap);

	void parseMesh();
	void parseFrame();
};

