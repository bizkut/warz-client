//----------------------------------------------------------------------------------------------------------------------
/// \file BumpMap.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/BumpMap.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include <math.h>

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XFn::Type XBumpMap::GetApiType() const 
{
  return XFn::BumpMap;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBumpMap::NodeDeath(XId id)
{
  return XTexture::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XBumpMap::PreDelete(XIdSet& extra_nodes)
{
  XTexture::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D* XBumpMap::GetPlacement() const
{
  return XTexture::GetPlacement(0);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBumpMap::SetPlacement(const XBase* ptr) 
{
  return XTexture::SetPlacement(ptr,0);
}

//----------------------------------------------------------------------------------------------------------------------
XImage* XBumpMap::GetImage() const
{
  XTexPlacement2D* img = GetPlacement();
  if(img)
    return img->GetImage();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBumpMap::SetImage(const XImage* image) 
{
  (void)image;
  XTexPlacement2D* img = GetPlacement();
  (void)img;
//    if(img)
//    return img->SetImage(image->);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBumpMap::SetImage(const XId id) 
{
  XTexPlacement2D* img = GetPlacement();
  if(img)
    return img->SetImage(id);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XBumpMap::GetBumpDepth() const 
{
  return m_BumpDepth;
}

//----------------------------------------------------------------------------------------------------------------------
void XBumpMap::SetBumpDepth(const XReal depth) 
{
  m_BumpDepth = depth;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBumpMap::WriteChunk(std::ostream& ofs) 
{
  ofs << "\tbump_depth " << m_BumpDepth << "\n"
    << "\ttexture_id " << m_PlacementIDs[0] << "\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBumpMap::ReadChunk(std::istream& ifs) 
{
  READ_CHECK("bump_depth",ifs);
  ifs >> m_BumpDepth;

  READ_CHECK("texture_id",ifs);
  ifs >> m_PlacementIDs[0];

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XBumpMap::XBumpMap(XModel* pMod)
  : XTexture(pMod)
{
  m_PlacementIDs.resize(1);
}

//----------------------------------------------------------------------------------------------------------------------
XBumpMap::~XBumpMap()
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XBumpMap::GetFn(XFn::Type type) 
{
  if(type == XFn::BumpMap) 
  {
    return (XBumpMap*)this;
  }
  return XTexture::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XBumpTexture::GetFn(XFn::Type type) const
{
  if(XFn::BumpMap==type)
    return (const XBumpMap*)this;
  return XTexture::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBumpMap::DoData(XFileIO& io)
{
  DUMPER(XBumpMap);

  IO_CHECK( XTexture::DoData(io) );

  IO_CHECK( io.DoData(&m_BumpDepth) );
  DPARAM( m_BumpDepth );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XBumpMap::DoCopy(const XBase* rhs)
{
  const XBumpMap* cb = rhs->HasFn<XBumpMap>();
  XMD_ASSERT(cb);

  m_BumpDepth = cb->m_BumpDepth;

  XTexture::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBumpMap::GetDataSize() const
{
  return XTexture::GetDataSize() + sizeof(XReal) ;
}
}
