//--------------------------------------------------------------------------------------------------------------------
/// \file   Air.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the interface for an Air dynamics field. This connects
///         to a particle shape to move the particles upwards (eg, smoke). 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//--------------------------------------------------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------------------------------------------------
#include "XMD/Field.h"
//--------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//--------------------------------------------------------------------------------------------------------------------
/// \class XMD::XAir 
/// \brief Defines an air field  
//--------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XAir 
  : public XField
{
  XMD_CHUNK(XAir);
public:

  //------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kSpeed            = XField::kLast+1; // XReal
  static const XU32 kSpread           = XField::kLast+2; // XReal
  static const XU32 kDirection        = XField::kLast+3; // XVector3
  static const XU32 kIsSpreadEnabled  = XField::kLast+4; // bool
  static const XU32 kIsComponentOnly  = XField::kLast+5; // bool
  static const XU32 kInheritsRotation = XField::kLast+6; // bool
  static const XU32 kInheritsVelocity = XField::kLast+7; // XReal
  static const XU32 kLast             = kInheritsVelocity;
  //------------------------------------------------------------------------------------------------------------------

  /// \name   Air Dynamics Field Attributes
  /// \brief  Sets/Gets the params for the XAir dynamics field
  //@{

  /// \brief  Returns the direction of the air field effect
  /// \return The direction vector
  const XVector3& GetDirection() const;

  /// \brief  Returns the air speed
  /// \return returns the speed of the air
  XReal GetSpeed() const;

  /// \brief  returns the spread
  /// \return returns the spread angle
  XReal GetSpread() const;

  /// \brief  Is spread used?
  /// \return true of false
  bool GetSpreadEnabled() const;

  /// \brief  returns true if the air affects the geometry components 
  ///         rather than the actual objects themselves
  /// \return true or false
  bool GetComponentOnly() const;

  /// \brief  Returns true if the rotation gets inherited
  /// \return true or false
  bool GetInheritsRotation() const;

  /// \brief  Returns the velocity inherited
  /// \return the amount of the moving air field's velocity that is 
  ///         added to the direction and magnitude of the wind
  XReal GetInheritsVelocity() const;

  /// \brief  Returns the direction of the air field effect
  /// \param  v - The new direction vector
  void SetDirection(const XVector3& v);

  /// \brief  sets the air speed
  /// \param  v - the new air speed
  void SetSpeed(XReal v);

  /// \brief  sets the spread angle
  /// \param  v - the new speed angle
  void SetSpread(XReal v);

  /// \brief  sets spread usage
  /// \param  v - the new spread angle
  void SetSpreadEnabled(bool v);

  /// \brief  sets the air to affect the geometry components
  /// \param  v - true or false
  void SetComponentOnly(bool v);

  /// \brief  sets the rotation to be inherited
  /// \param  v - true or false
  void SetInheritsRotation(bool v);

  /// \brief  sets the velocity to be inherited
  /// \param  v - the amount of the moving air field's velocity that 
  ///             is added to the direction and magnitude of the wind
  void SetInheritsVelocity(XReal v);

  /// \brief  \b DEPRECATED Is spread used?
  /// \return true of false
  /// \deprecated Use XAir::GetSpreadEnabled() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XAir::GetSpreadEnabled() instead")
  #endif
  inline bool IsSpreadEnabled() const { return GetSpreadEnabled(); }

  /// \brief  \b DEPRECATED returns true if the air affects the geometry components 
  ///         rather than the actual objects themselves
  /// \return true or false
  /// \deprecated Use XAir::GetComponentOnly() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XAir::GetComponentOnly() instead")
  #endif
  inline bool IsComponentOnly() const { return GetComponentOnly(); }

  /// \brief  \b DEPRECATED Returns true if the rotation gets inherited
  /// \return  true or false
  /// \deprecated Use XAir::GetInheritsRotation() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XAir::GetInheritsRotation() instead")
  #endif
  inline bool InheritsRotation() const { return GetInheritsRotation(); }

  /// \brief  \b DEPRECATED Returns true if the velocity gets inherited
  /// \return  true or false
  /// \deprecated Use XAir::GetInheritsVelocity() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XAir::GetInheritsVelocity() instead")
  #endif
  inline XReal InheritsVelocity() const { return GetInheritsVelocity(); }

  //@}

private:
#ifndef DOXYGEN_PROCESSING

  /// the direction of the air. By default up
  XVector3 m_Direction;

  /// the speed of the air?
  XReal m_Speed;

  /// the velocity to inherit?
  XReal m_InheritVelocity;

  /// the spread distance
  XReal m_Spread;

  /// should the rotation of the particles be passed on?
  bool m_bInheritRotation;

  /// affects the components of the geometry rather than the whole objects
  bool m_bComponentOnly;

  /// should spread be enabled?
  bool m_bEnableSpread;
#endif
};

#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Air,XAir,"air","AIR_FIELD");
#endif
}
