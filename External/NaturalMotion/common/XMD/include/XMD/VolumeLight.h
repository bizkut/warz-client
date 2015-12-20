//----------------------------------------------------------------------------------------------------------------------
/// \file   VolumeLight.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the class XVolumeLight.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ExtendedLight.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XRampAttribute
/// \brief a class to hold a ramp (2D graph) colour or float.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XRampAttribute
{
public:
#ifndef DOXYGEN_PROCESSING

  /// \brief  ctor
  /// \param  colour - true to specify a colour ramp, false to specify
  ///         a single float value ramp
  XRampAttribute(bool colour);

  /// \brief  ctor
  /// \param  start - the value to set at position 0.0
  /// \param  end   - the value to set at position 1.0
  /// \param  num   - total number of marks on the ramp.
  XRampAttribute(XReal start, XReal end, XU32 num = 2);

  /// \brief  ctor
  /// \param  start - the value to set at position 0.0
  /// \param  end   - the value to set at position 1.0
  /// \param  num   - total number of marks on the ramp.
  XRampAttribute(const XColour& start, const XColour& end, XU32 num = 2);

  /// \brief  dtor
  ~XRampAttribute();
#endif

  /// \name   Colour Ramp Params
  /// \brief  Sets/Gets the params for a colour based ramp attribute
  //@{

  /// \brief  returns true if this ramp is for a oolour
  /// \return true if curve ramp, false if float
  bool IsColour() const;

  /// \brief  retrieves info about the ramp entries
  /// \param  index  - the index of the entry to retrieve
  /// \param  pos    - the returned position [0>1]
  /// \param  val    - the returned value
  /// \param  interp - the returned interpolation type
  /// \return true if OK
  bool GetEntry(XU32 index, XReal& pos, XColour& val, XS32& interp) const;

  /// \brief  adds a new colour entry into the ramp.
  /// \param  pos    - the entry's position [0>1]
  /// \param  val    - the entry's value
  /// \param  interp - the entry's interpolation type
  /// \return true if OK
  bool AddEntry(XReal pos, const XColour& val, XS32 interp);

  //@}

  /// \name   Float Ramp Params
  /// \brief  Sets/Gets the params for a float based ramp attribute
  //@{

  /// \brief  returns true if this ramp is for a float
  /// \return false if curve ramp, true if float
  bool IsCurve() const;

  /// \brief  retrieves info about the ramp entries
  /// \param  index  - the index of the entry to retrieve
  /// \param  pos    - the returned position [0>1]
  /// \param  val    - the returned value
  /// \param  interp - the returned interpolation type
  /// \return true if OK
  bool GetEntry(XU32 index, XReal& pos, XReal& val, XS32& interp) const;

  /// \brief  adds a new float entry into the ramp.
  /// \param  pos    - the entry's position [0>1]
  /// \param  val    - the entry's value
  /// \param  interp - the entry's interpolation type
  /// \return true if OK
  bool AddEntry(XReal pos, XReal val, XS32 interp);

  //@}

  /// \name   Common Ramp Attribute Data
  //@{

  /// \brief  returns the number of entries in the ramp
  /// \return number of ramp entries
  XU32 GetNumEntries() const;

  //@}

#ifndef DOXYGEN_PROCESSING
  bool WriteChunk(std::ostream&);
  bool ReadChunk(std::istream&);
  bool DoData(XFileIO&);
  XU32 GetDataSize() const;

  /// \brief  assignment operator
  /// \param  rhs - the attribute to copy
  /// \return a constant reference to this
  const XRampAttribute& operator = (const XRampAttribute& rhs);

protected:
  /// true if ramp for a colour
  bool m_IsColour;
  /// the number of entries in the ramp
  XU32 m_NumEntries;
  union 
  {
    /// the colour values
    XColour* m_ColourEntries;
    /// the float values
    XReal* m_CurveEntries;
  };
  /// the interpolation types at the key values
  XS32* m_Interps;
  /// the positions of the ramp values
  XReal* m_Positions;
#endif
};


//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XVolumeLight
/// \brief A volume light type
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XVolumeLight
  : public XExtendedLight
{
  XMD_CHUNK(XVolumeLight);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kArc           = XExtendedLight::kLast+1; // XReal
  static const XU32 kConeEndRadius = XExtendedLight::kLast+2; // XReal
  static const XU32 kLast          = kConeEndRadius;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Volume Light Params
  /// \brief  Sets/Gets the params for the XVolumeLight
  //@{

  /// \brief  returns the arc value of the volume light
  /// \return the arc value
  XReal GetArc() const;

  /// \brief  returns the end cone radius of the volume light
  /// \return the end cone radius
  XReal GetConeEndRadius() const;

  /// \brief  returns a reference for the volume light colour ramp
  /// \return reference to the ramp attribute
  const XRampAttribute& ColourRamp() const;

  /// \brief  returns a reference for the volume light penumbra ramp
  /// \return reference to the ramp attribute
  const XRampAttribute& PenumbraRamp() const;

  /// \brief  returns a reference for the volume light colour ramp
  /// \return reference to the ramp attribute
  XRampAttribute& ColourRamp();

  /// \brief  returns a reference for the volume light penumbra ramp
  /// \return reference to the ramp attribute
  XRampAttribute& PenumbraRamp();

  /// \brief  sets the arc value for the volume light
  /// \param  value   - the value of the lights arc
  void SetArc(XReal value);

  /// \brief  sets the cone end radius for the volume light
  /// \param  value   - the value of the lights end cone radius
  void SetConeEndRadius(XReal value);
  
  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the arc
  XReal m_Arc;
  /// the end radius
  XReal m_EndRadius;
  /// the color range
  XRampAttribute m_ColorRamp;
  /// the penumbra ramp
  XRampAttribute m_PenumbraRamp;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::VolumeLight, XVolumeLight, "volumeLight", "VOLUME_LIGHT");
#endif
}
