#pragma once
#include "IDatBase.h"

//lion
class CDat34 : public IDatBase
{
public:
	CDat34(void);
	~CDat34(void);
	//base method
	int getOriginalTotalDDS();
	int getTotalDDS();
	char* getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap);

	void parseFrame();
	void parseFrameMesh();
	void parseBaseMesh();
	void parseMesh();

	void extractDDS();
	void corelateNormal();

private:

};

