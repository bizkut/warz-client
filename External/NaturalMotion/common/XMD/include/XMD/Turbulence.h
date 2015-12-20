//----------------------------------------------------------------------------------------------------------------------
/// \file   Turbulence.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a turbulence dynamics field
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Field.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XTurbulence
/// \brief a turbulence dynamics field
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XTurbulence :
  public XField
{
  XMD_CHUNK(XTurbulence);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kFrequency  = XField::kLast+1; // XReal
  static const XU32 kPhase      = XField::kLast+2; // XVector3
  static const XU32 kLast       = kPhase;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Turbulence Dynamics Field Attributes
  /// \brief  Sets/Gets the params for the XTurbulence dynamics field
  //@{

  /// \brief  This function returns the frequency of the turbulence
  /// \return the turbulence frequency
  XReal GetFrequency() const;

  /// \brief  This function gets the frequency of the turbulence
  /// \param  v - the turbulence frequency
  void SetFrequency(XReal v);

  /// \brief  This function gets the phase for the turbulence field. 
  /// \param  phase - the turbulence phase
  void GetPhase(XVector3& phase) const;

  /// \brief  This function sets the phase for the x,y,z components of
  ///         the turbulence field.
  /// \param  phase - the turbulence phase
  void SetPhase(const XVector3& phase);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name deprecated functions
  //@{

  /// \brief  \b [DEPRECATED] This function gets the phase for the turbulence field. 
  /// \return the axis
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Phase is now stored as an XVector3 to provide seperate phases for x,y and z")
  #endif
  XReal GetPhase() const;

  /// \brief  \b [DEPRECATED] This function sets the phase amount 
  /// \return the axis
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Phase is now stored as an XVector3 to provide seperate phases for x,y and z")
  #endif
  void SetPhase(const XReal v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the turbulence frequency
  XReal m_Frequency;
  /// the turbulence phase
  XVector3 m_Phase;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Turbulence, XTurbulence, "turbulence", "TURBULENCE_FIELD");
#endif
}
