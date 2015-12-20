//----------------------------------------------------------------------------------------------------------------------
/// \file Blinn.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Blinn.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XBlinn::XBlinn(XModel* pmod) 
  : XSpecular(pmod),m_Eccentricity(0),m_ShininessRollOff(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XBlinn::~XBlinn()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XBlinn::PreDelete(XIdSet& extra_nodes)
{
  XSpecular::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlinn::NodeDeath(XId id) 
{
  return XSpecular::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XBlinn::GetEccentricity() const
{
  return m_Eccentricity;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XBlinn::GetSpecularRollOff() const
{
  return m_ShininessRollOff;
}

//----------------------------------------------------------------------------------------------------------------------
void XBlinn::SetEccentricity(const XReal ec)
{
  m_Eccentricity = ec;
}

//----------------------------------------------------------------------------------------------------------------------
void XBlinn::SetSpecularRollOff(const XReal sro) 
{
  m_ShininessRollOff = sro;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XBlinn::GetApiType() const
{
  return XFn::Blinn;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XBlinn::GetFn(XFn::Type type)
{
  if(XFn::Blinn==type)
    return (XBlinn*)this;
  return XSpecular::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XBlinn::GetFn(XFn::Type type) const
{
  if(XFn::Blinn==type)
    return (const XBlinn*)this;
  return XSpecular::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XBlinn::DoCopy(const XBase* rhs)
{
  const XBlinn* cb = rhs->HasFn<XBlinn>();
  XMD_ASSERT(cb);

  m_Eccentricity = cb->m_Eccentricity;
  m_ShininessRollOff = cb->m_ShininessRollOff;
  m_Eccentricity = cb->m_Eccentricity;

  XMaterial::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBlinn::GetDataSize() const 
{
  return XSpecular::GetDataSize() + 2*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlinn::DoData(XFileIO& io) 
{
  DUMPER(XBlinn);

  if(io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XSpecular::DoData(io) );

    IO_CHECK( io.DoData(&m_Eccentricity) );
    DPARAM( m_Eccentricity );

    IO_CHECK( io.DoData(&m_ShininessRollOff) );
    DPARAM( m_ShininessRollOff );
  }
  else
  {
    IO_CHECK( XBase::DoData(io) );

    IO_CHECK( m_Ambient.DoData(io) );
    IO_CHECK( m_Ambient.m_Name == "ambient_color" );
    m_Ambient.m_Name = "ambient_colour";

    IO_CHECK( m_Diffuse.DoData(io) );
    IO_CHECK( m_Diffuse.m_Name == "color" );
    m_Diffuse.m_Name = "colour";

    IO_CHECK( m_Specular.DoData(io) );
    IO_CHECK( m_Specular.m_Name == "specular_color" );
    m_Specular.m_Name = "specular_colour";

    IO_CHECK( m_Emission.DoData(io) );
    IO_CHECK( m_Transparency.DoData(io) );

    IO_CHECK( io.DoData(&m_Shininess) );
    DPARAM( m_Shininess );
    IO_CHECK( io.DoData(&m_BumpMap.m_InputNode) );
    DPARAM( m_BumpMap.m_InputNode );
    IO_CHECK( io.DoData(&m_EnvMap.m_InputNode) );
    DPARAM( m_EnvMap.m_InputNode );
    IO_CHECK( io.DoData(&m_HwShader) );
    DPARAM( m_HwShader );

    IO_CHECK( io.DoData(&m_Eccentricity) );
    DPARAM( m_Eccentricity );

    IO_CHECK( io.DoData(&m_ShininessRollOff) );
    DPARAM( m_ShininessRollOff );
  }

  SetShininess((1.0f-m_Eccentricity)*90.0f);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlinn::ReadChunk(std::istream& ifs)
{
  if(GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK(XSpecular::ReadChunk(ifs));
  }
  else
  {
    READ_CHECK("ambient_color", ifs);
    ifs >> m_Ambient.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> m_Ambient.m_InputNode;

    READ_CHECK("color", ifs);
    ifs >> m_Diffuse.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> m_Diffuse.m_InputNode;

    READ_CHECK("specular_color", ifs);
    ifs >> m_Specular.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> m_Specular.m_InputNode;

    READ_CHECK(m_Emission.m_Name.c_str(), ifs);
    ifs >> m_Emission.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> m_Emission.m_InputNode;

    READ_CHECK(m_Transparency.m_Name.c_str(), ifs);
    ifs >> m_Transparency.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> m_Transparency.m_InputNode;

    READ_CHECK(m_BumpMap.m_Name.c_str(),ifs);
    ifs >> m_BumpMap.m_InputNode;
    READ_CHECK(m_EnvMap.m_Name.c_str(),ifs);
    ifs >> m_EnvMap.m_InputNode;
    READ_CHECK("hw_shader",ifs);
    ifs >> m_HwShader;
  }
  READ_CHECK("eccentricity",ifs);
  ifs >> m_Eccentricity;

  READ_CHECK("specular_roll_off",ifs);
  ifs >> m_ShininessRollOff;

  SetShininess((1.0f-m_Eccentricity)*90.0f);

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlinn::WriteChunk(std::ostream& ofs)
{
  IO_CHECK(XSpecular::WriteChunk(ofs));

  ofs << "\teccentricity "  << m_Eccentricity << std::endl;
  ofs << "\tspecular_roll_off " << m_ShininessRollOff << std::endl;

  return ofs.good();
}
}
