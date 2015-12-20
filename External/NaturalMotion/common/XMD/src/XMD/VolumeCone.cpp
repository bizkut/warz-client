//----------------------------------------------------------------------------------------------------------------------
/// \file VolumeCone.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/VolumeCone.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XVolumeCone::XVolumeCone(XModel* pmod) 
  : XVolumeObject(pmod),m_Angle(0.785398f),m_Cap(1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XVolumeCone::~XVolumeCone()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XVolumeCone::GetApiType() const
{
  return XFn::VolumeCone;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XVolumeCone::GetDataSize() const 
{
  return XVolumeObject::GetDataSize() + 2*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XVolumeCone::GetFn(XFn::Type type)
{
  if(XFn::VolumeCone==type)
    return (XVolumeCone*)this;
  return XVolumeObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XVolumeCone::GetFn(XFn::Type type) const
{
  if(XFn::VolumeCone==type)
    return (const XVolumeCone*)this;
  return XVolumeObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeCone::NodeDeath(XId id)
{
  return XVolumeObject::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeCone::PreDelete(XIdSet& extra_nodes)
{
  XVolumeObject::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeCone::GetAngle() const
{
  return m_Angle;
}

//---------------------------------------------------------------------------------------------------------------------- 
XReal XVolumeCone::GetCap() const
{
  return m_Cap;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeCone::SetAngle(const XReal v)
{
  m_Angle  = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeCone::SetCap(const XReal v)
{
  m_Cap = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeCone::ReadChunk(std::istream& ifs)
{
  READ_CHECK("angle",ifs);
  ifs >> m_Angle;

  READ_CHECK("cap",ifs);
  ifs >> m_Cap;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeCone::WriteChunk(std::ostream& ofs)
{
  // the camera's aspect ratio
  ofs << "\tangle " << m_Angle << "\n";
  ofs << "\tcap " << m_Cap << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeCone::DoData(XFileIO& io) 
{
  DUMPER(XVolumeCone);

  IO_CHECK( XVolumeObject::DoData(io) );

  IO_CHECK( io.DoData(&m_Angle) );
  DPARAM( m_Angle );

  IO_CHECK( io.DoData(&m_Cap) );
  DPARAM( m_Cap );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeCone::DoCopy(const XBase* rhs)
{
  const XVolumeCone* cb = rhs->HasFn<XVolumeCone>();
  XMD_ASSERT(cb);
  m_Angle = cb->m_Angle;
  m_Cap = cb->m_Cap;
  XVolumeObject::DoCopy(cb);
}
}
