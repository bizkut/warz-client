//----------------------------------------------------------------------------------------------------------------------
/// \file JiggleDeformer.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/JiggleDeformer.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XJiggleDeformer::XJiggleDeformer(XModel* pmod) 
  : XDeformer(pmod),point_indices()
{
  force_along_normal=1.0f;
  force_on_tangent=1.0f;
  motion_multiplier=1.0f;
  stiffness=0.5f;
  damping=0.5f;
  jiggle_weight=1.0f;
  direction_bias=0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
XJiggleDeformer::~XJiggleDeformer(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
const XIndexList& XJiggleDeformer::GetPointIndices() const 
{ 
  return point_indices; 
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::SetPointIndices(const XIndexList& indices) 
{ 
  point_indices = indices; 
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XJiggleDeformer::GetApiType() const
{
  return XFn::JiggleDeformer;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XJiggleDeformer::GetFn(XFn::Type type)
{
  if(XFn::JiggleDeformer==type)
    return (XJiggleDeformer*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XJiggleDeformer::GetFn(XFn::Type type) const
{
  if(XFn::JiggleDeformer==type)
    return (const XJiggleDeformer*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XJiggleDeformer::NodeDeath(XId id)
{
  return XDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::PreDelete(XIdSet& extra_nodes)
{
  XDeformer::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XJiggleDeformer::ReadChunk(std::istream& ifs)
{  
  if (!XDeformer::ReadChunk(ifs)) 
    return false;

  READ_CHECK("enable",ifs);
  ifs >> enable;
  READ_CHECK("ignore_transform",ifs);
  ifs >> ignore_transform;
  READ_CHECK("force_along_normal",ifs);
  ifs >> force_along_normal;
  READ_CHECK("force_on_tangent",ifs);
  ifs >> force_on_tangent;
  READ_CHECK("motion_multiplier",ifs);
  ifs >> motion_multiplier;
  READ_CHECK("stiffness",ifs);
  ifs >> stiffness;
  READ_CHECK("damping",ifs);
  ifs >> damping;
  READ_CHECK("jiggle_weight",ifs);
  ifs >> jiggle_weight;
  READ_CHECK("direction_bias",ifs);
  ifs >> direction_bias;
  READ_CHECK("point_indices",ifs);
  XS32 n_points=0;
  ifs >> n_points;
  point_indices.resize(n_points);
  for (XS32 i=0;i!=n_points;++i) 
  {
    ifs >> point_indices[i];
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XJiggleDeformer::WriteChunk(std::ostream& ofs) 
{
  if (!m_AffectedGeometry.size())
  {
    return true;
  }

  if (!XDeformer::WriteChunk(ofs))
    return false;

  ofs << "\tenable " << enable << "\n";
  ofs << "\tignore_transform " << ignore_transform << "\n";
  ofs << "\tforce_along_normal " << force_along_normal << "\n";
  ofs << "\tforce_on_tangent " << force_on_tangent << "\n";
  ofs << "\tmotion_multiplier " << motion_multiplier << "\n";
  ofs << "\tstiffness " << stiffness << "\n";
  ofs << "\tdamping " << damping << "\n";
  ofs << "\tjiggle_weight " << jiggle_weight << "\n";
  ofs << "\tdirection_bias " << direction_bias << "\n";

  ofs << "\tpoint_indices " << static_cast<XU32>(point_indices.size());
  for (XU32 i=0;i!=point_indices.size();++i) 
  {
    ofs << " " << point_indices[i];
  }
  ofs << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XJiggleDeformer::DoData(XFileIO& io) 
{
  DUMPER(XJiggleDeformer);

  IO_CHECK( XDeformer::DoData(io) );

  IO_CHECK( io.DoData(&enable) );
  DPARAM( enable );

  IO_CHECK( io.DoData(&ignore_transform) );
  DPARAM( ignore_transform );

  IO_CHECK( io.DoData(&force_along_normal) );
  DPARAM( force_along_normal );

  IO_CHECK( io.DoData(&force_on_tangent) );
  DPARAM( force_on_tangent );

  IO_CHECK( io.DoData(&motion_multiplier) );
  DPARAM( motion_multiplier );

  IO_CHECK( io.DoData(&stiffness) );
  DPARAM( stiffness );

  IO_CHECK( io.DoData(&damping) );
  DPARAM( damping );

  IO_CHECK( io.DoData(&jiggle_weight) );
  DPARAM( jiggle_weight );

  IO_CHECK( io.DoData(&direction_bias) );
  DPARAM( direction_bias );

  IO_CHECK( io.DoDataVector(point_indices) );
  DAPARAM( point_indices );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::DoCopy(const XBase* rhs)
{
  const XJiggleDeformer* cb = rhs->HasFn<XJiggleDeformer>();
  XMD_ASSERT(cb);
  enable = cb->enable;
  ignore_transform = cb->ignore_transform;
  force_along_normal = cb->force_along_normal;
  force_on_tangent = cb->force_on_tangent;
  motion_multiplier = cb->motion_multiplier;
  stiffness = cb->stiffness;
  damping = cb->damping;
  jiggle_weight = cb->jiggle_weight;
  direction_bias = cb->direction_bias;
  point_indices = cb->point_indices;
  XDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XJiggleDeformer::GetDataSize() const
{
  return static_cast<XU32>( sizeof(XS32) + sizeof(bool) + sizeof(XU32) + sizeof(XU32)*point_indices.size() + 7*sizeof(XReal) + XDeformer::GetDataSize() );
}

//----------------------------------------------------------------------------------------------------------------------
bool XJiggleDeformer::GetEnable() const
{
  return enable != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XJiggleDeformer::GetIgnoreTransform() const
{
  return ignore_transform;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XJiggleDeformer::GetForceAlongNormal() const
{
  return force_along_normal;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XJiggleDeformer::GetForceOnTangent() const
{
  return force_on_tangent;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XJiggleDeformer::GetMotionMultiplier() const
{
  return motion_multiplier;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XJiggleDeformer::GetStiffness() const
{
  return stiffness;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XJiggleDeformer::GetDamping() const
{
  return damping;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XJiggleDeformer::GetJiggleWeight() const
{
  return jiggle_weight;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XJiggleDeformer::GetDirectionBias() const
{
  return direction_bias;
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::SetEnable(const bool v)
{
  enable=(XS32)v;
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::SetForceAlongNormal(const XReal v)
{
  force_along_normal=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::SetForceOnTangent(const XReal v)
{
  force_on_tangent=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::SetMotionMultiplier(const XReal v)
{
  motion_multiplier=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::SetStiffness(const XReal v)
{
  stiffness=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::SetDamping(const XReal v)
{
  damping=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::SetJiggleWeight(const XReal v)
{
  jiggle_weight=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::SetDirectionBias(const XReal v)
{
  direction_bias=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XJiggleDeformer::SetIgnoreTransform(const bool v)
{
  ignore_transform=v;
}
}
