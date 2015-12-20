//----------------------------------------------------------------------------------------------------------------------
/// \file NurbsSurface.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/NurbsSurface.h"
#include "XMD/FileIO.h"
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XNurbsSurface::XNurbsSurface(XModel* pmod) 
  : XParametricSurface(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XNurbsSurface::~XNurbsSurface()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XNurbsSurface::GetApiType() const
{
  return XFn::NurbsSurface;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNurbsSurface::GetFn(XFn::Type type)
{
  if(XFn::NurbsSurface==type)
    return (XNurbsSurface*)this;
  return XParametricSurface::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XNurbsSurface::GetFn(XFn::Type type) const
{
  if(XFn::NurbsSurface==type)
    return (const XNurbsSurface*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XRealArray& XNurbsSurface::KnotsU() const 
{
  return m_KnotsU; 
}

//----------------------------------------------------------------------------------------------------------------------
const XRealArray& XNurbsSurface::KnotsV() const 
{
  return m_KnotsV; 
}

//----------------------------------------------------------------------------------------------------------------------
XU8 XNurbsSurface::GetDegreeU() const 
{
  return m_DegreeU;
}

//----------------------------------------------------------------------------------------------------------------------
XU8 XNurbsSurface::GetDegreeV() const
{
  return m_DegreeV;
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsSurface::SetDegreeU(XU8 v) 
{
  m_DegreeU = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsSurface::SetDegreeV(XU8 v) 
{
  m_DegreeV = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XNurbsSurface::WriteChunk(std::ostream& ofs) 
{
  if (!XParametricSurface::WriteChunk(ofs)) 
    return false;

  ofs << "\tdegree_u " << static_cast<XU32>(m_DegreeU) << "\n";
  ofs << "\tdegree_v " << static_cast<XU32>(m_DegreeV) << "\n";
  ofs << "\tnum_cvs_u " << static_cast<XU32>(m_NumCvsU) << "\n";
  ofs << "\tnum_cvs_v " << static_cast<XU32>(m_NumCvsV) << "\n";
  ofs << "\ttype_u " << static_cast<XU32>(GetTypeInU()) << "\n";
  ofs << "\ttype_v " << static_cast<XU32>(GetTypeInV()) << "\n";

  ofs << "\tknots_u " << static_cast<XU32>(m_KnotsU.size()) << "\n\t\t";
  for(XU32 i=0;i<m_KnotsU.size();++i) 
  {
    ofs << m_KnotsU[i] ;
    if(i!=(m_KnotsU.size()-1))
      ofs << " ";
  }
  ofs << "\n";

  ofs << "\tknots_v " << static_cast<XU32>(m_KnotsV.size()) << "\n\t\t";
  for(XU32 i=0;i<m_KnotsV.size();++i)
  {
    ofs << m_KnotsV[i] ;
    if(i!=(m_KnotsV.size()-1))
      ofs << " ";
  }
  ofs << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNurbsSurface::DoData(XFileIO& io)
{
  DUMPER(XNurbsSurface);

  IO_CHECK( XParametricSurface::DoData(io) );

  IO_CHECK( io.DoData(&m_DegreeU) );
  DPARAM( m_DegreeU );
  IO_CHECK( io.DoData(&m_DegreeV) );
  DPARAM( m_DegreeV );

  IO_CHECK( io.DoDataVector(m_KnotsU) );
  DAPARAM( m_KnotsU );
  IO_CHECK( io.DoDataVector(m_KnotsV) );
  DAPARAM( m_KnotsV );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsSurface::DoCopy(const XBase* rhs)
{
  const XNurbsSurface* cb = rhs->HasFn<XNurbsSurface>();
  XMD_ASSERT(cb);

  m_DegreeU = cb->m_DegreeU;
  m_DegreeV = cb->m_DegreeV;
  m_KnotsU = cb->m_KnotsU;
  m_KnotsV = cb->m_KnotsV;

  XParametricSurface::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNurbsSurface::GetDataSize() const 
{
  size_t sz = XParametricSurface::GetDataSize() +
              sizeof(XU8)*2 +
              sizeof(XReal)*(m_KnotsU.size()+m_KnotsV.size()) + 8;
  return (XU32)sz;
}

//----------------------------------------------------------------------------------------------------------------------
bool XNurbsSurface::ReadChunk(std::istream& ifs)
{
  if (!XParametricSurface::ReadChunk(ifs))
    return false;

  XS32 iii;


  READ_CHECK("degree_u",ifs);
  ifs >> iii;
  m_DegreeU=(XU8)iii;

  READ_CHECK("degree_v",ifs);
  ifs >> iii;
  m_DegreeV=(XU8)iii;

  READ_CHECK("num_cvs_u",ifs);
  ifs >> m_NumCvsU;

  READ_CHECK("num_cvs_v",ifs);
  ifs >> m_NumCvsV;

  READ_CHECK("type_u",ifs);
  ifs >> iii;
  SetTypeInU(iii);

  READ_CHECK("type_v",ifs);
  ifs >> iii;
  SetTypeInV(iii);

  READ_CHECK("knots_u",ifs);
  ifs >> iii;
  m_KnotsU.resize(iii);
  XRealArray::iterator it = m_KnotsU.begin();
  for(;it != m_KnotsU.end();++it)
  {
    ifs >> *it;
  }

  READ_CHECK("knots_v",ifs);
  ifs >> iii;
  m_KnotsV.resize(iii);
  it = m_KnotsV.begin();
  for(;it != m_KnotsV.end();++it)
  {
    ifs >> *it;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsSurface::PreDelete(XIdSet& extra_nodes)
{
  XParametricSurface::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNurbsSurface::NodeDeath(XId id)
{
  return XParametricSurface::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsSurface::GetKnotsU(XRealArray& knots) const
{
  knots.resize(m_KnotsU.size());
  XRealArray::iterator it_out = knots.begin();
  XRealArray::const_iterator it = m_KnotsU.begin();
  for( ; it != m_KnotsU.end(); ++it, ++it_out )
    *it_out = static_cast<XRealArray::value_type>( *it );
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsSurface::SetKnotsU(const XRealArray& knots) 
{
  m_KnotsU.resize(knots.size());
  XRealArray::const_iterator it_out = knots.begin();
  XRealArray::iterator it = m_KnotsU.begin();
  for( ; it != m_KnotsU.end(); ++it, ++it_out )
    *it = static_cast<XReal>( *it_out );
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsSurface::GetKnotsV(XRealArray& knots) const 
{
  knots.resize(m_KnotsV.size());
  XRealArray::iterator it_out = knots.begin();
  XRealArray::const_iterator it = m_KnotsV.begin();
  for( ; it != m_KnotsV.end(); ++it, ++it_out )
    *it_out = static_cast<XRealArray::value_type>( *it );
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsSurface::SetKnotsV(const XRealArray& knots) 
{
  m_KnotsV.resize(knots.size());
  XRealArray::const_iterator it_out = knots.begin();
  XRealArray::iterator it = m_KnotsV.begin();
  for( ; it != m_KnotsV.end(); ++it, ++it_out )
    *it = static_cast<XReal>( *it_out );
}
}
