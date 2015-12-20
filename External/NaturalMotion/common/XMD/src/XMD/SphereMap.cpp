//----------------------------------------------------------------------------------------------------------------------
/// \file SphereMap.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/SphereMap.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include <math.h>

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XFn::Type XSphereMap::GetApiType() const 
{
  return XFn::SphereMap;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSphereMap::WriteChunk(std::ostream& ofs) 
{
  ofs << "\timage "   << m_PlacementIDs[0] << "\n";
  ofs << "\tplacement " << m_Placement << "\n";
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSphereMap::ReadChunk(std::istream& ifs)
{
  READ_CHECK("image",ifs);
  ifs >> m_PlacementIDs[0];

  READ_CHECK("placement",ifs);
  ifs >> m_Placement;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XSphereMap::PreDelete(XIdSet& extra_nodes)
{
  XEnvmapBaseTexture::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XSphereMap::NodeDeath(XId id) 
{
  return XEnvmapBaseTexture::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XSphereMap::GetFn(XFn::Type type) 
{
  if(type == XFn::SphereMap)
  {
    return (XSphereMap*)this;
  }
  return XEnvmapBaseTexture::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XSphereMap::GetFn(XFn::Type type) const
{
  if(XFn::SphereMap==type)
    return (const XSphereMap*)this;
  return XEnvmapBaseTexture::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XSphereMap::DoCopy(const XBase* rhs)
{
  const XSphereMap* cb = rhs->HasFn<XSphereMap>();
  XMD_ASSERT(cb);
  XEnvmapBaseTexture::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XSphereMap::XSphereMap(XModel* pmod)
  : XEnvmapBaseTexture(pmod)
{
  m_PlacementIDs.resize(1);
}

//----------------------------------------------------------------------------------------------------------------------
XSphereMap::~XSphereMap()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XSphereMap::DoData(XFileIO& io)
{
  DUMPER(XSphereMap);

  IO_CHECK( XEnvmapBaseTexture::DoData(io) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSphereMap::GetDataSize() const
{
  return XEnvmapBaseTexture::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D* XSphereMap::GetPlacement() const
{
  return XTexture::GetPlacement(0);
}

//----------------------------------------------------------------------------------------------------------------------
bool XSphereMap::SetPlacement(const XBase* placement)
{
  const XTexPlacement2D* place = placement ? placement->HasFn<XTexPlacement2D>() : 0;
  return XTexture::SetPlacement(place,0);
}

//----------------------------------------------------------------------------------------------------------------------
XImage* XSphereMap::GetImage() const
{
  XTexPlacement2D* place = GetPlacement();
  if (place)
  {
    return place->GetImage();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSphereMap::SetImage( const XId idx )
{
  XTexPlacement2D* place = GetPlacement();
  if (place)
  {
    return place->SetImage(idx);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSphereMap::SetImage( const XImage* img )
{
  XTexPlacement2D* place = GetPlacement();
  if (place)
  {
    /// \todo implement
    (void)img;
    // return place->SetImage(img->);
  }
  return false;
}
}
