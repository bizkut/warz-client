//----------------------------------------------------------------------------------------------------------------------
/// \file   NonLinearDeformer.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines all the non linear deformer types
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Deformer.h"  
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
#ifndef DOXYGEN_PROCESSING
// forward declaration
class XMD_EXPORT XBone;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XNonLinearDeformer
/// \brief This class holds the data for a simple non-linear deformation
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XNonLinearDeformer
  : public XDeformer
{
  XMD_ABSTRACT_CHUNK(XNonLinearDeformer);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kHandle = XDeformer::kLast+1; // XBase*
  static const XU32 kLast   = kHandle;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   NonLinear Deformer Handle
  /// \brief  All non linear deformers are driven by a specific locator
  ///         type that defines the orientation and extents of the 
  ///         deformation
  //@{

  /// \brief returns the transform around which the non-linear deformation 
  ///        takes place. In maya this is basically the transform for the 
  ///        non linear deformation handle.
  /// \return the transform for the handle.
  XBone* GetHandle() const;

  /// \brief  This function sets the handle of the none linear deformation handle. 
  /// \param  hand  - the transformation handle 
  /// \return true if OK
  bool SetHandle(const XBase* hand);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING

  /// the ID of the deformer location in space around which the deformation occurs
  XId handle;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::NonLinearDeformer, XNonLinearDeformer, "nonLinear");
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XNonLinearBend
/// \brief data storage for a simple non linear bend deformer
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XNonLinearBend
  : public XNonLinearDeformer
{
  XMD_CHUNK(XNonLinearBend);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLowBound  = XNonLinearDeformer::kLast+1; // XReal
  static const XU32 kHighBound = XNonLinearDeformer::kLast+2; // XReal
  static const XU32 kCurvature = XNonLinearDeformer::kLast+3; // XReal
  static const XU32 kLast      = kCurvature;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   NonLinearBend Params
  /// \brief  Sets/Gets the params for the XNonLinearBend deformer
  //@{

  /// \brief  returns the low bound for the bend deformer
  /// \return the low bound
  XReal GetLowBound() const;

  /// \brief  returns the high bound for the bend deformer
  /// \return the high bound
  XReal GetHighBound() const;

  /// \brief  returns the curvature of the bend deformer
  /// \return the curvature amount
  XReal GetCurvature() const;

  /// \brief  sets the low bound for the bend deformer
  /// \param  v -  the low bound
  void SetLowBound(XReal v);

  /// \brief  sets the high bound for the bend deformer
  /// \param  v -  the high bound
  void SetHighBound(XReal v);

  /// \brief  sets the curvature of the bend deformer
  /// \param  v -   the curvature amount
  void SetCurvature(XReal v);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING

  /// the low bound for the bend deformer
  XReal low_bound;
  /// the high bound for the bend deformer
  XReal high_bound;
  /// the curvature of the bend deformation
  XReal curvature;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::NonLinearBend, XNonLinearBend, "nonLinearBend", "NONLINEAR_BEND");
#endif


//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XNonLinearFlare
/// \brief data storage for a simple non linear flare deformer
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XNonLinearFlare
  : public XNonLinearDeformer
{
  XMD_CHUNK(XNonLinearFlare);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLowBound    = XNonLinearDeformer::kLast+1; // XReal
  static const XU32 kHighBound   = XNonLinearDeformer::kLast+2; // XReal
  static const XU32 kStartFlareX = XNonLinearDeformer::kLast+3; // XReal
  static const XU32 kStartFlareZ = XNonLinearDeformer::kLast+4; // XReal
  static const XU32 kEndFlareX   = XNonLinearDeformer::kLast+5; // XReal
  static const XU32 kEndFlareZ   = XNonLinearDeformer::kLast+6; // XReal
  static const XU32 kCurve       = XNonLinearDeformer::kLast+7; // XReal
  static const XU32 kLast        = kCurve;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   NonLinearFlare Params
  /// \brief  Sets/Gets the params for the XNonLinearFlare deformer
  //@{

  /// \brief  returns the low bound for the flare deformer
  /// \return the low bound
  XReal GetLowBound() const;

  /// \brief  returns the high bound for the flare deformer
  /// \return the high bound
  XReal GetHighBound() const;

  /// \brief  returns the start flare X factor of the flare deformer
  /// \return the start flare X factor amount
  XReal GetStartFlareX() const;

  /// \brief  returns the start flare Z factor of the flare deformer
  /// \return the start flare Z factor amount
  XReal GetStartFlareZ() const;

  /// \brief  returns the end flare X factor of the flare deformer
  /// \return the end flare X factor amount
  XReal GetEndFlareX() const;

  /// \brief  returns the end flare Z factor of the flare deformer
  /// \return the end flare Z factor amount
  XReal GetEndFlareZ() const;

  /// \brief  returns the curvature of the flare deformer
  /// \return the curvature amount
  XReal GetCurve() const;

  /// \brief  sets the low bound for the flare deformer
  /// \param  v - the low bound
  void SetLowBound(XReal v);

  /// \brief  sets the high bound for the flare deformer
  /// \param  v - the high bound
  void SetHighBound(XReal v);

  /// \brief  sets the start flare X factor for the flare deformer
  /// \param  v - the start flare X factor
  void SetStartFlareX(XReal v);

  /// \brief  sets the start flare Z factor for the flare deformer
  /// \param  v - the start flare Z factor
  void SetStartFlareZ(XReal v);

  /// \brief  sets the end flare X factor for the flare deformer
  /// \param  v - the end flare X factor
  void SetEndFlareX(XReal v);

  /// \brief  sets the end flare Z factor for the flare deformer
  /// \param  v - the end flare Z factor
  void SetEndFlareZ(XReal v);

  /// \brief  sets the curvature of the flare deformer
  /// \param  v - the curvature amount
  void SetCurve(XReal v);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING
  XReal low_bound;
  XReal high_bound;
  XReal start_flareX;
  XReal start_flareZ;
  XReal end_flareX;
  XReal end_flareZ;
  XReal curve;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::NonLinearFlare,XNonLinearFlare,"nonLinearFlare","NONLINEAR_FLARE");
#endif


//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XNonLinearSine
/// \brief data storage for a simple non linear sine deformer
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XNonLinearSine
  : public XNonLinearDeformer
{
  XMD_CHUNK(XNonLinearSine);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLowBound    = XNonLinearDeformer::kLast+1; // XReal
  static const XU32 kHighBound   = XNonLinearDeformer::kLast+2; // XReal
  static const XU32 kAmplitude   = XNonLinearDeformer::kLast+3; // XReal
  static const XU32 kWavelength  = XNonLinearDeformer::kLast+4; // XReal
  static const XU32 kDropoff     = XNonLinearDeformer::kLast+5; // XReal
  static const XU32 kOffset      = XNonLinearDeformer::kLast+6; // XReal
  static const XU32 kLast        = kOffset;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   NonLinearSine Params
  /// \brief  Sets/Gets the params for the XNonLinearSine deformer
  //@{

  /// \brief  returns the low bound for the sine deformer
  /// \return the low bound
  XReal GetLowBound() const;

  /// \brief  returns the high bound for the sine deformer
  /// \return the high bound
  XReal GetHighBound() const;

  /// \brief  returns the amplitude of the sine deformer
  /// \return the amplitude amount
  XReal GetAmplitude() const;

  /// \brief  returns the wavelength of the sine deformer
  /// \return the wavelength amount
  XReal GetWavelength() const;

  /// \brief  returns the dropoff of the sine deformer
  /// \return the dropoff amount
  XReal GetDropoff() const;

  /// \brief  returns the offset of the sine deformer
  /// \return the offset amount
  XReal GetOffset() const;

  /// \brief  sets the low bound for the sine deformer
  /// \param  v -  the low bound
  void SetLowBound(XReal v);

  /// \brief  sets the high bound for the sine deformer
  /// \param  v -  the high bound
  void SetHighBound(XReal v);

  /// \brief  sets the amplitude for the sine deformer
  /// \param  v -  the amplitude
  void SetAmplitude(XReal v);

  /// \brief  sets the wavelength for the sine deformer
  /// \param  v -  the wavelength
  void SetWavelength(XReal v);

  /// \brief  sets the dropoff for the sine deformer
  /// \param  v -  the dropoff
  void SetDropoff(XReal v);

  /// \brief  sets the offset for the sine deformer
  /// \param  v -  the offset
  void SetOffset(XReal v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal low_bound;
  XReal high_bound;
  XReal amplitude;
  XReal wavelength;
  XReal dropoff;
  XReal offset;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::NonLinearSine, XNonLinearSine, "nonLinearSine", "NONLINEAR_SINE");
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XNonLinearSquash
/// \brief data storage for a simple non linear squash deformer
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XNonLinearSquash
  : public XNonLinearDeformer
{
  XMD_CHUNK(XNonLinearSquash);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLowBound          = XNonLinearDeformer::kLast+1; // XReal
  static const XU32 kHighBound         = XNonLinearDeformer::kLast+2; // XReal
  static const XU32 kStartSmoothness   = XNonLinearDeformer::kLast+3; // XReal
  static const XU32 kEndSmoothness     = XNonLinearDeformer::kLast+4; // XReal
  static const XU32 kExpand            = XNonLinearDeformer::kLast+5; // XReal
  static const XU32 kFactor            = XNonLinearDeformer::kLast+6; // XReal
  static const XU32 kMaxExpandPosition = XNonLinearDeformer::kLast+7; // XReal
  static const XU32 kLast              = kMaxExpandPosition;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   NonLinearSquash Params
  /// \brief  Sets/Gets the params for the XNonLinearSquash deformer
  //@{

  /// \brief  returns the low bound for the squash deformer
  /// \return the low bound
  XReal GetLowBound() const;

  /// \brief  returns the high bound for the squash deformer
  /// \return the high bound
  XReal GetHighBound() const;

  /// \brief  returns the start smoothness of the squash deformer
  /// \return the start smoothness amount
  XReal GetStartSmoothness() const;

  /// \brief  returns the end smoothness of the squash deformer
  /// \return the end smoothness amount
  XReal GetEndSmoothness() const;

  /// \brief  returns the maximum expand position of the squash deformer
  /// \return the maximum expand position amount
  XReal GetMaxExpandPosition() const;

  /// \brief  returns the expand amount of the squash deformer
  /// \return the expand amount amount
  XReal GetExpand() const;

  /// \brief  returns the factor of the squash deformer
  /// \return the factor amount
  XReal GetFactor() const;

  /// \brief  sets the low bound for the squash deformer
  /// \param  v -  the low bound
  void SetLowBound(XReal v);

  /// \brief  sets the high bound for the squash deformer
  /// \param  v -  the high bound
  void SetHighBound(XReal v);

  /// \brief  sets the start smoothness for the squash deformer
  /// \param  v -  the start smoothness
  void SetStartSmoothness(XReal v);

  /// \brief  sets the end smoothness for the squash deformer
  /// \param  v -  the end smoothness
  void SetEndSmoothness(XReal v);

  /// \brief  sets the maximum expand position for the squash deformer
  /// \param  v -  the maximum expand position
  void SetMaxExpandPosition(XReal v);

  /// \brief  sets the expand amount for the squash deformer
  /// \param  v -  the expand amount
  void SetExpand(XReal v);

  /// \brief  sets the factor for the squash deformer
  /// \param  v -  the factor
  void SetFactor(XReal v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal low_bound;
  XReal high_bound;
  XReal start_smoothness;
  XReal end_smoothness;
  XReal max_expand_position;
  XReal expand;
  XReal factor;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::NonLinearSquash, XNonLinearSquash, "nonLinearSquash", "NONLINEAR_SQUASH");
#endif

//---------------------------------------------------------------------------------------------------------------------- 
/// \class XMD::XNonLinearTwist
/// \brief data storage for a simple non linear twist deformer
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XNonLinearTwist
  : public XNonLinearDeformer
{
  XMD_CHUNK(XNonLinearTwist);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLowBound   = XNonLinearDeformer::kLast+1; // XReal
  static const XU32 kHighBound  = XNonLinearDeformer::kLast+2; // XReal
  static const XU32 kStartAngle = XNonLinearDeformer::kLast+3; // XReal
  static const XU32 kEndAngle   = XNonLinearDeformer::kLast+4; // XReal
  static const XU32 kLast       = kEndAngle;
  //--------------------------------------------------------------------------------------------------------------------


  /// \name   NonLinearTwist Params
  /// \brief  Sets/Gets the params for the XNonLinearTwist deformer
  //@{

  /// \brief  returns the low bound for the twist deformer
  /// \return the low bound
  XReal GetLowBound() const;

  /// \brief  returns the high bound for the twist deformer
  /// \return the high bound
  XReal GetHighBound() const;

  /// \brief  returns the start angle of the twist deformer
  /// \return the start angle
  XReal GetStartAngle() const;

  /// \brief  returns the end angle of the twist deformer
  /// \return the end angle amount
  XReal GetEndAngle() const;

  /// \brief  sets the low bound for the twist deformer
  /// \param  v -  the low bound
  void SetLowBound(XReal v);

  /// \brief  sets the high bound for the twist deformer
  /// \param  v -  the high bound
  void SetHighBound(XReal v);

  /// \brief  sets the start angle for the twist deformer
  /// \param  v -  the start angle
  void SetStartAngle(XReal v);

  /// \brief  sets the end angle for the twist deformer
  /// \param  v -  the end angle
  void SetEndAngle(XReal v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal low_bound;
  XReal high_bound;
  XReal start_angle;
  XReal end_angle;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::NonLinearTwist, XNonLinearTwist, "nonLinearTwist", "NONLINEAR_TWIST");
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \brief  data storage for a simple non linear wave deformer
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XNonLinearWave
  : public XNonLinearDeformer
{
  XMD_CHUNK(XNonLinearWave);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kMinRadius       = XNonLinearDeformer::kLast+1; // XReal
  static const XU32 kMaxRadius       = XNonLinearDeformer::kLast+2; // XReal
  static const XU32 kAmplitude       = XNonLinearDeformer::kLast+3; // XReal
  static const XU32 kWavelength      = XNonLinearDeformer::kLast+4; // XReal
  static const XU32 kDropoff         = XNonLinearDeformer::kLast+5; // XReal
  static const XU32 kDropoffPosition = XNonLinearDeformer::kLast+6; // XReal
  static const XU32 kOffset          = XNonLinearDeformer::kLast+7; // XReal
  static const XU32 kLast            = kOffset;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   NonLinearWave Params
  /// \brief  Sets/Gets the params for the XNonLinearWave deformer
  //@{

  /// \brief  returns the low bound for the wave deformer
  /// \return the low bound
  XReal GetMinRadius() const;

  /// \brief  returns the high bound for the wave deformer
  /// \return the high bound
  XReal GetMaxRadius() const;

  /// \brief  returns the amplitude of the wave deformer
  /// \return the amplitude amount
  XReal GetAmplitude() const;

  /// \brief  returns the wavelength of the wave deformer
  /// \return the wavelength amount
  XReal GetWavelength() const;

  /// \brief  returns the dropoff of the wave deformer
  /// \return the dropoff amount
  XReal GetDropoff() const;

  /// \brief  returns the dropoff position of the wave deformer
  /// \return the dropoff position
  XReal GetDropoffPosition() const;

  /// \brief  returns the offset of the wave deformer
  /// \return the offset amount
  XReal GetOffset() const;

  /// \brief  sets the low bound for the wave deformer
  /// \param  v -  the low bound
  void SetMinRadius(XReal v);

  /// \brief  sets the high bound for the wave deformer
  /// \param  v -  the high bound
  void SetMaxRadius(XReal v);

  /// \brief  sets the amplitude for the wave deformer
  /// \param  v -  the amplitude
  void SetAmplitude(XReal v);

  /// \brief  sets the wavelength for the wave deformer
  /// \param  v -  the wavelength
  void SetWavelength(XReal v);

  /// \brief  sets the dropoff for the wave deformer
  /// \param  v -  the dropoff
  void SetDropoff(XReal v);

  /// \brief  sets the dropoff position for the wave deformer
  /// \param  v -  the dropoff position
  void SetDropoffPosition(XReal v);

  /// \brief  sets the offset for the wave deformer
  /// \param  v -  the offset
  void SetOffset(XReal v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal min_radius;
  XReal max_radius;
  XReal amplitude;
  XReal wavelength;
  XReal dropoff;
  XReal dropoff_position;
  XReal offset;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::NonLinearWave, XNonLinearWave, "nonLinearWave", "NONLINEAR_WAVE");
#endif
}
