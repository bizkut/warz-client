//----------------------------------------------------------------------------------------------------------------------
/// \file ExtendedLight.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ExtendedLight.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XExtendedLight::XExtendedLight(XModel* pmod) 
  : XLight(pmod)
{
  m_bUseDepthMapShadows = false;
  m_bCastSoftShadows = false;
  m_ShadowRadius = 0;
  m_DecayRate = 0;
  m_DepthMapFilterSize= 1;
  m_DepthMapResolution=512;
  m_DepthMapBias=0.01f;
}

//----------------------------------------------------------------------------------------------------------------------
XExtendedLight::~XExtendedLight()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XExtendedLight::GetApiType() const
{
  return XFn::ExtendedLight;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XExtendedLight::GetFn(XFn::Type type)
{
  if(XFn::ExtendedLight==type)
    return (XExtendedLight*)this;
  return XLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XExtendedLight::GetFn(XFn::Type type) const
{
  if(XFn::ExtendedLight==type)
    return (const XExtendedLight*)this;
  return XLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XExtendedLight::WriteChunk(std::ostream& ofs)
{
  if(!XLight::WriteChunk(ofs))
    return false;

  ofs << "\tuse_depth_map_shadows\t" << m_bUseDepthMapShadows << "\n";
  ofs << "\tcast_soft_shadows\t" << m_bCastSoftShadows << "\n";
  ofs << "\tshadow_radius\t" << m_ShadowRadius << "\n";
  ofs << "\tdecay_rate\t" << m_DecayRate << "\n";
  ofs << "\tdepth_map_filter_size\t" << m_DepthMapFilterSize << "\n";
  ofs << "\tdepth_map_resolution\t" << m_DepthMapResolution << "\n";
  ofs << "\tdepth_map_bias\t" << m_DepthMapBias << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XExtendedLight::DoData(XFileIO& io)
{
  DUMPER(XExtendedLight);

  IO_CHECK( XLight::DoData(io) );

  IO_CHECK( io.DoData(&m_bUseDepthMapShadows) );
  DPARAM( m_bUseDepthMapShadows );

  IO_CHECK( io.DoData(&m_bCastSoftShadows) );
  DPARAM( m_bCastSoftShadows );

  IO_CHECK( io.DoData(&m_ShadowRadius) );
  DPARAM( m_ShadowRadius );

  IO_CHECK( io.DoData(&m_DecayRate) );
  DPARAM( m_DecayRate );

  IO_CHECK( io.DoData(&m_DepthMapFilterSize) );
  DPARAM( m_DepthMapFilterSize );

  IO_CHECK( io.DoData(&m_DepthMapResolution) );
  DPARAM( m_DepthMapResolution );

  IO_CHECK( io.DoData(&m_DepthMapBias) );
  DPARAM( m_DepthMapBias );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XExtendedLight::DoCopy(const XBase* rhs)
{
  const XExtendedLight* cb = rhs->HasFn<XExtendedLight>();
  XMD_ASSERT(cb);

  m_bUseDepthMapShadows = cb->m_bUseDepthMapShadows;
  m_bCastSoftShadows = cb->m_bCastSoftShadows;
  m_ShadowRadius = cb->m_ShadowRadius;
  m_DecayRate = cb->m_DecayRate;
  m_DepthMapFilterSize = cb->m_DepthMapFilterSize;
  m_DepthMapResolution = cb->m_DepthMapResolution;
  m_DepthMapBias = cb->m_DepthMapBias;

  XLight::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XExtendedLight::GetDataSize() const
{
  return sizeof(XReal)*5 + 2*sizeof(bool) + XLight::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XExtendedLight::ReadChunk(std::istream& ifs)
{
  if(!XLight::ReadChunk(ifs))
    return false;

  READ_CHECK("use_depth_map_shadows",ifs);
  ifs >> m_bUseDepthMapShadows;

  READ_CHECK("cast_soft_shadows",ifs);
  ifs >> m_bCastSoftShadows;

  READ_CHECK("shadow_radius",ifs);
  ifs >> m_ShadowRadius;

  READ_CHECK("decay_rate",ifs);
  ifs >> m_DecayRate;

  READ_CHECK("depth_map_filter_size",ifs);
  ifs >> m_DepthMapFilterSize;

  READ_CHECK("depth_map_resolution",ifs);
  ifs >> m_DepthMapResolution;

  READ_CHECK("depth_map_bias",ifs);
  ifs >> m_DepthMapBias;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XExtendedLight::NodeDeath(XId id)
{
  return XLight::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XExtendedLight::PreDelete(XIdSet& extra_nodes)
{
  XLight::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XExtendedLight::GetUseDepthMapShadows() const 
{
  return m_bUseDepthMapShadows;
}

//----------------------------------------------------------------------------------------------------------------------
bool XExtendedLight::GetCastSoftShadows() const 
{
  return m_bCastSoftShadows;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XExtendedLight::GetShadowRadius() const 
{
  return m_ShadowRadius;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XExtendedLight::GetDecayRate() const
{
  return m_DecayRate;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XExtendedLight::GetDepthMapFilterSize() const
{
  return m_DepthMapFilterSize;
}

//-------------------------------------------------------------------  XExtendedLight :: GetDepthMapResolution
// 
XReal XExtendedLight::GetDepthMapResolution() const 
{
  return m_DepthMapResolution;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XExtendedLight::GetDepthMapBias() const
{
  return m_DepthMapBias;
}

//----------------------------------------------------------------------------------------------------------------------
void XExtendedLight::SetUseDepthMapShadows(bool v) 
{
  m_bUseDepthMapShadows=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XExtendedLight::SetCastSoftShadows(bool v)  
{
  m_bCastSoftShadows=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XExtendedLight::SetShadowRadius(XReal v)  
{
  m_ShadowRadius=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XExtendedLight::SetDecayRate(XReal v) 
{
  m_DecayRate=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XExtendedLight::SetDepthMapFilterSize(XReal v) 
{
  m_DepthMapFilterSize=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XExtendedLight::SetDepthMapResolution(XReal v) 
{
  m_DepthMapResolution=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XExtendedLight::SetDepthMapBias(XReal v) 
{
  m_DepthMapBias=v;
}
}
