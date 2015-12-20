//----------------------------------------------------------------------------------------------------------------------
/// \file CubeMap.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/CubeMap.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include <math.h>

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XFn::Type XCubeMap::GetApiType() const 
{
  return XFn::CubeMap;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCubeMap::SetLeftPlacement(const XTexPlacement2D* placement)
{
  return SetPlacement(placement,0);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCubeMap::SetRightPlacement(const XTexPlacement2D* placement)
{
  return SetPlacement(placement,1);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCubeMap::SetTopPlacement(const XTexPlacement2D* placement)
{
  return SetPlacement(placement,2);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCubeMap::SetBottomPlacement(const XTexPlacement2D* placement)
{
  return SetPlacement(placement,3);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCubeMap::SetFrontPlacement(const XTexPlacement2D* placement)
{
  return SetPlacement(placement,4);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCubeMap::SetBackPlacement(const XTexPlacement2D* placement)
{
  return SetPlacement(placement,5);
}

//----------------------------------------------------------------------------------------------------------------------
XImage* XCubeMap::GetLeft() const
{
  return GetPlacement(0)->GetImage();
}

//----------------------------------------------------------------------------------------------------------------------
XImage* XCubeMap::GetRight() const
{
  return GetPlacement(1)->GetImage();
}

//----------------------------------------------------------------------------------------------------------------------
XImage* XCubeMap::GetTop() const
{
  return GetPlacement(2)->GetImage();
}

//----------------------------------------------------------------------------------------------------------------------
XImage* XCubeMap::GetBottom() const
{
  return GetPlacement(3)->GetImage();
}

//----------------------------------------------------------------------------------------------------------------------
XImage* XCubeMap::GetFront() const
{
  return GetPlacement(4)->GetImage();
}

//----------------------------------------------------------------------------------------------------------------------
XImage* XCubeMap::GetBack() const
{
  return GetPlacement(5)->GetImage();
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D* XCubeMap::GetLeftPlacement() const
{
  return GetPlacement(0);
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D* XCubeMap::GetRightPlacement() const
{
  return GetPlacement(1);
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D* XCubeMap::GetTopPlacement() const
{
  return GetPlacement(2);
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D* XCubeMap::GetBottomPlacement() const
{
  return GetPlacement(3);
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D* XCubeMap::GetFrontPlacement() const
{
  return GetPlacement(4);
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D* XCubeMap::GetBackPlacement() const
{
  return GetPlacement(5);
}

//----------------------------------------------------------------------------------------------------------------------
void XCubeMap::PreDelete(XIdSet& extra_nodes)
{
  XEnvmapBaseTexture::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCubeMap::NodeDeath(XId id) 
{
  return XEnvmapBaseTexture::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCubeMap::WriteChunk(std::ostream& ofs) 
{
  ofs << "\tdynamic " << m_Dynamic << "\n";
  ofs << "\tplacement " << m_Placement << "\n";
  if(!m_Dynamic)
  {
    ofs << "\tleft "  << m_PlacementIDs[0] << "\n";
    ofs << "\tright "   << m_PlacementIDs[1] << "\n";
    ofs << "\ttop "   << m_PlacementIDs[2] << "\n";
    ofs << "\tbottom "  << m_PlacementIDs[3] << "\n";
    ofs << "\tfront "   << m_PlacementIDs[4] << "\n";
    ofs << "\tback "  << m_PlacementIDs[5] << "\n";
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCubeMap::ReadChunk(std::istream& ifs) 
{
  READ_CHECK("dynamic",ifs);
  ifs >> m_Dynamic;

  READ_CHECK("placement",ifs);
  ifs >> m_Placement;

  if(!m_Dynamic)
  {
    READ_CHECK("left",ifs);
    ifs >> m_PlacementIDs[0];

    READ_CHECK("right",ifs);
    ifs >> m_PlacementIDs[1];

    READ_CHECK("top",ifs);
    ifs >> m_PlacementIDs[2];

    READ_CHECK("bottom",ifs);
    ifs >> m_PlacementIDs[3];

    READ_CHECK("front",ifs);
    ifs >> m_PlacementIDs[4];

    READ_CHECK("back",ifs);
    ifs >> m_PlacementIDs[5];
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCubeMap::DoData(XFileIO& io)
{
  DUMPER(XCubeMap);

  IO_CHECK( XEnvmapBaseTexture::DoData(io) );

  IO_CHECK( io.DoData(&m_Dynamic) );
  DPARAM( m_Dynamic );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XCubeMap::DoCopy(const XBase* rhs)
{
  const XCubeMap* cb = rhs->HasFn<XCubeMap>();
  XMD_ASSERT(cb);

  m_Dynamic = cb->m_Dynamic;

  XEnvmapBaseTexture::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCubeMap::GetDataSize() const
{
  return XEnvmapBaseTexture::GetDataSize() + sizeof(bool) ;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XCubeMap::GetFn(XFn::Type type) 
{
  if(type == XFn::CubeMap) 
  {
    return (XCubeMap*)this;
  }
  return XEnvmapBaseTexture::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XCubeMap::GetFn(XFn::Type type) const
{
  if(XFn::CubeMap==type)
    return (const XCubeMap*)this;
  return XEnvmapBaseTexture::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XCubeMap::XCubeMap(XModel* pmod)
  : XEnvmapBaseTexture(pmod),m_Dynamic(false)
{
  m_PlacementIDs.resize(6);
}

//----------------------------------------------------------------------------------------------------------------------
XCubeMap::~XCubeMap() 
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XCubeMap::GetIsDynamic() const
{
  return m_Dynamic;
}

//----------------------------------------------------------------------------------------------------------------------
void XCubeMap::SetIsDynamic(const bool dynamic)
{
  m_Dynamic = dynamic;
}
}
