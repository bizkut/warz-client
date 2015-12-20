//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma once
#endif
#ifndef MR_NODE_SCALE_CHARACTER_BUILDER_H
#define MR_NODE_SCALE_CHARACTER_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

  //----------------------------------------------------------------------------------------------------------------------
  /// \class AP::NodeScaleCharacterBuilder
  /// \brief For construction of ScaleCharacter Node.
  /// \ingroup
  //----------------------------------------------------------------------------------------------------------------------
  class NodeScaleCharacterBuilder : public NodeDefBuilder
  {
  public:
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
  };

}

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_SCALE_CHARACTER_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
