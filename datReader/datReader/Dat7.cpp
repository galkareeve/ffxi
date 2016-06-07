#include "StdAfx.h"
#include "Dat7.h"

#include "myStruct.h"
#include "CorelateMgr.h"
#include "DDSLoader.h"
#include "MyMathLib.h"

using namespace std;
CDat7::CDat7(void)
{
	m_totalDDS=0;
	m_curDDS=0;
}


CDat7::~CDat7(void)
{
	m_vecDDS.clear();
}

int CDat7::getOriginalTotalDDS()
{
	return 29;
}

int CDat7::getTotalDDS()
{
	return m_totalDDS;
}

char* CDat7::getDDS(int i, unsigned int &width, unsigned int &height, unsigned int &mipmap)
{
	std::ifstream ifs ("7.dat", std::ifstream::in|std::ifstream::binary);
	if( !ifs ) {
		std::cout << "unable to open infile" << std::endl;
		return nullptr;
	}
	char *pStorage = nullptr;
	DDS_Header header;

	switch(i)
	{
	case 0:	ifs.seekg(2331473);
		break;

	case 1:	ifs.seekg(2462641);
		break;

	case 2:	ifs.seekg(2987025);
		break;

	case 3:	ifs.seekg(3118233-40);
		break;

	case 4:	ifs.seekg(3151097-40);
		break;

	case 5:	ifs.seekg(3183961-40);
		break;

	case 6:	ifs.seekg(3249593-40);
		break;

	case 7:	ifs.seekg(3315225-40);
		break;

	case 8:	ifs.seekg(3331705-40);
		break;

	case 9:	ifs.seekg(3397337-40);
		break;

	case 10:	ifs.seekg(3462969-40);
		break;

	case 11:	ifs.seekg(3528601-40);
		break;

	case 12:	ifs.seekg(3659769-40);
		break;

	case 13:	ifs.seekg(3790937-40);
		break;

	case 14:	ifs.seekg(3856569-40);
		break;

	case 15:	ifs.seekg(3987737-40);
		break;

	case 16:	ifs.seekg(8835577-40);
		break;

	case 17:	ifs.seekg(8839769-40);
		break;

	case 18:	ifs.seekg(8843961-40);
		break;

	case 19:	ifs.seekg(8848153-40);
		break;

	case 20:	ifs.seekg(9256649-40);
		break;

	case 21:	ifs.seekg(9320617-40);
		break;

	case 22:	ifs.seekg(9386249-40);
		break;

	case 23:	ifs.seekg(9408873-40);
		break;

	case 24:	ifs.seekg(9590889-40);
		break;

	case 25:	ifs.seekg(9617641-40);
		break;

	case 26:	ifs.seekg(9683273-40);
		break;

	case 27:	ifs.seekg(9743241-40);
		break;

	case 28:	ifs.seekg(9840937-40);
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

void CDat7::parseFrameMesh()
{
}

void CDat7::parseFrame()
{
}

void CDat7::parseBaseMesh()
{
}

void CDat7::parseMesh()
{
}