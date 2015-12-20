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
#ifndef MR_CHARACTERCONTROLLERDEFBUILDER_H
#define MR_CHARACTERCONTROLLERDEFBUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"

#include "assetProcessor/AssetProcessor.h"
#include "morpheme/mrCharacterControllerDef.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief For construction of a CharacterControllerDef from a CharacterController Description.
//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerDefBuilder : public MR::CharacterControllerDef
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    AP::AssetProcessor*                  processor,
    const ME::CharacterControllerExport* controllerExport);

  static NMP::Memory::Resource init(
    AP::AssetProcessor*                  processor,
    const ME::CharacterControllerExport* controllerExport);
protected:
  static const char* ShapeTypeTokens[NumShapeTypes];
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_CHARACTERCONTROLLERDEFBUILDER_H
//----------------------------------------------------------------------------------------------------------------------
