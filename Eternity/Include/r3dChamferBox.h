#ifndef R3DCHAMFERBOX_H_INCLUDED
#define R3DCHAMFERBOX_H_INCLUDED

static const int NUM_CHAMFERBOX_VERTICES = 24;
static const int NUM_CHAMFERBOX_INDICES = 44 * 3 ;

extern const float	g_ChamferBoxVertices[NUM_CHAMFERBOX_VERTICES][3];
extern const UINT16	g_ChamferBoxIndices[NUM_CHAMFERBOX_INDICES];

#endif