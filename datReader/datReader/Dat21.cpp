#include "StdAfx.h"
#include "Dat21.h"
#include <iostream>
#include <fstream>
#include "myStruct.h"
#include "CorelateMgr.h"
#include <vector>

using namespace std;
CDat21::CDat21(void)
{
	m_totalDDS=0;
	m_curDDS=0;
}


CDat21::~CDat21(void)
{
}

int CDat21::getOriginalTotalDDS()
{
	return 3;
}

int CDat21::getTotalDDS()
{
	return m_totalDDS;
}

char* CDat21::getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap)
{
	std::ifstream ifs ("21.dat", std::ifstream::in|std::ifstream::binary);
	if( !ifs ) {
		std::cout << "unable to open infile" << std::endl;
		return nullptr;
	}
	char *pStorage = nullptr;
	DDS_Header header;
	switch(i)
	{
	case 0:	ifs.seekg(10993);
		break;

	case 1:	ifs.seekg(15185);
		break;

	case 2:	ifs.seekg(16305);
		break;
	}

	ifs.read((char*)&header, 52);
	width = header.width;
	height = header.height;
	mipmap=1;
	pStorage=createData(ifs, header.size);

	ifs.close();
	return pStorage;
}

void CDat21::parseFrame()
{

}

void CDat21::parseMesh()
{
	string str;
	char buf[500];
	
	ifstream ifs ("21.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) {
		cout << "unable to open infile" << endl;
		return;
	}
	char *pStorage = new char[14000];
	memset(pStorage, 0, 14000);	
	char *ptr=pStorage;

	ofstream ofs ("mesh21.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}

	ifs.seekg(55012);
	ifs.read(pStorage,3840);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	uvFace *uvF=nullptr;
	//write length
	ofs.write("128\n",4);
	for(int i=0; i<128; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(58856);
	ifs.read(pStorage,3840);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	uvF=nullptr;
	ptr=pStorage;
	//write length
	ofs.write("128\n",4);
	for(int i=0; i<128; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(62700);
	ifs.read(pStorage,3840);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	uvF=nullptr;
	ptr=pStorage;
	//write length
	ofs.write("128\n",4);
	for(int i=0; i<128; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(66544);
	ifs.read(pStorage,2640);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	uvF=nullptr;
	ptr=pStorage;
	//write length
	ofs.write("88\n",3);
	for(int i=0; i<88; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(70452);
	ifs.read(pStorage,13948);
	if( ifs.fail() ) {
		cout << "error walk read3: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
//	vertNormal *v=nullptr;
	myVector *v=nullptr;
	ptr = pStorage;
	ofs.write("1162\n",5);
	for(int i=0; i<1162; ++i) {
//		v = (vertNormal*)ptr;
//		sprintf_s(buf,"%d) %f,%f,%f  (%f,%f,%f)\n",i,v->vertices.x,v->vertices.z,v->vertices.y, v->normal.x,v->normal.z,v->normal.y);
		v = (myVector*)ptr;
		sprintf_s(buf,"%f,%f,%f\n",v->x,v->z,v->y);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(vertNormal);
		ptr += sizeof(myVector);
	}

	ifs.close();
	ofs.close();
	delete pStorage;
}

void CDat21::corelateNormal()
{
	vector<myFace> vecFaces;
	vector<myVector> vecVertex;

	ifstream ifs ("21.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) {
		cout << "unable to open infile" << endl;
		return;
	}
	char *pStorage = new char[14000];
	memset(pStorage, 0, 14000);	
	char *ptr=pStorage;

	ifs.seekg(55012);
	ifs.read(pStorage,3840);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	uvFace *uvF=nullptr;
	for(int i=0; i<128; ++i) {
		uvF = (uvFace*)ptr;
		vecFaces.push_back(uvF->face);
		ptr += sizeof(uvFace);
	}

	ifs.seekg(58856);
	ifs.read(pStorage,3840);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	uvF=nullptr;
	ptr=pStorage;
	for(int i=0; i<128; ++i) {
		uvF = (uvFace*)ptr;
		vecFaces.push_back(uvF->face);
		ptr += sizeof(uvFace);
	}

	ifs.seekg(62700);
	ifs.read(pStorage,3840);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	uvF=nullptr;
	ptr=pStorage;
	for(int i=0; i<128; ++i) {
		uvF = (uvFace*)ptr;
		vecFaces.push_back(uvF->face);
		ptr += sizeof(uvFace);
	}

	ifs.seekg(66544);
	ifs.read(pStorage,2640);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	uvF=nullptr;
	ptr=pStorage;
	for(int i=0; i<88; ++i) {
		uvF = (uvFace*)ptr;
		vecFaces.push_back(uvF->face);
		ptr += sizeof(uvFace);
	}

	ifs.seekg(70452);
	ifs.read(pStorage,13948);
	if( ifs.fail() ) {
		cout << "error walk read3: " << ifs.gcount() << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	
	vertNormal *v=nullptr;
	ptr = pStorage;
	for(int i=0; i<581; ++i) {
		v = (vertNormal*)ptr;
		vecVertex.push_back(v->normal);
		ptr += sizeof(vertNormal);
	}

/*
	//corelate normal
	int index=0;
	float degree;
	for(int i=0; i<580; ++i) {
		v = listVects.front();
		listVects.pop_front();

		index=i+1;
		for(auto it=listVects.begin(); it!=listVects.end(); ++it) {
			degree = CMyMathLib::angleBtwVector(&v->normal, &(*it)->normal);

			sprintf_s(buf,"%d - %d) degree: %f\n",i, index, degree);
			str.assign(buf);
			ofs.write(str.c_str(), str.length());
			++index;
		}

	}
*/
	ifs.close();
	delete pStorage;

	ofstream ofs ("normal21.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		return;
	}

	CCorelateMgr mgr;
	int maxIndices = mgr.findMaxVertexIndices(vecFaces);
	for(int i=0; i<maxIndices; ++i)
		mgr.corelate(ofs, i, vecFaces, vecVertex);

	ofs.close();
}
