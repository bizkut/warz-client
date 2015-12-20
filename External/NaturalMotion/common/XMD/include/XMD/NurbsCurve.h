//---------------------------------------------------------------------------------------------------------------------- 
/// \file   NurbsCurve.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the structures to hold a NurbsCurve. The curve is evaluated using the Cox-De-Boor algorithm.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ParametricCurve.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XNurbsCurve 
/// \brief This class holds and draws the data for a nurbs curve
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XNurbsCurve 
  : public XParametricCurve
{
  XMD_CHUNK(XNurbsCurve);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kDegree = XParametricCurve::kLast+1; // XU32
  static const XU32 kKnots  = XParametricCurve::kLast+2; // XRealArray
  static const XU32 kLast   = kKnots;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   NURBS Curve Information
  /// \brief  Sets/Gets the params for the NURBS curve data
  //@{

  /// \brief  returns The degree of the curve
  /// \return the curve degree
  XU16 GetDegree() const;

  /// \brief  returns The degree of the curve
  /// \return the curve degree
  void SetDegree(XU16);

  /// \brief  Allows you to query the knot vector 
  /// \param  knots  -  the returned array of surface knots
  void GetKnots(XRealArray& knots) const;

  /// \brief  Allows you to set the knot vector 
  /// \param  knots  -  the new array of surface knots
  void SetKnots(const XRealArray& knots);

  /// \brief  returns an internal reference to the knots array
  /// \return a reference to the knots array
  const XRealArray& Knots() const;

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the curve degree in the U direction
  XU16 m_Degree;
  /// the knot vector in U direction
  XRealArray m_Knots;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::NurbsCurve, XNurbsCurve, "nurbsCurve", "NURBS_CURVE");
#endif
}
