//----------------------------------------------------------------------------------------------------------------------
///  \file    CollisionBox.cpp
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/CollisionBox.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XCollisionBox::XCollisionBox(XModel* pmod) 
  : XCollisionObject(pmod), x(1.0f),y(1.0f),z(1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XCollisionBox::~XCollisionBox()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XCollisionBox::GetApiType() const
{
  return XFn::CollisionBox;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XCollisionBox::GetFn(XFn::Type type)
{
  if(XFn::CollisionBox==type)
    return (XCollisionBox*)this;
  return XCollisionObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XCollisionBox::GetFn(XFn::Type type) const
{
  if(XFn::CollisionBox==type)
    return (const XCollisionBox*)this;
  return XCollisionObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionBox::NodeDeath(XId id)
{
  return XCollisionObject::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionBox::PreDelete(XIdSet& extra_nodes)
{
  XCollisionObject::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCollisionBox::GetDataSize() const 
{
  return XCollisionObject::GetDataSize() + 3*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionBox::GetX() const
{
  return x;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionBox::GetY() const
{ 
  return y;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionBox::GetZ() const
{ 
  return z;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionBox::SetX(const XReal v)
{
  x = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionBox::SetY(const XReal v)
{ 
  y = v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionBox::SetZ(const XReal v) 
{ 
  z = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionBox::ReadChunk(std::istream& ifs)
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
bool XCollisionBox::WriteChunk(std::ostream& ofs)
{
  ofs << "\tx_size " << x << "\n";
  ofs << "\ty_size " << y << "\n";
  ofs << "\tz_size " << z << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionBox::DoData(XFileIO& io) 
{
  DUMPER(XCollisionBox);

  IO_CHECK( XCollisionObject::DoData(io) );

  IO_CHECK( io.DoData(&x) );
  DPARAM( x );

  IO_CHECK( io.DoData(&y) );
  DPARAM( y );

  IO_CHECK( io.DoData(&z) );
  DPARAM( z );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionBox::DoCopy(const XBase* rhs)
{
  const XCollisionBox* cb = rhs->HasFn<XCollisionBox>();
  XMD_ASSERT(cb);
  x = cb->x;
  y = cb->y;
  z = cb->z;
  XCollisionObject::DoCopy(cb);
}
}
