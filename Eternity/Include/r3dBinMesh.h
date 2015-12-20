#ifndef	__R3D_OBJ_BINFMT_H
#define	__R3D_OBJ_BINFMT_H

#include "r3dBinFmt.h"

#pragma pack(push)
#pragma pack(1)

#define R3D_OBJ_BINARY_ID	'hseM'
#define R3D_OBJ_BINARY_VER	0x00020001

struct R3D_BIN_MESH_HEADER
{
 R3D_DEFAULT_BINARY_HDR

 char		Name[128];
 int		NumVertices;
 int		NumFaces;
 int		hasVCP;
};

struct R3D_BIN_MESH_VERTEX
{
 float		V[3];
};

struct R3D_BIN_MESH_VCP
{
 char		C[3];
};

struct R3D_BIN_MESH_FACE
{
 short		VertInd[3];
 char		MatName[64];
 float		TX[3];
 float		TY[3];
};

#pragma pack(pop)

#endif	//__R3D_OBJ_BINFMT_H
