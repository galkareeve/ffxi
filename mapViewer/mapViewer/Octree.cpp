#include "Octree.h"
#include <glm/glm.hpp>
#include "MeshBufferGroup.h"
#include <string>
#include <iostream>

static int writeNode=0;

COctree::COctree(void)
{
}


COctree::~COctree(void)
{
	for(auto it=m_vecOctNode.begin(); it!=m_vecOctNode.end(); ++it)
		delete *it;

	m_vecOctNode.clear();
	m_vecMeshBufferIndexPtr.clear();
}

float COctree::getExtend()
{
	//return the root node
	return abs(m_vecOctNode[0]->BBox.x2);
}

void COctree::FindBox(std::vector<CMeshBufferGroup*> &in, BoundingBox *BBox)
{
	float   Extent=0;
	int	    i, count=in.size();

	for(i=0; i<count; i++)	{
		CMeshBufferGroup *pmb=in[i];
		
		//compare its BoundingRect000 - 111
		if(glm::abs(pmb->m_minBoundRect.x) > Extent)
			Extent = glm::abs(pmb->m_minBoundRect.x);
		if(glm::abs(pmb->m_minBoundRect.y) > Extent)
			Extent = glm::abs(pmb->m_minBoundRect.y);
		if(glm::abs(pmb->m_minBoundRect.z) > Extent)
			Extent = glm::abs(pmb->m_minBoundRect.z);

		if(glm::abs(pmb->m_maxBoundRect.x) > Extent)
			Extent = glm::abs(pmb->m_maxBoundRect.x);
		if(glm::abs(pmb->m_maxBoundRect.y) > Extent)
			Extent = glm::abs(pmb->m_maxBoundRect.y);
		if(glm::abs(pmb->m_maxBoundRect.z) > Extent)
			Extent = glm::abs(pmb->m_maxBoundRect.z);

	}
	//reverse x/y because the viewMatrix is inverted
	//BBox->x2= -Extent;
	//BBox->y2= -Extent;
	//BBox->z2= Extent;
	//
	//BBox->x1= Extent;
	//BBox->y1= Extent;
	//BBox->z1= -Extent;

	BBox->x2= Extent;
	BBox->y2= Extent;
	BBox->z2= Extent;
	
	BBox->x1= -Extent;
	BBox->y1= -Extent;
	BBox->z1= -Extent;
	std::cout << "Extend:  x:" << BBox->x1 <<" ," << BBox->x2 << " y:" << BBox->y1 << " ," <<  BBox->y2 << " z:" << BBox->z1 << " ," << BBox->z2 << std::endl;
}
///
//	BuildRootNode()
//  
//      Build the first node in the tree which encloses all the triangles
//
int COctree::BuildRootNode(std::vector<CMeshBufferGroup*> &in)
{
	pOCT_NODE pRoot = new OCT_NODE;
	int		 i, mbCount=in.size();

	///
	//	Start by constructing a root node that contains
	//	all triangles
	//
	FindBox(in, &pRoot->BBox);

	for(i= 0; i < NUM_CHILDREN; i++)
		pRoot->ChildIdx[i]= NULL_NODE;

	for(i= 0; i < NUM_NEIGHBORS; i++)
		pRoot->NeighborIdx[i]= NULL_NODE;

	pRoot->meshBufferIndexStart= m_vecMeshBufferIndexPtr.size();
	pRoot->meshBufferIndexCount= mbCount;

	//	Fill the start of the meshIndexList with pointers to all
	//	the mesh
	for(i= 0; i < mbCount; i++)	{
		m_vecMeshBufferIndexPtr.push_back(i);
	}
	
	//	Add the root node to the table
	m_vecOctNode.push_back(pRoot);

	return 1;
}

///
//	GetBox()
//
//		Given a parent box, finds a partitioned child box based on the 
//		the index i (i refers to the octant of the new box 0-7)
//
//
void COctree::GetBox(BoundingBox ParentBox, BoundingBox *NewBox, int i)
{
	glm::vec3 BoxSize;
	glm::vec3 Center;
	glm::vec3 NewCenter;

	Center.x= (ParentBox.x2 + ParentBox.x1) / 2;
	Center.y= (ParentBox.y2 + ParentBox.y1) / 2;
	Center.z= (ParentBox.z2 + ParentBox.z1) / 2;

	BoxSize.x= (ParentBox.x2 - ParentBox.x1);
	BoxSize.y= (ParentBox.y2 - ParentBox.y1);
	BoxSize.z= (ParentBox.z2 - ParentBox.z1);

	if(i % 2 == 0)
		NewCenter.x= (Center.x - (BoxSize.x / 4));
	else
		NewCenter.x= (Center.x + (BoxSize.x / 4));

	if(i < 4)
		NewCenter.y= (Center.y - (BoxSize.y / 4));
	else
		NewCenter.y= (Center.y + (BoxSize.y / 4));

	if(i== 2 || i== 3 || i== 6 || i== 7)
		NewCenter.z= (Center.z - (BoxSize.z / 4));
	else
		NewCenter.z= (Center.z + (BoxSize.z / 4));

	NewBox->x1= (NewCenter.x - BoxSize.x / 4);
	NewBox->y1= (NewCenter.y - BoxSize.y / 4);
	NewBox->z1= (NewCenter.z - BoxSize.z / 4);

	NewBox->x2= (NewCenter.x + BoxSize.x / 4);
	NewBox->y2= (NewCenter.y + BoxSize.y / 4);
	NewBox->z2= (NewCenter.z + BoxSize.z / 4);
}

bool COctree::cubeIntersect(BoundingBox childBox, CMeshBufferGroup *mb)
{
	if( childBox.x2 < mb->m_minBoundRect.x )
		return false;

	if( childBox.x1 > mb->m_maxBoundRect.x )
		return false;

	if( childBox.y2 < mb->m_minBoundRect.y )
		return false;

	if( childBox.y1 > mb->m_maxBoundRect.y )
		return false;

	if( childBox.z2 < mb->m_minBoundRect.z )
		return false;

	if( childBox.z1 > mb->m_maxBoundRect.z )
		return false;

	return true;
}
//
//bool COctree::cubeIntersect(BoundingBox childBox, CMeshBufferGroup *mb)
//{
//	//return (childBox.x2 >= mb->m_maxBoundRect.x && childBox.x1 <= mb->m_minBoundRect.x)
// //    && (childBox.y2 >= mb->m_maxBoundRect.y && childBox.y1 <= mb->m_minBoundRect.y)
// //    && (childBox.z2 >= mb->m_maxBoundRect.z && childBox.z1 <= mb->m_minBoundRect.z);
//
//	//check all 8 point, reverse x,y, viewMatrix is inverted
//	if(childBox.x2 <= mb->m_maxBoundRect.x && childBox.x1 >= mb->m_maxBoundRect.x
//     && childBox.y2 <= mb->m_maxBoundRect.y && childBox.y1 >= mb->m_maxBoundRect.y
//     && childBox.z2 >= mb->m_maxBoundRect.z && childBox.z1 <= mb->m_maxBoundRect.z) {
//		 return true;
//	}
//
//	if(childBox.x2 <= mb->m_maxBoundRect.x && childBox.x1 >= mb->m_maxBoundRect.x
//     && childBox.y2 <= mb->m_maxBoundRect.y && childBox.y1 >= mb->m_maxBoundRect.y
//     && childBox.z2 >= mb->m_minBoundRect.z && childBox.z1 <= mb->m_minBoundRect.z) {
//		 return true;
//	}
//
//	if(childBox.x2 <= mb->m_maxBoundRect.x && childBox.x1 >= mb->m_maxBoundRect.x
//     && childBox.y2 <= mb->m_minBoundRect.y && childBox.y1 >= mb->m_minBoundRect.y
//     && childBox.z2 >= mb->m_maxBoundRect.z && childBox.z1 <= mb->m_maxBoundRect.z) {
//		 return true;
//	}
//
//	if(childBox.x2 <= mb->m_maxBoundRect.x && childBox.x1 >= mb->m_maxBoundRect.x
//     && childBox.y2 <= mb->m_minBoundRect.y && childBox.y1 >= mb->m_minBoundRect.y
//     && childBox.z2 >= mb->m_minBoundRect.z && childBox.z1 <= mb->m_minBoundRect.z) {
//		 return true;
//	}
//
//	if(childBox.x2 <= mb->m_minBoundRect.x && childBox.x1 >= mb->m_minBoundRect.x
//     && childBox.y2 <= mb->m_maxBoundRect.y && childBox.y1 >= mb->m_maxBoundRect.y
//     && childBox.z2 >= mb->m_maxBoundRect.z && childBox.z1 <= mb->m_maxBoundRect.z) {
//		 return true;
//	}
//
//	if(childBox.x2 <= mb->m_minBoundRect.x && childBox.x1 >= mb->m_minBoundRect.x
//     && childBox.y2 <= mb->m_maxBoundRect.y && childBox.y1 >= mb->m_maxBoundRect.y
//     && childBox.z2 >= mb->m_minBoundRect.z && childBox.z1 <= mb->m_minBoundRect.z) {
//		 return true;
//	}
//
//	if(childBox.x2 <= mb->m_minBoundRect.x && childBox.x1 >= mb->m_minBoundRect.x
//     && childBox.y2 <= mb->m_minBoundRect.y && childBox.y1 >= mb->m_minBoundRect.y
//     && childBox.z2 >= mb->m_maxBoundRect.z && childBox.z1 <= mb->m_maxBoundRect.z) {
//		 return true;
//	}
//
//	if(childBox.x2 <= mb->m_minBoundRect.x && childBox.x1 >= mb->m_minBoundRect.x
//     && childBox.y2 <= mb->m_minBoundRect.y && childBox.y1 >= mb->m_minBoundRect.y
//     && childBox.z2 >= mb->m_minBoundRect.z && childBox.z1 <= mb->m_minBoundRect.z) {
//		 return true;
//	}
//	return false;
//}
//

/*
bool COctree::cubeIntersect(BoundingBox childBox, CMeshBufferGroup *mb)
{
	//check all 8 point
	if(childBox.x2 < mb->m_maxBoundRect.x || childBox.x1 > mb->m_maxBoundRect.x
     || childBox.y2 < mb->m_maxBoundRect.y || childBox.y1 > mb->m_maxBoundRect.y
     || childBox.z2 < mb->m_maxBoundRect.z || childBox.z1 > mb->m_maxBoundRect.z) {
		 return false;
	}

	if(childBox.x2 < mb->m_maxBoundRect.x || childBox.x1 > mb->m_maxBoundRect.x
     || childBox.y2 < mb->m_maxBoundRect.y || childBox.y1 > mb->m_maxBoundRect.y
     || childBox.z2 < mb->m_minBoundRect.z || childBox.z1 > mb->m_minBoundRect.z) {
		 return false;
	}

	if(childBox.x2 < mb->m_maxBoundRect.x || childBox.x1 > mb->m_maxBoundRect.x
     || childBox.y2 < mb->m_minBoundRect.y || childBox.y1 > mb->m_minBoundRect.y
     || childBox.z2 < mb->m_maxBoundRect.z || childBox.z1 > mb->m_maxBoundRect.z) {
		 return false;
	}

	if(childBox.x2 < mb->m_maxBoundRect.x || childBox.x1 > mb->m_maxBoundRect.x
     || childBox.y2 < mb->m_minBoundRect.y || childBox.y1 > mb->m_minBoundRect.y
     || childBox.z2 < mb->m_minBoundRect.z || childBox.z1 > mb->m_minBoundRect.z) {
		 return false;
	}

	if(childBox.x2 < mb->m_minBoundRect.x || childBox.x1 > mb->m_minBoundRect.x
     || childBox.y2 < mb->m_maxBoundRect.y || childBox.y1 > mb->m_maxBoundRect.y
     || childBox.z2 < mb->m_maxBoundRect.z || childBox.z1 > mb->m_maxBoundRect.z) {
		 return false;
	}

	if(childBox.x2 < mb->m_minBoundRect.x || childBox.x1 > mb->m_minBoundRect.x
     || childBox.y2 < mb->m_maxBoundRect.y || childBox.y1 > mb->m_maxBoundRect.y
     || childBox.z2 < mb->m_minBoundRect.z || childBox.z1 > mb->m_minBoundRect.z) {
		 return false;
	}

	if(childBox.x2 < mb->m_minBoundRect.x || childBox.x1 > mb->m_minBoundRect.x
     || childBox.y2 < mb->m_minBoundRect.y || childBox.y1 > mb->m_minBoundRect.y
     || childBox.z2 < mb->m_maxBoundRect.z || childBox.z1 > mb->m_maxBoundRect.z) {
		 return false;
	}

	if(childBox.x2 < mb->m_minBoundRect.x || childBox.x1 > mb->m_minBoundRect.x
     || childBox.y2 < mb->m_minBoundRect.y || childBox.y1 > mb->m_minBoundRect.y
     || childBox.z2 < mb->m_minBoundRect.z || childBox.z1 > mb->m_minBoundRect.z) {
		 return false;
	}
	return true;
}
*/
bool COctree::cubeInside(BoundingBox childBox, CMeshBufferGroup *mb)
{
	if( (mb->m_minBoundRect.x >= childBox.x1) && (mb->m_maxBoundRect.x <= childBox.x2) 
		&& (mb->m_minBoundRect.y >= childBox.y1) && (mb->m_maxBoundRect.y <= childBox.y2)
		&& (mb->m_minBoundRect.z >= childBox.z1) && (mb->m_maxBoundRect.z <= childBox.z2) )
		return true;
	return false;
}

///
//	BuildTree()
//
//      After building the root node, this function recursively subdivides
//      the tree into octants.  Each octants gets a new bounding box and
//      all the polygons in the parent are tested to see which lie within
//      the new box.  Stops once each node contains no more than TrisPerNode
//
void COctree::BuildTree(OCT_NODE *Node, int minMeshPerNode, std::vector<CMeshBufferGroup*> &in)
{
	char buf[128];
	OCT_NODE tmp;
	GetBox(Node->BBox, &tmp.BBox, 0);
//	if( glm::abs(tmp.BBox.x2 - tmp.BBox.x1) <=10)
	if (glm::abs(tmp.BBox.x2 - tmp.BBox.x1) <= 20)
		return;

	//	Node has more than threshold number of triangles, create children
	if(Node->meshBufferIndexCount > minMeshPerNode)
	{
		int i, mbindex=0;
		if(writeNode) {
			sprintf_s(buf,128,"ParentNode [%d,%d] X:%.2f,%.2f  Y:%.2f,%.2f  Z:%.2f,%.2f", Node->meshBufferIndexStart, Node->meshBufferIndexCount, Node->BBox.x1, Node->BBox.x2, Node->BBox.y1, Node->BBox.y2, Node->BBox.z1, Node->BBox.z2);
			ofs << buf << std::endl;
		}
		for(i=0; i<NUM_CHILDREN; i++)
		{
			int       j;
			pOCT_NODE  NewNode = new OCT_NODE;
			
			//	Set this node's child pointer
			Node->ChildIdx[i]= m_vecOctNode.size();

			//	Partition the new box for the child
			GetBox(Node->BBox, &NewNode->BBox, i);
			
			if(writeNode) {
				sprintf_s(buf,128,"Parent: %d,%d] ChildBox %d)  X:(%.2f,%.2f)  Y:(%.2f,%.2f)  Z:(%.2f,%.2f)", Node->meshBufferIndexStart, Node->meshBufferIndexCount, i,
					NewNode->BBox.x1, NewNode->BBox.x2,NewNode->BBox.y1,NewNode->BBox.y2,NewNode->BBox.z1,NewNode->BBox.z2);
				ofs << buf << std::endl;
			}
			//	Initialize fields
			for(j= 0; j < NUM_CHILDREN; j++)
				NewNode->ChildIdx[j]= NULL_NODE;

			for(j= 0; j < NUM_NEIGHBORS; j++)
				NewNode->NeighborIdx[j]= NULL_NODE;

			//	See which of the parent's triangles lie within the new node
			NewNode->meshBufferIndexCount= 0;
			NewNode->meshBufferIndexStart= m_vecMeshBufferIndexPtr.size();
			int s = Node->meshBufferIndexStart;
			//foreach mb in parent, check if it intersect the child node, add to child
			for(j=0; j<Node->meshBufferIndexCount; j++,s++) {
				mbindex = m_vecMeshBufferIndexPtr[s];
				CMeshBufferGroup *pmb = in[mbindex];

				//if(cubeIntersect(NewNode->BBox, pmb)) {
				//	m_vecMeshBufferIndexPtr.push_back(mbindex);
				//	NewNode->meshBufferIndexCount++;
				//}

				//add only if the mb is completely inside the newNode
				if(cubeInside(NewNode->BBox, pmb)) {
					if(writeNode) {
						sprintf_s(buf,128,"mzb:%d, mmb:%d  X:(%.2f,%.2f),  Y:(%.2f,%.2f),  Z:(%.2f,%.2f)",
							pmb->m_MZBIndex, pmb->m_MMBIndex, pmb->m_minBoundRect.x, pmb->m_maxBoundRect.x, pmb->m_minBoundRect.y, pmb->m_maxBoundRect.y, pmb->m_minBoundRect.z, pmb->m_maxBoundRect.z);
						ofs << buf << std::endl;
					}
					m_vecMeshBufferIndexPtr.push_back(mbindex);
					NewNode->meshBufferIndexCount++;
				}
			}

			///
			//	Add the new node and recurse on its children
			//
			m_vecOctNode.push_back(NewNode);
			BuildTree((OCT_NODE*)m_vecOctNode.back(), minMeshPerNode, in);
		}
	}
}

int COctree::Create(std::vector<CMeshBufferGroup*> &in, int minMeshPerNode)
{
	if(!BuildRootNode(in))
		return 0;

	if(writeNode)
		ofs.open("octnode.txt");

	BuildTree((OCT_NODE*)m_vecOctNode.back(), minMeshPerNode, in);
	std::cout << "Oct Node: " << m_vecOctNode.size() << "  meshBuffer Index: " << m_vecMeshBufferIndexPtr.size() << std::endl;

	if(writeNode)
		ofs.close();

	return 1;
}

void COctree::GetTables(std::vector<OCT_NODE*> &outOctNode, std::vector<int> &outIndexPtr)
{
	outOctNode=m_vecOctNode;
	outIndexPtr=m_vecMeshBufferIndexPtr;
}
