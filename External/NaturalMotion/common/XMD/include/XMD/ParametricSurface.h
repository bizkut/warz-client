//----------------------------------------------------------------------------------------------------------------------
/// \file   ParametricSurface.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the base class of all parametric surfaces
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ParametricBase.h"
#include "XMD/TrimData.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class forms the common base class of all parametric (and or dynamic) surface types. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XParametricSurface
  : public XParametricBase
{
  XMD_ABSTRACT_CHUNK(XParametricSurface);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kTypeInU   = XParametricBase::kLast+1; // XU32
  static const XU32 kTypeInV   = XParametricBase::kLast+2; // XU32
  static const XU32 kNumCvsInU = XParametricBase::kLast+3; // XU32
  static const XU32 kNumCvsInV = XParametricBase::kLast+4; // XU32
  static const XU32 kLast      = kNumCvsInV;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Trim Curve Information
  /// \brief  Sets/Gets the Trim curve data for the class
  //@{

  /// \brief  returns the number of regions in the surface
  XS32 GetNumRegions() const;

  /// \brief  returns the number of boundaries in a given region
  /// \param  region  -  the region to query
  XS32 GetNumBoundaries(XS32 region) const;

  /// \brief  returns the boundary type 
  /// \param  region  -  the region to query
  /// \param  boundary  -  the boundary to query
  XTrimData::BoundaryType GetBoundaryType(XS32 region, XS32 boundary) const;

  /// \brief  returns the number of boundaries in a given region
  /// \param  region  -  the region to query
  /// \param  boundary  -  the boundary to query
  XS32 GetNumCurves(XS32 region, XS32 boundary) const;

  /// \brief  returns a pointer to the requested 2D parametric curve
  /// \param  region  -  the region to query
  /// \param  boundary  -  the boundary to query
  /// \param  curve - returns the requested 2D curve
  const Curve2D* Get2DCurve(XS32 region, XS32 boundary, XS32 curve) const;

  /// \brief  returns a pointer to the requested 3D parametric curve
  /// \param  region  -  the region to query
  /// \param  boundary  -  the boundary to query
  /// \param  curve - returns the requested 3D curve
  const Curve3D* Get3DCurve(XS32 region, XS32 boundary, XS32 curve) const;

  //@}
  
  /// \name   Common Parametric Surface Information
  /// \brief  Sets/Gets the common params for the parametric surface data
  //@{

  /// \brief  returns the type of the surface in the U direction
  /// \return  the type of the curve in U, 
  /// 0  =  open
  /// 1  =  closed    -  force last set of control points to be where first set are
  /// 2  =  periodic  -  The surface must maintain smoothness across it's surface.
  ///              The last degree num of control points must be clamped to the first degree'th.
  ///              Err, basically if the u direction is of degree 3, then the last three control
  ///              points are clamped to the first three in that direction.
  XU8 GetTypeInU() const;

  /// \brief  returns the type of the surface in the V direction
  /// \return  the type of the curve in V,
  /// 0  =  open
  /// 1  =  closed    -  force last set of control points to be where first set are
  /// 2  =  periodic  -  The surface must maintain smoothness across it's surface.
  ///              The last degree num of control points must be clamped to the first degree'th.
  ///              Err, basically if the u direction is of degree 3, then the last three control
  ///              points are clamped to the first three in that direction.
  XU8 GetTypeInV() const;

  /// \brief  sets the curve type in the U direction
  /// \param  curve_type - 0 = open, 1 = closed, 2 = periodic
  void SetTypeInU(XS32 curve_type);

  /// \brief  sets the curve type in the V direction
  /// \param  curve_type - 0 = open, 1 = closed, 2 = periodic
  void SetTypeInV(XS32 curve_type);

  /// \brief  sets the number of CV's in the U surface direction
  /// \param  num_cvs_u - the number of CV's in U 
  void SetNumCvsInU(XS32 num_cvs_u);

  /// \brief  sets the number of CV's in the V surface direction
  /// \param  num_cvs_v - the number of CV's in V
  void SetNumCvsInV(XS32 num_cvs_v);

  /// \brief  returns the number of CV's in the U direction
  /// \return  the number of CV's in the U direction
  XU32 GetNumCvsInU() const;

  /// \brief  returns the number of CV's in the V direction
  /// \return  the number of CV's in the V direction
  XU32 GetNumCvsInV() const;

  /// \brief  returns a reference to the requested CV
  /// \param  u  -  the u index
  /// \param  v  -  the v index
  /// \return  the CV
  XVector3 GetCV(XU32 u, XU32 v) const;

  /// \brief  \b DEPRECATED returns the number of CV's in the U direction
  /// \return  the number of CV's in the U direction
  /// \deprecated Use XParametricSurface::GetNumCvsInU() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XParametricSurface::GetNumCvsInU() instead")
  #endif
  inline XU32 NumCvsInU() const { return GetNumCvsInU(); } 

  /// \brief  \b DEPRECATED returns the number of CV's in the V direction
  /// \return  the number of CV's in the V direction
  /// \deprecated Use XParametricSurface::GetNumCvsInV() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XParametricSurface::GetNumCvsInU() instead")
  #endif
  inline XU32 NumCvsInV() const { return GetNumCvsInV(); } 

  //@}

// internal data
protected:
#ifndef DOXYGEN_PROCESSING
  /// the number of CVs in the U direction
  XU16 m_NumCvsU;
  /// the number of CVs in the V direction
  XU16 m_NumCvsV;
  /// the type of the curve in U,
  /// 0  =  open
  /// 1  =  closed    -  force last set of control points to be where first set are
  /// 2  =  periodic  -  The surface must maintain smoothness across it's surface.
  ///              The last degree num of control points must be clamped to the first degree'th.
  ///              Err, basically if the u direction is of degree 3, then the last three control
  ///              points are clamped to the first three in that direction.
  XU8 m_TypeU;
  /// the type of the curve in V,
  /// 0  =  open
  /// 1  =  closed    -  force last set of control points to be where first set are
  /// 2  =  periodic  -  The surface must maintain smoothness across it's surface.
  ///              The last degree num of control points must be clamped to the first degree'th.
  ///              Err, basically if the u direction is of degree 3, then the last three control
  ///              points are clamped to the first three in that direction.
  XU8 m_TypeV;
  /// a pointer to the trimmed surface data, if there is any
  XTrimData* m_TrimmedSurface;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::ParametricSurface, XParametricSurface, "parametricSurface");
#endif
}
