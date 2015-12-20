//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "Exporter.h"
#include "VertexCandidate.h"
#include "BoneCandidate.h"
#include "SkeletonCandidate.h"
#include "math.h"
#include "MeshNormalSpec.h"

//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void kill_denormal_by_quantization(float& val)
{
  static const float anti_denormal = 1e-18f;
  val += anti_denormal;
  val -= anti_denormal;
}

int test_denormal(float val)
{
  const int x = *reinterpret_cast<const int *>(&val);
  // needs 32-bit int
  const int abs_mantissa = x & 0x007FFFFF;
  const int biased_exponent = x & 0x7F800000;
  if(biased_exponent == 0 && abs_mantissa != 0) {
    return 1;
  }
  
  return 0;
}


//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

CMaxMesh::CMaxMesh()
{
	m_pNode = 0;
	m_pIMesh = 0;
	m_pTriObject = 0;
	m_bDelete = FALSE;
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMaxMesh::~CMaxMesh()
{
  if(m_bDelete) delete m_pTriObject;
}

static BOOL TMNegParity(Matrix3 &m)
{
  return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}

//----------------------------------------------------------------------------//
// Create a max node instance                                                 //
//----------------------------------------------------------------------------//
void CMaxMesh::Create(INode *pNode, TriObject *pTriObject, BOOL bDelete, TimeValue time)
{
  // check for valid mesh
  if(pTriObject == 0)
    ThrowError("Invalid TriObject handle.");

  m_time       = time;
  m_pNode      = pNode;
  m_pTriObject = pTriObject;
  m_pIMesh     = &m_pTriObject->GetMesh();

  m_bDelete    = bDelete;

  // recursively create materials
  FillMaterials(m_pNode->GetMtl());

  // build all normals if necessary
  m_pIMesh->checkNormals(TRUE);
  m_pIMesh->SpecifyNormals();

  // precalculate the object transformation matrix
  m_ObjTM  = m_pNode->GetObjectTM(time);
  m_NodeTM = m_pNode->GetNodeTM(time);

  // get the physique modifier
  m_pPhysiqueModifier = gMaxHelper.FindModifier(pNode, Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));
  m_pSkinModifier     = gMaxHelper.FindModifier(pNode, SKIN_CLASSID);

  // Order of the vertices. Get 'em counter clockwise if the objects is
  // negatively scaled.
  BOOL negScale = TMNegParity(pNode->GetObjTMAfterWSM(time));
  if(!negScale) {
    m_iFaceOrder[0] = 2;
    m_iFaceOrder[1] = 1;
    m_iFaceOrder[2] = 0;
  } else {
    m_iFaceOrder[0] = 0;
    m_iFaceOrder[1] = 1;
    m_iFaceOrder[2] = 2;
  }

  m_pIMesh->faceSel.ClearAll();
  m_pIMesh->vertSel.ClearAll();

/*
  if(m_pIMesh->RemoveDegenerateFaces())
    U_Log("Mesh had Degenerated Faces\n");
  if(m_pIMesh->RemoveIllegalFaces())
    U_Log("Mesh had Illegal Faces\n");
*/    

  return;
}

void CMaxMesh::FillMaterials(Mtl *pMtl)
{
  if(pMtl == 0) 
    return;

  if(pMtl->ClassID().PartA() != MULTI_CLASS_ID) 
    m_vectorMtl.push_back((Mtl *)pMtl);

  // handle all submaterials
  for(int subId = 0; subId < pMtl->NumSubMtls(); subId++)
    FillMaterials(pMtl->GetSubMtl(subId));

  return;
}

int CMaxMesh::GetFaceMaterialId(int faceId)
{
  // get the material count of this mesh
  int materialCount = int(m_vectorMtl.size());
  if(!materialCount)
    return 0;

  // get the material id of the face
  int materialId    = m_pIMesh->getFaceMtlIndex(faceId);

  // make sure we return a valid material id in all cases:
  // 3ds max assigns predefined material ids to primitives. these material
  // ids can be higher than the actual material count of the mesh itself.
  // the official workaround is to calculate the modulo of the value to the
  // number of existing materials.
  return materialId % materialCount;
}

CMaxMaterial *CMaxMesh::GetFaceMaxMaterial(int faceId)
{
  // get the material count of this mesh
  int materialCount = int(m_vectorMtl.size());
  if(materialCount == 0)
    return CMaxMaterial::FindMaterial(NULL);

  // get the material id of the face
  int materialId    = m_pIMesh->getFaceMtlIndex(faceId);

  // make sure we return a valid material id in all cases:
  // 3ds max assigns predefined material ids to primitives. these material
  // ids can be higher than the actual material count of the mesh itself.
  // the official workaround is to calculate the modulo of the value to the
  // number of existing materials.
  return CMaxMaterial::FindMaterial(m_vectorMtl[materialId % materialCount]);
}

Mtl* CMaxMesh::GetSubmeshMtl(int submeshId) 
{
  if(m_vectorMtl.size() == 0)
    return NULL;

  // check if the submesh id is valid
  if((submeshId < 0) || (submeshId >= (int)m_vectorMtl.size()))
    ThrowError("Invalid handle.");

  return m_vectorMtl[submeshId];
}

//----------------------------------------------------------------------------//
// Get the vertex  for a given vertex of a given face of the mesh    //
//----------------------------------------------------------------------------//
CVertex *CMaxMesh::GetVertex(int faceId, int faceVertexId)
{
  // check for valid mesh and physique modifier
  if(m_pIMesh == 0)
    ThrowError("Invalid handle.");

  // check if face id is valid
  if((faceId < 0) || (faceId >= m_pIMesh->getNumFaces())) 
    ThrowError("Invalid face id found.");

  // check if face vertex id is valid
  if((faceVertexId < 0) || (faceVertexId >= 3))
    ThrowError("Invalid face vertex id found.");

  // allocate a new vertex 
  CVertex *pVertex = new CVertex();
  if(pVertex == 0)
    ThrowError("Memory allocation failed.");

  // get corner & vertex id
  int cornerId = m_iFaceOrder[faceVertexId];
  int vertexId = m_pIMesh->faces[faceId].v[cornerId];

 // position / normal
  pVertex->m_position = m_pIMesh->getVert(vertexId) * m_ObjTM;
  pVertex->m_normal   = GetVertexNormal(faceId, vertexId, cornerId) * Inverse(Transpose(m_ObjTM));
  pVertex->m_normal   = pVertex->m_normal.Normalize();
  
 // map 0 - vertex colors  
  if(theExporter.m_ExpVtxColors && m_pIMesh->numCVerts > 0)
  { 
    pVertex->m_color = m_pIMesh->vertCol[m_pIMesh->vcFace[faceId].t[m_iFaceOrder[faceVertexId]]];
  }

 // set UVs - maps starts from 1
  pVertex->m_iMapChannels = 0;
  for(int iMap = 1; iMap < m_pIMesh->getNumMaps(); iMap++) 
  {
    if(iMap >= 3) // no need to scan more that 1/2 uvs
      break;
    if(!m_pIMesh->mapSupport(iMap)) 
      continue;
    
    const TVFace* pTVFace = m_pIMesh->mapFaces(iMap);
    if(!pTVFace) 
      continue;

    const UVVert* pUVVert = m_pIMesh->mapVerts(iMap);

    UVVert uvVert;
    uvVert = pUVVert[pTVFace[faceId].t[m_iFaceOrder[faceVertexId]]];
    
    // check for material and it's UVGen
    if(m_vectorMtl.size()) 
    {
      // get the material id of the face
      int materialId = GetFaceMaterialId(faceId);
      if((materialId < 0) || (materialId >= (int)m_vectorMtl.size()))
        ThrowError("Invalid material id found. #1");

      // get the material of the face
      Mtl *pMtl = m_vectorMtl[materialId];

      // loop through all the mapping channels
      for(int iSubMap = 0; iSubMap < pMtl->NumSubTexmaps(); iSubMap++) {
        Texmap* subTex = pMtl->GetSubTexmap(iSubMap);
        if(!subTex) continue;

        // skip applying uv gen to animated materials
        StdUVGen* pStdUVGen  = (StdUVGen *)subTex->GetTheUVGen();
        if(pStdUVGen && pStdUVGen->IsAnimated()) continue;

        // apply a possible uv generator to the mapping channel, if we don't have UV anim
        if(subTex->GetMapChannel() == iMap) {
          Matrix3 tmUV;
          subTex->GetUVTransform(tmUV);
          uvVert = uvVert * tmUV;
        }
      }
    }

    // sometimes, uv coordinates is denormalized. this may be a error, but we will skip it
    if(test_denormal(uvVert.x) || test_denormal(uvVert.y)) {
      //U_Log("!rface %d vertex %d have very small UV %f %f\n", faceId, faceVertexId, uvVert.x, uvVert.y);
      //m_pIMesh->faceSel.Set(faceId, 1);
      //m_pIMesh->vertSel.Set(vertexId, 1);
      uvVert.x = 0;
      uvVert.y = 0;
    }

    if(!_finite(uvVert.x) || !_finite(uvVert.y)) {
      m_pIMesh->faceSel.Set(faceId, 1);
      m_pIMesh->vertSel.Set(vertexId, 1);
      char buf[512];
      sprintf(buf, "face %d vertex %d have undefined UV, export failed", faceId, faceVertexId);
      ThrowError(buf);
      uvVert.x = 0;
      uvVert.y = 0;
    }

    const float BAD_UV_LIMIT = 14.0f;	// 65k packed values, 4k texture
    if(uvVert.x < -BAD_UV_LIMIT || uvVert.x > BAD_UV_LIMIT || uvVert.y < -BAD_UV_LIMIT || uvVert.y > BAD_UV_LIMIT) 
    {
      static int shown   = 0;
      static int needfix = 0;
      if(!shown) {
        shown = 1;
        char buf[512];
        sprintf(buf, "Object %s have suspiciously large UVs on face %d vertex %d\nu:%f v:%f, map:%d\n\nPress YES to FIX it, NO to continue", 
		m_pNode->GetName(), faceId, faceVertexId, uvVert.x, uvVert.y, iMap - 1);
        if(IDYES == MessageBox(AfxGetMainWnd()->GetSafeHwnd(), buf, "Big UVs", MB_YESNO | MB_ICONQUESTION))
          needfix = 1;
      }

      if(needfix) {
        U_Log("!rface %d vertex %d have big UV %f %f\n", faceId, faceVertexId, uvVert.x, uvVert.y);
        while(uvVert.x < -BAD_UV_LIMIT) uvVert.x += 1;
        while(uvVert.x >  BAD_UV_LIMIT) uvVert.x -= 1;
        while(uvVert.y < -BAD_UV_LIMIT) uvVert.y += 1;
        while(uvVert.y >  BAD_UV_LIMIT) uvVert.y -= 1;
      }
    }

    pVertex->SetTextureCoordinate(iMap - 1, uvVert.x, uvVert.y);
  }

  FillVertexWeight(pVertex, vertexId);

  return pVertex;
}

class CRAII_ISkin
{
  public:
	Modifier	*pBase;
	ISkin		*pRes;
	CRAII_ISkin(Modifier *p) {
	  pBase = p;
	  pRes  = (ISkin*)pBase->GetInterface(I_SKIN);
          if(pRes == NULL)
            ThrowError("ISkin modifier interface not found.");
	}
	~CRAII_ISkin() {
	  pBase->ReleaseInterface(I_SKIN, pRes);
	}
	operator ISkin*()   { return pRes; }
	ISkin* operator->() { return pRes; }
};

 

class CRAII_ISkinContextData
{
  public:
	ISkin	  *pBase;
	ISkinContextData *pRes;
	CRAII_ISkinContextData(ISkin *p, INode *pNode) {
	  pBase = p;
	  pRes  = pBase->GetContextInterface(pNode);
          if(pRes == 0)
            ThrowError("ISkinContextData interface not found.");
	}
	~CRAII_ISkinContextData() {
          //pBase->ReleaseContextInterface(pRes);
	}
	operator ISkinContextData*()   { return pRes; }
	ISkinContextData* operator->() { return pRes; }
};

class CRAII_IPhysiqueExport 
{
  public:
	Modifier	*pBase;
	IPhysiqueExport *pRes;
	CRAII_IPhysiqueExport(Modifier *p) {
	  pBase = p;
	  pRes  = (IPhysiqueExport*)pBase->GetInterface(I_PHYINTERFACE);
          if(pRes == NULL)
            ThrowError("Physique modifier interface not found.");
	}
	~CRAII_IPhysiqueExport() {
	  pBase->ReleaseInterface(I_PHYINTERFACE, pRes);
	}
	operator IPhysiqueExport*()   { return pRes; }
	IPhysiqueExport* operator->() { return pRes; }
};

class CRAII_IPhyContextExport
{
  public:
	IPhysiqueExport	  *pBase;
	IPhyContextExport *pRes;
	CRAII_IPhyContextExport(IPhysiqueExport *p, INode *pNode) {
	  pBase = p;
	  pRes  = pBase->GetContextInterface(pNode);
          if(pRes == 0)
            ThrowError("Physique modifier interface not found.");
	}
	~CRAII_IPhyContextExport() {
          pBase->ReleaseContextInterface(pRes);
	}
	operator IPhyContextExport*()   { return pRes; }
	IPhyContextExport* operator->() { return pRes; }
};

void CMaxMesh::FillVertexWeight(CVertex *pVertex, int vertexId)
{
  char buf[1024];

  if(m_pSkinModifier) 
  { 
    CRAII_ISkin             skin(m_pSkinModifier);
    CRAII_ISkinContextData  sd(skin, m_pNode);

    int cBones = sd->GetNumAssignedBones(vertexId);
    if(cBones > 4) {
      sprintf(buf, "There is vertex idx %d in mesh %s that have %d affecting bones in skin\nExport failed", vertexId, m_pNode->GetName(), cBones);
      ThrowError(buf);
    }

    for(int i=0; i<cBones; i++) {
      int   b = sd->GetAssignedBone(vertexId, i);
      float w = sd->GetBoneWeight(vertexId, i);
      
      INode *bn = skin->GetBone(b);
      pVertex->AddInfluence(bn, w);
    }

    return;
  }

  if(m_pPhysiqueModifier)
  {
    // create a physique export interface
    CRAII_IPhysiqueExport   pPhysiqueExport(m_pPhysiqueModifier);
    // create a context export interface
    CRAII_IPhyContextExport pContextExport(pPhysiqueExport, m_pNode);

    // set the flags in the context export interface
    pContextExport->ConvertToRigid(TRUE);
    pContextExport->AllowBlending(TRUE);

    // get the vertex export interface
    IPhyVertexExport *pVertexExport;
    pVertexExport = (IPhyVertexExport *)pContextExport->GetVertexInterface(vertexId);
    if(pVertexExport == 0) 
      ThrowError("Vertex export interface not found.");

    // get the vertex type
    int vertexType;
    vertexType = pVertexExport->GetVertexType();

    // handle the specific vertex type
    if(vertexType == RIGID_TYPE)
    {
      // typecast to rigid vertex
      IPhyRigidVertex *pTypeVertex = (IPhyRigidVertex *)pVertexExport;

      pVertex->AddInfluence(pTypeVertex->GetNode(), 1.0f);
    }
    else if(vertexType == RIGID_BLENDED_TYPE)
    {
      // typecast to blended vertex
      IPhyBlendedRigidVertex *pTypeVertex = (IPhyBlendedRigidVertex *)pVertexExport;

      // loop through all influencing bones
      int cBones = pTypeVertex->GetNumberNodes();
      if(cBones > 4) {
        sprintf(buf, "There is vertex idx %d in mesh %s that have %d affecting bones in physique\nExport failed", vertexId, m_pNode->GetName(), cBones);
        ThrowError(buf);
      }
      
      for(int nodeId = 0; nodeId < cBones; nodeId++)
      {
        INode* bn = pTypeVertex->GetNode(nodeId);
        float  w  = pTypeVertex->GetWeight(nodeId);
        
        if(w < 0.0001f) {
          //U_Log("!dWarning: Vertex %d have zero influence over bone %s\n", vertexId, bn->GetName());
        }

        pVertex->AddInfluence(bn, w);
      }
    }
    
    return;
  } 
  
  return;
}

//----------------------------------------------------------------------------//
// Get the absolute normal of a given vertex of a given face                  //
//----------------------------------------------------------------------------//
Point3 CMaxMesh::GetVertexNormal(int faceId, int vertexId, int cornerId)
{
#if 1
	// check if we have "edit normal" modified normals
	MeshNormalSpec* meshNorm = m_pIMesh->GetSpecifiedNormals();
	if(meshNorm && meshNorm->GetNumFaces()) {
		if(faceId >= meshNorm->GetNumFaces())
			ThrowError("bad meshNorm->GetNumFaces()");
		int normID = meshNorm->Face(faceId).GetNormalID(cornerId);
		if(normID == -1) 
			ThrowError("bad GetNormalID()");
		if(meshNorm->GetNormalExplicit(normID)) {
			return meshNorm->Normal(normID);
		}
	}
#endif

	// get the "rendered" vertex
	RVertex *pRVertex;
	pRVertex = m_pIMesh->getRVertPtr(vertexId);

	// get the face
	Face *pFace;
	pFace = &m_pIMesh->faces[faceId];

	// get the smoothing group of the face
	DWORD smGroup;
	smGroup = pFace->smGroup;

	// get the number of normals
	int normalCount;
	normalCount = pRVertex->rFlags & NORCT_MASK;

	// check if the normal is specified ...
	if(pRVertex->rFlags & SPECIFIED_NORMAL)
	{
		return pRVertex->rn.getNormal();
	}
	// ... otherwise, check for a smoothing group
	else if((normalCount > 0) && (smGroup != 0))
	{
		// If there is only one vertex is found in the rn member.
		if(normalCount == 1)
		{
			return pRVertex->rn.getNormal();
		}
		else
		{
			int normalId;
			for(normalId = 0; normalId < normalCount; normalId++)
			{
				if(pRVertex->ern[normalId].getSmGroup() & smGroup)
				{
					return pRVertex->ern[normalId].getNormal();
				}
			}
		}
	}

	// if all fails, return the face normal
	return m_pIMesh->getFaceNormal(faceId);
}

//----------------------------------------------------------------------------//
// Get the transpose of a given matrix                                        //
//----------------------------------------------------------------------------//

Matrix3 CMaxMesh::Transpose(Matrix3& matrix)
{
	float transpose[4][3];

	int row;
	for(row = 0; row < 3; row++)
	{
		int column;
		for(column = 0; column < 3; column++)
		{
			transpose[column][row] = matrix.GetAddr()[row][column];
		}
	}

	int column;
	for(column = 0; column < 3; column++)
	{
		transpose[3][column] = 0;
	}

	Matrix3 transposeMatrix(transpose);
	transposeMatrix.NoTrans();

	return transposeMatrix;
}

//----------------------------------------------------------------------------//
