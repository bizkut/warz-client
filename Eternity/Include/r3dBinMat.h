#ifndef	__R3D_MAT_BINFMT_H
#define	__R3D_MAT_BINFMT_H

#include "r3dBinFmt.h"

#pragma pack(push)
#pragma pack(1)

class r3dMatLib_BinFmt
{
 public:
  #define R3D_MATLIB_BINARY_ID		'biLM'
  #define R3D_MATLIB_BINARY_VER		0x00010002
	struct hdr_s
	{
	  R3D_DEFAULT_BINARY_HDR
	  int		NumMaterials;
	};
};

class r3dMaterial_BinFmt
{
 public:
  #define R3D_MAT_BINARY_ID		' taM'
  #define R3D_MAT_BINARY_VER		0x00010002
	struct hdr_s
	{
	  R3D_DEFAULT_BINARY_HDR
	  char		Name[R3D_MAX_MATERIAL_NAME];
	  int		Flags;
	  float		Color[4];
	};
};

#pragma pack(pop)

#endif	//__R3D_MAT_BINFMT_H
