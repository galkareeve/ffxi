#include <string>
#include <iostream>
#include <fstream>

#include "CmyBitmap.h"
CmyBitmap::CmyBitmap(void)
{
}


CmyBitmap::~CmyBitmap(void)
{
}

std::string CmyBitmap::fixFileName(char *fn)
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

void CmyBitmap::outputBitmap(u8 *image, char *fn, unsigned int width, unsigned height)
{
	//write to bmp file
	int imageSize = width*height*4;
	BITMAPFILEHEADER fh;
	fh.bfType= 0x4D42;		//BM
	fh.bfReserved1=0;
	fh.bfReserved2=0;
	fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(WIN4XBITMAPHEADER);
	fh.bfSize = fh.bfOffBits + imageSize;

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
	writeFile.write(reinterpret_cast<char*>(&fh), sizeof(BITMAPFILEHEADER));
	writeFile.write(reinterpret_cast<char*>(&ih), sizeof(WIN4XBITMAPHEADER));
	writeFile.write((char*)image, imageSize);
	writeFile.close();
}