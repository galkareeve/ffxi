#pragma once

#pragma pack(push,1)
typedef struct myRow
{
	BYTE a:2;
	BYTE b:2;
	BYTE c:2;
	BYTE d:2;
} *pRow;

typedef struct myRGB
{
	union {
		WORD val;
		struct {
			WORD r:5;
			WORD g:6;
			WORD b:5;
		};
	};
} *pRGB;
//
//typedef struct myRGB
//{
//	WORD r:5;
//	WORD g:6;
//	WORD b:5;
//} *pRGB;

typedef struct myRGBA
{
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
} *pRGBA;

typedef struct myRGB2
{
	BYTE b;
	BYTE g;
	BYTE r;
} *pmyRGB2;

typedef struct myAlpha
{
	WORD a:4;
	WORD b:4;
	WORD c:4;
	WORD d:4;
} *pAlpha;

typedef struct bc3Color
{
	DWORD a:2;
	DWORD b:2;
	DWORD c:2;
	DWORD d:2;
	DWORD e:2;
	DWORD f:2;
	DWORD g:2;
	DWORD h:2;
	DWORD i:2;
	DWORD j:2;
	DWORD k:2;
	DWORD l:2;
	DWORD m:2;
	DWORD n:2;
	DWORD o:2;
	DWORD p:2;
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
	BYTE a;
	BYTE b;
	BYTE c;
	BYTE d;
	BYTE e;
	BYTE f;
	BYTE g;
	BYTE h;
	BYTE i;
	BYTE j;
	BYTE k;
	BYTE l;
	BYTE m;
	BYTE n;
	BYTE o;
	BYTE p;
} *pSColor;

typedef struct SAlpha
{
	BYTE a0;
	BYTE a1;
	BYTE a2;
	BYTE a3;
	BYTE a4;
	BYTE a5;
	BYTE a6;
	BYTE a7;
	BYTE a;
	BYTE b;
	BYTE c;
	BYTE d;
	BYTE e;
	BYTE f;
	BYTE g;
	BYTE h;
	BYTE i;
	BYTE j;
	BYTE k;
	BYTE l;
	BYTE m;
	BYTE n;
	BYTE o;
	BYTE p;
} *pSAlpah;

#pragma pack(pop)

typedef struct _Win4xBitmapHeader
{
	DWORD Size;            /* Size of this header in bytes */
	LONG  Width;           /* Image width in pixels */
	LONG  Height;          /* Image height in pixels */
	WORD  Planes;          /* Number of color planes */
	WORD  BitsPerPixel;    /* Number of bits per pixel */
	DWORD Compression;     /* Compression methods used */
	DWORD SizeOfBitmap;    /* Size of bitmap in bytes */
	LONG  HorzResolution;  /* Horizontal resolution in pixels per meter */
	LONG  VertResolution;  /* Vertical resolution in pixels per meter */
	DWORD ColorsUsed;      /* Number of colors in the image */
	DWORD ColorsImportant; /* Minimum number of important colors */
	/* Fields added for Windows 4.x follow this line */

	DWORD RedMask;       /* Mask identifying bits of red component */
	DWORD GreenMask;     /* Mask identifying bits of green component */
	DWORD BlueMask;      /* Mask identifying bits of blue component */
	DWORD AlphaMask;     /* Mask identifying bits of alpha component */
	DWORD CSType;        /* Color space type */
	LONG  RedX;          /* X coordinate of red endpoint */
	LONG  RedY;          /* Y coordinate of red endpoint */
	LONG  RedZ;          /* Z coordinate of red endpoint */
	LONG  GreenX;        /* X coordinate of green endpoint */
	LONG  GreenY;        /* Y coordinate of green endpoint */
	LONG  GreenZ;        /* Z coordinate of green endpoint */
	LONG  BlueX;         /* X coordinate of blue endpoint */
	LONG  BlueY;         /* Y coordinate of blue endpoint */
	LONG  BlueZ;         /* Z coordinate of blue endpoint */
	DWORD GammaRed;      /* Gamma red coordinate scale value */
	DWORD GammaGreen;    /* Gamma green coordinate scale value */
	DWORD GammaBlue;     /* Gamma blue coordinate scale value */
} WIN4XBITMAPHEADER;

class CDDSBlock
{
public:
	CDDSBlock(int w, int h);
	~CDDSBlock(void);

	void BC1decodeBlock(char *src, char *&des, int brow, int bcol);
	void BC1decodeColor(BYTE a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, pRow r, int col, int row, char *&des);
	myRGBA BC1getColor(BYTE sa, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, BYTE pixel);

	void BC2decodeBlock(char *src, char *&des, int brow, int bcol);
	void BC2decodeColor(pAlpha a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, pRow r, int col, int row, char *&des);
	myRGBA BC2getColor(WORD a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, BYTE pixel);

	void BC3decodeBlock(char *src, char *&des, int brow, int bcol);
	myRGBA BC3getColor(SAlpha sa, SColor sc, BYTE apos, BYTE cpos);

	int m_width;
	int m_height;
};

class CDDS2Bmp
{
public:
	CDDS2Bmp(void);
	~CDDS2Bmp(void);

	void convert2BMP(char *p, unsigned int width, unsigned int height, unsigned int ddsSize, unsigned int pixelWidth, char ddsType, char *fn, char *&out);
	std::string fixFileName(char *fn);
	void testCreateBMP(char *p, int width, int height, char *fn);
};

