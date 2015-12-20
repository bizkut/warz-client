//----------------------------------------------------------------------------------------------------------------------
/// \file Newton.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Newton.h"
#include "XMD/FileIO.h"

namespace XMD
{
  //----------------------------------------------------------------------------------------------------------------------
  XNewton::XNewton(XModel* pmod) 
    : XField(pmod),m_MinDistance(0)
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  XNewton::~XNewton()
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  XFn::Type XNewton::GetApiType() const
  {
    return XFn::Newton;
  }

  //----------------------------------------------------------------------------------------------------------------------
  XU32 XNewton::GetDataSize() const 
  {
    return XField::GetDataSize() + sizeof(XReal) ;
  }

  //----------------------------------------------------------------------------------------------------------------------
  XBase* XNewton::GetFn(XFn::Type type)
  {
    if(XFn::Newton==type)
      return (XNewton*)this;
    return XField::GetFn(type);
  }

  //----------------------------------------------------------------------------------------------------------------------
  const XBase* XNewton::GetFn(XFn::Type type) const
  {
    if(XFn::Newton==type)
      return (const XNewton*)this;
    return XField::GetFn(type);
  }

  //----------------------------------------------------------------------------------------------------------------------
  XReal XNewton::GetMinDistance() const 
  {
    return m_MinDistance;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XNewton::SetMinDistance(const XReal dist)
  {
    m_MinDistance = dist;
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XNewton::WriteChunk(std::ostream& ofs)
  {
    if(!XField::WriteChunk(ofs))
      return false;

    // the camera's aspect ratio
    ofs << "\tmin_distance " << m_MinDistance << "\n";

    return ofs.good();
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XNewton::DoData(XFileIO& io) 
  {
    DUMPER(XNewton);

    CHECK_IO( XField::DoData(io) );

    CHECK_IO( io.DoData( &m_MinDistance ) );
    DPARAM(m_MinDistance);
    return true;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XNewton::DoCopy(const XBase* rhs)
  {
    const XNewton* cb = rhs->HasFn<XNewton>();
    XMD_ASSERT(cb);
    m_MinDistance = cb->m_MinDistance;
    XField::DoCopy(cb);
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XNewton::ReadChunk(std::istream& ifs)
  {
    if(!XField::ReadChunk(ifs))
      return false;

    READ_CHECK("min_distance",ifs);
    ifs >> m_MinDistance;

    return ifs.good();
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XNewton::PreDelete(XIdSet& extra_nodes)
  {
    XField::PreDelete(extra_nodes);
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XNewton::NodeDeath(XId id)
  {
    return XField::NodeDeath(id);
  }
}
