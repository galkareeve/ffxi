#pragma once

#include "IReferenceCounted.h"

class IMeshBuffer;
class IMesh : public IReferenceCounted
{
public:
	IMesh(void);
	virtual ~IMesh(void);

	virtual bool animate(int frame)=0;
	virtual void recalculateBoundingBox()=0;
	virtual int getMeshBufferCount()=0;
	virtual IMeshBuffer* getMeshBuffer(unsigned int i)=0;
};

