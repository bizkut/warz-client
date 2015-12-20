//----------------------------------------------------------------------------------------------------------------------
/// \file Turbulence.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Turbulence.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XTurbulence::XTurbulence(XModel* pmod) 
  : XField(pmod),m_Frequency(1.0f),m_Phase(0,0,0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XTurbulence::~XTurbulence()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XTurbulence::GetApiType() const
{
  return XFn::Turbulence;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XTurbulence::GetDataSize() const 
{
  return XField::GetDataSize() + sizeof(XReal) + 3*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTurbulence::GetFrequency() const 
{
  return m_Frequency;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTurbulence::GetPhase() const 
{
  return m_Phase.z;
}

//----------------------------------------------------------------------------------------------------------------------
void XTurbulence::GetPhase(XVector3& phase) const
{
  phase = m_Phase;
}

//----------------------------------------------------------------------------------------------------------------------
void XTurbulence::SetPhase(const XVector3& phase)
{
  m_Phase = phase;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XTurbulence::GetFn(XFn::Type type)
{
  if(XFn::Turbulence==type)
    return (XTurbulence*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XTurbulence::GetFn(XFn::Type type) const
{
  if(XFn::Turbulence==type)
    return (const XTurbulence*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XTurbulence::SetFrequency(const XReal v) 
{
  m_Frequency = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XTurbulence::SetPhase(const XReal v) 
{
  m_Phase.x = m_Phase.y = m_Phase.z = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTurbulence::WriteChunk(std::ostream& ofs)
{
  if(!XField::WriteChunk(ofs))
    return false;

  ofs << "\tfrequency " << m_Frequency << "\n";
  ofs << "\tphase " << m_Phase << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XTurbulence::DoData(XFileIO& io)
{
  DUMPER(XTurbulence);

  CHECK_IO( XField::DoData(io) );

  CHECK_IO( io.DoData( &m_Frequency ) );
  DPARAM(m_Frequency);

  if (GetModel()->GetInputVersion()>=4)
  {
    CHECK_IO( io.DoData( &m_Phase ) );
    DPARAM(m_Phase);
  }
  else
  {
    CHECK_IO( io.DoData( &m_Phase.x ) );
    DPARAM(m_Phase.x);
    m_Phase.y = m_Phase.z = m_Phase.x;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XTurbulence::DoCopy(const XBase* rhs)
{
  const XTurbulence* cb = rhs->HasFn<XTurbulence>();
  XMD_ASSERT(cb);
  m_Phase = cb->m_Phase;
  m_Frequency = cb->m_Frequency;
  XField::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTurbulence::ReadChunk(std::istream& ifs)
{
  if(!XField::ReadChunk(ifs))
    return false;

  READ_CHECK("frequency",ifs);
  ifs >> m_Frequency;
  
  READ_CHECK("phase",ifs);
  if (GetModel()->GetInputVersion()>=4)
  {
    ifs >> m_Phase;
  }
  else
  {
    ifs >> m_Phase.x;
    m_Phase.y = m_Phase.z = m_Phase.x;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XTurbulence::NodeDeath(XId id)
{
  return XField::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XTurbulence::PreDelete(XIdSet& extra_nodes)
{
  XField::PreDelete(extra_nodes);
}
}
