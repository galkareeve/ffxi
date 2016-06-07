#include "StdAfx.h"
#include "Dat30.h"

#include <iostream>
#include <fstream>
#include "myStruct.h"
#include <Windows.h>

using namespace std;
CDat30::CDat30(void)
{
	m_totalDDS=0;
	m_curDDS=0;
}


CDat30::~CDat30(void)
{
}

int CDat30::getOriginalTotalDDS()
{
	return 1;
}

int CDat30::getTotalDDS()
{
	return m_totalDDS;
}

char* CDat30::getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap)
{
	std::ifstream ifs ("30.dat", std::ifstream::in|std::ifstream::binary);
	if( !ifs ) {
		std::cout << "unable to open infile" << std::endl;
		return nullptr;
	}
	char *pStorage = nullptr;
	DDS_Header header;
	ifs.seekg(145);

	ifs.read((char*)&header, 52);
	width = header.width;
	height = header.height;
	mipmap=1;
	pStorage=createData(ifs, header.size);

	ifs.close();
	return pStorage;
}

void CDat30::parseFrame()
{

}

void CDat30::parseMesh()
{
	DWORD *b=nullptr;
	string str;
	char buf[500];
	
	ifstream ifs ("30.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) {
		cout << "unable to open infile" << endl;
		return;
	}
	char *pStorage = new char[1720];
	memset(pStorage, 0, 1720);	

	ofstream ofs ("mesh30.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	//read mesh
	ifs.seekg(4406);
	ifs.read(pStorage,1320);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	uvFace *uvF=nullptr;
	char *ptr=pStorage;
	//write length
	ofs.write("44\n",3);
	for(int i=0; i<44; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(6022);
	ifs.read(pStorage,1720);
	if( ifs.fail() ) {
		cout << "error walk read2: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}

	vertNormal *vN=nullptr;
	ptr = pStorage;
	ofs.write("72\n",3);
	for(int i=0; i<72; ++i) {
		vN = (vertNormal*)ptr;
		sprintf_s(buf,"%f,%f,%f (%f,%f,%f)\n",vN->vertices.x,vN->vertices.y,vN->vertices.z,vN->normal.x,vN->normal.y,vN->normal.z);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(vertNormal);
	}

	ifs.close();
	ofs.close();
	delete pStorage;
}
