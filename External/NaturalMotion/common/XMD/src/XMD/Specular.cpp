//----------------------------------------------------------------------------------------------------------------------
/// \file Specular.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Specular.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XSpecular::XSpecular(XModel* pmod) : XLambert(pmod),
  m_Specular("specular_colour",XColour(0.0f,0.0f,0.0f))
{
  m_Connections.push_back(&m_Specular);
}

//----------------------------------------------------------------------------------------------------------------------
XSpecular::~XSpecular()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XSpecular::PreDelete(XIdSet& extra_nodes)
{
  XLambert::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XSpecular::NodeDeath(XId id) 
{
  return XLambert::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XSpecular::SetSpecular(const XReal r,const XReal g,const XReal b,const XReal a) 
{
  m_Specular.m_Colour.r = r;
  m_Specular.m_Colour.g = g;
  m_Specular.m_Colour.b = b;
  m_Specular.m_Colour.a = a;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpecular::SetSpecular(const XReal rgba[4]) 
{
  m_Specular.m_Colour.r = rgba[0];
  m_Specular.m_Colour.g = rgba[1];
  m_Specular.m_Colour.b = rgba[2];
  m_Specular.m_Colour.a = rgba[3];
}

//----------------------------------------------------------------------------------------------------------------------
void XSpecular::SetSpecular(const XColour& rgba) 
{
  m_Specular.m_Colour = rgba;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSpecular::SetSpecular(const XShadingNode* node)
{
  if(node)
  {
    XMD_ASSERT( IsValidObj(node) );
    if(IsValidObj(node))
    {
      m_Specular.m_InputNode = node->GetID();
      return true;
    }
  }
  else
  {
    m_Specular.m_InputNode = 0;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpecular::GetSpecular(XReal &r,XReal &g,XReal &b,XReal &a) const
{
  r = m_Specular.m_Colour.r;
  g = m_Specular.m_Colour.g;
  b = m_Specular.m_Colour.b;
  a = m_Specular.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpecular::GetSpecular(XColour& c) const
{
  c = m_Specular.m_Colour; 
}

//----------------------------------------------------------------------------------------------------------------------
void XSpecular::GetSpecular( XReal &r,
  XReal &g,
  XReal &b) const
{
  r = m_Specular.m_Colour.r;
  g = m_Specular.m_Colour.g;
  b = m_Specular.m_Colour.b;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpecular::GetSpecular(XReal rgba[4]) const
{
  rgba[0] = m_Specular.m_Colour.r;
  rgba[1] = m_Specular.m_Colour.g;
  rgba[2] = m_Specular.m_Colour.b;
  rgba[3] = m_Specular.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode* XSpecular::GetSpecular()
{
  XBase* base = GetModel()->FindNode(m_Specular.m_InputNode);
  if(base)
  {
    return base->HasFn<XShadingNode>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XSpecular::SetShininess(const XReal shine)
{
  m_Shininess = shine;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XSpecular::GetShininess() const
{
  return m_Shininess;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XSpecular::GetApiType() const 
{
  return XFn::Specular;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XSpecular::GetFn(XFn::Type type)
{
  if (type == XFn::Specular)
    return (XSpecular*)this;
  return XLambert::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XSpecular::GetFn(XFn::Type type) const
{
  if(XFn::Specular==type)
    return (const XSpecular*)this;
  return XLambert::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XSpecular::DoCopy(const XBase* rhs)
{
  const XSpecular* cb = rhs->HasFn<XSpecular>();
  XMD_ASSERT(cb);

  if(cb)
  {
    m_Specular = cb->m_Specular;

    XLambert::DoCopy(cb);
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSpecular::GetDataSize() const
{
  XU32 size = XLambert::GetDataSize();
  size += m_Specular.GetDataSize();
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSpecular::DoData(XFileIO& io) 
{
  DUMPER(XSpecular);

  if(io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XLambert::DoData(io) );

    IO_CHECK( m_Specular.DoData(io) );
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSpecular::ReadChunk(std::istream& ifs)
{
  DUMPER(XSpecular__ReadChunk);
  if(GetModel()->GetInputVersion()>=4)
  {
    XLambert::ReadChunk(ifs);
    IO_CHECK(m_Specular.ReadChunk(ifs));
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSpecular::WriteChunk(std::ostream& ofs)
{
  DUMPER(XSpecular__WriteChunk);

  IO_CHECK(XLambert::WriteChunk(ofs));
  IO_CHECK(m_Specular.WriteChunk(ofs));

  return true;
}
}
