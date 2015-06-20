#include "DDS2Bmp.h"
#include <string.h>


CDDSBlock::CDDSBlock(int w, int h)
{
	m_width=w;
	m_height=h;
}

CDDSBlock::~CDDSBlock(void)
{

}

void CDDSBlock::decodeBlock(u8 *src, u8 *&des, u32 brow, u32 bcol)
{
	int coloffset = bcol<<2;
	int roloffset = brow<<2;

	//get color_0 & color_1, @byte 8,9,10,11
	pRGB color0, color1, color2, color3;
	color0 = (pRGB)(src+8);
	color1 = (pRGB)(src+10);

/*
	integer implementation
	R8 = ( R5 * 527 + 23 ) >> 6;
	G8 = ( G6 * 259 + 33 ) >> 6;
	B8 = ( B5 * 527 + 23 ) >> 6;

	R5 = ( R8 * 249 + 1014 ) >> 11;
	G6 = ( G8 * 253 +  505 ) >> 10;
	B5 = ( B8 * 249 + 1014 ) >> 11;

	floating point implementation
	R8 = (int) floor( R5 * 255.0 / 31.0 + 0.5);
	G8 = (int) floor( G6 * 255.0 / 63.0 + 0.5);
	B8 = (int) floor( R5 * 255.0 / 31.0 + 0.5);
*/
	myRGBA c0, c1, c2, c3;
	c0.r = (color0->r * 527 + 23) >> 6;
	c0.g = (color0->g * 259 + 33) >> 6;
	c0.b = (color0->b * 527 + 23) >> 6;

	c1.r = (color1->r * 527 + 23) >> 6;
	c1.g = (color1->g * 259 + 33) >> 6;
	c1.b = (color1->b * 527 + 23) >> 6;

	c2.r = 0.667*c0.r + 0.333*c1.r;
	c2.g = 0.667*c0.g + 0.333*c1.g;
	c2.b = 0.667*c0.b + 0.333*c1.b;

	c3.r = 0.333*c0.r + 0.667*c1.r;
	c3.g = 0.333*c0.g + 0.667*c1.g;
	c3.b = 0.333*c0.b + 0.667*c1.b;

	//get color index @byte 12,13,14,15
	pRow r = (pRow)(src+12);
	//get alpha @byte 0-7
	pAlpha a = (pAlpha)(src);
	decodeColor(a, c0, c1, c2, c3, r, coloffset, roloffset, des);

	r = (pRow)(src+13);
	a = (pAlpha)(src+2);
	decodeColor(a, c0, c1, c2, c3,r, coloffset, roloffset+1, des);

	r = (pRow)(src+14);
	a = (pAlpha)(src+4);
	decodeColor(a, c0, c1, c2, c3,r, coloffset, roloffset+2, des);

	r = (pRow)(src+15);
	a = (pAlpha)(src+6);
	decodeColor(a, c0, c1, c2, c3,r, coloffset, roloffset+3, des);
}

void CDDSBlock::decodeColor(pAlpha a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, pRow r, int col, int row, u8 *&des)
{
	int offset=row*m_width*4 + col*4;
	myRGBA rgba;

	rgba = getColor(a->d, c0,c1,c2,c3, r->d);
	memcpy(des+offset, &rgba, 4);
	offset +=4;

	rgba = getColor(a->c, c0,c1,c2,c3, r->c);
	memcpy(des+offset, &rgba, 4);
	offset +=4;

	rgba = getColor(a->b, c0,c1,c2,c3, r->b);
	memcpy(des+offset, &rgba, 4);
	offset +=4;

	rgba = getColor(a->a, c0,c1,c2,c3, r->a);
	memcpy(des+offset, &rgba, 4);
}

myRGBA CDDSBlock::getColor(u16 a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, u8 pixel)
{
	myRGBA rgba;
	//a=(a<<4);
	rgba.a=(a==0)? 0:0xff;
//	rgba.a=0xFF;
	switch(pixel)
	{
	case 0:		rgba.r=c0.r; rgba.g=c0.g; rgba.b=c0.b;
		break;
	case 1:		rgba.r=c1.r; rgba.g=c1.g; rgba.b=c1.b;
		break;
	case 2:		rgba.r=c2.r; rgba.g=c2.g; rgba.b=c2.b;
		break;
	case 3:		rgba.r=c3.r; rgba.g=c3.g; rgba.b=c3.b;
		break;
	}
	return rgba;
}



CDDS2Bmp::CDDS2Bmp(void)
{
}


CDDS2Bmp::~CDDS2Bmp(void)
{
}

void CDDS2Bmp::convert2BMP(u8 *p, u32 width, u32 height, u8 *&out)
{
	CDDSBlock cdds(width,height);
	int size = width*height;
	int imageSize = size*4;
	out = new u8[imageSize];
	memset(out, 0, imageSize);

	u32 bcol = (width+3)>>2;
	u32 brow = (height+3)>>2;

	int totalBlock = bcol*brow;
	//each block is 16byte to represent 64byte of RGBA
	u8 mem[16];
	u8 *off=p;
	int offset=0;
	for(u32 i=0; i<brow; ++i) {
		for(u32 j=0; j<bcol; ++j) {
			memcpy(&mem, off, 16);
			cdds.decodeBlock(mem, out,i,j);
			off += 16;
		}
		offset++;	//dummy for debug
	}

}

/*
void CDDS2Bmp::convert2BMP(char *p, int width, int height, char *fn)
{
	CDDSBlock cdds(width,height);
	int size = width*height;
	int imageSize = size*4;
	char *image = new char[imageSize];
	memset(image, 0, imageSize);

	int bcol = (width+3)>>2;
	int brow = (height+3)>>2;

	int totalBlock = bcol*brow;
	//each block is 16byte to represent 64byte of RGBA
	char mem[16];
	char *off=p;
	int offset=0;
	for(int i=0; i<brow; ++i) {
		for(int j=0; j<bcol; ++j) {
			memcpy(&mem, off, 16);
			cdds.decodeBlock(mem, image,i,j);
			off += 16;
		}
		offset++;	//dummy for debug
	}

	//write to bmp file
	BITMAPFILEHEADER fh;
	fh.bfType= 0x4D42;		//BM
	fh.bfReserved1=0;
	fh.bfReserved2=0;
	fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(WIN4XBITMAPHEADER);
	fh.bfSize = fh.bfOffBits + size*4;

	WIN4XBITMAPHEADER ih;
	ih.Size=108;
	ih.Width=width;
	ih.Height=height;
	ih.Planes=1;
	ih.BitsPerPixel=32;
	ih.Compression=BI_BITFIELDS;
	ih.SizeOfBitmap=imageSize;
	ih.HorzResolution=2835;
	ih.VertResolution=2835;
	ih.ColorsUsed=0;
	ih.ColorsImportant=0;
	ih.RedMask=0x00FF0000;
	ih.GreenMask=0x0000FF00;
	ih.BlueMask=0x000000FF;
	ih.AlphaMask=0xFF000000;
	ih.CSType=0x01;
	ih.RedX=ih.RedY=ih.RedZ=ih.GammaRed=0;
	ih.GreenX=ih.GreenY=ih.GreenZ=ih.GammaGreen=0;
	ih.BlueX=ih.BlueY=ih.BlueZ=ih.GammaBlue=0;
	
	//ensure fn does not contain space and end with bmp
	std::string sfn = fixFileName(fn);
	std::ofstream writeFile( sfn, std::ofstream::binary );
	if( writeFile ) {
		writeFile.write(reinterpret_cast<char*>(&fh), sizeof(BITMAPFILEHEADER));
		writeFile.write(reinterpret_cast<char*>(&ih), sizeof(WIN4XBITMAPHEADER));
		writeFile.write(image, imageSize);
		writeFile.close();
    }
}

std::string CDDS2Bmp::fixFileName(char *fn)
{
	std::string str;

	int pos=-1;
	for(int i=0; i<15; ++i) {
		if(fn[i]==' ') {
			fn[i]='_';
		}
	}
	str.assign(fn);
	str += ".bmp";
	return str;
}

void CDDS2Bmp::testCreateBMP(char *p, int width, int height, char *fn)
{
	//write to bmp file
	int size=width*height;
	int imageSize = size*4;
	char *image = new char[imageSize];
	memset(image, 0, imageSize);

	BITMAPFILEHEADER fh;
	fh.bfType= 0x4D42;		//BM
	fh.bfReserved1=0;
	fh.bfReserved2=0;
	fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(WIN4XBITMAPHEADER);
	fh.bfSize = fh.bfOffBits + size*4;

	WIN4XBITMAPHEADER ih;
	ih.Size=108;
	ih.Width=width;
	ih.Height=height;
	ih.Planes=1;
	ih.BitsPerPixel=32;
	ih.Compression=BI_BITFIELDS;
	ih.SizeOfBitmap=imageSize;
	ih.HorzResolution=2835;
	ih.VertResolution=2835;
	ih.ColorsUsed=0;
	ih.ColorsImportant=0;
	ih.RedMask=0x00FF0000;
	ih.GreenMask=0x0000FF00;
	ih.BlueMask=0x000000FF;
	ih.AlphaMask=0xFF000000;
	ih.CSType=0x01;
	ih.RedX=ih.RedY=ih.RedZ=ih.GammaRed=0;
	ih.GreenX=ih.GreenY=ih.GreenZ=ih.GammaGreen=0;
	ih.BlueX=ih.BlueY=ih.BlueZ=ih.GammaBlue=0;

	myRGBA rgb;
	//blue is lsb, alpha is msb
	rgb.b=0x00;
	rgb.g=0xFF;
	rgb.r=0;
	rgb.a=0x64;
	char *ptr=image;
	for(int i=0; i<size; ++i) {
		memcpy(ptr, &rgb, 4);
		ptr+=4;
	}
	//ensure fn does not contain space and end with bmp
	std::string sfn = fixFileName(fn);
	std::ofstream writeFile( sfn, std::ofstream::binary );
	if( writeFile ) {
		writeFile.write(reinterpret_cast<char*>(&fh), sizeof(BITMAPFILEHEADER));
		writeFile.write(reinterpret_cast<char*>(&ih), sizeof(WIN4XBITMAPHEADER));
		writeFile.write(image, imageSize);
		writeFile.close();
    }
}
*/


