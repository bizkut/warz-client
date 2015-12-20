//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma once
#endif
#ifndef MR_NODE_RETARGET_BUILDER_H
#define MR_NODE_RETARGET_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"
#include "NMRetarget/NMFKRetarget.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeRetargetBuilder
/// \brief For construction of Retarget Node.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeRetargetBuilder : public NodeDefBuilder
{
public:

  static void getAnimSetsFromDataBlockExport( const ME::DataBlockExport* nodeDefDataBlock, 
                                        std::vector<MR::AnimSetIndex> & animSets,
                                        uint32_t numAnimationSets );

  virtual bool hasSemanticLookupTable() NM_OVERRIDE;
  virtual MR::SemanticLookupTable* initSemanticLookupTable(NMP::Memory::Resource& memRes) NM_OVERRIDE;

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
  void initTaskQueuingFns(
    MR::NodeDef*                nodeDef,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    NMP::BasicLogger*           logger);

  // Go through all the retarget mappings in all the rigs counting the various metrics needed to
  // initialise the solver storage stats, which are used to determine how much memory to allocate
  // the retarget solver at runtime.
  void calculateRetargetMetrics(
    const ME::AnimationLibraryExport* animLibraryExport,
    uint32_t& intermediateRigSize,
    uint32_t& maxRigSize,
    uint32_t& mostMappedJoints,
    uint32_t& longestSequence);
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_RETARGET_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
