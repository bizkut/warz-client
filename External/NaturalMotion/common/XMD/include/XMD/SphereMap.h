//----------------------------------------------------------------------------------------------------------------------
/// \file   SphereMap.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the XSphereMap base class to all environment 
///         mapping effects - i.e. cube and sphere maps
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/EnvmapBaseTexture.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XSphereMap
/// \brief This class holds a sphere map
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XSphereMap 
  : public XEnvmapBaseTexture
{
  XMD_CHUNK(XSphereMap);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kPlacement = XEnvmapBaseTexture::kLast+1; // XBase*
  static const XU32 kLast      = kPlacement;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Sphere Map Attributes
  /// \brief  Sets/Gets the params for the sphere map texture
  //@{

  /// \brief  returns the texture placement node that is determines
  ///         the mapping information used to map the texture onto the sphere.
  /// \return the texture placement node
  XTexPlacement2D* GetPlacement() const;

  /// \brief  sets the texture placement node to use with the sphere map.
  /// \param  placement - the 2d texture placement info node
  /// \return true if successful.
  bool SetPlacement(const XBase* placement);

  /// \brief  This function returns a pointer to the requested image
  /// \return a pointer to the image
  XImage* GetImage() const;

  /// \brief  This function sets the image to use for the specified image slot.
  /// \param  img - pointer to the image
  /// \return true if Set OK
  bool SetImage(const XImage* img);

  /// \brief  This function sets the image to use for the specified image slot.
  /// \param  idx - the internal image index to use
  /// \return true if Set OK
  bool SetImage(XId idx);

  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::SphereMap, XSphereMap, "sphereMap", "SPHERE_MAP");
#endif
}
