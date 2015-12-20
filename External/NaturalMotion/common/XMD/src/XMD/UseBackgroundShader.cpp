//----------------------------------------------------------------------------------------------------------------------
/// \file UseBackground.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/UseBackgroundShader.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XUseBackground::XUseBackground(XModel* pmod)
  : XMaterial(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XUseBackground::~XUseBackground()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XUseBackground::GetApiType() const
{
  return XFn::UseBackgroundShader;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XUseBackground::GetFn(XFn::Type type)
{
  if(XFn::UseBackgroundShader==type)
    return (XUseBackground*)this;
  return XMaterial::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XUseBackground::GetFn(XFn::Type type) const
{
  if(XFn::UseBackgroundShader==type)
    return (const XUseBackground*)this;
  return XMaterial::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XUseBackground::DoCopy(const XBase* rhs)
{
  const XUseBackground* cb = rhs->HasFn<XUseBackground>();
  XMD_ASSERT(cb);
  if(cb)
  {
    XMaterial::DoCopy(cb);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XUseBackground::NodeDeath(XId id)
{
  return XMaterial::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XUseBackground::PreDelete(XIdSet& extra_nodes)
{
  XMaterial::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XUseBackground::GetDataSize() const
{
  return XMaterial::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XUseBackground::DoData(XFileIO& io)
{
  DUMPER(XUseBackground);

  if(io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XMaterial::DoData(io) );
  }
  else
  {
    IO_CHECK( XBase::DoData(io) );

    XColourConnection temp("ambient");
    IO_CHECK( temp.DoData(io) );

    IO_CHECK( m_Diffuse.DoData(io) );
    IO_CHECK( m_Diffuse.m_Name == "color" );
    m_Diffuse.m_Name = "colour";

    temp.m_Name = "specular_color";
    IO_CHECK( temp.DoData(io) );
    temp.m_Name = "incandescence";
    IO_CHECK( temp.DoData(io) );

    temp.m_Name = "transparency";
    IO_CHECK( temp.DoData(io) );

    // legacy, lambert has no shininess
    XReal shininess = 0;
    IO_CHECK( io.DoData(&shininess) );
    DPARAM( shininess );

    temp.m_Name = "bump_map";
    IO_CHECK( io.DoData(&temp.m_InputNode) );
    DPARAM( temp.m_InputNode );
    temp.m_Name = "env_map";
    IO_CHECK( io.DoData(&temp.m_InputNode) );
    DPARAM( temp.m_InputNode );
    IO_CHECK( io.DoData(&m_HwShader) );
    DPARAM( m_HwShader );
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XUseBackground::ReadChunk(std::istream& ifs)
{
  if(GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XMaterial::ReadChunk(ifs) );
  }
  else
  {
    XId temp_id;
    XColour temp_colour;
    READ_CHECK("ambient_color", ifs);
    ifs >> temp_colour;
    READ_CHECK("tex", ifs);
    ifs >> temp_id;

    READ_CHECK("color", ifs);
    ifs >> m_Diffuse.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> m_Diffuse.m_InputNode;

    READ_CHECK("specular_color", ifs);
    ifs >> temp_colour;
    READ_CHECK("tex", ifs);
    ifs >> temp_id;

    READ_CHECK("incandescence", ifs);
    ifs >> temp_colour;
    READ_CHECK("tex", ifs);
    ifs >> temp_id;

    READ_CHECK("transparency", ifs);
    ifs >> temp_colour;
    READ_CHECK("tex", ifs);
    ifs >> temp_id;

    READ_CHECK("bump_map",ifs);
    ifs >> temp_id;
    READ_CHECK("env_map",ifs);
    ifs >> temp_id;
    READ_CHECK("hw_shader",ifs);
    ifs >> m_HwShader;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XUseBackground::WriteChunk(std::ostream& ofs)
{
  IO_CHECK( XMaterial::WriteChunk(ofs) );

  return ofs.good();
}
}
