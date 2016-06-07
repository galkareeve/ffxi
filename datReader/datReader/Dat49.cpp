#include "StdAfx.h"
#include "Dat49.h"

#include <iostream>
#include <fstream>
#include "myStruct.h"
#include <Windows.h>

using namespace std;
CDat49::CDat49(void)
{
	m_totalDDS=0;
	m_curDDS=0;
}


CDat49::~CDat49(void)
{
}

int CDat49::getOriginalTotalDDS()
{
	return 3;
}

int CDat49::getTotalDDS()
{
	return m_totalDDS;
}

char* CDat49::getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap)
{
	std::ifstream ifs ("49.dat", std::ifstream::in|std::ifstream::binary);
	if( !ifs ) {
		std::cout << "unable to open infile" << std::endl;
		return nullptr;
	}
	char *pStorage = nullptr;
	DDS_Header header;
	switch(i)
	{
	case 0:	ifs.seekg(38257);
		break;

	case 1:	ifs.seekg(54737);
		break;

	case 2:	ifs.seekg(58929);
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

void CDat49::parseFrame()
{

}

void CDat49::parseMesh()
{

}

void CDat49::parseWalkMesh()
{
	DWORD *b=nullptr;
	string str;
	char buf[500];
	
	ifstream ifs ("49.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) {
		cout << "unable to open infile" << endl;
		return;
	}
	char *pStorage = new char[16800];
	memset(pStorage, 0, 16800);	

	ofstream ofs ("mesh49.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	//read mesh
	ifs.seekg(7498);
	ifs.read(pStorage,16800);
	if( ifs.fail() ) {
		cout << "error walk read: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	myVector *fv=nullptr;
	myIndices *iv=nullptr;
	char *ptr=pStorage;
	ofs.write("wlk mesh\n",9);
	ofs.write("========\n",9);
//	for(int i=0; i<4200; ++i) {
	for(int i=0; i<1400; ++i) {
		fv = (myVector*)ptr;
		iv = (myIndices*)ptr;
		sprintf_s(buf,500,"%u %u %u === %f %f %f \n", iv->x,iv->y,iv->z, fv->x,fv->y,fv->z);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(myVector);
	}
	ifs.close();
	ofs.close();
	delete pStorage;
}
