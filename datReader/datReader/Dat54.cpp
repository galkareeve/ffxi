#include "StdAfx.h"
#include "Dat54.h"
#include <iostream>
#include <fstream>
#include "myStruct.h"
#include "CorelateMgr.h"
#include <vector>

using namespace std;
CDat54::CDat54(void)
{
	m_totalDDS=0;
	m_curDDS=0;
}


CDat54::~CDat54(void)
{
}

int CDat54::getOriginalTotalDDS()
{
	return 7;
}

int CDat54::getTotalDDS()
{
	return m_totalDDS;
}

char* CDat54::getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap)
{
	std::ifstream ifs ("54.dat", std::ifstream::in|std::ifstream::binary);
	if( !ifs ) {
		std::cout << "unable to open infile" << std::endl;
		return nullptr;
	}
	char *pStorage = nullptr;
	DDS_Header header;

	switch(i)
	{
	case 0:	ifs.seekg(1889);
		break;

	case 1:	ifs.seekg(14529);
		break;

	case 2:	ifs.seekg(62209);
		break;

	case 3:	ifs.seekg(79537);
		break;

	case 4:	ifs.seekg(237857);
		break;

	case 5:	ifs.seekg(246689);
		break;

	case 6:	ifs.seekg(255889);
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

void CDat54::parseFrame()
{

}

void CDat54::parseMesh()
{
	string str;
	char buf[500];
	
	ifstream ifs ("54.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) {
		cout << "unable to open infile" << endl;
		return;
	}
	char *pStorage = new char[14000];
	memset(pStorage, 0, 14000);	
	char *ptr=pStorage;

	ofstream ofs ("mesh54.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}
/*
	ifs.seekg(272484);
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

	ifs.seekg(276328);
	ifs.read(pStorage,2100);
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
	ofs.write("70\n",3);
	for(int i=0; i<70; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(279142);
	ifs.read(pStorage,8136);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	vertNormal *v=nullptr;
	ptr = pStorage;
	ofs.write("339\n",4);
	for(int i=0; i<339; ++i) {
		v = (vertNormal*)ptr;
		sprintf_s(buf,"%d) %f,%f,%f  (%f,%f,%f)\n",i,v->vertices.x,v->vertices.z,v->vertices.y, v->normal.x,v->normal.z,v->normal.y);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(vertNormal);
	}
*/

	ifs.seekg(287444);
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

	ifs.seekg(291288);
	ifs.read(pStorage,1710);
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
	ofs.write("57\n",3);
	for(int i=0; i<57; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(293648);
	ifs.read(pStorage,6984);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	vertNormal *v=nullptr;
	ptr = pStorage;
	ofs.write("291\n",4);
	for(int i=0; i<291; ++i) {
		v = (vertNormal*)ptr;
		sprintf_s(buf,"%d) %f,%f,%f  (%f,%f,%f)\n",i,v->vertices.x,v->vertices.z,v->vertices.y, v->normal.x,v->normal.z,v->normal.y);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(vertNormal);
	}
	ifs.close();
	ofs.close();
	delete pStorage;
}

void CDat54::corelateNormal()
{

}
