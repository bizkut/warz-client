//----------------------------------------------------------------------------------------------------------------------
/// \file   NurbsSurface.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the structures to hold a NurbsSurface. The surface is evaluated
///         using the Cox-De-Boor algorithm.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ParametricSurface.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XNurbsSurface 
/// \brief Defines a class to load and render a nurbs surface. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XNurbsSurface 
  : public XParametricSurface
{
  XMD_CHUNK(XNurbsSurface);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kDegreeU = XParametricSurface::kLast+1; // XU32
  static const XU32 kDegreeV = XParametricSurface::kLast+2; // XU32
  static const XU32 kKnotsU  = XParametricSurface::kLast+3; // XRealArray
  static const XU32 kKnotsV  = XParametricSurface::kLast+4; // XRealArray
  static const XU32 kLast    = kKnotsV;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   NURBS Surface Information
  /// \brief  Sets/Gets the params for the NURBS surface data
  //@{

  /// \brief  Allows you to query the surface knot vector in the U direction.
  /// \param  knots  -  the returned array of surface knots
  void GetKnotsU(XRealArray& knots) const;

  /// \brief  Allows you to query the surface knot vector in the V direction.
  /// \param  knots  -  the returned array of surface knots
  void GetKnotsV(XRealArray& knots) const;

  /// \brief  returns the surface degree in the U direction
  /// \return the degree of the curve in U
  XU8 GetDegreeU() const;

  /// \brief  returns the surface degree in the V direction
  /// \return the degree of the curve in V
  XU8 GetDegreeV() const;

  /// \brief  Allows you to query the surface knot vector in the U direction.
  /// \param  knots  -  the returned array of surface knots
  ///
  void SetKnotsU(const XRealArray& knots);

  /// \brief  Allows you to query the surface knot vector in the V direction.
  /// \param  knots  -  the returned array of surface knots
  void SetKnotsV(const XRealArray& knots);

  /// \brief  sets the surface degree in the U direction
  /// \param  degree - the degree of the curve in U
  void SetDegreeU(XU8 degree);

  /// \brief  sets the surface degree in the V direction
  /// \param  degreethe degree of the curve in V
  void SetDegreeV(const XU8 degree);

  /// \brief  returns an internal reference to the U knots array
  /// \return a reference to the U knots array
  const XRealArray& KnotsU() const;

  /// \brief  returns an internal reference to the V knots array
  /// \return a reference to the V knots array
  const XRealArray& KnotsV() const;

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the knot vector in U direction
  XRealArray m_KnotsU;
  
  /// the knot vector in U direction
  XRealArray m_KnotsV;

  /// the curve degree in the U direction
  XU8 m_DegreeU;
  
  /// the curve degree in the V direction
  XU8 m_DegreeV;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::NurbsSurface, XNurbsSurface, "nurbsSurface", "NURBS_SURFACE");
#endif
}
