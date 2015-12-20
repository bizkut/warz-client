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
#ifndef MR_MESSAGE_BUILDER_H
#define MR_MESSAGE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"

#include "nmtl/pod_vector.h"

#include "export/mcExportMessagePresets.h"

#include "assetProcessor/AssetProcessor.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
/// \class AP::MessageBuilder
/// \brief For construction of Message presets in the asset compiler - pure virtual base class
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class MessageBuilder
{
public:
  virtual ~MessageBuilder() { }

  virtual NMP::Memory::Format getMemoryRequirements(
    const ME::MessageExport*        messageExport,
    const ME::MessagePresetExport*  messagePresetExport,
    AssetProcessor*                 processor) = 0;

  virtual MR::Message* init(
    NMP::Memory::Resource&          memRes,
    const ME::MessageExport*        messageExport,
    const ME::MessagePresetExport*  messagePresetExport,
    AssetProcessor*                 processor) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::MessageDistributorBuilder
/// \brief For construction of Message distributor in the asset compiler.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class MessageDistributorBuilder
{
public:

  static NMP::Memory::Format getMemoryRequirements(
    const ME::MessageExport*            messageExport,
    const ME::MessagePresetsListExport*     messagePresetsExport,
    const nmtl::pod_vector<MR::NodeID>& interestedNodes,
    AssetProcessor*                     processor);

  static MR::MessageDistributor* init(
    NMP::Memory::Resource&              memRes,
    const ME::MessageExport*            messageExport,
    const ME::MessagePresetsListExport*     messagePresetsExport,
    const nmtl::pod_vector<MR::NodeID>& interestedNodes,
    AssetProcessor*                     processor);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_MESSAGE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
