//----------------------------------------------------------------------------------------------------------------------
/// \file SurfaceShader.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/SurfaceShader.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XSurfaceShader::XSurfaceShader(XModel* pmod) : XMaterial(pmod),
  m_Transparency("transparency")
{
  m_Connections.push_back(&m_Transparency);
}

//----------------------------------------------------------------------------------------------------------------------
XSurfaceShader::~XSurfaceShader(void)
{
}
//----------------------------------------------------------------------------------------------------------------------
void XSurfaceShader::PreDelete(XIdSet& extra_nodes)
{
  XMaterial::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XSurfaceShader::NodeDeath(XId id) 
{
  return XMaterial::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XSurfaceShader::SetTransparency(const XReal r,const XReal g,const XReal b,const XReal a)
{
  m_Transparency.m_Colour.r = r;
  m_Transparency.m_Colour.g = g;
  m_Transparency.m_Colour.b = b;
  m_Transparency.m_Colour.a = a;
}

//----------------------------------------------------------------------------------------------------------------------
void XSurfaceShader::SetTransparency(const XReal rgba[4])
{
  m_Transparency.m_Colour.r = rgba[0];
  m_Transparency.m_Colour.g = rgba[1];
  m_Transparency.m_Colour.b = rgba[2];
  m_Transparency.m_Colour.a = rgba[3];
}

//----------------------------------------------------------------------------------------------------------------------
void XSurfaceShader::SetTransparency(const XColour& rgba) 
{
  m_Transparency.m_Colour = rgba;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSurfaceShader::SetTransparency(const XShadingNode* node)
{
  XMD_ASSERT(node);
  if(node)
  {
    if(m_pModel == node->GetModel())
    {
      m_Transparency.m_InputNode = node->GetID();
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XSurfaceShader::GetTransparency(XReal &r,XReal &g,XReal &b) const
{
  r = m_Transparency.m_Colour.r;
  g = m_Transparency.m_Colour.g;
  b = m_Transparency.m_Colour.b;
}

//----------------------------------------------------------------------------------------------------------------------
void XSurfaceShader::GetTransparency(XReal &r,XReal &g,XReal &b,XReal &a) const
{
  r = m_Transparency.m_Colour.r;
  g = m_Transparency.m_Colour.g;
  b = m_Transparency.m_Colour.b;
  a = m_Transparency.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
void XSurfaceShader::GetTransparency(XColour& c) const
{
  c = m_Transparency.m_Colour;
}

//----------------------------------------------------------------------------------------------------------------------
void XSurfaceShader::GetTransparency(XReal rgba[4]) const
{
  rgba[0] = m_Transparency.m_Colour.r;
  rgba[1] = m_Transparency.m_Colour.g;
  rgba[2] = m_Transparency.m_Colour.b;
  rgba[3] = m_Transparency.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode* XSurfaceShader::GetTransparency()
{
  XBase* base = GetModel()->FindNode(m_Transparency.m_InputNode);
  if(base)
  {
    return base->HasFn<XShadingNode>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XSurfaceShader::GetApiType() const
{
  return XFn::SurfaceShader;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XSurfaceShader::GetFn(XFn::Type type)
{
  if(XFn::SurfaceShader==type)
    return (XSurfaceShader*)this;
  return XMaterial::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XSurfaceShader::GetFn(XFn::Type type) const
{
  if(XFn::SurfaceShader==type)
    return (const XSurfaceShader*)this;
  return XMaterial::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XSurfaceShader::DoCopy(const XBase* rhs)
{
  const XSurfaceShader* cb = rhs->HasFn<XSurfaceShader>();
  XMD_ASSERT(cb);
  if(cb)
  {
    m_Transparency = cb->m_Transparency;
  }
  XMaterial::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSurfaceShader::GetDataSize() const
{
  XU32 size = XMaterial::GetDataSize();
  size += m_Transparency.GetDataSize();

  return size;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSurfaceShader::DoData(XFileIO& io) 
{
  DUMPER(XSurfaceShader);

  if(io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XMaterial::DoData(io) );
    IO_CHECK( m_Transparency.DoData(io) );
  }
  else
  {
    IO_CHECK( XBase::DoData(io) );

    // legacy, surface shader has no ambient
    XColourConnection ambient("ambient_color");
    IO_CHECK( ambient.DoData(io) );

    IO_CHECK( m_Diffuse.DoData(io) );
    IO_CHECK( m_Diffuse.m_Name == "color" );
    m_Diffuse.m_Name = "colour";

    // legacy, surface shader has no specular
    XColourConnection specular("specular_color");
    IO_CHECK( specular.DoData(io) );

    // legacy, surface shader has no emission
    XColourConnection emission("incandescence");
    IO_CHECK( emission.DoData(io) );

    IO_CHECK( m_Transparency.DoData(io) );

    // legacy, surface shader has no shininess
    XReal shininess = 0;
    IO_CHECK( io.DoData(&shininess) );
    DPARAM( shininess );

    // legacy, surface shader has no bump map
    XColourConnection bumpMap("bump_map");
    IO_CHECK( io.DoData(&bumpMap.m_InputNode) );
    DPARAM( bumpMap.m_InputNode );

    // legacy, surface shader has no environment map
    XColourConnection envMap("env_map");
    IO_CHECK( io.DoData(&envMap.m_InputNode) );
    DPARAM( envMap.m_InputNode );

    IO_CHECK( io.DoData(&m_HwShader) );
    DPARAM( m_HwShader );
  }
 
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSurfaceShader::ReadChunk(std::istream& ifs)
{
  if(GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XMaterial::ReadChunk(ifs) );
    IO_CHECK( m_Transparency.ReadChunk(ifs) );
  }
  else
  {
    XColourConnection temp("");

    READ_CHECK("ambient_color", ifs);
    ifs >> temp.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> temp.m_InputNode;

    READ_CHECK("color", ifs);
    ifs >> m_Diffuse.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> m_Diffuse.m_InputNode;

    READ_CHECK("specular_color", ifs);
    ifs >> temp.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> temp.m_InputNode;

    READ_CHECK("incandescence", ifs);
    ifs >> temp.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> temp.m_InputNode;

    READ_CHECK(m_Transparency.m_Name.c_str(), ifs);
    ifs >> m_Transparency.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> m_Transparency.m_InputNode;

    READ_CHECK("bump_map",ifs);
    ifs >> temp.m_InputNode;
    READ_CHECK("env_map",ifs);
    ifs >> temp.m_InputNode;

    READ_CHECK("hw_shader",ifs);
    ifs >> m_HwShader;
  }  

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XSurfaceShader::WriteChunk(std::ostream& ofs)
{
  IO_CHECK(XMaterial::WriteChunk(ofs));
  IO_CHECK(m_Transparency.WriteChunk(ofs));

  return ofs.good();
}
}
