#pragma once

#include "IReferenceCounted.h"

class CMeshBufferGroup;
class IMesh : public IReferenceCounted
{
public:
	IMesh(void);
	virtual ~IMesh(void);

	virtual bool animate(int frame)=0;
	virtual void recalculateBoundingBox()=0;
	virtual int getMeshBufferGroupCount()=0;
	virtual CMeshBufferGroup* getMeshBufferGroup(unsigned int i)=0;
	virtual void writeMeshInfo(int frame, int mbIndex)=0;
};

