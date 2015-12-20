//----------------------------------------------------------------------------------------------------------------------
/// \file WrapDeformer.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/WrapDeformer.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XWrapDeformer::XWrapDeformer(XModel* pmod) 
  : XDeformer(pmod)
{
  dropoff=4.0f;
  smoothness=0.0f;
  weight_threshold=0.0f;
  max_distance=0.0f;
  influence_type=2;
  nurbs_samples=10;
  driver_geometry=0;
}

//----------------------------------------------------------------------------------------------------------------------
XWrapDeformer::~XWrapDeformer(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XWrapDeformer::GetApiType() const
{
  return XFn::WrapDeformer;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XWrapDeformer::GetFn(XFn::Type type)
{
  if(XFn::WrapDeformer==type)
    return (XWrapDeformer*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XWrapDeformer::GetFn(XFn::Type type) const
{
  if(XFn::WrapDeformer==type)
    return (const XWrapDeformer*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XWrapDeformer::NodeDeath(XId id)
{
  if(driver_geometry == id)
    driver_geometry=0;
  return XDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XWrapDeformer::PreDelete(XIdSet& extra_nodes)
{
  XDeformer::PreDelete(extra_nodes);

  const XDeletePolicy& dp = GetModel()->GetDeletePolicy();
  if ( dp.IsEnabled(XDeletePolicy::RelatedDeformerInputs) ) 
  {
    // should probably leave the driving geometry
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XWrapDeformer::ReadChunk(std::istream& ifs)
{  
  if (!XDeformer::ReadChunk(ifs)) 
    return false;


  READ_CHECK("dropoff",ifs);
  ifs >> dropoff;
  READ_CHECK("smoothness",ifs);
  ifs >> smoothness;
  READ_CHECK("weight_threshold",ifs);
  ifs >> weight_threshold;
  READ_CHECK("max_distance",ifs);
  ifs >> max_distance;
  READ_CHECK("influence_type",ifs);
  ifs >> influence_type;
  READ_CHECK("nurbs_samples",ifs);
  ifs >> nurbs_samples;
  READ_CHECK("driver_geometry",ifs);
  ifs >> driver_geometry;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XWrapDeformer::WriteChunk(std::ostream& ofs) 
{
  if (!m_AffectedGeometry.size())
  {
    return true;
  }

  if (!XDeformer::WriteChunk(ofs))
    return false;

  ofs << "\tdropoff " << dropoff << "\n";
  ofs << "\tsmoothness " << smoothness << "\n";
  ofs << "\tweight_threshold " << weight_threshold << "\n";
  ofs << "\tmax_distance " << max_distance << "\n";
  ofs << "\tinfluence_type " << influence_type << "\n";
  ofs << "\tnurbs_samples " << nurbs_samples << "\n";
  ofs << "\tdriver_geometry " << driver_geometry << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XWrapDeformer::DoData(XFileIO& io) 
{
  DUMPER(XWrapDeformer);

  IO_CHECK( XDeformer::DoData(io) );

  IO_CHECK( io.DoData(&dropoff) );
  DPARAM( dropoff );

  IO_CHECK( io.DoData(&smoothness) );
  DPARAM( smoothness );

  IO_CHECK( io.DoData(&weight_threshold) );
  DPARAM( weight_threshold );

  IO_CHECK( io.DoData(&max_distance) );
  DPARAM( max_distance );

  IO_CHECK( io.DoData(&influence_type) );
  DPARAM( influence_type );

  IO_CHECK( io.DoData(&nurbs_samples) );
  DPARAM( nurbs_samples );

  IO_CHECK( io.DoData(&driver_geometry) );
  DPARAM( driver_geometry );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XWrapDeformer::DoCopy(const XBase* rhs)
{
  const XWrapDeformer* cb = rhs->HasFn<XWrapDeformer>();
  XMD_ASSERT(cb);
  dropoff = cb->dropoff;
  smoothness = cb->smoothness;
  weight_threshold = cb->weight_threshold;
  max_distance = cb->max_distance;
  influence_type = cb->influence_type;
  nurbs_samples = cb->nurbs_samples;
  driver_geometry = cb->driver_geometry;
  XDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XWrapDeformer::GetDataSize() const
{
  return 4*sizeof(XReal) + sizeof(XId) + 2*sizeof(XU32) + XDeformer::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XReal XWrapDeformer::GetDropoff() const
{
  return dropoff;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XWrapDeformer::GetSmoothness() const
{
  return smoothness;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XWrapDeformer::GetWeightThreshold() const
{
  return weight_threshold;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XWrapDeformer::GetMaxDistance() const
{
  return max_distance;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XWrapDeformer::GetInfluenceType() const
{
  return influence_type;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XWrapDeformer::GetNurbsSamples() const
{
  return nurbs_samples;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XWrapDeformer::GetDriver() const
{
  return m_pModel->FindNode(driver_geometry);
}

//----------------------------------------------------------------------------------------------------------------------
void XWrapDeformer::SetDropoff(const XReal v)
{
  dropoff = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XWrapDeformer::SetSmoothness(const XReal v)
{
  smoothness = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XWrapDeformer::SetWeightThreshold(const XReal v)
{
  weight_threshold = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XWrapDeformer::SetMaxDistance(const XReal v)
{
  max_distance = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XWrapDeformer::SetInfluenceType(const XU32 v)
{
  influence_type = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XWrapDeformer::SetNurbsSamples(const XU32 v)
{
  nurbs_samples = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XWrapDeformer::SetDriver(const XBase* v)
{
  if(v && !v->HasFn<XGeometry>())
  {
    return false;
  }

  if(!v)
  {
    driver_geometry = 0;
    return true;
  }

  bool valid = IsValidObj(v);

  // if you assert here, the driver is NULL or from another XModel
  XMD_ASSERT( valid );

  if(valid)
  {
    if(!v)
      driver_geometry = 0;
    else
      driver_geometry = v->GetID();
    return true;
  }
  return false;
}
}
