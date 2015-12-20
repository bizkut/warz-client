//----------------------------------------------------------------------------------------------------------------------
/// \file   Field.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  The base class of all dynamics fields
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Bone.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XGeometry;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XField
/// \brief Defines the base class for all force fields  
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XField :
  public XBone
{
  XMD_ABSTRACT_CHUNK(XField);
public:


  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kMagnitude       = XBone::kLast+1; // XReal
  static const XU32 kAttenuation     = XBone::kLast+2; // XReal
  static const XU32 kMaxDistance     = XBone::kLast+3; // XReal
  static const XU32 kIsDistanceUsed  = XBone::kLast+4; // bool
  static const XU32 kIsPerVertex     = XBone::kLast+5; // bool
  static const XU32 kShapes          = XBone::kLast+6; // XList
  static const XU32 kVolumeExclusion = XBone::kLast+7; // bool
  static const XU32 kVolumeOffset    = XBone::kLast+8; // XVector3
  static const XU32 kSectionRadius   = XBone::kLast+9; // XReal
  static const XU32 kSweepAngle      = XBone::kLast+10;// XReal
  static const XU32 kVolumeType      = XBone::kLast+11;// XU32
  static const XU32 kLast            = kVolumeType;
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  the type of volume in which the field will operate
  enum XVolumeType
  {
    kNone,      ///< The field has no volume
    kCube,      ///< The field uses a cube volume
    kCylinder,  ///< The field uses a cylinder volume
    kCone,      ///< The field uses a cone shaped volume
    kSphere,    ///< The field uses a sphere shaped volume
    kTorus      ///< The field uses a torus shaped volume
  };

  /// \name   Dynamics Field Volume Attributes
  /// \brief  Sets/Gets a volume in which the field has an effect.
  //@{

  /// \brief  sets the type of volume in which the field acts
  /// \param  type - the volume type.
  void SetVolumeType(XVolumeType type);

  /// \brief  returns the type of volume this field acts within.
  /// \return the type of volume
  XVolumeType GetVolumeType() const;

  /// \brief  returns the sweep angle for the fields volume
  /// \return the sweep angle in radians
  XReal GetSweepAngle() const;

  /// \brief  sets the sweep angle for the volume object. 
  /// \param  sweep_angle - the sweep angle in radians
  void SetSweepAngle(XReal sweep_angle);

  /// \brief  returns the section radius of the cross section of the 
  ///         torus volume shape. 
  /// \return the torus cross section radius
  XReal GetSectionRadius() const;

  /// \brief  returns the section radius of the torus volume type
  /// \param  section_radius - the section radius of the torus volume
  void SetSectionRadius(XReal section_radius);

  /// \brief  returns a positional offset for the volume
  /// \return the volumes offset
  const XVector3& GetVolumeOffset() const;

  /// \brief  sets the offset vector for the volume. 
  /// \param  offset - the offset amount
  void SetVolumeOffset(const XVector3& offset);

  /// \brief  returns true if the field affects everything outside of the 
  ///         volume, false if it only affects things inside of the volume.
  /// \return true if affects everything outside of the volume. false if
  ///         it only affects everything inside. 
  bool GetVolumeExclusion() const;

  /// \brief  sets whether the field only acts within the volume (false), 
  ///         or whether it affects everything outside of the volume (true).
  /// \param  exclusion - true to affect everything outside of the field,
  ///         false to affect everything inside the field
  void SetVolumeExclusion(bool exclusion);

  //@}

  /// \name   Common Dynamics Field Attributes
  /// \brief  Sets/Gets the common params for all dynamics fields
  //@{

  /// \brief  returns the magnitude of the force
  /// \return the fields force
  XReal GetMagnitude() const;

  /// \brief  returns the attenuation factor for the field effect
  /// \return the field attenuation
  XReal GetAttenuation() const;

  /// \brief  returns the maximum distance of the field effect
  /// \return the field maximum influence distance
  XReal GetMaxDistance() const;

  /// \brief  returns true if the field has an effect confined to a
  ///         specific distance from the fields position.
  /// \return true if the distance & attenuation are used
  bool GetMaxDistanceUsed() const;

  /// \brief  returns true if the field is applied on each vertex. False
  ///         if it just applies to a whole geometry object. 
  /// \return true if OK
  bool GetPerVertex() const;

  /// \brief  returns the magnitude of the force
  /// \return the fields force
  void SetMagnitude(XReal v);

  /// \brief  returns the attenuation factor for the field effect
  /// \return the field attenuation
  void SetAttenuation(XReal v);

  /// \brief  returns the maximum distance of the field effect
  /// \return the field maximum influence distance
  void SetMaxDistance(XReal v);

  /// \brief  returns true if the field has an effect confined to a
  ///         specific distance from the fields position.
  /// \return true if the distance & attenuation are used
  void SetMaxDistanceUsed(bool v);

  /// \brief  returns true if the field is applied on each vertex. False
  ///         if it just applies to a whole geometry object
  /// \return true if OK
  void SetPerVertex(bool v);

  /// \brief  returns the number of geometry objects affected by the 
  ///         field effect.
  /// \return the number of shapes affected
  XU32 GetNumShapes() const;

  /// \brief  returns a pointer to the i'th geometry object affected by
  ///         this field.
  /// \param  idx  -  the index of the shape to query
  /// \return A pointer to the geometry object
  XGeometry* GetShape(XU32 idx) const;

  /// \brief  adds the specified shape into the affected shape list.
  /// \param  shape  -  pointer to the shape
  bool AddShape(const XBase* shape);

  /// \brief  returns a pointer to the i'th geometry object affected by
  ///         this field.
  /// \param  idx  -  the index of the shape to query=
  bool DeleteShape(XU32 idx);

  /// \brief  \b DEPRECATED returns true if the field has an effect confined to a
  ///         specific distance from the fields position.
  /// \return true if the distance & attenuation are used
  /// \deprecated Use XField::GetMaxDistanceUsed() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XField::GetMaxDistanceUsed() instead")
  #endif
  inline bool IsMaxDistanceUsed() const { return GetMaxDistanceUsed(); }

  /// \brief  \b DEPRECATED returns true if the field is applied on each vertex. False
  ///         if it just applies to a whole geometry object
  /// \return true if OK
  /// \deprecated Use XField::GetPerVertex() instead
  ///
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XField::GetPerVertex() instead")
  #endif
  inline bool IsPerVertex() const { return GetPerVertex(); }

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  void PostLoad();
private:
  /// the magnitude of the fields force
  XReal m_Magnitude;
  /// the effect fall off
  XReal m_Attenuation;
  /// the maximum distance that this node has an effect
  XReal m_MaxDistance;
  /// does the max distance have any effect?
  bool m_bUseMaxDistance;
  /// is the calculation done per vertex?
  bool m_bPerVertex;
  /// an array of ID's for shapes that are influenced by this field
  XIndexList m_Shapes;
  /// a volume for this dynamics field to act within.
  XVolumeType m_VolumeType;
  /// the sweep angle of the volume primitive (0 to 2*PI)
  XReal m_SweepAngle;
  /// the inner radius 
  XReal m_SectionRadius;
  /// an offset vector for the volume
  XVector3 m_VolumeOffset;
  /// this flag determines if the field acts inside (false) or outside 
  /// the volume. 
  bool m_VolumeExclusion;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::Field, XField, "field");
#endif
}
