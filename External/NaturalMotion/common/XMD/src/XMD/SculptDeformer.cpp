//----------------------------------------------------------------------------------------------------------------------
/// \file SculptDeformer.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/SculptDeformer.h"
#include "XMD/Model.h"
#include "XMD/VolumeSphere.h"
#include "XMD/Locator.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XSculptDeformer::XSculptDeformer(XModel* pmod) 
  : XDeformer(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XSculptDeformer::~XSculptDeformer(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XSculptDeformer::GetApiType() const
{
  return XFn::SculptDeformer;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XSculptDeformer::GetFn(XFn::Type type)
{
  if(XFn::SculptDeformer==type)
    return (XSculptDeformer*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XSculptDeformer::GetFn(XFn::Type type) const
{
  if(XFn::SculptDeformer==type)
    return (const XSculptDeformer*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XSculptDeformer::NodeDeath(XId id)
{
  return XDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XSculptDeformer::PreDelete(XIdSet& extra_nodes)
{
  XDeformer::PreDelete(extra_nodes);

  const XDeletePolicy& dp = GetModel()->GetDeletePolicy();

  if ( dp.IsEnabled(XDeletePolicy::RelatedDeformerInputs) ) 
  {
    if( sculpt_object )
    {
      extra_nodes.insert(sculpt_object);
    }
    if( start_object )
    {
      extra_nodes.insert(start_object);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XSculptDeformer::ReadChunk(std::istream& ifs)
{  
  if (!XDeformer::ReadChunk(ifs)) 
    return false;


  READ_CHECK("mode",ifs);
  ifs >> mode;
  READ_CHECK("inside_mode",ifs);
  ifs >> inside_mode;
  READ_CHECK("maximum_displacement",ifs);
  ifs >> maximum_displacement;
  READ_CHECK("dropoff_type",ifs);
  ifs >> dropoff_type;
  READ_CHECK("dropoff_distance",ifs);
  ifs >> dropoff_distance;
  READ_CHECK("start_position",ifs);
  ifs >> start_position;
  READ_CHECK("sculpt_object",ifs);
  ifs >> sculpt_object;
  READ_CHECK("start_object",ifs);
  ifs >> start_object;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XSculptDeformer::WriteChunk(std::ostream& ofs) 
{
  if (!m_AffectedGeometry.size()) 
  {
    return true;
  }

  if (!XDeformer::WriteChunk(ofs))
    return false;

  ofs << "\tmode " << mode << "\n";
  ofs << "\tinside_mode " << inside_mode << "\n";
  ofs << "\tmaximum_displacement " << maximum_displacement << "\n";
  ofs << "\tdropoff_type " << dropoff_type << "\n";
  ofs << "\tdropoff_distance " << dropoff_distance << "\n";
  ofs << "\tstart_position " << start_position << "\n";
  ofs << "\tsculpt_object " << sculpt_object << "\n";
  ofs << "\tstart_object " << start_object << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XSculptDeformer::DoData(XFileIO& io) 
{
  DUMPER(XSculptDeformer);

  IO_CHECK( XDeformer::DoData(io) );

  IO_CHECK( io.DoData(&mode) );
  IO_CHECK( io.DoData(&inside_mode) );
  IO_CHECK( io.DoData(&maximum_displacement) );
  IO_CHECK( io.DoData(&dropoff_type) );
  IO_CHECK( io.DoData(&dropoff_distance) );
  IO_CHECK( io.DoData(&start_position) );
  IO_CHECK( io.DoData(&sculpt_object) );
  IO_CHECK( io.DoData(&start_object) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XSculptDeformer::DoCopy(const XBase* rhs)
{
  const XSculptDeformer* cb = rhs->HasFn<XSculptDeformer>();
  XMD_ASSERT(cb);
  mode = cb->mode;
  inside_mode = cb->inside_mode;
  maximum_displacement = cb->maximum_displacement;
  dropoff_type = cb->dropoff_type;
  dropoff_distance = cb->dropoff_distance;
  start_position = cb->start_position;
  sculpt_object = cb->sculpt_object;
  start_object = cb->start_object;
  XDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSculptDeformer::GetDataSize() const 
{
  return 3*sizeof(XU32) + 5*sizeof(XReal) + 2*sizeof(XId) + XDeformer::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XSculptDeformer::SetSculptObject(const XBase* obj)
{
  if (!obj)
  {
    sculpt_object = 0;
    return true;
  }

  XMD_ASSERT( (obj) && IsValidObj(obj) );
  if(obj)
  {
    const XVolumeSphere* loc = obj->HasFn<XVolumeSphere>();
    if (loc)
    {
      sculpt_object = loc->GetID();
    }
    else
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError,
        "Sculpt object for a sculpt deformer needs to be of type XVolumeSphere");

      // If you assert here, you've tried to pass an incorrect type 
      // as the sculpt object. It should be XVolumeSphere
      XMD_ASSERT(0);
    }
  }
  return obj !=0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSculptDeformer::SetStartObject(const XBase* obj)
{
  if (!obj)
  {
    start_object = 0;
    return true;
  }

  XMD_ASSERT( (obj) && IsValidObj(obj) );
  if(obj)
  {
    const XShape* loc = obj->HasFn<XShape>();
    if (loc)
    {
      start_object = loc->GetID();
    }
    else
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError,"Locator for a start object needs to be of type XLocator");

      // If you assert here, you've tried to pass an incorrect type 
      // as the sculpt object. It should be XLocator
      XMD_ASSERT(0);
    }
  }
  return obj !=0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XSculptDeformer::GetSculptObject() const
{
  return m_pModel->FindNode(sculpt_object);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XSculptDeformer::GetStartObject() const
{
  return m_pModel->FindNode(start_object);
}

//----------------------------------------------------------------------------------------------------------------------
void XSculptDeformer::SetMode(const XU32 v)  
{ 
  mode=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XSculptDeformer::SetInsideMode(const XU32 v)  
{ 
  inside_mode=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XSculptDeformer::SetMaxDisplacement(const XReal v)  
{ 
  maximum_displacement=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XSculptDeformer::SetDropoffType(const XU32 v)  
{ 
  dropoff_type=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XSculptDeformer::SetDropoffDistance(const XReal v)
{ 
  dropoff_distance=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XSculptDeformer::SetStartPosition(const XVector3& v)  
{
  start_position = v; 
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSculptDeformer::GetMode() const 
{ 
  return mode; 
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSculptDeformer::GetInsideMode() const 
{ 
  return inside_mode; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XSculptDeformer::GetMaxDisplacement() const 
{ 
  return maximum_displacement; 
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSculptDeformer::GetDropoffType() const 
{ 
  return dropoff_type; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XSculptDeformer::GetDropoffDistance() const 
{ 
  return dropoff_distance; 
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XSculptDeformer::GetStartPosition() const 
{
  return start_position; 
}
}
