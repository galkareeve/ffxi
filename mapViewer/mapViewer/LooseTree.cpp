#include "LooseTree.h"

CLooseTree::CLooseTree()
{
}


CLooseTree::~CLooseTree()
{
	//no need to delete node, since it is store in ffxi...Mesh
	m_mapLT_Node.clear();
}

void CLooseTree::create(std::map<unsigned int, pLT_Node> &in)
{
	if (in.empty())
		return;

	m_mapLT_Node = in;
	m_rootNodeID = in.begin()->second->addrID;
}

pLT_Node CLooseTree::getLTNode(unsigned int addrID)
{
	auto mit = m_mapLT_Node.find(addrID);
	if (mit == m_mapLT_Node.end())
		return nullptr;

	return mit->second;
}

void CLooseTree::getAllMZBref(pLT_Node in, std::vector<unsigned int> &out)
{
	//loop thru all its child node

	//check if leaf node
	if (in->noChild == 0) {
		out.insert(out.end(), in->vecMZBref.begin(), in->vecMZBref.end());
		return;
	}

	for (auto it = in->vecChild.begin(); it != in->vecChild.end(); it++) {
		pLT_Node pLT = getLTNode(*it);
		if (pLT != nullptr) {
			getAllMZBref(pLT, out);
		}
	}
}

