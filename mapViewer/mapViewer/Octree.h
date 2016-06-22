#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "myEnum.h"

#define NUM_CHILDREN	8
#define NUM_NEIGHBORS	6
#define NULL_NODE		-1

///
//	Types
//


typedef struct
{
	BoundingBox	BBox;
	int	ChildIdx[NUM_CHILDREN];
	int	NeighborIdx[NUM_NEIGHBORS];
	int	meshBufferIndexStart;
	int	meshBufferIndexCount;
	
} OCT_NODE, *pOCT_NODE;

class CMeshBufferGroup;
class COctree
{
public:
	COctree(void);
	~COctree(void);

	int Create(std::vector<CMeshBufferGroup*> &in, int minMeshPerNode);
	void GetTables(std::vector<OCT_NODE*> &outOctNode, std::vector<int> &outIndexPtr);
	float getExtend();

protected:

//      int  GetOppositeIdx(int Idx);  	
//      void FindNeighbor(OCT_NODE **OctTable, OCT_NODE *Node, BoxSide *Side, int Idx, int *Found, float *FoundSize);

	void FindBox(std::vector<CMeshBufferGroup*> &in, BoundingBox *BBox);
	int  BuildRootNode(std::vector<CMeshBufferGroup*> &in);
	void BuildTree(OCT_NODE *Node, int minMeshPerNode, std::vector<CMeshBufferGroup*> &in);
	void GetBox(BoundingBox ParentBox, BoundingBox *NewBox, int i);

	bool cubeIntersect(BoundingBox childBox, CMeshBufferGroup *mb);
	bool cubeInside(BoundingBox childBox, CMeshBufferGroup *mb);
	std::vector<OCT_NODE*> m_vecOctNode;
	std::vector<int> m_vecMeshBufferIndexPtr;

	std::ofstream ofs;
};

