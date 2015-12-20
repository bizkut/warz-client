#ifndef	__R3D_SKELETON_H_36d6813
#define	__R3D_SKELETON_H_36d6813

#include "r3dBinFmt.h"

class r3dBone;
class r3dSkeleton;
class r3dAnimData;

//NOTE temporary crap, will be moved somewhere else later
typedef D3DXQUATERNION	r3dQuat;

#define R3D_BONENAME_LEN	128

enum
{
	// see Skin_VS.hls, SkinSM_VS.hls, Transparent_vs.hls
	MAX_BONES = 73,
	START_BONE_CONSTANT = 24
};

class r3dBone
{
	friend class r3dSkeleton;
public:
	const char* GetBoneName() const;

public:
	char*		Name;
	int		iBoneId;
	int		iParentId;
	float		fLength;
	float		fCollisionRadius;

	// from skeletal data
	D3DXMATRIX	mAbsPlacement;
	r3dQuat		qRelPlacement;
	r3dPoint3D	vRelPlacement;
	// current animation
	r3dQuat		qCur;
	r3dPoint3D	vCur;
	// world matrix
	D3DXMATRIX	mBonePlacement;
	// bone transform matrix
	D3DXMATRIX	CurrentTM;
};

class r3dSkeleton
{
	friend class r3dAnimPool;
	friend class r3dSkinMesh;

public:
	// binary format
	#define R3D_SKELETON_BINARY_ID	'tlks'
	#define R3D_SKELETON_BINARY_VER	0x00000001
	struct binhdr_s
	{
		R3D_DEFAULT_BINARY_HDR;
	};
	// table for remapping bone_id to track_id
	struct BoneRemap_s
	{
		int	iBoneToTrack[128];
	};

	typedef void (*fn_AdjustBoneCallback)(DWORD dwData, int boneId, D3DXMATRIX &mp, D3DXMATRIX &anim);

public:
	char*		pFileName;

	fn_AdjustBoneCallback pAdjustBoneCallback;
	DWORD		dwCallbackData1;

	int		bLoaded;
	//int		bCloned;

	DWORD		dwSkeletonId;
	int		NumBones;
	r3dBone*	Bones;     // uniqoe for each skeleton instance
	char*		BoneNames; // shared between all instanced

public:
	r3dSkeleton();
	~r3dSkeleton();

	const char*	GetFileName() const;

	int		GetNumBones() const;
	const char*	GetBoneName( int idx ) const;
	int		GetParentBoneId( int idx ) const;
	r3dPoint3D	GetRootPos() const;

	void		LoadBinary(const char* fname);
	void		LoadBinaryV1(r3dFile *f);
	void		Unload();
	r3dSkeleton*	Clone() const;

	// set default pose to frame 0 of that animation, or to default skeleton pose if NULL
	void		SetDefaultPose(r3dAnimData *ad = NULL);
	void		SetCallback(fn_AdjustBoneCallback pFN, DWORD dwData);

	void		ResetTransform();
	void		Apply(const r3dAnimData* pAnim, r3dSkeleton::BoneRemap_s &rt, float fCurFrame, float fInfluence);
	/**	boneInclusionArr should be null, or array with size of NumBones */
	void            Recalc(D3DXMATRIX *mBase = NULL, int bDisableRootMove = 0, bool *boneSkipArr = 0);

	void		DrawSkeleton(const r3dCamera& Cam, const r3dPoint3D& off);

	void		SetShaderConstants() const;
	static void	SetDefaultShaderConstants();

	int		GetBoneID(const char* name) const;
	void		GetBoneWorldTM( int iBoneId, D3DXMATRIX *m, const D3DXMATRIX& offset ) const;
	void		GetBoneWorldTM( const char* name, D3DXMATRIX *m, const D3DXMATRIX& offset ) const 
	{
		GetBoneWorldTM( GetBoneID(name), m, offset );
	}
	void		SetBoneWorldTM(int iBoneId, D3DXMATRIX *m);

	const r3dBone*	CheckCollision(const r3dPoint3D& vRayOrg, const r3dPoint3D& vRayDir, float fLength) const;
};

R3D_FORCEINLINE const char*
r3dSkeleton::GetFileName() const
{
	return pFileName;
}

R3D_FORCEINLINE int
r3dSkeleton::GetNumBones() const
{
	return NumBones;
}

R3D_FORCEINLINE const char*
r3dSkeleton::GetBoneName( int idx ) const
{
	return Bones[ idx ].Name;
}

R3D_FORCEINLINE
int
r3dSkeleton::GetParentBoneId( int idx ) const
{
	return Bones[ idx ].iParentId;
}

R3D_FORCEINLINE
r3dPoint3D
r3dSkeleton::GetRootPos() const
{
	r3dBone &b = Bones[0];

	r3dPoint3D Pos;

	Pos.X = b.CurrentTM._41;
	Pos.Y = b.CurrentTM._42;
	Pos.Z = b.CurrentTM._43;

	return Pos;
}

#endif	// __R3D_SKELETON_H_36d6813
