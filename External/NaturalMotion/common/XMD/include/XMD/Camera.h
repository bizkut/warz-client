//----------------------------------------------------------------------------------------------------------------------
/// \file   Camera.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This defines a camera type
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Shape.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  A class to define a camera
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XCamera 
  : public XShape
{
  XMD_CHUNK(XCamera);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kAspect           = XShape::kLast+1; // XReal
  static const XU32 kNear             = XShape::kLast+2; // XReal
  static const XU32 kFar              = XShape::kLast+3; // XReal
  static const XU32 kHorizontalAspect = XShape::kLast+4; // XReal
  static const XU32 kVerticalAspect   = XShape::kLast+5; // XReal
  static const XU32 kLast             = kVerticalAspect;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Camera Params
  /// \brief  Sets/Gets the params for the XCamera
  //@{

  /// \brief  returns the aspect ratio of the camera
  /// \return  the cameras aspect ratio
  XReal GetAspect() const;

  /// \brief  returns the near viewing plane distance for the camera.
  /// \return the near viewing distance
  XReal GetNear() const;

  /// \brief  returns the far viewing distance of the plane
  /// \return the far viewing plane distance
  XReal GetFar() const;

  /// \brief  returns the horizontal aspect ratio of the camera
  /// \return the horizontal aspect
  XReal GetHorizontalAspect() const;

  /// \brief  returns the viewing aspect ratio
  /// \return returns the vertical aspect
  XReal GetVerticalAspect() const;

  /// \brief  sets the aspect ratio of the camera
  /// \param  aspect - the new aspect ratio
  void SetAspect(XReal aspect);

  /// \brief  sets the near viewing plane distance of the camera
  /// \param  near_ - the near viewing plane distance
  void SetNear(XReal near_);

  /// \brief  sets the far viewing plane distance
  /// \param  far_ - the viewing plane distance
  void SetFar(XReal far_);

  /// \brief  sets the horizontal aspect ratio of the camera
  /// \param  aspect_ - the new horizontal aspect ratio
  void SetHorizontalAspect(XReal aspect_ );

  /// \brief  sets the vertical aspect of the camera
  /// \param  aspect_ - the new vertical aspect ratio
  void SetVerticalAspect(XReal aspect_);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the camera's aspect ratio
  XReal m_Aspect;
  /// the near viewing plane distance
  XReal m_Near;
  /// the far viewing plane distance
  XReal m_Far;
  /// the horizontal aspect
  XReal m_HorizAspect;
  /// the vertical aspect
  XReal m_VertAspect;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Camera, XCamera, "camera", "CAMERA");
#endif
}
