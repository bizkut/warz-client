// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_CHARACTER_CONTROLLER_BASE_H
#define MR_CHARACTER_CONTROLLER_BASE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrCharacterControllerInterface.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::CharacterControllerInterfaceBase
/// \brief A non-physics implementation of CharacterControllerInterface.
//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerInterfaceBase : public CharacterControllerInterface
{
public:

  CharacterControllerInterfaceBase() {}
  virtual ~CharacterControllerInterfaceBase() {}
    
  //--------------------------------------------------------------------------------------------------------------------
  // Functions that can be called by the application
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief Cast a ray into the game's collision world. Returns true/false to indicate a hit, and if there is a hit
  ///  then hitDist etcetera will be set. The CharacterController and any associated physics rig will be ignored in the
  ///  calculation.
  /// \details This is an empty implementation of a pure virtual base class function.
  /// \return false, to say that a collision never happened.
  virtual bool castRayIntoCollisionWorld(
    const NMP::Vector3& start,
    const NMP::Vector3& delta,
    float&              hitDist,
    NMP::Vector3&       hitPosition,
    NMP::Vector3&       hitNormal,
    NMP::Vector3&       hitVelocity,
    MR::Network*        network) const NM_OVERRIDE;  

  /// \brief Store a requested property change on the character controller.
  /// \details This is an empty implementation of a pure virtual base class function.
  /// \return false, to say that the property was not set.
  virtual bool setRequestedPropertyOverride(
    CCPropertyType     propertyType,            ///< The type of property we are trying to set. e.g. height, radius, shape, collision, behaviour state etc.
    AttribData*        property,                ///< The property value to apply. This data is interpreted internally, based on the propertyType provideded.
    CCOverridePriority priority,                ///< If this priority is higher than that of previous requests then this request will take priority .
    FrameCount         frameIndex) NM_OVERRIDE; ///< The frame on which this override is being requested.
                                                ///<  If the frame index is different from previous request this request will take priority.

  /// \brief Get a requested property change on the character controller.
  /// \details This is an empty implementation of a pure virtual base class function.
  /// \return NULL as there is no caching of property overrides in this implementation.
  virtual const AttribData* getRequestedPropertyOverride(
    CCPropertyType propertyType,                 ///< The type of property we are trying to get. e.g. height, radius, shape, collision, behaviour state etc.
    FrameCount     frameIndex) const NM_OVERRIDE;
};

//---------------------------------------------------------------------------------------------------------------------
// CharacterControllerInterfaceBase functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool CharacterControllerInterfaceBase::castRayIntoCollisionWorld(const NMP::Vector3& NMP_UNUSED(start),
    const NMP::Vector3& NMP_UNUSED(delta),
    float&              NMP_UNUSED(hitDist),
    NMP::Vector3&       NMP_UNUSED(hitPosition),
    NMP::Vector3&       NMP_UNUSED(hitNormal),
    NMP::Vector3&       NMP_UNUSED(hitVelocity),
    Network*            NMP_UNUSED(network)) const
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool CharacterControllerInterfaceBase::setRequestedPropertyOverride(
  CCPropertyType     NMP_UNUSED(propertyType),
  AttribData*        NMP_UNUSED(property),
  CCOverridePriority NMP_UNUSED(priority),
  FrameCount         NMP_UNUSED(frameIndex))
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::AttribData* CharacterControllerInterfaceBase::getRequestedPropertyOverride(
  CCPropertyType NMP_UNUSED(propertyType),
  FrameCount     NMP_UNUSED(frameIndex)) const
{
  return NULL;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_CHARACTER_CONTROLLER_BASE_H
//----------------------------------------------------------------------------------------------------------------------
