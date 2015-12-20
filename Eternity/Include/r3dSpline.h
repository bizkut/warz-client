#ifndef __R3D_SPLINE_H_4B5CC2271977
#define __R3D_SPLINE_H_4B5CC2271977

#include "r3dBinFmt.h"

class r3dSpline3D
{
  public:
	int		bLoaded;
	// binary format
	#define R3D_SPLINE3D_BINARY_ID	'nlps'
	#define R3D_SPLINE3D_BINARY_VER	0x00000001
	struct binhdr_s
	{
	  R3D_DEFAULT_BINARY_HDR;
	};
  
 
	r3dPoint3D	*pV;		// vertices
	r3dPoint3D	*pIn;		// in vector for bezier
	r3dPoint3D	*pOut;		// out vector for bezier
	int		iNumPt;
	int		bBezier;
	float		fSplineLength;	// spline length
	
	struct seg_s
	{
	  float		fPos1;		// start of segment
	  float		fPos2;		// end of segment
	};
	seg_s		*pSeg;
	
  
  public:
	r3dSpline3D();
	~r3dSpline3D();
	void		Unload();
	
	void		LoadBinary(char *fname);
	void		Scale(float fScale);
	void		Draw(const r3dCamera& Cam);
	
	r3dPoint3D	GetPos(float fLen, int *iSeg, r3dPoint3D *vForw);

// Three control point Bezier interpolation
static	r3dPoint3D	Bezier3(const r3dPoint3D& p1, const r3dPoint3D& p2, const r3dPoint3D& p3, float mu);
	// Four control point Bezier interpolation
static	r3dPoint3D	Bezier4(const r3dPoint3D& p1, const r3dPoint3D& p2, const r3dPoint3D& p3, const r3dPoint3D& p4, float mu);
	//   General Bezier curve
static	r3dPoint3D	Bezier(r3dPoint3D *p, int n, float mu);
};

#endif	// __R3D_SPLINE_4B5CC2271977
