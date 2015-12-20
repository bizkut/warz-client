#ifndef	__ETERNITY_R3DCOLOR_H
#define	__ETERNITY_R3DCOLOR_H

#include "r3dPoint.h"

#define R3D_RGB(R,G,B)    (                  ((long)R << 16) + ((long)G << 8) + B)
#define R3D_RGBA(R,G,B,A) (((long)A << 24) + ((long)R << 16) + ((long)G << 8) + B)
#define R3D_ARGB(A,R,G,B) (((long)A << 24) + ((long)R << 16) + ((long)G << 8) + B)

class r3dColor24
{
public:
	// intel byte color order
	BYTE		B, G, R, A;

public:
	r3dColor24() {} 
	explicit r3dColor24(DWORD packed) 
	{
		SetPacked(packed);
	}
	r3dColor24(int r, int g, int b, int a = 255) 
	{
		SetPacked(R3D_RGBA(r, g, b, a));
	}
	r3dColor24(float r, float g, float b, float a = 255.0f) 
	{
		R = (BYTE)r3dFloatToInt(R3D_CLAMP(r, 0.0f, 255.0f));
		G = (BYTE)r3dFloatToInt(R3D_CLAMP(g, 0.0f, 255.0f));
		B = (BYTE)r3dFloatToInt(R3D_CLAMP(b, 0.0f, 255.0f));
		A = (BYTE)r3dFloatToInt(R3D_CLAMP(a, 0.0f, 255.0f));
	}

	// return packed 32bit color.
	DWORD		GetPacked() const {
		return *(DWORD *)&B;
	};
	void		SetPacked(DWORD color) {
		*(DWORD *)&B = color;
	}

	r3dColor24 	operator + (r3dColor24 &koef);

	r3dColor24 	operator * (float koef) const;
	r3dColor24 	operator / (float koef);
	r3dColor24 	operator *= (float koef);
	r3dColor24 	operator *= (r3dVector &koef);
	r3dColor24 	operator *= (r3dColor24 &koef);
	r3dColor24 	operator /= (float koef);

	bool operator==(const r3dColor24& c) const;
	bool operator!=(const r3dColor24& c) const;

public:
	static	const r3dColor24	yellow;
	static	const r3dColor24	black;	
	static	const r3dColor24	grey;	
	static	const r3dColor24	white;	
	static	const r3dColor24	red;	
	static	const r3dColor24	green;	
	static	const r3dColor24	blue;
	static  const r3dColor24    orange;
};
typedef r3dColor24 r3dColor;

inline r3dColor24 r3dColor24::operator + (r3dColor &koef)
{
	return r3dColor24(R+koef.R, G+koef.G, B+koef.B, A);
}



inline r3dColor24 r3dColor24::operator * (float koef) const
{
	float R1, G1, B1, A1;

	R1 = (float)R * koef;
	if (R1 > 255 ) R1 = 255;

	G1 = (float)G * koef;
	if (G1 > 255 ) G1 = 255;

	B1 = (float)B * koef;
	if (B1 > 255 ) B1 = 255;

	A1 = (float)A * koef;
	if (A1 > 255 ) A1 = 255;

	return r3dColor24(R1,G1,B1, A1);
}


inline r3dColor24 r3dColor24::operator / (float koef)
{
	return r3dColor24(
		r3dFloatToInt((float)R / koef),
		r3dFloatToInt((float)G / koef),
		r3dFloatToInt((float)B / koef));
}

inline r3dColor24 r3dColor24::operator *= (float koef)
{
	float C;

	C = (float)R * koef;
	if (C > 255 ) C = 255;
	R = (unsigned char)C;

	C = (float)G * koef;
	if (C > 255 ) C = 255;
	G = (unsigned char)C;

	C = (float)B * koef;
	if (C > 255 ) C = 255;
	B = (unsigned char)C;

	return *this;
}


inline r3dColor24 r3dColor24::operator *= (r3dVector &koef)
{
	R = (BYTE)r3dFloatToInt((float)R * koef.X);
	G = (BYTE)r3dFloatToInt((float)G * koef.Y);
	B = (BYTE)r3dFloatToInt((float)B * koef.Z);
	return *this;
}


inline r3dColor24 r3dColor24::operator *= (r3dColor &koef)
{
	R = (BYTE)(float(R) * (float(koef.R) / 255.0f));
	G = (BYTE)(float(G) * (float(koef.G) / 255.0f));
	B = (BYTE)(float(B) * (float(koef.B) / 255.0f));

	return *this;
}


inline r3dColor24 r3dColor24::operator /= (float koef)
{
	R = (BYTE)r3dFloatToInt((float)R / koef);
	G = (BYTE)r3dFloatToInt((float)G / koef);
	B = (BYTE)r3dFloatToInt((float)B / koef);
	return *this;
}

inline bool r3dColor24::operator==(const r3dColor24& c) const
{
	return c.A == A && c.B == B && c.G == G && c.R == R;
}

inline bool r3dColor24::operator!=(const r3dColor24& c) const
{
	return !operator==(c);
}


//C
//
// r3dColorFormat - color convertion
//
//
typedef DWORD (*r3dfnMakeColor)(int A, int R, int G, int B);
typedef void  (*r3dfnSplitColor)(DWORD Clr, int &A, int &R, int &G, int &B);

struct r3dColorFormat
{
public:
	int		Bits;
	int		Bytes;

	// bits per each color channel
	DWORD		ABits, RBits, GBits, BBits;
	// position of each color channel
	DWORD		APos,  RPos,  GPos,  BBos;

	DWORD		ColorMask;
	DWORD		RGBMask;

	r3dfnMakeColor	Make;
	r3dfnSplitColor	Split;

	void		SetPixel(BYTE *LineStart, int X, DWORD clr)
	{
		switch(Bytes) {
case 1: ((BYTE *)LineStart)[X]  = (BYTE)clr;	break;
case 2: ((WORD *)LineStart)[X]  = (WORD)clr;	break;
case 4: ((DWORD *)LineStart)[X] = (DWORD)clr;	break;
		}
	}
	void		SetPixel(BYTE *LineStart, int X, int A, int R, int G, int B)
	{
		SetPixel(LineStart, X, Make(A, R, G, B));
	}

	DWORD		GetPixel(BYTE *LineStart, int X)
	{
		switch(Bytes) {
case 1: return ((BYTE *)LineStart)[X];
case 2: return ((WORD *)LineStart)[X];
case 4: return ((DWORD *)LineStart)[X];
		}
		return 0;
	}
	void		GetPixel(BYTE *LineStart, int X, int &A, int &R, int &G, int &B)
	{
		Split(GetPixel(LineStart, X), A, R, G, B);
	}
};

#define DEFINE_COLOR_FUNCS(Name, NumBytes, ABits, RBits, GBits, BBits)		\
	\
	extern r3dColorFormat Name;							\
	\
	inline DWORD Make##Name(int A, int R, int G, int B)    			\
{                                                             		\
	return(                                                     		\
	((A >> (8-ABits)) << (BBits + GBits + RBits))  |                  	\
	((R >> (8-RBits)) << (BBits + GBits))          |                  	\
	((G >> (8-GBits)) << (BBits))  	             |                  	\
	((B >> (8-BBits)))                                        		\
	);                                                          		\
}                                                                             \
	\
	inline void Split##Name(DWORD Clr, int &A, int &R, int &G, int &B)   		\
{                                                                             \
	A = ((Clr >> (BBits + GBits + RBits)) & ((1<<ABits)-1)) << (8-ABits);	\
	R = ((Clr >> (BBits + GBits))         & ((1<<RBits)-1)) << (8-RBits);	\
	G = ((Clr >> (BBits))                 & ((1<<GBits)-1)) << (8-GBits);	\
	B = ((Clr)                            & ((1<<BBits)-1)) << (8-BBits);    	\
}                                                                             \

namespace r3dColorConv
{
	DEFINE_COLOR_FUNCS(R8G8B8,   3, 0, 8, 8, 8)
	DEFINE_COLOR_FUNCS(A8R8G8B8, 4, 8, 8, 8, 8)
	DEFINE_COLOR_FUNCS(Q8W8V8U8, 4, 8, 8, 8, 8)
	DEFINE_COLOR_FUNCS(X8R8G8B8, 4, 0, 8, 8, 8)
	DEFINE_COLOR_FUNCS(R5G6B5,   2, 0, 5, 6, 5)
	DEFINE_COLOR_FUNCS(X1R5G5B5, 2, 0, 5, 5, 5)
	DEFINE_COLOR_FUNCS(A1R5G5B5, 2, 1, 5, 5, 5)
	DEFINE_COLOR_FUNCS(A4R4G4B4, 2, 4, 4, 4, 4)
	DEFINE_COLOR_FUNCS(R3G3B2,   1, 0, 3, 3, 2)
	DEFINE_COLOR_FUNCS(A8,       1, 8, 0, 0, 0)
	DEFINE_COLOR_FUNCS(A8R3G3B2, 2, 8, 3, 3, 2)
	DEFINE_COLOR_FUNCS(X4R4G4B4, 2, 0, 4, 4, 4)
};


extern	r3dVector	r3dRGBtoHSV( r3dColor24 rgb );
extern	r3dColor24	r3dHSVtoRGB( r3dVector HSV );

#endif	//__ETERNITY_R3DCOLOR_H
