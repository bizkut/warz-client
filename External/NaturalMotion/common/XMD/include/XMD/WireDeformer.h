//----------------------------------------------------------------------------------------------------------------------
/// \file   WireDeformer.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a Wire deformation. A controlling wire can be pulled about 
///         to deform a specific surface.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Deformer.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XWireDeformer
/// \brief This class holds the data for a wire deformer
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XWireDeformer
  : public XDeformer
{
  XMD_CHUNK(XWireDeformer);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kWireScale           = XDeformer::kLast+1; // XRealArray
  static const XU32 kWireDropoffDistance = XDeformer::kLast+2; // XRealArray
  static const XU32 kLocalIntensity      = XDeformer::kLast+3; // XReal
  static const XU32 kRotation            = XDeformer::kLast+4; // XReal
  static const XU32 kWires               = XDeformer::kLast+5; // XList
  static const XU32 kCrossingEffect      = XDeformer::kLast+6; // XReal
  static const XU32 kBaseWires           = XDeformer::kLast+7; // XList
  static const XU32 kLast                = kBaseWires;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Wire Deformer Params
  /// \brief  Sets/Gets the params for the XWireDeformer
  //@{

  /// \brief  returns the number of wires
  /// \return the number of wires for the deformer
  XU32 GetNumWires() const;

  /// \brief  returns the requested wire
  /// \param  wire_index  - the wire index
  /// \return a handle to the wire (NURBS curve) shape
  XBase* GetWire(XU32 wire_index) const;

  /// \brief  returns the requested base wire
  /// \param  wire_index  - the wire index
  /// \return a handle to the wire (NURBS curve) shape used as the 
  ///         reference for the undeformed wire. 
  XBase* GetBaseWire(XU32 wire_index) const;

  /// \brief  returns a list of the wires used in the deformer
  /// \param  wires - the returned list of wires
  void GetWires(XList& wires) const;

  /// \brief  returns a list of the base wires used in the deformer
  /// \param  wires - the returned list of wires
  void GetBaseWires(XList& wires) const;

  /// \brief  sets the wires used by the deformers. These should be of 
  ///         type XFn::ParametricCurve
  /// \param  wires - the new set of deformer wires. This should be an
  ///         array of XMD::XParametricCurve nodes.
  /// \param  base_wires - the new set of base wires. This should be an
  ///         array of XMD::XParametricCurve nodes.
  /// \return true if ok, false otherwise (normally if one of the wires is
  ///         not a curve object for example. 
  bool SetWires(const XList& wires, const XList& base_wires);

  /// \brief  removes the specified wire from the deformer
  /// \param  wire_index - the index of the wire to remove
  /// \return true if OK 
  bool RemoveWire(XU32 wire_index);

  /// \brief  returns the requested wire dropoff distance
  /// \param  wire_index  - the wire index
  /// \return the dropoff distance for the specified wire
  XReal GetWireDropoffDistance(XU32 wire_index) const;

  /// \brief  returns the requested wire scale
  /// \param  wire_index  - the wire index
  /// \return the scale for the specified wire
  XReal GetWireScale(XU32 wire_index) const;

  /// \brief  returns the crossing effect of the wire deformer
  /// \return the crossing effect amount
  XReal GetCrossingEffect() const;

  /// \brief  returns the local intensity for the wire deformer
  /// \return the local intensity
  XReal GetLocalIntensity() const;

  /// \brief  returns the rotation for the wire deformer
  /// \return the rotation amount
  XReal GetRotation() const;

  /// \brief  adds a new wire into the deformer
  /// \param  wire_object - the handle to the wire. This should be a 
  ///         valid pointer to an XParametricCurve derived node.
  /// \param  base_wire_object - the handle to the base wire. This should 
  ///         be a valid pointer to an XParametricCurve derived node.
  /// \return true is OK
  /// \note   the new index for the wire will always be, (GetNumWires()-1)
  bool AddWire(const XBase* wire_object, const XBase* base_wire_object);
 
  /// \brief  sets the dropoff distance for a specified wire
  /// \param  wire_index - the index of the wire
  /// \param  value - the new value for the dropoff distance
  /// \return true if OK 
  bool SetWireDropoffDistance(XU32 wire_index, XReal value);

  /// \brief  sets the scaling amount for a specified wire
  /// \param  wire_index - the index of the wire
  /// \param  value - the new value for the scaling amount
  /// \return true if OK 
  bool SetWireScale(XU32 wire_index, XReal value);

  /// \brief  sets the crossing effect for a specified wire
  /// \param  value - the new value for the crossing effect
  /// \return true if OK 
  void SetCrossingEffect(XReal value);

  /// \brief  sets the local intensity for a specified wire
  /// \param  value - the new value for the local intensity
  /// \return true if OK 
  void SetLocalIntensity(XReal value);

  /// \brief  sets the rotation for a specified wire
  /// \param  value - the new value for the rotation
  /// \return true if OK 
  void SetRotation(XReal value);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  struct XWire 
  {
    XWire();
    XWire(const XWire& wire);
    /// the id number of the original wire
    XId m_BaseWire;
    /// the id of the deformed wire
    XId m_Wire;
    /// the dropoff distance of the wire
    XReal m_DropoffDistance;
    /// the scale factor caused by the wire
    XReal m_WireScale;
  };
  typedef XM2::pod_vector<XWire> XWires;
  /// the wires used for the deformation
  XWires m_Wires;
  ///  The crossing effect applies when two wire curves cross each other.
  ///  Normally, the deformation is dampened so that the surface is only 
  ///  affected by the most deformed curve at the intersection point. When 
  ///  the crossing effect is increased, the effect of the wires becomes additive. 
  ///  So, the object will be deform more near crossing wires.
  XReal m_CrossingEffect;
  /// the local intensity of the deformation
  XReal m_LocalIntensity;
  /// the rotation of the deformation
  XReal m_Rotation;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::WireDeformer, XWireDeformer, "wireDeformer", "WIRE_DEFORMER");
#endif
}
