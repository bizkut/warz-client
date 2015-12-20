//----------------------------------------------------------------------------------------------------------------------
/// \file   SculptDeformer.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a Sculpt deformation
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Deformer.h"  
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XSculptDeformer
/// \brief This class holds the data for a sculpt deformation
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XSculptDeformer
  : public XDeformer
{
  XMD_CHUNK(XSculptDeformer);
public:

  //-------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kMaxDisplacement = XDeformer::kLast+1; // XReal
  static const XU32 kStartPosition   = XDeformer::kLast+2; // XVector3
  static const XU32 kDropoffDistance = XDeformer::kLast+3; // XReal
  static const XU32 kMode            = XDeformer::kLast+4; // XU32
  static const XU32 kInsideMode      = XDeformer::kLast+5; // XU32
  static const XU32 kDropoffType     = XDeformer::kLast+6; // XU32
  static const XU32 kSculptObject    = XDeformer::kLast+7; // XBase*
  static const XU32 kStartObject     = XDeformer::kLast+8; // XBase*
  static const XU32 kLast            = kStartObject;
  //-------------------------------------------------------------------------------------------------------------------

  /// \name   Sculpt Deformer Params
  /// \brief  Sets/Gets the params for the XSculptDeformer deformer
  //@{

  /// \brief  returns the mode of the deformer. 0=flip, 1=project, 2=stretch
  /// \return the algorithm used to deform the points 
  XU32 GetMode() const;

  /// \brief  returns the mode of the deformer. 0=ring, 1=even
  /// \return the algorithm for the points in the sculpt sphere
  XU32 GetInsideMode() const;

  /// \brief  returns the maximum displacement of the deformer
  /// \return the max displacement
  XReal GetMaxDisplacement() const;

  /// \brief  returns the drop off type. 0=none, 1=linear
  /// \return the drop off type
  XU32 GetDropoffType() const;

  /// \brief  returns the drop off distance of the deformer
  /// \return the drop off distance of the deformer
  XReal GetDropoffDistance() const;

  /// \brief  returns the start position of the deformer
  /// \return the start position of the deformer
  const XVector3& GetStartPosition() const;

  /// \brief  returns the sculpt sphere object
  /// \return the sculpt object
  XBase* GetSculptObject() const;

  /// \brief  returns the start sphere object
  /// \return the start object
  XBase* GetStartObject() const;

  /// \brief  sets the mode of the deformer. 0=flip, 1=project, 2=stretch
  /// \param  v - the algorithm used to deform the points 
  void SetMode(XU32 v);

  /// \brief  sets the mode of the deformer. 0=ring, 1=even
  /// \param  v - the algorithm for the points in the sculpt sphere
  void SetInsideMode(XU32 v);

  /// \brief  sets the maximum displacement of the deformer
  /// \param  v - the max displacement
  void SetMaxDisplacement(XReal v);

  /// \brief  sets the drop off type. 0=none, 1=linear
  /// \param  v - the drop off type
  void SetDropoffType(XU32 v);

  /// \brief  sets the drop off distance of the deformer
  /// \param  v - the drop off distance of the deformer
  void SetDropoffDistance(XReal v);

  /// \brief  sets the start position of the deformer
  /// \param  v - the start position of the deformer
  void SetStartPosition(const XVector3& v);

  /// \brief  sets the sculpt sphere object
  /// \param  obj - the sculpt object
  bool SetSculptObject(const XBase* obj);

  /// \brief  sets the start sphere object
  /// \param  obj - the start object
  bool SetStartObject(const XBase* obj);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING
  XU32 mode;
  XU32 inside_mode;
  XReal maximum_displacement;
  XU32 dropoff_type;
  XReal dropoff_distance;
  XVector3 start_position;
  XId sculpt_object;
  XId start_object;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::SculptDeformer, XSculptDeformer, "sculptDeformer", "SCULPT_DEFORMER");
#endif
}
