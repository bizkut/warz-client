//----------------------------------------------------------------------------------------------------------------------
/// \file Phong.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Phong.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XPhong::XPhong(XModel* pmod) 
  : XSpecular(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XPhong::~XPhong()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XPhong::PreDelete(XIdSet& extra_nodes)
{
  XSpecular::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XPhong::NodeDeath(XId id) 
{
  return XSpecular::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XPhong::GetApiType() const
{
  return XFn::Phong;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XPhong::GetFn(XFn::Type type)
{
  if(XFn::Phong==type)
    return (XPhong*)this;
  return XSpecular::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XPhong::GetFn(XFn::Type type) const
{
  if(XFn::Phong==type)
    return (const XPhong*)this;
  return XSpecular::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XPhong::DoCopy(const XBase* rhs)
{
  const XPhong* cb = rhs->HasFn<XPhong>();
  XMD_ASSERT(cb);
  if(cb)
  {
    m_Shininess = cb->m_Shininess;
    XSpecular::DoCopy(cb);
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPhong::GetDataSize() const
{
  XU32 size = XSpecular::GetDataSize();
  size += sizeof(XReal);
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPhong::DoData(XFileIO& io) 
{
  DUMPER(XPhong);

  if(io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XSpecular::DoData(io) );
    IO_CHECK( io.DoData(&m_Shininess) );
    DPARAM( m_Shininess );
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

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPhong::ReadChunk(std::istream& ifs)
{
  DUMPER(XPhong__ReadChunk);

  if(GetModel()->GetInputVersion()>=4)
  {
    XSpecular::ReadChunk(ifs);
    READ_CHECK("shininess",ifs);
    ifs >> m_Shininess;
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
    READ_CHECK("shininess",ifs);
    ifs >> m_Shininess;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPhong::WriteChunk(std::ostream& ofs)
{
  DUMPER(XPhong__WriteChunk);

  IO_CHECK(XSpecular::WriteChunk(ofs));
  ofs << "\tshininess " << m_Shininess << std::endl;

  return ofs.good();
}
}
