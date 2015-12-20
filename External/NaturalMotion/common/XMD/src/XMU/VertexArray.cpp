//----------------------------------------------------------------------------------------------------------------------
/// \file   VertexArray.cpp
/// \date   11-9-2004
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a class to convert an XMD::XMesh to a vertex array
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMU/VertexArray.h"
#include "XM2/Vector3.h"
#include "XM2/Matrix34.h"
#include "XMU/Normals.h"
#include "XMU/Tangents.h"
#include "XMD/Mesh.h"
#include "XMD/PolygonMesh.h"
#include "XMD/ItPolygon.h"

namespace XMU
{
//----------------------------------------------------------------------------------------------------------------------
const XMD::XU32Array& XVertexArray::Mappings() const
{
  return m_Mappings;
}

//----------------------------------------------------------------------------------------------------------------------
const XMD::XU32Array& XVertexArray::NormalFixes() const
{
  return m_NormalFixes;
}

//----------------------------------------------------------------------------------------------------------------------
void XVertexArray::GetMappings2(XMD::XU16* val) const
{
  XMD::XU32Array::const_iterator it = m_Mappings.begin();
  for( ; it != m_Mappings.end(); ++it,++val )
    *val = static_cast<XMD::XU16>(*it);
}

//----------------------------------------------------------------------------------------------------------------------
void XVertexArray::GetMappings2(XMD::XU32* val) const
{
  XMD::XU32Array::const_iterator it = m_Mappings.begin();
  for( ; it != m_Mappings.end(); ++it,++val )
    *val = static_cast<XMD::XU32>(*it);
}

//----------------------------------------------------------------------------------------------------------------------
void XVertexArray::GetIndices(XMD::XU32Array& val) const
{
  val.resize(m_Indices.size());

  XMD::XU32Array::iterator itb = val.begin();
  XMD::XU32Array::const_iterator it = m_Indices.begin();
  for( ; it != m_Indices.end(); ++it, ++itb )
     *itb  = *it;
}

//----------------------------------------------------------------------------------------------------------------------
void XVertexArray::GetIndices(XMD::XU16Array& val) const
{
  val.resize(m_Indices.size());

  XMD::XU16Array::iterator itb = val.begin();
  XMD::XU32Array::const_iterator it = m_Indices.begin();
  for( ; it != m_Indices.end(); ++it, ++itb )
    *itb  = static_cast<XMD::XU16>(*it);
}

//----------------------------------------------------------------------------------------------------------------------
const XMD::XU32Array& XVertexArray::Indices() const
{
  return m_Indices;
}

//----------------------------------------------------------------------------------------------------------------------
void XVertexArray::FacesToIndices(const XMD::XIndexList& FaceIds,XMD::XIndexList& VertexIds) const
{
  XMD::XIndexList::const_iterator it = FaceIds.begin();
  for( ; it != FaceIds.end(); ++it )
  {
    XMD::XU32 idx=3* (*it);
    if(idx<m_Indices.size()-2)
    {
      VertexIds.push_back(m_Indices[idx]);
      VertexIds.push_back(m_Indices[idx+1]);
      VertexIds.push_back(m_Indices[idx+2]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XVertexArray::XVertexArray(XMD::XBase* ptr)
  : m_VertexData(),m_NormalData(),m_TangentData(),m_BiNormalData(),
    m_UvData0(),m_UvData1(),m_UvData2(),m_UvData3(),
    m_Indices(),m_Mappings(),m_NormalFixes()
{

  XMD::XMesh* pm = ptr->HasFn<XMD::XMesh>();
  if (pm)
  {
    XMD_ASSERT(pm); // ERROR! You Must attach the Vertex Array generator to a mesh!
    pm->Triangulate();
    if(!pm->GetNumNormals())
    {
      pm->CalculateNormals(true);
    }

    mInternalMap.resize( pm->GetNumPoints() );

    XM2::XReal null_uv[3] = {0.0f,0.0f,0.0f};

    XMD::XFaceList::iterator it = pm->m_Faces.begin();
    for( ; it != pm->m_Faces.end(); ++it )
    {
      XM2::XReal* t0 = 0;
      XM2::XReal* t1 = 0;
      XM2::XReal* t2 = 0;
      XM2::XReal* t3 = 0;
      XM2::XReal* n  = 0;
      XM2::XReal* v  = 0;
      XM2::XReal* t  = 0;
      XM2::XReal* b  = 0;

      for(XM2::XU32 i=0;i<3;++i)
      {
        if( pm->m_TexCoords.size() == 4 )
        {
          XM2::XS32 idx = it->GetUvIndex(i,3);
          if (idx<0) idx = 0;
          XMD::XTexCoordList& tl = pm->m_TexCoords[3];
          if (tl.size()>0)
          {
            t3 = tl[ idx ].data;
          }
          else
            t3 = null_uv;
        }
        if( pm->m_TexCoords.size() >= 3 )
        {
          XM2::XS32 idx = it->GetUvIndex(i,2);
          if (idx<0) idx = 0;
          XMD::XTexCoordList& tl = pm->m_TexCoords[2];
          if (tl.size()>0)
          {
            t2 = tl[ idx ].data;
          }
          else
            t2 = null_uv;
        }
        if( pm->m_TexCoords.size() >= 2 )
        {
          XM2::XS32 idx = it->GetUvIndex(i,1);
          if (idx<0) idx = 0;
          XMD::XTexCoordList& tl = pm->m_TexCoords[1];
          if (tl.size()>0)
          {
            t1 = tl[ idx ].data;
          }
          else
            t1 = null_uv;
        }
        if( pm->m_TexCoords.size() >= 1 )
        {
          XM2::XS32 idx = it->GetUvIndex(i,0);
          if (idx<0) idx = 0;
          XMD::XTexCoordList& tl = pm->m_TexCoords[0];
          if (tl.size()>0)
          {
            t0 = tl[ idx ].data;
          }
          else
            t0 = null_uv;
        }
        if( pm->m_Normals.size() )
          n = pm->m_Normals[ it->GetNormalIndex(i) ].data;

        if( pm->m_BiNormals.size() )
          b = pm->m_BiNormals[ it->GetNormalIndex(i) ].data;

        if( pm->m_Tangents.size() )
          t = pm->m_Tangents[ it->GetNormalIndex(i) ].data;

        v = pm->m_Points[ it->GetVertexIndex(i) ].data;

        XMD::XU32 idx = GetElem(it->GetVertexIndex(i),v,n,t,b,t0,t1,t2,t3) ;
        m_Indices.push_back( idx );
      }
    }
    GenerateNormalFixes(pm);

    mInternalMap.clear();
  }

  XMD::XPolygonMesh* pm2 = ptr->HasFn<XMD::XPolygonMesh>();
  if (pm2)
  {
    pm2->Triangulate();
    if(!pm2->GetNormals())
    {
      pm2->CalculateNormals(true,true);
    }

    mInternalMap.resize( pm2->GetNumPoints() );

    XM2::XReal null_uv[3] = {0.0f,0.0f,0.0f};

    XMD::XItPolygon iter(pm2);

    for( ; !iter.IsDone(); iter.Next() )
    {

      XM2::XReal* t0 = 0;
      XM2::XReal* t1 = 0;
      XM2::XReal* t2 = 0;
      XM2::XReal* t3 = 0;
      XM2::XReal* n  = 0;
      XM2::XReal* v  = 0;
      XM2::XReal* t  = 0;
      XM2::XReal* b  = 0;

      for(XM2::XU32 i=0;i<3;++i)
      {

        XM2::XU32 vi = iter.GetVertexIndex(i);

        v = pm2->m_Points[ vi ].data;

        XMD::XVertexSet* nset = pm2->GetNormals();
        if (nset)
        {
          XM2::XU32 ni = iter.GetNormalIndex(i);
          ni *= nset->GetElementSize();
          if(nset->m_VertexData.size())
            n = &nset->m_VertexData[(ni >= 0) ? ni : 0];
          else
            n = null_uv;
        }

        XMD::XVertexSet* bset = pm2->GetBiNormals();
        if (bset)
        {
          XM2::XS32 ni = iter.GetNormalIndex(i);
          ni *= bset->GetElementSize();
          if(bset->m_VertexData.size())
            b = &bset->m_VertexData[(ni >= 0) ? ni : 0];
          else
            b = null_uv;
        }

        XMD::XVertexSet* tset = pm2->GetTangents();
        if (tset)
        {
          XM2::XS32 ni = iter.GetNormalIndex(i);
          ni *= tset->GetElementSize();
          if(tset->m_VertexData.size())
            t = &tset->m_VertexData[(ni >= 0) ? ni : 0];
          else
            t = null_uv;
        }

        if(pm2->GetNumUvSet()>0)
        {
          XMD::XVertexSet* vset = pm2->GetUvSet(0);
          if(vset)
          {
            XM2::XS32 uvi = iter.GetUvCoordIndex(i,0);
            uvi *= vset->GetElementSize();
            if(vset->m_VertexData.size())
              t0 = &vset->m_VertexData[(uvi >= 0) ? uvi : 0];
            else
              t0 = null_uv;
          }
          else
          {
            t0 = null_uv;
          }
        }
        if(pm2->GetNumUvSet()>1)
        {
          XMD::XVertexSet* vset = pm2->GetUvSet(1);
          if (vset)
          {
            XM2::XS32 uvi = iter.GetUvCoordIndex(i,1);
            uvi *= vset->GetElementSize();
            if(vset->m_VertexData.size())
              t1 = &vset->m_VertexData[(uvi >= 0) ? uvi : 0];
            else
              t1 = null_uv;
          }
          else
          {
            t1 = null_uv;
          }
        }
        if(pm2->GetNumUvSet()>2)
        {
          XMD::XVertexSet* vset = pm2->GetUvSet(2);
          if(vset)
          {
            XM2::XS32 uvi = iter.GetUvCoordIndex(i,2);
            uvi *= vset->GetElementSize();
            if(vset->m_VertexData.size())
              t2 = &vset->m_VertexData[(uvi >= 0) ? uvi : 0];
            else
              t2 = null_uv;
          }
          else
          {
            t2 = null_uv;
          }
        }
        if(pm2->GetNumUvSet()>3)
        {
          XMD::XVertexSet* vset = pm2->GetUvSet(3);
          if(vset)
          {
            XM2::XS32 uvi = iter.GetUvCoordIndex(i,3);
            uvi *= vset->GetElementSize();
            if(vset->m_VertexData.size())
              t3 = &vset->m_VertexData[(uvi >= 0) ? uvi : 0];
            else
              t3 = null_uv;
          }
          else
          {
            t3 = null_uv;
          }
        }

        XM2::XU32 idx = GetElem(vi,v,n,t,b,t0,t1,t2,t3);
        m_Indices.push_back( idx );
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVertexArray::SetVertexData(const XM2::XRealArray& vertexData)
{
  m_VertexData = vertexData;
}

//----------------------------------------------------------------------------------------------------------------------
void XVertexArray::GenerateNormals()
{
  XM2::XRealArray::iterator it = m_NormalData.begin();
  for (; it != m_NormalData.end(); ++it )
  {
    *it = 0.0f;
  }

  // resize the normal array if necessary. There are always the same number of normals as positions
  if(m_NormalData.empty())
  {
    m_NormalData.resize(m_VertexData.size());
  }

  XMD::XU32Array::iterator itn = m_Indices.begin();
  for (; itn < m_Indices.end();)
  {
    XM2::XU32 n0 = *itn; ++itn;
    XM2::XU32 n1 = *itn; ++itn;
    XM2::XU32 n2 = *itn; ++itn;
    n0 *= 3; n1 *= 3; n2 *= 3;

    XM2::XVector3 v0( m_VertexData[n0+0],m_VertexData[n0+1],m_VertexData[n0+2] );
    XM2::XVector3 v1( m_VertexData[n1+0],m_VertexData[n1+1],m_VertexData[n1+2] );
    XM2::XVector3 v2( m_VertexData[n2+0],m_VertexData[n2+1],m_VertexData[n2+2] );

    v0 *= 100;
    v1 *= 100;
    v2 *= 100;

    XM2::XVector3 e0( v0-v1 );
    XM2::XVector3 e1( v0-v2 );

    XM2::XVector3 cross;
    cross.cross( e1 , e0);
    cross.normalise();

    m_NormalData[n0+0] += cross.x;
    m_NormalData[n0+1] += cross.y;
    m_NormalData[n0+2] += cross.z;

    m_NormalData[n1+0] += cross.x;
    m_NormalData[n1+1] += cross.y;
    m_NormalData[n1+2] += cross.z;

    m_NormalData[n2+0] += cross.x;
    m_NormalData[n2+1] += cross.y;
    m_NormalData[n2+2] += cross.z;
  }

 it = m_NormalData.begin();
  for (; it != m_NormalData.end(); it+=3 )
  {
    XM2::XVector3 norm( *it, *(it+1), *(it+2) );
    norm.normalise();
    *it = norm.x;
    *(it+1) = norm.y;
    *(it+2) = norm.z;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVertexArray::GenerateTBN()
{
  // MORPH-21322: implementation.
}

//----------------------------------------------------------------------------------------------------------------------
XVertexArray::XVertexArray(XMD::XBase* ptr,const XM2::XMatrix& world)
  : m_VertexData(),m_NormalData(),m_TangentData(),m_BiNormalData(),
    m_UvData0(),m_UvData1(),m_UvData2(),m_UvData3(),
    m_Indices(),m_Mappings(),m_NormalFixes()
{
  XMD::XMesh* pm = ptr->HasFn<XMD::XMesh>();
  XMD_ASSERT(pm);  // ERROR! You Must attach the Vertex Array generator to a mesh!
  pm->Triangulate();

  mInternalMap.resize(pm->GetNumPoints());

  XMD::XFaceList::iterator it = pm->m_Faces.begin();
  for( ; it != pm->m_Faces.end(); ++it )
  {
    XM2::XReal* t0 = 0;
    XM2::XReal* t1 = 0;
    XM2::XReal* t2 = 0;
    XM2::XReal* t3 = 0;
    XM2::XReal* n  = 0;
    XM2::XReal* v  = 0;
    XM2::XReal* t  = 0;
    XM2::XReal* b  = 0;

    for(XMD::XU32 i=0;i<3;++i)
    {
      if( pm->m_TexCoords.size() == 4 )
      {
        XMD::XTexCoordList& tl = pm->m_TexCoords[3];
        t3 = tl[ it->GetUvIndex(i,3) ].data;
      }
      if( pm->m_TexCoords.size() >= 3 )
      {
        XMD::XTexCoordList& tl = pm->m_TexCoords[2];
        t2 = tl[ it->GetUvIndex(i,2) ].data;
      }
      if( pm->m_TexCoords.size() >= 2 )
      {
        XMD::XTexCoordList& tl = pm->m_TexCoords[1];
        t1 = tl[ it->GetUvIndex(i,1) ].data;
      }
      if( pm->m_TexCoords.size() >= 1 )
      {
        XMD::XTexCoordList& tl = pm->m_TexCoords[0];
        t0 = tl[ it->GetUvIndex(i,0) ].data;
      }
      if( pm->m_Normals.size() )
        n = pm->m_Normals[ it->GetNormalIndex(i) ].data;

      if( pm->m_BiNormals.size() )
        b = pm->m_BiNormals[ it->GetNormalIndex(i) ].data;

      if( pm->m_Tangents.size() )
        t = pm->m_Tangents[ it->GetNormalIndex(i) ].data;

      v = pm->m_Points[ it->GetVertexIndex(i) ].data;

      XM2::XU32 idx = GetElem(it->GetVertexIndex(i),v,n,t,b,t0,t1,t2,t3) ;

      if (m_Indices.size() == m_Indices.capacity())
      {
        m_Indices.reserve(m_Indices.size()+1000);
      }

      m_Indices.push_back( idx );
    }
  }
  GenerateNormalFixes(pm);

  XM2::XMatrix nm;
  nm.invert(world);
  nm.transpose();

  XM2::XRealArray::iterator itv = m_VertexData.begin();
  XM2::XRealArray::iterator itn = m_NormalData.begin();
  XM2::XRealArray::iterator itb = m_BiNormalData.begin();
  XM2::XRealArray::iterator itt = m_TangentData.begin();

  // move the data into world space, based upon the matrix passed
  for( ; itv != m_VertexData.end(); itv += 3, itn += 3, itt += 3, itb += 3 )
  {
    XM2::XVector3 v( *itv, *(itv+1), *(itv+2) );
    XM2::XVector3 n( *itn, *(itn+1), *(itn+2) );
    XM2::XVector3 b( *itb, *(itb+1), *(itb+2) );
    XM2::XVector3 t( *itt, *(itt+1), *(itt+2) );

    world.transformPoint(v);
    nm.transformVector(n);
    nm.transformVector(b);
    nm.transformVector(t);

    n.normalise();
    t.normalise();
    b.normalise();

    *itv     = v.x;
    *(itv+1) = v.y;
    *(itv+2) = v.z;

    *itn     = n.x;
    *(itn+1) = n.y;
    *(itn+2) = n.z;

    *itb     = b.x;
    *(itb+1) = b.y;
    *(itb+2) = b.z;

    *itt     = t.x;
    *(itt+1) = t.y;
    *(itt+2) = t.z;
  }

  mInternalMap.clear();

  m_VertexData.resize(m_VertexData.size());
  m_NormalData.resize(m_NormalData.size());
  m_BiNormalData.resize(m_BiNormalData.size());
  m_TangentData.resize(m_TangentData.size());
  m_UvData0.resize(m_UvData0.size());
  m_UvData1.resize(m_UvData1.size());
  m_UvData2.resize(m_UvData2.size());
  m_UvData3.resize(m_UvData3.size());
  m_Indices.resize(m_Indices.size());
  m_Mappings.resize(m_Mappings.size());
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexArray::GenerateNormalFixes(XMD::XMesh* pm)
{
  if(!m_NormalData.size())
    return false;
  XM2::vector< XMD::XU32Array > Mapp;

  Mapp.resize(pm->m_Points.size());

  XMD::XU32Array::iterator iti = m_Indices.begin();

  for( ; iti != m_Indices.end(); ++iti )
  {
    Mapp[ m_Mappings[*iti] ].push_back( *iti );
  }

  XM2::vector< XMD::XU32Array > OutputMap;


  // loop through the map to build the sets of un-averaged vertices
  XM2::vector< XMD::XU32Array >::iterator it = Mapp.begin();

  for( ; it != Mapp.end(); ++it )
  {
    // keep removing sets of indices until they have all gone
    // (we know that having 1 index will be alight, so ignore it)
    //
    while (it->size() > 1)
    {
      // get first index.
      XM2::XU32 idx0 = *(it->begin())  * 3;

      // erase first position
      it->erase( it->begin() );

      // this will hold the indices into the varray mappings
      XMD::XU32Array TexFix;

      // add the first index
      TexFix.push_back(idx0/3);

      // loop through the rest of the vertex array positions to see if any others
      // share that normal
      //
      XM2::XU32 iii=0;
      while( iii < it->size() )
      {
        XM2::XU32 idx1 = (*it)[iii] * 3;

        // if normals are the same at both vertex array locations...
        //
        if( XM2::float_equal( m_NormalData[idx1+0], m_NormalData[idx0+0]) &&
            XM2::float_equal( m_NormalData[idx1+1], m_NormalData[idx0+1]) &&
            XM2::float_equal( m_NormalData[idx1+2], m_NormalData[idx0+2]) )
        {

          // add index into list
          TexFix.push_back( (*it)[iii] );

          // erase the element from the list
          it->erase( it->begin() + iii );
        }
        else
        {
          // check next position
          ++iii;
        }
      }

      // if we have more than one index in this array, then we have a set of points to
      // average, so add them to the list as well.
      //
      if (TexFix.size() > 1)
      {
        OutputMap.push_back( TexFix );
      }
    }
  }

  // don't need this data anymore..
  Mapp.clear();

  it = OutputMap.begin();

  // add each array of shared indices to the byte stream
  for( ; it != OutputMap.end(); ++it )
  {
    m_NormalFixes.push_back(static_cast<XM2::XU32>(it->size()));
    for(XM2::XU32 i=0;i<it->size();++i)
    {
      m_NormalFixes.push_back( (*it)[i] );
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XVertexArray::GetNormalFixesByteStream(XMD::XU8** ppdata,XMD::XU32& nBytes,bool& is32bit) const
{
  if(m_NormalFixes.size()==0)
  {
    nBytes = 0;
    *ppdata = 0;
    return;
  }


  // used to construct the byte stream
  XMD::XU8Array Bytes;

  is32bit= (m_VertexData.size()>0xFFFF) ? true : false;

  // a way of easily getting the bytes from a short
  union
  {
    XM2::XU16 i;
    XM2::XU8 bytes[2];
  }
  Fix2;

  // a way of easily getting the bytes from an int
  union
  {
    XM2::XU32 i;
    XM2::XU8 bytes[4];
  }
  Fix4;


  if(is32bit)
  {
    XMD::XU32Array::const_iterator it = m_NormalFixes.begin();
    for( ; it < m_NormalFixes.end(); )
    {
      XM2::XU8 cc = (XMD::XU8)*it;
      ++it;
      Bytes.push_back(cc);
      for(XM2::XU8 i = 0;i!=cc;++i,++it)
      {
        Fix4.i = *it;
        Bytes.push_back( Fix4.bytes[0] );
        Bytes.push_back( Fix4.bytes[1] );
        Bytes.push_back( Fix4.bytes[2] );
        Bytes.push_back( Fix4.bytes[3] );
      }
    }
  }
  else
  {
    XMD::XU32Array::const_iterator it = m_NormalFixes.begin();
    for( ; it < m_NormalFixes.end(); )
    {
      XM2::XU8 cc = (XM2::XU8)*it;
      ++it;
      Bytes.push_back(cc);
      for(XM2::XU8 i = 0;i!=cc;++i,++it)
      {
        Fix2.i = (XM2::XU16)*it;
        Bytes.push_back( Fix2.bytes[0] );
        Bytes.push_back( Fix2.bytes[1] );
      }
    }
  }

  nBytes = static_cast<XM2::XU32>(Bytes.size());
  *ppdata = new XM2::XU8[ nBytes ];
  memcpy(*ppdata,&(Bytes[0]),nBytes);
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 XVertexArray::GetElem(XM2::XU32 idx,XM2::XReal* v,XM2::XReal* n,XM2::XReal* t,XM2::XReal* b,XM2::XReal* t0,XM2::XReal* t1,XM2::XReal* t2,XM2::XReal* t3)
{
  XMD_ASSERT(idx<mInternalMap.size());

  // check previous indices
  XMD::XIndexList& temp_indices = mInternalMap[idx];

  XM2::XU32 temp = static_cast<XM2::XU32>(temp_indices.size());

  for(XMD::XU32 i=0;i<temp;++i)
  {
    XM2::XU32 index = temp_indices[i];
    XM2::XU32 i2=2*index;
    XM2::XU32 i3=3*index;

    if(n)
    {
      // perform dot product to get cos angle between normals
      XMD::XReal dot = (n[0] * m_NormalData[i3]) +
                       (n[1] * m_NormalData[i3+1]) +
                       (n[2] * m_NormalData[i3+2]);
      if(dot < 0.9998f)
        continue;
    }

    if(t0)
    {
      if( !(XM2::float_equal(t0[0],m_UvData0[i2]) &&
            XM2::float_equal(t0[1],m_UvData0[i2+1])) )
        continue;
    }
    if(t1)
    {
      if( !(XM2::float_equal(t1[0],m_UvData1[i2]) &&
            XM2::float_equal(t1[1],m_UvData1[i2+1])) )
        continue;
    }
    if(t2)
    {
      if( !(XM2::float_equal(t2[0],m_UvData2[i2]) &&
            XM2::float_equal(t2[1],m_UvData2[i2+1])) )
        continue;
    }
    if(t3)
    {
      if( !(XM2::float_equal(t3[0],m_UvData3[i2]) &&
            XM2::float_equal(t3[1],m_UvData3[i2+1])) )
        continue;
    }
    return index;
  }

  // if size max hit, add another 1000 verts into the buffers.
  if (m_VertexData.capacity()==m_VertexData.size())
  {
    size_t new_size_v = m_VertexData.size() + 3000;
    size_t new_size_u = m_VertexData.size() + 2000;
    m_VertexData.reserve(new_size_v);

    if(n)
    {
      m_NormalData.reserve(new_size_v);
    }

    if(t0)
    {
      m_UvData0.reserve(new_size_u);

      if(b)
      {
        m_BiNormalData.reserve(new_size_v);
      }
      if(t)
      {
        m_TangentData.reserve(new_size_v);
      }
    }
    if(t1)
    {
      m_UvData1.reserve(new_size_u);
    }
    if(t2)
    {
      m_UvData2.reserve(new_size_u);
    }
    if(t3)
    {
      m_UvData3.reserve(new_size_u);
    }
  }

  m_VertexData.push_back(v[0]);
  m_VertexData.push_back(v[1]);
  m_VertexData.push_back(v[2]);

  if(n)
  {
    m_NormalData.push_back(n[0]);
    m_NormalData.push_back(n[1]);
    m_NormalData.push_back(n[2]);
  }

  if(t0)
  {
    m_UvData0.push_back(t0[0]);
    m_UvData0.push_back(t0[1]);

    if(b)
    {
      m_BiNormalData.push_back(b[0]);
      m_BiNormalData.push_back(b[1]);
      m_BiNormalData.push_back(b[2]);
    }
    if(t)
    {
      m_TangentData.push_back(t[0]);
      m_TangentData.push_back(t[1]);
      m_TangentData.push_back(t[2]);
    }
  }
  if(t1)
  {
    m_UvData1.push_back(t1[0]);
    m_UvData1.push_back(t1[1]);
  }
  if(t2)
  {
    m_UvData2.push_back(t2[0]);
    m_UvData2.push_back(t2[1]);
  }
  if(t3)
  {
    m_UvData3.push_back(t3[0]);
    m_UvData3.push_back(t3[1]);
  }

  if (m_Mappings.capacity()==m_Mappings.size())
  {
    m_Mappings.reserve(m_Mappings.size()+1000);
  }

  m_Mappings.push_back( idx );
  XM2::XU32 new_index = static_cast<XM2::XU32>(m_VertexData.size())/3 - 1;

  if (temp_indices.capacity()==temp_indices.size())
  {
    temp_indices.reserve(temp_indices.size()+1000);
  }

  temp_indices.push_back(new_index);

  return new_index;
}

//----------------------------------------------------------------------------------------------------------------------
XM2::XU32 XVertexArray::GetNumVertices() const
{
  return static_cast<XM2::XU32>(m_VertexData.size()/3);
}

//----------------------------------------------------------------------------------------------------------------------
XM2::XU32 XVertexArray::GetNumIndices() const
{
  return static_cast<XM2::XU32>(m_Indices.size());
}

//----------------------------------------------------------------------------------------------------------------------
XM2::XU32 XVertexArray::GetNumUvSets() const
{
  if(m_UvData3.size())
    return 4;
  if(m_UvData2.size())
    return 3;
  if(m_UvData1.size())
    return 2;
  if(m_UvData0.size())
    return 1;
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XRealArray& XVertexArray::Vertices() const
{
  return m_VertexData;
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XRealArray& XVertexArray::Normals() const
{
  return m_NormalData;
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XRealArray& XVertexArray::BiNormals() const
{
  return m_BiNormalData;
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XRealArray& XVertexArray::Tangents() const
{
  return m_TangentData;
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XRealArray& XVertexArray::Uvs0() const
{
  return m_UvData0;
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XRealArray& XVertexArray::Uvs1() const
{
  return m_UvData1;
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XRealArray& XVertexArray::Uvs2() const
{
  return m_UvData2;
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XRealArray& XVertexArray::Uvs3() const
{
  return m_UvData3;
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XRealArray* XVertexArray::GetUvs( const XM2::XU32 uv_set ) const
{
  switch(uv_set)
  {
  case 0:
    return &m_UvData0;
  case 1:
    return &m_UvData1;
  case 2:
    return &m_UvData2;
  case 3:
    return &m_UvData3;
  default:
    break;
  }
  return 0;
}
}
