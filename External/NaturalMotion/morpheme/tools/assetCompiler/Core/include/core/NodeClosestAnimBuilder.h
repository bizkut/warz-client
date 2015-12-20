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
#ifndef MR_NODE_CLOSEST_ANIM_BUILDER_H
#define MR_NODE_CLOSEST_ANIM_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeClosestAnimBuilder
/// \brief For construction of NodeClosestAnim.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeClosestAnimBuilder : public NodeDefBuilder
{
public:
  virtual bool hasSemanticLookupTable() NM_OVERRIDE;

  virtual MR::SemanticLookupTable* initSemanticLookupTable(
    NMP::Memory::Resource&      memRes) NM_OVERRIDE;

  virtual void getNodeDefInputConnections(
    std::vector<MR::NodeID>&    childNodeIDs,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) NM_OVERRIDE;

  virtual void preInit(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) NM_OVERRIDE;

  virtual NMP::Memory::Format getNodeDefMemoryRequirements(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) NM_OVERRIDE;

  virtual MR::NodeDef* init(
    NMP::Memory::Resource&      memRes,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) NM_OVERRIDE;

private:
  // Initialise as correct NodeDef type; fills in queuing ops table and outputCP ops table.
  static void initTaskQueuingFns(
    MR::NodeDef*                nodeDef,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const MR::AttribDataClosestAnimDef* closestAnimDefAttribData,
    NMP::BasicLogger*           logger);

  static float computePositionScaleFactor(const std::vector<MR::AnimRigDef*>& animRigs);

  static void computePosVel(
    uint32_t numAnimJoints,
    float updateTime,
    NMP::Vector3* animChannelPosKeys0,
    NMP::Vector3* animChannelPosKeys1,
    NMP::Vector3* channelVec3Buffer);

  static void computeAngVel(
    uint32_t numAnimJoints,
    float updateTime,
    NMP::Quat* animChannelAttKeys0,
    NMP::Quat* animChannelAttKeys1,
    NMP::Vector3* channelVec3Buffer);

  static void optimizeAnimChannelDistribution(
    std::vector<const char*>& conglomeratedRigBoneNames,
    std::vector<MR::AttribDataClosestAnimDefAnimSet*>& setupDataAnimSetArray);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_CLOSEST_ANIM_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
