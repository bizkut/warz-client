#pragma once

#pragma pack(push)
#pragma pack(1)

typedef unsigned char ubyte;
typedef unsigned short ushort;

struct ubyte3
{
	ubyte3();
	ubyte3( unsigned char x, unsigned char y, unsigned char z );

	unsigned char x;
	unsigned char y;
	unsigned char z;
};

struct ubyte4
{
	ubyte4();
	ubyte4( unsigned char x, unsigned char y, unsigned char z, unsigned char w );

	unsigned char x;
	unsigned char y;
	unsigned char z;
	unsigned char w;
};

struct short2
{
	short2();
	short2( short x, short y );

	short x;
	short y;
};

struct ushort2
{
	ushort2();
	ushort2( unsigned short x, unsigned short y );

	unsigned short x;
	unsigned short y;
};

struct short3
{
	short3();
	short3( short x, short y, short z );

	short x;
	short y;
	short z;
};

struct ushort3
{
	ushort3();
	ushort3( unsigned short x, unsigned short y, unsigned short z );

	unsigned short x;
	unsigned short y;
	unsigned short z;
};

struct float2
{
	float2();
	float2( float x, float y );

	float x;
	float y;
};

struct float3
{
	float3();
	float3( float x, float y, float z );

	float x;
	float y;
	float z;
};

struct float4
{
	float4();
	float4( float x, float y, float z, float w );

	float x;
	float y;
	float z;
	float w;
};


struct short4
{
	short4();
	short4( short x, short y, short z, short w );

	short x;
	short y;
	short z;
	short w;
};

struct int2
{
	int x ;
	int y ;

	int2();
	int2( int x, int y );
};

struct int3
{
	int x ;
	int y ;
	int z ;
};

struct int4
{
	int x;
	int y;
	int z;
	int w;
};

//------------------------------------------------------------------------

R3D_FORCEINLINE
ubyte3::ubyte3()
: x( 0 )
, y( 0 )
, z( 0 )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
ubyte3::ubyte3( unsigned char x, unsigned char y, unsigned char z )
: x( x )
, y( y )
, z( z )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
ubyte4::ubyte4()
: x ( 0 )
, y ( 0 )
, z ( 0 )
, w ( 0 )
{


}

//------------------------------------------------------------------------

R3D_FORCEINLINE
ubyte4::ubyte4( unsigned char x, unsigned char y, unsigned char z, unsigned char w )
: x( x )
, y( y )
, z( z )
, w( w )
{

}
//------------------------------------------------------------------------

R3D_FORCEINLINE
short4::short4()
: x ( 0 )
, y ( 0 )
, z ( 0 )
, w ( 0 )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
short4::short4( short x, short y, short z, short w )
: x ( x )
, y ( y )
, z ( z )
, w ( w )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
int2::int2()
: x ( 0 )
, y ( 0 )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
int2::int2( int x, int y )
: x( x )
, y( y )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
short2::short2()
: x( 0 )
, y( 0 )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
short2::short2( short x, short y )
: x( x )
, y( y )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
ushort2::ushort2()
: x ( 0 )
, y ( 0 )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
ushort2::ushort2( unsigned short x, unsigned short y )
: x ( x )
, y ( y )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
short3::short3()
: x( 0 )
, y( 0 )
, z( 0 )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
short3::short3( short x, short y, short z )
: x( x )
, y( y )
, z( z )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
ushort3::ushort3()
: x( 0 )
, y( 0 )
, z( 0 )
{

}

//------------------------------------------------------------------------


R3D_FORCEINLINE
ushort3::ushort3( unsigned short x, unsigned short y, unsigned short z )
: x( x )
, y( y )
, z( z )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
float4::float4()
: x ( 0 )
, y ( 0 )
, z ( 0 )
, w ( 0 )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
float4::float4( float x, float y, float z, float w )
: x ( x )
, y ( y )
, z ( z )
, w ( w )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
float3::float3()
: x ( 0 )
, y ( 0 )
, z ( 0 )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
float3::float3( float x, float y, float z )
: x ( x )
, y ( y )
, z ( z )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
float2::float2()
: x ( 0 )
, y ( 0 )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
float2::float2( float x, float y )
: x ( x )
, y ( y )
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
int4 operator+ ( const short4& a, const short4& b )
{
	int4 res;

	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;
	res.w = a.w + b.w;

	return res;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
int4 operator- ( const short4& a, const short4& b )
{
	int4 res;

	res.x = a.x - b.x;
	res.y = a.y - b.y;
	res.z = a.z - b.z;
	res.w = a.w - b.w;

	return res;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE short r3dNormShort( float val, float scale )
{
	float mul = val < 0.f ? 32768.f : 32767.f;

	r3d_assert( fabs( val ) <= scale );
	return short( R3D_MIN( R3D_MAX( int( mul * val / scale ), -32768 ), 32767 ) );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE short r3dUnitNormShort( float val )
{
	float mul = val < 0.f ? 32768.f : 32767.f;

	r3d_assert( fabs( val ) <= 1.f );
	return short( R3D_MIN( R3D_MAX( int( mul * val ), -32768 ), 32767 ) );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE unsigned char r3dUnitNormByte( float val )
{
	val = val * 0.5f + 0.5f;

	r3d_assert( val >= 0.f && val <= 1.f );

	return unsigned char( R3D_MIN( R3D_MAX( int( val * 255.f), 0 ), 255 ) );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE short3 r3dNormShort3( const r3dPoint3D& val, const r3dPoint3D& scale )
{
	short3 res;

	res.x = r3dNormShort( val.x, scale.x );
	res.y = r3dNormShort( val.y, scale.y );
	res.z = r3dNormShort( val.z, scale.z );

	return res;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE short3 r3dUnitNormShort3( const r3dPoint3D& val )
{
	short3 res;

	res.x = r3dUnitNormShort( val.x );
	res.y = r3dUnitNormShort( val.y );
	res.z = r3dUnitNormShort( val.z );

	return res;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE short2 r3dUnitNormShort2( const r3dPoint2D& val )
{
	short2 res;

	res.x = r3dUnitNormShort( val.x );
	res.y = r3dUnitNormShort( val.y );

	return res;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE ubyte4 r3dUnitNormByte4( const r3dPoint3D& val )
{
	ubyte4 res;

	res.x = r3dUnitNormByte( val.x );
	res.y = r3dUnitNormByte( val.y );
	res.z = r3dUnitNormByte( val.z );
	res.w = 255;

	return res;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE short4 r3dNormShort4( const r3dPoint3D& val, const r3dPoint3D& scale )
{
	short4 res;

	res.x = r3dNormShort( val.x, scale.x );
	res.y = r3dNormShort( val.y, scale.y );
	res.z = r3dNormShort( val.z, scale.z );
	res.w = 32767;

	return res;
}

#pragma pack(pop)