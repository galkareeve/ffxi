#pragma once
#include <vector>
#include <map>
#include "myEnum.h"

typedef struct
{
	int addrID;
	BoundingBox bbox;
	int noChild;
	std::vector<unsigned int> vecChild;
	int noMZB;
	std::vector<unsigned int> vecMZBref;
} LT_Node, *pLT_Node;

class CLooseTree
{
public:
	CLooseTree();
	~CLooseTree();

	void create(std::map<unsigned int, pLT_Node> &in);
	pLT_Node getLTNode(unsigned int addrID);
	void getAllMZBref(pLT_Node in, std::vector<unsigned int> &out);
	unsigned int getRootNode() { return m_rootNodeID; };

protected:
	unsigned int m_rootNodeID;
	std::map<unsigned int, pLT_Node> m_mapLT_Node;
};

