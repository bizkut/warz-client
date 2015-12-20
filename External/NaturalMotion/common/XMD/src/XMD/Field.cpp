//----------------------------------------------------------------------------------------------------------------------
/// \file Field.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Field.h"
#include "XMD/ParticleShape.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include <stdlib.h>

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
void XField::PostLoad() 
{
  // build list of deformers in the queue. First blend shapes, skinning, lattices
  XList objs;

  m_pModel->List(objs,XFn::ParticleShape);

  XList::iterator it = objs.begin();
  for( ; it != objs.end(); ++it )
  {
    XParticleShape* pd = (*it)->HasFn<XParticleShape>();

    XIndexList::iterator itt = pd->m_Fields.begin();
    for( ; itt != pd->m_Fields.end(); ++itt )
    {
      if(*itt == GetID())
      {
        m_Shapes.push_back( pd->GetID() );
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XField::XField(XModel* pmod)
  : XBone(pmod),
  m_Magnitude(1),
  m_Attenuation(0),
  m_MaxDistance(10),
  m_bUseMaxDistance(0),
  m_bPerVertex(0),
  m_VolumeType(kNone),
  m_SweepAngle(2.0f*XM2::XM2_PI),
  m_SectionRadius(0.5f),
  m_VolumeOffset(0,0,0),
  m_VolumeExclusion(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
XField::~XField()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XField::SetVolumeType(const XVolumeType type)
{
  m_VolumeType = type;
}

//----------------------------------------------------------------------------------------------------------------------
XField::XVolumeType XField::GetVolumeType() const
{
  return m_VolumeType;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XField::GetSweepAngle() const
{
  return m_SweepAngle;
}

//----------------------------------------------------------------------------------------------------------------------
void XField::SetSweepAngle(const XReal sweep_angle)
{
  m_SweepAngle = sweep_angle;
  if (m_SweepAngle>2.0f*XM2::XM2_PI)
    m_SweepAngle=2.0f*XM2::XM2_PI;
  if (m_SweepAngle<0)
    m_SweepAngle=0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XField::GetSectionRadius() const
{
  return m_SectionRadius;
}

//----------------------------------------------------------------------------------------------------------------------
void XField::SetSectionRadius(const XReal section_radius)
{
  m_SectionRadius = section_radius;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XField::GetVolumeOffset() const
{
  return m_VolumeOffset;
}

//----------------------------------------------------------------------------------------------------------------------
void XField::SetVolumeOffset(const XVector3& offset)
{
  m_VolumeOffset = offset;
}

//----------------------------------------------------------------------------------------------------------------------
bool XField::GetVolumeExclusion() const
{
  return m_VolumeExclusion;
}

//----------------------------------------------------------------------------------------------------------------------
void XField::SetVolumeExclusion(const bool exclusion)
{
  m_VolumeExclusion = exclusion;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XField::GetApiType() const
{
  return XFn::Field;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XField::GetFn(XFn::Type type)
{
  if(XFn::Field==type)
    return (XField*)this;
  return XBone::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XField::GetFn(XFn::Type type) const
{
  if(XFn::Field==type)
    return (const XField*)this;
  return XBone::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XField::NodeDeath(XId id)
{
  // an array of indices to shapes that are influenced by this field
  XIndexList::iterator it = m_Shapes.begin();
  for( ; it != m_Shapes.end(); ++it )
  {
    if (id == *it)
    {
      m_Shapes.erase(it);
      break;
    }
  }
  if (m_Shapes.size()==0) 
  {
    return false;
  }
  return XBone::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XField::PreDelete(XIdSet& extra_nodes)
{
  XBone::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XField::GetMagnitude() const
{
  return m_Magnitude;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XField::GetAttenuation() const 
{
  return m_Attenuation;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XField::GetMaxDistance() const
{
  return m_MaxDistance;
}

//----------------------------------------------------------------------------------------------------------------------
bool XField::GetMaxDistanceUsed() const 
{
  return m_bUseMaxDistance;
}

//----------------------------------------------------------------------------------------------------------------------
bool XField::GetPerVertex() const 
{
  return m_bPerVertex;
}

//----------------------------------------------------------------------------------------------------------------------
void XField::SetMagnitude(const XReal v)
{
  m_Magnitude=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XField::SetAttenuation(const XReal v) 
{
  m_Attenuation=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XField::SetMaxDistance(const XReal v)
{
  m_MaxDistance=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XField::SetMaxDistanceUsed(const bool v) 
{
  m_bUseMaxDistance=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XField::SetPerVertex(const bool v) 
{
  m_bPerVertex=v;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XField::GetNumShapes() const 
{
  return static_cast<XU32>( m_Shapes.size() ); 
}

//----------------------------------------------------------------------------------------------------------------------
XGeometry* XField::GetShape(const XU32 idx) const
{
  if (GetNumShapes() > idx) 
  {
    XBase* ptr = m_pModel->FindNode( m_Shapes[idx] );
    return ptr->HasFn< XGeometry >();

  }
  return 0; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XField::AddShape(const XBase* shape) 
{
  // if you assert here, the shape is NULL or from another XModel
  XMD_ASSERT( (shape) && IsValidObj(shape) );

  // if you assert here, it's because only XParticleShapes can be 
  // affected by a field
  XMD_ASSERT( !shape->HasFn<XParticleShape>() );

  if( shape && shape->HasFn<XGeometry>() )
  {
    m_Shapes.push_back( shape->GetID() );
    return true;
  }

  // if you assert here, you have not provided a geometry object to be affected.
  XMD_ASSERT(0);

  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XField::DeleteShape(const XU32 idx)
{
  if (GetNumShapes() > idx)
  {
    m_Shapes.erase( m_Shapes.begin() + idx );
    return true;
  }
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XField::ReadChunk(std::istream& ifs)
{
  if(!XBone::ReadChunk(ifs))
    return false;

  READ_CHECK("magnitude",ifs);
  ifs >> m_Magnitude;

  READ_CHECK("attenuation",ifs);
  ifs >> m_Attenuation;

  READ_CHECK("max_distance",ifs);
  ifs >> m_MaxDistance;

  READ_CHECK("use_max_distance",ifs);
  ifs >> m_bUseMaxDistance;

  READ_CHECK("per_vertex",ifs);
  ifs >> m_bPerVertex;

  if (GetModel()->GetInputVersion()>=4)
  {
    std::string temp;
    READ_CHECK("volume_shape",ifs);
    ifs >> temp;
    if (temp=="NONE")
      m_VolumeType = kNone;
    else
    if(temp=="CUBE")
      m_VolumeType = kCube;
    else
    if(temp=="CYLINDER")
      m_VolumeType = kCylinder;
    else
    if(temp=="CONE")
      m_VolumeType = kCone;
    else
    if(temp=="SPHERE")
      m_VolumeType = kSphere;
    else
    if(temp=="TORUS")
      m_VolumeType = kTorus;
    else
    {
      XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,"Unknown volume type for XField - \"%s\"",temp.c_str());
    }

    READ_CHECK("volume_exclusion",ifs);
    ifs >> m_VolumeExclusion;

    READ_CHECK("volume_offset",ifs);
    ifs >> m_VolumeOffset;

    READ_CHECK("volume_sweep",ifs);
    ifs >> m_SweepAngle;

    READ_CHECK("inner_radius",ifs);
    ifs >> m_SectionRadius;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XField::WriteChunk(std::ostream& ofs)
{
  if(!XBone::WriteChunk(ofs))
    return false;

  ofs << "\tmagnitude ";
  ofs << m_Magnitude;
  ofs << "\n\tattenuation ";
  ofs << m_Attenuation;
  ofs << "\n\tmax_distance ";
  ofs << m_MaxDistance;
  ofs << "\n\tuse_max_distance ";
  ofs << m_bUseMaxDistance;
  ofs << "\n\tper_vertex ";
  ofs << m_bPerVertex << "\n";

  ofs << "\n\tvolume_shape ";
  switch(m_VolumeType)
  {
  case kCube: ofs << "CUBE\n"; break;
  case kCylinder: ofs << "CYLINDER\n"; break;
  case kCone: ofs << "CONE\n"; break;
  case kSphere: ofs << "SPHERE\n"; break;
  case kTorus: ofs << "TORUS\n"; break;
  case kNone: 
  default: ofs << "NONE\n"; break;
  }

  ofs << "\n\tvolume_exclusion ";
  ofs << m_VolumeExclusion << "\n";
  ofs << "\n\tvolume_offset ";
  ofs << m_VolumeOffset << "\n";
  ofs << "\n\tvolume_sweep ";
  ofs << m_SweepAngle << "\n";
  ofs << "\n\tinner_radius ";
  ofs << m_SectionRadius << "\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XField::DoData(XFileIO& io)
{
  DUMPER(XField);

  IO_CHECK( XBone::DoData(io) );

  IO_CHECK( io.DoData(&m_Magnitude) );
  DPARAM( m_Magnitude );

  IO_CHECK( io.DoData(&m_Attenuation) );
  DPARAM( m_Attenuation );

  IO_CHECK( io.DoData(&m_MaxDistance) );
  DPARAM( m_MaxDistance );

  IO_CHECK( io.DoData(&m_bUseMaxDistance) );
  DPARAM( m_bUseMaxDistance );

  IO_CHECK( io.DoData(&m_bPerVertex) );
  DPARAM( m_bPerVertex );

  if (GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( io.DoData(&m_VolumeType) );
    DPARAM( m_VolumeType );

    IO_CHECK( io.DoData(&m_VolumeExclusion) );
    DPARAM( m_VolumeExclusion );

    IO_CHECK( io.DoData(&m_VolumeOffset) );
    DPARAM( m_VolumeOffset );

    IO_CHECK( io.DoData(&m_SweepAngle) );
    DPARAM( m_SweepAngle );

    IO_CHECK( io.DoData(&m_SectionRadius) );
    DPARAM( m_SectionRadius );
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XField::DoCopy(const XBase* rhs)
{
  const XField* cb = rhs->HasFn<XField>();
  XMD_ASSERT(cb);
  m_Magnitude = cb->m_Magnitude;
  m_Attenuation = cb->m_Attenuation;
  m_MaxDistance = cb->m_MaxDistance;
  m_bUseMaxDistance = cb->m_bUseMaxDistance;
  m_bPerVertex = cb->m_bPerVertex;

  m_VolumeType = cb->m_VolumeType;
  m_VolumeExclusion = cb->m_VolumeExclusion;
  m_VolumeOffset = cb->m_VolumeOffset;
  m_SweepAngle = cb->m_SweepAngle;
  m_SectionRadius = cb->m_SectionRadius;

  XBone::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XField::GetDataSize() const
{
  return XBone::GetDataSize() +
      sizeof(XReal)*3 +
      sizeof(bool)*2 +
      sizeof(XVolumeType) + 
      3*sizeof(XReal) + 
      sizeof(bool) +
      sizeof(XReal)*2;
}
}

