//----------------------------------------------------------------------------------------------------------------------
/// \file BaseLattice.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/BaseLattice.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XBaseLattice::XBaseLattice(XModel* pmod) 
  : XShape(pmod),mXSize(1.0f),mYSize(1.0f),
  mZSize(1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XBaseLattice::~XBaseLattice()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XBaseLattice::GetApiType() const
{
  return XFn::BaseLattice;
}

//----------------------------------------------------------------------------------------------------------------------
void XBaseLattice::DoCopy(const XBase* rhs)
{
  const XBaseLattice* cb = rhs->HasFn<XBaseLattice>();
  XMD_ASSERT(cb);
  XShape::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBaseLattice::GetDataSize() const 
{
  return XShape::GetDataSize() + 3*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XBaseLattice::GetFn(XFn::Type type)
{
  if(XFn::BaseLattice==type)
    return (XBaseLattice*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XBaseLattice::GetFn(XFn::Type type) const
{
  if(XFn::BaseLattice==type)
    return (const XBaseLattice*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XBaseLattice::PreDelete(XIdSet& extra_nodes)
{
  XShape::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseLattice::NodeDeath(XId id)
{
  return XShape::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseLattice::ReadChunk(std::istream& ifs)
{
  if (GetModel()->GetInputVersion()>=4)
  {
    READ_CHECK("x_size",ifs);
    ifs >> mXSize;
    READ_CHECK("y_size",ifs);
    ifs >> mYSize;
    READ_CHECK("z_size",ifs);
    ifs >> mZSize;
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseLattice::WriteChunk(std::ostream& ofs)
{
  if (GetModel()->GetInputVersion()>=4)
  {
    ofs << "\tx_size " << mXSize << "\n";
    ofs << "\ty_size " << mXSize << "\n";
    ofs << "\tz_size " << mXSize << "\n";
  }
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseLattice::DoData(XFileIO& io) 
{
  CHECK_IO( XShape::DoData(io) );

  if (GetModel()->GetInputVersion()>=4)
  {
    CHECK_IO( io.DoData(&mXSize) );
    CHECK_IO( io.DoData(&mYSize) );
    CHECK_IO( io.DoData(&mZSize) );
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XBaseLattice::GetXSize() const
{
  return mXSize;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XBaseLattice::GetYSize() const
{
  return mYSize;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XBaseLattice::GetZSize() const
{
  return mZSize;
}

//----------------------------------------------------------------------------------------------------------------------
void XBaseLattice::SetXSize(const XReal x)
{
  mXSize = x;
}

//----------------------------------------------------------------------------------------------------------------------
void XBaseLattice::SetYSize(const XReal y)
{
  mYSize = y;
}

//----------------------------------------------------------------------------------------------------------------------
void XBaseLattice::SetZSize(const XReal z)
{
  mZSize = z;
}
}
