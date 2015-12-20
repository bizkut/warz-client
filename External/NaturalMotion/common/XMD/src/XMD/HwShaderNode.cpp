//----------------------------------------------------------------------------------------------------------------------
/// \file HwShaderNode.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Model.h"
#include "XMD/HwShaderNode.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XHwShaderNode::XHwShaderNode(XModel* M) : XBase(M)
{
}

//----------------------------------------------------------------------------------------------------------------------
XHwShaderNode::~XHwShaderNode()
{
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* g_AmbientNames[] = {
  "ambient",
  "ambientColor",
  "ambientColour",
  "ambient_color",
  "ambient_colour",
  0
};

//----------------------------------------------------------------------------------------------------------------------
XAttribute* XHwShaderNode::GetAmbientAttribute() 
{
  XU32 i=0;
  while(g_AmbientNames[i])
  {
    XAttribute *pattr = GetAttribute(g_AmbientNames[i]);
    if(pattr)
      return pattr;
    ++i;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* g_DiffuseNames[] = {
  "color",
  "colour",
  "diffuse",
  "diffuseColor",
  "diffuseColour",
  "diffuse_color",
  "diffuse_colour",
  0
};

//----------------------------------------------------------------------------------------------------------------------
XAttribute* XHwShaderNode::GetDiffuseAttribute()
{
  XU32 i=0;
  while(g_DiffuseNames[i])
  {
    XAttribute *pattr = GetAttribute(g_DiffuseNames[i]);
    if(pattr)
      return pattr;
    ++i;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* g_SpecularNames[] = {
  "specular",
  "specularColor",
  "specularColour",
  "specular_color",
  "specular_colour",
  0
};

//----------------------------------------------------------------------------------------------------------------------
XAttribute* XHwShaderNode::GetSpecularAttribute()
{
  XU32 i=0;
  while(g_SpecularNames[i]) 
  {
    XAttribute *pattr = GetAttribute(g_SpecularNames[i]);
    if(pattr)
      return pattr;
    ++i;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* g_EmissionNames[] = {
  "emission",
  "emissionColor",
  "emissionColour",
  "emission_color",
  "emission_colour",
  "incandescence",
  0
};

//----------------------------------------------------------------------------------------------------------------------
XAttribute* XHwShaderNode::GetEmissionAttribute() 
{
  XU32 i=0;
  while(g_EmissionNames[i])
  {
    XAttribute *pattr = GetAttribute(g_EmissionNames[i]);
    if(pattr)
      return pattr;
    ++i;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* g_TransparencyNames[] = {
  "transparency",
  "transparencyColor",
  "transparencyColour",
  "transparency_color",
  "transparency_colour",
  0
};

//----------------------------------------------------------------------------------------------------------------------
XAttribute* XHwShaderNode::GetTransparencyAttribute() 
{
  XU32 i=0;
  while(g_TransparencyNames[i]) 
  {
    XAttribute *pattr = GetAttribute(g_TransparencyNames[i]);
    if(pattr)
      return pattr;
    ++i;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* g_ShininessNames[] = {
  "shine",
  "shininess",
  "power",
  "cosinePower",
  "cosPower",
  0
};

//----------------------------------------------------------------------------------------------------------------------
XAttribute* XHwShaderNode::GetShininessAttribute() 
{
  XU32 i=0;
  while(g_ShininessNames[i]) 
  {
    XAttribute *pattr = GetAttribute(g_ShininessNames[i]);
    if(pattr)
      return pattr;
    ++i;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* g_BumpNames[] = {
  "bump",
  "bumpMap",
  "bump_map",
  "normal_camera",
  "normalCamera",
  0
};

//----------------------------------------------------------------------------------------------------------------------
XAttribute* XHwShaderNode::GetBumpAttribute()
{
  XU32 i=0;
  while(g_BumpNames[i]) 
  {
    XAttribute *pattr = GetAttribute(g_BumpNames[i]);
    if(pattr)
      return pattr;
    ++i;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* g_EnvMapNames[] = {
  "cubeMap",
  "cube_map",
  "environment",
  "envMap",
  "env_map",
  "environment_map",
  "reflectedColor",
  "reflectedColour",
  "reflected_color",
  "reflected_colour",
  "sphereMap",
  "sphere_map",
  0
};

//----------------------------------------------------------------------------------------------------------------------
XAttribute* XHwShaderNode::GetEnvMapAttribute() 
{
  XU32 i=0;
  while(g_EnvMapNames[i])
  {
    XAttribute *pattr = GetAttribute(g_EnvMapNames[i]);
    if(pattr)
      return pattr;
    ++i;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XHwShaderNode::GetApiType() const
{
  return XFn::HwShader;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XHwShaderNode::GetDataSize() const 
{
  return XBase::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XHwShaderNode::GetFn(XFn::Type type)
{
  if (type == XFn::HwShader)
    return (XHwShaderNode*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XHwShaderNode::GetFn(XFn::Type type) const
{
  if(XFn::HwShader==type)
    return (const XHwShaderNode*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::SetAmbient(const XReal r,const XReal g,const XReal b,const XReal a)
{
  XAttribute* pambient = GetAmbientAttribute();
  if(pambient) 
  {
    pambient->Set( XColour(r,g,b,a) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::SetAmbient(const XReal rgba[4]) 
{
  XAttribute* pambient = GetAmbientAttribute();
  if(pambient)
  {
    pambient->Set( XColour(rgba[0],rgba[1],rgba[2],rgba[3]) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetAmbient(XReal& r,XReal& g,XReal& b,XReal& a) 
{
  XAttribute* pambient = GetAmbientAttribute();
  if(pambient)
  {
    XColour col;
    pambient->Get( col );
    r=col.r;
    g=col.g;
    b=col.b;
    a=col.a;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetAmbient(XReal& r,XReal& g,XReal& b)
{
  XAttribute* pambient = GetAmbientAttribute();
  if(pambient)
  {
    XColour col;
    pambient->Get( col );
    r=col.r;
    g=col.g;
    b=col.b;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetAmbient(XReal rgba[4])
{
  XAttribute* pambient = GetAmbientAttribute();
  if(pambient) 
  {
    XColour col;
    pambient->Get( col );
    rgba[0]=col.r;
    rgba[1]=col.g;
    rgba[2]=col.b;
    rgba[3]=col.a;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::SetDiffuse(const XReal r,const XReal g,const XReal b,const XReal a)
{
  XAttribute* pdiffuse = GetDiffuseAttribute();
  if(pdiffuse) 
  {
    pdiffuse->Set( XColour(r,g,b,a) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::SetDiffuse(const XReal rgba[4])
{
  XAttribute* pdiffuse = GetDiffuseAttribute();
  if(pdiffuse) 
  {
    pdiffuse->Set( XColour(rgba[0],rgba[1],rgba[2],rgba[3]) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetDiffuse(XReal& r,XReal& g,XReal& b,XReal& a)
{
  XAttribute* pdiffuse = GetDiffuseAttribute();
  if(pdiffuse) 
  {
    XColour col;
    pdiffuse->Get( col );
    r=col.r;
    g=col.g;
    b=col.b;
    a=col.a;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetDiffuse(XReal& r,XReal& g,XReal& b) 
{
  XAttribute* pdiffuse = GetDiffuseAttribute();
  if(pdiffuse) 
  {
    XColour col;
    pdiffuse->Get( col );
    r=col.r;
    g=col.g;
    b=col.b;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetDiffuse(XReal rgba[4]) 
{
  XAttribute* pdiffuse = GetDiffuseAttribute();
  if(pdiffuse)
  {
    XColour col;
    pdiffuse->Get( col );
    rgba[0]=col.r;
    rgba[1]=col.g;
    rgba[2]=col.b;
    rgba[3]=col.a;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::SetSpecular(const XReal r,const XReal g,const XReal b,const XReal a)
{
  XAttribute* pspecular = GetSpecularAttribute();
  if(pspecular)
  {
    pspecular->Set( XColour(r,g,b,a) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::SetSpecular(const XReal rgba[4])
{
  XAttribute* pspecular = GetSpecularAttribute();
  if(pspecular)
  {
    pspecular->Set( XColour(rgba[0],rgba[1],rgba[2],rgba[3]) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetSpecular(XReal& r,XReal& g,XReal& b,XReal& a)
{
  XAttribute* pspecular = GetSpecularAttribute();
  if(pspecular)
  {
    XColour col;
    pspecular->Get( col );
    r=col.r;
    g=col.g;
    b=col.b;
    a=col.a;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetSpecular(XReal& r,XReal& g,XReal& b) 
{
  XAttribute* pspecular = GetSpecularAttribute();
  if(pspecular) 
  {
    XColour col;
    pspecular->Get( col );
    r=col.r;
    g=col.g;
    b=col.b;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetSpecular(XReal rgba[4])
{
  XAttribute* pspecular = GetSpecularAttribute();
  if(pspecular)
  {
    XColour col;
    pspecular->Get( col );
    rgba[0]=col.r;
    rgba[1]=col.g;
    rgba[2]=col.b;
    rgba[3]=col.a;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::SetEmission(const XReal r,const XReal g,const XReal b,const XReal a)
{
  XAttribute* pemission = GetEmissionAttribute();
  if(pemission) 
  {
    pemission->Set( XColour(r,g,b,a) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::SetEmission(const XReal rgba[4])
{
  XAttribute* pemission = GetEmissionAttribute();
  if(pemission)
  {
    pemission->Set( XColour(rgba[0],rgba[1],rgba[2],rgba[3]) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetEmission(XReal& r,XReal& g,XReal& b,XReal& a) 
{
  XAttribute* pemission = GetEmissionAttribute();
  if(pemission)
  {
    XColour col;
    pemission->Get( col );
    r=col.r;
    g=col.g;
    b=col.b;
    a=col.a;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetEmission(XReal& r,XReal& g,XReal& b)
{
  XAttribute* pemission = GetEmissionAttribute();
  if(pemission)
  {
    XColour col;
    pemission->Get( col );
    r=col.r;
    g=col.g;
    b=col.b;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetEmission(XReal rgba[4])
{
  XAttribute* pemission = GetEmissionAttribute();
  if(pemission)
  {
    XColour col;
    pemission->Get( col );
    rgba[0]=col.r;
    rgba[1]=col.g;
    rgba[2]=col.b;
    rgba[3]=col.a;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::SetTransparency(const XReal r,const XReal g,const XReal b,const XReal a)
{
  XAttribute* ptransparency = GetTransparencyAttribute();
  if(ptransparency)
  {
    ptransparency->Set( XColour(r,g,b,a) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::SetTransparency(const XReal rgba[4])
{
  XAttribute* ptransparency = GetTransparencyAttribute();
  if(ptransparency)
  {
    ptransparency->Set( XColour(rgba[0],rgba[1],rgba[2],rgba[3]) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetTransparency(XReal& r,XReal& g,XReal& b,XReal& a) 
{
  XAttribute* ptransparency = GetTransparencyAttribute();
  if(ptransparency)
  {
    XColour col;
    ptransparency->Get( col );
    r=col.r;
    g=col.g;
    b=col.b;
    a=col.a;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetTransparency(XReal& r,XReal& g,XReal& b)
{
  XAttribute* ptransparency = GetTransparencyAttribute();
  if(ptransparency)
  {
    XColour col;
    ptransparency->Get( col );
    r=col.r;
    g=col.g;
    b=col.b;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::GetTransparency(XReal rgba[4])
{
  XAttribute* ptransparency = GetTransparencyAttribute();
  if(ptransparency)
  {
    XColour col;
    ptransparency->Get( col );
    rgba[0]=col.r;
    rgba[1]=col.g;
    rgba[2]=col.b;
    rgba[3]=col.a;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::SetShininess(const XReal shine) 
{
  XAttribute* pshine = GetShininessAttribute();
  if(pshine)
    pshine->Set(shine);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XHwShaderNode::GetShininess() 
{
  XReal temp=0.0f;
  XAttribute* pshine = GetShininessAttribute();
  if(pshine)
    pshine->Get(temp);
  return temp;
}

//----------------------------------------------------------------------------------------------------------------------
XEnvmapBaseTexture* XHwShaderNode::GetEnvMap()
{
  XAttribute* pattr = GetEnvMapAttribute();
  XBase* pb = 0;
  if(pattr)
  {
    switch(pattr->Type()) 
    {
    case XFn::IntAttribute :
      {
        XS32 id;
        pattr->Get(id);
        pb = m_pModel->FindNode(id);
      }
      break;

    case XFn::StringAttribute :
      {
        XString id;
        pattr->Get(id);
        pb = m_pModel->FindNode(id);
      }
      break;

    default:
      break;
    }
  }
  if(pb) 
  {
    return pb->HasFn<XEnvmapBaseTexture>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBumpMap* XHwShaderNode::GetBumpMap() 
{
  XAttribute* pattr = GetBumpAttribute();
  XBase* pb = 0;
  if(pattr)
  {
    switch(pattr->Type())
    {
    case XFn::IntAttribute :
      {
        XS32 id;
        pattr->Get(id);
        pb = m_pModel->FindNode(id);
      }
      break;
    case XFn::StringAttribute :
      {
        XString id;
        pattr->Get(id);
        pb = m_pModel->FindNode(id);
      }
      break;

    default:
      break;
    }
  }
  if(pb)
  {
    return pb->HasFn<XBumpMap>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XHwShaderNode::WriteChunk(std::ostream& ofs) 
{
  if(!XBase::WriteChunk(ofs))
    return false;
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XHwShaderNode::DoData(XFileIO& io)
{
  DUMPER(XHwShaderNode);

  IO_CHECK( XBase::DoData(io) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::DoCopy(const XBase* rhs)
{
  const XHwShaderNode* cb = rhs->HasFn<XHwShaderNode>();
  XMD_ASSERT(cb);

  XBase::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XHwShaderNode::ReadChunk(std::istream& ifs)
{
  if(!XBase::ReadChunk(ifs))
    return false;
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XHwShaderNode::NodeDeath(XId id)
{
  return XBase::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XHwShaderNode::PreDelete(XIdSet& extra_nodes)
{
  XBase::PreDelete(extra_nodes);
}
}


