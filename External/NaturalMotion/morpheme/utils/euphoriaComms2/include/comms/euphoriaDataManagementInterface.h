// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef MCOMMS_EUPHORIADATAMANAGEMENTINTERFACE_H
#define MCOMMS_EUPHORIADATAMANAGEMENTINTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
#include "sharedDefines/mEuphoriaDebugInterface.h"

#include "comms/runtimeTargetInterface.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class EuphoriaDataManagementInterface
{
protected:
  /// EuphoriaDataManagementInterface is a pure virtual interface.
  EuphoriaDataManagementInterface() {}

public:

  virtual ~EuphoriaDataManagementInterface() {}

  /// \brief Return the number of modules in the given network definition.
  virtual uint32_t getModuleCount(InstanceID id) const = 0;

  /// \brief Return the name of the given module's parent module
  virtual const char* getModuleParentName(InstanceID id, uint32_t moduleIndex) const = 0;

  /// \brief Return the UTF-8 name of the given module.
  virtual const char* getModuleName(InstanceID id, uint32_t moduleIndex) const = 0;

  /// \brief Queries if a modules debug information is
  virtual bool isModuleDebugEnabled(InstanceID id, uint32_t moduleIndex) const = 0;

  /// \brief Enables or disables the debug output of a given module.
  virtual bool enableModuleDebug(InstanceID id, uint32_t moduleIndex, bool enable) = 0;

  /// \brief Return the number of euphoria debug controls this runtime target has.
  virtual uint32_t getDebugControlCount(InstanceID id) = 0;

  /// \brief Get information about a debug control, is it a per limb control and which data type.
  /// Returns false if index doesn't point to a valid debug control index.
  virtual bool getDebugControlInfo(
    InstanceID                id,
    uint32_t                  index,
    const char**              controlName,
    ER::DebugControlID*       controlId,
    ER::DebugControlFlags*    controlFlags,
    ER::DebugControlDataType* controlDataType) = 0;

  /// \brief Return the number of control types that this instance supports.
  virtual uint32_t getNumControlTypes(InstanceID id) const = 0;

  /// \brief Returns the name of the specified control type
  virtual const char* getControlTypeName(InstanceID id, uint32_t controlTypeIndex) = 0;

  /// \brief Sets a debug control boolean value for a specific instance. Returns false if any of the values
  /// are invalid.
  virtual bool setDebugControl(InstanceID id, ER::DebugControlID controlId, uint8_t limbIndex, bool value) = 0;
  /// \brief Gets a debug control boolean value for a specific instance. Returns false if the controlId or limbIndex
  /// are invalid or if the control is not a boolean control.
  virtual bool getDebugControl(InstanceID id, ER::DebugControlID controlId, uint8_t limbIndex, bool* value) = 0;

  /// \overload Overload for setting int32_t debug controls.
  virtual bool setDebugControl(InstanceID id, ER::DebugControlID controlId, uint8_t limbIndex, int32_t value) = 0;
  /// \overload Overload for getting int32_t debug controls.
  virtual bool getDebugControl(InstanceID id, ER::DebugControlID controlId, uint8_t limbIndex, int32_t* value) = 0;

  /// \overload Overload for setting uint32_t debug controls.
  virtual bool setDebugControl(InstanceID id, ER::DebugControlID controlId, uint8_t limbIndex, uint32_t value) = 0;
  /// \overload Overload for getting uint32_t debug controls.
  virtual bool getDebugControl(InstanceID id, ER::DebugControlID controlId, uint8_t limbIndex, uint32_t* value) = 0;

  /// \overload Overload for setting float debug controls.
  virtual bool setDebugControl(InstanceID id, ER::DebugControlID controlId, uint8_t limbIndex, float value) = 0;
  /// \overload Overload for getting float debug controls.
  virtual bool getDebugControl(InstanceID id, ER::DebugControlID controlId, uint8_t limbIndex, float* value) = 0;

  /// \brief Return the number of limbs in the euphoria body.
  virtual uint32_t getLimbCount(InstanceID id) const = 0;

  /// \brief Return the LimbControlAmounts for the given limb, or zero if not available. It will
  ///        also set stiffnessFraction to indicate how strongly the arm is acting.
  virtual const ER::LimbControlAmounts* getLimbControlAmounts(InstanceID id, uint8_t limbIndex, float& stiffnessFraction) const = 0;

  /// \brief Serialize the euphoria body
  virtual uint32_t serializeLimbTx(InstanceID instanceID, uint32_t limbIndex, void* outputBuffer, uint32_t outputBufferSize) const = 0;

  /// \brief Serialize the euphoria body
  virtual uint32_t serializeBodyTx(InstanceID instanceID, void* outputBuffer, uint32_t outputBufferSize) const = 0;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_EUPHORIADATAMANAGEMENTINTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
