//----------------------------------------------------------------------------------------------------------------------
/// \file   Newton.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a newton force field
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Field.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XNewton
/// \brief A newton force field
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XNewton :
  public XField
{
  XMD_CHUNK(XNewton);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kMinDistance= XField::kLast+1; // XReal
  static const XU32 kLast       = kMinDistance;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Newton Dynamics Field Attributes
  /// \brief  Sets/Gets the params for the XNewton dynamics field
  //@{

  /// \brief  returns the min distance for the effect
  /// \return  the min distance value
  XReal GetMinDistance() const;

  /// \brief  allows you to set the minimum distance of the effect
  /// \param  dist  -  the min distance value
  void SetMinDistance(XReal dist);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// vector for the direction of the gravity effect
  XReal m_MinDistance;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Newton, XNewton, "newton", "NEWTON_FIELD");
#endif
}
