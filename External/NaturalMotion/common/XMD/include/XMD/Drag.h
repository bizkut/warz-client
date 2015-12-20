//----------------------------------------------------------------------------------------------------------------------
/// \file   Drag.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the output method for Drag Fields
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Field.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XDrag
/// \brief The definition of a drag dynamic field. This applies a drag force 
///        to a particle system. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XDrag :
  public XField
{
  XMD_CHUNK(XDrag);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kDirection     = XField::kLast+1; // XVector3
  static const XU32 kDirectionUsed = XField::kLast+2; // bool
  static const XU32 kLast          = kDirectionUsed;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Drag Dynamics Field Attributes
  /// \brief  Sets/Gets the params for the XDrag dynamics field
  //@{

  /// \brief  returns the direction of the drag field
  /// \return  the direction vector of the drag field effect
  const XVector3& GetDirection() const;

  /// \brief  sets the direction vector for the drag field 
  /// \param  dir - the new direction
  void SetDirection(const XVector3& dir);

  /// \brief  returns true if the direction is used by this dynamics field
  /// \return  the direction vector
  bool GetIsDirectionUsed() const;

  /// \brief  sets a flag to indicate if the direction is used by the field
  /// \param  usage - on(true) off(false)
  void SetIsDirectionUsed(bool usage);

  /// \brief  \b DEPRECATED returns true if the direction is used by this dynamics field
  /// \return  the direction vector
  /// \deprecated Use XDrag::GetIsDirectionUsed() instead of this function
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XDrag::GetIsDirectionUsed() instead of this function")
  #endif
  inline bool IsDirectionUsed() const { return GetIsDirectionUsed(); }

  /// \brief  \b DEPRECATED sets a flag to indicate if the direction is used by the field
  /// \param  usage - on(true) off(false)
  /// \deprecated Use XDrag::SetIsDirectionUsed() instead of this function
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XDrag::SetIsDirectionUsed() instead of this function")
  #endif
  inline void SetDirectionUsage(bool usage) { SetIsDirectionUsed(usage); }

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// vector for the direction of the drag effect
  XVector3 m_Direction;
  /// is drag going to affect anything?
  bool m_bUseDirection;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Drag, XDrag, "drag", "DRAG_FIELD");
#endif
}
