#include "StdAfx.h"
#include "Dat96.h"
#include <iostream>
#include <fstream>
#include "myStruct.h"
#include "CorelateMgr.h"
#include <vector>

using namespace std;
CDat96::CDat96(void)
{
	m_totalDDS=0;
	m_curDDS=0;
}


CDat96::~CDat96(void)
{
}

int CDat96::getOriginalTotalDDS()
{
	return 1;
}

int CDat96::getTotalDDS()
{
	return m_totalDDS;
}

char* CDat96::getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap)
{
	std::ifstream ifs ("96.dat", std::ifstream::in|std::ifstream::binary);
	if( !ifs ) {
		std::cout << "unable to open infile" << std::endl;
		return nullptr;
	}
	char *pStorage = nullptr;
	DDS_Header header;
	ifs.seekg(5297);

	ifs.read((char*)&header, 52);
	width = header.width;
	height = header.height;
	mipmap=1;
	pStorage=createData(ifs, header.size);

	ifs.close();
	return pStorage;
}

void CDat96::parseFrame()
{

}

void CDat96::parseMesh()
{
	string str;
	char buf[500];
	
	ifstream ifs ("96.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) {
		cout << "unable to open infile" << endl;
		return;
	}
	char *pStorage = new char[14000];
	memset(pStorage, 0, 14000);	
	char *ptr=pStorage;

	ofstream ofs ("mesh96.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}
//first
	ifs.seekg(71062);
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
		sprintf_s(buf,500,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(74906);
	ifs.read(pStorage,1200);
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
	ofs.write("40\n",3);
	for(int i=0; i<40; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,500,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(76530);
	ifs.read(pStorage,1248);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
//	vertNormal *v=nullptr;
	myVector *v=nullptr;
	ptr = pStorage;
	ofs.write("104\n",4);
	for(int i=0; i<104; ++i) {
		v = (myVector*)ptr;
//		sprintf_s(buf,500,"%d) %f,%f,%f  (%f,%f,%f)\n",i,v->vertices.x,v->vertices.z,v->vertices.y, v->normal.x,v->normal.z,v->normal.y);
		sprintf_s(buf,500,"%f,%f,%f\n",v->x,v->z,v->y);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(myVector);
	}

/*
//second
	ifs.seekg(80742);
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
		sprintf_s(buf,500,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(84586);
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
		sprintf_s(buf,500,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(88430);
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
		sprintf_s(buf,500,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(92274);
	ifs.read(pStorage,1680);
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
	ofs.write("56\n",3);
	for(int i=0; i<56; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,500,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(95726);
	ifs.read(pStorage,10560);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	v=nullptr;
	ptr = pStorage;
	ofs.write("440\n",4);
	for(int i=0; i<440; ++i) {
		v = (vertNormal*)ptr;
//		sprintf_s(buf,500,"%d) %f,%f,%f  (%f,%f,%f)\n",i,v->vertices.x,v->vertices.z,v->vertices.y, v->normal.x,v->normal.z,v->normal.y);
		sprintf_s(buf,500,"%f,%f,%f\n",v->vertices.x,v->vertices.z,v->vertices.y);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(vertNormal);
	}
*/

	ifs.close();
	ofs.close();
	delete pStorage;
}

void CDat96::corelateNormal()
{

}
