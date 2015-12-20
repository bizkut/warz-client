//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//
#include "StdAfx.h"
#include "EternityExport.h"

#include "Exporter.h"

//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


	int		g_SCO_ExportTangents = 1;

EternityExp::EternityExp()
{
  nUVPrecision 	     = 12;
  nGeometryPrecision = 6;

  sprintf(szFmtUVStr,       "%%4.%df", nUVPrecision);
  sprintf(szFmtGeometryStr, "%%4.%df", nGeometryPrecision);
}

EternityExp::~EternityExp()
{
}

// International settings in Windows could cause a number to be written
// with a "," instead of a ".".
// To compensate for this we need to convert all , to . in order to make the
// format consistent.
void EternityExp::CommaScan(char* buf)
{
  for(; *buf; buf++) if(*buf == ',') *buf = '.';
}

CStr EternityExp::Format(int value)
{
  char buf[50];

  sprintf(buf, _T("%d"), value);
  return buf;
}


CStr EternityExp::Format(float value)
{
  char buf[256];

  sprintf(buf, szFmtGeometryStr, value);
  CommaScan(buf);
  return buf;
}

CStr EternityExp::Format(Point3 value, bool convert)
{
  char fmt[256];
  char buf[1024];
  
  Point3 pt = convert ? U_FixCoordSys(value, TRUE) : value;

  sprintf(fmt, "%s %s %s", szFmtGeometryStr, szFmtGeometryStr, szFmtGeometryStr);
  sprintf(buf, fmt, pt.x, pt.y, pt.z);

  CommaScan(buf);
  return buf;
}

CStr EternityExp::FormatUV(UVVert value, bool convert)
{
  char buf[256];
  char fmt[256];
  
  float u = value.x;
  float v = convert ? (1.0f - value.y) : value.y;

  sprintf(fmt, "%s %s", szFmtUVStr, szFmtUVStr);
  sprintf(buf, fmt, u, v);

  CommaScan(buf);
  return buf;
}


CStr EternityExp::FormatColor(VertColor value)
{
  char buf[256];
  sprintf(buf, "%d %d %d", int(255.0f * value.x), int(255.0f * value.y), int(255.0f * value.z));

  CommaScan(buf);
  return buf;
}

CStr EternityExp::Format(Color value)
{
  char buf[256];
  sprintf(buf, "%d %d %d", int(255.0f * value.r), int(255.0f * value.g), int(255.0f * value.b));

  CommaScan(buf);
  return buf;
}

void EternityExp::ExportGeomObjectAscii(FILE *fMesh, const CMesh* mesh)
{
  const char* orgName = mesh->m_pMesh->m_pNode->GetName();
  U_Log("ScoMesh2: %-20s\tV:%-5d\tF:%-5d\n", U_FixName(orgName), mesh->m_iVertexCount, mesh->m_iFaceCount);
  CLOG_INDENT;
  
  //if(g_SCO_ExportTangents)
  //  U_Log("tangents exported to sco\n");

  Point3 vPivot = mesh->m_pMesh->m_NodeTM.GetTrans();
  fprintf(fMesh, "[ObjectBegin]\n"
		 "Name= %s\n"
		 "CentralPoint= %s\n"
		 "Verts= %d\n",
		 U_FixName(orgName),
		 Format(vPivot, true),
		 mesh->m_iVertexCount
		 );

  // vertices
  for(int i=0; i<mesh->m_iVertexCount; i++)
  {
    const CVertex* v = mesh->m_vVertices[i];
    Point3 p = U_FixCoordSys(v->m_position, TRUE);
    Point3 n = U_FixCoordSys(v->m_normal, FALSE);
    Point3 t = U_FixCoordSys(v->m_tangent, FALSE);
    CStr s1 = Format(p, false);
    CStr s2 = Format(n, false);
    CStr s3 = Format(t, false);

    if(g_SCO_ExportTangents && theExporter.m_ExpVtxColors)
      fprintf(fMesh, "%s %s %s %.0f %.0f %.0f %.0f\n", s1.data(), s2.data(), s3.data(), v->m_w_tangent, v->m_color.x * 255.0f, v->m_color.y * 255.0f, v->m_color.z * 255.0f);
    else if(g_SCO_ExportTangents)
      fprintf(fMesh, "%s %s %s %.0f\n", s1.data(), s2.data(), s3.data(), v->m_w_tangent);
    else
      fprintf(fMesh, "%s %s\n", s1.data(), s2.data());
  }

  // faces
  fprintf(fMesh, "Faces= %d\n", mesh->m_iFaceCount);

  int totFaces = 0;
  for(size_t iSubMat=0; iSubMat < mesh->m_SubMaterials.size(); iSubMat++)
  {
    const CMesh::SubMaterial* sbm = mesh->m_SubMaterials[iSubMat];
    for(int idx = sbm->iIndexStart; idx < sbm->iIndexEnd; idx += 3)
    {
      int i0 = mesh->m_vIndices[idx+0];
      int i1 = mesh->m_vIndices[idx+1];
      int i2 = mesh->m_vIndices[idx+2];
      fprintf(fMesh, "3\t%4d %4d %4d\t", i0, i1, i2);
      fprintf(fMesh, "%-20s\t", sbm->pMyMtl->m_materialName);
      
      UVVert tv1;
      UVVert tv2;
      UVVert tv3;
      tv1.x = mesh->m_vVertices[i0]->m_tu[0].u;
      tv1.y = mesh->m_vVertices[i0]->m_tu[0].v;
      tv2.x = mesh->m_vVertices[i1]->m_tu[0].u;
      tv2.y = mesh->m_vVertices[i1]->m_tu[0].v;
      tv3.x = mesh->m_vVertices[i2]->m_tu[0].u;
      tv3.y = mesh->m_vVertices[i2]->m_tu[0].v;
      fprintf(fMesh, "%s %s %s\n", FormatUV(tv1), FormatUV(tv2), FormatUV(tv3));
      totFaces++;
    }
  }
  
  if(totFaces != mesh->m_iFaceCount) 
    ThrowError("totFaces != mesh->m_iFaceCount");
  
  // export 2nd UV set
  if(mesh->m_vVertices.size() > 0 && mesh->m_vVertices[0]->m_iMapChannels == 2)
  {
    U_Log("have 2nd UV\n");
    assert(mesh->m_vVertices.size() == mesh->m_iVertexCount);
    fprintf(fMesh, "[UV2]\n");
    for(size_t i=0; i<mesh->m_vVertices.size(); i++)
    {
      UVVert tv1;
      tv1.x = mesh->m_vVertices[i]->m_tu[1].u;
      tv1.y = mesh->m_vVertices[i]->m_tu[1].v;
      fprintf(fMesh, "%s\n", FormatUV(tv1));
    }
  }
  
  fprintf(fMesh, "[ObjectEnd]\n\n");

  return;
}
