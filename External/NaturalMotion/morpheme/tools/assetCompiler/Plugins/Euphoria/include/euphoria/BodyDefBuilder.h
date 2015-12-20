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
#ifndef MR_BODYDEFBUILDER_H
#define MR_BODYDEFBUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"

#include "assetProcessor/AssetProcessor.h"
#include "physics/mrPhysicsRigDef.h"
#include "export/mcExportBody.h"
//----------------------------------------------------------------------------------------------------------------------


namespace ER
{
  class BodyDef;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
bool dislocateBodyDef(void* object);

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
bool bodySetRigAndPhysicsRig(AP::ProcessedAsset* asset, MR::AnimRigDef* rig, MR::PhysicsRigDef* physicsRigDef);

//----------------------------------------------------------------------------------------------------------------------
/// \brief For construction of a BodyDef from a BodyDef Description.
//----------------------------------------------------------------------------------------------------------------------
class BodyDefBuilder
{

public:
  static NMP::Memory::Format getMemoryRequirements(
    AP::AssetProcessor*      processor,
    const ME::BodyDefExport* bodyExport,
    uint32_t numAnimBones
  );
  static NMP::Memory::Resource init(
    AP::AssetProcessor*      processor,
    const ME::BodyDefExport* bodyExport,
    uint32_t numAnimBones
  );
  static void buildBodyDefAnimPoseData(
    ER::BodyDef*            bodyDef,
    MR::AnimSetIndex        animSetIndex,
    AP::AssetProcessor*     processor,
    ME::AnimationSetExport* animSetExport
  );
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_BODYDEFBUILDER_H
//----------------------------------------------------------------------------------------------------------------------
