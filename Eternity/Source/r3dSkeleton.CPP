#include "r3dPCH.h"
#include "r3d.h"

#include "r3dSkeleton.h"
#include "r3dAnimation.h"

extern void r3dDumpSkeleton(const r3dSkeleton* skel, int bone);

r3dSkeleton::r3dSkeleton()
{ 
  Bones     = NULL;
  BoneNames = NULL;
  NumBones  = 0;
  bLoaded   = 0;
  
  pAdjustBoneCallback = NULL;
}

r3dSkeleton::~r3dSkeleton() 
{ 
  Unload(); 
}

void r3dSkeleton::Unload()
{
  if(!bLoaded) 
    return;

    delete[] BoneNames;
    free(pFileName);

  delete[] Bones;
  Bones     = NULL;
  BoneNames = NULL;
  NumBones  = 0;
  bLoaded   = 0;
  return;
}

r3dSkeleton *r3dSkeleton::Clone() const
{
  r3dSkeleton *s = game_new r3dSkeleton;

  s->pFileName = strdup(pFileName);
  s->bLoaded   = 1;
  s->NumBones  = NumBones;
  s->Bones     = game_new r3dBone[NumBones];

  int l = NumBones * R3D_BONENAME_LEN;
  s->BoneNames = new char[l];

  memcpy_s(s->BoneNames, l, BoneNames, l);

  if (NumBones > 0)
  {
	  for(int i=0; i<NumBones; i++)
	  {
		  ptrdiff_t nameDiff = Bones[i].Name - BoneNames;
		  s->Bones[i] = Bones[i];
		  s->Bones[i].Name = s->BoneNames + nameDiff;
	  }
  }
  
  return s;
}
	
void r3dSkeleton::LoadBinary(const char* fname)
{
  if(bLoaded) 
    Unload();

  r3dFile *f = r3d_open(fname, "rb");
  if(!f)
    r3dError("can't open %s", fname); 

  pFileName = strdup(fname);

  binhdr_s hdr;
  if(fread(&hdr, sizeof(hdr), 1, f) != 1) r3dError("%s - can't read header from", fname);
  if(hdr.r3dID != R3D_BINARY_FILE_ID)     r3dError("%s - invalid header", fname);
  if(hdr.ID != R3D_SKELETON_BINARY_ID)	  r3dError("%s - not an skeleton file", fname);

  switch(hdr.Version) {
    default:
      r3dError("%s - unsupported version", fname);
      break;
      
    case 0x00000001:
      LoadBinaryV1(f);
      break;
  }

  fclose(f);
  
  //r3dDumpSkeleton(this, -1);
  
  return;
};

void r3dSkeleton::LoadBinaryV1(r3dFile *f)
{
  DWORD dw1, dw2;
  
  fread(&dw1, sizeof(DWORD), 1, f);
  fread(&dw2, sizeof(DWORD), 1, f);
  
  dwSkeletonId = dw1;
  NumBones     = dw2;
  
  Bones     = game_new r3dBone[NumBones];
  BoneNames = game_new char[NumBones * R3D_BONENAME_LEN];
  
  for(int i=0; i<NumBones; i++) {
    r3dBone &b  = Bones[i];

    b.iBoneId = i ;

    // read name
    b.Name = BoneNames + i * R3D_BONENAME_LEN;
    fread(b.Name, 32, 1, f);
    // read parentId
    fread(&b.iParentId, sizeof(DWORD), 1, f);
    // read bone length
    fread(&b.fLength, sizeof(float), 1, f);
    b.fCollisionRadius = 2.1f;

    // read matrix    
    D3DXMATRIX 	&m = b.mAbsPlacement;
    fread(&m._11, sizeof(float), 1, f);
    fread(&m._21, sizeof(float), 1, f);
    fread(&m._31, sizeof(float), 1, f);
    fread(&m._41, sizeof(float), 1, f);
    fread(&m._12, sizeof(float), 1, f);
    fread(&m._22, sizeof(float), 1, f);
    fread(&m._32, sizeof(float), 1, f);
    fread(&m._42, sizeof(float), 1, f);
    fread(&m._13, sizeof(float), 1, f);
    fread(&m._23, sizeof(float), 1, f);
    fread(&m._33, sizeof(float), 1, f);
    fread(&m._43, sizeof(float), 1, f);
    m._14 = 0;
    m._24 = 0;
    m._34 = 0;
    m._44 = 1;
  }
  
  bLoaded = 1;
  SetDefaultPose(NULL);
  
  return;
}

void r3dSkeleton::SetDefaultPose(r3dAnimData *ad)
{
  if(!bLoaded) return;
  
  // calculate relative placement
  for(int i=0; i<NumBones; i++) {
    r3dBone &b  = Bones[i];
    D3DXMATRIX mpi; // inverse parent matrix
    if(b.iParentId == -1) D3DXMatrixIdentity(&mpi);
    else                  D3DXMatrixInverse(&mpi, NULL, &Bones[b.iParentId].mAbsPlacement);
    D3DXMATRIX mr;  // relative matrix
    mr = b.mAbsPlacement * mpi;
      
    D3DXVECTOR3 s;
    D3DXMatrixDecompose(&s, &b.qRelPlacement, b.vRelPlacement.d3dx(), &mr);
  }
    
  if(ad) {

    r3dSkeleton::BoneRemap_s rt;
    ad->BindSkeleton(this, rt);

    // pose from animation
    for(int i=0; i<NumBones; i++) {
      r3dBone &b  = Bones[i];
      if(rt.iBoneToTrack[i] == -1) continue;

      ad->GetTM(rt.iBoneToTrack[i], 0, b.qRelPlacement, b.vRelPlacement);
    }
  }

  return;
}

void r3dSkeleton::SetCallback(fn_AdjustBoneCallback pFN, DWORD dwData)
{
  pAdjustBoneCallback = pFN;
  dwCallbackData1     = dwData;
  return;
}


void r3dSkeleton::ResetTransform()
{
  if(!bLoaded) return;

  for(int i=0; i<NumBones; i++) {
    r3dBone &b   = Bones[i];
    b.qCur = b.qRelPlacement;
    b.vCur = b.vRelPlacement;
  }
  
  return;
}

void r3dSkeleton::Apply(const r3dAnimData* pAnim, r3dSkeleton::BoneRemap_s &rt, float fCurFrame, float fInfluence)
{
  if(!bLoaded) return;
  
  for(int i=0; i<NumBones; i++) {
    r3dBone &b   = Bones[i];
    if(rt.iBoneToTrack[i] == -1) continue;
    if(fInfluence < 0.01f) continue;

    r3dQuat    q1;
    r3dPoint3D v1;
    if(!pAnim->GetTM(rt.iBoneToTrack[i], fCurFrame, q1, v1))
      continue;

    if(fInfluence > 0.98f) {
      b.qCur = q1;
      b.vCur = v1;
      continue;
    }

    D3DXQuaternionSlerp(&b.qCur, &b.qCur, &q1, fInfluence);
    b.vCur = b.vCur + (v1 - b.vCur) * fInfluence;
  }  

  return;
}

//////////////////////////////////////////////////////////////////////////

void r3dSkeleton::Recalc(D3DXMATRIX *mBase /* = NULL */, int bDisableRootMove /* = 0 */, bool *boneSkipArr /* = 0 */)
{
  if(!bLoaded) return;

  // calc hierarchy
  for (int i = 0; i < NumBones; i++)
  {
    if (boneSkipArr && boneSkipArr[i])
      continue;

    r3dBone& b = Bones[i];

    D3DXMATRIX mp;
    if(b.iParentId == -1) {
      if(mBase) mp = *mBase;
      else      D3DXMatrixIdentity(&mp);
    } else {
      mp = Bones[b.iParentId].CurrentTM;
    }

    // create matrix from calculated animation
    D3DXMATRIX anim;
    D3DXMatrixRotationQuaternion(&anim, &b.qCur);
    anim._41 = b.vCur.x;
    anim._42 = b.vCur.y;
    anim._43 = b.vCur.z;

    // disable movement of root bone
    if(bDisableRootMove && b.iParentId == -1) {
      anim._41  = 0;
      anim._43  = 0;
    }

    if(pAdjustBoneCallback)
      pAdjustBoneCallback(dwCallbackData1, i, mp, anim);

    D3DXMatrixMultiply(&b.CurrentTM, &anim, &mp);
    b.mBonePlacement = b.CurrentTM;
  }


  // to object space
  for(int i=0; i<NumBones; i++) {
    r3dBone &b   = Bones[i];
    
    D3DXMATRIX mInvert = b.mAbsPlacement;
    D3DXMatrixInverse(&mInvert, 0, &b.mAbsPlacement);
    D3DXMatrixMultiply(&b.CurrentTM, &mInvert, &b.CurrentTM);
  }
}

int r3dSkeleton::GetBoneID(const char* name) const
{
  if(!bLoaded) 
    return -1;

  for(int i=0; i<NumBones; i++)
    if(stricmp(Bones[i].Name, name) == NULL)
      return i;
    
  return -1;
}

void r3dSkeleton::GetBoneWorldTM( int iBoneId, D3DXMATRIX *m, const D3DXMATRIX& offset ) const
{
  if(iBoneId == -1) {
    D3DXMatrixIdentity( m );
    return;
  }
  
  *m = Bones[iBoneId].mBonePlacement;

  D3DXMatrixMultiply( m, m, &offset ) ;

  return;
}

//////////////////////////////////////////////////////////////////////////

void r3dSkeleton::SetBoneWorldTM(int iBoneId, D3DXMATRIX *m)
{
	if(iBoneId == -1)
		return;

	r3dBone &b = Bones[iBoneId];

	b.CurrentTM = *m;
	b.mBonePlacement = *m;
}


//////////////////////////////////////////////////////////////////////////

void r3dDrawCylinder(const r3dPoint3D& vStart, const r3dPoint3D& vRay, float fHeight, float fRadius, r3dColor clr1)
{
  #define CYL_STEPS 10

  r3dPoint3D fv[CYL_STEPS*2+1];

  r3dRenderer->SetMaterial(NULL);
  r3dRenderer->SetTex(NULL);
  r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC);

  r3dRenderer->SetCullMode( D3DCULL_NONE );
  
  // make vector perpendicular to the ray
  r3dVector n;
  if(vRay.Y > 0.99f || vRay.Y < 0.99f) n = r3dVector(1, 0, 0).Cross(vRay);
  else                                 n = r3dVector(0, 1, 0).Cross(vRay);
  n.Normalize();

  // rotate point around that ray
  for(int i=0; i<CYL_STEPS; i++) {
    r3dPoint3D v1 = n * fRadius;
    v1.RotateAroundVector(vRay, 360.0f / (float)CYL_STEPS * (float)i);
    fv[i*2+0] = vStart + v1;
    fv[i*2+1] = vStart + v1 + vRay * fHeight;
  }

  for(int i=0; i<CYL_STEPS; i++) {
    R3D_DEBUG_VERTEX V[4];
    V[0].color = clr1;
    V[1].color = clr1;
    V[2].color = clr1;
    V[3].color = clr1;
    V[0].tu   = 0; V[0].tv = 0;
    V[1].tu   = 1; V[1].tv = 0;
    V[2].tu   = 1; V[2].tv = 1;
    V[3].tu   = 0; V[3].tv = 1;
    V[0].Pos  = fv[i*2+0];
    V[1].Pos  = fv[i*2+1];
    V[2].Pos  = fv[((i+1)%CYL_STEPS)*2+1];
    V[3].Pos  = fv[((i+1)%CYL_STEPS)*2+0];
    R3D_DEBUG_VERTEX::ComputeNormals(V, 4);
    r3dRenderer->Render3DPolygon(4, V);
  }

  r3dRenderer->Flush();

  r3dRenderer->RestoreCullMode();

  return;
  #undef CYL_STEPS
}

int r3dRayVsCylinder(const r3dPoint3D& raybase, const r3dPoint3D& raycos, const r3dPoint3D& base, const r3dPoint3D& axis, float radius, float *in, float *out)
{
    bool        hit;        /* True if ray intersects cyl   */
    r3dPoint3D  RC;     /* Ray base to cylinder base    */
    float       d;      /* Shortest distance between    */
                    /*   the ray and the cylinder   */
    float       t, s;       /* Distances along the ray  */
    r3dPoint3D  n, D, O;
    float       ln;
    const   float       pinf = 1.0e21f; /* Positive infinity        */


    RC.x = raybase.x - base.x;
    RC.y = raybase.y - base.y;
    RC.z = raybase.z - base.z;
    n = raycos.Cross(axis);

    if( (ln = n.Length()) <= R3D_EPSILON ) {    /* ray parallel to cyl  */
        d    = RC.Dot(axis);
        D.x  = RC.x - d*axis.x;
        D.y  = RC.y - d*axis.y;
        D.z  = RC.z - d*axis.z;
        d    = D.Length();
        *in  = -pinf;
        *out =  pinf;
        return (d <= radius);       /* true if ray is in cyl*/
    }

    n.Normalize();
    d    = fabs (RC.Dot(n));        /* shortest distance    */
    hit  = (d <= radius);

    if(hit) {               /* if ray hits cylinder */
        O = RC.Cross(axis);
        t = -O.Dot(n) / ln;
        O = n.Cross(axis);
        O.Normalize();
        s = fabs (sqrt(radius*radius - d*d) / raycos.Dot(O));
        *in  = t - s;           /* entering distance    */
        *out = t + s;           /* exiting  distance    */
    }

    return (hit);
}

// simulate collision with capsules around the bones
const r3dBone* r3dSkeleton::CheckCollision(const r3dPoint3D& vRayOrg, const r3dPoint3D& vRayDir, float fLength) const
{
  r3dSegment3 segRay;
  segRay.Origin    = vRayOrg + vRayDir * fLength / 2.0f;
  segRay.Direction = vRayDir;
  segRay.Extent    = fLength / 2.0f;
  
  r3dSegment3 segBone;
  
  int   iColBone = -1;
  float fMinDist = 9999999.9f;
  
  for(int iBone=0; iBone<NumBones; iBone++) {
    r3dBone &b = Bones[iBone];
    //float fLen = R3D_MAX(b.fLength, 1.0f);
    float fLen = b.fLength;
    b.fCollisionRadius = 2.0;
    if(b.fCollisionRadius < 0.001f) continue;
    
    r3dPoint3D v1(b.mBonePlacement._41, b.mBonePlacement._42, b.mBonePlacement._43);
    r3dPoint3D v3(b.mBonePlacement._11, b.mBonePlacement._12, b.mBonePlacement._13);

    segBone.Origin    = v1 + v3 * (fLen / 2.0f);
    segBone.Direction = v3;
    segBone.Extent    = fLen / 2.0f;
    
    segRay.Direction.Normalize();
    segBone.Direction.Normalize();
    
    float fDist = r3dDistSegment3Segment3_GetSquared(segRay, segBone);
    if(fDist > 0 && fDist <= b.fCollisionRadius * b.fCollisionRadius) {
      //return &Bones[iBone];
      if(fMinDist > fDist) {
        fMinDist = fDist;
        iColBone = iBone;
      }
    }
  }
  
  if(iColBone != -1) 
    return &Bones[iColBone];
  
  return NULL;
}

void r3dSkeleton::DrawSkeleton(const r3dCamera& Cam, const r3dPoint3D& off)
{
  R3DPROFILE_FUNCTION("r3dSkeleton::DrawSkeleton");
  if(!bLoaded) 
    return;

  int alpha = 255;
  int CCol  = 0;
  r3dColor Col[] = { r3dColor(255, 155, 0, alpha), r3dColor(200, 0, 255, alpha), r3dColor(255, 0, 200, alpha) };
  float w1 = 0.01f;
  float w2 = 0.025f;

#if 1
  // drawing actual bone position/locations 
  for(int i=0;i<NumBones;i++) 
  {
    r3dBone &b = Bones[i];

    r3dPoint3D v1(b.mBonePlacement._41, b.mBonePlacement._42, b.mBonePlacement._43);
    v1 += off;
    
    r3dColor cc = Col[CCol];
    if(b.iParentId == -1) {
      float sz = 0.2f;
      r3dDrawLine3D(v1 - r3dVector(0,sz,0), v1 + r3dVector(0,sz,0), Cam, w2, r3dColor(255,0,0,alpha));
      r3dDrawLine3D(v1 - r3dVector(sz,0,0), v1 + r3dVector(sz,0,0), Cam, w2, r3dColor(0,0,255,alpha));
      r3dDrawLine3D(v1 - r3dVector(0,0,sz), v1 + r3dVector(0,0,sz), Cam, w2, r3dColor(0,255,0,alpha));
    } else {
      r3dPoint3D v2;
      v2.X = Bones[b.iParentId].mBonePlacement._41;
      v2.Y = Bones[b.iParentId].mBonePlacement._42;
      v2.Z = Bones[b.iParentId].mBonePlacement._43;
      v2 += off;
      r3dDrawLine3D(v1, v2, Cam, w1, Col[CCol]);
      //cc.A = 60;
      //r3dDrawCylinder(v1, v2, fLen, b.fCollisionRadius, cc);
    }

	++CCol;
    CCol = CCol % 2;
  }

#else

  for(int i=0;i<NumBones;i++) {
    r3dBone &b = Bones[i];

    r3dPoint3D v1, v2, v3;
    v1.X = b.mBonePlacement._41;
    v1.Y = b.mBonePlacement._42;
    v1.Z = b.mBonePlacement._43;

    if(b.iParentId != -1) {
      v2.X = Bones[b.iParentId].mBonePlacement._41;
      v2.Y = Bones[b.iParentId].mBonePlacement._42;
      v2.Z = Bones[b.iParentId].mBonePlacement._43;

      r3dDrawLine3D(v1, v2, Cam, 0.1f, Col[CCol]);
    } else {
      float Sz = 2.0f;
      r3dDrawLine3D(v1 - r3dVector(0,Sz,0), v1 + r3dVector(0,Sz,0), Cam, 0.1f, r3dColor(255,0,0,alpha));
      r3dDrawLine3D(v1 - r3dVector(Sz,0,0), v1 + r3dVector(Sz,0,0), Cam, 0.1f, r3dColor(0,0,255,alpha));
      r3dDrawLine3D(v1 - r3dVector(0,0,Sz), v1 + r3dVector(0,0,Sz), Cam, 0.1f, r3dColor(0,255,0,alpha));
    }

    CCol = 1 - CCol;
  }
#endif
  
  r3dRenderer->Flush();
  r3dRenderer->RestoreCullMode();

  return;
}

void r3dSkeleton::SetShaderConstants() const
{
  assert(bLoaded);
  if(!bLoaded) return;

  float Mat[12*128];
  float *MM = Mat;

  //r3dOutToLog("NumBones is %d\n", NumBones);

  COMPILE_ASSERT( MAX_BONES <= R3D_ARRAYSIZE( Mat ) / 12 );

  r3d_assert(NumBones < MAX_BONES);
  for(int i=0; i<NumBones; i++) {
    const r3dBone& b = Bones[i];
    D3DXMATRIX mat2;
    D3DXMatrixTranspose(&mat2, &b.CurrentTM);
    memcpy(MM, &mat2, 12*sizeof(float));
    MM += 12;
  }

  r3dRenderer->pd3ddev->SetVertexShaderConstantF(START_BONE_CONSTANT, Mat, 3*NumBones);
  return;
}

/*static*/ void r3dSkeleton::SetDefaultShaderConstants() 
{
    float idents[ ( MAX_BONES + 1 ) * 12 ];

    for( int i = 0, e = MAX_BONES ; i < e; i ++ )
    {
        D3DXMatrixIdentity( (D3DXMATRIX*) &idents[ i * 12 ] );
    }

    r3dRenderer->pd3ddev->SetVertexShaderConstantF(START_BONE_CONSTANT, (float*)idents, 3*MAX_BONES);
}

void r3dDumpSkeleton(const r3dSkeleton* skel, int bone)
{
    if(bone == -1) {
        r3dOutToLog( "ROOT\n" );
    } else {
        r3d_assert( bone >= 0 && bone < skel->GetNumBones() );
        r3dOutToLog( "%s\n", skel->GetBoneName( bone ) );
    }
    CLOG_INDENT;

    for(int i=0; i<skel->GetNumBones(); i++) 
    {
        if( skel->GetParentBoneId( i ) == bone )
        {
            r3dDumpSkeleton( skel, i );
        }
    }

    return;
}
