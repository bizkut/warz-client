//----------------------------------------------------------------------------------------------------------------------
/// \file Material.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Material.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XMaterial::XMaterial(XModel* pmod) : XShadingNode(pmod),
  m_HwShader(0),m_Diffuse("colour",XColour(0.65f,0.65f,0.65f))
{
  m_Connections.push_back(&m_Diffuse);
}

//----------------------------------------------------------------------------------------------------------------------
XMaterial::~XMaterial()
{
}

//----------------------------------------------------------------------------------------------------------------------
XHwShaderNode* XMaterial::GetHwShader() const
{
  XBase* p = m_pModel->FindNode(m_HwShader);
  if(p)
    return p->HasFn<XHwShaderNode>();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMaterial::SetHwShader(const XBase* hw_shader)
{
  if (hw_shader && hw_shader->HasFn<XHwShaderNode>())
  {
    m_HwShader = hw_shader->GetID();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XMaterial::SetDiffuse(const XReal r,const XReal g,const XReal b,const XReal a) 
{
  m_Diffuse.m_Colour.r = r;
  m_Diffuse.m_Colour.g = g;
  m_Diffuse.m_Colour.b = b;
  m_Diffuse.m_Colour.a = a;
}

//----------------------------------------------------------------------------------------------------------------------
void XMaterial::SetDiffuse(const XReal rgba[4]) 
{
  m_Diffuse.m_Colour.r = rgba[0];
  m_Diffuse.m_Colour.g = rgba[1];
  m_Diffuse.m_Colour.b = rgba[2];
  m_Diffuse.m_Colour.a = rgba[3];
}

//----------------------------------------------------------------------------------------------------------------------
void XMaterial::SetDiffuse(const XColour& rgba) 
{
  m_Diffuse.m_Colour = rgba;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMaterial::SetDiffuse(const XShadingNode* node)
{
  XMD_ASSERT(node);
  if(node)
  {
    if(m_pModel == node->GetModel())
    {
      m_Diffuse.m_InputNode = node->GetID();
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XMaterial::GetDiffuse(XReal &r,XReal &g,XReal &b,XReal &a) const
{
  r = m_Diffuse.m_Colour.r;
  g = m_Diffuse.m_Colour.g;
  b = m_Diffuse.m_Colour.b;
  a = m_Diffuse.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
void XMaterial::GetDiffuse(XColour& c) const
{
  c = m_Diffuse.m_Colour; 
}

//----------------------------------------------------------------------------------------------------------------------
void XMaterial::GetDiffuse(XReal& r,XReal& g,XReal& b) const
{
  r = m_Diffuse.m_Colour.r;
  g = m_Diffuse.m_Colour.g;
  b = m_Diffuse.m_Colour.b;
}

//----------------------------------------------------------------------------------------------------------------------
void XMaterial::GetDiffuse(XReal rgba[4]) const
{
  rgba[0] = m_Diffuse.m_Colour.r;
  rgba[1] = m_Diffuse.m_Colour.g;
  rgba[2] = m_Diffuse.m_Colour.b;
  rgba[3] = m_Diffuse.m_Colour.a;
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode* XMaterial::GetDiffuse()
{
  XBase* base = GetModel()->FindNode(m_Diffuse.m_InputNode);
  if(base)
  {
    return base->HasFn<XShadingNode>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XShadingNode* XMaterial::GetDiffuse() const
{
  const XBase* base = GetModel()->FindNode(m_Diffuse.m_InputNode);
  if(base)
  {
    return base->HasFn<XShadingNode>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XMaterial::GetApiType() const 
{
  return XFn::Material;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XMaterial::GetFn(XFn::Type type)
{
  if (type == XFn::Material)
    return (XMaterial*)this;
  return XShadingNode::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XMaterial::GetFn(XFn::Type type) const
{
  if(XFn::Material==type)
    return (const XMaterial*)this;
  return XShadingNode::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XMaterial::DoCopy(const XBase* rhs)
{
  const XMaterial* cb = rhs->HasFn<XMaterial>();
  XMD_ASSERT(cb);

  if(cb)
  {
    m_HwShader = cb->m_HwShader;
    m_Diffuse = cb->m_Diffuse;

    XShadingNode::DoCopy(cb);
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMaterial::GetDataSize() const
{
  XU32 size = XShadingNode::GetDataSize();
  size += sizeof(XId);
  size += m_Diffuse.GetDataSize();
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMaterial::DoData(XFileIO& io) 
{
  DUMPER(XMaterial);

  if(io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XShadingNode::DoData(io) );

    IO_CHECK( io.DoData(&m_HwShader) );
    DPARAM( m_HwShader );
    IO_CHECK( m_Diffuse.DoData(io) );
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMaterial::ReadChunk(std::istream& ifs)
{
  DUMPER(XMaterial__ReadChunk);
  if(GetModel()->GetInputVersion()>=4)
  {
    XShadingNode::ReadChunk(ifs);

    READ_CHECK("hw_shader",ifs);
    ifs >> m_HwShader;
    IO_CHECK(m_Diffuse.ReadChunk(ifs));
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMaterial::NodeDeath(XId id)
{
  return XShadingNode::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XMaterial::PreDelete(XIdSet& extra_nodes)
{
  XShadingNode::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XMaterial::WriteChunk(std::ostream& ofs)
{
  DUMPER(XMaterial__WriteChunk);

  XShadingNode::WriteChunk(ofs);

  ofs << "\thw_shader " << m_HwShader << std::endl;
  IO_CHECK(m_Diffuse.WriteChunk(ofs));

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMaterial::GetObjectInstances(XInstanceList& _list) const
{
  XInstanceList _temp;
  m_pModel->GetObjectInstances(_temp);
  XInstanceList::iterator it = _temp.begin();
  for (; it != _temp.end();++it) 
  {
    XInstance* inst = *it;
    if (inst) 
    {
      if(inst->GetNumMaterialGroups())
      {
        for (XU32 i=0;i!=inst->GetNumMaterialGroups();++i) 
        {
          XMaterial* m = inst->GetMaterial(i);
          if (m) 
          {
            if (m==this)
              _list.push_back( inst );
          }
        }
      }
      else
      {
        XMaterial* m = inst->GetMaterial();
        if (m) 
        {
          if (m==this)
            _list.push_back( inst );
        }
      }
    }
  }
  return true;
}
}
