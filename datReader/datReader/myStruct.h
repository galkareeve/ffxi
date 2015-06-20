#pragma once
#pragma pack(push,1)

typedef struct 
{
  DWORD  id;
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

//57 Byte
typedef struct
{
  BYTE  flg;
  char id[16];
  DWORD dwnazo1;			//nazo = unknown
  long  imgx, imgy;
  DWORD dwnazo2[6];
  DWORD widthbyte;
  DWORD palet[0x100];
} IMGINFO;
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

struct block {
	unsigned int frame;
	unsigned int rot1;
	unsigned int rot2;
	unsigned int rot3;
	unsigned int rot4;
	float rotf1;
	float rotf2;
	float rotf3;
	float rotf4;
	unsigned int tra1;
	unsigned int tra2;
	unsigned int tra3;
	float traf1;
	float traf2;
	float traf3;
	unsigned int sca1;
	unsigned int sca2;
	unsigned int sca3;
	float scaf1;
	float scaf2;
	float scaf3;
};


struct myVector {
	float x;
	float y;
	float z;
};

struct myIndices {
	unsigned int x;
	unsigned int y;
	unsigned int z;
};

struct myUV {
	float u;
	float v;
};

struct myFace {
	short v1;
	short v2;
	short v3;
};

struct uvFace {
	myFace face;
	myUV uv1;
	myUV uv2;
	myUV uv3;
};

struct vertNormal {
	myVector vertices;
	myVector normal;
};

struct vertNormalExtra {
	myVector p0;
	myVector p1;
	myUV blend;
	myVector p2;
	myVector p3;

};

typedef struct  //これで確定
{
  float x1,x2,y1,y2,z1,z2;
  float w1,w2;
  float hx1,hx2;  //法線ベクトル
  float hy1,hy2;  //法線ベクトル
  float hz1,hz2;  //法線ベクトル
} MODELVERTEX2;

struct baseMesh {
	BYTE index;
	BYTE flag;
	float f1;
	float f2;
	float f3;
	float f4;
	float f5;
	float f6;
	float f7;
};

#pragma pack(pop)
struct DDS_Header {
	unsigned int unk1;
	unsigned int width;
	unsigned int height;
	WORD unk2;
	WORD unk3;
	DWORD unk4;
	DWORD unk5;
	DWORD unk6;
	DWORD unk7;
	DWORD unk8;
	unsigned int bpp;
	char encoding[4];
	unsigned int size;
	unsigned int unk9;		//512
};

