//----------------------------------------------------------------------------------------------------------------------
/// \file NurbsCurve.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/NurbsCurve.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XNurbsCurve::XNurbsCurve(XModel* pmod) 
  : XParametricCurve(pmod),m_Degree(0),m_Knots()
{
}

//----------------------------------------------------------------------------------------------------------------------
XNurbsCurve::~XNurbsCurve()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XNurbsCurve::GetApiType() const
{
  return XFn::NurbsCurve;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNurbsCurve::GetFn(XFn::Type type)
{
  if(XFn::NurbsCurve==type)
    return (XNurbsCurve*)this;
  return XParametricCurve::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XNurbsCurve::GetFn(XFn::Type type) const
{
  if(XFn::NurbsCurve==type)
    return (const XNurbsCurve*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNurbsCurve::WriteChunk(std::ostream& ofs)
{
  // writes point data
  if(!XParametricCurve::WriteChunk(ofs))
    return false;

  ofs << "\tdegree " << m_Degree << "\n";
  ofs << "\tknots " << static_cast<XU32>(m_Knots.size()) << "\n\t\t";

  for( XU32 i=0;i<m_Knots.size();++i)
  {
    ofs << m_Knots[i] ;
    if(i!=(m_Knots.size()-1))
      ofs << " ";
  }
  ofs << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNurbsCurve::DoData(XFileIO& io)
{
  DUMPER(XNurbsCurve);

  IO_CHECK( XParametricCurve::DoData(io) );

  IO_CHECK( io.DoData(&m_Degree) );
  DPARAM( m_Degree );

  IO_CHECK( io.DoDataVector(m_Knots) );
  DAPARAM( m_Knots );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsCurve::DoCopy(const XBase* rhs)
{
  const XNurbsCurve* cb = rhs->HasFn<XNurbsCurve>();
  XMD_ASSERT(cb);

  m_Degree = cb->m_Degree;
  m_Knots = cb->m_Knots;

  XParametricCurve::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNurbsCurve::GetDataSize() const 
{
  XU32 size = XParametricCurve::GetDataSize();
  size += 6;
  size += sizeof(XReal)*static_cast<XU32>(m_Knots.size());
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
const XRealArray& XNurbsCurve::Knots() const 
{
  return m_Knots; 
}

//----------------------------------------------------------------------------------------------------------------------
XU16 XNurbsCurve::GetDegree() const 
{
  return m_Degree;
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsCurve::SetDegree(const XU16 d)
{
  m_Degree = d;
}

//----------------------------------------------------------------------------------------------------------------------
bool XNurbsCurve::ReadChunk(std::istream& ifs)
{
  // reads points
  if(!XParametricCurve::ReadChunk(ifs))
    return false;

  READ_CHECK("degree",ifs);
  ifs >> m_Degree;

  READ_CHECK("knots",ifs);
  XS32 NumKnots;
  ifs >> NumKnots;

  m_Knots.resize(NumKnots);
  XRealArray::iterator it = m_Knots.begin();

  for(;it != m_Knots.end(); ++it)
  {
    if (!ifs.good()) 
    {
      XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,"XMD::XNurbsCurve::ReadChunk >> unexpected File-IO error whilst reading knots");
      return false;
    }
    ifs >> *it;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsCurve::PreDelete(XIdSet& extra_nodes)
{
  XParametricCurve::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNurbsCurve::NodeDeath(XId id)
{
  return XParametricCurve::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsCurve::GetKnots(XRealArray& knots) const
{
  knots.resize(m_Knots.size());
  XRealArray::iterator it_out = knots.begin();
  XRealArray::const_iterator it = m_Knots.begin();
  for( ; it != m_Knots.end(); ++it, ++it_out )
    *it_out = static_cast<XRealArray::value_type>( *it );
}

//----------------------------------------------------------------------------------------------------------------------
void XNurbsCurve::SetKnots(const XRealArray& knots) 
{
  m_Knots.resize(knots.size());
  XRealArray::const_iterator it_out = knots.begin();
  XRealArray::iterator it = m_Knots.begin();
  for( ; it != m_Knots.end(); ++it, ++it_out )
    *it = static_cast<XReal>( *it_out );

}
}
