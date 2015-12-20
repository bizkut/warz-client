//----------------------------------------------------------------------------------------------------------------------
/// \file ParametricSurface.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ParametricSurface.h"
#include "XM2/Plane.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XVector3 XParametricSurface::GetCV(XU32 u,XU32 v) const
{
  if( u < GetNumCvsInU() && v < GetNumCvsInV() ) 
  {
    return m_Points[ u*GetNumCvsInV() + v ];
  }
  return XVector3();
}

//----------------------------------------------------------------------------------------------------------------------
void XParametricSurface::SetTypeInU(XS32 ii)
{
  m_TypeU=(XU8)ii;
}

//----------------------------------------------------------------------------------------------------------------------
void XParametricSurface::SetTypeInV(XS32 ii) 
{
  m_TypeV=(XU8)ii;
}

//----------------------------------------------------------------------------------------------------------------------
void XParametricSurface::SetNumCvsInU(XS32 ii) 
{
  m_NumCvsU=(XU16)ii;
}

//----------------------------------------------------------------------------------------------------------------------
void XParametricSurface::SetNumCvsInV(XS32 ii)
{
  m_NumCvsV=(XU16)ii;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XParametricSurface::GetNumCvsInU() const 
{
  return m_NumCvsU;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XParametricSurface::GetNumCvsInV() const
{
  return m_NumCvsV;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XParametricSurface::GetNumRegions() const 
{
  if(m_TrimmedSurface)
    return m_TrimmedSurface->GetNumRegions();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XParametricSurface::GetNumBoundaries(XS32 region) const 
{
  if(m_TrimmedSurface)
    return m_TrimmedSurface->GetNumBoundaries(region);
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XTrimData::BoundaryType XParametricSurface::GetBoundaryType(XS32 region,XS32 boundary) const
{
  if(m_TrimmedSurface)
    return m_TrimmedSurface->GetBoundaryType(region,boundary);
  return XTrimData::kLast;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XParametricSurface::GetNumCurves(XS32 region,XS32 boundary) const 
{
  if(m_TrimmedSurface)
    return m_TrimmedSurface->GetNumCurves(region,boundary);
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const Curve2D* XParametricSurface::Get2DCurve(XS32 region,XS32 boundary,XS32 curve) const 
{
  if(m_TrimmedSurface)
    return m_TrimmedSurface->Get2DCurve(region,boundary,curve);
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const Curve3D* XParametricSurface::Get3DCurve(XS32 region,XS32 boundary,XS32 curve) const  
{
  if(m_TrimmedSurface)
    return m_TrimmedSurface->Get3DCurve(region,boundary,curve);
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XParametricSurface::XParametricSurface(XModel* pmod) 
  : XParametricBase(pmod),
  m_NumCvsU(0),
  m_NumCvsV(0),
  m_TypeU(0),
  m_TypeV(0),
  m_TrimmedSurface(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XParametricSurface::~XParametricSurface()
{
  delete m_TrimmedSurface;
  m_TrimmedSurface=0;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XParametricSurface::GetApiType() const
{
  return XFn::ParametricSurface;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XParametricSurface::GetFn(XFn::Type type)
{
  if(XFn::ParametricSurface==type)
    return (XParametricSurface*)this;
  return XParametricBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XParametricSurface::GetFn(XFn::Type type) const
{
  if(XFn::ParametricSurface==type)
    return (const XParametricSurface*)this;
  return XParametricBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricSurface::WriteChunk(std::ostream& ofs) 
{
  if(!XParametricBase::WriteChunk(ofs))
    return false;

  if(!m_TrimmedSurface)
    ofs << "\tis_trimmed 0\n";
  else
    ofs << "\tis_trimmed 1\n" << *m_TrimmedSurface;

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricSurface::DoData(XFileIO& io)
{
  DUMPER(XParametricSurface);

  CHECK_IO( XParametricBase::DoData(io) );

  IO_CHECK( io.DoData(&m_NumCvsU) );
  DPARAM( m_NumCvsU );
  IO_CHECK( io.DoData(&m_NumCvsV) );
  DPARAM( m_NumCvsV );
  IO_CHECK( io.DoData(&m_TypeU) );
  DPARAM( m_TypeU );
  IO_CHECK( io.DoData(&m_TypeV) );
  DPARAM( m_TypeV );

  bool btrim=false;
  if(io.IsWriting())
    btrim = (m_TrimmedSurface!=0);

  IO_CHECK( io.DoData(&btrim) );
  DPARAM( btrim );

  if(btrim) 
  {
    if(io.IsReading())
      m_TrimmedSurface = new XTrimData();
    IO_CHECK( m_TrimmedSurface->DoData(io)  );
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XParametricSurface::DoCopy(const XBase* rhs)
{
  const XParametricSurface* cb = rhs->HasFn<XParametricSurface>();
  XMD_ASSERT(cb);

  m_NumCvsU = cb->m_NumCvsU;
  m_NumCvsV = cb->m_NumCvsV;
  m_TypeU = cb->m_TypeU;
  m_TypeV = cb->m_TypeV;
  if (cb->m_TrimmedSurface)
  {
    m_TrimmedSurface = new XTrimData(*cb->m_TrimmedSurface);
  }

  XParametricBase::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XParametricSurface::GetDataSize() const 
{

  XU32 _size_ = XParametricBase::GetDataSize() +
    sizeof(XU8)*2 +
    sizeof(XU16)*2 + sizeof(bool);

  if(m_TrimmedSurface)
    _size_ += m_TrimmedSurface->GetDataSize();
  return _size_;
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricSurface::ReadChunk(std::istream& ifs)
{
  if(!XParametricBase::ReadChunk(ifs))
    return false;

  // see if we have a trimmed surface...
  XS32 isTrimmed;

  READ_CHECK("is_trimmed",ifs);
  ifs >> isTrimmed;

  // in which case read the trim data
  if (isTrimmed == 1)
  {
    // create a trim data structure
    XTrimData* pTrimInfo = new XTrimData();
    XMD_ASSERT(pTrimInfo);

    ifs >> *pTrimInfo;

    // wang it into the surface
    m_TrimmedSurface = pTrimInfo;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
void XParametricSurface::PreDelete(XIdSet& extra_nodes)
{
  XParametricBase::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricSurface::NodeDeath(XId id)
{
  return XParametricBase::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU8 XParametricSurface::GetTypeInV() const
{
  return m_TypeV;
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU8 XParametricSurface::GetTypeInU() const
{
  return m_TypeU;
}
}
