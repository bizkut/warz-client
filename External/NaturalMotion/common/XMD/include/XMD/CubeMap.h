//----------------------------------------------------------------------------------------------------------------------
/// \file   CubeMap.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the XCubeMap base class to all environment 
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
/// \brief This class holds cube map data (basically 6 images references, and
///         a transform matrix).
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XCubeMap :
  public XEnvmapBaseTexture
{
  XMD_CHUNK(XCubeMap);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kDynamic     = XEnvmapBaseTexture::kLast+1; // bool
  static const XU32 kLeft        = XEnvmapBaseTexture::kLast+2; // XBase*
  static const XU32 kRight       = XEnvmapBaseTexture::kLast+3; // XBase*
  static const XU32 kTop         = XEnvmapBaseTexture::kLast+4; // XBase*
  static const XU32 kBottom      = XEnvmapBaseTexture::kLast+5; // XBase*
  static const XU32 kFront       = XEnvmapBaseTexture::kLast+6; // XBase*
  static const XU32 kBack        = XEnvmapBaseTexture::kLast+7; // XBase*
  static const XU32 kLast        = kBack;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Cube Map Attributes
  /// \brief  Sets/Gets the params for the cube map texture
  //@{

  /// \brief  returns the texture placement node for the left face of the cube
  /// \return the left texture placement.
  XTexPlacement2D* GetLeftPlacement() const;

  /// \brief  returns the texture placement node for the right face of the cube
  /// \return the right texture placement.
  XTexPlacement2D* GetRightPlacement() const;

  /// \brief  returns the texture placement node for the top face of the cube
  /// \return the top texture placement.
  XTexPlacement2D* GetTopPlacement() const;

  /// \brief  returns the texture placement node for the bottom face of the cube
  /// \return the bottom texture placement.
  XTexPlacement2D* GetBottomPlacement() const;

  /// \brief  returns the texture placement node for the front face of the cube
  /// \return the front texture placement.
  XTexPlacement2D* GetFrontPlacement() const;

  /// \brief  returns the texture placement node for the back face of the cube
  /// \return the back texture placement.
  XTexPlacement2D* GetBackPlacement() const;

  /// \brief  returns the image applied to the left face of the cube
  /// \return the image data
  XImage* GetLeft() const;

  /// \brief  returns the image applied to the right face of the cube
  /// \return the image data
  XImage* GetRight() const;

  /// \brief  returns the image applied to the top face of the cube
  /// \return the image data
  XImage* GetTop() const;

  /// \brief  returns the image applied to the bottom face of the cube
  /// \return the image data
  XImage* GetBottom() const;

  /// \brief  returns the image applied to the front face of the cube
  /// \return the image data
  XImage* GetFront() const;

  /// \brief  returns the image applied to the back face of the cube
  /// \return the image data
  XImage* GetBack() const;

  /// \brief  sets the texture placement node for the left face of the cube.
  /// \param  placement - the left texture placement.
  bool SetLeftPlacement(const XTexPlacement2D* placement);

  /// \brief  sets the texture placement node for the right face of the cube.
  /// \param  placement - the right texture placement.
  bool SetRightPlacement(const XTexPlacement2D* placement);

  /// \brief  sets the texture placement node for the top face of the cube.
  /// \param  placement - the top texture placement.
  bool SetTopPlacement(const XTexPlacement2D* placement);

  /// \brief  sets the texture placement node for the bottom face of the cube.
  /// \param  placement - the bottom texture placement.
  bool SetBottomPlacement(const XTexPlacement2D* placement);

  /// \brief  sets the texture placement node for the front face of the cube.
  /// \param  placement - the front texture placement.
  bool SetFrontPlacement(const XTexPlacement2D* placement);

  /// \brief  sets the texture placement node for the back face of the cube.
  /// \param  placement - the back texture placement.
  bool SetBackPlacement(const XTexPlacement2D* placement);

  /// \brief  queries whether the cube map gets dynamically generated.
  /// \return true if the cube map is dynamically generated, false if the
  ///         cube map always uses the same images.
  bool GetIsDynamic() const;

  /// \brief  sets the flag for dynamic cube map generation.
  /// \param  dynamic - true to dynamically generate the cube map, false 
  ///         to specify fixed images.
  void SetIsDynamic(const bool dynamic);

  /// \brief  \b DEPRECATED sets the texture placement node for the left face of 
  ///         the cube.
  /// \param  placement - the left texture placement.
  /// \deprecated Use XCubeMap::SetLeftPlacement() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XCubeMap::SetLeftPlacement() instead of this function")
  #endif
  inline void SetLeft(const XTexPlacement2D* placement) { SetLeftPlacement(placement); }

  /// \brief  \b DEPRECATED sets the texture placement node for the right face of 
  ///         the cube.
  /// \param  placement - the right texture placement.
  /// \deprecated Use XCubeMap::SetRightPlacement() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XCubeMap::SetRightPlacement() instead of this function")
  #endif
  inline void SetRight(const XTexPlacement2D* placement) { SetRightPlacement(placement); }

  /// \brief  \b DEPRECATED sets the texture placement node for the top face of 
  ///         the cube.
  /// \param  placement - the top texture placement.
  /// \deprecated Use XCubeMap::SetTopPlacement() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XCubeMap::SetTopPlacement() instead of this function")
  #endif
  inline void SetTop(const XTexPlacement2D* placement) { SetTopPlacement(placement); }

  /// \brief  \b DEPRECATED sets the texture placement node for the bottom face of 
  ///         the cube.
  /// \param  placement - the bottom texture placement.
  /// \deprecated Use XCubeMap::SetBottomPlacement() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XCubeMap::SetBottomPlacement() instead of this function")
  #endif
  inline void SetBottom(const XTexPlacement2D* placement) { SetBottomPlacement(placement); }

  /// \brief  \b DEPRECATED sets the texture placement node for the front face of 
  ///         the cube.
  /// \param  placement - the front texture placement.
  /// \deprecated Use XCubeMap::SetFrontPlacement() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XCubeMap::SetFrontPlacement() instead of this function")
  #endif
  inline void SetFront(const XTexPlacement2D* placement) { SetFrontPlacement(placement); }

  /// \brief  \b DEPRECATED sets the texture placement node for the back face of 
  ///         the cube.
  /// \param  placement - the back texture placement.
  /// \deprecated Use XCubeMap::SetBackPlacement() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XCubeMap::SetBackPlacement() instead of this function")
  #endif
  inline void SetBack(const XTexPlacement2D* placement) { SetBackPlacement(placement); }

  /// \brief  \b DEPRECATED queries whether the cube map gets dynamically generated.
  /// \return true if the cube map is dynamically generated, false if the
  ///         cube map always uses the same images.
  /// \deprecated Use XCubeMap::GetIsDynamic() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XCubeMap::GetIsDynamic() instead of this function")
  #endif
  inline bool IsDynamic() const { return GetIsDynamic(); }

  /// \brief  \b DEPRECATED sets the flag for dynamic cube map generation.
  /// \param  dynamic - true to dynamically generate the cube map, false 
  ///         to specify fixed images.
  /// \deprecated Use XCubeMap::SetIsDynamic() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XCubeMap::SetIsDynamic() instead of this function")
  #endif
  inline void SetDynamic(const bool dynamic) { SetIsDynamic(dynamic); }

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  bool m_Dynamic;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::CubeMap, XCubeMap, "cubeMap", "CUBE_MAP");
#endif
}
