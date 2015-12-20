#include "r3dPCH.h"
#include "r3d.h"

#include "r3dSkin.h"


r3dSkinMesh::r3dSkinMesh()
{
  Reset();
}

r3dSkinMesh::~r3dSkinMesh()
{
  Unload();
}


void r3dSkinMesh::Reset()
{
  bLoaded       	= 0;

  pMesh         	= NULL;
  VArray        	= NULL;
  pSkinMaterial  	= NULL;
  pBindSkeleton 	= NULL;
  SkinScale		= 1;
  SelfIllumination = 0;
}


void r3dSkinMesh::Unload()
{
  if(!bLoaded)  
    return; 

  delete[]	VArray;
  delete	pMesh;
  delete	pBindSkeleton;
  delete		pSkinMaterial;
  
  Reset();
  return;
}


void r3dSkinMesh::LoadINI(const char* base_dir, const char* ini_fname)
{
  if(bLoaded) 
    Unload();

  char fname[MAX_PATH];
  sprintf(fname, "%s\\%s.ini", base_dir, ini_fname);

  // skeleton
  char buf[MAX_PATH];
  sprintf(buf, "%s\\%s", base_dir, r3dReadCFG_S(fname, "MeshSkin", "Skeleton", "_skeleton"));
  pBindSkeleton = game_new r3dSkeleton;
  pBindSkeleton->LoadBinary(buf);

  // load mesh with it weights
  {
    extern int r3dMeshObject_SkipFillBuffers;
    r3dMeshObject_SkipFillBuffers = 1;

    sprintf(buf, "%s\\%s", base_dir, r3dReadCFG_S(fname, "MeshSkin", "Body", "_mesh"));
    LoadMesh(buf);

	// we do not support custom weight files anymore *sob*
#if 0
    sprintf(buf, "%s\\%s", base_dir, r3dReadCFG_S(fname, "MeshSkin", "Weight", "_weight"));
    LoadWeights(buf);
#endif

	r3dMeshObject_SkipFillBuffers = 0;
  }
  
  // load skin texture
  sprintf(buf, "%s\\%s", base_dir, r3dReadCFG_S(fname, "MeshSkin", "Texture", "-NONE-"));
  if (strstr(buf,"-NONE-"))
       sprintf(buf, "Data\\Shaders\\Texture\\checker.dds");

  {
	  r3dMaterial *M = gfx_new r3dMaterial( 0 );

	  M->Texture			= r3dRenderer->LoadThumbnailTexture(buf, 1, 0, D3DPOOL_MANAGED, TexMem );
	  M->GlossTexture		= r3dRenderer->LoadTexture(buf);
	  M->SpecularPower = 0.4f;

	  pSkinMaterial =  M ;
  }

  // create vertex buffers for mesh
  if(pMesh->NumMatChunks) pMesh->MatChunks[0].Mat = pSkinMaterial;
  pMesh->FillBuffers();


  SkinScale = r3dReadCFG_F(fname, "MeshSkin", "SkinScale", 1.0f);
  SelfIllumination = r3dReadCFG_F(fname, "MeshSkin", "SelfIllumination", 0.0f);

  //! minor hack
  //FIXME
  r3dscpy(Bone1, "Bone01");
  if(pBindSkeleton->GetBoneID("PrimaryWeapon") != -1)
    r3dscpy(Bone1, "PrimaryWeapon");
  if(pBindSkeleton->GetBoneID("Weapon1") != -1)
    r3dscpy(Bone1, "Weapon1");

    if(pBindSkeleton->GetBoneID("Bone_Weapon") != -1)
    r3dscpy(Bone1, "Bone_Weapon");

  bLoaded = 1;
  return;
}

void r3dSkinMesh::LoadMesh(const char* fname)
{
  assert(!pMesh);

  pMesh = gfx_new r3dMesh( 0 );

  pMesh->Load(fname);

  return;
};

/*
void r3dSkinMesh::DeformMesh(r3dSkeleton *Skel)
{
  if(!bLoaded) return;

  for(int i=0;i<iNumVertices;i++)
  {
    r3dPoint3D PP = r3dPoint3D(0,0,0);

    for(int k=0; k<4; k++)
    {
      if(VWeights[i].Weight[k] < 0.00001f) 
        continue;

      D3DXVECTOR3 V3 = D3DXVECTOR3(VArray[i].X, VArray[i].Y, VArray[i].Z);
      D3DXVECTOR3 V4;

      r3dBone *Bone = &Skel->Bones[VWeights[i].BoneID[k]];

      D3DXVec3TransformCoord(&V4, &V3, &Bone->CurrentTM);
 
      r3dPoint3D P;
      P.X = V4.x * VWeights[i].Weight[k];
      P.Y = V4.y * VWeights[i].Weight[k];
      P.Z = V4.z * VWeights[i].Weight[k];

      PP += P;
    }

    pMesh->VertexPositions[i] = PP;
  }  

  pMesh->FillBuffers();
  
  return;
}
*/

void r3dSkinMesh::DrawShader(r3dSkeleton *Skel, int SID)
{
  if(!bLoaded) return;

  Skel->SetShaderConstants();
  
  r3dPoint3D Pos = Skel->GetRootPos();

  r3dVector VV = r3dRenderer->CameraPosition - Pos;
  int Idx = int(VV.Length()/200.0f);
  if (Idx > 2 ) Idx = 2;

  D3DXMATRIX world; D3DXMatrixIdentity(&world);
  pMesh->SetShaderConsts(world);
  pMesh->DrawMeshDeferred( r3dColor::white, 0 );
  
  return;
}

void r3dSkinMesh::DrawSMPass(r3dSkeleton *Skel)
{
  if(!bLoaded) return;

  Skel->SetShaderConstants();

  D3DXMATRIX world; D3DXMatrixIdentity(&world);
  pMesh->SetShaderConsts(world);
  pMesh->DrawMeshShadows();

  return;
}
