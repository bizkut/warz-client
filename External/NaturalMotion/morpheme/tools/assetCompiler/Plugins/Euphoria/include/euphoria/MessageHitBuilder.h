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
#ifndef MR_MESSAGE_HIT_BUILDER_H
#define MR_MESSAGE_HIT_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/MessageBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
/// \class AP::MessageHitBuilder
/// \brief For construction of Hit messages.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class MessageHitBuilder : public MessageBuilder
{
public:
  virtual NMP::Memory::Format getMemoryRequirements(
    const ME::MessageExport*        messageExport,
    const ME::MessagePresetExport*  messagePresetExport,
    AssetProcessor*                 processor) NM_OVERRIDE;

  virtual MR::Message* init(
    NMP::Memory::Resource&          memRes,
    const ME::MessageExport*        messageExport,
    const ME::MessagePresetExport*  messagePresetExport,
    AssetProcessor*                 processor) NM_OVERRIDE;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_MESSAGE_HIT_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
