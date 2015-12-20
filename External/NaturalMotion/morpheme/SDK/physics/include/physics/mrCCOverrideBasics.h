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
#ifndef MR_CHARACTER_CONTROLLER_OVERRIDE_BASICS_H
#define MR_CHARACTER_CONTROLLER_OVERRIDE_BASICS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrCharacterControllerInterface.h"
//----------------------------------------------------------------------------------------------------------------------

class NxActor;

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::CCOverrideBasics
/// \brief Provides some basic provision for caching character controller property overrides.
///  For sharing between character controller implementations.
//----------------------------------------------------------------------------------------------------------------------
class CCOverrideBasics
{
public:
  /// \brief Set some default starting values.
  void init();

  /// \brief Store a requested property change on the character controller.
  /// \return true if property was actually set, 
  ///         false if set did not happen (not high enough priority, property type not handled).
  bool setRequestedPropertyOverride(
    CCPropertyType     propertyType,
    AttribData*        property,
    CCOverridePriority priority,
    FrameCount         frameIndex);
  
  /// \brief Get a requested property change on the character controller.
  /// \return NULL if there is no cached requested override of this property type for this frame.
  const AttribData* getRequestedPropertyOverride(
    CCPropertyType propertyType,
    FrameCount     frameIndex) const;

public:
  //--------------------------------
  // Cached override property values.
  NMP::Vector3      m_defaultPositionOffset; 
  NMP::Vector3      m_currentPositionOffset;            ///< The currently applied position offset. Defined in characters local space.
  AttribDataVector3 m_positionOffsetOverride;           ///< The cached override for the position offset of the character controller.
  NMP::Vector3      m_defaultPositionAbs; 
  NMP::Vector3      m_currentPositionAbs;               ///< The currently applied absolute position for the character controller. Defined in world space.
  AttribDataVector3 m_positionAbsOverride;              ///< The cached override for the absolute position of the character controller.

  CharacterControllerInterface::OverrideStatus m_positionOffsetOverrideStatus;   ///< The last frame the override was set and at what priority.
  CharacterControllerInterface::OverrideStatus m_positionAbsOverrideStatus;      ///< The last frame the override was set and at what priority.
  bool              m_overrideAbsPosition;              ///< Whether we should override the character controllers position this frame or not.
  
  float             m_defaultHorizontalScale;
  float             m_currentHorizontalScale;           ///< The currently applied fraction of the default character controller radius to use.
  AttribDataFloat   m_horizontalScaleOverride;          ///< The cached override for the fraction of the default character controller radius to use.
  CharacterControllerInterface::OverrideStatus m_horizontalScaleOverrideStatus;   ///< The last frame the override was set and at what priority.
  
  float             m_defaultVerticalScale;
  float             m_currentVerticalScale;             ///< The currently applied fraction of the default character controller height to use.
  AttribDataFloat   m_verticalScaleOverride;            ///< The cached override for the fraction of the default character controller height to use.
  CharacterControllerInterface::OverrideStatus m_verticalScaleOverrideStatus;     ///< The last frame the override was set and at what priority.
  
  uint32_t          m_defaultVerticalMoveState;
  uint32_t          m_currentVerticalMoveState;         ///< The currently applied vertical movement control state.
  AttribDataUInt    m_verticalMoveStateOverride;        ///< The cached override for the vertical movement control state.
  CharacterControllerInterface::OverrideStatus m_verticalMoveStateOverrideStatus; ///< The last frame the override was set and at what priority.
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_CHARACTER_CONTROLLER_OVERRIDE_BASICS_H
//----------------------------------------------------------------------------------------------------------------------
