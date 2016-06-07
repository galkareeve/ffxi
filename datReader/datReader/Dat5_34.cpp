#include "StdAfx.h"
#include "Dat5_34.h"
#include <iostream>
#include <fstream>
#include "myStruct.h"

using namespace std;
CDat5_34::CDat5_34(void)
{
	m_totalDDS=0;
	m_curDDS=0;
}


CDat5_34::~CDat5_34(void)
{
}

int CDat5_34::getOriginalTotalDDS()
{
	return 1;
}

int CDat5_34::getTotalDDS()
{
	return m_totalDDS;
}

char* CDat5_34::getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap)
{
	std::ifstream ifs ("5_34.dat", std::ifstream::in|std::ifstream::binary);
	if( !ifs ) {
		std::cout << "unable to open infile" << std::endl;
		return nullptr;
	}
	char *pStorage = nullptr;
	DDS_Header header;
	ifs.seekg(353);

	ifs.read((char*)&header, 52);
	width = header.width;
	height = header.height;
	mipmap=1;
	pStorage=createData(ifs, header.size);

	ifs.close();
	return pStorage;
}

void CDat5_34::parseFrame()
{

}

void CDat5_34::parseMesh()
{
	DWORD *b=nullptr;
	string str;
	char buf[500];
	
	ifstream ifs ("5_34.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) {
		cout << "unable to open infile" << endl;
		return;
	}
	char *pStorage = new char[14000];
	memset(pStorage, 0, 14000);	

	ofstream ofs ("mesh5_34.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}

	ifs.seekg(9332);
	ifs.read(pStorage,570);
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
	ofs.write("19\n",3);
	for(int i=0; i<19; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(10046);
	ifs.read(pStorage,816);
	if( ifs.fail() ) {
		cout << "error walk read3: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}

	vertNormal *v=nullptr;
	ptr = pStorage;
	ofs.write("34\n",3);
	for(int i=0; i<34; ++i) {
		v = (vertNormal*)ptr;
		sprintf_s(buf,"%f,%f,%f\n",v->vertices.x,v->vertices.z,v->vertices.y);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(vertNormal);
	}

	ifs.close();
	ofs.close();
	delete pStorage;
}
