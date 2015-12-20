#ifndef	__R3D_SKIN_H_36d6815
#define	__R3D_SKIN_H_36d6815

#include "r3dBinFmt.h"
#include "r3dSkeleton.h"

class r3dSkinMesh;
class r3dSkeleton;

// temporary class for skinned mesh - it should go to r3dMesh instead
class r3dSkinMesh
{
  public:
	int		bLoaded;

	int		iNumVertices;
	r3dSkeleton*	pBindSkeleton;

	r3dMesh*	pMesh;
	r3dMaterial* pSkinMaterial;
	
	// array for debugging/collision/whatever
        r3dPoint3D	*VArray;

	// various names for binding bones
	char 		Bone1[R3D_BONENAME_LEN];
	char 		Bone2[R3D_BONENAME_LEN];
	char 		Bone3[R3D_BONENAME_LEN];
	char 		Bone4[R3D_BONENAME_LEN];

	float		SkinScale;
	float		SelfIllumination;

  private:
	void		LoadMesh(const char* fname);
	void		Reset();

  public:
	r3dSkinMesh();
	~r3dSkinMesh();
	void		LoadINI(const char* base_dir, const char* ini_fname);
	void		Unload();

	void		DeformMesh(r3dSkeleton *Skel);

	void		DrawShader(r3dSkeleton *Skel, int SID = 0);
	void		DrawSMPass(r3dSkeleton *Skel);

};

#endif	// __R3D_SKIN_H_36d6815
