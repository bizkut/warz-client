#include "r3dPCH.h"
#include "r3d.h"


#define DEFINE_COLOR_DATA(Name, Bytes, ABits, RBits, GBits, BBits)	\
									\
  r3dColorFormat Name = {						\
    Bytes << 3,                                                         \
    Bytes,                                                              \
    ABits,                                                              \
    RBits,                                                              \
    GBits,                                                              \
    BBits,                                                              \
    (BBits + GBits + RBits),                                            \
    (BBits + GBits),                                                    \
    (BBits),                                                            \
    0,                                                                  \
    (((unsigned __int64)1)<<(BBits + GBits + RBits + ABits)) - 1,       \
    (1<<(BBits + GBits + RBits)) - 1,                                   \
    &Make##Name,                                                        \
    &Split##Name							\
  };

namespace r3dColorConv
{
  DEFINE_COLOR_DATA(R8G8B8,   3, 0, 8, 8, 8)
  DEFINE_COLOR_DATA(A8R8G8B8, 4, 8, 8, 8, 8)
  DEFINE_COLOR_DATA(Q8W8V8U8, 4, 8, 8, 8, 8)
  DEFINE_COLOR_DATA(X8R8G8B8, 4, 0, 8, 8, 8)
  DEFINE_COLOR_DATA(R5G6B5,   2, 0, 5, 6, 5)
  DEFINE_COLOR_DATA(X1R5G5B5, 2, 0, 5, 5, 5)
  DEFINE_COLOR_DATA(A1R5G5B5, 2, 1, 5, 5, 5)
  DEFINE_COLOR_DATA(A4R4G4B4, 2, 4, 4, 4, 4)
  DEFINE_COLOR_DATA(R3G3B2,   1, 0, 3, 3, 2)
  DEFINE_COLOR_DATA(A8,       1, 8, 0, 0, 0)
  DEFINE_COLOR_DATA(A8R3G3B2, 2, 8, 3, 3, 2)
  DEFINE_COLOR_DATA(X4R4G4B4, 2, 0, 4, 4, 4)
};

r3dColorFormat *r3dRenderLayer::GetColorFormatData(D3DFORMAT Fmt)
{
#define MK_CASE(x)			\
  case D3DFMT_##x:			\
    return &r3dColorConv::##x;

  switch(Fmt)
  {
    default:
    MK_CASE(R8G8B8)
    MK_CASE(A8R8G8B8)
    MK_CASE(Q8W8V8U8)
    MK_CASE(X8R8G8B8)
    MK_CASE(R5G6B5)
    MK_CASE(X1R5G5B5)
    MK_CASE(A1R5G5B5)
    MK_CASE(A4R4G4B4)
    MK_CASE(R3G3B2)
    MK_CASE(A8)
    MK_CASE(A8R3G3B2)
    MK_CASE(X4R4G4B4)
  }
#undef MK_CASE

  return NULL;
}

	const r3dColor	r3dColor::yellow(255,255,0);
	const r3dColor	r3dColor::black(0, 0, 0);
	const r3dColor	r3dColor::grey(80,80,80);
	const r3dColor	r3dColor::white(255, 255, 255);
	const r3dColor	r3dColor::red(255, 0, 0);
	const r3dColor	r3dColor::green(0, 255, 0);
	const r3dColor	r3dColor::blue(0, 0, 255);
	const r3dColor	r3dColor::orange(255, 140, 0);


r3dVector r3dRGBtoHSV( r3dColor24 rgb )
{
	float r = float(rgb.R) / 255.0f;
	float g = float(rgb.G) / 255.0f;
	float b = float(rgb.B) / 255.0f;

	float mi, ma, delta ;

	r3dVector HSV;

	mi = R3D_MIN( R3D_MIN( r, g ), b );
	ma = R3D_MAX( R3D_MAX( r, g ), b );

	HSV.z = ma;				// v

	delta = ma - mi + 0.0001f;

	if( ma != 0 )
	{
		HSV.y = delta / ma;		// s
	}
	else 
	{
		// r = g = b = 0	// s = 0, v is undefined
		HSV.y = 0;
		HSV.x = -1;

		return HSV;
	}

	if( r == ma )
		HSV.x = ( g - b ) / delta;		// between yellow & magenta
	else if( g == ma )
		HSV.x = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		HSV.x = 4 + ( r - g ) / delta;	// between magenta & cyan

	if( HSV.x < 0 )
		HSV.x += 6;

	HSV.x /= 6;

	return HSV;
}

r3dColor24 r3dHSVtoRGB ( r3dVector HSV )
{
	if (  HSV.y < 0.0001f )
		return r3dColor24 ( HSV.z * 255.0f, HSV.z * 255.0f, HSV.z * 255.0f );

	float r, g, b;
	float var_h = HSV.x * 6.0f;
	int var_i 	= (int) ( var_h );
	float var_1 = HSV.z * ( 1.0f - HSV.y );
	float var_2 = HSV.z * ( 1.0f - HSV.y * ( var_h - (float)var_i ) );
	float var_3 = HSV.z * ( 1.0f - HSV.y * ( 1.0f - ( var_h - (float)var_i ) ) );

	if      ( var_i == 0 ) { r = HSV.z ; g = var_3 ; b = var_1; }
	else if ( var_i == 1 ) { r = var_2 ; g = HSV.z ; b = var_1; }
	else if ( var_i == 2 ) { r = var_1 ; g = HSV.z ; b = var_3; }
	else if ( var_i == 3 ) { r = var_1 ; g = var_2 ; b = HSV.z; }
	else if ( var_i == 4 ) { r = var_3 ; g = var_1 ; b = HSV.z; }
	else                   { r = HSV.z ; g = var_1 ; b = var_2; }

	return r3dColor24 ( R3D_CLAMP ( (int)(r * 255.0f), 0, 255 ), R3D_CLAMP ( (int)(g * 255.0f), 0, 255 ), R3D_CLAMP ( (int)(b * 255.0f), 0, 255 ) );
}
