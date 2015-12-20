//----------------------------------------------------------------------------------------------------------------------
/// \file Anisotropic.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Anisotropic.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XAnisotropic::XAnisotropic(XModel* pmod) 
  : XSpecular(pmod),m_TangentUCamera(),m_TangentVCamera(),m_CorrelationX(0),m_CorrelationY(0),
    m_Roughness(0),m_RotateAngle(0),m_RefractiveIndex(0),m_AnisotropicReflectivity(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
XAnisotropic::~XAnisotropic()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XAnisotropic::PreDelete(XIdSet& extra_nodes)
{
  XSpecular::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnisotropic::NodeDeath(XId id) 
{
  return XSpecular::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
const XColour& XAnisotropic::GetTangentUCamera() const
{
  return m_TangentUCamera;
}

//----------------------------------------------------------------------------------------------------------------------
const XColour& XAnisotropic::GetTangentVCamera() const
{
  return m_TangentVCamera;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnisotropic::GetCorrelationX() const
{
  return m_CorrelationX;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnisotropic::GetCorrelationY() const
{
  return m_CorrelationY;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnisotropic::GetRoughness() const
{
  return m_Roughness;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnisotropic::GetRotateAngle() const
{
  return m_RotateAngle;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnisotropic::GetRefractiveIndex() const
{
  return m_RefractiveIndex;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnisotropic::GetAnisotropicReflectivity() const
{
  return m_AnisotropicReflectivity;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnisotropic::SetTangentUCamera(const XColour& value) 
{
  m_TangentUCamera = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnisotropic::SetTangentVCamera(const XColour& value) 
{
  m_TangentVCamera = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnisotropic::SetCorrelationX(const XReal value) 
{
  m_CorrelationX = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnisotropic::SetCorrelationY(const XReal value) 
{
  m_CorrelationY = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnisotropic::SetRoughness(const XReal value) 
{
  m_Roughness = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnisotropic::SetRotateAngle(const XReal value) 
{
  m_RotateAngle = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnisotropic::SetRefractiveIndex(const XReal value) 
{
  m_RefractiveIndex = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnisotropic::SetAnisotropicReflectivity(const bool value) 
{
  m_AnisotropicReflectivity = value;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XAnisotropic::GetApiType() const
{
  return XFn::Anisotropic;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAnisotropic::GetFn(XFn::Type type)
{
  if(XFn::Anisotropic==type)
    return (XAnisotropic*)this;
  return XSpecular::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XAnisotropic::GetFn(XFn::Type type) const
{
  if(XFn::Anisotropic==type)
    return (const XAnisotropic*)this;
  return XSpecular::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XAnisotropic::DoCopy(const XBase* rhs)
{
  const XAnisotropic* cb = rhs->HasFn<XAnisotropic>();
  XMD_ASSERT(cb);

  m_TangentUCamera = cb->m_TangentUCamera;
  m_TangentVCamera = cb->m_TangentVCamera;
  m_CorrelationX = cb->m_CorrelationX;
  m_CorrelationY = cb->m_CorrelationY;
  m_Roughness = cb->m_Roughness;
  m_RotateAngle = cb->m_RotateAngle;
  m_RefractiveIndex = cb->m_RefractiveIndex;
  m_AnisotropicReflectivity = cb->m_AnisotropicReflectivity;

  XMaterial::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnisotropic::GetDataSize() const 
{
  return XSpecular::GetDataSize() + 2*sizeof(XColour) + 5*sizeof(XReal) + sizeof(bool);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnisotropic::DoData(XFileIO& io) 
{
  DUMPER(XAnisotropic);

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
  IO_CHECK( io.DoData(&m_TangentUCamera) );
  DPARAM( m_TangentUCamera );

  IO_CHECK( io.DoData(&m_TangentVCamera) );
  DPARAM( m_TangentVCamera );

  IO_CHECK( io.DoData(&m_CorrelationX) );
  DPARAM( m_CorrelationX );

  IO_CHECK( io.DoData(&m_CorrelationY) );
  DPARAM( m_CorrelationY );

  IO_CHECK( io.DoData(&m_Roughness) );
  DPARAM( m_Roughness );

  IO_CHECK( io.DoData(&m_RotateAngle) );
  DPARAM( m_RotateAngle );

  IO_CHECK( io.DoData(&m_RefractiveIndex) );
  DPARAM( m_RefractiveIndex );

  IO_CHECK( io.DoData(&m_AnisotropicReflectivity) );
  DPARAM( m_AnisotropicReflectivity );
 
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnisotropic::ReadChunk(std::istream& ifs)
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

  READ_CHECK("tangentu",ifs);
  ifs >> m_TangentUCamera.r >> m_TangentUCamera.g >> m_TangentUCamera.b >> m_TangentUCamera.a;
  READ_CHECK("tangentv",ifs);
  ifs >> m_TangentVCamera.r >> m_TangentVCamera.g >> m_TangentVCamera.b >> m_TangentVCamera.a;
  READ_CHECK("correlation_x",ifs);
  ifs >> m_CorrelationX;
  READ_CHECK("correlation_y",ifs);
  ifs >> m_CorrelationY;

  READ_CHECK("roughness",ifs);
  ifs >> m_Roughness;
  READ_CHECK("rotate_angle",ifs);
  ifs >> m_RotateAngle;
  READ_CHECK("refractive_index",ifs);
  ifs >> m_RefractiveIndex;
  READ_CHECK("anisotropic_reflectivity",ifs);
  ifs >> m_AnisotropicReflectivity;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnisotropic::WriteChunk(std::ostream& ofs)
{
  IO_CHECK( XSpecular::WriteChunk(ofs) );

  ofs << "\ttangentu "  << m_TangentUCamera << std::endl;
  ofs << "\ttangentv " << m_TangentVCamera << std::endl;

  ofs << "\tcorrelation_x " << m_CorrelationX << std::endl;
  ofs << "\tcorrelation_y " << m_CorrelationY << std::endl;

  ofs << "\troughness " << m_Roughness << std::endl;
  ofs << "\trotate_angle " << m_RotateAngle << std::endl;
  ofs << "\trefractive_index " << m_RefractiveIndex << std::endl;
  ofs << "\tanisotropic_reflectivity " << m_AnisotropicReflectivity << std::endl;

  return ofs.good();
}
}
