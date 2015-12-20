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
#ifndef ER_INTERACTIONPROXYDEFBUILDER_H
#define ER_INTERACTIONPROXYDEFBUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"

#include "assetProcessor/AssetProcessor.h"
#include "euphoria/erInteractionProxyDef.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief For construction of a InteractionProxyDef from a InteractionProxy Description.
//----------------------------------------------------------------------------------------------------------------------
class InteractionProxyDefBuilder : public ER::InteractionProxyDef
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    AP::AssetProcessor*               processor,
    const ME::InteractionProxyExport* proxyExport);

  static NMP::Memory::Resource init(
    AP::AssetProcessor*               processor,
    const ME::InteractionProxyExport* proxyExport);
protected:
  static const char* ShapeTypeTokens[NumShapeTypes];
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ER_INTERACTIONPROXYDEFBUILDER_H
//----------------------------------------------------------------------------------------------------------------------
