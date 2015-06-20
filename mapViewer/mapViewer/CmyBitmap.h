#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>

using namespace glm;

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER {
        u16    bfType;
        u32   bfSize;
        u16    bfReserved1;
        u16    bfReserved2;
        u32   bfOffBits;
} BITMAPFILEHEADER;

typedef struct _Win4xBitmapHeader
{
	u32 Size;            /* Size of this header in bytes */
	long  Width;           /* Image width in pixels */
	long  Height;          /* Image height in pixels */
	u16  Planes;          /* Number of color planes */
	u16  BitsPerPixel;    /* Number of bits per pixel */
	u32 Compression;     /* Compression methods used */
	u32 SizeOfBitmap;    /* Size of bitmap in bytes */
	long  HorzResolution;  /* Horizontal resolution in pixels per meter */
	long  VertResolution;  /* Vertical resolution in pixels per meter */
	u32 ColorsUsed;      /* Number of colors in the image */
	u32 ColorsImportant; /* Minimum number of important colors */
	/* Fields added for Windows 4.x follow this line */

	u32 RedMask;       /* Mask identifying bits of red component */
	u32 GreenMask;     /* Mask identifying bits of green component */
	u32 BlueMask;      /* Mask identifying bits of blue component */
	u32 AlphaMask;     /* Mask identifying bits of alpha component */
	u32 CSType;        /* Color space type */
	long  RedX;          /* X coordinate of red endpoint */
	long  RedY;          /* Y coordinate of red endpoint */
	long  RedZ;          /* Z coordinate of red endpoint */
	long  GreenX;        /* X coordinate of green endpoint */
	long  GreenY;        /* Y coordinate of green endpoint */
	long  GreenZ;        /* Z coordinate of green endpoint */
	long  BlueX;         /* X coordinate of blue endpoint */
	long  BlueY;         /* Y coordinate of blue endpoint */
	long  BlueZ;         /* Z coordinate of blue endpoint */
	u32 GammaRed;      /* Gamma red coordinate scale value */
	u32 GammaGreen;    /* Gamma green coordinate scale value */
	u32 GammaBlue;     /* Gamma blue coordinate scale value */
} WIN4XBITMAPHEADER;
#pragma pack(pop)

class CmyBitmap
{
public:
	CmyBitmap(void);
	~CmyBitmap(void);

	std::string fixFileName(char *fn);
	void outputBitmap(u8 *image, char *fn, unsigned int width, unsigned height);
};

