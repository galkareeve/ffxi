#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>

using namespace glm;

struct test
{
	u8 a:2;
	u8 b:2;
	u8 c:2;
	u8 d:2;
};

typedef struct myRow
{
	u8 d:2;
	u8 c:2;
	u8 b:2;
	u8 a:2;
} *pRow;

typedef struct myRGB
{
	union {
		u16 val;
		struct {
			u16 r:5;
			u16 g:6;
			u16 b:5;
		};
	};
} *pRGB;

typedef struct myRGBA
{
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} *pRGBA;

typedef struct myRGB2
{
	u8 b;
	u8 g;
	u8 r;
} *pmyRGB2;

typedef struct myAlpha
{
	u16 d:4;
	u16 c:4;
	u16 b:4;
	u16 a:4;
} *pAlpha;


typedef struct bc3Color
{
	u32 a:2;
	u32 b:2;
	u32 c:2;
	u32 d:2;
	u32 e:2;
	u32 f:2;
	u32 g:2;
	u32 h:2;
	u32 i:2;
	u32 j:2;
	u32 k:2;
	u32 l:2;
	u32 m:2;
	u32 n:2;
	u32 o:2;
	u32 p:2;
} *pbc3Color;

typedef struct bc3Alpha
{
	_int8 alpha0:8;
	_int8 alpha1:8;
	_int8 aa:3;
	_int8 ab:3;
	_int8 ac:3;
	_int8 ad:3;
	_int8 ae:3;
	_int8 af:3;
	_int8 ag:3;
	_int8 ah:3;
	_int8 ai:3;
	_int8 aj:3;
	_int8 ak:3;
	_int8 al:3;
	_int8 am:3;
	_int8 an:3;
	_int8 ao:3;
	_int8 ap:3;
} *pbc3Alpha;

typedef struct SColor
{
	myRGBA c0;
	myRGBA c1;
	myRGBA c2;
	myRGBA c3;
	u8 a;
	u8 b;
	u8 c;
	u8 d;
	u8 e;
	u8 f;
	u8 g;
	u8 h;
	u8 i;
	u8 j;
	u8 k;
	u8 l;
	u8 m;
	u8 n;
	u8 o;
	u8 p;
} *pSColor;

typedef struct SAlpha
{
	u8 a0;
	u8 a1;
	u8 a2;
	u8 a3;
	u8 a4;
	u8 a5;
	u8 a6;
	u8 a7;
	u8 a;
	u8 b;
	u8 c;
	u8 d;
	u8 e;
	u8 f;
	u8 g;
	u8 h;
	u8 i;
	u8 j;
	u8 k;
	u8 l;
	u8 m;
	u8 n;
	u8 o;
	u8 p;
} *pSAlpah;

//typedef struct tagBITMAPFILEHEADER {
//  u16  bfType;
//  u32 bfSize;
//  u16  bfReserved1;
//  u16  bfReserved2;
//  u32 bfOffBits;
//} BITMAPFILEHEADER, *PBITMAPFILEHEADER;
//
//typedef struct _Win4xBitmapHeader
//{
//	u32 Size;            /* Size of this header in bytes */
//	u32  Width;           /* Image width in pixels */
//	u32  Height;          /* Image height in pixels */
//	u16  Planes;          /* Number of color planes */
//	u16  BitsPerPixel;    /* Number of bits per pixel */
//	u32 Compression;     /* Compression methods used */
//	u32 SizeOfBitmap;    /* Size of bitmap in bytes */
//	u32  HorzResolution;  /* Horizontal resolution in pixels per meter */
//	u32  VertResolution;  /* Vertical resolution in pixels per meter */
//	u32 ColorsUsed;      /* Number of colors in the image */
//	u32 ColorsImportant; /* Minimum number of important colors */
//	/* Fields added for Windows 4.x follow this line */
//
//	u32 RedMask;       /* Mask identifying bits of red component */
//	u32 GreenMask;     /* Mask identifying bits of green component */
//	u32 BlueMask;      /* Mask identifying bits of blue component */
//	u32 AlphaMask;     /* Mask identifying bits of alpha component */
//	u32 CSType;        /* Color space type */
//	u32  RedX;          /* X coordinate of red endpoint */
//	u32  RedY;          /* Y coordinate of red endpoint */
//	u32  RedZ;          /* Z coordinate of red endpoint */
//	u32  GreenX;        /* X coordinate of green endpoint */
//	u32  GreenY;        /* Y coordinate of green endpoint */
//	u32  GreenZ;        /* Z coordinate of green endpoint */
//	u32  BlueX;         /* X coordinate of blue endpoint */
//	u32  BlueY;         /* Y coordinate of blue endpoint */
//	u32  BlueZ;         /* Z coordinate of blue endpoint */
//	u32 GammaRed;      /* Gamma red coordinate scale value */
//	u32 GammaGreen;    /* Gamma green coordinate scale value */
//	u32 GammaBlue;     /* Gamma blue coordinate scale value */
//} WIN4XBITMAPHEADER;

class CDDSBlock
{
public:
	CDDSBlock(int w, int h);
	~CDDSBlock(void);

	void BC1decodeBlock(u8 *src, u8 *&des, u32 brow, u32 bcol);
	void BC1decodeColor(u8 a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, pRow r, u32 col, u32 row, u8 *&des);
	myRGBA BC1getColor(u8 sa, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, u8 pixel);

	void BC2decodeBlock(u8 *src, u8 *&des, u32 brow, u32 bcol);
	void BC2decodeColor(pAlpha a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, pRow r, u32 col, u32 row, u8 *&des);
	myRGBA BC2getColor(u16 a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, u8 pixel);

	void BC3decodeBlock(u8 *src, u8 *&des, u32 brow, u32 bcol);
	myRGBA BC3getColor(SAlpha sa, SColor sc, u8 apos, u8 cpos);

	int m_width;
	int m_height;
};

class CDDS2Bmp
{
public:
	CDDS2Bmp(void);
	~CDDS2Bmp(void);

	void convert2BMP(u8 *p, u32 width, u32 height, u32 ddsSize, u32 pixelWidth, u8 ddsType, u8 *fn, u8 *&image);
//	std::string fixFileName(char *fn);
//	void testCreateBMP(char *p, int width, int height, char *fn);
};

