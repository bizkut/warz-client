// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifdef _MSC_VER
  #pragma once
#endif
#ifndef ER_DEBUGCONTROLS_H
#define ER_DEBUGCONTROLS_H

#include "sharedDefines/mEuphoriaDebugInterface.h"

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
class Character;

//----------------------------------------------------------------------------------------------------------------------
enum EuphoriaDebugControlIDs
{
  // Debug Controls
  kIKSolverIterationsControlID = 0,
  kGravityCompensationControlID,
  kEnableJointLimitsControlID,
  kEnableSoftLimitsControlID,
  kEnableHamstringsControlID,
  kIKDrawingRelativeToRootControlID,
  kIKUsesJointLimits,

  // Debug Draw
  kDrawGravityCompensationTorquesControlID,
  kDrawGravityCompensationGravityControlID,
  kDrawHamstringsControlID,
  kDrawIKTargetsControlID,
  kDrawIKFinalResultsControlID,
  kDrawContactsControlID,
  kDrawDetailedContactsControlID,
  kDrawCharacterRootControlID,
  kDrawIKInputPoseControlID,
  kDrawIKGuidePoseControlID,
  kDrawGuidePoseRangeControlID,
  kDrawGuidePoseExtremesControlID,
  kDrawEndContraintControlID,

  kNumControlIDs
};

/// \brief Queries how many euphoria debug controls there are.
uint32_t getEuphoriaDebugControlCount();

/// \brief Get some info about a specific euphoria debug control.
bool getEuphoriaDebugControlInfo(
  uint32_t index,
  const char** controlName,
  DebugControlID* controlID,
  DebugControlFlags* controlFlags,
  DebugControlDataType* controlDataType);

/// \brief Get the name of a specific euphoria debug control.
const char* getEuphoriaDebugControlName(uint32_t index);

/// \brief Set a boolean control value for a euphoria character instance.
bool setEuphoriaDebugControl(ER::Character* character, DebugControlID controlID, uint8_t limbIndex, bool value);
/// \brief Get a boolean control value for a euphoria character instance.
bool getEuphoriaDebugControl(const ER::Character* character, DebugControlID controlID, uint8_t limbIndex, bool* value);

/// \overload Set an int32_t control.
bool setEuphoriaDebugControl(ER::Character* character, DebugControlID controlID, uint8_t limbIndex, int32_t value);
/// \overload Set an int32_t control.
bool getEuphoriaDebugControl(const ER::Character* character, DebugControlID controlID, uint8_t limbIndex, int32_t* value);

/// \overload Set an uint32_t control.
bool setEuphoriaDebugControl(ER::Character* character, DebugControlID controlID, uint8_t limbIndex, uint32_t value);
/// \overload Set an uint32_t control.
bool getEuphoriaDebugControl(const ER::Character* character, DebugControlID controlID, uint8_t limbIndex, uint32_t* value);

/// \overload Set an float control.
bool setEuphoriaDebugControl(ER::Character* character, DebugControlID controlID, uint8_t limbIndex, float value);
/// \overload Set an float control.
bool getEuphoriaDebugControl(const ER::Character* character, DebugControlID controlID, uint8_t limbIndex, float* value);

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // ER_DEBUGCONTROLS_H
//----------------------------------------------------------------------------------------------------------------------
