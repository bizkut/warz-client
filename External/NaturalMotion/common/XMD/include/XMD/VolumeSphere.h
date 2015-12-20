//----------------------------------------------------------------------------------------------------------------------
/// \file   VolumeSphere.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a volume sphere
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/VolumeObject.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class  XMD::XVolumeSphere
/// \brief  This class holds the data from a render sphere in Maya.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XVolumeSphere :
  public XVolumeObject
{
  XMD_CHUNK(XVolumeSphere);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kRadius = XVolumeObject::kLast+1; // XReal
  static const XU32 kLast   = kRadius;
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  returns the radius of the collision sphere
  /// \return the radius
  XReal GetRadius() const;

  /// \brief  sets the radius of the collision sphere
  /// \param  radius - the radius
  void SetRadius(XReal radius);

private:
#ifndef DOXYGEN_PROCESSING
  XReal m_Radius;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::VolumeSphere, XVolumeSphere, "volumeSphere", "VOLUME_SPHERE");
#endif
}
