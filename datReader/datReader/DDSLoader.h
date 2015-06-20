#pragma once
#include "MyDDS.h"

class CDDSLoader
{
public:
	CDDSLoader(void);
	~CDDSLoader(void);

	bool loadDDS( FILE * f );
	bool loadDDS( void *byte, unsigned int width, unsigned int height, unsigned int mipMap  );
	bool loadImage(void *byte, unsigned int width, unsigned int height);
};

