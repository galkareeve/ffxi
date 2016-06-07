#pragma once
#include "idatbase.h"
class CDat7 :
	public IDatBase
{
public:
	CDat7(void);
	~CDat7(void);

	//base method
	int getOriginalTotalDDS();
	int getTotalDDS();
	char* getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap);

	void parseFrame();
	void parseFrameMesh();
	void parseBaseMesh();
	void parseMesh();
};

