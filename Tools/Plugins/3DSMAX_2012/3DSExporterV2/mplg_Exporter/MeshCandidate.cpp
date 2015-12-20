#include "StdAfx.h"
#include "Exporter.h"
#include "MeshCandidate.h"
#include "VertexCandidate.h"
#include "vcacheopt.h"

	int	cmesh_debug = 0;
extern	int	vtx_sort_debug;
extern	int	vtx_sort_stats[64];

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//
CMesh::CMesh()
{
  m_pMesh = 0;
  memset(&vtx_sort_stats, 0, sizeof(vtx_sort_stats));
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//
CMesh::~CMesh()
{
  Clear();
}

//----------------------------------------------------------------------------//
// Clear this mesh                                                   //
//----------------------------------------------------------------------------//
void CMesh::Clear()
{
  // delete all submaterials
  int i;
  for(i=0; i<m_SubMaterials.size(); i++) {
    SubMaterial *sbm = m_SubMaterials[i];
    // and vertices there
    for(int j=0; j<sbm->vVertices.size(); j++) {
      delete sbm->vVertices[j];
    }
    sbm->vVertices.clear();
    sbm->vIndices.clear();

    delete sbm;
  }
  m_SubMaterials.clear();

  m_vVertices.clear();
  m_vIndices.clear();
  
  // delete max mesh
  if(m_pMesh) {
    delete m_pMesh;
    m_pMesh = NULL;
  }

  m_iVertexCount = 0;
  m_iFaceCount   = 0;
}

int CMesh::AddMesh(INode* pNode, TimeValue time)
{
D_LOG(1);
  gMaxHelper.StartProgressInfo(" AddMesh");

  // get the original mesh if we're not exporting vertex anim, otherwise get mesh at that timeframe
  m_pMesh = gMaxHelper.GetMesh(pNode, time);
  if(m_pMesh == 0) return FALSE;

  // get the number of materials & faces of the mesh
  int MeshMaterialCount = int(m_pMesh->m_vectorMtl.size());
  int MeshFaceCount     = m_pMesh->m_pIMesh->getNumFaces();
  
  if(MeshFaceCount == 0) {
    U_Log("!B!rMesh %s have zero faces!\n", pNode->GetName());
    ThrowError("some of meshes have zero faces");
    return FALSE;
  }

  U_Log("Mesh: %-20s\tV:%-5d\tF:%-5d\tM:%-2d\tT:frame%03d\n", pNode->GetName(), m_pMesh->m_pIMesh->getNumVerts(), MeshFaceCount, MeshMaterialCount, time / GetTicksPerFrame());
  CLOG_INDENT;

  if(MeshMaterialCount == 0) MeshMaterialCount = 1;

D_LOG(1);
  gMaxHelper.StartProgressInfo(" SubMaterials");

  // for each material in the mesh
  for(int curMaterial = 0; curMaterial < MeshMaterialCount; curMaterial++) 
  {
  // fill faces array which using this material
    std::vector<int> faceList;
    faceList.reserve(MeshFaceCount);
    for(int i = 0; i < MeshFaceCount; i++) {
      if(m_pMesh->GetFaceMaterialId(i) == curMaterial)
        faceList.push_back(i);
    }
    if(faceList.size() == 0) {
      continue;
    }

    // check if we already have that SubMaterial in our list.
    SubMaterial *sbm = NULL;

    CMaxMaterial *pMyMtl = m_pMesh->GetFaceMaxMaterial(faceList[0]);
    for(int i=0; i<m_SubMaterials.size(); i++) {
      if(m_SubMaterials[i]->pMyMtl == pMyMtl) {
        sbm = m_SubMaterials[i];
	break;
      }
    }
D_LOG(1);
    
    CVertex	**pVertexs;
    int		*pIndices;
    int		iIdxStart;

    if(sbm == NULL) 
    {
      // if not found, create new
      sbm = new SubMaterial;
      m_SubMaterials.push_back(sbm);

      sbm->pMyMtl      = m_pMesh->GetFaceMaxMaterial(faceList[0]);
      sbm->iFaceCount  = 0;
      sbm->vVertices.resize(faceList.size() * 3);
      sbm->vIndices.resize(faceList.size() * 3);
      pVertexs         = &sbm->vVertices[0];
      pIndices         = &sbm->vIndices[0];
      iIdxStart        = 0;

      U_Log("!dAddMesh: %d/%d: %5d faces, new material: '%s'\n", curMaterial+1, MeshMaterialCount, faceList.size(), pMyMtl->m_materialName);
    } 
    else 
    {
      sbm->vVertices.resize((sbm->iFaceCount + faceList.size()) * 3);
      sbm->vIndices.resize((sbm->iFaceCount + faceList.size()) * 3);
      pVertexs         = &sbm->vVertices[sbm->iFaceCount * 3];
      pIndices         = &sbm->vIndices[sbm->iFaceCount * 3];
      iIdxStart        = sbm->iFaceCount * 3;

      U_Log("!dAddMesh: %d/%d: %5d faces, added to material: '%s'\n", curMaterial+1, MeshMaterialCount, faceList.size(), pMyMtl->m_materialName);
    }
D_LOG(1);

    // create all vertices
    for(int faceiter=0; faceiter<faceList.size(); faceiter++) 
    {
      int faceId = faceList[faceiter];

      //gMaxHelper.SetProgressInfo(100.0f * (float)faceiter / (float)faceCount);

      CVertex* vtx[3];
      vtx[0] = m_pMesh->GetVertex(faceId, 0);
      vtx[1] = m_pMesh->GetVertex(faceId, 1);
      vtx[2] = m_pMesh->GetVertex(faceId, 2);
      
      // check for degenerated tri
      if(vtx[0]->m_position.Equals(vtx[1]->m_position, CVertex::POS_EPSILON) ||
         vtx[0]->m_position.Equals(vtx[2]->m_position, CVertex::POS_EPSILON) ||
         vtx[1]->m_position.Equals(vtx[2]->m_position, CVertex::POS_EPSILON)) {

         //U_Log("!ddegenerated face: %d\n", faceId);
         delete vtx[0];
         delete vtx[1];
         delete vtx[2];
         continue;
      }

      // add actual face
      for(int j=0; j<3; j++) 
      {
        pVertexs[j] = vtx[j];
        pVertexs[j]->m_iVertexId = iIdxStart + j; // remember index position for that vertex

        pIndices[j] = iIdxStart + j;
      }

      pVertexs  += 3;
      pIndices  += 3;
      iIdxStart += 3;
      sbm->iFaceCount++;
    }
    
    if(faceList.size() != sbm->iFaceCount) {
      U_Log("%d degenerated tris, ", faceList.size() - sbm->iFaceCount);
      sbm->vVertices.resize(sbm->iFaceCount*3);
      sbm->vIndices.resize(sbm->iFaceCount*3);
    }
  }
D_LOG(1);

  // stop the progress info
  gMaxHelper.StopProgressInfo();
  return 1;
}

void CMesh::SubMat_CalcTangentSpace(SubMaterial* sbm)
{
  /*
   * Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”. 
   * Terathon Software 3D Graphics Library, 2001. 
   * http://www.terathon.com/code/tangent.html
   *
   */

  // sort submesh & find duplicates & fix indices
  if(cmesh_debug) U_Log(", tangents");

  // entries in sbm vVertices & vIndices arrays
  const int vertexCount = sbm->iFaceCount * 3;

  std::vector<Point3> tan1(vertexCount, Point3(0, 0, 0));
  std::vector<Point3> tan2(vertexCount, Point3(0, 0, 0));
    
  for(long a = 0; a < sbm->iFaceCount; a++)
  {
    int i1 = sbm->vIndices[a*3+0];
    int i2 = sbm->vIndices[a*3+1];
    int i3 = sbm->vIndices[a*3+2];
        
    const Point3& v1 = sbm->vVertices[i1]->m_position;
    const Point3& v2 = sbm->vVertices[i2]->m_position;
    const Point3& v3 = sbm->vVertices[i3]->m_position;
        
    const Point2  w1 = Point2(sbm->vVertices[i1]->m_tu[0].u, sbm->vVertices[i1]->m_tu[0].v);
    const Point2  w2 = Point2(sbm->vVertices[i2]->m_tu[0].u, sbm->vVertices[i2]->m_tu[0].v);
    const Point2  w3 = Point2(sbm->vVertices[i3]->m_tu[0].u, sbm->vVertices[i3]->m_tu[0].v);
        
    float x1 = v2.x - v1.x;
    float x2 = v3.x - v1.x;
    float y1 = v2.y - v1.y;
    float y2 = v3.y - v1.y;
    float z1 = v2.z - v1.z;
    float z2 = v3.z - v1.z;
        
    float s1 = w2.x - w1.x;
    float s2 = w3.x - w1.x;
    float t1 = w2.y - w1.y;
    float t2 = w3.y - w1.y;
        
    float r = (s1 * t2 - s2 * t1);
    if(r != 0.0f) r = 1.0f / r;
    Point3 sdir((t2 * x1 - t1 * x2) * r, 
                (t2 * y1 - t1 * y2) * r,
                (t2 * z1 - t1 * z2) * r);
    Point3 tdir((s1 * x2 - s2 * x1) * r, 
                (s1 * y2 - s2 * y1) * r,
                (s1 * z2 - s2 * z1) * r);
                
    if(!_finite(tdir.Length())) {
      U_Log("!r bad tangent happen %f %f %f\n", tdir.x, tdir.y, tdir.z);
      tdir = Point3(1, 0, 0);
    }	      
    if(!_finite(sdir.Length())) {
      U_Log("!r bad binormal happen %f %f %f\n", sdir.x, sdir.y, sdir.z);
      sdir = Point3(0, 1, 0);
    }
      
    tan1[i1] += sdir;
    tan1[i2] += sdir;
    tan1[i3] += sdir;
        
    tan2[i1] += tdir;
    tan2[i2] += tdir;
    tan2[i3] += tdir;
  }
  
  for(int a = 0; a < vertexCount; a++)
  {
    CVertex* vtx = sbm->vVertices[a];

    const Point3& n = vtx->m_normal;
    const Point3& t = tan1[a];
        
    // Gram-Schmidt orthogonalize
    vtx->m_tangent = (t - n * DotProd(n, t)).Normalize();
        
    // Calculate handedness
    vtx->m_w_tangent = (DotProd(CrossProd(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
    vtx->m_bitangent = CrossProd(vtx->m_normal, vtx->m_tangent) * vtx->m_w_tangent;
  }
  
  return;
}

static int _sort_SortVertices(const void *arg1, const void *arg2 )
{
  const CVertex& v1 = **(CVertex**)arg1;
  const CVertex& v2 = **(CVertex**)arg2;
  /*
  if(v1.m_position[0] != v2.m_position[0]) return (v1.m_position[0] >= v2.m_position[0]) ? -1 : 1;
  if(v1.m_position[1] != v2.m_position[1]) return (v1.m_position[1] >= v2.m_position[1]) ? -1 : 1;
  if(v1.m_position[2] != v2.m_position[2]) return (v1.m_position[2] >= v2.m_position[2]) ? -1 : 1;
  */
  
  return v1.SortOperand(v2, false);
}

bool isMagicVector(const CVertex* v)
{
	if(fabs(v->m_position.x - -7.707050) < 0.01f && 
	   fabs(v->m_position.z - 0.000000) < 0.01f && 
	   fabs(v->m_position.y - 27.930155) < 0.01f) {
		return true;
	}
	return false;
}

void CMesh::SubMat_RemoveDups(SubMaterial* sbm)
{
  // entries in sbm vVertices & vIndices arrays
  const int iNumE = sbm->iFaceCount * 3;

  // sort submesh & find duplicates & fix indices
  if(cmesh_debug) U_Log(", sorting");

  // disable logging for first pass vertex sorting by coordinates
  int prev_vtx_sort_debug = vtx_sort_debug;
  vtx_sort_debug = 0;

  std::vector<CVertex*> vsort = sbm->vVertices;
  qsort((void *)&vsort[0], vsort.size(), sizeof(CVertex*), _sort_SortVertices);
  
  vtx_sort_debug = prev_vtx_sort_debug;
  
  /*if(1) {
    char fname[64];
    sprintf(fname, "z:\\2\\shit_%d_%d.txt", sbm->vVertices.size(), sbm->vIndices.size());
    FILE* f = fopen(fname, "wt");
    for(int i=0; i<iNumE; i++) {
      fprintf(f, "%d: %f %f %f %f %f %f %f %f %f\n", 
        i,
        vsort[i]->m_position.x, vsort[i]->m_position.z, vsort[i]->m_position.y,
        vsort[i]->m_normal.x, vsort[i]->m_normal.z, vsort[i]->m_normal.y,
        vsort[i]->m_tangent.x, vsort[i]->m_tangent.z, vsort[i]->m_tangent.y);
    }
    fclose(f);
  }*/

  if(cmesh_debug) U_Log(", dups");

  // index remap table
  std::vector<int> idxRemap(iNumE, -1);
      
  // we start from 0 and compare with previous value.
  const CVertex* compareVtx = vsort[0];
  for(int i=0; i<iNumE; i++) 
  { 
    const int vidx = vsort[i]->m_iVertexId;

    /*
    vtx_sort_debug = 0;
    if(isMagicVector(compareVtx)) {
      U_Log("magicvector: %f %f %f\n", compareVtx->m_position.x, compareVtx->m_position.y, compareVtx->m_position.z);
      vtx_sort_debug = 1;
    }*/
    
    if(compareVtx->SortOperand(*vsort[i], true) == 0) 
    {
      // we found a duplicate of the vertex - add remap.
      idxRemap[vidx] = compareVtx->m_iVertexId;
      continue;
    }
		       
    // store old index and advance compare vertex
    idxRemap[vidx] = vidx;
    compareVtx = vsort[i];
  }

  // actually remap indices
  for(int j=0; j<iNumE; j++) 
  { 
    assert(idxRemap[j] != -1);

    int idx1 = sbm->vIndices[j]; // old index
    int idx2 = idxRemap[idx1];   // new (remapped) index
    assert(idx2 >= 0 && idx2 < iNumE);
    sbm->vIndices[j] = idx2;
  }
  
  return;
}

void CMesh::SubMat_Optimize(SubMaterial* sbm)
{
  U_Log(",optimizing:");
  int* pIndices = &sbm->vIndices[0];
  VertexCacheOptimizer vcache;
  VertexCache cache;
  int miss_count1 = cache.GetCacheMissCount(pIndices, sbm->iFaceCount);
  if(vcache.Optimize(pIndices, sbm->iFaceCount) != VertexCacheOptimizer::Success)
  {
    ThrowError("vertex cache optimize failed");
  }
  int miss_count2 = cache.GetCacheMissCount(pIndices, sbm->iFaceCount);
  U_Log("%.2f->%.2f", (float)miss_count1/(float)(sbm->iFaceCount*3), (float)miss_count2/(float)(sbm->iFaceCount*3));
  
  return;
}

void CMesh::SubMat_AddToMesh(const SubMaterial* sbm)
{
  // entries in sbm vVertices & vIndices arrays
  const int iNumE = sbm->iFaceCount * 3;
  if(cmesh_debug) U_Log(", adding");

  // used vertices array
  std::vector<int> usedIdx(iNumE, -1);

  int addedIdx = 0;
  int addedVtx = 0;
  for(int i=0; i<iNumE; i++) 
  { 
    int idx = sbm->vIndices[i];
        
    // check for new vertex
    if(usedIdx[idx] == -1) {
      usedIdx[idx] = addedVtx;

      m_vVertices.push_back(sbm->vVertices[idx]);
      addedVtx++;
    }
        
    m_vIndices.push_back(sbm->iVertexStart + usedIdx[idx]);
    addedIdx++;
  }
      
  assert((addedIdx % 3) == 0);
  m_iFaceCount   += addedIdx / 3;
  m_iVertexCount += addedVtx;

  U_Log(", vtx=%d->%d", sbm->iFaceCount*3, addedVtx);

  return;
}

void CMesh::Finalize_SubMat(SubMaterial* sbm)
{
  assert(m_vVertices.size() == m_iVertexCount);
  assert(m_vIndices.size() == m_iFaceCount*3);

  sbm->iIndexStart  = m_iFaceCount*3;
  sbm->iVertexStart = m_iVertexCount;

  // entries in sbm vVertices & vIndices arrays
  const int iNumE = sbm->iFaceCount * 3;
  if(sbm->vVertices.size() != iNumE) ThrowError("sbm->vVertices.size() != iNumE");
  if(sbm->vIndices.size()  != iNumE) ThrowError("sbm->vIndices.size() != iNumE");
  
  int uprop_NoVertexJoin = 0;
  m_pMesh->m_pNode->GetUserPropInt("NoVertexJoin", uprop_NoVertexJoin);

  U_Log("!d%s:", sbm->pMyMtl->m_materialName);
  extern int g_SCO_ExportTangents;
  if(g_SCO_ExportTangents) 
    SubMat_CalcTangentSpace(sbm);
  
  if(uprop_NoVertexJoin == 0) 
    SubMat_RemoveDups(sbm);

  // SubMat_Optimize(sbm); - disabled, *way* too slow for 

  SubMat_AddToMesh(sbm);
  U_Log("\n");
      
  sbm->iVertexEnd   = m_iVertexCount;
  sbm->iIndexEnd    = m_iFaceCount*3;

  return;
}

int CMesh::Finalize(int maxBoneCount, float weightThreshold)
{
  CLOG_INDENT;
  
D_LOG(1); 
  gMaxHelper.StartProgressInfo(" Finalize");

  // calc total number of faces
  int iTotalFaces = 0;
  for(int iSubMat = 0; iSubMat < m_SubMaterials.size(); iSubMat++) 
  {
    SubMaterial *sbm = m_SubMaterials[iSubMat];
    iTotalFaces += sbm->iFaceCount;
  }

  gMaxHelper.SetProgressInfo(10);
  
  m_iFaceCount   = 0;
  m_iVertexCount = 0;
  m_vVertices.reserve(iTotalFaces * 3);
  m_vIndices.reserve(iTotalFaces * 3);

  // add each submesh
  for(int iSubMat = 0; iSubMat < m_SubMaterials.size(); iSubMat++) 
  {
    SubMaterial* sbm = m_SubMaterials[iSubMat];
    Finalize_SubMat(sbm);
  }

D_LOG(1);

  gMaxHelper.SetProgressInfo(66);

  // adjust the bone assignments in all vertices
  int vertexId;
  for(vertexId = 0; vertexId < m_vVertices.size(); vertexId++)
  {
    m_vVertices[vertexId]->AdjustBoneInfluences(maxBoneCount, weightThreshold);
  }

  // output some sorting info
  U_Log("!dvertices had different: %d normals, %d bi, %d tan, %d uv, %d colors\n", 
    vtx_sort_stats[1], vtx_sort_stats[2], vtx_sort_stats[3], vtx_sort_stats[4], vtx_sort_stats[5]);

  gMaxHelper.SetProgressInfo(100);
  U_Log("!dMesh created, %d vertices, %d faces, %d materials\n", m_iVertexCount, m_iFaceCount, m_SubMaterials.size());

D_LOG(1);      

 // export weights ONLY if we have skeleton data,
 // do not export if we exporting vertex animation
  m_exportWeights = 0;
  if(m_pMesh->m_pPhysiqueModifier)    m_exportWeights = 1;
  if(m_pMesh->m_pSkinModifier)        m_exportWeights = 1;

  if(m_exportWeights && theExporter.m_iStartFrame != theExporter.m_iEndFrame)
    theExporter.m_bBoneAnimationPresent++;

  gMaxHelper.StopProgressInfo();

D_LOG(1);

  return TRUE;
}
