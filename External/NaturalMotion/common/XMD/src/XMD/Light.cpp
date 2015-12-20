//----------------------------------------------------------------------------------------------------------------------
/// \file Light.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Light.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XLight::XLight(XModel* pmod)
  : XShape(pmod),m_fIntensity(1.0f)
{
  m_Color[0]=m_Color[1]=m_Color[2]=0.9f;
  m_Color[3]=1.0f;

  m_ShadowColor[0]=m_ShadowColor[1]=m_ShadowColor[2]=0.0f;
  m_ShadowColor[3]=0.0f;

  m_bEmitAmbient = m_bEmitDiffuse = m_bEmitSpecular = false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLight::GetEmitAmbient() const 
{
  return m_bEmitAmbient;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLight::GetEmitDiffuse() const 
{
  return m_bEmitDiffuse;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLight::GetEmitSpecular() const 
{
  return m_bEmitSpecular;
}

//----------------------------------------------------------------------------------------------------------------------
void XLight::SetEmitAmbient(const bool v)
{
  m_bEmitAmbient = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XLight::SetEmitDiffuse(const bool v) 
{
  m_bEmitDiffuse = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XLight::SetEmitSpecular(const bool v) 
{
  m_bEmitSpecular = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XLight::GetShadowColour(XColour& value) const
{
  value.r = m_ShadowColor[0];
  value.g = m_ShadowColor[1];
  value.b = m_ShadowColor[2];
  value.a = m_ShadowColor[3];
}

//----------------------------------------------------------------------------------------------------------------------
void XLight::SetShadowColour(const XColour& value) 
{
  m_ShadowColor[0] = value.r;
  m_ShadowColor[1] = value.g;
  m_ShadowColor[2] = value.b;
  m_ShadowColor[3] = value.a;
}

//----------------------------------------------------------------------------------------------------------------------
XColour XLight::GetColour() const
{
  return XColour(m_Color[0],m_Color[1],m_Color[2],m_Color[3]);
}

//----------------------------------------------------------------------------------------------------------------------
XColour XLight::GetShadowColour() const
{
  return XColour(m_ShadowColor[0],m_ShadowColor[1],m_ShadowColor[2],m_ShadowColor[3]);
}

//---------------------------------------------------------------------------------------------------------------------- 
XLight::~XLight()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XLight::GetApiType() const
{
  return XFn::Light;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XLight::GetFn(XFn::Type type)
{
  if(XFn::Light==type)
    return (XLight*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XLight::GetFn(XFn::Type type) const
{
  if(XFn::Light==type)
    return (const XLight*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XLight::GetIntensity() const 
{
  return m_fIntensity;
}

//----------------------------------------------------------------------------------------------------------------------
void XLight::GetColour(XColour& c) const 
{
  c.r = m_Color[0];
  c.g = m_Color[1];
  c.b = m_Color[2];
  c.a = m_Color[3];
}

//----------------------------------------------------------------------------------------------------------------------
void XLight::SetIntensity(const XReal v) 
{
  m_fIntensity = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XLight::SetColour(const XColour& c) 
{
  m_Color[0] = c.r;
  m_Color[1] = c.g;
  m_Color[2] = c.b;
  m_Color[3] = c.a;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLight::WriteChunk(std::ostream& ofs) 
{
  ofs << "\tcolour\t\t"; 

  ofs << m_Color[0] <<" "
    << m_Color[1] <<" "
    << m_Color[2] <<"\n";

  ofs << "\tintensity\t";

  ofs << m_fIntensity << "\n";

  ofs << "\tshadow_colour\t"; 

  ofs << m_ShadowColor[0] <<" "
      << m_ShadowColor[1] <<" "
      << m_ShadowColor[2] <<"\n";

  ofs << "\temit_ambient " << m_bEmitAmbient << "\n";
  ofs << "\temit_diffuse " << m_bEmitDiffuse << "\n";
  ofs << "\temit_specular " << m_bEmitSpecular << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XLight::DoData(XFileIO& io) 
{
  DUMPER(XLight);

  IO_CHECK( XShape::DoData(io) );

  IO_CHECK( io.DoData(m_Color,3) );
  DPARAM( m_Color[0] );
  DPARAM( m_Color[1] );
  DPARAM( m_Color[2] );

  IO_CHECK( io.DoData(&m_fIntensity) );
  DPARAM( m_fIntensity );

  IO_CHECK( io.DoData(m_ShadowColor,3) );
  DPARAM( m_ShadowColor[0] );
  DPARAM( m_ShadowColor[1] );
  DPARAM( m_ShadowColor[2] );

  if(io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( io.DoData(&m_bEmitAmbient) );
    IO_CHECK( io.DoData(&m_bEmitDiffuse) );
    IO_CHECK( io.DoData(&m_bEmitSpecular) );
    DPARAM( m_bEmitAmbient );
    DPARAM( m_bEmitDiffuse );
    DPARAM( m_bEmitSpecular );
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XLight::DoCopy(const XBase* rhs)
{
  const XLight* cb = rhs->HasFn<XLight>();
  XMD_ASSERT(cb);

  memcpy(m_Color,cb->m_Color,sizeof(XReal)*4);
  m_fIntensity = cb->m_fIntensity;
  memcpy(m_ShadowColor,cb->m_ShadowColor,sizeof(XReal)*4);

  m_bEmitAmbient = cb->m_bEmitAmbient;
  m_bEmitDiffuse = cb->m_bEmitDiffuse;
  m_bEmitSpecular = cb->m_bEmitSpecular;

  XShape::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XLight::GetDataSize() const  
{
  return sizeof(XReal)*7 + sizeof(bool)*3 + XShape::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XLight::ReadChunk(std::istream& ifs)
{
  READ_CHECK("colour",ifs);
  ifs >> m_Color[0]
      >> m_Color[1]
      >> m_Color[2];

  READ_CHECK("intensity",ifs);
  ifs >> m_fIntensity;

  READ_CHECK("shadow_colour",ifs);
  ifs >> m_ShadowColor[0]
      >> m_ShadowColor[1]
      >> m_ShadowColor[2];
    
  if(GetModel()->GetInputVersion()>=4)
  {
    READ_CHECK("emit_ambient",ifs);
    ifs >> m_bEmitAmbient;

    READ_CHECK("emit_diffuse",ifs);
    ifs >> m_bEmitDiffuse;

    READ_CHECK("emit_specular",ifs);
    ifs >> m_bEmitSpecular;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XLight::NodeDeath(XId id)
{
  return XShape::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XLight::PreDelete(XIdSet& extra_nodes)
{
  XShape::PreDelete(extra_nodes);
}
}
