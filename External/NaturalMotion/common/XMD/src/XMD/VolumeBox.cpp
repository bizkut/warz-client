//----------------------------------------------------------------------------------------------------------------------
/// \file VolumeBox.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/VolumeBox.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XVolumeBox::XVolumeBox(XModel* pmod) 
  : XVolumeObject(pmod), x(1.0f),y(1.0f),z(1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XVolumeBox::~XVolumeBox()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XVolumeBox::GetApiType() const
{
  return XFn::VolumeBox;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XVolumeBox::GetDataSize() const 
{
  return XVolumeObject::GetDataSize() + 3*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XVolumeBox::GetFn(XFn::Type type)
{
  if(XFn::VolumeBox==type)
    return (XVolumeBox*)this;
  return XVolumeObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XVolumeBox::GetFn(XFn::Type type) const
{
  if(XFn::VolumeBox==type)
    return (const XVolumeBox*)this;
  return XVolumeObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeBox::NodeDeath(XId id)
{
  return XVolumeObject::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeBox::PreDelete(XIdSet& extra_nodes)
{
  XVolumeObject::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeBox::GetX() const
{
  return x;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeBox::GetY() const
{ 
  return y;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeBox::GetZ() const
{ 
  return z;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeBox::SetX(const XReal v)
{
  x = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeBox::SetY(const XReal v)
{ 
  y = v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeBox::SetZ(const XReal v) 
{ 
  z = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeBox::ReadChunk(std::istream& ifs)
{
  READ_CHECK("x_size",ifs);
  ifs >> x;

  READ_CHECK("y_size",ifs);
  ifs >> y;

  READ_CHECK("z_size",ifs);
  ifs >> z;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeBox::WriteChunk(std::ostream& ofs)
{
  ofs << "\tx_size " << x << "\n";
  ofs << "\ty_size " << y << "\n";
  ofs << "\tz_size " << z << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeBox::DoData(XFileIO& io) 
{
  DUMPER(XVolumeBox);

  IO_CHECK( XVolumeObject::DoData(io) );

  IO_CHECK( io.DoData(&x) );
  DPARAM( x );

  IO_CHECK( io.DoData(&y) );
  DPARAM( y );

  IO_CHECK( io.DoData(&z) );
  DPARAM( z );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeBox::DoCopy(const XBase* rhs)
{
  const XVolumeBox* cb = rhs->HasFn<XVolumeBox>();
  XMD_ASSERT(cb);
  x = cb->x;
  y = cb->y;
  z = cb->z;
  XVolumeObject::DoCopy(cb);
}
}
