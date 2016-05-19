//------------------------------------------------------------------------------
//  File: TDWAnalysis.h
//  Desc: ffxiｵﾄｽ簧･ﾔｪ     
//  Revision history:
//      * 2007/08/10 Created by Muzisoft05@163.com.
//
//  Todo:
//
//------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <string>

#pragma pack(push,1)

typedef struct {
  int syuzoku;
  int facetype;
  int head;
  int body;
  int waist;
  int hands;
  int legs;
  int buki;
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
  unsigned int  id;
  long   type:7;
  long   next:19;
  long   is_shadow:1;
  long   is_extracted:1;
  long   ver_num:3;
  long   is_virtual:1;
  unsigned int 	*parent;
  unsigned int 	*child;
	//ResourceHeader * parent;
	//ResourceHeader * next;
} DATHEAD;

typedef struct
{
  char  flg;
  char id[16];
  unsigned int  dwnazo1;
  long  imgx, imgy;
  unsigned int  dwnazo2[6];
  unsigned int  widthbyte;
  unsigned int  palet[0x100];
} IMGINFO;

typedef struct
{
  char  flg;
  char id[16];
  unsigned int  dwnazo1;
  long  imgx, imgy;
  unsigned int  dwnazo2[6];
  unsigned int  widthbyte;
  char ddsType[4];
  unsigned int  size;
  unsigned int  noBlock;
} IMGINFO1;

typedef struct
{
  unsigned int idno;
  unsigned short itemtype;
  unsigned short nazo11[0x02];
  unsigned int no;
  unsigned short lvl;
  unsigned short basyo;
  unsigned short syuzoku;
  unsigned short jobs;
  unsigned short dmg;
  unsigned short kaku;
  unsigned short nazo13;
  unsigned short type;
  char namej[22];
  char namee[22];
  char info[96];
  char infoe[96];
  char nazo2[512-0x1e-22-22-96-96];

//0x200
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
} BUKIINFO;

typedef struct
{
  unsigned int idno;
  unsigned short itemtype;
  unsigned short nazo11[0x02];
  unsigned int no;
  unsigned short lvl;
  unsigned short basyo;
  unsigned short syuzoku;
  unsigned short jobs;
  unsigned short dmg;
  unsigned short kaku;
  unsigned short nazo13;
  unsigned short type;
  char namet[22];
  char name[34];
  char name2[64];
  char name2s[64];
  char info[96];
  char nazo2[512-0x1e-22-34-64-64-96];

//0x200
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
} BUKIINFOE;

typedef struct
{
  unsigned int idno;
  unsigned short itemtype;
  unsigned short nazo11[2];
  unsigned int no;
  unsigned short lvl;
  unsigned short basyo;
  unsigned short syuzoku;
  unsigned short jobs;
  unsigned short dex;
  char namej[22];
  char namee[22];
  char info[96];
  char infoe[96];
  char nazo2[0x200-0x18-22-22-96-96];

//0x200
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
} BOGUINFO;

typedef struct
{
  unsigned int idno;
  unsigned short itemtype;
  unsigned short nazo11[2];
  unsigned int no;
  unsigned short lvl;
  unsigned short basyo;
  unsigned short syuzoku;
  unsigned short jobs;
  unsigned short dex;

  char namet[22];
  char name[34];
  char name2[64];
  char name2s[64];
  char info[96];
  char nazo2[0x200-0x18-22-34-64-64-96];

//0x200
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
} BOGUINFOE;

typedef struct
{
  unsigned int idno;
  unsigned short itemtype;
  unsigned short  nazo1[4];
  char namej[22];
  char namee[22];
  char info[96];
  char infoe[96];
  char nazo2[0x200-14-22-22-96-96];

//0x200
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
} ITEMINFO;

typedef struct
{
  unsigned int idno;
  unsigned short itemtype;
  unsigned short  nazo1[4];
  char namet[22];
  char name[34];
  char name2[64];
  char name2s[64];
  char info[96];
  char nazo2[0x200-14-22-34-64-64-96];

//0x200
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
} ITEMINFOE;

typedef struct
{
  unsigned int idno;
  unsigned short itemtype;
  unsigned short nazo1[4];
  char namej[22];
  char namee[22];
  char info[96];
  char infoe[96];
  char nazo2[0x200-14-22-22-96-96];

//0x200
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
} CHOUDOINFO;

typedef struct
{
  unsigned int idno;
  unsigned short itemtype;
  unsigned short nazo1[4];
  char namet[22];
  char name[34];
  char name2[64];
  char name2s[64];
  char info[96];
  char nazo2[0x200-14-22-34-64-64-96];

//0x200
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
} CHOUDOINFOE;

typedef struct
{
  unsigned int idno;
  unsigned short nazo1[5];
  char namej[22];
  char namee[22];
  char infoj[96];
  char infoe[96];
  char nazo2[0x200-14-22-22-96-96];

//0x200
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
} MATEINFO;

typedef struct
{
  unsigned int idno;
  unsigned short nazo1[5];
  char namet[22];
  char name[34];
  char name2[64];
  char name2s[64];
  char info[96];
  char nazo2[0x200-14-22-34-64-64-96];

//0x200
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
} MATEINFOE;

typedef struct
{
  unsigned short nazo1[5];
  char namej[32];
  char info[96];
  char nazo2[0x400-10-32-96];
} ABIINFO;

typedef struct
{
  unsigned short nazo1[5];
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
  char reeisyou;
  char eisyou;
  char joblvl[16];
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
  char reeisyou;
  char eisyou;
  char joblvl[16];
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
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
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
  unsigned int size;
  IMGINFO ii;
  char img[0xa00-4-sizeof(IMGINFO)];
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
  unsigned int nano2[4];
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
  float sx,sy,sz;
} DAT2B;


typedef struct{
  char  ver;     //0x00
  char  nazo;    //0x01
  unsigned short  type;    //0x02 &7f==0モデル 1=クロス
  unsigned short  flip;    //0x04 0==OFF  ON

  unsigned int offsetPoly;       //0x06
  unsigned short  PolySuu;        //0x0A  //PolySuu
  unsigned int offsetBoneTbl;      //0x0C
  unsigned short  BoneTblSuu;        //0x10
  unsigned int offsetWeight;   //0x12
  unsigned short  WeightSuu;      //0x16
  unsigned int offsetBone;   //0x18
  unsigned short  BoneSuu;        //0x1C
  unsigned int offsetVertex;    //0x1E  
  unsigned short  VertexSuu;   //0x22
  unsigned int offsetPolyLoad;  //0x24
  unsigned short  PolyLoadSuu;       //0x28
  unsigned short  PolyLodVtx0Suu;  //0x2A
  unsigned short  PolyLodVtx1Suu;  //0x2C
  unsigned int offsetPolyLod2;     //0x2E
  unsigned short   PolyLod2Suu;       //0x32
//---------------------------------------
  unsigned int  nazo1;       //0x34
  unsigned int  nazo2;       //0x38
  unsigned short   nazo3;       //0x3C
  unsigned short   nazo4;       //0x3E
} DAT2AHeader;

#pragma warning(disable:4200)
typedef struct{
  char  ver;     //0x00
  char  nazo;    //0x01
  unsigned short  element;
  unsigned short  frame;
  float speed;
  union{
    float f[];   //waring出るけど許してね
    DAT2B dat[]; //waring出るけど許してね
  };
} DAT2BHeader;

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
  char parent;
  char term;
  float i,j,k,w;
  float x,y,z;
} BONE;
typedef struct 
{
  unsigned short no;             //ボーン番号を指してるッぽい 指定のボーンに依存しているって言うこと？
  float a,b,c,d,e,f;
} BONE2;

struct bitType {
	unsigned short left:7;
	unsigned short right:7;
	unsigned short flg:2;
};

struct BONE3 {
	union {
		unsigned short low;
		bitType bitLow;
	};
	union {
		unsigned short high;
		bitType bitHigh;
	};
};

struct BONE4 {
	unsigned short low;
	unsigned short lflg;
	unsigned short high;
	unsigned short hflg;
};

typedef struct 
{
  unsigned int col;
  float dat1[7];
  unsigned int ddd1;
  unsigned int ddd2;
  float dat2;
} dat8000;

typedef struct _D3DTEXVERTEX
{
  float x,y,z;     //座標
  float hx,hy,hz;  //法線ベクトル
  unsigned int color;     //色
  float tu,tv;     // UV座標
} D3DTEXVERTEX;

//#define D3DFVF_TEXVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_NORMAL|D3DFVF_TEX1) 

inline float DEGtoRAD(float angle)
{
  return (angle-90.0f)*3.1415926f/180.0f;
}

typedef struct
{
  float x,y,z;     //座標
  float hx,hy,hz;  //法線ベクトル
} MODELVERTEX1;

typedef struct  //これで確定
{
  float x1,x2,y1,y2,z1,z2;
  float w1,w2;
  float hx1,hx2;  //法線ベクトル
  float hy1,hy2;  //法線ベクトル
  float hz1,hz2;  //法線ベクトル
} MODELVERTEX2;

typedef struct 
{
  float x1,x2,y1,y2,z1,z2;
  float w1,w2;
  float hx1,hx2; 
  float hy1,hy2; 
  float hz1,hz2;
  float u, v;
} MODELVERTEX3;

typedef struct
{
  float x,y,z;     //座標
} CLOTHVERTEX1;

typedef struct
{
  float x1,x2;
  float y1,y2;
  float z1,z2;
  float w1,w2;
} CLOTHVERTEX2;

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
  unsigned short *pBone;
  unsigned short *pBoneTbl;
  MODELVERTEX1 *pVertex;
  MODELVERTEX2 *pVertex2;
  CLOTHVERTEX1 *pVertexC;
  CLOTHVERTEX2 *pVertexC2;
  char *pPoly;
  char *dat2A;
  DAT2AHeader *dat2ahead;
} DAT2A;

class FFXIFile
{
	char *pData;
	char filename[256];
	bool Load(void);

public:
	char *pdat;
	unsigned int dwSize;
	int vnum;
	int fnum;

	FFXIFile(char *p, unsigned int len) 
	{ 
		pdat = p; 
		pData = NULL; 
		dwSize = len; 
	}
   ~FFXIFile() 
   { 
	   Free(); 
   }

	unsigned int GetSize(){return dwSize;}
	bool IsNull(){
		return (!pdat || dwSize==0);
	}
	bool IsFFXIFormat()
	{   
		if( IsNull() ) return false;
		DATHEAD *phd = (DATHEAD *)pdat;
		if( phd->next<=0 || dwSize<(unsigned int)phd->next*16 ) return false;
		//if( phd->type!=0x01) return FALSE;
		return true;
	}
	char* FistData(DATHEAD *phd)
	{
		if( !pdat ) return NULL;
		phd = (DATHEAD *)pdat;
		pData = pdat;
		return pData;
	}
	char* NextData(DATHEAD *phd)
	{
		if(!pData) return NULL;
		*phd = *(DATHEAD *)pData;
		unsigned int next = phd->next;
		if( next<=0 ) return NULL;
		next = (next & 0x7ffff) * 16;
		if( pdat+dwSize<=pData+next ) return NULL;
		pData += next;
		*phd = *(DATHEAD *)pData;
		return pData;
	}
	bool Free(void)
	{
		dwSize = 0;
		if( pdat )
		{
			delete []pdat;
			pdat = NULL;
		}
		pData=NULL;
		return true;
	}
	//LPSTR GetShortFilename()
	//{
	//	return &filename[lstrlen(ffxidir)];
	//}
//	BOOL Load(unsigned int fno);
//	BOOL LoadF(int flg, int no);
};

