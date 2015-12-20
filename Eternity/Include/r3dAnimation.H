#ifndef	__R3D_SKELETALANIM_H_36d6814
#define __R3D_SKELETALANIM_H_36d6814

#include "r3dBinFmt.h"
#include "r3dSkeleton.h"
#include "r3dSkin.h"
#include "Tsg_stl/HashTable.h"

class r3dAnimData;
class r3dAnimation;

typedef r3dgameVector(r3dSTLString) stringlist_t;

class r3dAnimData
{
	friend class r3dAnimPool;
	friend class r3dAnimation;
public:
	struct track_s 
	{
		char	boneName[R3D_BONENAME_LEN];
		int	bEnabled;

		struct frame_s {
			r3dQuat q;
			r3dPoint3D  v;
		};
		frame_s*	data;		// NumFrames[] array
		int		bOwnData;	// true if data* is owned by this instance.
	};

	// binary format
	#define R3D_ANIMDATA_BINARY_ID  'dmna'
	#define R3D_ANIMDATA_BINARY_VER 0x00000003
	struct binhdr_s
	{
		R3D_DEFAULT_BINARY_HDR;
	};
	
	track_s::frame_s* getSharedFrameData(int iTrack);

public:
	// data
	int		bLoaded;
	r3dAnimPool*	pAnimPool;	// animation pool used for sharing anim track data.
  
	DWORD		dwSkeletonId;

	int		NumTracks;
	int		NumFrames;
	float		fFrameRate;

	char*		pAnimName;
	char*		pAnimFileName;
	track_s*	pTracks;	   // NumTracks[] array
	int		bDisableRootMove;
	
	int		iAnimId;

private:	
	// make copy constructor and assignment operator inaccessible
	r3dAnimData(const r3dAnimData& rhs);
	r3dAnimData& operator=(const r3dAnimData& rhs);

public:
	r3dAnimData(r3dAnimPool* pool);
	~r3dAnimData();

	BOOL		LoadBinary(const char* fname, float fInitialAngle);
	BOOL		LoadBinaryV1(r3dFile *f, float fInitialAngle);
	void		SaveBinary(const char* fname);
	void		Unload();
	void		Reload();

	void		EDITOR_ReloadFromPool();

	// fill boneremap table
	void		BindSkeleton(r3dSkeleton *pSkeleton, r3dSkeleton::BoneRemap_s &rt);

	// return false if track isn't found or disabled
	int		GetTM(int trackId, float fFrame, D3DXMATRIX &m) const;
	int		GetTM(int trackId, float fFrame, r3dQuat &q, r3dPoint3D& v) const;
	
	// track control
	track_s*	GetTrack(const char* pTrack);
	void		BipedSetEnabled(int bEnabled);
	void		BipedSetEnabledGroup(int bEnabled, int group);
	void		EnableTrack(const char* pTrack, int bEnable);

	int		GetNumFrames() const ;
	int		GetNumTracks() const ;
	int		GetAnimID() const ;
	const char*	GetAnimName() const ;
	const char*	GetAnimFileName() const;
	float		GetFrameRate() const;

private:
	void		UnloadData();
	void		EDITOR_UnloadDataFromPool();
};

R3D_FORCEINLINE int r3dAnimData::GetNumFrames() const 
{
	return NumFrames; 
}

R3D_FORCEINLINE int r3dAnimData::GetNumTracks() const 
{
	return NumTracks;
}

R3D_FORCEINLINE int r3dAnimData::GetAnimID() const 
{ 
	return iAnimId; 
}

R3D_FORCEINLINE const char* r3dAnimData::GetAnimName() const 
{
	return pAnimName;
}

R3D_FORCEINLINE const char* r3dAnimData::GetAnimFileName() const 
{
	return pAnimFileName; 
}

R3D_FORCEINLINE float r3dAnimData::GetFrameRate() const 
{
	return fFrameRate;
}


class r3dAnimPool
{
public:
	r3dgameVector(r3dAnimData*) Anims;

	// struct used to check duplicate track data between animations, enabled by r_use_shared_animtracks
	struct trackInfo_s
	{
	  r3dAnimData*	ad;
	  int		iTrack;
	  DWORD		crc32;
	};

	struct Crc32HashFunc_T 
	{
		inline int operator () (DWORD key) { return key; }
	};
	typedef HashTableDynamic<trackInfo_s, DWORD, Crc32HashFunc_T, 1024> HashTable;
	//	For lazy initialization and memory saving purposes
	HashTable & GetDupTracks();
	int		animDataTotalSize;
	int		animDataSharedSize;

private:	
	// make copy constructor and assignment operator inaccessible
	r3dAnimPool(const r3dAnimPool& rhs);
	r3dAnimPool& operator=(const r3dAnimPool& rhs);

	HashTable *dupTracks;

public:
	r3dAnimPool();
	~r3dAnimPool();
	void		Unload();   
	int		Add(const char* name, const char* fname, float fInitialAngle = 0.0f);
	r3dAnimData*	Get(int iId);
	r3dAnimData*	Get(const char* name);
	void		Reload(const char* fname);
};

class r3dAnimation
{
public:
#define ANIMFLAG_Looped		(1<<1)
#define ANIMFLAG_PauseOnEnd	(1<<2)
#define ANIMFLAG_FadeOther	(1<<3)  // fade out all previous animation
#define ANIMFLAG_RemoveOtherFade (1<<4) // remove all previous animation when this one faded in completely
#define ANIMFLAG_RemoveOtherNow (1<<5)  // remove all previous animation instantly

#define ANIMSTATUS_Playing	(1<<10)
#define ANIMSTATUS_Paused	(1<<11)
#define ANIMSTATUS_Finished	(1<<12)
#define ANIMSTATUS_Fading	(1<<13)
#define ANIMSTATUS_Expiring	(1<<14)

	class r3dAnimInfo
	{
	  public:
	  const r3dAnimData* pAnim;
	  float		fInfluence;
	  float		fCurFrame;

	  DWORD		dwStatus;
	  int		iTrackId;
	  float		fFadeStart, fFadeEnd, fFadeTime;
	  float		fExpireTime;

	  float		fFPS;
	  float		fSpeed;
	  float		fStartFrame;

	  r3dAnimInfo() : iTrackId(0), fSpeed(1.0f), pAnim(0), fStartFrame(0) {}

	  public: //TODO: remove later
	  DWORD GetStatus() const { return dwStatus; }
	  void SetStatus(DWORD newStatus) { dwStatus = newStatus; }
	  int GetTrackID() const { return iTrackId; }
	  float GetSpeed() const { return fSpeed; }
	  void SetSpeed(float s) { fSpeed = s; }
	  const r3dAnimData * GetAnim() const { return pAnim; }
	  float GetCurFrame() const { return fCurFrame; }
	  void SetCurFrame(float f) { fCurFrame = f; }
	  float GetInfluence() const { return fInfluence; }
	  void SetInfluence(float f) { fInfluence = f; }
	  void SetStartFame(float f) { fStartFrame = f; fCurFrame = f; }
	};
	typedef r3dgameVector(r3dAnimInfo) AnimTracksVec;
	AnimTracksVec AnimTracks;

public:
	int		bInited;

	bool		bSkelDirty;
	r3dSkeleton*	pSkeleton;	// our instance of skeleton

	D3DXMATRIX	mRotation;
	r3dPoint3D	vPosition;
  
	r3dAnimPool*	pAnimPool;	// animation pool
#define MAX_NUMBER_ANIMS_IN_POOL 2048
	r3dSkeleton::BoneRemap_s* pBoneRemaps[MAX_NUMBER_ANIMS_IN_POOL]; // table to bone-track remaps for pAnimPool entries, max 512 animations

private:	
	// make copy constructor and assignment operator inaccessible
	r3dAnimation(const r3dAnimation& rhs);
	r3dAnimation& operator=(const r3dAnimation& rhs);

public:
	r3dAnimation();
	~r3dAnimation();
	
	void		Init(const r3dSkeleton* bindSkeleton, r3dAnimPool *anim, r3dSkeleton::fn_AdjustBoneCallback pFN = NULL, DWORD dwData = 0);
	void		Unload();

	// update frame of the animations, do not recalc skeleton
	void		Update(float fTimePassed, const r3dPoint3D& pos, const D3DXMATRIX& mat);
	// recalc skeleton, based on current state of the animation
	void		Recalc();
	r3dSkeleton*	GetCurrentSkeleton();
	r3dSkeleton*	GetCurrentSkeletonNoUpdate();

	// start/stop animation, return track ID - not track number
	int		StartAnimation(const char* pAnimName, DWORD flags, float fStartFade = 0.0f, float fEndFade = 1.0f, float fFadeTime = 0.2f);
	int		StartAnimation(int iAnimId, DWORD flags, float fStartFade = 0.0f, float fEndFade = 1.0f, float fFadeTime = 0.2f);
	
	void		Stop(int iTrackId);
	void		StopAll();
	void		FadeOut(int iTrackId, float fFadeTime);
	r3dAnimInfo*	GetTrack(int iTrackId);

	void		GetActiveAnimationNames( stringlist_t& oAnims ) const;
	r3dAnimPool*	GetAnimPool() { return pAnimPool; }
	void		CalcBasePose(D3DXMATRIX &outMat) const;
};

#endif  // __R3D_SKELETALANIM_H_36d6814