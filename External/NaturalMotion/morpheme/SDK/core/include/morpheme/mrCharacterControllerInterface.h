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
#ifndef MR_CHARACTER_CONTROLLER_H
#define MR_CHARACTER_CONTROLLER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMatrix34.h"
#include "sharedDefines/mSharedDefines.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class Network;

//----------------------------------------------------------------------------------------------------------------------
/// \brief Indicates a property component of a character controller.
///
/// For example height, radius, shape, collision, behaviour state etc.
/// Which property types are relevant are specific to a particular character controller implementation.
typedef uint16_t CCPropertyType;

//----------------------------------------------------------------------------------------------------------------------
/// \brief The priority of an override vs other active overrides.
///
/// If within a frame a character controller property is overridden more than once the override with the 
/// highest priority will be set.
typedef uint16_t CCOverridePriority;
const CCOverridePriority MAX_CC_OVERRIDE_PRIORITY = 0x7FFF;

//----------------------------------------------------------------------------------------------------------------------
/// Some basic character controller property type identifiers.

/// \brief Scaling of the horizontal dimension of the character controller: Positive AttribDataFloat.
///  e.g. For a capsule controller this should scale radius.
const CCPropertyType CC_PROPERTY_TYPE_HORIZONTAL_SCALE         = GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 0);

/// \brief Scaling of the vertical dimension of the character controller: Positive AttribDataFloat.
///  This should scale the height of the controller.
const CCPropertyType CC_PROPERTY_TYPE_VERTICAL_SCALE           = GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 1);

/// \brief Character controller world position base: World space AttribDataVector3.
///  Position base generally reflects the world position of the kinematic characters trajectory root bone.
///  This is not necessarily the true position of the character capsule, this is given by:
///  the position base of the controller + the position offset of the controller.
///  This feature accounts for situations where the position of the controller is not well placed
///  relative to its collision shape. e.g. position being in the center of a character capsule.
const CCPropertyType CC_PROPERTY_TYPE_POSITION_ABSOLUTE        = GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 3);

/// \brief Character controller offset from its world base position: Character Controller space AttribDataVector3.
///  This value is expressed relative the characters current orientation (if it has one).
///  It accounts for situations where the position of the controller is not well placed
///  relative to its collision shape. e.g. position being in the center of a character capsule.
const CCPropertyType CC_PROPERTY_TYPE_POSITION_OFFSET          = GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 4);
const CCPropertyType CC_PROPERTY_TYPE_ROTATION_OFFSET          = GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 5);

/// \brief The internal state of a character controller: Positive AttribDataUInt.
///  Can be used if the character controller implementation has a concept of internal state.
const CCPropertyType CC_PROPERTY_TYPE_VERTICAL_MOVEMENT_STATE  = GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 7);

static const uint32_t CC_STATE_VERTICAL_MOVEMENT_GRAVITY   = 0;  // Vertical movement is gravity driven.
static const uint32_t CC_STATE_VERTICAL_MOVEMENT_ANIMATION = 1;  // Vertical movement is animation driven.

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::CharacterControllerInterface
/// \brief A virtual base class that defines an interface. A Network uses an implementation of this interface to 
///  communicate with an external character controller.
///
/// The character controller will be a structure created and managed outside of morpheme core.
///  It may for example be a gameCharacter class that holds a reference to a physics engine implementation of a 
///  character controller.
/// The user must provide a Network with a pointer to an instance of this class if they want communication between
///  their game character controller and the Network to happen.
///  This communication is important for:
///       + Updating the world position of character controllers,
///       + Setting controller properties such as height, width, shape and gravity control from the Network,
///       + etc.
//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerInterface
{
protected:
  CharacterControllerInterface() {}
  virtual ~CharacterControllerInterface() {}

public:
  
  //--------------------------------------------------------------------------------------------------------------------
  // Functions that can be called by the application
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief Cast a ray into the game's collision world. Returns true/false to indicate a hit, and if there is a hit
  ///  then hitDist etcetera will be set. The CharacterController and any associated physics rig will be ignored in the
  ///  calculation.
  ///
  /// Note: the base class implementation just returns false, to say that a collision never happened.
  virtual bool castRayIntoCollisionWorld(
    const NMP::Vector3& start,
    const NMP::Vector3& delta,
    float&              hitDist,
    NMP::Vector3&       hitPosition,
    NMP::Vector3&       hitNormal,
    NMP::Vector3&       hitVelocity,
    Network*            network) const = 0;

  /// \brief Store a requested property change on the character controller.
  /// \return true if property was actually set, 
  ///         false if set did not happen (not high enough priority, property type not handled).
  ///
  /// If the priority of this request is higher than previous requests then this property will 
  /// be cached for later application to the character controller.
  /// Cached values are applied at the next character controller update phase.
  /// Note: that if there are no requested properties set in a frame then the character controllers properties 
  /// will be reset to the authored defaults. This means means that maintaining a state other than the default
  /// requires setting every frame.
  virtual bool setRequestedPropertyOverride(
    CCPropertyType     propertyType,    ///< The type of property we are trying to set. e.g. height, radius, shape, collision, behaviour state etc.
    AttribData*        property,        ///< The property value to apply. This data is interpreted internally, based on the propertyType provideded.
    CCOverridePriority priority,        ///< If this priority is higher than that of previous requests then this request will take priority .
    FrameCount         frameIndex) = 0; ///< The frame on which this override is being requested.
                                        ///<  If the frame index is different from previous request this request will take priority.

  /// \brief Get a requested property change on the character controller.
  /// \return NULL if there is no cached requested override of this property type for this frame.
  virtual const AttribData* getRequestedPropertyOverride(
    CCPropertyType propertyType,         ///< The type of property we are trying to get. e.g. height, radius, shape, collision, behaviour state etc.
    FrameCount     frameIndex) const = 0;

  //----------------------------------------------------------------------------------------------------------------------
  /// \struct MR::OverrideStatus
  /// \brief Stores information about the last setting of a character controller parameter.
  ///
  /// Used by inherited classes to determine if a new parameter set if of higher priority than a previous one.
  //----------------------------------------------------------------------------------------------------------------------
  struct OverrideStatus
  {
    /// \brief If the frameIndex is not equal to the last set frame index or the priority is greater than the
    ///  last set priority then  return true, otherwise return false.
    NM_INLINE bool canBeUpdated(
      FrameCount         frameIndex,
      CCOverridePriority priority);
    
    /// \brief Store these new values internally. Asserts that these values can validly be set.
    NM_INLINE void update(
      FrameCount         frameIndex,
      CCOverridePriority priority);

    FrameCount         m_lastFrameSet;     ///< The last frame this value was set.
    CCOverridePriority m_lastSetPriority;  ///< The priority of the last set value.
  };

protected:

  /// Padding for 16 byte alignment.
#ifdef NM_HOST_64_BIT
  uint8_t  m_pad[8]; // Includes allowance for 64 bit vtable ptr.  
#else
  uint8_t  m_pad[12]; // Includes allowance for 32 bit vtable ptr.  
#endif // NM_HOST_64_BIT
};

//----------------------------------------------------------------------------------------------------------------------
// LastSetStatus functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool CharacterControllerInterface::OverrideStatus::canBeUpdated(
  FrameCount         frameIndex,
  CCOverridePriority priority)
{
  if ((frameIndex != m_lastFrameSet) || (priority > m_lastSetPriority))
  {
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void CharacterControllerInterface::OverrideStatus::update(
  FrameCount         frameIndex,
  CCOverridePriority priority)
{
  NMP_ASSERT((frameIndex != m_lastFrameSet) || (priority > m_lastSetPriority));
  m_lastFrameSet = frameIndex;
  m_lastSetPriority = priority; 
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_CHARACTER_CONTROLLER_H
//----------------------------------------------------------------------------------------------------------------------
