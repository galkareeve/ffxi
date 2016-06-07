#include "FFXIMesh.h"
#include <iostream>
#include <fstream>
#include "TDWAnalysis.h"
#include "FFXI_Character.h"
#include "myEnum.h"
#include "IMeshBuffer.h"
#include "OpenGLDriver.h"
//#include <Windows.h>

using namespace std;
using namespace glm;

char ffxidir[512]="E:\\Program Files\\PlayOnline\\SquareEnix\\FINAL FANTASY XI\\";

//void LocateFFXIDataFolder()
//{
//	HKEY hKey;
//	memset(ffxidir,0,512);
//
//	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\PlayOnlineUS\\InstallFolder",0,KEY_READ,&hKey))
//	{
//		DWORD dwData = sizeof(ffxidir);
//		DWORD dwType = REG_SZ;
//		RegQueryValueEx(hKey,"0001",NULL,&dwType,(LPBYTE)ffxidir,&dwData);
//		RegCloseKey( hKey );
//	}
//	if (lstrlen(ffxidir) == 0)
//	{
//		MessageBox(NULL,"FINAL FANTASY XI","Err",MB_OK);
//		GetCurrentDirectory(sizeof(ffxidir),ffxidir);
//	}
//	if (lstrlen(ffxidir) > 0)
//	{
//		if (ffxidir[lstrlen(ffxidir)-1] != '\\')
//		{
//			lstrcat(ffxidir,"\\");
//		}
//	}
//}

CFFXIMesh::CFFXIMesh(IDriver *in)
{
	p_driver = in;
	m_numFrameLoaded=0;
	m_totalAnimationFrame=0;
	m_numAnimation=0;
}


CFFXIMesh::~CFFXIMesh(void)
{
	delete p_mFFChar;
	for(auto it=m_meshBuffers.begin(); it!=m_meshBuffers.end(); ++it)
		delete *it;

	m_meshBuffers.clear();
}

bool CFFXIMesh::loadModelFile(std::string FN)
{
//	LocateFFXIDataFolder();
	int fno = atoi(FN.c_str());

	int dir=0,rem=fno;
	if(fno>=1000000) {
		dir=fno/1000000;
		rem -= dir*1000000;
	}
	char filename[256];
	if(dir==0) sprintf_s(filename,255,"%sROM\\%d\\%d.dat",ffxidir,rem/1000,rem%1000);  
	else       sprintf_s(filename,255,"%sROM%d\\%d\\%d.dat",ffxidir,dir,rem/1000,rem%1000);
	FN.assign(filename);

	ifstream ifs;
	ifs.open(FN.c_str(), ifstream::in | ifstream::binary);
	if(ifs.fail() ) {
		return false;
	}
	
	ifs.seekg (0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg (0, ifs.beg);

    // allocate memory:
    char * buffer = new char [length];

    // read data as a block:
    ifs.read (buffer,length);
    ifs.close();

	p_mFFChar = new CFFXI_Character;
	FFXIFile *f = new FFXIFile(buffer,length);
	m_meshBuffers.clear();

	//extract file
	string textureName;
	vector<STextureInfo> vectextureInfo;

	glm::u8 *ppImage=NULL;
	unsigned int width,height;
	unsigned int len;
	int MZBc=0,MMBc=0,IMGc=0,BONEc=0,ANIMc=0,VERTc=0;
	DATHEAD hd;
	for(int i=0; i<2; ++i)
	{
		char *p, *start=f->FistData(&hd);
		for( p=f->FistData(&hd); p; p=f->NextData(&hd) )
		{
			int type = (int)hd.type;
			len = hd.next*16;

			switch (type)
			{
				case 0x1c:  //MZB
					//if( i==1 ) {
						//decode_mzb((BYTE*)(p+16));
						//oj =  (OBJINFO *)(p+16+32);
						//noj = (*(int*)(p+16+4) )&0xffffff;
						//ofs << "offset: " << p-start << " MZB " << iname << " type: " << hd.type << " size: " << len << endl;
					//}
					MZBc++;
				break;
				case 0x2e:  //MMB
					//if( i==1 ) {
					//	decode_mmb((BYTE*)(p+16));		
					//	ofs << "offset: " << p-start << " MMB " << iname << " type: " << hd.type << " size: " << len << endl;
					//}
					MMBc++;
				break;
				case 0x20:  //IMG
					if( i==0 ) {
						STextureInfo tf;
						tf.name = p_mFFChar->extractImageName(p+sizeof(DATHEAD), width, height, ppImage);
						tf.id=p_driver->createTexture(width,height,1,ppImage);
						vectextureInfo.push_back(tf);
						delete ppImage;
						IMGc++;
					}
				break;
				case 0x29:	//Bone
					if( i==0 ) {
						p_mFFChar->SetBone(p+sizeof(DATHEAD),len-sizeof(DATHEAD));
						p_mFFChar->resetBoneMatrix();
						BONEc++;
					}
				break;
				case 0x2B:	//animation
					if( i==0 ) {
						p_mFFChar->AddAnimation(p+sizeof(DATHEAD), len-sizeof(DATHEAD));
						ANIMc++;
					}
				break;
				case 0x2a:	//vertex, create a meshBuffer for this part
					if( i==1 ) {
						p_mFFChar->AddVertex(p+sizeof(DATHEAD),len-sizeof(DATHEAD));
						VERTc++;
					}
				break;
			}
		}
	}

	char buf[255];
	sprintf_s(buf,255,"MZB:%d MMB:%d IMG:%d BONE:%d ANIM:%d VERT:%d", MZBc,MMBc,IMGc,BONEc,ANIMc,VERTc);
	std::cout << buf << std::endl;

	int maxParts = p_mFFChar->getPartsCount();
	std::vector<u16> vecIndices;
	std::vector<f32vec2> vecUV;

	int totalpn=0;
	for(int i=0; i<maxParts; ++i) {
		FFXIParts *pffxipart = p_mFFChar->getParts(i);		
		pffxipart->setMeshBufferStartIndex(totalpn);

		//there r many face for each part [TRIANGLE_LIST/TRIANGLE_STRIP]
		int maxFace = pffxipart->getFaceCount();
		for(int j=0; j<maxFace; ++j) {
			vecIndices.clear();
			vecUV.clear();
			
			//create a meshBuffer for each Face
			SFaceMB *pfmb = pffxipart->getFace(j);
			IMeshBuffer *mb = new IMeshBuffer(pfmb->type);
			//update its indices and uv, since it doesnt change
			for(auto it=pfmb->vecface.begin(); it!=pfmb->vecface.end(); ++it) {
				vecIndices.push_back((*it).indices);
				vecUV.push_back((*it).uv);
			}
			mb->updateIndicesBuffer(vecIndices);
			mb->updateUVBuffer(vecUV);
			//find the textureID
			for(auto it=vectextureInfo.begin(); it!=vectextureInfo.end(); ++it) {
				if( pfmb->m_textureName.compare((*it).name)==0) {
					mb->updateTextureID((*it).id);
					break;
				}
			}

			addMeshBuffer(mb);
			m_mappingMesh2Parts.push_back(i);
		}
		totalpn+=maxFace;
	}

	recalculateBoundingBox();
	vectextureInfo.clear();
	return true;
}

void CFFXIMesh::recalculateBoundingBox()
{

}

void CFFXIMesh::prepareFrameBuffer()
{
	m_totalAnimationFrame = p_mFFChar->getTotalAnimationFrame();
	for(int i=0; i!=m_meshBuffers.size(); ++i) {
		m_meshBuffers[i]->generateFrameBuffer(m_totalAnimationFrame);
	}
	m_numAnimation = p_mFFChar->getNumAnimation();

	//animate the first frame
	animate(0);
	m_numFrameLoaded=1;

	//float speed;
	//int frameStart, frameEnd;
	//for(int i=1; i<m_numAnimation; ++i) {
	//	//need to increment m_curAnimeDat2BIndex, in order to get the correct animation quaternion
	//	nextAnimation(speed, frameStart, frameEnd);
	//	for(int j=frameStart; j<=frameEnd; j++)
	//		animate(j);
	//}
}

//this is only called once to animate all the frameBuffer
bool CFFXIMesh::animate(int frame)
{
	if (m_meshBuffers.empty())
		return false;

	//check meshBuffer[0], since all parts are animated together for each frame
	if(m_meshBuffers[0]->isFrameBufferLoaded(frame))
		return true;

	if(m_meshBuffers[0]->isFrameBufferLock(frame))
		return false;
	//lock frameBuffer to prevent mulitply update from different sceneNode
	m_meshBuffers[0]->lockFrameBuffer(frame);

	//animate the bone
	p_mFFChar->animate(frame);
	cout << "animate frame " << frame << std::endl;

	// build current vertex
	for (int i = 0; i!= m_meshBuffers.size(); ++i)	{
		p_mFFChar->updateMeshBuffer(i, frame, m_mappingMesh2Parts[i], m_meshBuffers[i]);
//		if(i==0 && frame<5)
//			m_meshBuffers[i]->outputMeshInfo(frame);
	}
	m_meshBuffers[0]->unlockFrameBuffer(frame);
	m_numFrameLoaded++;
	recalculateBoundingBox();

	return true;
}

bool CFFXIMesh::isFrameFullyLoaded()
{
	return (m_numFrameLoaded==m_totalAnimationFrame);
}

IMeshBuffer* CFFXIMesh::getMeshBuffer(unsigned int i)
{
	return m_meshBuffers[i];
}

int CFFXIMesh::getAnimIndex(int frameStart, int frameEnd)
{
	return p_mFFChar->getAnimIndex(frameStart, frameEnd);
}

void CFFXIMesh::nextAnimation(float &speed, int &frameStart, int &frameEnd)
{
	p_mFFChar->nextAnimation(frameStart, frameEnd, speed);
}

void CFFXIMesh::setCurrentAnimation(int i, int &frameStart, int &frameEnd, float &speed)
{
	p_mFFChar->setCurrentAnimation(i, frameStart, frameEnd, speed);
}
