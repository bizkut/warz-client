//----------------------------------------------------------------------------------------------------------------------
/// \file   ParametricCurve.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the base class of all parametric curves
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ParametricBase.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XParametricCurve
/// \brief defines the base class of all parametric curve shapes
/// \note  All parametric curves should derive from this base class. I 
///        normally make this class distinction, since if you are drawing 
///        curves, you normally evaluate some output buffer and then render
///        as a line strip. 
///          Some runtime engine can therefore just provide a virtual
///        function evaluate, where then the higher derived types evaluate
///        the output vertices into the buffer.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XParametricCurve
  : public XParametricBase
{
  XMD_ABSTRACT_CHUNK(XParametricCurve);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kCurveType = XParametricBase::kLast+1;
  static const XU32 kLast      = kCurveType;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Parametric Curve Class Params
  /// \brief  Sets/Gets the params for the XParametricCurve class
  //@{

  /// \brief  returns the type of curve
  /// \return 0 if open, 1 if closed, 2 if periodic
  XU32 GetCurveType() const;

  /// \brief  returns the type of curve
  /// \param  type - 0 if open, 1 if closed, 2 if periodic
  void SetCurveType(XU32 type);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// 0=open, 1=closed, 2=periodic
  XU32 m_CurveType;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::ParametricCurve, XParametricCurve, "parametricCurve");
#endif
}
