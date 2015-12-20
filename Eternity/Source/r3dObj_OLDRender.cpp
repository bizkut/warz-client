#include	"r3dPCH.h"
#include	"r3d.h"


//#define _FORCE_XFORM

int r3dMeshObject_SkipFillBuffers = 0;
//static short _VNArray[70000];
static short _VNArray[1000000]; // BP bump up to 500K

extern	std::map<DWORD, std::string> _r3d_mVBufferMap;


//-----------------------------------------------------------------------
r3dMesh::r3dMesh()
//-----------------------------------------------------------------------
{
  D3DXMatrixIdentity(&RotateMatrix);

  NumVertices     = 0;
  WorldVertexList = NULL;

  g_pVB        	  = NULL;
  g_IB        	  = NULL;
  bUseWorldMatrix	= 0;
  
  m_pTransformedObj = NULL;
  D3DXMatrixIdentity(&m_CachedTransform);

  MatNamesArray   = NULL;

  NumFaces 	  = 0;
  FaceList 	  = NULL;
  bHaveVertexNormals  = 0;
  VertexNormals = NULL;
  vU = NULL;
  vV = NULL;
  //vUV = NULL;

  bHaveEdges	= 0;
  NumEdges	= 0;

  DynaLights	  = NULL;
  LightmapTex	  = NULL;

  bSimulateGI	  = 0;
  AmbientOcclusionTex = NULL;

  NumMatChunks	  = 0;

  vRotation = r3dPoint3D(0, 0, 0);
  vMove     = r3dPoint3D(0, 0, 0);
  vScale    = r3dPoint3D(1, 1, 1);
  vPivot    = r3dPoint3D(0, 0, 0);


  bCastShadows 	= 0;

  Flags = 0;
  SetValid(FALSE);

  VBMode    = 0;
  isDynamic = 0;
}

//-----------------------------------------------------------------------
r3dMesh::~r3dMesh()
//-----------------------------------------------------------------------
{
  Unload();
}

//-----------------------------------------------------------------------
void r3dMesh::Unload()
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

  SAFE_DELETE_ARRAY(WorldVertexList);
  SAFE_DELETE_ARRAY(FaceList);
  SAFE_DELETE_ARRAY(MatNamesArray);

  SAFE_DELETE_ARRAY(vU);
  SAFE_DELETE_ARRAY(vV);
  //SAFE_DELETE_ARRAY(vUV);

  SAFE_DELETE(m_pTransformedObj);

  if(bHaveVertexNormals)
  {
    SAFE_DELETE_ARRAY(VertexNormals);
    bHaveVertexNormals = 0;
  }

  if(g_pVB != NULL) {
    #ifdef _DEBUG
    _r3d_mVBufferMap.erase((DWORD)g_pVB);
    #endif

    g_pVB->Release();
    g_pVB = NULL;

    r3dRenderer->Stats.BufferMem -= NumFaces * 3 * sizeof(R3D_WORLD_VERTEX);
  }
        
  if(g_IB != NULL) {
    g_IB->Release();
    g_IB = NULL;
  }

 
  NumFaces 	 = 0;
  NumVertices   = 0;
  NumMatChunks  = 0;
    
  if(LightmapTex)
    r3dRenderer->DeleteTexture(LightmapTex);

  if(AmbientOcclusionTex)
    r3dRenderer->DeleteTexture(AmbientOcclusionTex);


  vRotation.Assign(0,0,0);
  vMove.Assign(0,0,0);
  vScale.Assign(1,1,1);
  Flags = 0;

  bCastShadows 	= 0;
   
  SetValid(FALSE);
  return;
}



int r3dMesh::LoadLightmapUV(const char* dirname)
{
 float *UV;

 LightmapTex = NULL;

 char Str1[512];
 sprintf(Str1,"%s\\%s.sco_uv2", dirname, Name);
 if(r3d_access(Str1, 0) == 0) 
 {
   r3dFile *f = r3d_open(Str1, "rt");
   if(!f) r3dError("can't open %f\n", Str1);

  UV = new float[3*2*NumFaces];

  for (int i=0;i<NumFaces;i++)
  {
   float u[3], v[3];

    fgets(Str1, 500, f);
    sscanf(Str1, "%f %f %f %f %f %f", &u[0], &v[0], &u[1], &v[1], &u[2], &v[2]);
    UV[i*6+0] = u[0];
    UV[i*6+1] = v[0];
    UV[i*6+2] = u[1];
    UV[i*6+3] = v[1];
    UV[i*6+4] = u[2];
    UV[i*6+5] = v[2];
  }
  fclose(f);  

  char TLN[4][256];

  sprintf(TLN[0],"%s\\%s_LM.dds", dirname, Name);
  sprintf(TLN[1],"%s\\%s_LM.tga", dirname, Name);

  for (int i=0;i<2;i++)
  {
   f = r3d_open(TLN[i],"rb");
   if ( f )
   {
    fclose(f);
   
    LightmapTex = r3dRenderer->LoadTexture(TLN[i], D3DFMT_A8R8G8B8, 0);
    break;
   }
  }

  sprintf(TLN[0],"%s\\%s_AO.dds", dirname, Name);
  sprintf(TLN[1],"%s\\%s_AO.bmp", dirname, Name);

  for (int i=0;i<2;i++)
  {
   f = r3d_open(TLN[i],"rb");
   if ( f )
   {
    fclose(f);
   
    AmbientOcclusionTex = r3dRenderer->LoadTexture(TLN[i], D3DFMT_A8R8G8B8, 0);
    bSimulateGI = 1;
    break;
   }
  }


  for (int i=0;i<NumFaces;i++)
   for(int k=0; k<3; k++)
   {
    FaceList[i].TX2[k] = UV[FaceList[i].IDN * 6 +2*k];
    FaceList[i].TY2[k] = UV[FaceList[i].IDN * 6 +2*k+1];
   } 

  FillBuffers();
  delete [] UV;

  return 1;
 }

 return 0;
}



int r3dMesh::LoadLightmapUV(const char* dirname, const char* ScoName, const char* TexName)
{
 r3dMesh* LO;

 LightmapTex = NULL;

 char Str1[512];
 sprintf(Str1,"%s\\%s", dirname, ScoName);

 r3dFile *f = NULL;
 
 f = r3d_open(Str1,"rt");

 if ( f )
 {
  fclose(f);

  LO = new r3dMesh;
  LO->LoadAscii(Str1);

  sprintf(Str1,"%s\\%s", dirname, TexName);
  LightmapTex = r3dRenderer->LoadTexture(Str1, D3DFMT_A8R8G8B8, 0);

  for (int i=0;i<NumFaces;i++)
   for(int k=0; k<3; k++)
   {
    FaceList[i].TX2[k] = LO->FaceList[FaceList[i].IDN].TX[k];
    FaceList[i].TY2[k] = LO->FaceList[FaceList[i].IDN].TY[k];
   } 

  FillBuffers();
  delete LO;

  return 1;
 }

 return 0;
}



static int AddUniqueEdge(WORD* pEdges, DWORD& dwNumEdges, WORD v0, WORD v1)
{
  // check if we already have that edge there.
  for(DWORD i=0; i < dwNumEdges; i++) {
    if((pEdges[3*i+0] == v0 && pEdges[3*i+1] == v1) ||
       (pEdges[3*i+0] == v1 && pEdges[3*i+1] == v0)) {
       
      pEdges[3*i+2]++; 
      return i;
    }
  }
  
  pEdges[3*dwNumEdges+0] = v0;
  pEdges[3*dwNumEdges+1] = v1;
  pEdges[3*dwNumEdges+2] = 1;
  return dwNumEdges++;
}


void r3dMesh::RecalcEdges()
{
  WORD *pEdges = new WORD[NumFaces * 3 * 3];
  DWORD dwNumEdges = 0;

  for(int iFace = 0; iFace<NumFaces; iFace++) 
  {
   r3dFace *FL = &FaceList[iFace];
   const WORD i0 = FL->VertIndex[0];
   const WORD i1 = FL->VertIndex[1];
   const WORD i2 = FL->VertIndex[2];

   // store 3 indices of edges for that face
   FL->Edges[0] = AddUniqueEdge(pEdges, dwNumEdges, i0, i1);
   FL->Edges[1] = AddUniqueEdge(pEdges, dwNumEdges, i1, i2);
   FL->Edges[2] = AddUniqueEdge(pEdges, dwNumEdges, i2, i0);
  }

  NumEdges 	= dwNumEdges;
  bHaveEdges 	= 1;
  
#if 1
  // check for edges, that is shared by more that 2 faces or unclosed
  int iWasSkipped = 0;
  for(int iFace = 0; iFace<NumFaces; iFace++) 
  {
    const WORD e0 = FaceList[iFace].Edges[0];
    const WORD e1 = FaceList[iFace].Edges[1];
    const WORD e2 = FaceList[iFace].Edges[2];

    //if(pEdges[3*e0+2] != 2 || pEdges[3*e1+2] != 2 || pEdges[3*e2+2] != 2) {
    if(pEdges[3*e0+2] > 2 || pEdges[3*e1+2] > 2 || pEdges[3*e2+2] > 2) {
      // special case - that edge is broken, skip it.
      FaceList[iFace].Edges[0] = 0;
      FaceList[iFace].Edges[1] = 0;
      FaceList[iFace].Edges[2] = 0;
      iWasSkipped++;
      //r3dOutToLog("face %3d have illegal edge count: %d %d %d\n", iFace, pEdges[3*i0+2], pEdges[3*i1+2], pEdges[3*i2+2]);
    }
  }

  if(iWasSkipped) 
    r3dOutToLog("\nWarning: RecalcEdges(%s), several faces (%d) had unclosed edges\n\n", Name, iWasSkipped);
#endif    

  delete[] pEdges;
}

void r3dMesh::RecalcBoundBox()
{
 if(!IsValid()) {
    BBox.Org.Assign(0, 0, 0);
    BBox.Size.Assign(0, 0, 0);
    return;
  }
  r3dPoint3D *WV = WorldVertexList;
  r3dPoint3D *Sz = &WorldVertexList[NumVertices];

  float minx = WV->X;
  float miny = WV->Y;
  float minz = WV->Z;
  float maxx = WV->X;
  float maxy = WV->Y;
  float maxz = WV->Z;

  while(++WV < Sz)
  {
   if(minx > WV->X) minx = WV->X;
   if(miny > WV->Y) miny = WV->Y;
   if(minz > WV->Z) minz = WV->Z;
   if(maxx < WV->X) maxx = WV->X;
   if(maxy < WV->Y) maxy = WV->Y;
   if(maxz < WV->Z) maxz = WV->Z;
  }

  BBox.Org.X  = minx;// + vMove.x;
  BBox.Org.Y  = miny;// + vMove.y;
  BBox.Org.Z  = minz;// + vMove.y;
  BBox.Size.X = R3D_ABS(maxx - minx);
  BBox.Size.Y = R3D_ABS(maxy - miny);
  BBox.Size.Z = R3D_ABS(maxz - minz);

  *CentralPoint = (BBox.Org + (BBox.Size / 2.0f));

  return;
}


void r3dMesh::ResetPivot(int PivotMode)
{
  vPivot = (BBox.Org + (BBox.Size / 2.0f));

  if (PivotMode == R3D_PIVOT_BOTTOM) vPivot.y = BBox.Org.y;

  return;
}




//-----------------------------------------------------------------------
void r3dMesh::RecalcLight(r3dLight *L, int NumLights)
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

  r3dFace   *FL   = FaceList;
  r3dFace   *FLSz = FaceList + NumFaces;

  float	ambR, ambG, ambB;

  ambR = 0; //_r3d_b2f[ r3dRenderer->AmbientLightValue.R ];
  ambG = 0; //_r3d_b2f[ r3dRenderer->AmbientLightValue.G ];
  ambB = 0; //_r3d_b2f[ r3dRenderer->AmbientLightValue.B ];

  do
  {
   float 	R, G, B;

   for(int i=0; i<3; i++)
   {
    R = ambR;
    G = ambG;
    B = ambB;

    for(int k = 0; k < NumLights; k++)
    {
     float	koef;

     // break if colors is fully white
     if(R + G + B >= 255*3)
       break;

     if(bHaveVertexNormals)
       koef = L[k].CalcLightIntensity(*FL->Vertices[i], VertexNormals[FL->VertIndex[i]]);
     else
       koef = L[k].CalcLightIntensity(*FL->Vertices[i], FL->Normal);

     if(koef <= R3D_EPSILON)
       continue;

     R += L[k].R * koef;
     G += L[k].G * koef;
     B += L[k].B * koef;
    }

    if(R > 255) R = 255;
    if(G > 255) G = 255;
    if(B > 255) B = 255;

    FL->Color[i].R = r3dFloatToInt(R);
    FL->Color[i].G = r3dFloatToInt(G);
    FL->Color[i].B = r3dFloatToInt(B);
   }

  } while(++FL < FLSz);

  FillBuffers();
  return;

}



//-----------------------------------------------------------------------
void r3dMesh::RecalcStaticLights(r3dLightSystem &Lights)
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

  r3dFace   *FL   = FaceList;
  r3dFace   *FLSz = FaceList + NumFaces;

  int n = Lights.FillVisibleArray(BBox);
  r3dOutToLog("RecalsStaticLight: %s: %d lights\n", Name, n);

	float	ambR, ambG, ambB;

  ambR = 0; //_r3d_b2f[ r3dRenderer->AmbientLightValue.R ];
  ambG = 0; //_r3d_b2f[ r3dRenderer->AmbientLightValue.G ];
  ambB = 0; //_r3d_b2f[ r3dRenderer->AmbientLightValue.B ];

  do
  {
   float 	R, G, B;

    for(int i=0; i<3; i++)
    {
      R = ambR;
      G = ambG;
      B = ambB;

	  r3dVector Normal;

	  Normal	= FL->Normal;

      if (bHaveVertexNormals == 100)
        Normal	= VertexNormals[FL->VertIndex[i]];

      Lights.CalcVisibleLightValues(*FL->Vertices[i], Normal, R, G, B);

      FL->Color[i].R = r3dFloatToInt(R);
      FL->Color[i].G = r3dFloatToInt(G);
      FL->Color[i].B = r3dFloatToInt(B);
    }

  } while(++FL < FLSz);

  FillBuffers();

  return;
}

//-----------------------------------------------------------------------
void r3dMesh::ResetStaticLights(r3dColor &Col)
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

  r3dFace   *FL   = FaceList;
  r3dFace   *FLSz = FaceList + NumFaces;

  do
  {
    for(int i=0; i<3; i++)
    {
      FL->Color[i].R = Col.R;
      FL->Color[i].G = Col.G;
      FL->Color[i].B = Col.B;
    }

  } while(++FL < FLSz);

  FillBuffers();

  return;
}



//-----------------------------------------------------------------------
void r3dMesh::RecalcTangentLights(r3dLight *L)
//-----------------------------------------------------------------------
{
}



//
// store pointer to dynamic light array so we will use it at ::Draw
//
void r3dMesh::RecalcDynamicLights(r3dLightSystem &Lights)
{
  DynaLights = &Lights;
}



static
int __MatSort(const void *P1, const void *P2)
{
	r3dFace *f1 = (r3dFace*)P1;
	r3dFace *f2 = (r3dFace*)P2;
	int	op1 = 0;
	int	op2 = 0;

  if(f1->Mat)
  {
    if((f1->Mat->Flags & R3D_MAT_SUBOP) || (f1->Mat->Flags & R3D_MAT_ADDOP) || (f1->Mat->Flags & R3D_MAT_MASKOP))
    op1 = 1;

    if((f2->Mat->Flags & R3D_MAT_SUBOP) || (f2->Mat->Flags & R3D_MAT_ADDOP) || (f2->Mat->Flags & R3D_MAT_MASKOP))
    op2 = 1;
  }

  if(op1 == op2)
    return strcmp( f1->MatName, f2->MatName);
  else
    return op1-op2;
}


void r3dMesh::SortFaces()
{
  qsort( (void *)FaceList, NumFaces, sizeof(FaceList[0]), __MatSort);

//  r3dOutToLog("--Face Sorted\n");
//  for(int i=0; i<NumFaces; i++) {
//  r3dOutToLog("%2d: %s\n", 0, FaceList[0].Mat->Name);
//  r3dOutToLog("%2d: %s\n", NumFaces-1, FaceList[NumFaces-1].Mat->Name);
//  }
//  r3dOutToLog("--Face Sorted\n");

  return;
}

//-----------------------------------------------------------------------
//void r3dMesh::AssignMaterial(r3dMaterial *Mat1, int MatNum)
void r3dMesh::AssignMaterials()
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

  r3dFace* 	FL   = FaceList;
  r3dFace* 	FLSz = FaceList + NumFaces;
  int 		idx  = 0;

  do
  {
/*
	  // if previously found material still apply for current face
    if(strcmp(FL->MatName, Mat1[idx].Name) != NULL) {
      if((idx = r3dFindMaterial(FL->MatName, Mat1, MatNum)) == -1) {
        //!!r3dOutToLog("AssignMaterial: %s, missing material %s\n", Name, FL->MatName);
	idx = 0;
      }
    }
*/
	FL->Mat = r3dMaterialLibrary::FindMaterial(FL->MatName);
	if(FL->Mat->Flags & R3D_MAT_ALPHAOP)
    {
     FL->Color[0].A = FL->Mat->DiffuseColor.A;
     FL->Color[1].A = FL->Mat->DiffuseColor.A;
     FL->Color[2].A = FL->Mat->DiffuseColor.A;
    }  

  } while(++FL < FLSz);

  SortFaces();
  FillBuffers();

  return;
}


//-----------------------------------------------------------------------
void r3dMesh::SetMaterial(r3dMaterial *Mat1)
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

  r3dFace* 	FL   = FaceList;
  r3dFace* 	FLSz = FaceList + NumFaces;

  do
  {
    FL->Mat = Mat1;
    if(FL->Mat->Flags & R3D_MAT_ALPHAOP)
    {
     FL->Color[0].A = FL->Mat->DiffuseColor.A;
     FL->Color[1].A = FL->Mat->DiffuseColor.A;
     FL->Color[2].A = FL->Mat->DiffuseColor.A;
    }  

  } while(++FL < FLSz);

  FillBuffers();

  return;
}

void r3dMesh::ResetDynamicLights()
{
 DynaLights = NULL;
 SetFlag(obfDynaLightApplied, 0);

  return;
}


void r3dMesh::ResetLights()
{

}


void r3dMesh::RecalcBasisVectors()
{
  for(int i = 0; i < NumFaces; i++)
  {
    r3dPoint3D P1,P2,P3;
    r3dVector  V1, V2, CP;
    r3dVector  U, V, UV;

    P1 = *FaceList[i].Vertices[0];
    P2 = *FaceList[i].Vertices[1];
    P3 = *FaceList[i].Vertices[2];

// Calculate X
    V1.X = P2.X - P1.X;
    V1.Y = FaceList[i].TX[1] - FaceList[i].TX[0];
    V1.Z = FaceList[i].TY[1] - FaceList[i].TY[0];

    V2.X = P3.X - P1.X;
    V2.Y = FaceList[i].TX[2] - FaceList[i].TX[0];
    V2.Z = FaceList[i].TY[2] - FaceList[i].TY[0];

    CP = V1.Cross(V2);

    U.X = -CP.Y / CP.X;
    V.X = -CP.Z / CP.X;

//Calculate Y
    V1.X = P2.Y - P1.Y;
    V1.Y = FaceList[i].TX[1] - FaceList[i].TX[0];
    V1.Z = FaceList[i].TY[1] - FaceList[i].TY[0];

    V2.X = P3.Y - P1.Y;
    V2.Y = FaceList[i].TX[2] - FaceList[i].TX[0];
    V2.Z = FaceList[i].TY[2] - FaceList[i].TY[0];

    CP = V1.Cross(V2);

    U.Y = -CP.Y / CP.X;
    V.Y = -CP.Z / CP.X;
 
//Calculate Z
    V1.X = P2.Z - P1.Z;
    V1.Y = FaceList[i].TX[1] - FaceList[i].TX[0];
    V1.Z = FaceList[i].TY[1] - FaceList[i].TY[0];

    V2.X = P3.Z - P1.Z;
    V2.Y = FaceList[i].TX[2] - FaceList[i].TX[0];
    V2.Z = FaceList[i].TY[2] - FaceList[i].TY[0];

    CP = V1.Cross(V2);

    U.Z = -CP.Y / CP.X;
    V.Z = -CP.Z / CP.X;

    U.Normalize();
    V.Normalize();

/*
    UV = U.Cross(V);  
    UV.Normalize();

    r3dVector Norm;

       if (bHaveVertexNormals == 100)
        Norm	= VertexNormals[FaceList[i].VertIndex[0]];
      else
      if(bHaveVertexNormals == 200)
        Norm	= VertexNormals[i*3 + 0];
      else
        Norm	= FaceList[i].Normal;

      if (UV.Dot(Norm) < 0.0f ) UV *= -1.0f;
*/
   
      FaceList[i].vU = U;
      FaceList[i].vV = V;
//      FaceList[i].vUV = UV;
  }


 // First smooth vU

 for(int i=0;i<NumVertices;i++)
 {
   vU[i].Assign(0,0,0);
   _VNArray[i] = 0;
 } 

 for(int i = 0; i < NumFaces; i++)
 {
  for (int k=0;k<3;k++)
  {
    int Idx = FaceList[i].VertIndex[k];
  
    if ( vU[Idx].IsVoid() )
      vU[Idx] = FaceList[i].vU;
    else
    {
      vU[Idx] += FaceList[i].vU;
    }
    _VNArray[Idx] ++;

  }
 }   

 for(int i=0;i<NumVertices;i++)
 {
  vU[i] /= float(_VNArray[i]);

  vU[i].Normalize();
 }



 // smooth vV

 for (int i=0;i<NumVertices;i++)
 {
  vV[i].Assign(0,0,0);
  _VNArray[i] = 0;
 } 

 for(int i = 0; i < NumFaces; i++)
 {
  for (int k=0;k<3;k++)
  {
    int Idx = FaceList[i].VertIndex[k];
  
    if ( vV[Idx].IsVoid() )
      vV[Idx] = FaceList[i].vV;
    else
    {
      vV[Idx] += FaceList[i].vV;
    }
    _VNArray[Idx] ++;

  }
 }   

 for (int i=0;i<NumVertices;i++)
 {
  vV[i] /= float(_VNArray[i]);

  vV[i].Normalize();
 }


/*
 // smooth vUV

 for (int i=0;i<NumVertices;i++)
 {
  vUV[i].Assign(0,0,0);
  _VNArray[i] = 0;
 } 

 for(i = 0; i < NumFaces; i++)
 {
  for (int k=0;k<3;k++)
  {
    int Idx = FaceList[i].VertIndex[k];
  
    if ( vUV[Idx].IsVoid() )
      vUV[Idx] = FaceList[i].vUV;
    else
    {
      vUV[Idx] += FaceList[i].vUV;
    }
    _VNArray[Idx] ++;

  }
 }   

 for (int i=0;i<NumVertices;i++)
 {
  vUV[i] /= float(_VNArray[i]);

  vUV[i].Normalize();
 }
*/


}



void r3dMesh::ResetNormals()
{
    #define UNLOAD_ARRAY(a)	\
      if(a)			\
      {				\
        delete [] a; a = 0;	\
      }

 if (bHaveVertexNormals)
 {
  UNLOAD_ARRAY(VertexNormals);
  bHaveVertexNormals = 0;
 }

 RecalcNormals();
}



void r3dMesh::RecalcNormals()
{
 for(int i = 0; i < NumFaces; i++)
 {
  FaceList[i].CalcFaceNormal();
  FaceList[i].OrgNormal = FaceList[i].Normal;
  FaceList[i].DetermineFaceTag();
 }

 if (!VBMode) FillBuffers();
}




void r3dMesh::RecalcSmoothNormals()
{
 RecalcNormals();

 if (!bHaveVertexNormals)
 {
  VertexNormals = new r3dVector[NumFaces*3];
 }
 else
  if (bHaveVertexNormals != 200)
  {
   delete VertexNormals;
   VertexNormals = new r3dVector[NumFaces*3];
  }


 for (int i=0;i<NumFaces*3;i++)
 {
  VertexNormals[i].Assign(0,0,0);
//  _VNArray[i] = 0;
 } 

 for(int i = 0; i < NumFaces; i++)
  for (int k=0;k<3;k++)
  { 
   VertexNormals[i*3 +k] = FaceList[i].Normal;
   _VNArray[i*3 +k] = 1;
  }


 float SmoothDif = 0.7f;

 for(int i = 0; i < NumFaces; i++)
 {
  for ( int k=0;k<3;k++)
  {
   r3dVector TN = VertexNormals[i*3+k];

   int Idx = FaceList[i].VertIndex[k];

   for (int j=0;j< NumFaces; j++)
   {
    int Idx1 = FaceList[j].VertIndex[0];
  
    if (Idx == Idx1)
     if(TN.Dot(FaceList[j].Normal) > SmoothDif)
     {
      TN += FaceList[j].Normal;
      //_VNArray[i*3 +k] ++;
      TN /= 2.0f;
      TN.Normalize();
     }

    Idx1 = FaceList[j].VertIndex[1];
  
    if (Idx == Idx1)
     if(TN.Dot(FaceList[j].Normal) > SmoothDif)
     {
      TN += FaceList[j].Normal;
      TN /= 2.0f;
      TN.Normalize();
     }

    Idx1 = FaceList[j].VertIndex[2];
  
    if (Idx == Idx1)
     if(TN.Dot(FaceList[j].Normal) > SmoothDif)
     {
      TN += FaceList[j].Normal;
      TN /= 2.0f;
      TN.Normalize();
     }
   }

//   TN.Normalize();
   VertexNormals[i*3+k] = TN;
  } // For each normal
 } //For each face   


 for (int i=0;i>NumFaces*3;i++)
 {
//  VertexNormals[i] /= float(_VNArray[i]);
  VertexNormals[i].Normalize();
 }


 bHaveVertexNormals = 200;
 if (!VBMode) FillBuffers();
}



void r3dMesh::RecalcVertexNormals()
{
 RecalcNormals();

 if (!bHaveVertexNormals)
 {
  VertexNormals = new r3dVector[NumVertices];
 }
 else
  if (bHaveVertexNormals != 100)
  {
   delete VertexNormals;
   VertexNormals = new r3dVector[NumVertices];
  }


 for (int i=0;i<NumVertices;i++)
 {
  VertexNormals[i].Assign(0,0,0);
  _VNArray[i] = 0;
 } 

 for(int i = 0; i < NumFaces; i++)
 {
  for (int k=0;k<3;k++)
  {
    int Idx = FaceList[i].VertIndex[k];
  
    if ( VertexNormals[Idx].IsVoid() )
      VertexNormals[Idx] = FaceList[i].Normal;
    else
    {
      VertexNormals[Idx] += FaceList[i].Normal;
//      VertexNormals[Idx] /=2;
    }
    _VNArray[Idx] ++;

  }
 }   

 for (int i=0;i<NumVertices;i++)
 {
  VertexNormals[i] /= float(_VNArray[i]);

  VertexNormals[i].Normalize();
 }

 bHaveVertexNormals = 100;
 FillBuffers();
}






//-----------------------------------------------------------------------
void r3dMesh::Move(const r3dPoint3D& D)
//-----------------------------------------------------------------------
{
  if(!IsValid() || IsFlagSet(obfStatic))
    return;

  vMove         += D;
  vPivot        += D;
  *CentralPoint += D;
  BBox.Org      += D;

//  r3dOutToLog("%s %f %f %f\n", Name, vMove.X, vMove.Y, vMove.Z);

#ifdef	_FORCE_XFORM
  for(int i = 0; i < NumVertices; i++)
    WorldVertexList[i] += D;

  vMove.Assign(0,0,0); 
  FillBuffers();
#endif
}

//-----------------------------------------------------------------------
void r3dMesh::Scale(const r3dPoint3D& koef)
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

//  vScale += koef;

//#ifdef	_FORCE_XFORM

//  for(int i=0; i<NumVertices; i++)
//    WorldVertexList[i] = ((WorldVertexList[i] - *CentralPoint) * koef) + *CentralPoint;

//  ResetXForm();

//#endif

}




void r3dMesh::FillBuffers()
{
  if(r3dMeshObject_SkipFillBuffers)
    return;

  if(!NumFaces || !NumVertices) 
    return;

  if(VBMode) return FillStripBuffers();


	DWORD	vb_usage;
	D3DPOOL	vb_pool;
	DWORD	vb_lock;

  isDynamic = 0;
  if(isDynamic) {
    vb_usage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
    vb_pool  = D3DPOOL_MANAGED; //D3DPOOL_DEFAULT;
    vb_lock  = D3DLOCK_DISCARD;
  } else {
    vb_usage = D3DUSAGE_WRITEONLY;
    vb_pool  = D3DPOOL_MANAGED;
    vb_lock  = 0;
  }


  vb_pool  = D3DPOOL_MANAGED; //D3DPOOL_DEFAULT;

  if(!g_pVB)
  {
    r3dRenderer->pd3ddev->CreateVertexBuffer(
      NumFaces * sizeof(R3D_WORLD_VERTEX) * 3,
      vb_usage, 
      FVF_R3D_WORLD_VERTEX,
      vb_pool, 
      &g_pVB, NULL);

    r3dRenderer->Stats.BufferMem += NumFaces * 3 * sizeof(R3D_WORLD_VERTEX);

  #ifdef _DEBUG
    _r3d_mVBufferMap.insert(std::pair<DWORD, std::string>((DWORD)g_pVB, Name));
  #endif

  }



//  if(!g_IB )
  if (0)
    r3dRenderer->pd3ddev->CreateIndexBuffer(
      NumFaces * sizeof(WORD) * 3,
      vb_usage, 
      D3DFMT_INDEX16,
      D3DPOOL_MANAGED, 
      &g_IB, NULL);

 // fill indices

//  WORD* pIndices;
  
//  if(FAILED(g_IB->Lock(0, NumFaces*3*sizeof(WORD), (void**)&pIndices, vb_lock)))            	
//    return;

  r3dMaterial *CMat  = (r3dMaterial *)-1;
  NumMatChunks = 0;
  
  int Idx = 0;
  for (int i=0;i<NumFaces;i++)
  {
   if (CMat != FaceList[i].Mat )
   { 
    CMat = FaceList[i].Mat;
    MatChunks[NumMatChunks].Mat = CMat;
    MatChunks[NumMatChunks].StartIndex = Idx;
   
    if (NumMatChunks) MatChunks[NumMatChunks-1].EndIndex = Idx; //-1;
    NumMatChunks++;
   }

Idx++; 
Idx++; 
Idx++; 

//   *pIndices++ = Idx++; 
//   *pIndices++ = Idx++;
//   *pIndices++ = Idx++; 
  }
  MatChunks[NumMatChunks-1].EndIndex = Idx;

//  g_IB->Unlock();


 // Fill the vertex buffer.
  R3D_WORLD_VERTEX* pVertices = NULL;
  if(FAILED(g_pVB->Lock(0, NumFaces*sizeof(R3D_WORLD_VERTEX)*3, (void**)&pVertices, vb_lock)))  
    return;

  for (int i=0;i<NumFaces;i++)
  {
   for (int k=0;k<3;k++)
   {
      pVertices->Pos 		= *FaceList[i].Vertices[k];

      if (bHaveVertexNormals == 100)
        pVertices->Normal	= VertexNormals[FaceList[i].VertIndex[k]];
      else
      if(bHaveVertexNormals == 200)
        pVertices->Normal	= VertexNormals[i*3 + k];
      else
        pVertices->Normal	= FaceList[i].Normal;

      if (bHaveVertexNormals > 99)
      {
        pVertices->vU =    vU[FaceList[i].VertIndex[k]];
        pVertices->vV =    vV[FaceList[i].VertIndex[k]];
//        pVertices->vUV =   vUV[FaceList[i].VertIndex[k]];
      }
      else
        {
         pVertices->vU  =    FaceList[i].vU;
         pVertices->vV  =    FaceList[i].vV;
//         pVertices->vUV =    FaceList[i].vUV;
        }

      pVertices->color    	= FaceList[i].Color[k];
      pVertices->tu 		= FaceList[i].TX[k];
      pVertices->tv 		= FaceList[i].TY[k];
      pVertices->tu2 		= FaceList[i].TX2[k];
      pVertices->tv2 		= FaceList[i].TY2[k];
      pVertices ++;
   }
  }
  g_pVB->Unlock();

  return;
}


void r3dMesh::FillStripBuffers()
{
  if(r3dMeshObject_SkipFillBuffers)
    return;

  if(!NumFaces || !NumVertices) 
    return;

	DWORD	vb_usage;
	D3DPOOL	vb_pool;
	DWORD	vb_lock;

  isDynamic = 0;
  if(isDynamic) {
    vb_usage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
    vb_pool  = D3DPOOL_DEFAULT;
    vb_lock  = D3DLOCK_DISCARD;
  } else {
    vb_usage = D3DUSAGE_WRITEONLY;
    vb_pool  = D3DPOOL_MANAGED;
    vb_lock  = 0;
  }


    vb_pool  = D3DPOOL_MANAGED; //D3DPOOL_DEFAULT;

  if(!g_pVB)
  {
    r3dRenderer->pd3ddev->CreateVertexBuffer(
      NumVertices * sizeof(R3D_WORLD_VERTEX),
      vb_usage, 
      FVF_R3D_WORLD_VERTEX,
      vb_pool, 
      &g_pVB, NULL);

    r3dRenderer->Stats.BufferMem += NumFaces * 3 * sizeof(R3D_WORLD_VERTEX);
  }



  if(!g_IB )
    r3dRenderer->pd3ddev->CreateIndexBuffer(
      NumFaces * sizeof(WORD) * 3,
      vb_usage, 
      D3DFMT_INDEX16,
      D3DPOOL_MANAGED, 
      &g_IB, NULL);


    WORD* pIndices;

 // fill indices
  if(FAILED(g_IB->Lock(0, NumFaces*3*sizeof(WORD), (void**)&pIndices, vb_lock)))            	
    return;
  
  r3dMaterial *CMat  = (r3dMaterial *)-1;
  NumMatChunks = 0;
  
  int Idx = 0;
  for (int i=0;i<NumFaces;i++)
  {
   if (CMat != FaceList[i].Mat )
   { 
    CMat = FaceList[i].Mat;
    MatChunks[NumMatChunks].Mat = CMat;
    MatChunks[NumMatChunks].StartIndex = Idx;
   
    if (NumMatChunks) MatChunks[NumMatChunks-1].EndIndex = Idx; //-1;
    NumMatChunks++;
   }

   *pIndices++ = FaceList[i].VertIndex[0]; 
   *pIndices++ = FaceList[i].VertIndex[1];
   *pIndices++ = FaceList[i].VertIndex[2];
   Idx++;
   Idx++;
   Idx++;
  }
  MatChunks[NumMatChunks-1].EndIndex = Idx;

  g_IB->Unlock();


 // Fill the vertex buffer.
  R3D_WORLD_VERTEX* pVertices = NULL;
  if(FAILED(g_pVB->Lock(0, NumVertices*sizeof(R3D_WORLD_VERTEX), (void**)&pVertices, vb_lock)))  
    return;

  for (int i=0;i<NumVertices;i++)
  {
   pVertices->Pos 	= WorldVertexList[i];
   pVertices->Normal	= VertexNormals[i];
 
   for (int j=0;j<NumFaces;j++)
    for (int k=0;k<3;k++)
        if ( i == FaceList[j].VertIndex[k])
        {
         pVertices->tu 	= FaceList[j].TX[k];
         pVertices->tv 	= FaceList[j].TY[k];
         pVertices->tu2 = FaceList[j].TX2[k];
         pVertices->tv2 = FaceList[j].TY2[k];

         break;
        }
    
   pVertices->color    	= r3dColor(255,255,255);
   pVertices ++;
  }
  g_pVB->Unlock();

  return;
}





/*

#define EPSILON 0.0001f
static temp_AddVertex(R3D_WORLD_VERTEX &v1, R3D_WORLD_VERTEX *pVertices, int &ctVertices)
{
  for(int i=0; i<ctVertices; i++) {
    R3D_WORLD_VERTEX &v2 = pVertices[i];
 
    if(fabs(v1.Pos.x - v2.Pos.x) >= EPSILON) continue;
    if(fabs(v1.Pos.y - v2.Pos.y) >= EPSILON) continue;
    if(fabs(v1.Pos.z - v2.Pos.z) >= EPSILON) continue;
    
    if(fabs(v1.Normal.x - v2.Normal.x) >= EPSILON) continue;
    if(fabs(v1.Normal.y - v2.Normal.y) >= EPSILON) continue;
    if(fabs(v1.Normal.z - v2.Normal.z) >= EPSILON) continue;
 
    if(fabs(v1.vU.x - v2.vU.x) >= EPSILON) continue;
    if(fabs(v1.vU.y - v2.vU.y) >= EPSILON) continue;
    if(fabs(v1.vU.z - v2.vU.z) >= EPSILON) continue;
 
    if(fabs(v1.vV.x - v2.vV.x) >= EPSILON) continue;
    if(fabs(v1.vV.y - v2.vV.y) >= EPSILON) continue;
    if(fabs(v1.vV.z - v2.vV.z) >= EPSILON) continue;
 
//    if(fabs(v1.vUV.x - v2.vUV.x) >= EPSILON) continue;
//    if(fabs(v1.vUV.y - v2.vUV.y) >= EPSILON) continue;
//    if(fabs(v1.vUV.z - v2.vUV.z) >= EPSILON) continue;
 
    if(fabs(v1.tu - v2.tu) >= EPSILON) continue;
    if(fabs(v1.tv - v2.tv) >= EPSILON) continue;
    if(fabs(v1.tu2 - v2.tu2) >= EPSILON) continue;
    if(fabs(v1.tv2 - v2.tv2) >= EPSILON) continue;
 
    // same vertex - return old index.
    return i;
  }
 
  pVertices[ctVertices] = v1;
  return ctVertices++;
}

 
void r3dMesh::FillBuffers()
{
  if(r3dMeshObject_SkipFillBuffers)
    return;
 
  if(!NumFaces || !NumVertices) 
    return;
 
//
// create & fill temporary buffers
//
  WORD              *pIndices  = new WORD[NumFaces * sizeof(WORD) * 3];
  R3D_WORLD_VERTEX  *pVertices = new R3D_WORLD_VERTEX[NumFaces*sizeof(R3D_WORLD_VERTEX)*3];
  int      ctIndices  = 0;
  int      ctVertices = 0;
 
  r3dMaterial *CMat  = (r3dMaterial *)-1;
  NumMatChunks = 0;
  
  MatChunks[NumMatChunks-1].EndIndex = ctIndices;
 
  for (int i=0;i<NumFaces;i++)
  {
   if (CMat != FaceList[i].Mat )
   { 
    CMat = FaceList[i].Mat;
    MatChunks[NumMatChunks].Mat        = CMat;
    MatChunks[NumMatChunks].StartIndex = ctIndices;
   
    if (NumMatChunks) MatChunks[NumMatChunks-1].EndIndex = ctIndices; //-1;
    NumMatChunks++;
   }
 
   for (int k=0;k<3;k++)
   {
     R3D_WORLD_VERTEX v;
     v.Pos   = *FaceList[i].Vertices[k];

     if (bHaveVertexNormals == 100)
       v.Normal = VertexNormals[FaceList[i].VertIndex[k]];
     else
     if(bHaveVertexNormals == 200)
       v.Normal = VertexNormals[i*3 + k];
     else
       v.Normal = FaceList[i].Normal;
 
     v.vU  = FaceList[i].vU;
     v.vV  = FaceList[i].vV;
//     v.vUV = FaceList[i].vUV;
//      pVertices->color     = FaceList[i].Color[k].GetPacked();
 
     v.tu  = FaceList[i].TX[k];
     v.tv  = FaceList[i].TY[k];
     v.tu2 = FaceList[i].TX2[k];
     v.tv2 = FaceList[i].TY2[k];
 
     pIndices[ctIndices++] = temp_AddVertex(v, pVertices, ctVertices);
 //    pIndices[ctIndices++] = temp_AddVertex(v, pVertices, ctVertices);
 //    pIndices[ctIndices++] = temp_AddVertex(v, pVertices, ctVertices);
   }
  }
 
//
// copy new buffers
//
 
  DWORD vb_usage;
  D3DPOOL vb_pool;
  DWORD vb_lock;
 
  vb_usage = D3DUSAGE_WRITEONLY;
  vb_pool  = D3DPOOL_MANAGED;
  vb_lock  = 0;
 
  if(!g_pVB)
  {
    r3dRenderer->pd3ddev->CreateVertexBuffer(
      ctVertices * sizeof(R3D_WORLD_VERTEX),
      vb_usage, 
      FVF_R3D_WORLD_VERTEX,
      vb_pool, 
      &g_pVB, NULL);
 
    r3dRenderer->Stats.BufferMem += ctVertices * sizeof(R3D_WORLD_VERTEX);
  }
 
  if(!g_IB )
    r3dRenderer->pd3ddev->CreateIndexBuffer(
      NumFaces * 3 * sizeof(WORD),
      vb_usage, 
      D3DFMT_INDEX16,
      D3DPOOL_MANAGED, 
      &g_IB, NULL);
 
  // fill indices
  WORD* pIndicesNew;
  if(FAILED(g_IB->Lock(0, NumFaces * 3 * sizeof(WORD), (void**)&pIndicesNew, vb_lock)))
    return;
  memcpy(pIndicesNew, pIndicesNew, ctIndices*sizeof(WORD));
 
  g_IB->Unlock();
 
  // Fill the vertex buffer.
  R3D_WORLD_VERTEX* pVerticesNew = NULL;
  if(FAILED(g_pVB->Lock(0, ctVertices*sizeof(R3D_WORLD_VERTEX), (void**)&pVerticesNew, vb_lock)))  
    return;
  memcpy(pVerticesNew, pVertices, ctVertices*sizeof(R3D_WORLD_VERTEX));
  g_pVB->Unlock();

  r3dOutToLog("Vertices  %d  was  %d\nIndeces  %d  was  %d\n", ctVertices, NumFaces*3, ctIndices, NumFaces *3);
 
//
// free temporary buffers
//
  delete[] pIndices;
  delete[] pVertices;
 
  return;
}
*/






int Dec = 1;

void r3dDrawNElements(int NumElements, int StartIdx, int EndIdx, int StartE)
{
 int NumR = NumElements / Dec;
 int Less = NumElements - NumR*Dec;

 for (int i=0;i<NumR;i++)
 {
  r3dRenderer->pd3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,  0,StartIdx, EndIdx, StartE + i*Dec*3, Dec);

  r3dRenderer->Stats.NumTrianglesRendered += Dec;
  r3dRenderer->Stats.NumDraws++;
 }

 
 r3dRenderer->pd3ddev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,  0,StartIdx, EndIdx, StartE + NumR*Dec*3, Less );

 r3dRenderer->Stats.NumTrianglesRendered += Less;
 r3dRenderer->Stats.NumDraws++;
}



//-----------------------------------------------------------------------
void r3dMesh::ResetXForm()
//-----------------------------------------------------------------------
{
	for(int i = 0; i < NumVertices; i++)
    WorldVertexList[i] += vMove;

  *CentralPoint += vMove;
  vPivot        += vMove;

  vMove.Assign(0,0,0); 
  vScale.Assign(1,1,1);

  RecalcNormals();
  RecalcBoundBox();

  FillBuffers();
}


//-----------------------------------------------------------------------
D3DXMATRIX r3dMesh::GetWorldMatrix()
//-----------------------------------------------------------------------
{
 D3DXMATRIX 	m1,m2, mWorld, mMove;

 D3DXMatrixTranslation(&mMove, vMove.X, vMove.Y, vMove.Z );
// D3DXMatrixMultiply(&mWorld, &RotateMatrix, &mMove);

 r3dVector RotP = vPivot; //*CentralPoint;

 D3DXMatrixTranslation(&m1, RotP.X, RotP.Y, RotP.Z );
 D3DXMatrixTranslation(&m2, -RotP.X, -RotP.Y, -RotP.Z );

 D3DXMatrixMultiply(&mWorld, &mMove, &m2);

 D3DXMatrixMultiply(&mWorld, &mWorld, &RotateMatrix );
 D3DXMatrixMultiply(&mWorld, &mWorld, &m1);

 return mWorld;
}



//-----------------------------------------------------------------------
void r3dMesh::BuildWorldMatrix()
//-----------------------------------------------------------------------
{
 D3DXMATRIX 	m1,m2, mWorld, mMove, mScale;

 D3DXMatrixTranslation(&mMove, vMove.X, vMove.Y, vMove.Z );
 D3DXMatrixScaling(&mScale, vScale.X, vScale.Y, vScale.Z );

 r3dVector RotP = vPivot; //*CentralPoint;

 D3DXMatrixTranslation(&m1, RotP.X, RotP.Y, RotP.Z );
 D3DXMatrixTranslation(&m2, -RotP.X, -RotP.Y, -RotP.Z );

 D3DXMatrixMultiply(&mWorld, &mMove, &m2);
 D3DXMatrixMultiply(&mWorld, &mWorld, &mScale);

 D3DXMatrixMultiply(&mWorld, &mWorld, &RotateMatrix );
 D3DXMatrixMultiply(&mWorld, &mWorld, &m1);

// mWorld = m1 * RotateMatrix * m2;

 D3DXMatrixInverse(&r3dRenderer->mInverseWorld, NULL, &RotateMatrix);

 //if (bUseWorldMatrix)
 //   mWorld = WorldMatrix;

 r3dRenderer->pd3ddev->SetTransform(D3DTS_WORLD,      &mWorld);

 D3DXMATRIX ShaderMat;
 ShaderMat =  mWorld * 	r3dRenderer->mCamera * r3dRenderer->mProj;
 D3DXMatrixTranspose( &ShaderMat, &ShaderMat );

 r3dRenderer->pd3ddev->SetVertexShaderConstantF(  0, (float *)&ShaderMat,  4 );

 ShaderMat =  mWorld;
 D3DXMatrixTranspose( &ShaderMat, &ShaderMat );
 r3dRenderer->pd3ddev->SetVertexShaderConstantF(  4, (float *)&ShaderMat,  4 );
}


//-----------------------------------------------------------------------
void r3dMesh::DrawDX7()
//-----------------------------------------------------------------------
{
 Draw();
}


/*
//-----------------------------------------------------------------------
void r3dMesh::DrawDX8()
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

 r3dRenderer->Flush();

 BuildWorldMatrix();

// r3dRenderer->pd3ddev->SetIndices( g_IB );
 r3dRenderer->pd3ddev->SetStreamSource( 0, g_pVB, 0, sizeof(R3D_WORLD_VERTEX) );

 int CurrentShaderID = -1;

 int ShaderID = r3dRenderer->LightTypeDX8;

 if (LightmapTex && ShaderID==4)
     r3dRenderer->SetTexture(LightmapTex,3);

 for (int i=0;i<NumMatChunks;i++)
 {
  ShaderID = r3dRenderer->LightTypeDX8;

  if (MatChunks[i].Mat->Texture)
    r3dRenderer->SetTexture(MatChunks[i].Mat->Texture);
  else
    r3dRenderer->SetTexture( __r3dShadeTexture[2]);

  if (MatChunks[i].Mat->SpecularPower)
  {
   ShaderID += 8;
//   r3dRenderer->SetTexture( __r3dShadeTexture[3], 4);
  }

  if (MatChunks[i].Mat->GlossTexture && MatChunks[i].Mat->SpecularPower) 
     r3dRenderer->SetTexture(MatChunks[i].Mat->GlossTexture,2);
  else
    r3dRenderer->SetTexture(MatChunks[i].Mat->Texture, 2);

  if (MatChunks[i].Mat->BumpTexture)
  {
   r3dRenderer->SetTexture(MatChunks[i].Mat->BumpTexture,1);
   ShaderID += 16;
  }

  if (CurrentShaderID != ShaderID)
  {
   CurrentShaderID = ShaderID;
   __r3dShadeVertexShader[ShaderID].SetActive(1);
   __r3dShadePixelShader[ShaderID].SetActive(1);
  }

  if (MatChunks[i].Mat->AlphaRef)
    r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHAREF, (unsigned long)MatChunks[i].Mat->AlphaRef);
//  r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHAREF, 200);

//  r3dRenderer->pd3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,  0, 0, NumFaces*3, 0,NumFaces);

  r3dRenderer->pd3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 
    MatChunks[i].StartIndex, 
    (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3);

  r3dRenderer->Stats.NumTrianglesRendered += (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3;
  r3dRenderer->Stats.AverageStripLength += (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3;
  r3dRenderer->Stats.AverageStripLength /= 2;
  r3dRenderer->Stats.NumDraws++;

  if (MatChunks[i].Mat->AlphaRef)
    r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHAREF, 1);
 }

 D3DXMATRIX 	mWorld;
 D3DXMatrixIdentity(&mWorld);
 r3dRenderer->pd3ddev->SetTransform(D3DTS_WORLD,      &mWorld);
}
*/



void SetObjLightsConstants(r3dLightSystem *WL, r3dBoundBox &BBox)
{
 r3dLight *L;
 
 r3dVector VL = r3dVector(1.0f,0.5f,1.0f);
 r3dVector LC = r3dVector(0,0,0);
 
 int  NumPointLights = 0;
 r3dVector PP[8];
 float    PR[8];
 r3dVector PC[8];
 
 for (int i=0;i<8;i++)
 {
  PP[i] = r3dVector(100,100,100);
  PR[i] = 0;
  PC[i] = r3dVector(0,0,0);
 }
 
 for(L = WL->Lights; L; L = L->pNext)
 {
  if (L->Type == R3D_DIRECT_LIGHT )
  {
    VL = -L->Direction;
    LC = r3dVector(float(L->R)/255.0f,float(L->G)/255.0f, float(L->B)/255.0f );
  }
 
  if (L->Type == R3D_OMNI_LIGHT && NumPointLights < 8 && L->bOn)
   if(BBox.Intersect(L->BBox))
   {
    PP[NumPointLights] = r3dVector(L->X,L->Y,L->Z);
    PC[NumPointLights] = r3dVector(float(L->R)/255.0f,float(L->G)/255.0f, float(L->B)/255.0f );
    PR[NumPointLights] = L->GetOuterRadius(); //*1.2f;
    NumPointLights ++;
   }
 }  
 
/*
 float AC[4]; 
 AC[0] = float(r3dRenderer->AmbientColor.R)/255.0f;
 AC[1] = float(r3dRenderer->AmbientColor.G)/255.0f;
 AC[2] = float(r3dRenderer->AmbientColor.B)/255.0f;
 AC[3] = 1;
 
 r3dRenderer->pd3ddev->SetVertexShaderConstantF(  5, (float *)&VL,  1 );
 r3dRenderer->pd3ddev->SetVertexShaderConstantF(  6, (float *)&LC,  1 );
 r3dRenderer->pd3ddev->SetVertexShaderConstantF(  7, AC,  1 );
 r3dRenderer->pd3ddev->SetPixelShaderConstantF(  7, AC,  1 );
*/

  float PPos[16][4]; 
//  float PCol[4][4]; 


 for (int i=0;i<8;i++)
 {
  PPos[i][0] = PP[i].X;
  PPos[i][1] = PP[i].Y;
  PPos[i][2] = PP[i].Z;
  PPos[i][3] = PR[i];
 
  PPos[i+8][0] = PC[i].X;
  PPos[i+8][1] = PC[i].Y;
  PPos[i+8][2] = PC[i].Z;
  PPos[i+8][3] = 1;
 }

 r3dRenderer->pd3ddev->SetVertexShaderConstantF(  30, PPos[0],  16 );
}



//-----------------------------------------------------------------------
void r3dMesh::DrawAmbientPass()
//-----------------------------------------------------------------------
{
 if (bSimulateGI)
 {
	 assert("SimulateGI path not fixed");
  r3dRenderer->SetVertexShader("VS_GIDRAW");
  r3dRenderer->SetPixelShader("PS_GIDRAW");

  r3dRenderer->SetTexture(AmbientOcclusionTex,1 );
  SetObjLightsConstants(DynaLights, BBox);

  DrawSimple(1);

  return;
 }


 if (LightmapTex) DrawLightPass();
 else
 {
  r3dRenderer->SetVertexShader("VS_AMBIENTDRAW");
  r3dRenderer->SetPixelShader("PS_AMBIENTDRAW");
  DrawSimple(1);
 }

}


//-----------------------------------------------------------------------
void r3dMesh::DrawLightPass()
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

// r3dRenderer->Flush();
 BuildWorldMatrix();

// r3dRenderer->pd3ddev->SetIndices( g_IB );
 r3dRenderer->pd3ddev->SetStreamSource( 0, g_pVB, 0, sizeof(R3D_WORLD_VERTEX) );

 if (LightmapTex && r3dRenderer->LightTypeDX8==4) r3dRenderer->SetTexture(LightmapTex,3);


 for (int i=0;i<NumMatChunks;i++)
 {
  MatChunks[i].Mat->Start();

/*
  r3dRenderer->pd3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,  0,
    MatChunks[i].StartIndex, 
    MatChunks[i].EndIndex-MatChunks[i].StartIndex, 
    MatChunks[i].StartIndex,
    (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3);
*/

  r3dRenderer->pd3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 
    MatChunks[i].StartIndex, 
    (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3);

  r3dRenderer->Stats.NumTrianglesRendered += (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3;
  r3dRenderer->Stats.AverageStripLength += (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3;
  r3dRenderer->Stats.AverageStripLength /= 2;
  r3dRenderer->Stats.NumDraws++;

  MatChunks[i].Mat->End();
 }

// D3DXMATRIX 	mWorld;
// D3DXMatrixIdentity(&mWorld);
// r3dRenderer->pd3ddev->SetTransform(D3DTS_WORLD,      &mWorld);
}




//-----------------------------------------------------------------------
void r3dMesh::Draw()
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

 r3dRenderer->Flush();

 for(int i = 0; i < 32; i++)
   r3dRenderer->pd3ddev->LightEnable(i, 0 );

 int bDynaLightsRecalc = 0;

 if(DynaLights)
 {
  int		nLights;

  // detect which lights we intersect and store them into array.
  nLights = DynaLights->FillVisibleArray(BBox);

  if(nLights == 0)
  {
   if(IsFlagSet(obfDynaLightApplied)) ResetDynamicLights();

   SetFlag(obfDynaLightApplied, 0);
   bDynaLightsRecalc = 0;
  } 
  else
  {
   SetFlag(obfDynaLightApplied, 1);
   bDynaLightsRecalc = 1;
  }
 }

 if(bDynaLightsRecalc)
 {
//  if (DynaLights->nVisibleLights > 16 ) DynaLights->nVisibleLights = 16;

  int CLight = 0;

  for(int i = 0; i < DynaLights->nVisibleLights; i++)
  {
   r3dLight *L = DynaLights->VisibleLights[i];

   if (L->bOn) L->SetD3DLight(CLight++);
  }  
 }
 else
   if (DynaLights) DynaLights->nVisibleLights = 0;

// if (DynaLights) 
//    r3dOutToLog("%s  L=%d\n",Name,  DynaLights->nVisibleLights);


 BuildWorldMatrix();

// r3dRenderer->pd3ddev->SetIndices( g_IB);
 r3dRenderer->pd3ddev->SetStreamSource( 0, g_pVB, 0,sizeof(R3D_WORLD_VERTEX) );

 for(int i=0;i<NumMatChunks;i++)
 {
  MatChunks[i].Mat->fn_ShadePtr(MatChunks[i].Mat, DynaLights,
    MatChunks[i].StartIndex, 
    MatChunks[i].EndIndex-MatChunks[i].StartIndex, 
    MatChunks[i].StartIndex,
    (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3);

//  r3dRenderer->Stats.NumTrianglesRendered += (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3;
//  r3dRenderer->Stats.AverageStripLength += (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3;
//  r3dRenderer->Stats.AverageStripLength /= 2;
 }


 // clear array pointer, so light recalc will not happen at next frame.
// DynaLights = NULL;

 if(bDynaLightsRecalc)
 {
  r3dRenderer->pd3ddev->SetRenderState(D3DRS_LIGHTING, FALSE );
  r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORVERTEX, 1);

  for(int i = 0; i < 32; i++)
          r3dRenderer->pd3ddev->LightEnable(i, 0 );
 }

 r3dRenderer->Flush();

 D3DXMATRIX 	mWorld;
 D3DXMatrixIdentity(&mWorld);
 r3dRenderer->pd3ddev->SetTransform(D3DTS_WORLD,      &mWorld);
}




//-----------------------------------------------------------------------
void r3dMesh::DrawSimple(int bTextured, int bUseVertexColor)
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

 BuildWorldMatrix();

// r3dRenderer->pd3ddev->SetIndices( g_IB );
 r3dRenderer->pd3ddev->SetStreamSource( 0, g_pVB, 0, sizeof(R3D_WORLD_VERTEX) );

 if (bTextured ) {

  for (int i=0;i<NumMatChunks;i++)
  {
   if (MatChunks[i].Mat->Texture)
      r3dRenderer->SetTexture(MatChunks[i].Mat->Texture);
   else
      r3dRenderer->SetTexture( __r3dShadeTexture[2]);

//   r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHAREF, (unsigned long)MatChunks[i].Mat->AlphaRef);

//   r3dOutToLog("%s\t\t%d\t%d\t%f\n", MatChunks[i].Mat->Name, MatChunks[i].StartIndex, MatChunks[i].EndIndex, float((MatChunks[i].EndIndex-MatChunks[i].StartIndex))/3.0f);
  
//   r3dDrawNElements ((MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3, MatChunks[i].StartIndex, MatChunks[i].EndIndex-MatChunks[i].StartIndex, MatChunks[i].StartIndex);

   if (VBMode)
    r3dRenderer->pd3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,  0,
      0, NumVertices, MatChunks[i].StartIndex, (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3);
   else
    r3dRenderer->pd3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 
      MatChunks[i].StartIndex, 
      (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3);


   r3dRenderer->Stats.NumTrianglesRendered += (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3;
   r3dRenderer->Stats.AverageStripLength += (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3;
   r3dRenderer->Stats.AverageStripLength /= 2;
   r3dRenderer->Stats.NumDraws++;
  }
 }
 else
 {
   if (VBMode)
    r3dRenderer->pd3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,  0, 0, NumVertices, 0,NumFaces);
   else
    r3dRenderer->pd3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, NumFaces);

   r3dRenderer->Stats.NumTrianglesRendered += NumFaces;
   r3dRenderer->Stats.AverageStripLength += NumFaces;
   r3dRenderer->Stats.AverageStripLength /= 2;
   r3dRenderer->Stats.NumDraws++;
 }


 D3DXMATRIX 	mWorld;
 D3DXMatrixIdentity(&mWorld);
 r3dRenderer->pd3ddev->SetTransform(D3DTS_WORLD,      &mWorld);
}




//-----------------------------------------------------------------------
void r3dMesh::DrawLightmap(int bTextured, int bLights, r3dColor TintColor)
//-----------------------------------------------------------------------
{
  if(!IsValid())
    return;

 BuildWorldMatrix();

// r3dRenderer->pd3ddev->SetIndices( g_IB );
 r3dRenderer->pd3ddev->SetStreamSource( 0, g_pVB, 0, sizeof(R3D_WORLD_VERTEX) );

 int bDynaLightsRecalc = 0;

// for(int i = 0; i < 32; i++)
//          r3dRenderer->pd3ddev->LightEnable( i, 0 );


 r3dRenderer->pd3ddev->SetRenderState( D3DRS_AMBIENT, r3dRenderer->AmbientColor.GetPacked());
/*
 if(DynaLights && bLights)
 {
  int		nLights;

  // detect which lights we intersect and store them into array.
  nLights = DynaLights->FillVisibleArray(BBox);

  if(nLights == 0)
  {
   if(IsFlagSet(obfDynaLightApplied)) ResetDynamicLights();

   SetFlag(obfDynaLightApplied, 0);
   bDynaLightsRecalc = 0;
  } 
  else
  {
     SetFlag(obfDynaLightApplied, 1);
     bDynaLightsRecalc = 1;
  }
 }

 if (bLights)
 {
  r3dRenderer->pd3ddev->SetRenderState( D3DRS_LIGHTING, TRUE );
  r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORVERTEX, 0);
 }

 if(bDynaLightsRecalc)
 {
  int CLight = 0;

  for(i = 0; i < DynaLights->nVisibleLights; i++)
  {
   r3dLight *L = DynaLights->VisibleLights[i];

   if (L->bOn) L->SetD3DLight(CLight++, TRUE);
  }  
  //r3dOutToLog("OBJ %s  Lights %d\n", Name, CLight);

 }
*/

 r3dRenderer->SetTexture(LightmapTex,1);

 r3dRenderer->pd3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
 r3dRenderer->pd3ddev->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
 r3dRenderer->pd3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

 r3dRenderer->pd3ddev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
 r3dRenderer->pd3ddev->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
 r3dRenderer->pd3ddev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT );
 r3dRenderer->pd3ddev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);


 if (bTextured ) {

  for (int i=0;i<NumMatChunks;i++)
  {
   r3dRenderer->SetTexture(MatChunks[i].Mat->Texture);
/*
   D3DMATERIAL8 mtrl;
   ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
   mtrl.Diffuse.r = 1.0f; //mtrl.Ambient.r = float(TintColor.R)/ 255.0f;
   mtrl.Diffuse.g = 1.0f; //mtrl.Ambient.g = float(TintColor.G)/ 255.0f;
   mtrl.Diffuse.b = 1.0f; //mtrl.Ambient.b = float(TintColor.B)/ 255.0f;
   mtrl.Diffuse.a = 1.0f; //mtrl.Ambient.a = float(MatChunks[i].Mat->DiffuseColor.A)/255.0f;
//   mtrl.Emissive.r = 0.1f;
//   mtrl.Emissive.g = 0.1f;
//   mtrl.Emissive.b = 0.1f;
//   mtrl.Emissive.a = 1.0f;

   r3dRenderer->pd3ddev->SetMaterial( &mtrl );
*/

/*
   r3dRenderer->pd3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,  0,
    MatChunks[i].StartIndex, 
    MatChunks[i].EndIndex-MatChunks[i].StartIndex, 
    MatChunks[i].StartIndex,
    (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3);
*/

  r3dRenderer->pd3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 
    MatChunks[i].StartIndex, 
    (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3);

   r3dRenderer->Stats.NumTrianglesRendered += (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3;
   r3dRenderer->Stats.AverageStripLength += (MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3;
   r3dRenderer->Stats.AverageStripLength /= 2;
   r3dRenderer->Stats.NumDraws++;
  }
 }
 else
 {
  D3DMATERIAL9 mtrl;
  ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
  mtrl.Diffuse.r = mtrl.Ambient.r = float(TintColor.R)/ 255.0f;
  mtrl.Diffuse.g = mtrl.Ambient.g = float(TintColor.G)/ 255.0f;
  mtrl.Diffuse.b = mtrl.Ambient.b = float(TintColor.B)/ 255.0f;
  mtrl.Diffuse.a = 1.0f; //mtrl.Ambient.a = 1.0f;

  r3dRenderer->pd3ddev->SetMaterial( &mtrl );

//  r3dRenderer->pd3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,  0, 0, NumFaces*3, 0,NumFaces);

  r3dRenderer->pd3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, NumFaces);

  r3dRenderer->Stats.NumTrianglesRendered += NumFaces;
  r3dRenderer->Stats.AverageStripLength += NumFaces;
  r3dRenderer->Stats.AverageStripLength /= 2;
  r3dRenderer->Stats.NumDraws++;
 }

/*
 // clear array pointer, so light recalc will not happen at next frame.
 DynaLights = NULL;
 if(bDynaLightsRecalc)
 {
  r3dRenderer->pd3ddev->SetRenderState( D3DRS_LIGHTING, FALSE );
  r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORVERTEX, 1);
 }

 r3dRenderer->pd3ddev->SetRenderState( D3DRS_AMBIENT, r3dRenderer->AmbientColor.GetPacked() );
*/

 r3dRenderer->pd3ddev->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
 r3dRenderer->pd3ddev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
 r3dRenderer->pd3ddev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

 D3DXMATRIX 	mWorld;
 D3DXMatrixIdentity(&mWorld);
 r3dRenderer->pd3ddev->SetTransform(D3DTS_WORLD,      &mWorld);

 D3DXMATRIX ShaderMat;
 ShaderMat =  mWorld * 	r3dRenderer->mCamera * r3dRenderer->mProj;
 D3DXMatrixTranspose( &ShaderMat, &ShaderMat );

 r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  4 );

//  for(i = 0; i < 32; i++)
//          r3dRenderer->pd3ddev->LightEnable( i, 0 );

 r3dRenderer->Flush();

}






#define TRIRAYCOLL_EPSILON	0.000001

#define CROSS(dest,v1,v2) \
  dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
  dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
  dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) \
  (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) \
  dest[0]=v1[0]-v2[0]; \
  dest[1]=v1[1]-v2[1]; \
  dest[2]=v1[2]-v2[2]; 

int intersect_triangle_floats_cull(float orig[3], float dir[3],
                   float vert0[3], float vert1[3], float vert2[3],
                   float *t, float *u, float *v)
{
   float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   float det,inv_det;

   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

   if(det < TRIRAYCOLL_EPSILON)
      return 0;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec);
   if(*u < 0.0f || *u > det)
      return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

    /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec);
   if(*v < 0.0f || *u + *v > det)
      return 0;

   /* calculate t, scale parameters, ray intersects triangle */
   *t = DOT(edge2, qvec);
   // Intersection point is valid if distance is positive (else it can just be a face behind the orig point)
   if(*t < 0.0f) 
     return 0;

   inv_det = 1.0f / det;
   *t *= inv_det;
   *u *= inv_det;
   *v *= inv_det;
   return 1;
}

int intersect_triangle_floats_nocull(float orig[3], float dir[3],
                   float vert0[3], float vert1[3], float vert2[3],
                   float *t, float *u, float *v)
{
   float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   float det,inv_det;

   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

   if(det > -TRIRAYCOLL_EPSILON && det < TRIRAYCOLL_EPSILON)
     return 0;
   inv_det = 1.0f / det;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec) * inv_det;
   if(*u < 0.0 || *u > 1.0)
     return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec) * inv_det;
   if(*v < 0.0 || *u + *v > 1.0)
     return 0;

   /* calculate t, ray intersects triangle */
   *t = DOT(edge2, qvec) * inv_det;
   // Intersection point is valid if distance is positive (else it can just be a face behind the orig point)
   if(*t < 0.0f) 
     return 0;

   return 1;
}

#undef SUB
#undef DOT
#undef CROSS

BOOL r3dMesh::ContainsRay(const r3dPoint3D& vStart, const r3dPoint3D& vRay, float RayLen, float *ClipDist, int *FaceN)
{
  if(!NumVertices) return FALSE;

  float 	MinClipDist = 9999999;
  int		MinFace     = -1;

/*
  // old code.. and also used for debugging transformed object
  if(m_pTransformedObj) {
    r3dMesh &t = *m_pTransformedObj;

    for(int i=0; i<NumFaces; i++)
    {
      float dist;
      if(t.FaceList[i].ContainsRay(vStart, vRay, RayLen, &dist)) {
        if(dist < MinClipDist) {
          MinClipDist = dist;
          MinFace     = i;
        }
      }
    }

    if(MinFace != -1)
    {
      *ClipDist = MinClipDist;
      *FaceN    = MinFace;
      return TRUE;
    }

    return FALSE;
  }
*/
  
  //NOTE: add caching of world matrix after changing RotateMatrix

  // code from BuildWorldMatrix
  D3DXMATRIX  m1, m2, mWorld, mMove;
  D3DXMatrixTranslation(&mMove, vMove.X, vMove.Y, vMove.Z);
  D3DXMatrixTranslation(&m1, vPivot.X, vPivot.Y, vPivot.Z );
  D3DXMatrixTranslation(&m2, -vPivot.X, -vPivot.Y, -vPivot.Z );
  D3DXMatrixMultiply(&mWorld, &mMove,  &m2);
  D3DXMatrixMultiply(&mWorld, &mWorld, &RotateMatrix);
  D3DXMatrixMultiply(&mWorld, &mWorld, &m1);

  D3DXMATRIX mwI, mrI;
  D3DXMatrixInverse(&mwI, NULL, &mWorld);
  D3DXMatrixInverse(&mrI, NULL, &RotateMatrix);
  
  // transform ray to object local space
  r3dPoint3D vRay1;
  r3dPoint3D vStart1;
  D3DXVec3TransformNormal(vRay1.d3dx(),  vRay.d3dx(),   &mrI);
  D3DXVec3TransformCoord(vStart1.d3dx(), vStart.d3dx(), &mwI);

  for(int i=0; i<NumFaces; i++) {
    r3dFace &f = FaceList[i];
    
    float t, u, v;   
    if(!intersect_triangle_floats_cull((float *)vStart1, (float *)vRay1, (float *)*f.Vertices[0], (float *)*f.Vertices[1], (float *)*f.Vertices[2], &t, &u, &v)) 
      continue;

/*
    if(t<0) {
      r3dOutToLog("r3dPoint3D vStart(%f, %f, %f)\n", vStart1.X, vStart1.Y, vStart1.Z);
      r3dOutToLog("r3dPoint3D vRay1(%f, %f, %f)\n", vRay1.X, vRay1.Y, vRay1.Z);
      r3dOutToLog("r3dPoint3D f.Vertices[0](%f, %f, %f)\n", f.Vertices[0].X, f.Vertices[0].Y, f.Vertices[0].Z);
      r3dOutToLog("r3dPoint3D f.Vertices[1](%f, %f, %f)\n", f.Vertices[1].X, f.Vertices[1].Y, f.Vertices[1].Z);
      r3dOutToLog("r3dPoint3D f.Vertices[2](%f, %f, %f)\n", f.Vertices[2].X, f.Vertices[2].Y, f.Vertices[2].Z);
      intersect_triangle_floats_cull((float *)vStart1, (float *)vRay1, (float *)*f.Vertices[0], (float *)*f.Vertices[1], (float *)*f.Vertices[2], &t, &u, &v);
      intersect_triangle_floats_nocull((float *)vStart1, (float *)vRay1, (float *)*f.Vertices[0], (float *)*f.Vertices[1], (float *)*f.Vertices[2], &t, &u, &v);
    }
    
    extern void U_DLog(const char* , ...);
    U_DLog("C %s %f\n", Name, t);
*/    
    if(t < MinClipDist) {
      MinFace     = i;
      MinClipDist = t;
    }
  }

  if(MinFace != -1) 
  {
    *ClipDist = MinClipDist;
    *FaceN    = MinFace;
    return TRUE;
  }

  return FALSE;
}


BOOL r3dMesh::ContainsQuickRay(const r3dPoint3D& vStart, const r3dPoint3D& vRay, float RayLen, float *ClipDist, int *FaceN)
{
  if(!NumVertices) return FALSE;

  float 	MinClipDist = 9999999;
  int		MinFace     = -1;
  
  //NOTE: add caching of world matrix after changing RotateMatrix

  // code from BuildWorldMatrix
  D3DXMATRIX  m1, m2, mWorld, mMove;
  D3DXMatrixTranslation(&mMove, vMove.X, vMove.Y, vMove.Z);
  D3DXMatrixTranslation(&m1, vPivot.X, vPivot.Y, vPivot.Z );
  D3DXMatrixTranslation(&m2, -vPivot.X, -vPivot.Y, -vPivot.Z );
  D3DXMatrixMultiply(&mWorld, &mMove,  &m2);
  D3DXMatrixMultiply(&mWorld, &mWorld, &RotateMatrix);
  D3DXMatrixMultiply(&mWorld, &mWorld, &m1);

  D3DXMATRIX mwI, mrI;
  D3DXMatrixInverse(&mwI, NULL, &mWorld);
  D3DXMatrixInverse(&mrI, NULL, &RotateMatrix);
  
  // transform ray to object local space
  r3dPoint3D vRay1;
  r3dPoint3D vStart1;
  D3DXVec3TransformNormal(vRay1.d3dx(),  vRay.d3dx(),   &mrI);
  D3DXVec3TransformCoord(vStart1.d3dx(), vStart.d3dx(), &mwI);

  for(int i=0; i<NumFaces; i++) {
    r3dFace &f = FaceList[i];

    float t, u, v;   
    if(intersect_triangle_floats_cull((float *)vStart1, (float *)vRay1, (float *)*f.Vertices[0], (float *)*f.Vertices[1], (float *)*f.Vertices[2], &t, &u, &v)) {
      *ClipDist = t;
      *FaceN    = i;
      return TRUE;
    }
  }

  return FALSE;
}

static int IsMatrixTheSame(D3DXMATRIX &m1, D3DXMATRIX &m2)
{
  for(int i=0; i<16; i++) {
    if(fabs(m1[i] - m2[i]) > 0.0001f)
      return 0;
  }
  
  return 1;
}

void r3dMesh::UpdateTransformedObject()
{
  if(!m_pTransformedObj) {

    // if object wasn't rotated or teleported at all
    D3DXMATRIX m0;
    D3DXMatrixIdentity(&m0);
    if(IsMatrixTheSame(m0, RotateMatrix) && vMove.Length() <= 0.0001f) 
      return;

    //r3dOutToLog("r3dMesh::UpdateTransformedObject() created for %s\n", Name);
    
    m_pTransformedObj = new r3dMesh;
    r3dMesh &t = *m_pTransformedObj;

    // copy vertex & faces data there!
    t.InitVertsList(NumVertices);
    t.InitFaceList(NumFaces);

    for(int i=0; i<NumVertices + 1; i++) {
      t.WorldVertexList[i] = WorldVertexList[i];
    }
   
    for(int i=0; i < NumFaces; i++)
    {
      for(int k = 0; k < 3; k++)
      {
        int idx;
        idx = FaceList[i].VertIndex[k];

        t.FaceList[i].VertIndex[k]      = idx;
        t.FaceList[i].Vertices[k]   	= &t.WorldVertexList[idx];
        t.FaceList[i].Edges[k]   	= FaceList[i].Edges[k];
      }
      
      t.FaceList[i].CalcFaceNormal();
    }

    t.NumEdges = NumEdges;
  }

  // create world matrix for that object - code from BuildWorldMatrix()
  D3DXMATRIX  m1, m2, mWorld, mMove;
  D3DXMatrixTranslation(&mMove, vMove.X, vMove.Y, vMove.Z);
  D3DXMatrixTranslation(&m1, vPivot.X, vPivot.Y, vPivot.Z );
  D3DXMatrixTranslation(&m2, -vPivot.X, -vPivot.Y, -vPivot.Z );
  D3DXMatrixMultiply(&mWorld, &mMove,  &m2);
  D3DXMatrixMultiply(&mWorld, &mWorld, &RotateMatrix);
  D3DXMatrixMultiply(&mWorld, &mWorld, &m1);

  if(IsMatrixTheSame(mWorld, m_CachedTransform))
    return;

  //r3dOutToLog("  r3dMesh::UpdateTransformedObject() called for %s\n", Name);

  // retransform vertices and recalculate face normals    
  r3dMesh &t = *m_pTransformedObj;

  D3DXVec3TransformCoordArray(t.WorldVertexList[0].d3dx(), sizeof(r3dPoint3D), WorldVertexList[0].d3dx(), sizeof(r3dPoint3D), &mWorld, NumVertices + 1);
  m_CachedTransform = mWorld;
  
  for(int i=0; i<NumFaces; i++) {
    t.FaceList[i].CalcFaceNormal();
  }
  return;
}
