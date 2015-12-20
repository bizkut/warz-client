#ifndef	__R3D_MATH_H
#define	__R3D_MATH_H

class	r3dMatrix;

#define R3D_EPSILON	0.00001

// byte (0-255) to float fast convertion. used for colors
extern	float		_r3d_b2f[256];


R3D_FORCEINLINE float r3dFastSin(float Degree) { return sinf(R3D_DEG2RAD(Degree)); }
R3D_FORCEINLINE float r3dFastCos(float Degree) { return cosf(R3D_DEG2RAD(Degree)); }
R3D_FORCEINLINE float r3dFastTan(float Degree) { return tanf(R3D_DEG2RAD(Degree)); }


void		r3dBuildRotationMatrix(r3dMatrix& Matrix, float xAngle, float yAngle, float zAngle);
void		r3dBuildRotationMatrix(r3dMatrix& Matrix, const r3dPoint3D& p);

void		r3dCalcOrientation(r3dPoint3D& p, const r3dVector& vRight, const r3dVector& vUp, const r3dVector& vForw);
void		r3dCalcOrientation(r3dPoint3D& p, r3dMatrix& Matrix);
r3dPoint3D 	r3dCalcOrientation(const r3dVector& vRight, const r3dVector& vUp, const r3dVector& vForw);

unsigned short	r3dFloatToHalf(float val);
float		r3dHalfToFloat(unsigned short val);
uint32_t r3dNextPow2(uint32_t v);

void		r3dOrthoInverse( D3DXMATRIX& Res, const D3DXMATRIX& Mtx );
void		r3dPerspProjInverse( D3DXMATRIX& Res, const D3DXMATRIX& Mtx );
void		r3dOrthoProjInverse( D3DXMATRIX& Res, const D3DXMATRIX& Mtx );
float		r3dTrianlgeArea_x2( r3dPoint2D a, r3dPoint2D b, r3dPoint2D c );
float		r3dViewCulledTriangleArea_x2( r3dPoint2D a, r3dPoint2D b, r3dPoint2D c ); // culled by -1..1 post projection plane

int			r3dIsPow2( unsigned val ) ;
int			r3dGetBitCount( unsigned val ) ;
void		r3dRotateVector(r3dVector& v, float xAngle, float yAngle, float zAngle) /*-------------------------------------------------------------------------- */;
uint32_t	r3dLog2(uint32_t v);

//------------------------------------------------------------------------

class r3dPerlinNoise
{
public:
	int			seed_;

	float		amplitude_;
	float		frequency_;
	int			octaves_;
	float		persistence_;

	BOOL		freqOctaveDependent;
	BOOL		ampOctaveDependent;

private:
	float		Noise_2D(int x, int y);
	float		SmoothedNoise_2D(int x, int y);
	float		Interpolate(float a, float b, float x);
	float		InterpolatedNoise_2D(float x, float y);

public:
	r3dPerlinNoise();
	~r3dPerlinNoise();

	float		PerlinNoise_2D(float x, float y);
};

#endif	//__R3D_MATH_H
