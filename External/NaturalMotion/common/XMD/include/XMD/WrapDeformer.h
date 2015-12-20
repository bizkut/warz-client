//----------------------------------------------------------------------------------------------------------------------
/// \file   WrapDeformer.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a Wrap deformation
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Deformer.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XWrapDeformer
/// \brief This class holds the data for a wrap deformer
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XWrapDeformer
  : public XDeformer
{
  XMD_CHUNK(XWrapDeformer);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kDropoff         = XDeformer::kLast+1; // XReal
  static const XU32 kSmoothness      = XDeformer::kLast+2; // XReal
  static const XU32 kWeightThreshold = XDeformer::kLast+3; // XReal
  static const XU32 kMaxDistance     = XDeformer::kLast+4; // XReal
  static const XU32 kInfluenceType   = XDeformer::kLast+5; // XU32
  static const XU32 kNurbsSamples    = XDeformer::kLast+6; // XU32
  static const XU32 kDriver          = XDeformer::kLast+7; // XBase*
  static const XU32 kLast            = kDriver;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Wrap Deformer Params
  /// \brief  Sets/Gets the params for the XWrapDeformer deformer
  //@{

  /// \brief  returns the drop off for the wrap deformer
  /// \return the drop off for the wrap deformer
  XReal GetDropoff() const;

  /// \brief  returns the smoothness for the wrap deformer
  /// \return the smoothness for the wrap deformer
  XReal GetSmoothness() const;

  /// \brief  returns the weight threshold for the wrap deformer
  /// \return the weight threshold for the wrap deformer
  XReal GetWeightThreshold() const;

  /// \brief  returns the max distance for the wrap deformer
  /// \return the max distance for the wrap deformer
  XReal GetMaxDistance() const;

  /// \brief  returns the influence type of the wrap deformer. 0=point, 1 = face.
  /// \return the influence type of the wrap deformer
  XU32 GetInfluenceType() const;

  /// \brief  returns the number of nurbs samples for the wrap deformer
  /// \return the number of nurbs samples for the wrap deformer
  XU32 GetNurbsSamples() const;

  /// \brief  returns the driving object for the wrap deformer
  /// \return the driving object for the wrap deformer
  XBase* GetDriver() const;

  /// \brief  sets the dropoff amount on the wrap deformer
  /// \param  value - the new dropoff amount
  void SetDropoff(XReal value);

  /// \brief  sets the smoothness amount on the wrap deformer
  /// \param  value - the new smoothness amount
  void SetSmoothness(XReal value);

  /// \brief  sets the weight threshold amount on the wrap deformer
  /// \param  value - the new weight threshold amount
  void SetWeightThreshold(XReal value);

  /// \brief  sets the max distance amount on the wrap deformer
  /// \param  value - the new max distance amount
  void SetMaxDistance(XReal value);

  /// \brief  sets the influence type on the wrap deformer. 1=point, 2 = face
  /// \param  value - the new influence type for the deformer. 1=point, 2 = face
  void SetInfluenceType(XU32 value);

  /// \brief  sets the number of nurbs samples on the wrap deformer
  /// \param  value - the new dropoff amount
  void SetNurbsSamples(XU32 value);

  /// \brief  sets the driver object for the wrap deformer
  /// \param  driver - the new driver object
  bool SetDriver(const XBase* driver);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal dropoff;
  XReal smoothness;
  XReal weight_threshold;
  XReal max_distance;
  XU32 influence_type;
  XU32 nurbs_samples;
  XId driver_geometry;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::WrapDeformer, XWrapDeformer, "wrapDeformer", "WRAP_DEFORMER");
#endif
}
