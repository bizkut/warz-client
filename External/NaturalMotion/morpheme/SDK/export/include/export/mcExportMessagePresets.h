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
#ifndef MCEXPORTMESSAGEPRESETS_H
#define MCEXPORTMESSAGEPRESETS_H
//----------------------------------------------------------------------------------------------------------------------
#include "export/mcExport.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class MessagePresetExport
{
public:
  virtual ~MessagePresetExport() { }

  virtual const char* getName() const = 0;

  virtual DataBlockExport* getDataBlock() = 0;
  virtual const DataBlockExport* getDataBlock() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class MessagePresetsListExport
{
public:
  virtual ~MessagePresetsListExport() { }

  virtual MessagePresetExport* addMessagePreset(const wchar_t* name) = 0;

  virtual const char* getMessageName() const = 0;
  virtual MR::MessageType getMessageType() const = 0;

  virtual uint32_t getNumMessagePresets() const = 0;
  virtual const MessagePresetExport* getMessagePreset(uint32_t index) const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class MessagePresetLibraryExport : public AssetExport
{
public:
  virtual ~MessagePresetLibraryExport() { }

  virtual MessagePresetsListExport* createMessagePresetsList(MR::MessageType messageType, const wchar_t* messageName) = 0;

  virtual uint32_t getNumMessages() const = 0;
  virtual MessagePresetsListExport* getMessagePresets(uint32_t index) = 0;
  virtual const MessagePresetsListExport* getMessagePresets(uint32_t index) const = 0;

};

} // namespace ME

//----------------------------------------------------------------------------------------------------------------------
#endif // MCPHYSICSEXPORT_H
//----------------------------------------------------------------------------------------------------------------------
