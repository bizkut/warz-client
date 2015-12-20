//----------------------------------------------------------------------------------------------------------------------
/// \file Emitter.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ParticleShape.h"
#include "XMD/Emitter.h"
#include "XMD/Base.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{

//----------------------------------------------------------------------------------------------------------------------
XEmitter::XEmitter(XModel* pmod) 
  : XBone(pmod),
  m_Shape(0),
  m_Rate(100.0f),
  m_MaxDistance(0.0f),
  m_MinDistance(0.0f),
  m_Direction(0.0f,1.0f,0.0f),
  m_Spread(0.0f),
  m_Colour(0.3f,0.3f,0.6f),
  m_Offset(0,0,0)
{
  m_Mass=1.0f;
  m_MassRandom=0.0f;
  m_Size=1.0f;
  m_SizeRandom=0.0f;
  m_Speed=1.0f;
  m_SpeedRandom=0.0f;
  m_EmitterType=kOmni;
  m_VolumeType=kCube;
  m_VolumeSweep = 2.0f * XM2::XM2_PI;
  m_Radius = 1.0f;
  m_SectionRadius = 0.5f;
  m_EmitterObject=0;
  m_EmitterTransform=0;
  m_Alpha=1.0f;
  m_AlphaRandom=0.0f;
  m_Hue=0.0f;
  m_Saturation=0.0f;
  m_Luminance=0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
XEmitter::~XEmitter()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XEmitter::GetApiType() const
{
  return XFn::Emitter;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XEmitter::GetFn(XFn::Type type)
{
  if(XFn::Emitter==type)
    return (XEmitter*)this;
  return XBone::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XEmitter::GetFn(XFn::Type type) const
{
  if(XFn::Emitter==type)
    return (const XEmitter*)this;
  return XBone::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XEmitter::NodeDeath(XId id)
{
  // kill the emitter if the system it spits particles into dies
  if (id == m_Shape) 
  {
    return false;
  }
  return XBone::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::PreDelete(XIdSet& extra_nodes)
{
  XBone::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XParticleShape* XEmitter::GetShape() const
{
  XBase* node = m_pModel->FindNode(m_Shape);
  if (node)
  {
    return node->HasFn< XParticleShape >();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XEmitter::SetShape(const XGeometry* ptr)
{
  // asserted because ptr is null or not from this XModel
  XMD_ASSERT( (ptr) && IsValidObj(ptr) );
  const XBase* p =ptr;
  if(ptr) 
  {
    const XParticleShape* ps = p->HasFn<XParticleShape>();
    if( ps )
    {
      m_Shape = ptr->GetID();
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetRate() const
{
  return m_Rate;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetMaxDistance() const
{
  return m_MaxDistance;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetMinDistance() const
{
  return m_MinDistance;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XEmitter::GetDirection() const
{
  return m_Direction;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetSpread() const
{
  return m_Spread;
}

//----------------------------------------------------------------------------------------------------------------------
const XColour& XEmitter::GetColour() const
{
  return m_Colour;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetRate(const XReal v)
{
  m_Rate = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetMaxDistance(const XReal v)
{
  m_MaxDistance = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetMinDistance(const XReal v)
{
  m_MinDistance = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetDirection(const XVector3& v) 
{
  m_Direction = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetSpread(const XReal v) 
{
  m_Spread = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetColour(const XColour& v) 
{
  m_Colour = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetColourOffset(const XReal hue,const XReal saturation,const XReal luminance)
{
  m_Hue = hue;
  m_Saturation = saturation;
  m_Luminance = luminance;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetColourOffsetHue(const XReal hue)
{
  m_Hue = hue;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetColourOffsetSaturation(const XReal saturation)
{
  m_Saturation = saturation;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetColourOffsetLuminance(const XReal luminance)
{
  m_Luminance = luminance;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetColourOffsetHue() const
{
  return m_Hue;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetColourOffsetSaturation() const
{
  return m_Saturation;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetColourOffsetLuminance() const
{
  return m_Luminance;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetAlpha() const
{
  return m_Alpha;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetAlphaRandom() const
{
  return m_AlphaRandom;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetAlpha(const XReal c)
{
  m_Alpha = c;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetAlphaRandom(const XReal c)
{
  m_AlphaRandom=c;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetSize() const
{
  return m_Size;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetSize(const XReal c)
{
  m_Size = c;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetSizeRandom() const
{
  return m_SizeRandom;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetSizeRandom(const XReal c)
{
  m_SizeRandom = c;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetMass() const
{
  return m_Mass;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetMass(const XReal c)
{
  m_Mass = c;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetMassRandom() const
{
  return m_MassRandom;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetMassRandom(const XReal c)
{
  m_MassRandom = c;
}

//----------------------------------------------------------------------------------------------------------------------
bool XEmitter::ReadChunk(std::istream& ifs)
{
  if(!XBone::ReadChunk(ifs))
    return false;

  READ_CHECK("shape",ifs);
  ifs >> m_Shape;

  READ_CHECK("rate",ifs);
  ifs >> m_Rate;

  READ_CHECK("max_distance",ifs);
  ifs >> m_MaxDistance;

  READ_CHECK("min_distance",ifs);
  ifs >> m_MinDistance;

  READ_CHECK("direction",ifs);
  ifs >> m_Direction;

  READ_CHECK("spread",ifs);
  ifs >> m_Spread;

  READ_CHECK("particle_colour",ifs);
  ifs >> m_Colour.r >> m_Colour.g >> m_Colour.b;

  if (GetModel()->GetInputVersion()>=4)
  {
    XString temp;
    READ_CHECK("emitter_type",ifs);
    ifs >> temp;
    if(temp == "OMNI") m_EmitterType = kOmni;
    else
    if(temp == "DIRECTIONAL") m_EmitterType = kDirectional;
    else
    if(temp == "GEOMETRY") m_EmitterType = kGeometry;
    else
    if(temp == "VOLUME") m_EmitterType = kVolume;
    else
    {
      m_EmitterType = kOmni;
      XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,"Unknown emitter type, \"%s\" when reading chunk \"%s\"",
        temp.c_str(), GetName() );
    }

    READ_CHECK("emitter_object",ifs);
    ifs >> m_EmitterObject;

    READ_CHECK("emitter_transform",ifs);
    ifs >> m_EmitterTransform;

    READ_CHECK("volume_shape",ifs);
    ifs >> temp;
    if(temp == "CUBE") m_VolumeType = kCube;
    else
    if(temp == "SPHERE") m_VolumeType = kSphere;
    else
    if(temp == "CYLINDER") m_VolumeType = kCylinder;
    else
    if(temp == "CONE") m_VolumeType = kCone;
    else
    if(temp == "TORUS") m_VolumeType = kTorus;
    else
    if(temp == "DISC") m_VolumeType = kDisc;
    else
    if(temp == "GRID") m_VolumeType = kGrid;
    else
    {
      m_VolumeType = kCube;
      XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,"Unknown volume type, \"%s\" when reading chunk \"%s\"",
        temp.c_str(), GetName() );
    }

    READ_CHECK("volume_offset",ifs);
    ifs >> m_Offset;

    READ_CHECK("volume_sweep",ifs);
    ifs >> m_VolumeSweep;

    READ_CHECK("inner_radius",ifs);
    ifs >> m_SectionRadius;

    READ_CHECK("speed",ifs);
    ifs >> m_Speed;

    READ_CHECK("speed_random",ifs);
    ifs >> m_SpeedRandom;

    READ_CHECK("size",ifs);
    ifs >> m_Size;

    READ_CHECK("size_random",ifs);
    ifs >> m_SizeRandom;

    READ_CHECK("mass",ifs);
    ifs >> m_Mass;

    READ_CHECK("mass_random",ifs);
    ifs >> m_MassRandom;

    READ_CHECK("alpha",ifs);
    ifs >> m_Alpha;

    READ_CHECK("alpha_random",ifs);
    ifs >> m_AlphaRandom;

    READ_CHECK("hue_offset",ifs);
    ifs >> m_Hue;

    READ_CHECK("saturation_offset",ifs);
    ifs >> m_Saturation;

    READ_CHECK("luminance_offset",ifs);
    ifs >> m_Luminance;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XEmitter::WriteChunk(std::ostream& ofs)
{
  if(!XBone::WriteChunk(ofs))
    return false;

  ofs << "\tshape " << m_Shape << "\n";
  ofs << "\trate " << m_Rate << "\n";
  ofs << "\tmax_distance " << m_MaxDistance << "\n";
  ofs << "\tmin_distance " << m_MinDistance << "\n";
  ofs << "\tdirection " << m_Direction << "\n";
  ofs << "\tspread " << m_Spread << "\n";
  ofs << "\tparticle_colour " 
      << m_Colour.r << " "
      << m_Colour.g << " " 
      << m_Colour.b << "\n";

  ofs << "\temitter_type ";
  switch(m_EmitterType)
  {
  case kDirectional: ofs << "DIRECTIONAL\n"; break;
  case kGeometry: ofs << "GEOMETRY\n"; break;
  case kVolume: ofs << "VOLUME\n"; break;
  default:
  case kOmni: ofs << "OMNI\n"; break;
  }

  ofs << "\temitter_object " << m_EmitterObject << "\n";
  ofs << "\temitter_transform " << m_EmitterTransform << "\n";

  ofs << "\tvolume_shape ";
  switch(m_VolumeType)
  {
  case kCube:     ofs << "CUBE\n"; break;
  case kSphere:   ofs << "SPHERE\n"; break;
  case kCylinder: ofs << "CYLINDER\n"; break;
  case kCone:     ofs << "CONE\n"; break;
  case kTorus:    ofs << "TORUS\n"; break;
  case kDisc:     ofs << "DISC\n"; break;   
  case kGrid:     ofs << "GRID\n"; break;
  }

  ofs << "\tvolume_offset " << m_Offset << "\n";
  ofs << "\tvolume_sweep " << m_VolumeSweep << "\n";
  ofs << "\tinner_radius " << m_SectionRadius << "\n";
  ofs << "\tspeed " << m_Speed << "\n";
  ofs << "\tspeed_random " << m_SpeedRandom << "\n";
  ofs << "\tsize " << m_Size << "\n";
  ofs << "\tsize_random " << m_SizeRandom << "\n";
  ofs << "\tmass " << m_Mass << "\n";
  ofs << "\tmass_random " << m_MassRandom << "\n";
  ofs << "\talpha " << m_Alpha << "\n";
  ofs << "\talpha_random " << m_AlphaRandom << "\n";
  ofs << "\thue_offset " << m_Hue << "\n";
  ofs << "\tsaturation_offset " << m_Saturation << "\n";
  ofs << "\tluminance_offset " << m_Luminance << "\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XEmitter::DoData(XFileIO& io)
{
  DUMPER(XEmitter);

  IO_CHECK( XBone::DoData(io) );

  IO_CHECK( io.DoData(&m_Shape) );
  DPARAM( m_Shape );

  IO_CHECK( io.DoData(&m_Rate) );
  DPARAM( m_Rate );

  IO_CHECK( io.DoData(&m_MaxDistance) );
  DPARAM( m_MaxDistance );

  IO_CHECK( io.DoData(&m_MinDistance) );
  DPARAM( m_MinDistance );

  IO_CHECK( io.DoData(&m_Direction) );
  DPARAM( m_Direction );

  IO_CHECK( io.DoData(&m_Spread) );
  DPARAM( m_Spread );

  IO_CHECK( io.DoData(&m_Colour) );
  DPARAM( m_Colour );

  if (GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( io.DoData(&m_EmitterType) );
    DPARAM( m_EmitterType );

    IO_CHECK( io.DoData(&m_EmitterObject) );
    DPARAM( m_EmitterObject );

    IO_CHECK( io.DoData(&m_EmitterTransform) );
    DPARAM( m_EmitterTransform );

    IO_CHECK( io.DoData(&m_VolumeType) );
    DPARAM( m_VolumeType );

    IO_CHECK( io.DoData(&m_Offset) );
    DPARAM( m_Offset );

    IO_CHECK( io.DoData(&m_VolumeSweep) );
    DPARAM( m_VolumeSweep );

    IO_CHECK( io.DoData(&m_Radius) );
    DPARAM( m_Radius );

    IO_CHECK( io.DoData(&m_SectionRadius) );
    DPARAM( m_SectionRadius );

    IO_CHECK( io.DoData(&m_Mass) );
    DPARAM( m_Mass );

    IO_CHECK( io.DoData(&m_MassRandom) );
    DPARAM( m_MassRandom );

    IO_CHECK( io.DoData(&m_Size) );
    DPARAM( m_Size );

    IO_CHECK( io.DoData(&m_SizeRandom) );
    DPARAM( m_SizeRandom );

    IO_CHECK( io.DoData(&m_Speed) );
    DPARAM( m_Speed );

    IO_CHECK( io.DoData(&m_SpeedRandom) );
    DPARAM( m_SpeedRandom );

    IO_CHECK( io.DoData(&m_Alpha) );
    DPARAM( m_Alpha );

    IO_CHECK( io.DoData(&m_AlphaRandom) );
    DPARAM( m_AlphaRandom );

    IO_CHECK( io.DoData(&m_Hue) );
    DPARAM( m_Hue );

    IO_CHECK( io.DoData(&m_Saturation) );
    DPARAM( m_Saturation );

    IO_CHECK( io.DoData(&m_Luminance) );
    DPARAM( m_Luminance );
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XEmitter::GetDataSize() const 
{
  XU32 sz = XBone::GetDataSize() +
            4*sizeof(XReal) +
            sizeof(XId) +
            3*sizeof(XReal) +
            sizeof(XColour); // V3

  sz += 14*sizeof(XReal) + 
        sizeof(XEmitterType) +
        sizeof(XEmitterVolumeType) + 
        3*sizeof(XReal) + 
        2*sizeof(XId);  // V4

  return sz;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::DoCopy(const XBase* rhs)
{
  const XEmitter* cb = rhs->HasFn<XEmitter>();
  XMD_ASSERT(cb);

  XParticleShape* ps = cb->GetShape();
  if (ps)
  {
    if (GetModel()->GetClonePolicy().IsEnabled(XClonePolicy::DuplicateParticleShapes))
    {
      XParticleShape* new_ps = ps->Clone()->HasFn<XParticleShape>();
      XMD_ASSERT(new_ps);

      m_Shape = new_ps->GetID();
    }
    else
    {
      m_Shape = cb->m_Shape;
    }
  }
  m_Rate = cb->m_Rate;
  m_MaxDistance = cb->m_MaxDistance;
  m_MinDistance = cb->m_MinDistance;
  m_Direction = cb->m_Direction;
  m_Spread = cb->m_Spread;
  m_Colour = cb->m_Colour;
  m_Offset = cb->m_Offset;
  m_Mass = cb->m_Mass;
  m_MassRandom = cb->m_MassRandom;
  m_Size = cb->m_Size;
  m_SizeRandom = cb->m_SizeRandom;
  m_Speed = cb->m_Speed;
  m_SpeedRandom = cb->m_SpeedRandom;
  m_EmitterType = cb->m_EmitterType;
  m_VolumeType = cb->m_VolumeType;
  m_VolumeSweep = cb->m_VolumeSweep;
  m_Radius = cb->m_Radius;
  m_SectionRadius = cb->m_SectionRadius;
  m_EmitterObject = cb->m_EmitterObject;
  m_EmitterTransform = cb->m_EmitterTransform;
  m_Alpha=cb->m_Alpha;
  m_AlphaRandom=cb->m_AlphaRandom;
  m_Hue=cb->m_Hue;
  m_Saturation=cb->m_Saturation;
  m_Luminance=cb->m_Luminance;

  XBone::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XEmitter::SetEmitterGeometry(XGeometry* geometry,XBone* parent)
{
  if(geometry == 0 || parent == 0)
  {
    m_EmitterObject = 0;
    m_EmitterTransform = 0;
    return true;
  }

  bool valid = IsValidObj(geometry) && IsValidObj(parent);

  // if you assert here, the geometry object and/or parent object are
  // not node's that belong to the same XModel as this node. 
  // 
  XMD_ASSERT(valid);

  if(!valid)
    return false;

  m_EmitterObject = geometry ? geometry->GetID() : 0;
  if(m_EmitterObject)
  { 
    m_EmitterTransform = parent ? parent->GetID() : 0;
    if(!m_EmitterTransform)
    {
      XInstanceList instances;
      geometry->GetObjectInstances(instances);
      if(instances.size())
      {
        if(instances[0]->GetParent())
          m_EmitterTransform = instances[0]->GetParent()->GetID();
      }
      else
        m_EmitterTransform = 0;
    }
  }
  else
    m_EmitterTransform = 0;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XEmitter::SetEmitterGeometry(XInstance* geometry)
{
  if (geometry)
  {
    if(!geometry->GetGeometry())
      return false;

    XMD_ASSERT(geometry->GetParent());
    if(!geometry->GetParent())
      return false;

    m_EmitterObject = geometry->GetGeometry()->GetID();
    m_EmitterTransform = geometry->GetParent()->GetID();
  }
  else
  {
    m_EmitterTransform=0;
    m_EmitterObject=0;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XGeometry* XEmitter::GetEmitterGeometry() const
{
  if(!m_EmitterObject)
    return 0;

  XBase* b = m_pModel->FindNode(m_EmitterObject);
  if(!b)
    return 0;

  return b->HasFn<XGeometry>();
}

//----------------------------------------------------------------------------------------------------------------------
XBone* XEmitter::GetEmitterTransform() const
{
  if(!m_EmitterTransform)
    return 0;

  XBase* b = m_pModel->FindNode(m_EmitterTransform);
  if(!b)
    return 0;

  return b->HasFn<XBone>();
}

//----------------------------------------------------------------------------------------------------------------------
XInstance* XEmitter::GetEmitterInstance() const
{
  if(!m_EmitterObject)
    return 0;

  XBase* b = m_pModel->FindNode(m_EmitterObject);
  if(!b)
    return 0;

  XGeometry* g = b->HasFn<XGeometry>();
  if (g)
  {
    XInstanceList instances;
    g->GetObjectInstances(instances);
    for(XInstanceList::iterator it = instances.begin();it != instances.end();++it)
    {
      if( (*it)->GetParent()->GetID() == m_EmitterTransform )
        return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XEmitter::GetOffset() const
{
  return m_Offset;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetOffset(const XVector3& v)
{
  m_Offset = v;
}

//----------------------------------------------------------------------------------------------------------------------
XEmitter::XEmitterType XEmitter::GetEmitterType() const
{
  return m_EmitterType;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetEmitterType(const XEmitterType et)
{
  m_EmitterType = et;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetSpeed() const
{
  return m_Speed;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetSpeed(const XReal c)
{
  m_Speed = c;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetSpeedRandom() const
{
  return m_SpeedRandom;
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetSpeedRandom(const XReal c)
{
  m_SpeedRandom = c;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetVolumeRadius() const 
{ 
  return m_Radius; 
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetVolumeRadius(const XReal r) 
{ 
  m_Radius=r; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetInnerRadius() const 
{ 
  return m_SectionRadius; 
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetInnerRadius(const XReal r) 
{
  m_SectionRadius=r;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetWidth() const 
{ 
  return m_Radius; 
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetWidth(const XReal r) 
{ 
  m_Radius=r; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetHeight() const 
{ 
  return m_SectionRadius; 
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetHeight(const XReal r) 
{
  m_SectionRadius=r;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XEmitter::GetVolumeSweep() const 
{ 
  return m_VolumeSweep; 
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetVolumeSweep(const XReal r) 
{ 
  m_VolumeSweep=r; 
}

//----------------------------------------------------------------------------------------------------------------------
XEmitter::XEmitterVolumeType XEmitter::GetVolumeType() const 
{ 
  return m_VolumeType; 
}

//----------------------------------------------------------------------------------------------------------------------
void XEmitter::SetVolumeType(const XEmitterVolumeType et) 
{ 
  m_VolumeType=et; 
}
}
