//----------------------------------------------------------------------------------------------------------------------
/// \file SpotLight.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/SpotLight.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XSpotLight::XSpotLight(XModel* pmod) 
  : XExtendedLight(pmod)
{
  m_ConeAngle = 0.698132f;

  m_PenumbraAngle = 0;

  m_DropOff = 0;

  m_BarnDoors = 0;

  m_LeftBarnDoorAngle =
    m_RightBarnDoorAngle =
    m_TopBarnDoorAngle =
    m_BottomBarnDoorAngle = 0.349066f;

  m_UseDecayRegions = 0;

  m_Decay1[0]=1;
  m_Decay1[1]=2;
  m_Decay2[0]=3;
  m_Decay2[1]=6;
  m_Decay3[0]=8;
  m_Decay3[1]=10;
}

//----------------------------------------------------------------------------------------------------------------------
XSpotLight::~XSpotLight()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::DoCopy(const XBase* rhs)
{
  const XSpotLight* cb = rhs->HasFn<XSpotLight>();
  XMD_ASSERT(cb);

  m_ConeAngle = cb->m_ConeAngle;
  m_PenumbraAngle = cb->m_PenumbraAngle;
  m_DropOff = cb->m_DropOff;
  m_BarnDoors = cb->m_BarnDoors;
  m_LeftBarnDoorAngle = cb->m_LeftBarnDoorAngle;
  m_RightBarnDoorAngle = cb->m_RightBarnDoorAngle;
  m_TopBarnDoorAngle = cb->m_TopBarnDoorAngle;
  m_BottomBarnDoorAngle = cb->m_BottomBarnDoorAngle;
  m_UseDecayRegions = cb->m_UseDecayRegions;
  m_Decay1[0] = cb->m_Decay1[0];
  m_Decay1[1] = cb->m_Decay1[1];
  m_Decay2[0] = cb->m_Decay2[0];
  m_Decay2[1] = cb->m_Decay2[1];
  m_Decay3[0] = cb->m_Decay3[0];
  m_Decay3[1] = cb->m_Decay3[1];

  XExtendedLight::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XSpotLight::GetApiType() const
{
  return XFn::SpotLight;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XSpotLight::GetFn(XFn::Type type)
{
  if(XFn::SpotLight==type)
    return (XSpotLight*)this;
  return XExtendedLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XSpotLight::GetFn(XFn::Type type) const
{
  if(XFn::SpotLight==type)
    return (const XSpotLight*)this;
  return XExtendedLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XSpotLight::WriteChunk(std::ostream& ofs)
{
  if(! XExtendedLight::WriteChunk(ofs))
    return false;

  ofs << "\tcone_angle\t" << m_ConeAngle << "\n";
  ofs << "\tpenumbra_angle\t" << m_PenumbraAngle << "\n";
  ofs << "\tdrop_off\t" << m_DropOff << "\n";
  ofs << "\tbarn_doors\t" << m_BarnDoors << "\n";
  ofs << "\t\tleft_barn_door_angle\t" << m_LeftBarnDoorAngle << "\n";
  ofs << "\t\tright_barn_door_angle\t" << m_RightBarnDoorAngle << "\n";
  ofs << "\t\ttop_barn_door_angle\t" << m_TopBarnDoorAngle << "\n";
  ofs << "\t\tbottom_barn_door_angle\t" << m_BottomBarnDoorAngle << "\n";
  ofs << "\tuse_decay_regions\t" << m_UseDecayRegions << "\n";
  ofs << "\t\tdecay_1\t" << m_Decay1[0] << " " << m_Decay1[1] << "\n";
  ofs << "\t\tdecay_2\t" << m_Decay2[0] << " " << m_Decay2[1] << "\n";
  ofs << "\t\tdecay_3\t" << m_Decay3[0] << " " << m_Decay3[1] << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XSpotLight::DoData(XFileIO& io) 
{
  DUMPER(XSpotLight);

  IO_CHECK( XExtendedLight::DoData(io) );

  IO_CHECK( io.DoData(&m_ConeAngle) );
  DPARAM( m_ConeAngle );

  IO_CHECK( io.DoData(&m_PenumbraAngle) );
  DPARAM( m_PenumbraAngle );

  IO_CHECK( io.DoData(&m_DropOff) );
  DPARAM( m_DropOff );

  IO_CHECK( io.DoData(&m_BarnDoors) );
  DPARAM( m_BarnDoors );

  IO_CHECK( io.DoData(&m_LeftBarnDoorAngle) );
  DPARAM( m_LeftBarnDoorAngle );

  IO_CHECK( io.DoData(&m_RightBarnDoorAngle) );
  DPARAM( m_RightBarnDoorAngle );

  IO_CHECK( io.DoData(&m_TopBarnDoorAngle) );
  DPARAM( m_TopBarnDoorAngle );

  IO_CHECK( io.DoData(&m_BottomBarnDoorAngle) );
  DPARAM( m_BottomBarnDoorAngle );

  IO_CHECK( io.DoData(&m_UseDecayRegions) );
  DPARAM( m_UseDecayRegions );

  IO_CHECK( io.DoData(m_Decay1,2) );
  DPARAM( m_Decay1[0] );
  DPARAM( m_Decay1[1] );

  IO_CHECK( io.DoData(m_Decay2,2) );
  DPARAM( m_Decay2[0] );
  DPARAM( m_Decay2[1] );

  IO_CHECK( io.DoData(m_Decay3,2) );
  DPARAM( m_Decay3[0] );
  DPARAM( m_Decay3[1] );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSpotLight::GetDataSize() const 
{
  return sizeof(XReal)*13 + 2*sizeof(bool) + XExtendedLight::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XSpotLight::ReadChunk(std::istream& ifs)
{
  if(!XExtendedLight::ReadChunk(ifs))
    return false;

  READ_CHECK("cone_angle",ifs);
  ifs >> m_ConeAngle;

  READ_CHECK("penumbra_angle",ifs);
  ifs >> m_PenumbraAngle;

  READ_CHECK("drop_off",ifs);
  ifs >> m_DropOff;

  READ_CHECK("barn_doors",ifs);
  ifs >> m_BarnDoors;

  READ_CHECK("left_barn_door_angle",ifs);
  ifs >> m_LeftBarnDoorAngle;

  READ_CHECK("right_barn_door_angle",ifs);
  ifs >> m_RightBarnDoorAngle;

  READ_CHECK("top_barn_door_angle",ifs);
  ifs >> m_TopBarnDoorAngle;

  READ_CHECK("bottom_barn_door_angle",ifs);
  ifs >> m_BottomBarnDoorAngle;

  READ_CHECK("use_decay_regions",ifs);
  ifs >> m_UseDecayRegions;

  READ_CHECK("decay_1",ifs);
  ifs >> m_Decay1[0] >> m_Decay1[1];

  READ_CHECK("decay_2",ifs);
  ifs >> m_Decay2[0] >> m_Decay2[1];

  READ_CHECK("decay_3",ifs);
  ifs >> m_Decay3[0] >> m_Decay3[1];

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XSpotLight::NodeDeath(XId id)
{
  return XExtendedLight::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::PreDelete(XIdSet& extra_nodes)
{
  XExtendedLight::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XSpotLight::GetConeAngle() const 
{
  return m_ConeAngle;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XSpotLight::GetPenumbraAngle() const
{
  return m_PenumbraAngle;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XSpotLight::GetDropOff() const
{
  return m_DropOff;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSpotLight::GetHasBarnDoors() const
{
  return m_BarnDoors;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XSpotLight::GetLeftBarnDoorAngle() const 
{
  return m_LeftBarnDoorAngle;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XSpotLight::GetRightBarnDoorAngle() const 
{
  return m_RightBarnDoorAngle;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XSpotLight::GetTopBarnDoorAngle() const 
{
  return m_TopBarnDoorAngle;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XSpotLight::GetBottomBarnDoorAngle() const
{
  return m_BottomBarnDoorAngle;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSpotLight::GetUseDecayRegions() const
{
  return m_UseDecayRegions;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::GetFirstDecayRegion(XReal& start, XReal& end) const 
{
  start = m_Decay1[0];
  end   = m_Decay1[1];
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::GetSecondDecayRegion(XReal& start, XReal& end) const
{
  start = m_Decay2[0];
  end   = m_Decay2[1];
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::GetThirdDecayRegion(XReal& start, XReal& end) const 
{
  start = m_Decay3[0];
  end   = m_Decay3[1];
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::SetConeAngle(const XReal v)
{
  m_ConeAngle=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::SetPenumbraAngle(const XReal v)
{
  m_PenumbraAngle=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::SetDropOff(const XReal v)
{
  m_DropOff=v;
}

//----------------------------------------------------------------------------------------------------------------------
void  XSpotLight::SetHasBarnDoors(const bool v)
{
  m_BarnDoors=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::SetLeftBarnDoorAngle(const XReal v)
{
  m_LeftBarnDoorAngle=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::SetRightBarnDoorAngle(const XReal v)
{
  m_RightBarnDoorAngle=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::SetTopBarnDoorAngle(const XReal v)
{
  m_TopBarnDoorAngle=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::SetBottomBarnDoorAngle(const XReal v)
{
  m_BottomBarnDoorAngle=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::SetUseDecayRegions(const bool v)
{
  m_UseDecayRegions=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::SetFirstDecayRegion(const XReal start, const XReal end)
{
  m_Decay1[0] = start;
  m_Decay1[1] = end;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::SetSecondDecayRegion(const XReal start, const XReal end)
{
  m_Decay2[0] = start;
  m_Decay2[1] = end;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpotLight::SetThirdDecayRegion(const XReal start, const XReal end)
{
  m_Decay3[0] = start;
  m_Decay3[1] = end;
}
}
