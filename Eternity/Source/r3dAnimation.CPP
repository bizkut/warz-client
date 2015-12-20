#include "r3dPCH.h"
#include "r3d.h"
#include "r3dAnimation.h"

extern	int	r3dAnimation_bInterpolation = 1;

	// used memory between all pools
	int	r3dAnimPool_SharedSize = 0;
	int	r3dAnimPool_TotalSize  = 0;

struct BipedBoneIds_s
{
    // 0 - generic
    // 1 - lower
    // 2 - upper
    int iGroup;
    char    *pName;
};
static BipedBoneIds_s _BipedBones[] = 
{
  0,   "Bip01",
  0,     "Bip01_Pelvis",
  0,       "Bip01_Spine",
  2,         "Bip01_Spine1",
  2,           "Bip01_Spine2",
  2,             "Bip01_Neck",
  2,               "Bip01_Head",
  2,                 "Bip01_HeadNub",
  2,               "Bip01_L_Clavicle",
  2,                 "Bip01_L_UpperArm",
  2,                   "Bip01_L_Forearm",
  2,                     "Bip01_L_Hand",
  2,                       "Bip01_L_Finger0",
  2,                         "Bip01_L_Finger0Nub",
  2,                       "Bip01_L_Finger1",
  2,                         "Bip01_L_Finger1Nub",
  2,                       "Bip01_L_Finger2",
  2,                         "Bip01_L_Finger2Nub",
  2,               "Bip01_R_Clavicle",
  2,                 "Bip01_R_UpperArm",
  2,                   "Bip01_R_Forearm",
  2,                     "Bip01_R_Hand",
  2,                       "Bip01_R_Finger0",
  2,                         "Bip01_R_Finger0Nub",
  2,                       "Bip01_R_Finger1",
  2,                         "Bip01_R_Finger1Nub",
  2,                       "Bip01_R_Finger2",
  2,                         "Bip01_R_Finger2Nub",
  2,                       "PrimaryWeapon",
  2,                 "SecondaryWeapon",
  1,         "Bip01_L_Thigh",
  1,           "Bip01_L_Calf",
  1,             "Bip01_L_Foot",
  1,               "Bip01_L_Toe0",
  1,                 "Bip01_L_Toe0Nub",
  1,         "Bip01_R_Thigh",
  1,           "Bip01_R_Calf",
  1,             "Bip01_R_Foot",
  1,               "Bip01_R_Toe0",
  1,                 "Bip01_R_Toe0Nub",
  1,         "Item1",
  1,         "Item2",
  1,         "Item3"
};



	//cr3dAnimData
	//
	//
	//
	//

r3dAnimData::r3dAnimData(r3dAnimPool* pool)
{
	pAnimPool     = pool;
	NumTracks     = 0;
	NumFrames     = 0;
	pTracks       = NULL;

	pAnimName     = NULL;
	pAnimFileName = NULL;

	bLoaded 	= 0;
}

r3dAnimData::~r3dAnimData()
{
	Unload();
}

void r3dAnimData::UnloadData()
{
	if(pAnimPool)
		r3dError("Can't unload animation data with shared pool - add \"r_use_shared_animtracks 0\" in local.ini");

	EDITOR_UnloadDataFromPool();
}

void r3dAnimData::EDITOR_UnloadDataFromPool()
{
	for(int i=0; i<NumTracks; ++i) {
		if(pTracks[i].bOwnData)
			delete[] pTracks[i].data; 
	}

	delete[] pTracks;

	bLoaded = 0;
}

void r3dAnimData::Unload()
{
  if(!bLoaded) return;

  UnloadData();

  free(pAnimName);
  free(pAnimFileName);

  pAnimFileName = NULL;
  
  bLoaded = 0;
}

void r3dAnimData::Reload()
{
	if(pAnimFileName)
	{
		UnloadData();
		LoadBinary(pAnimFileName, 0.0f);
	}
}

void r3dAnimData::EDITOR_ReloadFromPool()
{
	if(pAnimFileName)
	{
		EDITOR_UnloadDataFromPool();
		LoadBinary(pAnimFileName, 0.0f);
	}
}

BOOL r3dAnimData::LoadBinary(const char* fname, float fInitialAngle)
{
  if(bLoaded) Unload();

  //FIXME: implement resource leak fix if exception is thrown.
  r3dFile *f = r3d_open(fname, "rb");
  if(!f) return FALSE; //r3dError("can't open %s", fname);
  
  binhdr_s hdr;
  if(fread(&hdr, sizeof(hdr), 1, f) != 1) r3dError("%s - can't read header from", fname);
  if(hdr.r3dID != R3D_BINARY_FILE_ID)     r3dError("%s - invalid header", fname);
  if(hdr.ID != R3D_ANIMDATA_BINARY_ID)	  r3dError("%s - not an animation file", fname);

  pAnimFileName = strdup(fname);

  switch(hdr.Version) {
    default:
      r3dError("%s - unsupported version", fname);
      break;
      
    case 0x00000003:
      LoadBinaryV1(f, fInitialAngle);
      break;
  }

  fclose(f);

  return TRUE;
}

r3dAnimData::track_s::frame_s* r3dAnimData::getSharedFrameData(int iTrack)
{
	if(pAnimPool == NULL)
		return NULL;
		
	int size = sizeof(r3dAnimData::track_s::frame_s) * NumFrames;
	DWORD crc32 = r3dHash::MakeHash((char*)pTracks[iTrack].data, size);

	pAnimPool->animDataTotalSize += size;
	r3dAnimPool_TotalSize += size;

	r3dAnimPool::trackInfo_s ti;
	if(pAnimPool->GetDupTracks().GetObject(crc32, &ti) == false)
	{
		// add track info
		ti.ad        = this;
		ti.crc32     = crc32;
		ti.iTrack    = iTrack;
		pAnimPool->GetDupTracks().Add(crc32, ti);
		return NULL;
	}

	// compare data to avoid potential problems
	const void* m1 = ti.ad->pTracks[ti.iTrack].data;
	const void* m2 = pTracks[iTrack].data;
	if(ti.ad->NumFrames != NumFrames || memcmp(m1, m2, size) != 0)
	{
#ifndef FINAL_BUILD
		r3dOutToLog("!!! ADUP: %s:%s(%s) differ with same crc %s:%s\n", pAnimFileName, pTracks[iTrack].boneName, pAnimName, ti.ad->pAnimFileName, ti.ad->pTracks[ti.iTrack].boneName);
#endif
		return NULL;
	}

	pAnimPool->animDataSharedSize += size;
	r3dAnimPool_SharedSize += size;
	return ti.ad->pTracks[ti.iTrack].data;
	
}

BOOL r3dAnimData::LoadBinaryV1(r3dFile *f, float fInitialAngle)
{
  r3dQuat    qAdjust;
  D3DXMATRIX mAdjust;
  int bHaveAdjust = fabs(fInitialAngle) >= 1.0f;
  if(bHaveAdjust) 
  {
	  D3DXQuaternionRotationYawPitchRoll(&qAdjust, R3D_DEG2RAD(fInitialAngle/10.0f), 0, 0);
	  D3DXMatrixRotationYawPitchRoll(&mAdjust, R3D_DEG2RAD(fInitialAngle/10.0f), 0, 0);
  }

  // read file V1
  DWORD dw1, dw2, dw3, dw4;
  fread(&dw1, sizeof(DWORD), 1, f);
  fread(&dw2, sizeof(DWORD), 1, f);
  fread(&dw3, sizeof(DWORD), 1, f);
  fread(&dw4, sizeof(DWORD), 1, f);

  dwSkeletonId = dw1;
  NumTracks  = dw2;
  NumFrames  = dw3;
  // store original framerate
  fFrameRate = (float)dw4;
  pTracks    = game_new track_s[NumTracks];

  for(int iTrack = 0; iTrack < NumTracks; iTrack++) 
  {
    // read bone name
    fread(pTracks[iTrack].boneName, 32, 1, f);
    DWORD dwFlags;
    fread(&dwFlags, sizeof(DWORD), 1, f);
    int bRootBone = dwFlags & (1L<<1);
    
    pTracks[iTrack].data = game_new track_s::frame_s[NumFrames];

    for(int i=0;i<NumFrames; ++i) 
    {
      track_s::frame_s &fr = pTracks[iTrack].data[i];

      float in_f[7];
      fread(in_f, sizeof(float), 7, f);
      fr.q.x = in_f[0];
      fr.q.y = in_f[1];
      fr.q.z = in_f[2];
      fr.q.w = in_f[3];
      fr.v.x = in_f[4];
      fr.v.y = in_f[5];
      fr.v.z = in_f[6];
      if(bRootBone && bHaveAdjust) {
        fr.q = fr.q * qAdjust;
        D3DXVec3TransformCoord(fr.v.d3dx(), fr.v.d3dx(), &mAdjust);
      }
    }
    
    r3dAnimData::track_s::frame_s* data = getSharedFrameData(iTrack);
    if(data == NULL)
    {
      pTracks[iTrack].bOwnData = true;
    }
    else
    {
      SAFE_DELETE(pTracks[iTrack].data);
      pTracks[iTrack].data     = data;
      pTracks[iTrack].bOwnData = false;
    }
  }
 
  bLoaded = 1;

  BipedSetEnabled(TRUE);

  bDisableRootMove = 0;

  return TRUE;
}

void r3dAnimData::SaveBinary(const char* fname)
{
  if(!bLoaded)
    return;
    
  FILE *f = fopen(fname, "wb");
  if(!f) r3dError("can't open %s", fname);

  // write header
  binhdr_s hdr;
  hdr.r3dID   = R3D_BINARY_FILE_ID;
  hdr.ID      = R3D_ANIMDATA_BINARY_ID;
  hdr.Version = R3D_ANIMDATA_BINARY_VER;
  fwrite(&hdr, sizeof(hdr), 1, f);
  
  int NumTracks_new = 0;
  for(int iTrack = 0; iTrack < NumTracks; iTrack++) {
    if(pTracks[iTrack].bEnabled) NumTracks_new++;
  }

  // write file V1
  DWORD dw1, dw2, dw3, dw4;
  dw1 = dwSkeletonId;
  dw2 = NumTracks_new;
  dw3 = NumFrames;
  dw4 = (DWORD)fFrameRate;

  fwrite(&dw1, sizeof(DWORD), 1, f);
  fwrite(&dw2, sizeof(DWORD), 1, f);
  fwrite(&dw3, sizeof(DWORD), 1, f);
  fwrite(&dw4, sizeof(DWORD), 1, f);

  for(int iTrack = 0; iTrack < NumTracks; iTrack++) {
    if(!pTracks[iTrack].bEnabled) continue;

    // write bone name
    char bn[128];
    memset(bn, 0, sizeof(bn));
    r3dscpy(bn, pTracks[iTrack].boneName);
    fwrite(bn, 32, 1, f);

    DWORD dwFlags = 0;
    if(stricmp(pTracks[iTrack].boneName, "Bip01") == 0) dwFlags = dwFlags | (1L<<1);
    fwrite(&dwFlags, sizeof(DWORD), 1, f);
    
    for(int i=0; i<NumFrames; ++i) {
      track_s::frame_s &fr = pTracks[iTrack].data[i];

      float in_f[7];
      in_f[0] = fr.q.x;
      in_f[1] = fr.q.y;
      in_f[2] = fr.q.z;
      in_f[3] = fr.q.w;
      in_f[4] = fr.v.x;
      in_f[5] = fr.v.y;
      in_f[6] = fr.v.z;
      fwrite(in_f, sizeof(float), 7, f);
    }
  }
  
  fclose(f);
 
  return;
}


void r3dAnimData::BindSkeleton(r3dSkeleton *pSkeleton, r3dSkeleton::BoneRemap_s &rt)
{
  r3d_assert(bLoaded);

  for(int i=0; i<sizeof(rt.iBoneToTrack)/sizeof(rt.iBoneToTrack[0]); ++i)
    rt.iBoneToTrack[i] = -1;

  for(int i=0; i<NumTracks; ++i) {
    int boneId = pSkeleton->GetBoneID(pTracks[i].boneName);
    if(boneId == -1) {
#ifndef FINAL_BUILD
      // [semi-hack] do not show that warning for ai_player deleted bones!
      if(strstr(pTracks[i].boneName, "Nub") != NULL)
        continue;
	  if(strstr(pTracks[i].boneName, "Dummy") != NULL)
		  continue;
      r3dOutToLog("r3dAnimData(%s): unable to bind bone %s to skeleton %s\n", pAnimName, pTracks[i].boneName, pSkeleton->GetFileName() );
#endif
    } else {
      rt.iBoneToTrack[boneId] = i;
    }
  }

  return;
}

// biped control
void r3dAnimData::BipedSetEnabled(int bEnabled)
{
  r3d_assert(bLoaded);

  for(int i=0; i<NumTracks; ++i) {
    pTracks[i].bEnabled = bEnabled;
  }

  return;
}

void r3dAnimData::BipedSetEnabledGroup(int bEnabled, int group)
{
  r3d_assert(bLoaded);

  for(int j=0; j<sizeof(_BipedBones) / sizeof(_BipedBones[0]); j++) {
    if(_BipedBones[j].iGroup == group) {
      EnableTrack(_BipedBones[j].pName, bEnabled);
    }
  }

  return;
}

r3dAnimData::track_s* r3dAnimData::GetTrack(const char* pTrack)
{
  
  for(int i=0; i<NumTracks; ++i) {
    if(stricmp(pTracks[i].boneName, pTrack) == NULL) {
      return &pTracks[i];
    }
  }

  return NULL;
}

void r3dAnimData::EnableTrack(const char* pTrack, int bEnable)
{
  track_s *tr = GetTrack(pTrack);
  if(tr) tr->bEnabled = bEnable;
  return;
}

int r3dAnimData::GetTM(int trackId, float fCurFrame, r3dQuat &q, r3dPoint3D& v) const
{
  if(!bLoaded || trackId == -1) {
    return 0;
  }

  r3d_assert(trackId < NumTracks);
  track_s &tr = pTracks[trackId];
  if(!tr.bEnabled) 
    return 0;

  if(r3dAnimation_bInterpolation) {
    // interpolate between frames

    int   frame1 = (int)fCurFrame;
    int   frame2 = (int)fCurFrame + 1;
    float delta  = fCurFrame - (int)fCurFrame;
   
    if(frame1 >= NumFrames) frame1 = NumFrames - 1;
    if(frame2 >= NumFrames) frame2 = 0;
  
    // create current rotation
    r3dQuat    q1 = tr.data[frame1].q, q2 = tr.data[frame2].q;
    r3dPoint3D v1 = tr.data[frame1].v, v2 = tr.data[frame2].v;

    D3DXQuaternionSlerp(&q, &q1, &q2, delta);
    v = v1 + (v2-v1) * delta;
  } else {
    // non interpolate
    int frame1 = (int)fCurFrame;
    if(frame1 >= NumFrames) frame1 = NumFrames - 1;
  
    q = tr.data[frame1].q;
    v = tr.data[frame1].v;
  }
  
  return 1;
}

int r3dAnimData::GetTM(int trackId, float fCurFrame, D3DXMATRIX &m) const
{
  if(!bLoaded || trackId == -1) 
    return 0;

  track_s &tr = pTracks[trackId];
  if(!tr.bEnabled)
    return 0;

  r3dQuat    q;
  r3dPoint3D v; 
  GetTM(trackId, fCurFrame, q, v);
    
  D3DXMatrixRotationQuaternion(&m, &q);
  m._41 = v.x;
  m._42 = v.y;
  m._43 = v.z;

  return 1;
}

	//cr3dAnimPool
	//
	//
	//
	//
	
r3dAnimPool::r3dAnimPool()
: dupTracks(0)
{
  animDataTotalSize  = 0;
  animDataSharedSize = 0;
}

r3dAnimPool::~r3dAnimPool()
{
  Unload();
}

void r3dAnimPool::Unload()
{
  for(unsigned int i=0; i<Anims.size(); ++i) {
    Anims[i]->pAnimPool = NULL;
    delete Anims[i];
  }
  Anims.clear();
  delete dupTracks;
  dupTracks = 0;
  animDataSharedSize = 0;
  animDataTotalSize = 0;
 
  return;
}

void r3dAnimPool::Reload(const char* fname)
{
	for(unsigned int i=0; i<Anims.size(); ++i) 
	{
		const char* sn = Anims[i]->GetAnimFileName();
		if(stricmp(fname, sn) == NULL) 
		{
			Anims[i]->EDITOR_ReloadFromPool();
		}
	}
}

int r3dAnimPool::Add(const char* name, const char* fname, float fInitialAngle)
{
  if(!Anims.size()) Anims.reserve(32);
  
  r3dAnimData* ad = Get(name);
  if(ad) return ad->GetAnimID();
 
  ad = game_new r3dAnimData(r_use_shared_animtracks->GetInt() ? this : NULL);
  ad->pAnimName = strdup(name);
  // bp if cant find anim, return -1
  if(!ad->LoadBinary(fname, fInitialAngle)){
	  return -1;
  }

  ad->iAnimId = Anims.size();
  Anims.push_back(ad);

  
  return ad->iAnimId;
}

r3dAnimPool::HashTable & r3dAnimPool::GetDupTracks()
{
	if (!dupTracks)
		dupTracks = game_new HashTable;
	return *dupTracks;
}

r3dAnimData* r3dAnimPool::Get(int iId)
{
  if(iId < 0 || iId >= (int)Anims.size()) {
    #if _DEBUG
    r3dError("r3dAnimPool::Get() out of index\n");
    #endif
    return NULL;
  }
  return Anims[iId];
}

r3dAnimData* r3dAnimPool::Get(const char* name)
{
  for(unsigned int i=0; i<Anims.size(); ++i) {
	if(stricmp(name, Anims[i]->GetAnimName() ) == NULL) 
	{
		return Anims[i];
	}
  }

  return NULL;
}


	//cr3dAnimation
	//
	//
	//
	//

r3dAnimation::r3dAnimation()
{
  bInited    = 0;

  bSkelDirty = false;
  pSkeleton  = NULL;
  pAnimPool  = NULL;
}

r3dAnimation::~r3dAnimation()
{
  Unload();
  return;
}

void r3dAnimation::Unload()
{
  if(!bInited)
    return;
  
  delete pSkeleton;
  
  AnimTracks.clear();
  
  for(unsigned int i=0; i<sizeof(pBoneRemaps) / sizeof(pBoneRemaps[0]); ++i) {
    if(pBoneRemaps[i]) {
      delete pBoneRemaps[i];
      pBoneRemaps[i] = NULL;
    }
  }
  
  bInited = 0;
}

void r3dAnimation::Init(const r3dSkeleton* bindSkeleton, r3dAnimPool* anim, r3dSkeleton::fn_AdjustBoneCallback pFN, DWORD dwData)
{
	R3DPROFILE_FUNCTION("r3dAnimation::Init");
  Unload();

  pAnimPool = anim;
  for(int i=0; i<sizeof(pBoneRemaps) / sizeof(pBoneRemaps[0]); ++i) {
    pBoneRemaps[i] = NULL;
  }

  pSkeleton = bindSkeleton->Clone();
  pSkeleton->SetCallback(pFN, dwData);
  
  bInited   = 1;
  return;
}

void r3dAnimation::Stop(int iTrackId)
{
  if(!bInited) return;

  AnimTracksVec::iterator it;

  for(it = AnimTracks.begin(); it != AnimTracks.end(); ++it) {
    r3dAnimInfo &ai = *it;
    if(ai.iTrackId == iTrackId) {
      AnimTracks.erase(it);
      return;
    }
  }

  return;
}


void r3dAnimation::StopAll()
{
  if(!bInited) return;

  AnimTracks.clear();
}

/*
void r3dAnimation::Set_Speed(float speed)
{
	if(!bInited) return;
	r3dgameVector(r3dAnimInfo)::iterator it;
	for(it = AnimTracks.begin(); it != AnimTracks.end(); ++it) {
	r3dAnimInfo &ai = *it;
	if (ai.pAnim)
	 ai.pAnim->Set_Speed(speed);
	}
	return;
}
*/

r3dAnimation::r3dAnimInfo* r3dAnimation::GetTrack(int iTrackId)
{
  AnimTracksVec::iterator it;

  for(it = AnimTracks.begin(); it != AnimTracks.end(); ++it) {
    r3dAnimInfo &ai = *it;
    if(ai.iTrackId == iTrackId ) // && (ai.dwStatus & ANIMSTATUS_Playing)) 
      return &ai;
  }
  return NULL;
}

void r3dAnimation::GetActiveAnimationNames( stringlist_t& oAnims ) const
{
	AnimTracksVec::const_iterator it;
	for(it = AnimTracks.begin(); it != AnimTracks.end(); ++it) 
	{
		const r3dAnimInfo &ai = *it;

		oAnims.push_back( ai.pAnim->GetAnimName() );
	}
}

void r3dAnimation::FadeOut(int iTrackId, float fFadeTime)
{
  if(!bInited) return;

  AnimTracksVec::iterator it;

  for(it = AnimTracks.begin(); it != AnimTracks.end(); ++it) {
    r3dAnimInfo &ai = *it;
    if(ai.iTrackId == iTrackId) {
      if(ai.fInfluence < 0.01f) {
        AnimTracks.erase(it);
        return;
      }
      ai.dwStatus   |= ANIMSTATUS_Fading | ANIMSTATUS_Paused;
      ai.fFadeStart  = ai.fInfluence;
      ai.fFadeEnd    = 0.0f;
      ai.fFadeTime   = fFadeTime;
      return;
    }
  }
  
  return;
}


int r3dAnimation::StartAnimation(const char* pAnimName, DWORD flags, float fFadeStart, float fFadeEnd, float fFadeTime)
{
  if(!bInited) return 0;

  for(unsigned int i=0; i<pAnimPool->Anims.size(); ++i) {
    if(stricmp(pAnimName, pAnimPool->Anims[i]->GetAnimName()) == NULL)
      return StartAnimation(i, flags, fFadeStart, fFadeEnd, fFadeTime);
  }

  return 0;
}

int r3dAnimation::StartAnimation(int iAnimId, DWORD flags, float fFadeStart, float fFadeEnd, float fFadeTime)
{
  if(!bInited) 
    return 0;

  // bp just in case its a bad id
  if(iAnimId  < 0){
    return 0;
  }

  if(iAnimId >= (int)pAnimPool->Anims.size())
    return 0;
    
  if(iAnimId >= sizeof(pBoneRemaps) / sizeof(pBoneRemaps[0]))
    r3dError("over max number of anims in r3dAnimation");

  if(pBoneRemaps[iAnimId] == NULL) {
    pBoneRemaps[iAnimId] = game_new r3dSkeleton::BoneRemap_s;
    pAnimPool->Anims[iAnimId]->BindSkeleton(pSkeleton, *pBoneRemaps[iAnimId]);
  }
  
  static int iGlobalTrackID = 1;
  AnimTracksVec::iterator it;

  if(flags & ANIMFLAG_FadeOther) {
    for(it = AnimTracks.begin(); it != AnimTracks.end(); ++it) {
      r3dAnimInfo &ai = *it;
      ai.dwStatus   |= ANIMSTATUS_Fading | ANIMSTATUS_Paused;
      ai.fFadeStart  = ai.fInfluence;
      ai.fFadeEnd    = 0.0f;
      ai.fFadeTime   = fFadeTime;
    }
  }

  if(flags & ANIMFLAG_RemoveOtherNow) {
    for(it = AnimTracks.begin(); it != AnimTracks.end(); ++it) {
      r3dAnimInfo &ai = *it;
      ai.dwStatus &= ~ANIMSTATUS_Playing;
    }
  }

  if(flags & ANIMFLAG_RemoveOtherFade) {
    for(it = AnimTracks.begin(); it != AnimTracks.end(); ++it) {
      r3dAnimInfo &ai = *it;
      ai.dwStatus   |= ANIMSTATUS_Expiring | ANIMSTATUS_Paused;
      ai.fExpireTime = fFadeTime;
    }
  }
 
  // start & add animation  
  r3dAnimInfo ai;
  ai.dwStatus     = ANIMSTATUS_Playing | flags;
  ai.fInfluence   = 1.0f;
  ai.pAnim        = pAnimPool->Anims[iAnimId];
  ai.fFadeTime    = 0.0f;
  
  if(fabs(fFadeStart - fFadeEnd) > 0.001f) {
    ai.fFadeEnd   = fFadeEnd;
    ai.fFadeStart = fFadeStart;
    ai.fFadeTime  = fFadeTime;
    ai.dwStatus  |= ANIMSTATUS_Fading;

    ai.fInfluence = fFadeStart;
  }
    
  ai.fFPS      = ai.pAnim->fFrameRate * ai.fSpeed;
  ai.fCurFrame = 0.0f;
  ai.iTrackId  = ++iGlobalTrackID;
  
  AnimTracks.push_back(ai);

  return iGlobalTrackID;
}

void r3dAnimation::Update(float fTimePassed, const r3dPoint3D& pos, const D3DXMATRIX& mat)
{
  if(!bInited) return;
  R3DPROFILE_FUNCTION("r3dAnimation::Update");
  
  vPosition  = pos;
  mRotation  = mat;
  bSkelDirty = true;

  AnimTracksVec::iterator it;

  // remove stopped animations
  for(it = AnimTracks.begin(); it != AnimTracks.end();) 
  {
    r3dAnimInfo &ai = *it;
    if(!(ai.dwStatus & ANIMSTATUS_Playing)) {
      it = AnimTracks.erase(it);
      continue;
    }
    ++it;
  }
  
  if(fTimePassed < 0.0001f) return;
  
  // process all remaining animations   
  for(it = AnimTracks.begin(); it != AnimTracks.end(); ++it) 
  {
    r3dAnimInfo &ai = *it;
    if(!(ai.dwStatus & ANIMSTATUS_Playing)) 
      continue;

    if(!(ai.dwStatus & ANIMSTATUS_Paused)) 
    {
        // advance frame
        ai.fFPS       = ai.pAnim->fFrameRate * ai.fSpeed;
        ai.fCurFrame += fTimePassed * ai.fFPS;

        if( ai.dwStatus & ANIMFLAG_Looped )
        {
#if 1
          //ANIM_HACK: on Patrick's looping animations The last frame is the same as first frame
          if( (int)ai.fCurFrame >= ai.pAnim->NumFrames - 1 ) 
          {
            ai.fCurFrame -= ai.pAnim->NumFrames-1;
			ai.fCurFrame += ai.fStartFrame;
          }
#else
          if( (int)ai.fCurFrame >= ai.pAnim->NumFrames ) 
          {
            ai.fCurFrame -= ai.pAnim->NumFrames;
          }
#endif
        }
        else
        {
          if(ai.fCurFrame > (float)(ai.pAnim->NumFrames-1)) {
            if(ai.dwStatus & ANIMFLAG_PauseOnEnd) {
                // stop at animation end
                ai.fCurFrame  = (float)(ai.pAnim->NumFrames - 1);
                ai.dwStatus  |= ANIMSTATUS_Finished;
            } else if(ai.fFadeTime > 0.001f) {
                // fade it out
                ai.fCurFrame   = (float)(ai.pAnim->NumFrames - 1);
                ai.dwStatus   |= ANIMSTATUS_Fading | ANIMSTATUS_Paused;
                ai.fFadeStart  = ai.fInfluence;
                ai.fFadeEnd    = 0.0f;
                //ai.fFadeTime stay same
            } else {
                // just temove it
                ai.dwStatus &= ~ANIMSTATUS_Playing;
                continue;
            }
          }
        }
    }

    // just in case frame was out of range, adjust it
    if(ai.fCurFrame >= ai.pAnim->NumFrames) 
      ai.fCurFrame -= (float)(ai.pAnim->NumFrames * (int)(floorf(ai.fCurFrame) / (float)ai.pAnim->NumFrames));
    if(ai.fCurFrame < 0)
      ai.fCurFrame = 0.0f;
    
    // advance influence    
    if(ai.dwStatus & ANIMSTATUS_Fading) {
      if(ai.fFadeStart <= ai.fFadeEnd) {
        ai.fInfluence += fTimePassed * (1.0f / ai.fFadeTime);
        if(ai.fInfluence >= ai.fFadeEnd) {
          ai.fInfluence = ai.fFadeEnd;
          ai.dwStatus &= ~ANIMSTATUS_Fading;
        }
      } else {
        ai.fInfluence -= fTimePassed * (1.0f / ai.fFadeTime);
        if(ai.fInfluence <= ai.fFadeEnd) {
          ai.fInfluence = ai.fFadeEnd;
          ai.dwStatus &= ~ANIMSTATUS_Fading;
        }
        if(ai.fInfluence < 0.01f) {
          ai.dwStatus &= ~ANIMSTATUS_Playing;
          continue;
        }
      }
    }
    
    // remove when expired
    if(ai.dwStatus & ANIMSTATUS_Expiring) {
      ai.fExpireTime -= fTimePassed;
      if(ai.fExpireTime < 0.0f) {
        ai.dwStatus &= ~ANIMSTATUS_Playing;
        continue;
      }
    }
  }
  
  return;
}

void r3dAnimation::Recalc()
{
  if(!bInited) 
    return;
  
  if(!bSkelDirty)
    return; // there is no point of recalcing if skeleton isn't changed

  R3DPROFILE_FUNCTION("r3dAnimation::Recalc");
  pSkeleton->ResetTransform();
  
  int bDisableRootMove = 0;
  
  AnimTracksVec::iterator it;
  for(it = AnimTracks.begin(); it != AnimTracks.end(); ++it) 
  {
    r3dAnimInfo &ai = *it;
    if(!(ai.dwStatus & ANIMSTATUS_Playing)) 
      continue;

    // apply animation
    pSkeleton->Apply(ai.pAnim, *pBoneRemaps[ai.pAnim->iAnimId], ai.fCurFrame, ai.fInfluence);
    if(ai.pAnim->bDisableRootMove) bDisableRootMove = 1;
  }
  
  D3DXMATRIX mBase;
  CalcBasePose(mBase);
  pSkeleton->Recalc(&mBase, bDisableRootMove);
  
  bSkelDirty = false;
  
  return;
}

r3dSkeleton* r3dAnimation::GetCurrentSkeleton()
{
  r3d_assert(bInited);
  r3d_assert(pSkeleton);
  
  if(bSkelDirty) {
    Recalc();
  }
  
  return pSkeleton;
}

r3dSkeleton* r3dAnimation::GetCurrentSkeletonNoUpdate()
{
	r3d_assert(bInited);
	r3d_assert(pSkeleton);

	 return pSkeleton;
}

//////////////////////////////////////////////////////////////////////////

void r3dAnimation::CalcBasePose(D3DXMATRIX &outMat) const
{
	outMat = mRotation;
	outMat._41 = vPosition.X;
	outMat._42 = vPosition.Y;
	outMat._43 = vPosition.Z;
}