//----------------------------------------------------------------------------------------------------------------------
/// \file BlendShape.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/NonLinearDeformer.h"
#include "XMD/Bone.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XNonLinearDeformer::XNonLinearDeformer(XModel* pmod) 
  : XDeformer(pmod),handle(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearDeformer::~XNonLinearDeformer()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XNonLinearDeformer::GetApiType() const
{
  return XFn::NonLinearDeformer;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNonLinearDeformer::GetFn(XFn::Type type)
{
  if(XFn::NonLinearDeformer==type)
    return (XNonLinearDeformer*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XNonLinearDeformer::GetFn(XFn::Type type) const
{
  if(XFn::NonLinearDeformer==type)
    return (const XNonLinearDeformer*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearDeformer::NodeDeath(XId id)
{
  if(id==handle)
    handle=0;
  return XDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearDeformer::PreDelete(XIdSet& extra_nodes)
{
  XDeformer::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XBone* XNonLinearDeformer::GetHandle() const
{
  XBase* pnode = m_pModel-> FindNode(handle);
  if (pnode) 
  {
    return pnode->HasFn<XBone>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearDeformer::SetHandle(const XBase* hand) 
{
  // if you assert here, the handle is NULL or not from this XModel
  XMD_ASSERT( (hand) && IsValidObj(hand) );
  if (hand->HasFn<XBone>()) 
  {
    handle = hand->GetID();
    return true;
  }

  // if you assert here, the handle is not a valid transform
  XMD_ASSERT(0);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearDeformer::ReadChunk(std::istream& ifs)
{  
  if (!XDeformer::ReadChunk(ifs)) 
    return false;

  READ_CHECK("handle",ifs);
  ifs >> handle;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearDeformer::WriteChunk(std::ostream& ofs) 
{
  if (!XDeformer::WriteChunk(ofs))
    return false;

  ofs << "\thandle " << handle << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearDeformer::DoData(XFileIO& io) 
{
  DUMPER(XNonLinearDeformer);

  IO_CHECK( XDeformer::DoData(io) );

  IO_CHECK( io.DoData(&handle) );
  DPARAM( handle );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearDeformer::DoCopy(const XBase* rhs)
{
  const XNonLinearDeformer* cb = rhs->HasFn<XNonLinearDeformer>();
  XMD_ASSERT(cb);
  handle = cb->handle;
  XDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNonLinearDeformer::GetDataSize() const 
{
  return sizeof(XId) + XDeformer::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearBend::XNonLinearBend(XModel* pmod) 
  : XNonLinearDeformer(pmod)
{
  low_bound =-1;
  high_bound =1;
  curvature =0;
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearBend::~XNonLinearBend(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearBend::GetLowBound() const 
{ 
  return low_bound; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearBend::GetHighBound() const 
{ 
  return high_bound; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearBend::GetCurvature() const 
{ 
  return curvature; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearBend::SetLowBound(const XReal v) 
{ 
  low_bound=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearBend::SetHighBound(const XReal v) 
{ 
  high_bound=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearBend::SetCurvature(const XReal v) 
{
  curvature=v; 
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XNonLinearBend::GetApiType() const
{
  return XFn::NonLinearBend;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNonLinearBend::GetFn(XFn::Type type)
{
  if(XFn::NonLinearBend==type)
    return (XNonLinearBend*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XNonLinearBend::GetFn(XFn::Type type) const
{
  if(XFn::NonLinearBend==type)
    return (const XNonLinearBend*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearBend::NodeDeath(XId id)
{
  return XNonLinearDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearBend::PreDelete(XIdSet& extra_nodes)
{
  XNonLinearDeformer::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearBend::ReadChunk(std::istream& ifs)
{  
  if (!XNonLinearDeformer::ReadChunk(ifs)) 
    return false;

  READ_CHECK("low_bound",ifs);
  ifs >> low_bound;
  READ_CHECK("high_bound",ifs);
  ifs >> high_bound;
  READ_CHECK("curvature",ifs);
  ifs >> curvature;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearBend::WriteChunk(std::ostream& ofs) 
{
  if (!XNonLinearDeformer::WriteChunk(ofs))
    return false;

  ofs << "\tlow_bound " << low_bound << "\n";
  ofs << "\thigh_bound " << high_bound << "\n";
  ofs << "\tcurvature " << curvature << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearBend::DoData(XFileIO& io)
{
  DUMPER(XNonLinearBend);

  IO_CHECK( XNonLinearDeformer::DoData(io) );

  IO_CHECK( io.DoData(&low_bound) );
  DPARAM( low_bound );
  IO_CHECK( io.DoData(&high_bound) );
  DPARAM( high_bound );
  IO_CHECK( io.DoData(&curvature) );
  DPARAM( curvature );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearBend::DoCopy(const XBase* rhs)
{
  const XNonLinearBend* cb = rhs->HasFn<XNonLinearBend>();
  XMD_ASSERT(cb);
  low_bound = cb->low_bound;
  high_bound = cb->high_bound;
  curvature = cb->curvature;
  XNonLinearDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNonLinearBend::GetDataSize() const 
{
  return 3*sizeof(XReal) + XNonLinearDeformer::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearFlare::XNonLinearFlare(XModel* pmod) 
  : XNonLinearDeformer(pmod)
{
  low_bound =-1;
  high_bound =1;
  start_flareX =1;
  start_flareZ =1;
  end_flareX =1;
  end_flareZ =1;
  curve =0;
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearFlare::~XNonLinearFlare(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearFlare::GetLowBound() const 
{ 
  return low_bound; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearFlare::GetHighBound() const 
{ 
  return high_bound; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearFlare::GetStartFlareX() const 
{ 
  return start_flareX; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearFlare::GetStartFlareZ() const 
{ 
  return start_flareZ; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearFlare::GetEndFlareX() const 
{ 
  return end_flareX; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearFlare::GetEndFlareZ() const 
{ 
  return end_flareZ; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearFlare::GetCurve() const 
{ 
  return curve; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearFlare::SetLowBound(const XReal v)
{ 
  low_bound=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearFlare::SetHighBound(const XReal v) 
{ 
  high_bound=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearFlare::SetStartFlareX(const XReal v) 
{ 
  start_flareX=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearFlare::SetStartFlareZ(const XReal v)
{ 
  start_flareZ=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearFlare::SetEndFlareX(const XReal v) 
{ 
  end_flareX=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearFlare::SetEndFlareZ(const XReal v) 
{ 
  end_flareZ=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearFlare::SetCurve(const XReal v)
{ 
  curve=v; 
}

//---------------------------------------------------------------------------------------------------------------------- 
XFn::Type XNonLinearFlare::GetApiType() const
{
  return XFn::NonLinearFlare;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNonLinearFlare::GetFn(XFn::Type type)
{
  if(XFn::NonLinearFlare==type)
    return (XNonLinearFlare*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XNonLinearFlare::GetFn(XFn::Type type) const
{
  if(XFn::NonLinearFlare==type)
    return (const XNonLinearFlare*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearFlare::NodeDeath(XId id)
{
  return XNonLinearDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearFlare::PreDelete(XIdSet& extra_nodes)
{
  XNonLinearDeformer::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearFlare::ReadChunk(std::istream& ifs)
{
  if (!XNonLinearDeformer::ReadChunk(ifs)) 
    return false;

  READ_CHECK("low_bound",ifs);
  ifs >> low_bound;
  READ_CHECK("high_bound",ifs);
  ifs >> high_bound;
  READ_CHECK("start_flare_x",ifs);
  ifs >> start_flareX;
  READ_CHECK("start_flare_z",ifs);
  ifs >> start_flareZ;
  READ_CHECK("end_flare_x",ifs);
  ifs >> end_flareX;
  READ_CHECK("end_flare_z",ifs);
  ifs >> end_flareZ;
  READ_CHECK("curve",ifs);
  ifs >> curve;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearFlare::WriteChunk(std::ostream& ofs) 
{
  if (!XNonLinearDeformer::WriteChunk(ofs))
    return false;

  ofs << "\tlow_bound " << low_bound << "\n";
  ofs << "\thigh_bound " << high_bound << "\n";
  ofs << "\tstart_flare_x " << start_flareX << "\n";
  ofs << "\tstart_flare_z " << start_flareZ << "\n";
  ofs << "\tend_flare_x " << end_flareX << "\n";
  ofs << "\tend_flare_z " << end_flareZ << "\n";
  ofs << "\tcurve " << curve<< "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearFlare::DoData(XFileIO& io)
{
  DUMPER(XNonLinearFlare);

  IO_CHECK( XNonLinearDeformer::DoData(io) );

  IO_CHECK( io.DoData(&low_bound) );
  DPARAM( low_bound );
  IO_CHECK( io.DoData(&high_bound) );
  DPARAM( high_bound );
  IO_CHECK( io.DoData(&start_flareX) );
  DPARAM( start_flareX );
  IO_CHECK( io.DoData(&start_flareZ) );
  DPARAM( start_flareZ );
  IO_CHECK( io.DoData(&end_flareX) );
  DPARAM( end_flareX );
  IO_CHECK( io.DoData(&end_flareZ) );
  DPARAM( end_flareZ );
  IO_CHECK( io.DoData(&curve) );
  DPARAM( curve );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearFlare::DoCopy(const XBase* rhs)
{
  const XNonLinearFlare* cb = rhs->HasFn<XNonLinearFlare>();
  XMD_ASSERT(cb);
  low_bound = cb->low_bound;
  high_bound = cb->high_bound;
  start_flareX = cb->start_flareX;
  start_flareZ = cb->start_flareZ;
  end_flareX = cb->end_flareX;
  end_flareZ = cb->end_flareZ;
  curve = cb->curve;
  XNonLinearDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNonLinearFlare::GetDataSize() const 
{
  return 7*sizeof(XReal) + XNonLinearDeformer::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearSine::XNonLinearSine(XModel* pmod) 
  : XNonLinearDeformer(pmod)
{
  low_bound =-1;
  high_bound =1;
  amplitude =0;
  wavelength =2;
  dropoff= 0;
  offset =0;
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearSine::~XNonLinearSine()
{
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSine::GetLowBound() const
{ 
  return low_bound; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSine::GetHighBound() const 
{ 
  return high_bound; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSine::GetAmplitude() const 
{ 
  return amplitude; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSine::GetWavelength() const 
{ 
  return wavelength; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSine::GetDropoff() const 
{ 
  return dropoff; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSine::GetOffset() const 
{ 
  return offset; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSine::SetLowBound(const XReal v) 
{ 
  low_bound=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSine::SetHighBound(const XReal v) 
{ 
  high_bound=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSine::SetAmplitude(const XReal v)
{
  amplitude=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSine::SetWavelength(const XReal v) 
{ 
  wavelength=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSine::SetDropoff(const XReal v) 
{ 
  dropoff=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSine::SetOffset(const XReal v)
{ 
  offset=v; 
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XNonLinearSine::GetApiType() const
{
  return XFn::NonLinearSine;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNonLinearSine::GetFn(XFn::Type type)
{
  if(XFn::NonLinearSine==type)
    return (XNonLinearSine*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XNonLinearSine::GetFn(XFn::Type type) const
{
  if(XFn::NonLinearSine==type)
    return (const XNonLinearSine*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearSine::NodeDeath(XId id)
{
  return XNonLinearDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSine::PreDelete(XIdSet& extra_nodes)
{
  XNonLinearDeformer::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearSine::ReadChunk(std::istream& ifs)
{
  if (!XNonLinearDeformer::ReadChunk(ifs)) 
    return false;

  READ_CHECK("low_bound",ifs);
  ifs >> low_bound;
  READ_CHECK("high_bound",ifs);
  ifs >> high_bound;
  READ_CHECK("amplitude",ifs);
  ifs >> amplitude;
  READ_CHECK("wavelength",ifs);
  ifs >> wavelength;
  READ_CHECK("dropoff",ifs);
  ifs >> dropoff;
  READ_CHECK("offset",ifs);
  ifs >> offset;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearSine::WriteChunk(std::ostream& ofs) 
{
  if (!XNonLinearDeformer::WriteChunk(ofs))
    return false;

  ofs << "\tlow_bound " << low_bound << "\n";
  ofs << "\thigh_bound " << high_bound << "\n";
  ofs << "\tamplitude " << amplitude << "\n";
  ofs << "\twavelength " << wavelength << "\n";
  ofs << "\tdropoff " << dropoff << "\n";
  ofs << "\toffset " << offset << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearSine::DoData(XFileIO& io)
{
  DUMPER(XNonLinearSine);

  IO_CHECK( XNonLinearDeformer::DoData(io) );

  IO_CHECK( io.DoData(&low_bound) );
  DPARAM( low_bound );
  IO_CHECK( io.DoData(&high_bound) );
  DPARAM( high_bound );
  IO_CHECK( io.DoData(&amplitude) );
  DPARAM( amplitude );
  IO_CHECK( io.DoData(&wavelength) );
  DPARAM( wavelength );
  IO_CHECK( io.DoData(&dropoff) );
  DPARAM( dropoff );
  IO_CHECK( io.DoData(&offset) );
  DPARAM( offset );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSine::DoCopy(const XBase* rhs)
{
  const XNonLinearSine* cb = rhs->HasFn<XNonLinearSine>();
  XMD_ASSERT(cb);
  low_bound = cb->low_bound;
  high_bound = cb->high_bound;
  amplitude = cb->amplitude;
  wavelength = cb->wavelength;
  dropoff = cb->dropoff;
  offset = cb->offset;
  XNonLinearDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNonLinearSine::GetDataSize() const 
{
  return 6*sizeof(XReal) + XNonLinearDeformer::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearSquash::XNonLinearSquash(XModel* pmod) 
  : XNonLinearDeformer(pmod)
{
  low_bound =-1;
  high_bound =1;
  start_smoothness =0;
  end_smoothness =0;
  max_expand_position =0.5;
  expand =1;
  factor =0;
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearSquash::~XNonLinearSquash()
{
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSquash::GetLowBound() const
{ 
  return low_bound; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSquash::GetHighBound() const 
{ 
  return high_bound; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSquash::GetStartSmoothness() const 
{ 
  return start_smoothness; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSquash::GetEndSmoothness() const 
{ 
  return end_smoothness; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSquash::GetMaxExpandPosition() const 
{ 
  return max_expand_position; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSquash::GetExpand() const 
{ 
  return expand; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearSquash::GetFactor() const 
{ 
  return factor;
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSquash::SetLowBound(const XReal v) 
{ 
  low_bound=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSquash::SetHighBound(const XReal v) 
{ 
  high_bound=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSquash::SetStartSmoothness(const XReal v) 
{ 
  start_smoothness=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSquash::SetEndSmoothness(const XReal v) 
{ 
  end_smoothness=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSquash::SetMaxExpandPosition(const XReal v) 
{ 
  max_expand_position=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSquash::SetExpand(const XReal v) 
{ 
  expand=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSquash::SetFactor(const XReal v) 
{ 
  factor=v; 
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XNonLinearSquash::GetApiType() const
{
  return XFn::NonLinearSquash;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNonLinearSquash::GetFn(XFn::Type type)
{
  if(XFn::NonLinearSquash==type)
    return (XNonLinearSquash*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XNonLinearSquash::GetFn(XFn::Type type) const
{
  if(XFn::NonLinearSquash==type)
    return (const XNonLinearSquash*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearSquash::NodeDeath(XId id)
{
  return XNonLinearDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSquash::PreDelete(XIdSet& extra_nodes)
{
  XNonLinearDeformer::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearSquash::ReadChunk(std::istream& ifs)
{
  if (!XNonLinearDeformer::ReadChunk(ifs)) 
    return false;

  READ_CHECK("low_bound",ifs);
  ifs >> low_bound;
  READ_CHECK("high_bound",ifs);
  ifs >> high_bound;
  READ_CHECK("start_smoothness",ifs);
  ifs >> start_smoothness;
  READ_CHECK("end_smoothness",ifs);
  ifs >> end_smoothness;
  READ_CHECK("max_expand_pos",ifs);
  ifs >> max_expand_position;
  READ_CHECK("expand",ifs);
  ifs >> expand;
  READ_CHECK("factor",ifs);
  ifs >> factor;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearSquash::WriteChunk(std::ostream& ofs) 
{
  if (!XNonLinearDeformer::WriteChunk(ofs))
    return false;


  ofs << "\tlow_bound " << low_bound << "\n";
  ofs << "\thigh_bound " << high_bound << "\n";
  ofs << "\tstart_smoothness " << start_smoothness << "\n";
  ofs << "\tend_smoothness " << end_smoothness << "\n";
  ofs << "\tmax_expand_pos " << max_expand_position << "\n";
  ofs << "\texpand " << expand << "\n";
  ofs << "\tfactor " << factor << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearSquash::DoData(XFileIO& io) 
{
  DUMPER(XNonLinearSquash);

  IO_CHECK( XNonLinearDeformer::DoData(io) );


  IO_CHECK( io.DoData(&low_bound) );
  DPARAM( low_bound );
  IO_CHECK( io.DoData(&high_bound) );
  DPARAM( high_bound );
  IO_CHECK( io.DoData(&start_smoothness) );
  DPARAM( start_smoothness );
  IO_CHECK( io.DoData(&end_smoothness) );
  DPARAM( end_smoothness );
  IO_CHECK( io.DoData(&max_expand_position) );
  DPARAM( max_expand_position );
  IO_CHECK( io.DoData(&expand) );
  DPARAM( expand );
  IO_CHECK( io.DoData(&factor) );
  DPARAM( factor );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearSquash::DoCopy(const XBase* rhs)
{
  const XNonLinearSquash* cb = rhs->HasFn<XNonLinearSquash>();
  XMD_ASSERT(cb);
  low_bound = cb->low_bound;
  high_bound = cb->high_bound;
  start_smoothness = cb->start_smoothness;
  end_smoothness = cb->end_smoothness;
  max_expand_position = cb->max_expand_position;
  expand = cb->expand;
  factor = cb->factor;
  XNonLinearDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNonLinearSquash::GetDataSize() const 
{
  return 7*sizeof(XReal) + XNonLinearDeformer::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearTwist::XNonLinearTwist(XModel* pmod) 
  : XNonLinearDeformer(pmod)
{
  low_bound =-1;
  high_bound =1;
  start_angle =0;
  end_angle =0;
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearTwist::~XNonLinearTwist()
{
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearTwist::GetLowBound() const 
{ 
  return low_bound; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearTwist::GetHighBound() const
{ 
  return high_bound; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearTwist::GetStartAngle() const
{ 
  return start_angle; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearTwist::GetEndAngle() const 
{ 
  return end_angle; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearTwist::SetLowBound(const XReal v)
{ 
  low_bound=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearTwist::SetHighBound(const XReal v) 
{ 
  high_bound=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearTwist::SetStartAngle(const XReal v) 
{ 
  start_angle=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearTwist::SetEndAngle(const XReal v) 
{ 
  end_angle=v; 
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XNonLinearTwist::GetApiType() const
{
  return XFn::NonLinearTwist;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNonLinearTwist::GetFn(XFn::Type type)
{
  if(XFn::NonLinearTwist==type)
    return (XNonLinearTwist*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XNonLinearTwist::GetFn(XFn::Type type) const
{
  if(XFn::NonLinearTwist==type)
    return (const XNonLinearTwist*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearTwist::NodeDeath(XId id)
{
  return XNonLinearDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearTwist::PreDelete(XIdSet& extra_nodes)
{
  XNonLinearDeformer::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearTwist::ReadChunk(std::istream& ifs)
{
  if (!XNonLinearDeformer::ReadChunk(ifs)) 
    return false;

  READ_CHECK("low_bound",ifs);
  ifs >> low_bound;
  READ_CHECK("high_bound",ifs);
  ifs >> high_bound;
  READ_CHECK("start_angle",ifs);
  ifs >> start_angle;
  READ_CHECK("end_angle",ifs);
  ifs >> end_angle;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearTwist::WriteChunk(std::ostream& ofs) 
{
  if (!XNonLinearDeformer::WriteChunk(ofs))
    return false;


  ofs << "\tlow_bound " << low_bound << "\n";
  ofs << "\thigh_bound " << high_bound << "\n";
  ofs << "\tstart_angle " << start_angle << "\n";
  ofs << "\tend_angle " << end_angle << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearTwist::DoData(XFileIO& io)
{
  DUMPER(XNonLinearTwist);

  IO_CHECK( XNonLinearDeformer::DoData(io) );


  IO_CHECK( io.DoData(&low_bound) );
  DPARAM( low_bound );
  IO_CHECK( io.DoData(&high_bound) );
  DPARAM( high_bound );
  IO_CHECK( io.DoData(&start_angle) );
  DPARAM( start_angle );
  IO_CHECK( io.DoData(&end_angle) );
  DPARAM( end_angle );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearTwist::DoCopy(const XBase* rhs)
{
  const XNonLinearTwist* cb = rhs->HasFn<XNonLinearTwist>();
  XMD_ASSERT(cb);
  low_bound = cb->low_bound;
  high_bound = cb->high_bound;
  start_angle = cb->start_angle;
  end_angle = cb->end_angle;
  XNonLinearDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNonLinearTwist::GetDataSize() const
{
  return 4*sizeof(XReal) + XNonLinearDeformer::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearWave::XNonLinearWave(XModel* pmod) 
  : XNonLinearDeformer(pmod)
{
  max_radius =1;
  min_radius =0;
  amplitude =0;
  wavelength =1;
  dropoff =0;
  dropoff_position =0;
  offset =0;
}

//----------------------------------------------------------------------------------------------------------------------
XNonLinearWave::~XNonLinearWave()
{
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearWave::GetMinRadius() const 
{ 
  return min_radius; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearWave::GetMaxRadius() const 
{ 
  return max_radius; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearWave::GetAmplitude() const 
{ 
  return amplitude; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearWave::GetWavelength() const 
{ 
  return wavelength; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearWave::GetDropoff() const 
{ 
  return dropoff; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearWave::GetDropoffPosition() const 
{ 
  return dropoff_position; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XNonLinearWave::GetOffset() const 
{ 
  return offset; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearWave::SetMinRadius(const XReal v) 
{ 
  min_radius=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearWave::SetMaxRadius(const XReal v) 
{ 
  max_radius=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearWave::SetAmplitude(const XReal v) 
{ 
  amplitude=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearWave::SetWavelength(const XReal v) 
{ 
  wavelength=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearWave::SetDropoff(const XReal v) 
{ 
  dropoff=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearWave::SetDropoffPosition(const XReal v) 
{ 
  dropoff_position=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearWave::SetOffset(const XReal v)
{ 
  offset=v; 
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XNonLinearWave::GetApiType() const
{
  return XFn::NonLinearWave;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNonLinearWave::GetFn(XFn::Type type)
{
  if(XFn::NonLinearWave==type)
    return (XNonLinearWave*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XNonLinearWave::GetFn(XFn::Type type) const
{
  if(XFn::NonLinearWave==type)
    return (const XNonLinearWave*)this;
  return XNonLinearDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearWave::NodeDeath(XId id)
{
  return XNonLinearDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearWave::PreDelete(XIdSet& extra_nodes)
{
  XNonLinearDeformer::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearWave::ReadChunk(std::istream& ifs)
{
  if (!XNonLinearDeformer::ReadChunk(ifs)) 
    return false;

  READ_CHECK("max_radius",ifs);
  ifs >> max_radius;
  READ_CHECK("min_radius",ifs);
  ifs >> min_radius;
  READ_CHECK("amplitude",ifs);
  ifs >> amplitude;
  READ_CHECK("wavelength",ifs);
  ifs >> wavelength;
  READ_CHECK("dropoff",ifs);
  ifs >> dropoff;
  READ_CHECK("dropoff_position",ifs);
  ifs >> dropoff_position;
  READ_CHECK("offset",ifs);
  ifs >> offset;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearWave::WriteChunk(std::ostream& ofs) 
{
  if (!XNonLinearDeformer::WriteChunk(ofs))
    return false;

  ofs << "\tmax_radius " << max_radius << "\n";
  ofs << "\tmin_radius " << min_radius << "\n";
  ofs << "\tamplitude " << amplitude << "\n";
  ofs << "\twavelength " << wavelength << "\n";
  ofs << "\tdropoff " << dropoff << "\n";
  ofs << "\tdropoff_position " << dropoff_position << "\n";
  ofs << "\toffset " << offset << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNonLinearWave::DoData(XFileIO& io) 
{
  DUMPER(XNonLinearWave);

  IO_CHECK( XNonLinearDeformer::DoData(io) );

  IO_CHECK( io.DoData(&max_radius) );
  DPARAM( max_radius );
  IO_CHECK( io.DoData(&min_radius) );
  DPARAM( min_radius );
  IO_CHECK( io.DoData(&amplitude) );
  DPARAM( amplitude );
  IO_CHECK( io.DoData(&wavelength) );
  DPARAM( wavelength );
  IO_CHECK( io.DoData(&dropoff) );
  DPARAM( dropoff );
  IO_CHECK( io.DoData(&dropoff_position) );
  DPARAM( dropoff_position );
  IO_CHECK( io.DoData(&offset) );
  DPARAM( offset );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XNonLinearWave::DoCopy(const XBase* rhs)
{
  const XNonLinearWave* cb = rhs->HasFn<XNonLinearWave>();
  XMD_ASSERT(cb);
  max_radius = cb->max_radius;
  min_radius = cb->min_radius;
  amplitude = cb->amplitude;
  wavelength = cb->wavelength;
  dropoff = cb->dropoff;
  dropoff_position = cb->dropoff_position;
  offset = cb->offset;
  XNonLinearDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNonLinearWave::GetDataSize() const
{
  return 7*sizeof(XReal) + XNonLinearDeformer::GetDataSize();
}
}
