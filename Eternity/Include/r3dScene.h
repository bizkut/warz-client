#ifndef	__R3D_OBJ_BINFMT_H
#define	__R3D_OBJ_BINFMT_H

#include "r3dBinFmt.h"

#pragma pack(push)
#pragma pack(1)
class r3dMeshObject_BinFmt
{
  public:
  #define R3D_OBJ_BINARY_ID	'hseM'
  #define R3D_OBJ_BINARY_VER	0x00010005
	struct hdr_s
	{
	  R3D_DEFAULT_BINARY_HDR

	  char		Name[R3D_MAX_OBJECT_NAME];
	  int		NumVertices;
	  int		NumFaces;
	  float		CPoint[3];	// x, y, z
	  int		MatNameLen;
	  int		hasVCP;
	};

	struct vert_s
	{
	  float		V[3];
	};

	struct face_s
	{
	  short		VertInd[3];
	  short		MatNameOff;
	  float		TX[3];
	  float		TY[3];
	};
};
#pragma pack(pop)

#endif	//__R3D_OBJ_BINFMT_H
