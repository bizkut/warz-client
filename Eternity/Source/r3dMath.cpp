#include	"r3dPCH.h"
#include	"r3d.h"

// byte (0-255) to float fast convertion. used for colors
	float		_r3d_b2f[256];

static
int r3d_b2f_init()
{
  for(int i=0; i<256; i++)
    _r3d_b2f[i] = (float)i;
  return 1;
}
static	int		r3d_b2f_init_ = r3d_b2f_init();

//-----------------------------------------------------------------------
void	r3dBuildRotationMatrix(r3dMatrix& Matrix, const r3dPoint3D& p)
//-----------------------------------------------------------------------
{
  r3dBuildRotationMatrix(Matrix, p.X, p.Y, p.Z);
}

//----------------------------------------------------------------
void	r3dBuildRotationMatrix(r3dMatrix& Matrix, float xAngle, float yAngle, float zAngle)
//----------------------------------------------------------------
{
  float xSin = r3dFastSin(xAngle), xCos = r3dFastCos(xAngle);
  float ySin = r3dFastSin(yAngle), yCos = r3dFastCos(yAngle);
  float zSin = r3dFastSin(zAngle), zCos = r3dFastCos(zAngle);

  Matrix[0][0] = yCos * zCos;
  Matrix[0][1] = yCos * zSin;
  Matrix[0][2] = -ySin;
  Matrix[1][0] = xSin * ySin * zCos - xCos * zSin;
  Matrix[1][1] = xSin * ySin * zSin + xCos * zCos;
  Matrix[1][2] = xSin * yCos;
  Matrix[2][0] = xCos * ySin * zCos + xSin * zSin;
  Matrix[2][1] = xCos * ySin * zSin - xSin * zCos;
  Matrix[2][2] = xCos * yCos;
}


//-----------------------------------------------------------------------
void r3dRotateVectorAboutVector(const r3dVector& src, r3dVector& dest, const r3dVector& v, float Sin, float Cos)
//-----------------------------------------------------------------------
{
  /*Note than sin(-a), cos(-a)*/	 

  float x,y,z;
  float NewX, NewY, NewZ;

  x = v[0];
  y = v[1];
  z = -v[2];

  NewX = src[0]*(R3D_SQ(x) + Cos*(1 - R3D_SQ(x))) +
               src[1]*(x*y*(1 - Cos) + z*Sin) -
               src[2]*(z*x*(1 - Cos) - y*Sin);

  NewY = src[0]*(x*y*(1 - Cos) - z*Sin) +
               src[1]*(R3D_SQ(y) + Cos*(1 - R3D_SQ(y))) -
               src[2]*(y*z*(1 - Cos) + x*Sin);

  NewZ = src[0]*(z*x*(1 - Cos) + y*Sin) +
               src[1]*(y*z*(1 - Cos) - x*Sin) -
               src[2]*(R3D_SQ(z) + Cos*(1 - R3D_SQ(z)));

  dest[0] = NewX;  dest[1] = NewY; dest[2] = -NewZ;
}



//--------------------------------------------------------------------------
void r3dRotateVector(r3dVector& v, float xAngle, float yAngle, float zAngle)
//--------------------------------------------------------------------------
{
  r3dMatrix Matrix;
  r3dBuildRotationMatrix(Matrix, xAngle, yAngle, zAngle);
  v *= Matrix;
}

// truncate bits
unsigned short	r3dFloatToHalf(float val)
{
	unsigned char *tmp = (unsigned char*)&val;
	unsigned long bits = ((unsigned long)tmp[3] << 24) | ((unsigned long)tmp[2] << 16) | ((unsigned long)tmp[1] << 8) |(unsigned long)tmp[0];

	if (bits == 0) {
		return 0;
	}
	long e = ((bits & 0x7f800000) >> 23) - 127 + 15;
	if (e < 0) {
		return 0;
	}
	else if (e > 31) {
		e = 31;
	}
	unsigned long s = bits & 0x80000000;
	unsigned long m = bits & 0x007fffff;

	return ((s >> 16) & 0x8000) | ((e << 10) & 0x7c00) | ((m >> 13) & 0x03ff);
}

// add bits
float		r3dHalfToFloat(unsigned short val)
{
	if (val == 0) {
		return 0.0f;
	}
	unsigned long s = val & 0x8000;
	long e =((val & 0x7c00) >> 10) - 15 + 127;
	unsigned long m =  val & 0x03ff;
	unsigned long floatVal = (s << 16) | ((e << 23) & 0x7f800000) | (m << 13);
	float result = 0;
	unsigned char *tmp = (unsigned char*)&result;
	tmp[0] = (floatVal >> 0) & 0xff;
	tmp[1] = (floatVal >> 8) & 0xff;
	tmp[2] = (floatVal >> 16) & 0xff;
	tmp[3] = (floatVal >> 24) & 0xff;

	return result;
}

void r3dOrthoInverse( D3DXMATRIX& Res, const D3DXMATRIX& Mtx )
{
	D3DXMATRIX tmp = Mtx;
	D3DXMatrixTranspose( &tmp, &tmp );

	D3DXVECTOR3 transl = D3DXVECTOR3( Mtx.m[3][0], Mtx.m[3][1], Mtx.m[3][2] );

	tmp.m[3][0] = -D3DXVec3Dot( &transl, (D3DXVECTOR3*)Mtx.m[0] );
	tmp.m[3][1] = -D3DXVec3Dot( &transl, (D3DXVECTOR3*)Mtx.m[1] );
	tmp.m[3][2] = -D3DXVec3Dot( &transl, (D3DXVECTOR3*)Mtx.m[2] );

	tmp.m[0][3] = 0.0f;
	tmp.m[1][3] = 0.0f;
	tmp.m[2][3] = 0.0f;

	Res = tmp;
}

void r3dPerspProjInverse( D3DXMATRIX& Res, const D3DXMATRIX& Mtx )
{
	Res =
	D3DXMATRIX (	1.f / Mtx._11,	0.f,			0.f,	0.f,
					0.f,			1.f / Mtx._22,	0.f,	0.f, 
					0.f,			0.f,			0.f,	1.f / Mtx._43,
					0.f,			0.f,			1.f,	-Mtx._33 / Mtx._43							
		);
}

void r3dOrthoProjInverse( D3DXMATRIX& Res, const D3DXMATRIX& Mtx )
{
	Res =
	D3DXMATRIX (	1.f / Mtx._11,	0.f,			0.f,					0.f,
					0.f,			1.f / Mtx._22,	0.f,					0.f, 
					0.f,			0.f,			1.f / Mtx._33,			0.f,
					0.f,			0.f,			-Mtx._43 / Mtx._33,		1.f	);

}

float r3dTrianlgeArea_x2( r3dPoint2D a, r3dPoint2D b, r3dPoint2D c )
{
	// must be * 0.5 to be not _x2...
	return fabs( ( b.x - a.x ) * ( c.y - a.y ) - ( c.x - a.x ) * ( b.y - a.y ) );
}

R3D_FORCEINLINE static int IsInsideView( const r3dPoint2D& p )
{
	return fabsf( p.x ) <= 1.f && fabsf( p.y ) <= 1.f ;
}

R3D_FORCEINLINE static int GetSegmentIntersection( r3dPoint2D A0, r3dPoint2D A1, r3dPoint2D B0, r3dPoint2D B1, r3dPoint2D* Result )
{
	float x1, y1, x2, y2, x3, y3, x4, y4 ;

	x1 = A0.x ;
	y1 = A0.y ;

	x2 = A1.x ;
	y2 = A1.y ;

	x3 = B0.x ;
	y3 = B0.y ;

	x4 = B1.x ;
	y4 = B1.y ;

	float d = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
	
	if ( fabs( d ) <= 33 * FLT_EPSILON ) 
		return 0 ;

	float xi = ((x3-x4)*(x1*y2-y1*x2)-(x1-x2)*(x3*y4-y3*x4))/d;
	float yi = ((y3-y4)*(x1*y2-y1*x2)-(y1-y2)*(x3*y4-y3*x4))/d;

	const float EPSILON = 1e-05f ;

	if( xi < R3D_MIN(x1,x2) - EPSILON || xi > R3D_MAX(x1,x2) + EPSILON ) 
		return 0 ;

	if( xi < R3D_MIN(x3,x4) - EPSILON || xi > R3D_MAX(x3,x4) + EPSILON ) 
		return 0 ;

	if( yi < R3D_MIN(y1,y2) - EPSILON || yi > R3D_MAX(y1,y2) + EPSILON ) 
		return 0 ;

	if( yi < R3D_MIN(y3,y4) - EPSILON || yi > R3D_MAX(y3,y4) + EPSILON ) 
		return 0 ;

	*Result = r3dPoint2D( xi, yi ) ;

	return 1 ;
}

R3D_FORCEINLINE static void GetIntersections( const r3dPoint2D& pin, const r3dPoint2D& pout, r3dPoint2D* interPos, int* count )
{
	if( GetSegmentIntersection( pin, pout, r3dPoint2D( -1, -1 ), r3dPoint2D( +1, -1 ), interPos ) )
	{
		interPos ++ ;
		++ *count ;
	}

	if( GetSegmentIntersection( pin, pout, r3dPoint2D( -1, +1 ), r3dPoint2D( +1, +1 ), interPos ) )
	{
		interPos ++ ;
		++ *count ;
	}

	if( GetSegmentIntersection( pin, pout, r3dPoint2D( -1, -1 ), r3dPoint2D( -1, +1 ), interPos ) )
	{
		interPos ++ ;
		++ *count ;
	}

	if( GetSegmentIntersection( pin, pout, r3dPoint2D( +1, -1 ), r3dPoint2D( +1, +1 ), interPos ) )
	{
		interPos ++ ;
		++ *count ;
	}
}

R3D_FORCEINLINE static int GetLineToPointSign( r3dPoint2D a, r3dPoint2D b, r3dPoint2D p )
{
	float x1 = a.x ;
	float x2 = b.x ;

	float y1 = a.y ;
	float y2 = b.y ;

	float x = p.x ;
	float y = p.y ;

	return ( y1 - y2 ) * x + ( x2 - x1 ) * y + ( x1 * y2 - x2 * y1 ) > 0.f ;

}

R3D_FORCEINLINE static int IsInsideTri( r3dPoint2D a, r3dPoint2D b, r3dPoint2D c, r3dPoint2D p )
{
	int s1, s2, s3 ;

	s1 = GetLineToPointSign( a, b, p ) ;
	s2 = GetLineToPointSign( b, c, p ) ;
	s3 = GetLineToPointSign( c, a, p ) ;

	return s1 == s2 && s2 == s3 ;
}

R3D_FORCEINLINE void Collapse( r3dPoint2D* points, int* count )
{
	for( int i = 0, e = *count ; i < e; )
	{
		bool unique = true ;

		for( int j = i + 1 ; j < e ; j ++ )
		{
			r3dPoint2D& p0 = points[ i ] ;
			r3dPoint2D& p1 = points[ j ] ;

			if(		fabs( p0.x - p1.x ) < FLT_EPSILON 
						&&
					fabs( p0.y - p1.y ) < FLT_EPSILON )
			{
				R3D_SWAP( p1, points[ e - 1 ] ) ;

				-- * count ;
				-- e ;

				unique = false ;

				break ;
			}
		}

		if( unique )
		{
			i ++ ;
		}
	}

}

R3D_FORCEINLINE void Convexize( r3dPoint2D* points, int count )
{
	for( int i = 1, e = count; i < e; i ++ )
	{
		for( int j = i, e = count ; j < e ; j ++ )
		{
			bool first = true ;
			bool ok = true ;
			int s0 = 0 ;

			for( int k = 0, e = count ; k < e ; k ++ )
			{
				if( i - 1 == k || k == j )
					continue ;

				int s1 = GetLineToPointSign( points[ i - 1 ], points[ j ], points[ k ] ) ;

				if( first )
				{
					s0 = s1 ;
					first = false ;
				}
				else
				{
					if( s0 != s1 )
					{
						ok = false ;
						break ;
					}
				}
			}

			if( ok )
			{
				R3D_SWAP( points[ i ], points[ j ] ) ;
				break ;
			}
		}
	}
}


float r3dViewCulledTriangleArea_x2( r3dPoint2D a, r3dPoint2D b, r3dPoint2D c )
{
	r3dPoint2D area_points[ 16 ] ;
	int point_count = 0 ;

	r3dPoint2D view_inside[ 4 ] ;

	{
		r3dPoint2D p ( -1, -1 ) ;

		if( IsInsideTri( a, b, c, p ) )
		{
			area_points[ point_count ++ ] = p ;
		}
	}

	{
		r3dPoint2D p ( +1, -1 ) ;

		if( IsInsideTri( a, b, c, p ) )
		{
			area_points[ point_count ++ ] = p ;
		}
	}

	{
		r3dPoint2D p ( -1, +1 ) ;

		if( IsInsideTri( a, b, c, p ) )
		{
			area_points[ point_count ++ ] = p ;
		}
	}

	{
		r3dPoint2D p ( +1, +1 ) ;

		if( IsInsideTri( a, b, c, p ) )
		{
			area_points[ point_count ++ ] = p ;
		}
	}

	if( point_count == 4 )
		return 8.f ;

	if( IsInsideView( a ) )
	{
		area_points[ point_count ++ ] = a ;
	}

	if( IsInsideView( b ) )
	{
		area_points[ point_count ++ ] = b ;	
	}

	if( IsInsideView( c ) )
	{
		area_points[ point_count ++ ] = c ;	
	}

	GetIntersections( a, b, area_points + point_count, &point_count ) ;
	GetIntersections( b, c, area_points + point_count, &point_count ) ;
	GetIntersections( c, a, area_points + point_count, &point_count ) ;

	if( point_count < 3 )
		return 0 ;

	Collapse( area_points, &point_count ) ;

	Convexize( area_points, point_count ) ;

	float area_x2 = 0.f ;

	for( int i = 2 ; i < point_count ; i ++ )
	{
		area_x2 +=  r3dTrianlgeArea_x2( area_points[ 0 ], area_points[ i ], area_points[ i - 1 ] );
	}

	// tumik: keep asserting here...
	//r3d_assert( area_x2 < 8.f * ( 1.001f ) ) ;

	return area_x2 ;
}


int	r3dIsPow2( unsigned val )
{
	int c = 0 ;

	for( unsigned m = 0x80000000 ; m ; m >>= 1 )
	{
		if( val & m )
			c ++ ;
	}

	return c == 1 ;
}

int r3dGetBitCount( unsigned val )
{
	int count = 0 ;

	for( ; val ; )
	{
		val >>= 1 ;
		count ++ ;
	}

	return count ;
}

//------------------------------------------------------------------------

uint32_t r3dLog2(uint32_t v)
{
	unsigned int r;
	unsigned int shift;
	r = (v > 0xffff) << 4; v >>= r;
	shift = (v > 0xff) << 3; v >>= shift; r |= shift;
	shift = (v > 0xf) << 2; v >>= shift; r |= shift;
	shift = (v > 0x3) << 1; v >>= shift; r |= shift;
	r |= (v >> 1);
	return r;
}

//------------------------------------------------------------------------

uint32_t r3dNextPow2(uint32_t v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

//------------------------------------------------------------------------

r3dPerlinNoise::r3dPerlinNoise()
{
	frequency_   = 0.015f;
	amplitude_   = 1;
	octaves_     = 4;
	persistence_ = 0.25f;

	seed_        = 1;

	freqOctaveDependent = FALSE;
	ampOctaveDependent  = FALSE;
}

//------------------------------------------------------------------------

r3dPerlinNoise::~r3dPerlinNoise()
{
}

//------------------------------------------------------------------------

float r3dPerlinNoise::Noise_2D(int x, int y)
{
	int n;
	n = x + y * 57;
	n = (n<<13) ^ n;

	float res = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff ) / 1073741824.0);
	return res;
}

//------------------------------------------------------------------------

float r3dPerlinNoise::SmoothedNoise_2D(int x, int y)
{
	float corners = (Noise_2D(x-1, y-1) + Noise_2D(x+1, y-1) + Noise_2D(x-1, y+1) + Noise_2D(x+1, y+1) ) / 16;
	float sides   = (Noise_2D(x-1, y)   + Noise_2D(x+1, y)   + Noise_2D(x, y-1)   + Noise_2D(x, y+1) )   /  8;
	float center  =  Noise_2D(x, y) / 4;

	return corners + sides + center;
}

//------------------------------------------------------------------------

float r3dPerlinNoise::Interpolate(float a, float b, float x)
{
#if 0
	float fac1 = 3*pow(1-x, 2) - 2*pow(1-x,3);
	float fac2 = 3*pow(x, 2) - 2*pow(x, 3);

	return a*fac1 + b*fac2; //add the weighted factors
#else  
	float ft = x * R3D_PI;
	float f  = (1.0f - cosf(ft)) * 0.5f;
	return a * (1.0f - f) + b * f;
#endif  
}

//------------------------------------------------------------------------

float r3dPerlinNoise::InterpolatedNoise_2D(float x, float y)
{
	int   integer_X    = int(x);
	float fractional_X = x - integer_X;

	int   integer_Y    = int(y);
	float fractional_Y = y - integer_Y;

	float v1 = SmoothedNoise_2D(integer_X,     integer_Y);
	float v2 = SmoothedNoise_2D(integer_X + 1, integer_Y);
	float v3 = SmoothedNoise_2D(integer_X,     integer_Y + 1);
	float v4 = SmoothedNoise_2D(integer_X + 1, integer_Y + 1);

	float i1 = Interpolate(v1, v2, fractional_X);
	float i2 = Interpolate(v3, v4, fractional_X);

	return Interpolate(i1, i2, fractional_Y);
}

//------------------------------------------------------------------------

float r3dPerlinNoise::PerlinNoise_2D(float x, float y)
{
	float total = 0;
	float freq  = frequency_;
	float amp   = amplitude_;

	x += seed_;
	y += seed_;

	for(int i=0; i < octaves_; i++) {
		//freq = pow(2.0f, i);
		//amp  = pow(persistence_, i);

		total = total + InterpolatedNoise_2D(x * freq, y * freq) * amp;

		freq *= 2;
		amp  *= persistence_;
	}

	return total;
}
