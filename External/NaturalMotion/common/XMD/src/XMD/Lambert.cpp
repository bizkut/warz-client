//----------------------------------------------------------------------------------------------------------------------
/// \file Lambert.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Lambert.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD
{

//----------------------------------------------------------------------------------------------------------------------
XLambert::XLambert(XModel* pmod) : XMaterial(pmod),
  m_Ambient("ambient_colour", XColour(0.2f,0.2f,0.2f)),
  m_Emission("incandescence", XColour(0.0f,0.0f,0.0f)),
  m_Transparency("transparency", XColour(0.0f,0.0f,0.0f)),
  m_BumpMap("bump_map"), m_EnvMap("env_map")
{
  m_Connections.push_back(&m_Ambient);
  m_Connections.push_back(&m_Emission);
  m_Connections.push_back(&m_Transparency);
  m_Connections.push_back(&m_BumpMap);
}

//----------------------------------------------------------------------------------------------------------------------
XLambert::~XLambert()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::PreDelete(XIdSet& extra_nodes)
{
  XMaterial::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XLambert::NodeDeath(XId id) 
{
  return XMaterial::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::SetAmbient(const XReal r,const XReal g,const XReal b,const XReal a) 
{
  m_Ambient.m_Colour.r = r;
  m_Ambient.m_Colour.g = g;
  m_Ambient.m_Colour.b = b;
  m_Ambient.m_Colour.a = a;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::SetAmbient(const XReal rgba[4])
{
  m_Ambient.m_Colour.r = rgba[0];
  m_Ambient.m_Colour.g = rgba[1];
  m_Ambient.m_Colour.b = rgba[2];
  m_Ambient.m_Colour.a = rgba[3];
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::SetAmbient(const XColour& rgba)
{
  m_Ambient.m_Colour = rgba;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLambert::SetAmbient(const XShadingNode* node)
{
  XMD_ASSERT(node);
  if(node)
  {
    if(m_pModel == node->GetModel())
    {
      m_Ambient.m_InputNode = node->GetID();
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::GetAmbient(XReal &r,XReal &g,XReal &b) const
{
  r = m_Ambient.m_Colour.r;
  g = m_Ambient.m_Colour.g;
  b = m_Ambient.m_Colour.b;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::GetAmbient(XReal &r,XReal &g,XReal &b,XReal &a) const 
{
  r = m_Ambient.m_Colour.r;
  g = m_Ambient.m_Colour.g;
  b = m_Ambient.m_Colour.b;
  a = m_Ambient.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::GetAmbient(XColour& c) const
{
  c = m_Ambient.m_Colour; 
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::GetAmbient(XReal rgba[4]) const
{
  rgba[0] = m_Ambient.m_Colour.r;
  rgba[1] = m_Ambient.m_Colour.g;
  rgba[2] = m_Ambient.m_Colour.b;
  rgba[3] = m_Ambient.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode* XLambert::GetAmbient()
{
  XBase* base = GetModel()->FindNode(m_Ambient.m_InputNode);
  if(base)
  {
    return base->HasFn<XShadingNode>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::SetEmission(const XReal r,const XReal g,const XReal b,const XReal a)
{
  m_Emission.m_Colour.r = r;
  m_Emission.m_Colour.g = g;
  m_Emission.m_Colour.b = b;
  m_Emission.m_Colour.a = a;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::SetEmission(const XReal rgba[4]) 
{
  m_Emission.m_Colour.r = rgba[0];
  m_Emission.m_Colour.g = rgba[1];
  m_Emission.m_Colour.b = rgba[2];
  m_Emission.m_Colour.a = rgba[3];
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::SetEmission(const XColour& rgba) 
{
  m_Emission.m_Colour = rgba;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLambert::SetEmission(const XShadingNode* node)
{
  XMD_ASSERT(node);
  if(node)
  {
    if(m_pModel == node->GetModel())
    {
      m_Emission.m_InputNode = node->GetID();
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::GetEmission(XReal &r,XReal &g,XReal &b) const
{
  r = m_Emission.m_Colour.r;
  g = m_Emission.m_Colour.g;
  b = m_Emission.m_Colour.b;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::GetEmission(XReal &r,XReal &g,XReal &b,XReal &a) const
{
  r = m_Emission.m_Colour.r;
  g = m_Emission.m_Colour.g;
  b = m_Emission.m_Colour.b;
  a = m_Emission.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::GetEmission(XColour& c) const
{ 
  c = m_Emission.m_Colour; 
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::GetEmission(XReal rgba[4]) const
{
  rgba[0] = m_Emission.m_Colour.r;
  rgba[1] = m_Emission.m_Colour.g;
  rgba[2] = m_Emission.m_Colour.b;
  rgba[3] = m_Emission.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode* XLambert::GetEmission()
{
  XBase* base = GetModel()->FindNode(m_Emission.m_InputNode);
  if(base)
  {
    return base->HasFn<XShadingNode>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::SetTransparency(const XReal r,const XReal g,const XReal b,const XReal a)
{
  m_Transparency.m_Colour.r = r;
  m_Transparency.m_Colour.g = g;
  m_Transparency.m_Colour.b = b;
  m_Transparency.m_Colour.a = a;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::SetTransparency(const XReal rgba[4])
{
  m_Transparency.m_Colour.r = rgba[0];
  m_Transparency.m_Colour.g = rgba[1];
  m_Transparency.m_Colour.b = rgba[2];
  m_Transparency.m_Colour.a = rgba[3];
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::SetTransparency(const XColour& rgba) 
{
  m_Transparency.m_Colour = rgba;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLambert::SetTransparency(const XShadingNode* node)
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
void XLambert::GetTransparency(XReal &r,XReal &g,XReal &b) const
{
  r = m_Transparency.m_Colour.r;
  g = m_Transparency.m_Colour.g;
  b = m_Transparency.m_Colour.b;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::GetTransparency(XReal &r,XReal &g,XReal &b,XReal &a) const
{
  r = m_Transparency.m_Colour.r;
  g = m_Transparency.m_Colour.g;
  b = m_Transparency.m_Colour.b;
  a = m_Transparency.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::GetTransparency(XColour& c) const
{
  c = m_Transparency.m_Colour;
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::GetTransparency(XReal rgba[4]) const
{
  rgba[0] = m_Transparency.m_Colour.r;
  rgba[1] = m_Transparency.m_Colour.g;
  rgba[2] = m_Transparency.m_Colour.b;
  rgba[3] = m_Transparency.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode* XLambert::GetTransparency()
{
  XBase* base = GetModel()->FindNode(m_Transparency.m_InputNode);
  if(base)
  {
    return base->HasFn<XShadingNode>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBumpMap* XLambert::CreateBumpMap() 
{
  XBumpMap* bump = GetModel()->CreateNode(XFn::BumpMap)->HasFn<XBumpMap>();
  SetBumpMap(bump);
  return bump;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLambert::SetBumpMap(const XBumpMap* bump_map)
{
  if(!bump_map)
  {
    m_BumpMap.m_InputNode = 0;
    return true;
  }

  // if you assert here, you have tried to assign an invalid bump 
  // map node to the material (i.e. It does not belong to this
  // XModel). 
  XMD_ASSERT( IsValidObj(bump_map) );
  if (IsValidObj(bump_map))
  {
    m_BumpMap.m_InputNode = bump_map->GetID();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XBumpMap* XLambert::GetBumpMap() const
{
  XBase* p = m_pModel->FindNode(m_BumpMap.m_InputNode);
  if(!p)
    return 0;
  return p->HasFn<XBumpMap>();
}

//----------------------------------------------------------------------------------------------------------------------
XCubeMap* XLambert::CreateCubeMap()
{
  XCubeMap* cube_map = GetModel()->CreateNode(XFn::CubeMap)->HasFn<XCubeMap>();
  SetEnvMap(cube_map);
  return cube_map;
}

//----------------------------------------------------------------------------------------------------------------------
XSphereMap* XLambert::CreateSphereMap()
{
  XSphereMap* sphere_map = GetModel()->CreateNode(XFn::SphereMap)->HasFn<XSphereMap>();
  SetEnvMap(sphere_map);
  return sphere_map;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLambert::SetEnvMap(const XEnvmapBaseTexture* env_map)
{
  if(!env_map)
  {
    m_EnvMap.m_InputNode = 0;
    return true;
  }

  // if you assert here, you have tried to assign an invalid texture 
  // placement node to the material (i.e. It does not belong to this
  // XModel). 
  XMD_ASSERT( IsValidObj(env_map) );
  if(IsValidObj(env_map))
  {
    m_EnvMap.m_InputNode = env_map->GetID();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XEnvmapBaseTexture* XLambert::GetEnvMap() const
{
  XBase* p = m_pModel->FindNode(m_EnvMap.m_InputNode);
  if(!p)
    return 0;
  return p->HasFn<XEnvmapBaseTexture>();
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XLambert::GetApiType() const
{
  return XFn::Lambert;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XLambert::GetFn(XFn::Type type)
{
  if(XFn::Lambert==type)
    return (XLambert*)this;
  return XMaterial::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XLambert::GetFn(XFn::Type type) const
{
  if(XFn::Lambert==type)
    return (const XLambert*)this;
  return XMaterial::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XLambert::DoCopy(const XBase* rhs)
{
  const XLambert* cb = rhs->HasFn<XLambert>();
  XMD_ASSERT(cb);
  if(cb)
  {
    m_Ambient = cb->m_Ambient;
    m_Emission = cb->m_Emission;
    m_Transparency = cb->m_Transparency;
    m_BumpMap = cb->m_BumpMap;
    m_EnvMap = cb->m_EnvMap;

    XMaterial::DoCopy(cb);
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XLambert::GetDataSize() const
{
  XU32 size = XMaterial::GetDataSize();
  size += m_Ambient.GetDataSize();
  size += m_Emission.GetDataSize();
  size += m_Transparency.GetDataSize();
  size += sizeof(XId) * 2;
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLambert::DoData(XFileIO& io) 
{
  DUMPER(XLambert);

  if(io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XMaterial::DoData(io) );
    IO_CHECK( m_Ambient.DoData(io) );
    IO_CHECK( m_Emission.DoData(io) );
    IO_CHECK( m_Transparency.DoData(io) );

    IO_CHECK( io.DoData(&m_BumpMap.m_InputNode) );
    DPARAM( m_BumpMap.m_InputNode );
    IO_CHECK( io.DoData(&m_EnvMap.m_InputNode) );
    DPARAM( m_EnvMap.m_InputNode );
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

    // legacy, lambert has no specular
    XColourConnection specular("specular");
    IO_CHECK( specular.DoData(io) );
    IO_CHECK( specular.m_Name == "specular_color" );

    IO_CHECK( m_Emission.DoData(io) );
    IO_CHECK( m_Transparency.DoData(io) );

    // legacy, lambert has no shininess
    XReal shininess = 0;
    IO_CHECK( io.DoData(&shininess) );
    DPARAM( shininess );

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
bool XLambert::ReadChunk(std::istream& ifs)
{
  DUMPER(XLambert__ReadChunk);
  if(GetModel()->GetInputVersion()>=4)
  {
    XMaterial::ReadChunk(ifs);
    IO_CHECK(m_Ambient.ReadChunk(ifs));
    IO_CHECK(m_Emission.ReadChunk(ifs));
    IO_CHECK(m_Transparency.ReadChunk(ifs));

    READ_CHECK(m_BumpMap.m_Name.c_str(), ifs);
    ifs >> m_BumpMap.m_InputNode;
    READ_CHECK(m_EnvMap.m_Name.c_str(), ifs);
    ifs >> m_EnvMap.m_InputNode;
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

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XLambert::WriteChunk(std::ostream& ofs)
{
  DUMPER(XLambert__WriteChunk);

  IO_CHECK(XMaterial::WriteChunk(ofs));
  IO_CHECK(m_Ambient.WriteChunk(ofs));
  IO_CHECK(m_Emission.WriteChunk(ofs));
  IO_CHECK(m_Transparency.WriteChunk(ofs));

  ofs << "\t" << m_BumpMap.m_Name << " " << m_BumpMap.m_InputNode << "\n";
  ofs << "\t" << m_EnvMap.m_Name << " " << m_EnvMap.m_InputNode << "\n";

  return ofs.good();
}
}
