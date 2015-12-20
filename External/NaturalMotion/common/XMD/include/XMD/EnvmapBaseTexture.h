//----------------------------------------------------------------------------------------------------------------------
/// \file   EnvmapBaseTexture.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the XEnvmapBaseTexture base class to all environment 
///         mapping effects - i.e. cube and sphere maps
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Texture.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XEnvmapBaseTexture
/// \brief This forms the base class for any environment map type.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XEnvmapBaseTexture
  : public XTexture
{
  XMD_ABSTRACT_CHUNK(XEnvmapBaseTexture);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 k3dPlacement = XTexture::kLast+1; // XReal
  static const XU32 kLast        = k3dPlacement;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Common Environment Map Attributes
  /// \brief  Sets/Gets the 3D texture placement to use for the 
  ///         environment map.
  //@{

  /// \brief  This function returns the 3d placement node for this
  ///     environment map. Simply put, you can position a cube-map
  ///     or sphere map by changing the texture matrix stack.
  ///     The placement node is basically a transform.
  /// \return a pointer to the placement or NULL
  XTexPlacement3D* GetPlacement3D() const;

  /// \brief  sets the 3D placement node for the environment map
  /// \param  obj - the 3D transform
  /// \return true if OK
  bool SetPlacement3D(const XBase* obj);

  /// \brief  \b DEPRECATED This function returns the 3d placement node for this
  ///     environment map. Simply put, you can position a cube-map
  ///     or sphere map by changing the texture matrix stack.
  ///     The placement node is basically a transform.
  /// \return a pointer to the placement or NULL
  /// \deprecated Use XEnvmapBaseTexture::GetPlacement3D() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XEnvmapBaseTexture::GetPlacement3D() instead")
  #endif
  inline XTexPlacement3D* Get3DPlacement() const { return GetPlacement3D(); }

  /// \brief  \b DEPRECATED sets the 3D placement node for the environment map
  /// \param  obj - the 3D transform
  /// \return true if OK
  /// \deprecated Use XEnvmapBaseTexture::SetPlacement3D() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XEnvmapBaseTexture::SetPlacement3D() instead")
  #endif
  inline bool Set3DPlacement(const XBase* obj) { return SetPlacement3D(obj); }

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  /// The ID of the transform that places this Sphere map
  XU32 m_Placement;
#endif
};

#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::EnvMap,XEnvmapBaseTexture,"envMap");
#endif
}
