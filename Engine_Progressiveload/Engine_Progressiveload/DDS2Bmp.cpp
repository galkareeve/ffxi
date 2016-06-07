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


void CDDSBlock::BC1decodeBlock(u8 *src, u8 *&des, u32 brow, u32 bcol)
{
	int coloffset = bcol<<2;
	int roloffset = brow<<2;

	//get color_0 & color_1, @byte 0,1,2,3
	pRGB color0, color1;
	color0 = (pRGB)(src);
	color1 = (pRGB)(src+2);

	myRGBA c0, c1, c2, c3;
	c0.r = (color0->r * 527 + 23) >> 6;
	c0.g = (color0->g * 259 + 33) >> 6;
	c0.b = (color0->b * 527 + 23) >> 6;

	c1.r = (color1->r * 527 + 23) >> 6;
	c1.g = (color1->g * 259 + 33) >> 6;
	c1.b = (color1->b * 527 + 23) >> 6;

	u8 alpha = 0xff;
	if( color0->val > color1->val ) {
		c2.r = 0.667*c0.r + 0.333*c1.r;
		c2.g = 0.667*c0.g + 0.333*c1.g;
		c2.b = 0.667*c0.b + 0.333*c1.b;

		c3.r = 0.333*c0.r + 0.667*c1.r;
		c3.g = 0.333*c0.g + 0.667*c1.g;
		c3.b = 0.333*c0.b + 0.667*c1.b;
	}
	else {
		c2.r = 0.5*c0.r + 0.5*c1.r;
		c2.g = 0.5*c0.g + 0.5*c1.g;
		c2.b = 0.5*c0.b + 0.5*c1.b;

		c3.r = c3.g = c3.b = 0;
		alpha = 0;
	}
	//get color index @byte 4,5,6,7
	pRow r = (pRow)(src+4);
	BC1decodeColor(alpha, c0, c1, c2, c3, r, coloffset, roloffset, des);

	r = (pRow)(src+5);
	BC1decodeColor(alpha, c0, c1, c2, c3,r, coloffset, roloffset+1, des);

	r = (pRow)(src+6);
	BC1decodeColor(alpha, c0, c1, c2, c3,r, coloffset, roloffset+2, des);

	r = (pRow)(src+7);
	BC1decodeColor(alpha, c0, c1, c2, c3,r, coloffset, roloffset+3, des);
}

void CDDSBlock::BC1decodeColor(u8 a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, pRow r, u32 col, u32 row, u8 *&des)
{
	int offset=row*m_width*4 + col*4;
	myRGBA rgba;

	rgba = BC1getColor(a, c0,c1,c2,c3, r->a);
	memcpy(des+offset, &rgba, 4);
	offset +=4;			//32bit color for each pixel

	rgba = BC1getColor(a, c0,c1,c2,c3, r->b);
	memcpy(des+offset, &rgba, 4);
	offset +=4;

	rgba = BC1getColor(a, c0,c1,c2,c3, r->c);
	memcpy(des+offset, &rgba, 4);
	offset +=4;

	rgba = BC1getColor(a, c0,c1,c2,c3, r->d);
	memcpy(des+offset, &rgba, 4);
}

myRGBA CDDSBlock::BC1getColor(u8 a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, u8 pixel)
{
	myRGBA rgba;
	rgba.a = a;
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

void CDDSBlock::BC2decodeBlock(u8 *src, u8 *&des, u32 brow, u32 bcol)
{
	int coloffset = bcol<<2;
	int roloffset = brow<<2;

	//get color_0 & color_1, @byte 8,9,10,11
	pRGB color0, color1;
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
	//shift the endpoint color from 5:6:5 to 8:8:8
	myRGBA c0, c1, c2, c3;
	c0.r = (color0->r * 527 + 23) >> 6;
	c0.g = (color0->g * 259 + 33) >> 6;
	c0.b = (color0->b * 527 + 23) >> 6;

	c1.r = (color1->r * 527 + 23) >> 6;
	c1.g = (color1->g * 259 + 33) >> 6;
	c1.b = (color1->b * 527 + 23) >> 6;

	//calculate interpolate color c2,c3
	c2.r = 0.667*c0.r + 0.333*c1.r;
	c2.g = 0.667*c0.g + 0.333*c1.g;
	c2.b = 0.667*c0.b + 0.333*c1.b;

	c3.r = 0.333*c0.r + 0.667*c1.r;
	c3.g = 0.333*c0.g + 0.667*c1.g;
	c3.b = 0.333*c0.b + 0.667*c1.b;

	//get color index @byte 12,13,14,15, each pixel is a 2-bit reference to c0,c1,c2,c3
	pRow r = (pRow)(src+12);
	//get alpha @byte 0-7
	pAlpha a = (pAlpha)(src);
	BC2decodeColor(a, c0, c1, c2, c3, r, coloffset, roloffset, des);

	r = (pRow)(src+13);
	a = (pAlpha)(src+2);
	BC2decodeColor(a, c0, c1, c2, c3,r, coloffset, roloffset+1, des);

	r = (pRow)(src+14);
	a = (pAlpha)(src+4);
	BC2decodeColor(a, c0, c1, c2, c3,r, coloffset, roloffset+2, des);

	r = (pRow)(src+15);
	a = (pAlpha)(src+6);
	BC2decodeColor(a, c0, c1, c2, c3,r, coloffset, roloffset+3, des);
}

void CDDSBlock::BC2decodeColor(pAlpha a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, pRow r, u32 col, u32 row, u8 *&des)
{
	int offset=row*m_width*4 + col*4;
	myRGBA rgba;

	rgba = BC2getColor(a->d, c0,c1,c2,c3, r->d);
	memcpy(des+offset, &rgba, 4);
	offset +=4;

	rgba = BC2getColor(a->c, c0,c1,c2,c3, r->c);
	memcpy(des+offset, &rgba, 4);
	offset +=4;

	rgba = BC2getColor(a->b, c0,c1,c2,c3, r->b);
	memcpy(des+offset, &rgba, 4);
	offset +=4;

	rgba = BC2getColor(a->a, c0,c1,c2,c3, r->a);
	memcpy(des+offset, &rgba, 4);
}

myRGBA CDDSBlock::BC2getColor(u16 a, myRGBA c0, myRGBA c1, myRGBA c2, myRGBA c3, u8 pixel)
{
	myRGBA rgba;
	rgba.a=a;
	rgba.a = rgba.a << 4;
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

void CDDSBlock::BC3decodeBlock(u8 *src, u8 *&des, u32 brow, u32 bcol)
{
	int coloffset = bcol<<2;
	int roloffset = brow<<2;

	SAlpha sa;
	u8 t1, t2, t3, t4, t5, t6, t7, t8;
	//get alpha @byte 0-7
	sa.a0 = *(u8*)src;
	sa.a1 = *(u8*)(src+1);
	t3 = *(u8*)(src+2);
	t4 = *(u8*)(src+3);
	t5 = *(u8*)(src+4);
	t6 = *(u8*)(src+5);
	t7 = *(u8*)(src+6);
	t8 = *(u8*)(src+7);

	sa.a = t3 & 0x07;
	t3 = t3 >> 3;
	sa.b = t3 & 0x07;
	t3 = t3 >> 3;
	sa.c = (t3 & 0x03) | ((t4 & 0x01)<<2);
	t4 = t4 >> 1;
	sa.d = (t4 & 0x07);
	t4 = t4 >> 3;
	sa.e = (t4 & 0x07);
	t4 = t4 >> 3;
	sa.f = (t4 & 0x01) | ((t5 & 0x03)<<1);
	t5 = t5 >> 2;
	sa.g = (t5 & 0x07);
	t5 = t5 >> 3;
	sa.h = (t5 & 0x07);
	t5 = t5 >> 3;

	sa.i = t6 & 0x07;
	t6 = t6 >> 3;
	sa.j = t6 & 0x07;
	t6 = t6 >> 3;
	sa.k = (t6 & 0x03) | ((t7 & 0x01)<<2);
	t7 = t7 >> 1;
	sa.l = (t7 & 0x07);
	t7 = t7 >> 3;
	sa.m = (t7 & 0x07);
	t7 = t7 >> 3;
	sa.n = (t7 & 0x01) | ((t8 & 0x03)<<1);
	t8 = t8 >> 2;
	sa.o = (t8 & 0x07);
	t8 = t8 >> 3;
	sa.p = (t8 & 0x07);
	t8 = t8 >> 3;

	if( sa.a0==sa.a1 ) {
		sa.a2=sa.a3=sa.a4=sa.a5=sa.a6=sa.a7=sa.a0;
	}
	else if( sa.a0 > sa.a1 ) {	
		sa.a2 = (6*sa.a0 + sa.a1)/7;
		sa.a3 = (5*sa.a0 + 2*sa.a1)/7;
		sa.a4 = (4*sa.a0 + 3*sa.a1)/7;
		sa.a5 = (3*sa.a0 + 4*sa.a1)/7;
		sa.a6 = (2*sa.a0 + 5*sa.a1)/7;
		sa.a7 = (sa.a0 + 6*sa.a1)/7;
	}
	else {
		sa.a2 = (4*sa.a0 + sa.a1)/5;
		sa.a3 = (3*sa.a0 + 2*sa.a1)/5;
		sa.a4 = (2*sa.a0 + 3*sa.a1)/5;
		sa.a5 = (sa.a0 + 4*sa.a1)/5;
		sa.a6 = 0;
		sa.a7 = 255;
	}

	//get color_0 & color_1, @byte 8,9,10,11
	pRGB color0, color1;
	color0 = (pRGB)(src+8);
	color1 = (pRGB)(src+10);

	//shift to 8 bit per color
	SColor sc;
	sc.c0.r = (color0->r * 527 + 23) >> 6;
	sc.c0.g = (color0->g * 259 + 33) >> 6;
	sc.c0.b = (color0->b * 527 + 23) >> 6;

	sc.c1.r = (color1->r * 527 + 23) >> 6;
	sc.c1.g = (color1->g * 259 + 33) >> 6;
	sc.c1.b = (color1->b * 527 + 23) >> 6;

	sc.c2.r = 0.667*sc.c0.r + 0.333*sc.c1.r;
	sc.c2.g = 0.667*sc.c0.g + 0.333*sc.c1.g;
	sc.c2.b = 0.667*sc.c0.b + 0.333*sc.c1.b;

	sc.c3.r = 0.333*sc.c0.r + 0.667*sc.c1.r;
	sc.c3.g = 0.333*sc.c0.g + 0.667*sc.c1.g;
	sc.c3.b = 0.333*sc.c0.b + 0.667*sc.c1.b;

	//get color index @byte 12,13,14,15
	pbc3Color c = (pbc3Color)(src+12);
	sc.a = c->a;
	sc.b = c->b;
	sc.c = c->c;
	sc.d = c->d;
	sc.e = c->e;
	sc.f = c->f;
	sc.g = c->g;
	sc.h = c->h;
	sc.i = c->i;
	sc.j = c->j;
	sc.k = c->k;
	sc.l = c->l;
	sc.m = c->m;
	sc.n = c->n;
	sc.o = c->o;
	sc.p = c->p;

	myRGBA rgba;
	int row=roloffset;

	int offset = row*m_width*4 + coloffset*4;
	rgba = BC3getColor(sa, sc, sa.a, sc.a);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.b, sc.b);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.c, sc.c);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.d, sc.d);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	row++;

	offset = row*m_width*4 + coloffset*4;
	rgba = BC3getColor(sa, sc, sa.e, sc.e);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.f, sc.f);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.g, sc.g);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.h, sc.h);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	row++;

	offset = row*m_width*4 + coloffset*4;
	rgba = BC3getColor(sa, sc, sa.i, sc.i);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.j, sc.j);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.k, sc.k);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.l, sc.l);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	row++;

	offset = row*m_width*4 + coloffset*4;
	rgba = BC3getColor(sa, sc, sa.m, sc.m);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.n, sc.n);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.o, sc.o);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	rgba = BC3getColor(sa, sc, sa.p, sc.p);
	memcpy(des+offset, &rgba, 4);
	offset +=4;
	row++;

}
//
//void CDDSBlock::BC3decodeBlock(u8 *src, u8 *&des, u32 brow, u32 bcol)
//{
//	int coloffset = bcol<<2;
//	int roloffset = brow<<2;
//
//	//get alpha @byte 0-7
//	pbc3Alpha alpha = (pbc3Alpha)src;
//	SAlpha sa;
//	sa.a0 = alpha->alpha0;
//	sa.a1 = alpha->alpha1;
//	if( sa.a0==sa.a1 ) {
//		sa.a2=sa.a3=sa.a4=sa.a5=sa.a6=sa.a7=sa.a0;
//	}
//	else if( sa.a0 > sa.a1 ) {	
//		sa.a2 = (6*sa.a0 + sa.a1)/7;
//		sa.a3 = (5*sa.a0 + 2*sa.a1)/7;
//		sa.a4 = (4*sa.a0 + 3*sa.a1)/7;
//		sa.a5 = (3*sa.a0 + 4*sa.a1)/7;
//		sa.a6 = (2*sa.a0 + 5*sa.a1)/7;
//		sa.a7 = (sa.a0 + 6*sa.a1)/7;
//	}
//	else {
//		sa.a2 = (4*sa.a0 + sa.a1)/5;
//		sa.a3 = (3*sa.a0 + 2*sa.a1)/5;
//		sa.a4 = (2*sa.a0 + 3*sa.a1)/5;
//		sa.a5 = (sa.a0 + 4*sa.a1)/5;
//		sa.a6 = 0;
//		sa.a7 = 255;
//	}
//	sa.a = alpha->aa;
//	sa.b = alpha->ab;
//	sa.c = alpha->ac;
//	sa.d = alpha->ad;
//	sa.e = alpha->ae;
//	sa.f = alpha->af;
//	sa.g = alpha->ag;
//	sa.h = alpha->ah;
//	sa.i = alpha->ai;
//	sa.j = alpha->aj;
//	sa.k = alpha->ak;
//	sa.l = alpha->al;
//	sa.m = alpha->am;
//	sa.n = alpha->an;
//	sa.o = alpha->ao;
//	sa.p = alpha->ap;
//
//	//get color_0 & color_1, @byte 8,9,10,11
//	pRGB color0, color1;
//	color0 = (pRGB)(src+8);
//	color1 = (pRGB)(src+10);
//
//	//shift to 8 bit per color
//	SColor sc;
//	sc.c0.r = (color0->r * 527 + 23) >> 6;
//	sc.c0.g = (color0->g * 259 + 33) >> 6;
//	sc.c0.b = (color0->b * 527 + 23) >> 6;
//
//	sc.c1.r = (color1->r * 527 + 23) >> 6;
//	sc.c1.g = (color1->g * 259 + 33) >> 6;
//	sc.c1.b = (color1->b * 527 + 23) >> 6;
//
//	sc.c2.r = 0.667*sc.c0.r + 0.333*sc.c1.r;
//	sc.c2.g = 0.667*sc.c0.g + 0.333*sc.c1.g;
//	sc.c2.b = 0.667*sc.c0.b + 0.333*sc.c1.b;
//
//	sc.c3.r = 0.333*sc.c0.r + 0.667*sc.c1.r;
//	sc.c3.g = 0.333*sc.c0.g + 0.667*sc.c1.g;
//	sc.c3.b = 0.333*sc.c0.b + 0.667*sc.c1.b;
//
//	//get color index @byte 12,13,14,15
//	pbc3Color c = (pbc3Color)(src+12);
//	sc.a = c->a;
//	sc.b = c->b;
//	sc.c = c->c;
//	sc.d = c->d;
//	sc.e = c->e;
//	sc.f = c->f;
//	sc.g = c->g;
//	sc.h = c->h;
//	sc.i = c->i;
//	sc.j = c->j;
//	sc.k = c->k;
//	sc.l = c->l;
//	sc.m = c->m;
//	sc.n = c->n;
//	sc.o = c->o;
//	sc.p = c->p;
//
//	myRGBA rgba;
//	int row=roloffset;
//
//	int offset = row*m_width*4 + coloffset*4;
//	rgba = BC3getColor(sa, sc, sa.a, sc.a);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.b, sc.b);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.c, sc.c);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.d, sc.d);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	row++;
//
//	offset = row*m_width*4 + coloffset*4;
//	rgba = BC3getColor(sa, sc, sa.e, sc.e);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.f, sc.f);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.g, sc.g);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.h, sc.h);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	row++;
//
//	offset = row*m_width*4 + coloffset*4;
//	rgba = BC3getColor(sa, sc, sa.i, sc.i);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.j, sc.j);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.k, sc.k);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.l, sc.l);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	row++;
//
//	offset = row*m_width*4 + coloffset*4;
//	rgba = BC3getColor(sa, sc, sa.m, sc.m);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.n, sc.n);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.o, sc.o);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	rgba = BC3getColor(sa, sc, sa.p, sc.p);
//	memcpy(des+offset, &rgba, 4);
//	offset +=4;
//	row++;
//
//}

myRGBA CDDSBlock::BC3getColor(SAlpha sa, SColor sc, u8 apos, u8 cpos)
{
	myRGBA rgba;
	switch(cpos)
	{
	case 0:		rgba.r=sc.c0.r; rgba.g=sc.c0.g; rgba.b=sc.c0.b;
		break;
	case 1:		rgba.r=sc.c1.r; rgba.g=sc.c1.g; rgba.b=sc.c1.b;
		break;
	case 2:		rgba.r=sc.c2.r; rgba.g=sc.c2.g; rgba.b=sc.c2.b;
		break;
	case 3:		rgba.r=sc.c3.r; rgba.g=sc.c3.g; rgba.b=sc.c3.b;
		break;
	}

	switch(apos)
	{
	case 0:		rgba.a = sa.a0;
		break;
	case 1:		rgba.a = sa.a1;
		break;
	case 2:		rgba.a = sa.a2;
		break;
	case 3:		rgba.a = sa.a3;
		break;
	case 4:		rgba.a = sa.a4;
		break;
	case 5:		rgba.a = sa.a5;
		break;
	case 6:		rgba.a = sa.a6;
		break;
	case 7:		rgba.a = sa.a7;
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

void CDDS2Bmp::convert2BMP(u8 *p, u32 width, u32 height, u32 ddsSize, u32 pixelWidth, u8 ddsType, u8 *fn, u8 *&image)
{
	CDDSBlock cdds(width,height);
	u32 size = width*height;
//	unsigned int imageSize = ddsSize * (pixelWidth>>3);
	//ignore pixelWidth, always use 32bit for rgba
	u32 imageSize = size*4;
	image = new u8[imageSize];
	memset(image, 0, imageSize);

	u32 bcol = (width+3)>>2;
	u32 brow = (height+3)>>2;

	u32 totalBlock = bcol*brow;
	u32 blockSize = 16;
	if(ddsType=='1')
		blockSize = 8;

	//each block is 16byte to represent 64byte of RGBA
	u8 mem[16];
	u8 *off=p;
	u32 offset=0;
	for(u32 i=0; i<brow; ++i) {
		for(u32 j=0; j<bcol; ++j) {
			memcpy(&mem, off, blockSize);
			switch(ddsType)
			{
			case '1':	cdds.BC1decodeBlock(mem, image,i,j);
				break;
			case '2':	cdds.BC2decodeBlock(mem, image,i,j);
				break;
			case '3':	cdds.BC2decodeBlock(mem, image,i,j);
				break;
			case '5':	cdds.BC3decodeBlock(mem, image, i, j);
				break;
			}
			off += blockSize;
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


