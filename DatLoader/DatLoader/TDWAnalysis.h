//------------------------------------------------------------------------------
//  File: TDWAnalysis.h
//  Desc: ffxiµÄ½âÎöµ¥Ôª     
//  Revision history:
//      * 2007/08/10 Created by Muzisoft05@163.com.
//
//  Todo:
//
//------------------------------------------------------------------------------

#pragma once
//#include <d3d9.h>
//#include <d3dx9math.h>

#include <vector>
#include <string>

#pragma pack(push,1)
//16 byte
struct SMMBHEAD {
	char id[3];
	long   type:7;
	long   next:19;
	long   is_shadow:1;
	long   is_extracted:1;
	long   ver_num:3;
	long   is_virtual:1;
	char unk[9];
};
//16 byte
//struct SMMBHEAD2 {
//	unsigned int MMBSize : 24;
//	unsigned int d1 : 8;
//	unsigned short d3 : 8;
//	unsigned short d4 : 8;
//	char unk2[2];
//	char name[8];
//};
struct SMMBHEAD2 {
	unsigned int MMBSize:24;
	unsigned int d1 : 8;
	unsigned short d3 : 8;
	unsigned short d4 : 8;
	unsigned short d5 : 8;
	unsigned short d6 : 8;
	char name[8];
};

typedef struct {
  int syuzoku;		//race
  int facetype;
  int head;
  int body;
  int waist;
  int hands;
  int legs;
  int buki;			//weapon 武器
} PC;

typedef struct {
  std::string syuzoku;
  std::string facetype;
  std::string head;
  std::string body;
  std::string waist;
  std::string hands;
  std::string legs;
  std::string buki;
} PCFNO;

typedef struct 
{
  float x1,x2,y1,y2,z1,z2;
} OOO;

typedef struct 
{
//  DWORD  id;
  char name[4];
  long   type:7;
  long   next:19;
  long   is_shadow:1;
  long   is_extracted:1;
  long   ver_num:3;
  long   is_virtual:1;
  DWORD	*parent;
  DWORD *nextblock;
	//ResourceHeader * parent;
	//ResourceHeader * next;
} DATHEAD;

typedef struct
{
  char  flg;
  char id[16];
  DWORD dwnazo1;			//nazo = unknown
  long  imgx, imgy;
  DWORD dwnazo2[6];
  DWORD widthbyte;
  DWORD palet[0x100];
} IMGINFO;

//57 byte
typedef struct
{
  BYTE  flg;
  char id[16];
  DWORD dwnazo1;			//nazo = unknown
  long  imgx, imgy;
  DWORD dwnazo2[6];
  DWORD widthbyte;
} IMGINFO05;

//61 byte
typedef struct
{
  BYTE  flg;
  char id[16];
  DWORD dwnazo1;			//nazo = unknown
  long  imgx, imgy;
  DWORD dwnazo2[6];
  DWORD widthbyte;
  DWORD unk;				//B1-extra unk, 01-no unk
  DWORD palet[0x100];
} IMGINFOB1;
//69 byte
typedef struct
{
  BYTE  flg;
  char id[16];
  DWORD dwnazo1;
  long  imgx, imgy;
  DWORD dwnazo2[6];
  unsigned int widthbyte;
  char ddsType[4];
  unsigned int size;
  unsigned int noBlock;
} IMGINFOA1;

typedef struct
{
	char ddsType[4];
	unsigned int size;
	unsigned int noBlock;
} IMGINFO81_DDS;

//typedef struct
//{
//  char  flg;
//  char id[16];
//  DWORD dwnazo1;
//  long  imgx, imgy;
//  DWORD dwnazo2[6];
//  DWORD widthbyte;
//  char ddsType[4];
//  unsigned int size;
//  unsigned int noBlock;
//} IMGINFO1;

//typedef struct
//{
//  DWORD idno;
//  WORD itemtype;
//  WORD nazo11[0x02];		//謎
//  DWORD no;
//  WORD lvl;
//  WORD basyo;				//場所
//  WORD syuzoku;				//種族
//  WORD jobs;
//  WORD dmg;
//  WORD kaku;				//各 (1-hand/2-hand)
//  WORD nazo13;
//  WORD type;
//  char namej[22];
//  char namee[22];
//  char info[96];
//  char infoe[96];
//  char nazo2[512-0x1e-22-22-96-96];
//
////0x200
//  DWORD size;
//  IMGINFO ii;
//  BYTE img[0xa00-4-sizeof(IMGINFO)];
//} BUKIINFO;
//
//typedef struct
//{
//  DWORD idno;
//  WORD itemtype;
//  WORD nazo11[0x02];
//  DWORD no;
//  WORD lvl;
//  WORD basyo;
//  WORD syuzoku;
//  WORD jobs;
//  WORD dmg;
//  WORD kaku;
//  WORD nazo13;
//  WORD type;
//  char namet[22];
//  char name[34];
//  char name2[64];
//  char name2s[64];
//  char info[96];
//  char nazo2[512-0x1e-22-34-64-64-96];
//
////0x200
//  DWORD size;
//  IMGINFO ii;
//  BYTE img[0xa00-4-sizeof(IMGINFO)];
//} BUKIINFOE;

typedef struct
{
  DWORD idno;
  WORD itemtype;
  WORD nazo11[2];
  DWORD no;
  WORD lvl;
  WORD basyo;
  WORD syuzoku;
  WORD jobs;
  WORD dex;
  char namej[22];
  char namee[22];
  char info[96];
  char infoe[96];
  char nazo2[0x200-0x18-22-22-96-96];

//0x200
  DWORD size;
  IMGINFO ii;
  BYTE img[0xa00-4-sizeof(IMGINFO)];
} BOGUINFO;

typedef struct
{
  DWORD idno;
  WORD itemtype;
  WORD nazo11[2];
  DWORD no;
  WORD lvl;
  WORD basyo;
  WORD syuzoku;
  WORD jobs;
  WORD dex;

  char namet[22];
  char name[34];
  char name2[64];
  char name2s[64];
  char info[96];
  char nazo2[0x200-0x18-22-34-64-64-96];

//0x200
  DWORD size;
  IMGINFO ii;
  BYTE img[0xa00-4-sizeof(IMGINFO)];
} BOGUINFOE;

typedef struct
{
  DWORD idno;
  WORD itemtype;
  WORD  nazo1[4];
  char namej[22];
  char namee[22];
  char info[96];
  char infoe[96];
  char nazo2[0x200-14-22-22-96-96];

//0x200
  DWORD size;
  IMGINFO ii;
  BYTE img[0xa00-4-sizeof(IMGINFO)];
} ITEMINFO;

typedef struct
{
  DWORD idno;
  WORD itemtype;
  WORD  nazo1[4];
  char namet[22];
  char name[34];
  char name2[64];
  char name2s[64];
  char info[96];
  char nazo2[0x200-14-22-34-64-64-96];

//0x200
  DWORD size;
  IMGINFO ii;
  BYTE img[0xa00-4-sizeof(IMGINFO)];
} ITEMINFOE;

typedef struct
{
  DWORD idno;
  WORD itemtype;
  WORD nazo1[4];
  char namej[22];
  char namee[22];
  char info[96];
  char infoe[96];
  char nazo2[0x200-14-22-22-96-96];

//0x200
  DWORD size;
  IMGINFO ii;
  BYTE img[0xa00-4-sizeof(IMGINFO)];
} CHOUDOINFO;

typedef struct
{
  DWORD idno;
  WORD itemtype;
  WORD nazo1[4];
  char namet[22];
  char name[34];
  char name2[64];
  char name2s[64];
  char info[96];
  char nazo2[0x200-14-22-34-64-64-96];

//0x200
  DWORD size;
  IMGINFO ii;
  BYTE img[0xa00-4-sizeof(IMGINFO)];
} CHOUDOINFOE;

typedef struct
{
  DWORD idno;
  WORD nazo1[5];
  char namej[22];
  char namee[22];
  char infoj[96];
  char infoe[96];
  char nazo2[0x200-14-22-22-96-96];

//0x200
  DWORD size;
  IMGINFO ii;
  BYTE img[0xa00-4-sizeof(IMGINFO)];
} MATEINFO;

typedef struct
{
  DWORD idno;
  WORD nazo1[5];
  char namet[22];
  char name[34];
  char name2[64];
  char name2s[64];
  char info[96];
  char nazo2[0x200-14-22-34-64-64-96];

//0x200
  DWORD size;
  IMGINFO ii;
  BYTE img[0xa00-4-sizeof(IMGINFO)];
} MATEINFOE;

typedef struct
{
  WORD nazo1[5];
  char namej[32];
  char info[96];
  char nazo2[0x400-10-32-96];
} ABIINFO;

typedef struct
{
  WORD nazo1[5];
  char name[32];
  char info[96];
  char nazo2[0x400-10-32-96];
} ABIINFOE;

typedef struct
{
  short no;
  short syu;
  short zoku;
  short flg1;
  short flg2;
  short mp;
  BYTE reeisyou;
  BYTE eisyou;
  BYTE joblvl[16];
  char nazo11;
  char namej[20];
  char namee[20];
  char infoj[128];
  char infoe[128];
  char nazo2[0x400-0x1f-20-20-128-128];
} MAGINFO;

typedef struct
{
  short no;
  short syu;
  short zoku;
  short flg1;
  short flg2;
  short mp;
  BYTE reeisyou;
  BYTE eisyou;
  BYTE joblvl[16];
  char nazo11;
  char namej[20];
  char namee[20];
  char infoj[128];
  char infoe[128];
  char nazo2[0x400-0x1f-20-20-128-128];
} MAGINFOE;

typedef struct
{
  char nazo1[2];
  char namej[32];
  char namee[32];
  char infoj[128];
  char infoe[128];
  char nazo2[512-2-32-32-128-128];

//0x200
  DWORD size;
  IMGINFO ii;
  BYTE img[0xa00-4-sizeof(IMGINFO)];
} STATINFO;

typedef struct
{
  char nazo1[2];
  char namej[32];
  char namee[32];
  char infoj[128];
  char infoe[128];
  char nazo2[512-2-32-32-128-128];

//0x200
  DWORD size;
  IMGINFO ii;
  BYTE img[0xa00-4-sizeof(IMGINFO)];
} STATINFOE;

typedef struct
{
  char id[16];
  float fTransX,fTransY,fTransZ;
  float fRotX,fRotY,fRotZ;
  float fScaleX,fScaleY,fScaleZ;
  float fa,fb,fc,fd;
  long  fe,ff,fg,fh,fi,fj,fk,fl;
} OBJINFO;

typedef  struct
{
  //short d[12];
  short lt_x, lt_y;
  short rt_x, rt_y;
  short lb_x, bt_y;
  short rb_x, rb_y;
  short srcsx,srcsy; 
  short srcpx,srcpy; 
  char invxy;
  DWORD nano2[4];
  char nano3[4];
  char imgid[16];
} MENU31;

#pragma pack(push,2)

typedef struct
{
  int   no;
  int   idx_qtx,idx_qty,idx_qtz,idx_qtw;
  float qtx,qty,qtz,qtw;
  int   idx_tx,idx_ty,idx_tz;
  float tx,ty,tz;
  int   idx_sx,idx_sy,idx_sz;
  float   sx,sy,sz;
} DAT2B;


typedef struct{
  BYTE  ver;     //0x00
  BYTE  nazo;    //0x01
  WORD  type;    //0x02 &7f==0ƒ‚ƒfƒ‹ 1=ƒNƒƒX
  WORD  flip;    //0x04 0==OFF  ON

  DWORD offsetPoly;       //0x06
  WORD  PolySuu;        //0x0A  //PolySuu
  DWORD offsetBoneTbl;      //0x0C
  WORD  BoneTblSuu;        //0x10
  DWORD offsetWeight;   //0x12
  WORD  WeightSuu;      //0x16
  DWORD offsetBone;   //0x18
  WORD  BoneSuu;        //0x1C
  DWORD offsetVertex;    //0x1E  
  WORD  VertexSuu;   //0x22
  DWORD offsetPolyLoad;  //0x24
  WORD  PolyLoadSuu;       //0x28
  WORD  PolyLodVtx0Suu;  //0x2A
  WORD  PolyLodVtx1Suu;  //0x2C
  DWORD offsetPolyLod2;     //0x2E
  WORD   PolyLod2Suu;       //0x32
//---------------------------------------
  DWORD  nazo1;       //0x34
  DWORD  nazo2;       //0x38
  WORD   nazo3;       //0x3C
  WORD   nazo4;       //0x3E
} DAT2AHeader;

#pragma warning(disable:4200)
typedef struct{
  BYTE  ver;     //0x00
  BYTE  nazo;    //0x01
  WORD  element;
  WORD  frame;
  float speed;
  union{
    float f[];   //waringo‚é‚¯‚Ç‹–‚µ‚Ä‚Ë
    DAT2B dat[]; //waringo‚é‚¯‚Ç‹–‚µ‚Ä‚Ë
  };
} DAT2BHeader;

typedef struct{
  BYTE  ver;     //0x00
  BYTE  nazo;    //0x01
  WORD  element;
  WORD  frame;
  float speed;
} DAT2BHeader2;

#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
  short i1,i2,i3;
  float u1,v1;
  float u2,v2;
  float u3,v3;
} TEXLIST;
typedef struct
{
  short i;
  float u,v;
} TEXLIST2;
typedef struct 
{
  BYTE parent;			//parent node id
  BYTE term;			//is leaf node (1=terminate)
  float i,j,k,w;		//quaternionRotation
  float x,y,z;			//translation
} BONE;
typedef struct 
{
  WORD no;             //ƒ{[ƒ“”Ô†‚ðŽw‚µ‚Ä‚éƒb‚Û‚¢ Žw’è‚Ìƒ{[ƒ“‚ÉˆË‘¶‚µ‚Ä‚¢‚é‚Á‚ÄŒ¾‚¤‚±‚ÆH
  float a,b,c,d,e,f;
} BONE2;

typedef struct
{
	union {
		WORD low;
		struct {
			WORD leftL:7;
			WORD rightL:7;
			WORD flgL:2;
		};
	};
	union {
		WORD high;
		struct {
			WORD leftH:7;
			WORD rightH:7;
			WORD flgH:2;
		};
	};
} BONE3;

struct BONE4 {
	WORD low;
	WORD lflg;
	WORD high;
	WORD hflg;
};

typedef struct 
{
  DWORD col;
  float dat1[7];
  DWORD ddd1;
  DWORD ddd2;
  float dat2;
} dat8000;
typedef struct _D3DTEXVERTEX
{
  float x,y,z;     //À•W
  float hx,hy,hz;  //–@üƒxƒNƒgƒ‹
  DWORD color;     //F
  float tu,tv;     // UVÀ•W
} D3DTEXVERTEX;

#define D3DFVF_TEXVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_NORMAL|D3DFVF_TEX1) 

inline FLOAT DEGtoRAD(float angle)
{
  return (angle-90.0f)*3.1415926f/180.0f;
}

typedef struct
{
  float x,y,z;     //À•W
  float hx,hy,hz;  //–@üƒxƒNƒgƒ‹
} MODELVERTEX1;

typedef struct  //‚±‚ê‚ÅŠm’è
{
  float x1,x2,y1,y2,z1,z2;
  float w1,w2;
  float hx1,hx2;  //–@üƒxƒNƒgƒ‹
  float hy1,hy2;  //–@üƒxƒNƒgƒ‹
  float hz1,hz2;  //–@üƒxƒNƒgƒ‹
} MODELVERTEX2;

typedef struct
{
  float x,y,z;     //À•W
} CLOTHVERTEX1;

typedef struct
{
  float x1,x2;
  float y1,y2;
  float z1,z2;
  float w1,w2;
} CLOTHVERTEX2;

typedef struct
{
	BYTE i1;
	BYTE flg1;
	BYTE i2;
	BYTE flg2;
	float f;
} myExtra;

#pragma pack(pop)

typedef struct 
{
  char ID[17];
  //LPDIRECT3DTEXTURE9 pTex; 
  void *pTex;
} TEXTEX;

#pragma pack(pop)

typedef struct
{
  int weight1;
  int weight2;
  WORD *pBone;
  WORD *pBoneTbl;
  MODELVERTEX1 *pVertex;
  MODELVERTEX2 *pVertex2;
  CLOTHVERTEX1 *pVertexC;
  CLOTHVERTEX2 *pVertexC2;
  char *pPoly;
  char *dat2A;
  DAT2AHeader *dat2ahead;
} DAT2A;
//32 byte
struct SMZBHeader {
	char id[4];
//	unsigned int id;
	unsigned int totalRecord100:24;
	unsigned int R100Flag:8;
	unsigned int offsetHeader2;
	unsigned int d1:8;
	unsigned int d2:8;
	unsigned int d3:8;
	unsigned int d4:8;
	int offsetCubetree;
	unsigned int offsetEndRecord100;	
	unsigned int offsetEndCubetree;
	int unk5;
};

struct SMZBHeader2 {
	unsigned int totalRecord92:24;
	unsigned int R92Flag:8;
	unsigned int offsetBlock92;
	unsigned int totalBlock16;
	unsigned int offsetBlock16;
	unsigned int offsetVertex;
	unsigned int offsetBlock112;
	unsigned int totalRecord112;
	int unk1;
};

//shorter version of OBJINFO
struct SMZBBlock84 {
	char id[16];
	float fTransX,fTransY,fTransZ;
	float fRotX,fRotY,fRotZ;
	float fScaleX,fScaleY,fScaleZ;
	float fa,fb,fc,fd;				//0, 10, 100, 1000
	unsigned int i1, i2, i3, i4;
};
//observed in dat 116
struct SMZBBlock92b {
	char id[16];
	float fTransX,fTransY,fTransZ;
	float fRotX,fRotY,fRotZ;
	float fScaleX,fScaleY,fScaleZ;
	float fa,fb,fc,fd;				//0, 10, 100, 1000
	unsigned int i1, i2, i3, i4, i5, i6;
};

//same as OBJINFO
//instance of MMB model
struct SMZBBlock100 {
	char id[16];
	float fTransX,fTransY,fTransZ;
	float fRotX,fRotY,fRotZ;
	float fScaleX,fScaleY,fScaleZ;
	float fa,fb,fc,fd;				//0, 10, 100, 1000
	long fe,ff,fg;
	long fh, fi, fj, fk, fl;
};

//special
struct SMZBBlock76 {
	char datno[4];
	char unk[72];
};

//cubetree???
struct SMZBBlock128 {
	float x1,y1,z1;		//min/max boundingRect
	float x2,y2,z2;
	float x3,y3,z3;
	float x4,y4,z4;
	float x5,y5,z5;
	float x6,y6,z6;
	float x7,y7,z7;
	float x8,y8,z8;
	unsigned int offsetMZB;
	unsigned int numMZB;
	unsigned int offset3;	//front
	unsigned int offset4;	//back
	unsigned int offset5;	//left
	unsigned int offset6;	//right
	unsigned int offset7;	//top
	unsigned int offset8;	//bottom
};

//variable length, no of vertex/normal depend on range.
struct SMZBBlock92 {
	unsigned int range1;	//start of Vertex offset
	unsigned int range2;	//end of vertex, start of normal offset  ==> (range2 - range1 == no. of vertex)
	unsigned int range3;	//end of normal offset  ==> (range3 - range2 = no. of normal)
	char unk1[4];			//2, 1,
	float f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15;	//12 float (x,y,z) + 3 float (hx,hy,hz)
	char footer[16];		//0, 1, 2, 0, 1, 3, 2, 0
};
//f1-f16 = transform matrix(4X4) for vertex, f17-f28 transform matrix(4X3) for normal
struct SMZBBlock112 {
	float f1,f2,f3,f4;			//1, 0, 0, 0
	float f5,f6,f7,f8;			//0, 1, 0, 0
	float f9,f10,f11,f12;		//0, 0, 1, 0
	float f13,f14,f15,f16;		//-180, 0, -180, 1
	float f17,f18,f19;			//1, 0, 0
	float f20,f21,f22;			//0, 1, 0
	float f23,f24,f25;			//0, 0, 1
	float f26,f27,f28;			//0, 0, 0
};
//variable length, need to read until 0
struct SMZBBlock16 {
	unsigned int i1;	//unknown
	unsigned int i2;	//offset112
	unsigned int i3;	//offset92
	unsigned int i4;	//0
};

//s1, s2, s3 == vertex index, s4=normal index (index to SMZBBlock92)
struct SMZBBlock8 {
	unsigned short s1;
	unsigned short s2;
	unsigned short s3;
	unsigned short s4;
};

struct SMZBVector {
	float x;
	float y;
	float z;
};

struct SMZBFace {
	unsigned int i1;
	unsigned int i2;
	unsigned int i3;
};

//48Byte
struct SMMBHeader {
	char imgID[16];
	int pieces;			//No of BlockHeader
	float x1,x2;		//BoundingRec Combine all BlockHeader (min,max)
	float y1,y2;
	float z1,z2;
	unsigned int offsetBlockHeader;	//offset to first SMMBBlockHeader
};

//struct SMMBRange {
//	unsigned int r1;
//	unsigned int numModel;	//repeat SMMBBlockHeader
//	float x1,x2;
//	float y1,y2;
//	float z1,z2;
//	unsigned int unk1;
//};

struct SMMBBlockHeader {
	int numModel;		//no of model block
	float x1,x2;		//BoundingRec Single (min,max)
	float y1,y2;
	float z1,z2;
	int numFace;		//1 face have 3 indices
};

struct SMMBModelHeader {
	char textureName[16];
	unsigned short vertexsize;			//No of SMMBBlockVertex
	unsigned short blending;			//useAlpha & multipler
};

struct SMMBBlockVertex {
	float x,y,z;
	float hx,hy,hz;
	unsigned int R:8;
	unsigned int G:8;
	unsigned int B:8;
	unsigned int A:8;
	float u, v;
};

struct SMMBBlockVertex2 {
	float x,y,z;
	float dx,dy,dz;		//displacement?? cloth
	float hx,hy,hz;
	unsigned int R:8;
	unsigned int G:8;
	unsigned int B:8;
	unsigned int A:8;
	float u, v;
};


extern COLORREF colBk;
extern char ffxidir[512];

BOOL GetFileNameFromFileID(LPSTR filename,DWORD dwID);
void GetRorStr(LPSTR a,LPSTR b,int c) ;
BOOL GetBMPImage(IMGINFO *ii, BYTE *ppImage );
void LocateFFXIDataFolder(void);
BOOL IsImg(LPSTR p);
BOOL IsImage(IMGINFO *ii);

int BitCount(BYTE x);
bool decode_mmb(BYTE *p, unsigned int maxLen);
bool decode_mzb(BYTE *p, unsigned int maxLen);

class FFXIFile
{
	LPSTR pData;
	char filename[256];
	BOOL Load(void);

public:
	char *pdat;
	DWORD dwSize;
	int vnum;
	int fnum;

	FFXIFile() 
	{ 
		pdat = NULL; 
		pData = NULL; 
		dwSize = 0; 
	}
   ~FFXIFile() 
   { 
	   Free(); 
   }

	DWORD GetSize(){return dwSize;}
	BOOL IsNull(){
		return (!pdat || dwSize==0);
	}
	BOOL IsFFXIFormat()
	{   
		if( IsNull() ) return FALSE;
		DATHEAD *phd = (DATHEAD *)pdat;
		if( phd->next<=0 || dwSize<(DWORD)phd->next*16 ) return FALSE;
		//if( phd->type!=0x01) return FALSE;
		return TRUE;
	}
	LPSTR FistData(DATHEAD *phd)
	{
		if( !pdat ) return NULL;
		*phd = *(DATHEAD *)pdat;
		pData = pdat;
		return pData;
	}
	LPSTR NextData(DATHEAD *phd);
	//{
	//	if(!pData) return NULL;
	//	*phd = *(DATHEAD *)pData;
	//	int next = phd->next;
	//	if( next<=0 ) return NULL;
	//	if( pdat+dwSize<=pData+next*16 ) return NULL;
	//	pData += next*16;
	//	*phd = *(DATHEAD *)pData;
	//	return pData;
	//}
	BOOL Free(void)
	{
		dwSize = 0;
		if( pdat )
		{
			delete []pdat;
			pdat = NULL;
		}
		pData=NULL;
		return TRUE;
	}
	LPSTR GetShortFilename()
	{
		return &filename[lstrlen(ffxidir)];
	}
	BOOL Load(DWORD fno);
	BOOL LoadF(int flg, int no);
};

