//----------------------------------------------------------------------------------------------------------------------
/// \file Phong.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/PhongE.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XPhongE::XPhongE(XModel* pmod) 
  : XSpecular(pmod),m_HighlightSize(0),m_Roughness(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XPhongE::~XPhongE()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XPhongE::PreDelete(XIdSet& extra_nodes)
{
  XSpecular::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XPhongE::NodeDeath(XId id) 
{
  return XSpecular::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XPhongE::GetHighlightSize() const
{
  return m_HighlightSize;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XPhongE::GetRoughness() const
{
  return m_Roughness;
}

//----------------------------------------------------------------------------------------------------------------------
void XPhongE::SetHighlightSize(const XReal value) 
{
  m_HighlightSize=value;
}

//----------------------------------------------------------------------------------------------------------------------
void XPhongE::SetRoughness(const XReal value) 
{
  m_Roughness=value;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XPhongE::GetApiType() const
{
  return XFn::PhongE;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XPhongE::GetFn(XFn::Type type)
{
  if(XFn::PhongE==type)
    return (XPhongE*)this;
  return XSpecular::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XPhongE::GetFn(XFn::Type type) const
{
  if(XFn::PhongE==type)
    return (const XPhongE*)this;
  return XSpecular::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPhongE::GetDataSize() const 
{
  return XSpecular::GetDataSize() + 2*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
void XPhongE::DoCopy(const XBase* rhs)
{
  const XPhongE* cb = rhs->HasFn<XPhongE>();
  XMD_ASSERT(cb);
  if(cb)
  {
    m_HighlightSize = cb->m_HighlightSize;
    m_Roughness = cb->m_Roughness;
    XSpecular::DoCopy(cb);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XPhongE::DoData(XFileIO& io) 
{
  DUMPER(XPhongE);

  if(io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XSpecular::DoData(io) );
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
  }

  IO_CHECK( io.DoData(&m_HighlightSize) );
  DPARAM( m_HighlightSize );

  IO_CHECK( io.DoData(&m_Roughness) );
  DPARAM( m_Roughness );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPhongE::ReadChunk(std::istream& ifs)
{
  if(GetModel()->GetInputVersion()>=4)
  {
    XSpecular::ReadChunk(ifs);
    READ_CHECK("highlight_size",ifs);
    ifs >> m_HighlightSize;
    READ_CHECK("roughness",ifs);
    ifs >> m_Roughness;
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

    READ_CHECK("highlight_size",ifs);
    ifs >> m_HighlightSize;
    READ_CHECK("roughness",ifs);
    ifs >> m_Roughness;
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPhongE::WriteChunk(std::ostream& ofs)
{
  IO_CHECK(XSpecular::WriteChunk(ofs));

  ofs << "\thighlight_size " << m_HighlightSize << std::endl;
  ofs << "\troughness " << m_Roughness << std::endl;
  return ofs.good();
}
}
