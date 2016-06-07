#include "StdAfx.h"
#include "Dat69.h"
#include <iostream>
#include <fstream>
#include "myStruct.h"
#include <Windows.h>

using namespace std;
CDat69::CDat69(void)
{
	m_totalDDS=0;
	m_curDDS=0;
}


CDat69::~CDat69(void)
{
}

int CDat69::getOriginalTotalDDS()
{
	return 1;
}

int CDat69::getTotalDDS()
{
	return m_totalDDS;
}

char* CDat69::getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap)
{
	std::ifstream ifs ("69.dat", std::ifstream::in|std::ifstream::binary);
	if( !ifs ) {
		std::cout << "unable to open infile" << std::endl;
		return nullptr;
	}
	char *pStorage = nullptr;
	DDS_Header header;
	ifs.seekg(101825);

	ifs.read((char*)&header, 52);
	width = header.width;
	height = header.height;
	mipmap=1;
	pStorage=createData(ifs, header.size);

	ifs.close();
	return pStorage;
}

void CDat69::parseMesh()
{
	DWORD *b=nullptr;
	string str;
	char buf[500];
	
	ifstream ifs ("69.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) {
		cout << "unable to open infile" << endl;
		return;
	}
	char *pStorage = new char[1800];
	memset(pStorage, 0, 1800);	

	ofstream ofs ("mesh69.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}

	ifs.seekg(106132);
	ifs.read(pStorage,1800);
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
	ofs.write("60\n",3);
	for(int i=0; i<60; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(108096);
	ifs.read(pStorage,864);
	if( ifs.fail() ) {
		cout << "error walk read3: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	vertNormal *v=nullptr;
	ptr = pStorage;
	ofs.write("36\n",3);
	for(int i=0; i<36; ++i) {
		v = (vertNormal*)ptr;
		sprintf_s(buf,"%f,%f,%f\n",v->vertices.x,v->vertices.z,v->vertices.y,v->normal.x,v->normal.z,v->normal.y);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(vertNormal);
	}

	ifs.close();
	ofs.close();
	delete pStorage;
}

void CDat69::parseFrame()
{
	string str;
	char buf[500];
	block *b=nullptr;
	ifstream ifs ("69.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) cout << "unable to open infile" << endl;

	char *pStorage = new char[864];
	memset(pStorage, 0, 864);

	ifs.seekg(7434);
	ifs.read(pStorage, 756);
	if( ifs.fail() ) {
		cout << "error walk read: " << ifs.gcount() << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	
	ofstream ofs ("frame69.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}

	ofs.write("fh4\n",4);
	char *ptr=pStorage;
	for(int i=0; i<9; ++i) {
		b = (block*)ptr;

		sprintf_s(buf,500,"frame:%d) rot:%u %u %u %u : %f %f %f %f, tra:%u %u %u : %f %f %f, sca:%u %u %u : %f %f %f\n",b->frame,b->rot1,b->rot2,b->rot3,b->rot4,b->rotf1,b->rotf2,b->rotf3,b->rotf4,
			b->tra1,b->tra2,b->tra3,b->traf1,b->traf2,b->traf3,b->sca1,b->sca2,b->sca3,b->scaf1,b->scaf2,b->scaf3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());

		ptr+=sizeof(block);
	}

	ifs.seekg(8190);
	ifs.read(pStorage,864);
	if( ifs.fail() ) {
		cout << "error walk read3: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}

	vertNormal *v=nullptr;
	ptr = pStorage;
	ofs.write("36\n",3);
	for(int i=0; i<36; ++i) {
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
