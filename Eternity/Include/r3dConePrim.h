#ifndef R3DCONEPRIM_H_INCLUDED
#define R3DCONEPRIM_H_INCLUDED

static const int NUM_CONEPRIM_VERTICES	= 24;
static const int NUM_CONEPRIM_INDICES	= 132;
static const int NUM_CONELINES_INDICES	= 66;

extern const float g_ConeVertices[ NUM_CONEPRIM_VERTICES ][ 3 ];
extern const UINT16 g_ConeIndices[ NUM_CONEPRIM_INDICES ];
extern const UINT16 g_ConeLineIndices[ NUM_CONELINES_INDICES ];

bool IsInsideCone( float radius, float padding, float zscale, const r3dPoint3D& p );

#endif