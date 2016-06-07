#include "StdAfx.h"
#include "Dat34.h"

#include "myStruct.h"
#include "CorelateMgr.h"
#include "DDSLoader.h"
#include "MyMathLib.h"

using namespace std;
CDat34::CDat34(void)
{
	m_totalDDS=0;
	m_curDDS=0;
}


CDat34::~CDat34(void)
{
	m_vecDDS.clear();
}

int CDat34::getOriginalTotalDDS()
{
	return 5;
}

int CDat34::getTotalDDS()
{
	return m_totalDDS;
}

char* CDat34::getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap)
{
	std::ifstream ifs ("34.dat", std::ifstream::in|std::ifstream::binary);
	if( !ifs ) {
		std::cout << "unable to open infile" << std::endl;
		return nullptr;
	}
	char *pStorage = nullptr;
	DDS_Header header;

	switch(i)
	{
	case 0:	ifs.seekg(51665);
		break;

	case 1:	ifs.seekg(84529);
		break;

	case 2:	ifs.seekg(92817);
		break;

	case 3:	ifs.seekg(101105);
		break;

	case 4:	ifs.seekg(133969);
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

void CDat34::parseFrameMesh()
{
	unsigned int left=6636;
	string str;
	char buf[500];
	block *b=nullptr;
	ifstream ifs ("34.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) cout << "unable to open infile" << endl;

	char *pStorage = new char[left];
	memset(pStorage, 0, left);
	//read mesh
	myVector *v=nullptr;
	pStorage = new char[10272];
	ofstream ofs ("framemesh34.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}

	//mesh
	ifs.seekg(14742, ios_base::beg);
	ifs.read(pStorage,10272);

	char *ptr=pStorage;
	ofs.write("wlk mesh\n",9);
	ofs.write("========\n",9);
	for(int i=0; i<856; ++i) {
		v = (myVector*)ptr;
		sprintf_s(buf,500,"%f %f %f\n", v->x, v->y, v->z);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(myVector);
	}
	ifs.close();
	ofs.close();
	delete pStorage;

}

void CDat34::parseFrame()
{
	unsigned int left=6636;
	string str;
	char buf[500];
	block *b=nullptr;
	ifstream ifs ("34.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) cout << "unable to open infile" << endl;

	char *pStorage = new char[left];
	memset(pStorage, 0, left);
	ifs.seekg(8106);

	ifs.read(pStorage, left);
	if( ifs.fail() ) {
		cout << "error walk read: " << ifs.gcount() << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	
	ofstream ofs ("frame34.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}

	ofs.write("walk\n",5);
	ofs.write("====\n",5);
	char *ptr=pStorage;
	for(int i=0; i<79; ++i) {
		b = (block*)ptr;

		sprintf_s(buf,500,"frame:%d) rot:%u %u %u %u : %f %f %f %f, tra:%u %u %u : %f %f %f, sca:%u %u %u : %f %f %f\n",b->frame,b->rot1,b->rot2,b->rot3,b->rot4,b->rotf1,b->rotf2,b->rotf3,b->rotf4,
			b->tra1,b->tra2,b->tra3,b->traf1,b->traf2,b->traf3,b->sca1,b->sca2,b->sca3,b->scaf1,b->scaf2,b->scaf3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());

		ptr+=sizeof(block);
	}

	ifs.seekg(25050, ios_base::beg);
	memset(pStorage, 0, left);
	ifs.read(pStorage, left);
	if( ifs.fail() ) {
		cout << "error idl read: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	ofs.write("idl0\n",5);
	ofs.write("====\n",5);
	ptr=pStorage;
	for(int i=0; i<79; ++i) {
		b = (block*)ptr;

		sprintf_s(buf,500,"frame:%d) rot:%u %u %u %u : %f %f %f %f, tra:%u %u %u : %f %f %f, sca:%u %u %u : %f %f %f\n",b->frame,b->rot1,b->rot2,b->rot3,b->rot4,b->rotf1,b->rotf2,b->rotf3,b->rotf4,
			b->tra1,b->tra2,b->tra3,b->traf1,b->traf2,b->traf3,b->sca1,b->sca2,b->sca3,b->scaf1,b->scaf2,b->scaf3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());

		ptr+=sizeof(block);
	}

	ifs.seekg(39466, ios_base::beg);
	memset(pStorage, 0, left);
	ifs.read(pStorage, left);
	if( ifs.fail() ) {
		cout << "error run read: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	ofs.write("run0\n",5);
	ofs.write("====\n",5);
	ptr=pStorage;
	for(int i=0; i<79; ++i) {
		b = (block*)ptr;

		sprintf_s(buf,500,"frame:%d) rot:%u %u %u %u : %f %f %f %f, tra:%u %u %u : %f %f %f, sca:%u %u %u : %f %f %f\n",b->frame,b->rot1,b->rot2,b->rot3,b->rot4,b->rotf1,b->rotf2,b->rotf3,b->rotf4,
			b->tra1,b->tra2,b->tra3,b->traf1,b->traf2,b->traf3,b->sca1,b->sca2,b->sca3,b->scaf1,b->scaf2,b->scaf3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());

		ptr+=sizeof(block);
	}

	ifs.close();
	ofs.close();
	delete pStorage;

}

void CDat34::parseBaseMesh()
{
	string str;
	char buf[500];
	baseMesh *b=nullptr;
	ifstream ifs ("34.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) cout << "unable to open infile" << endl;

	char *pStorage = new char[2370];
	memset(pStorage, 0, 2370);
	ifs.seekg(84);

	ifs.read(pStorage, 2370);
	if( ifs.fail() ) {
		cout << "error walk read: " << ifs.gcount() << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	
	ofstream ofs ("base34.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}

	ofs.write("base\n",5);
	ofs.write("====\n",5);
	char *ptr=pStorage;
	for(int i=0; i<79; ++i) {
		b = (baseMesh*)ptr;

		sprintf_s(buf,500,"index:%d flag:%d) %f %f %f %f %f %f %f\n",(int)b->index,(int)b->flag,b->f1,b->f2,b->f3,b->f4,b->f5,b->f6,b->f7);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());

		ptr+=sizeof(baseMesh);
	}

	ifs.close();
	ofs.close();
	delete pStorage;
}

void CDat34::parseMesh()
{
	string str;
	char buf[600];
	uvFace *uvF=nullptr;
	char *ptr=nullptr;
	MODELVERTEX2 *mv=nullptr;
	unsigned int *hex;

	ifstream ifs ("34.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) {
		cout << "unable to open infile" << endl;
		return;
	}
	char *pStorage = new char[8000];
	memset(pStorage, 0, 8000);	

	ofstream ofs ("mesh34.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}
/*
	//hf_b11 dagger_case
	ifs.seekg(150564);
	ifs.read(pStorage,2220);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}

	//write length
	uvF=nullptr;
	ptr=pStorage;
	ofs.write("74\n",3);
	for(int i=0; i<74; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,500,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}
	ifs.seekg(153072);
	ifs.read(pStorage,1104);
	if( ifs.fail() ) {
		cout << "error walk read3: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}

	vertNormal *v=nullptr;
	ptr = pStorage;
	ofs.write("64\n",3);
	for(int i=0; i<46; ++i) {
		v = (vertNormal*)ptr;
		sprintf_s(buf,"%d) %f,%f,%f\n",i,v->vertices.x,v->vertices.y,v->vertices.z);
//		sprintf_s(buf,500,"%f,%f,%f\n",v->x,v->z,v->y);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(vertNormal);
//		ptr += sizeof(myVector);
	}

	ifs.read(pStorage,1008);
	ptr=pStorage;
	vertNormalExtra *vNE=nullptr;
	for(int i=0; i<18; ++i) {
		vNE = (vertNormalExtra*)ptr;
		sprintf_s(buf,600,"%d) %f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",i+46,vNE->vertices.x,vNE->vertices.y,vNE->vertices.z, vNE->normal.x,vNE->normal.y,vNE->normal.z,
			vNE->unk1.x,vNE->unk1.y,vNE->unk1.z, vNE->unk2.x,vNE->unk2.y,vNE->unk2.z, vNE->uv.u, vNE->uv.v);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(vertNormalExtra);
	}
*/
/*
	//hf_b11 weapon, decode ok
	ifs.seekg(155332);
	ifs.read(pStorage,840);
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
	ofs.write("28\n",3);
	for(int i=0; i<28; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	//array float....verts?
	vertNormal *v=nullptr;
	ifs.seekg(156274, ios_base::beg);
	ifs.read(pStorage,552);

	ptr=pStorage;
	ofs.write("23\n",3);
	for(int i=0; i<23; ++i) {
		v = (vertNormal*)ptr;
		sprintf_s(buf,500,"%f,%f,%f\n", v->vertices.x, v->vertices.z, v->vertices.y);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(vertNormal);
	}
*/

//	//hf_h11
	//128
//	ifs.seekg(156980);
//	ifs.read(pStorage,3840);
//	uvF=nullptr;
//	ptr=pStorage;
//	//write length
//	ofs.write("473\n",4);
//	for(int i=0; i<128; ++i) {
//		uvF = (uvFace*)ptr;
//		//skip uv
//		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(uvFace);
//	}
//	//128
//	ifs.seekg(160824);
//	ifs.read(pStorage,3840);
//	uvF=nullptr;
//	ptr=pStorage;
//	//write length
////	ofs.write("128\n",4);
//	for(int i=0; i<128; ++i) {
//		uvF = (uvFace*)ptr;
//		//skip uv
//		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(uvFace);
//	}
//	//128
//	ifs.seekg(164668);
//	ifs.read(pStorage,3840);
//	uvF=nullptr;
//	ptr=pStorage;
//	//write length
/////	ofs.write("128\n",4);
//	for(int i=0; i<128; ++i) {
//		uvF = (uvFace*)ptr;
//		//skip uv
//		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(uvFace);
//	}
//	//89
//	ifs.seekg(168512);
//	ifs.read(pStorage,2670);
//	uvF=nullptr;
//	ptr=pStorage;
//	//write length
////	ofs.write("89\n",3);
//	for(int i=0; i<89; ++i) {
//		uvF = (uvFace*)ptr;
//		//skip uv
//		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(uvFace);
//	}
//
//	//flag
//	//ifs.seekg(171220);
//	//ifs.read(pStorage,1540);
//	//ptr = pStorage;
//	//for(int i=0; i<385; ++i) {
//	//	hex = (unsigned int*)ptr;
//	//	sprintf_s(buf,500,"%d) %X\n",i,*hex);
//	//	str.assign(buf);
//	//	ofs.write(str.c_str(), str.length());
//	//	ptr +=4;
//	//}
//
//	//vertices
//	ifs.seekg(172760);
//	ifs.read(pStorage,6336);
//	vertNormal *v=nullptr;
//	ptr = pStorage;
//	ofs.write("385\n",4);
//	for(int i=0; i<264; ++i) {
//		v = (vertNormal*)ptr;
////		sprintf_s(buf,600,"%d) %f,%f,%f  %f,%f,%f\n",i, v->vertices.x,v->vertices.y,v->vertices.z, v->normal.x, v->normal.y, v->normal.z);
//		sprintf_s(buf,600,"%f,%f,%f\n",v->vertices.x,v->vertices.y,v->vertices.z);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(vertNormal);
//	}
//
//	ifs.read(pStorage,6776);
//	ptr=pStorage;
//	vertNormalExtra *vNE=nullptr;
//	for(int i=0; i<121; ++i) {
//		mv = (MODELVERTEX2*)ptr;
//		sprintf_s(buf,500,"%f,%f,%f\n",mv->x1, mv->y1, mv->z1);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(MODELVERTEX2);
//	}
//
//
//	//hf_g11
	ifs.seekg(230420);
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
	ofs.write("204\n",4);
	for(int i=0; i<128; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}
	
	ifs.seekg(234264);
	ifs.read(pStorage,2280);
	if( ifs.fail() ) {
		cout << "error walk read2: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}
	uvF=nullptr;
	ptr=pStorage;
//	ofs.write("76\n",3);
	for(int i=0; i<76; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	////read flag
	//ifs.seekg(236570);
	//ifs.read(pStorage, 444);
	//ptr = pStorage;
	//for(int i=0; i<110; ++i) {
	//	hex = (unsigned int*)ptr;
	//	sprintf_s(buf,500,"%d) %X\n", i, *hex);
	//	str.assign(buf);
	//	ofs.write(str.c_str(), str.length());
	//	ptr +=4;
	//}

	//read vertices
	ifs.seekg(237014);
	ifs.read(pStorage,1392);
	if( ifs.fail() ) {
		cout << "error walk read3: " << ifs.gcount() << endl;
		ifs.close();
		ofs.close();
		delete pStorage;
		return;
	}

	vertNormal *v=nullptr;
	ptr = pStorage;
	ofs.write("111\n",4);
//	ofs.write("58\n",3);
	for(int i=0; i<58; ++i) {
		v = (vertNormal*)ptr;
//		sprintf_s(buf,600,"%d) %f,%f,%f  %f,%f,%f\n",i, v->vertices.x,v->vertices.y,v->vertices.z, v->normal.x, v->normal.y, v->normal.z);
		sprintf_s(buf,600,"%f,%f,%f\n",v->vertices.x,v->vertices.y,v->vertices.z);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(vertNormal);
	}

	ifs.read(pStorage,2968);
	ptr=pStorage;
	vertNormalExtra *vNE=nullptr;
	myVector myV;
	for(int i=0; i<53; ++i) {
		mv = (MODELVERTEX2*)ptr;
		sprintf_s(buf,500,"%f,%f,%f\n",(mv->x1/mv->w1+mv->x2/mv->w2),(mv->y1/mv->w1+mv->y2/mv->w2),(mv->z1/mv->w1+mv->z2/mv->w2));
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(MODELVERTEX2);
	}


/*
	//hf_f11
	ifs.seekg(224596);
	ifs.read(pStorage,2940);
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
	ofs.write("98\n",3);
	for(int i=0; i<98; ++i) {
		uvF = (uvFace*)ptr;
		//skip uv
		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(uvFace);
	}

	ifs.seekg(227858);
//	ifs.seekg(228434);
	ifs.read(pStorage,1824);
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
	ofs.write("76\n",3);
	for(int i=0; i<76; ++i) {
		v = (myVector*)ptr;
		sprintf_s(buf,"%f,%f,%f\n",v->x,v->z,v->y);
		str.assign(buf);
		ofs.write(str.c_str(), str.length());
		ptr += sizeof(myVector);
	}
*/


	//hf_l11 (leg)
//	ifs.seekg(186020);
//	ifs.read(pStorage,3840);
//	uvF=nullptr;
//	ptr=pStorage;
//	//write length
//	ofs.write("268\n",4);
//	for(int i=0; i<128; ++i) {
//		uvF = (uvFace*)ptr;
//		//skip uv
//		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(uvFace);
//	}
//	
//	ifs.seekg(189864);
//	ifs.read(pStorage,3840);
//	uvF=nullptr;
//	ptr=pStorage;
//	for(int i=0; i<128; ++i) {
//		uvF = (uvFace*)ptr;
//		//skip uv
//		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(uvFace);
//	}
//	ifs.seekg(193708);
//	ifs.read(pStorage,360);
//	uvF=nullptr;
//	ptr=pStorage;
//	for(int i=0; i<12; ++i) {
//		uvF = (uvFace*)ptr;
//		//skip uv
//		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(uvFace);
//	}
//
//	////read flag
//	ifs.seekg(194100);
//	ifs.read(pStorage, 580);
//	ptr = pStorage;
//	for(int i=0; i<145; ++i) {
//		hex = (unsigned int*)ptr;
//		sprintf_s(buf,500,"%d) %X\n", i, *hex);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr +=4;
//	}
//
//	//read vertices
//	ifs.seekg(194680);
//	ifs.read(pStorage,2208);
//	vertNormal *v=nullptr;
//	ptr = pStorage;
//	ofs.write("145\n",4);
//	for(int i=0; i<92; ++i) {
//		v = (vertNormal*)ptr;
//		sprintf_s(buf,600,"%d) %f,%f,%f  %f,%f,%f\n",i, v->vertices.x,v->vertices.y,v->vertices.z, v->normal.x, v->normal.y, v->normal.z);
////		sprintf_s(buf,600,"%f,%f,%f\n",v->vertices.x,v->vertices.y,v->vertices.z);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(vertNormal);
//	}
//
//	ifs.read(pStorage,2968);
//	ptr=pStorage;
//	vertNormalExtra *vNE=nullptr;
//	myVector myV;
//	for(int i=0; i<53; ++i) {
//		vNE = (vertNormalExtra*)ptr;
//		sprintf_s(buf,600,"%d) %f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",i+92,vNE->p0.x,vNE->p0.y,vNE->p0.z, vNE->p1.x,vNE->p1.y,vNE->p1.z,
//			vNE->p2.x,vNE->p2.y,vNE->p2.z, vNE->p3.x,vNE->p3.y,vNE->p3.z, vNE->blend.u, vNE->blend.v);
////		myV = CMyMathLib::bezierCubic(vNE->blend.u, vNE->p0, vNE->p1, vNE->p3, vNE->p2);
////		myV = CMyMathLib::straightLine(vNE->blend.u, vNE->p0, vNE->p1);
////		sprintf_s(buf,500,"%d) %f,%f,%f\n",i+58,myV.x, myV.y, myV.z);
////		sprintf_s(buf,500,"%f,%f,%f\n",myV.x, myV.y, myV.z);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(vertNormalExtra);
//	}

//	//hf_f11 (boot)
//	ifs.seekg(224596);
//	ifs.read(pStorage,2940);
//	uvF=nullptr;
//	ptr=pStorage;
//	//write length
//	ofs.write("98\n",3);
//	for(int i=0; i<98; ++i) {
//		uvF = (uvFace*)ptr;
//		//skip uv
//		sprintf_s(buf,"%d,%d,%d\n",uvF->face.v1,uvF->face.v2,uvF->face.v3);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(uvFace);
//	}
//	
//	//ifs.seekg(227554);
//	//ifs.read(pStorage,304);
//	//ptr = pStorage;
//	//for(int i=0; i<76; ++i) {
//	//	hex = (unsigned int*)ptr;
//	//	sprintf_s(buf,500,"%d) %X\n", i, *hex);
//	//	str.assign(buf);
//	//	ofs.write(str.c_str(), str.length());
//	//	ptr +=4;
//	//}
//
//	//read vertices
//	ifs.seekg(227858);
//	ifs.read(pStorage,1392);
//	vertNormal *v=nullptr;
//	ptr = pStorage;
//	ofs.write("76\n",3);
//	for(int i=0; i<58; ++i) {
//		v = (vertNormal*)ptr;
////		sprintf_s(buf,600,"%d) %f,%f,%f  %f,%f,%f\n",i, v->vertices.x,v->vertices.y,v->vertices.z, v->normal.x, v->normal.y, v->normal.z);
//		sprintf_s(buf,600,"%f,%f,%f\n",v->vertices.x,v->vertices.y,v->vertices.z);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(vertNormal);
//	}
//
//	ifs.read(pStorage,1008);
//	ptr=pStorage;
////	vertNormalExtra *vNE=nullptr;
//	
//	myVector myV;
//	for(int i=0; i<18; ++i) {
//		mv = (MODELVERTEX2*)ptr;
//		sprintf_s(buf,500,"%f,%f,%f\n",mv->x1, mv->y1, mv->z1);
//		str.assign(buf);
//		ofs.write(str.c_str(), str.length());
//		ptr += sizeof(MODELVERTEX2);
//	}
	ifs.close();
	ofs.close();
	delete pStorage;
}

void CDat34::corelateNormal()
{
	ifstream ifs ("34.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) {
		cout << "unable to open infile" << endl;
		return;
	}
	char *pStorage = new char[2940];
	memset(pStorage, 0, 2940);	

	//hf_f11
	vector<myFace> vecFaces;
	vector<myVector> vecVertex;

	ifs.seekg(224596);
	ifs.read(pStorage,2940);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	uvFace *uvF=nullptr;
	char *ptr=pStorage;
	for(int i=0; i<98; ++i) {
		uvF = (uvFace*)ptr;
		vecFaces.push_back(uvF->face);
		ptr += sizeof(uvFace);
	}

	ifs.seekg(227858);
	ifs.read(pStorage,2400);
	if( ifs.fail() ) {
		cout << "error walk read3: " << ifs.gcount() << endl;
		ifs.close();
		delete pStorage;
		return;
	}

	vertNormal *v=nullptr;
	ptr = pStorage;
	for(int i=0; i<100; ++i) {
		v = (vertNormal*)ptr;
		vecVertex.push_back(v->normal);
		ptr += sizeof(vertNormal);
	}

	ifs.close();
	delete pStorage;

	ofstream ofs ("normal34.txt", std::ofstream::out);
	if( !ofs ) {
		cout << "unable to open outfile" << endl;
		ifs.close();
		delete pStorage;
		return;
	}

	CCorelateMgr mgr;
	int maxIndices = mgr.findMaxVertexIndices(vecFaces);
	for(int i=0; i<maxIndices; ++i)
		mgr.corelate(ofs, i, vecFaces, vecVertex);

	ofs.close();
}

void CDat34::extractDDS()
{
/*	ifstream ifs ("34.dat", std::ifstream::in|ifstream::binary);
	if( !ifs ) {
		cout << "unable to open infile" << endl;
		return;
	}
	char *pStorage = new char[32768];
	memset(pStorage, 0, 32768);	

	ifs.seekg(51717);
	ifs.read(pStorage,32768);
	if( ifs.fail() ) {
		cout << "error walk read1: " << ifs.gcount() << endl;
		ifs.close();
		delete pStorage;
		return;
	}
	
	CDDSLoader loader;
	loader.loadDDS(pStorage, 128, 128, 1);

	ifs.close();
	delete pStorage;
*/


	FILE *f;
	errno_t err;
	CDDSLoader loader;

	if((err=fopen_s(&f,"foliage.dds", "r"))==0)
		loader.loadDDS(f);
	fclose(f);

}
