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
#ifndef MR_BASIC_CHARACTER_CONTROLLER_H
#define MR_BASIC_CHARACTER_CONTROLLER_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrCharacterControllerInterfaceBase.h"
#include "NMPlatform/NMMemory.h"
#include "acCore.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// This BasicCharacterController maintains the position and rotation of an instantiated character network
class BasicCharacterController : public MR::CharacterControllerInterfaceBase
{
public:
  AC_CORE_EXPORT static NMP::Memory::Format getMemoryRequirements();

  AC_CORE_EXPORT static BasicCharacterController* init(
    NMP::Memory::Resource& resource,
    const NMP::Vector3&    characterStartPosition,
    const NMP::Quat&       characterStartRotation);

  AC_CORE_EXPORT static BasicCharacterController* createAndInit(
    const NMP::Vector3& characterStartPosition,
    const NMP::Quat&    characterStartRotation);

public:
  AC_CORE_EXPORT BasicCharacterController()
  {
    init(
      NMP::Vector3(NMP::Vector3::InitZero),
      NMP::Quat(NMP::Quat::kIdentity));
  }

  AC_CORE_EXPORT BasicCharacterController(
    const NMP::Vector3& characterStartPosition,
    const NMP::Quat&    characterStartRotation)
  {
    init(characterStartPosition, characterStartRotation);
  }

  AC_CORE_EXPORT ~BasicCharacterController() {};

  //----------------------------
  // Initialise the BasicCharacterController, setting its initial position and orientation
  AC_CORE_EXPORT void init(
    const NMP::Vector3& characterStartPosition,
    const NMP::Quat&    characterStartRotation);

  //----------------------------
  // Update the character controller after the network has been updated to get the new position and orientation
  AC_CORE_EXPORT void updateController(
    const NMP::Vector3& deltaTranslation,
    const NMP::Quat&    deltaOrientation);

  //----------------------------
  // Set the character position and orientation
  AC_CORE_EXPORT NM_INLINE int32_t setCharacterPosition(NMP::Vector3* position);
  AC_CORE_EXPORT NM_INLINE int32_t setCharacterOrientation(NMP::Quat* orientation);

  //----------------------------
  // Accessors
  AC_CORE_EXPORT NM_INLINE NMP::Vector3& getCharacterPosition()    { return m_characterPosition; }
  AC_CORE_EXPORT NM_INLINE NMP::Quat&    getCharacterOrientation() { return m_characterOrientation; }

  /// \brief Store a requested property change on the character controller.
  /// \return true if property was actually set, 
  ///         false if set did not happen (not high enough priority, property type not handled).
  AC_CORE_EXPORT virtual bool setRequestedPropertyOverride(
    MR::CCPropertyType     propertyType,
    MR::AttribData*        property,
    MR::CCOverridePriority priority,
    MR::FrameCount         frameIndex) NM_OVERRIDE;

  /// \brief Get a requested property change on the character controller.
  /// \return true if there is a cached requested override of this property type, false otherwise.
  AC_CORE_EXPORT virtual MR::AttribData* getRequestedPropertyOverride(
    MR::CCPropertyType propertyType,
    MR::FrameCount     frameIndex) const NM_OVERRIDE;

private:
  NMP::Vector3 m_requestedMovement;

  NMP::Vector3 m_characterPosition;    // in World Space
  NMP::Quat    m_characterOrientation; // in World Space

  NMP::Vector3 m_characterPositionOld;
  NMP::Quat    m_characterOrientationOld;

  NMP::Vector3 m_deltaTranslation; // in local space
  NMP::Quat    m_deltaOrientation; // in local space
};

//----------------------------------------------------------------------------------------------------------------------
// BasicCharacterController functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int32_t BasicCharacterController::setCharacterPosition(NMP::Vector3* position)
{
  m_characterPosition = *position;
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int32_t BasicCharacterController::setCharacterOrientation(NMP::Quat* orientation)
{
  m_characterOrientation = *orientation;
  return 0;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_BASIC_CHARACTER_CONTROLLER_H
//----------------------------------------------------------------------------------------------------------------------
