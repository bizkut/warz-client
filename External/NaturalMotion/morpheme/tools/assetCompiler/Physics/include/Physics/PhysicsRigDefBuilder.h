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
#ifndef MR_PHYSICSRIGBUILDER_H
#define MR_PHYSICSRIGBUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"

#include "assetProcessor/AssetProcessor.h"

#include "export/mcExportPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
struct PhysicsJointDef;
}

namespace AP
{
class PhysicsDriverDataBuilder;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::PhysicsRigDefBuilder
/// \brief For construction of a PhysicsRigDef from a PhysicsRigDef Description.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class PhysicsRigDefBuilder
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    AP::AssetProcessor*            processor,
    AP::PhysicsDriverDataBuilder*  physicsDriverBuilder,
    const ME::PhysicsRigDefExport* rigExport);

  static NMP::Memory::Resource init(
    AP::AssetProcessor*            processor,
    AP::PhysicsDriverDataBuilder*  physicsDriverBuilder,
    const ME::PhysicsRigDefExport* rigExport);

protected:
  static MR::PhysicsJointDef* initSixDOFJoint(
    NMP::Memory::Resource&        resource,
    const ME::PhysicsJointExport* jointExport);

  static MR::PhysicsJointDef* initRagdollJoint(
    NMP::Memory::Resource&        resource,
    const ME::PhysicsJointExport* jointExport);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICSRIGBUILDER_H
//----------------------------------------------------------------------------------------------------------------------
