//----------------------------------------------------------------------------------------------------------------------
/// \file   RampShader.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a shader that is akin to the 'predator' effect, where the
///         object uses the background colour, but may have a specular highlight 
///         contained over the top. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/SurfaceShader.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
///  \brief  THIS NODE IS NOT YET FULLY COMPLETE. 
///         defines a shader that is akin to the 'predator' effect, where 
///         the object uses the background colour, but may have a specular  
///         highlight contained over the top. 
/// \todo   [RB] handle the ramp attributes
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XRampShader :
  public XSurfaceShader
{
  XMD_CHUNK(XRampShader);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast          = XMaterial::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \brief  The available ramp types
  enum RampType
  {
    kColourRamp = 0,          ///< colour ramp
    kSpecularRamp = 1,        ///< specular colour ramp
    kTransparencyRamp = 2,    ///< transparency colour ramp
    kEmissionRamp = 3,        ///< emission colour ramp
    kEnvironmentRamp = 4,     ///< environment colour ramp
    kSpecularRollOffRamp = 5, ///< single value specular roll off
    kReflectivityRamp = 6,    ///< single value reflectivity
    kLastRamp = 7
  };

  //--------------------------------------------------------------------------------------------------------------------
  /// \brief  The ramp interpolation types
  enum InterpolationType
  {
    kNone,    ///< no interpolation
    kLinear,  ///< linear interpolation
    kSmooth,  ///< smooth interpolation
    kSpline   ///< spline interpolation
  };

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Ramp Shader Params
  /// \brief  Sets/Gets the params for the XRampShader Material type
  //@{

  /// \brief  sets the number of values used by the ramp
  /// \param  ramp - which ramp to set
  /// \param  num_colours  - the number of layers in the shader
  void SetNumRampValues(RampType ramp, XU32 num_values);

  /// \brief  returns the number of values used by the ramp
  /// \param  ramp - which ramp to query
  /// \return the total number of values in the ramp
  XU32 GetNumRampValues(RampType ramp) const;

  /// \brief  adds a new colour to the ramp
  /// \param  ramp - which ramp to add to
  /// \param  colour  - the new colour
  /// \param  position - the position in the ramp
  /// \param  interp_type - the interpolation type
  /// \return true if successfully added
  /// \note   not valid for specular roll off ramp or reflectivity ramp
  bool AddValue(RampType ramp, const XColour& colour, XReal position, XU32 interp_type);

  /// \brief  adds a new colour input to the ramp
  /// \param  ramp - which ramp to add to
  /// \param  colour  - the new colour input
  /// \param  position - the position in the ramp
  /// \param  interp_type - the interpolation type
  /// \return true if successfully added
  /// \note   not valid for specular roll off ramp or reflectivity ramp
  bool AddValue(RampType ramp, const XShadingNode* colour, XReal position, XU32 interp_type);

  /// \brief  adds a new real value to the ramp
  /// \param  ramp - which ramp to add to
  /// \param  value  - the new value
  /// \param  position - the position in the ramp
  /// \param  interp_type - the interpolation type
  /// \return true if successfully added
  /// \note   only valid for specular roll off ramp or reflectivity ramp
  bool AddValue(RampType ramp, XReal value, XReal position, XU32 interp_type);

  /// \brief  removes an existing layer from the layered shader.
  /// \param  ramp - which ramp to remove from
  /// \param  index - the index of the value
  /// \return true if the index was valid and was removed
  bool RemoveValue(RampType ramp, XU32 index);

  /// \brief  sets the value of an index on the specified ramp
  /// \param  ramp - the specified ramp
  /// \param  index - the values index
  /// \param  colour  - the new colour value
  /// \return true if layer set
  bool SetValue(RampType ramp, XU32 index, const XColour& colour);

  /// \brief  sets the value of an index on the specified ramp
  /// \param  ramp - the specified ramp
  /// \param  index - the values index
  /// \param  input  - the new colour input
  /// \return true if layer set
  bool SetValue(RampType ramp, XU32 index, const XShadingNode* input);

  /// \brief  sets the value of an index on the specified ramp
  /// \param  ramp - the specified ramp
  /// \param  index - the values index
  /// \param  value  - the new value
  /// \return true if set
  bool SetValue(RampType ramp, XU32 index, XReal value);

  /// \brief  sets the interpolation type of an index on the specified ramp
  /// \param  ramp - the specified ramp
  /// \param  index - the values index
  /// \param  t - the new interpolation type
  /// \return true if set
  bool SetInterpType(RampType ramp, XU32 index, InterpolationType t);

  /// \brief  sets the position of an index on the specified ramp
  /// \param  ramp - the specified ramp
  /// \param  index - the values index
  /// \param  pos - the new position
  /// \return true if set
  bool SetPosition(RampType ramp, XU32 index, XReal pos);

  /// \brief  sets the value of an index on the specified ramp
  /// \param  ramp - the specified ramp
  /// \param  index - the values index
  /// \param  colour  - the returned colour value
  /// \return true if successfully returned
  bool GetValue(RampType ramp, XU32 index, XColour& colour) const;

  /// \brief  gets the input of an index on the specified ramp
  /// \param  ramp - the specified ramp
  /// \param  index - the values index
  /// \param  colour  - the returned shading node input
  /// \return true if successfully returned
  bool GetValue(RampType ramp, XU32 index, XShadingNode*& input) const;

  /// \brief  gets the value of an index on the specified ramp
  /// \param  ramp - the specified ramp
  /// \param  index - the values index
  /// \param  value - the returned value
  /// \return true if successfully returned
  bool GetValue(RampType ramp, XU32 index, XReal& value) const;

  /// \brief  gets the interpolation type of an index on the specified ramp
  /// \param  ramp - the specified ramp
  /// \param  index - the index
  /// \return the interpolation type
  InterpolationType GetInterpType(RampType ramp, XU32 index) const;

  /// \brief  gets the position of an index on the specified ramp
  /// \param  ramp - the specified ramp
  /// \param  index - the index
  /// \return the position
  XReal GetPosition(RampType ramp, XU32 index) const;
  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XU32Array m_RampInterpTypes[kLastRamp];
  XRealArray m_RampPositions[kLastRamp];
  XShadingNode::XConnectionArray m_RampColourValues[5];
  XRealArray m_RampRealValues[2];
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::RampShader, XRampShader, "rampShader", "RAMP_SHADER");
#endif
}
