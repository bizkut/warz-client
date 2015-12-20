// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_NODE_SCATTER_BLEND_1D_BUILDER_H
#define MR_NODE_SCATTER_BLEND_1D_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"
#include "assetProcessor/BlendNodeBuilderUtils.h"
#include "NodeScatterBlend1DAnnotationExport.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ScatterBlend1DExportInfo
/// \brief A structure to contain the scatter blend header information (blending options etc) that
/// are exported from connect.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class ScatterBlend1DExportInfo
{
public:
  ScatterBlend1DExportInfo() {}
  ~ScatterBlend1DExportInfo() {}

  void readHeaderInfo(
    const ME::NodeExport* nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor* processor);

public:
  MR::AnimSetIndex  m_numAnimSets;

  // Unit scaling: Converts ControlParameter space units to ScatterBlend units
  float             m_unitScale;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeScatterBlend1DPreInitData
/// \brief A structure to contain the compiled binary ready attribute data for the scatter blend node.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeScatterBlend1DPreInitData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numAnimSets);
  static NodeScatterBlend1DPreInitData* init(NMP::Memory::Resource& resource, uint32_t numAnimSets);
  static NodeScatterBlend1DPreInitData* createAndInit(uint32_t numAnimSets);

  NodeScatterBlend1DPreInitData() {}
  ~NodeScatterBlend1DPreInitData() {}

  void releaseAndDestroy();

public:
  bool                          m_exportForAnalysis;    ///< Mode of operation flag: Analysis or preview

  uint32_t                      m_numAnimSets;          ///< The number of animation sets
  MR::AttribDataHandle*         m_attribDataHandles;    ///< The compiled animation set attribute data
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeScatterBlend1DBuilder
/// \brief
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeScatterBlend1DBuilder : public NodeDefBuilder
{
public:
  virtual bool hasSemanticLookupTable() NM_OVERRIDE;
  
  virtual MR::SemanticLookupTable* initSemanticLookupTable(
    NMP::Memory::Resource&        memRes) NM_OVERRIDE;

  virtual void getNodeDefInputConnections(
    std::vector<MR::NodeID>&      childNodeIDs,
    const ME::NodeExport*         nodeDefExport,
    const ME::NetworkDefExport*   netDefExport,
    AssetProcessor*               processor) NM_OVERRIDE;

  virtual void preInit(
    NetworkDefCompilationInfo*    netDefCompilationInfo,
    const ME::NodeExport*         nodeDefExport,
    const ME::NetworkDefExport*   netDefExport,
    AssetProcessor*               processor) NM_OVERRIDE;

  virtual NMP::Memory::Format getNodeDefMemoryRequirements(
    NetworkDefCompilationInfo*    netDefCompilationInfo,
    const ME::NodeExport*         nodeDefExport,
    const ME::NetworkDefExport*   netDefExport,
    AssetProcessor*               processor) NM_OVERRIDE;

  virtual MR::NodeDef* init(
    NMP::Memory::Resource&        memRes,
    NetworkDefCompilationInfo*    netDefCompilationInfo,
    const ME::NodeExport*         nodeDefExport,
    const ME::NetworkDefExport*   netDefExport,
    AssetProcessor*               processor) NM_OVERRIDE;

private:
  static void buildAttribDataForAnalysis(
    NetworkDefCompilationInfo*      netDefCompilationInfo,
    const ME::NodeExport*           nodeDefExport,
    const ME::NetworkDefExport*     netDefExport,
    AssetProcessor*                 processor);

  static void buildAttribDataForPreview(
    NetworkDefCompilationInfo*      netDefCompilationInfo,
    const ME::NodeExport*           nodeDefExport,
    const ME::NetworkDefExport*     netDefExport,
    AssetProcessor*                 processor);

  static NMP::Memory::Format getNodeDefMemoryRequirementsForAttribData(
    NetworkDefCompilationInfo*      netDefCompilationInfo,
    NodeScatterBlend1DPreInitData*  preInitData,
    const ME::NodeExport*           nodeDefExport);

  static MR::AttribDataHandle createAndInitAttribDataScatterBlend1DDef(
    MR::AnimSetIndex animSetIndex,
    const ScatterBlend1DExportInfo& scatterBlendExportInfo,
    const SB1DAnnotationExportDataPacked& annotationExportData,
    const SB1DAnnotationSubSampleRangeData& annotationRangeData);

  /// \brief Initialisation function for asset compilation of the offline sub-sample data
  static MR::NodeDef* initForAnalysis(
    NMP::Memory::Resource&          memRes,
    NetworkDefCompilationInfo*      netDefCompilationInfo,
    NodeScatterBlend1DPreInitData*  preInitData,
    const ME::NodeExport*           nodeDefExport,
    AssetProcessor*                 processor);

  /// \brief Initialisation function for the fully annotated attribute data
  static MR::NodeDef* initForPreview(
    NMP::Memory::Resource&          memRes,
    NetworkDefCompilationInfo*      netDefCompilationInfo,
    NodeScatterBlend1DPreInitData*  preInitData,
    const ME::NodeExport*           nodeDefExport,
    AssetProcessor*                 processor);

  // Initialise as correct NodeDef type; fills in queuing ops table and outputCP ops table.
  static void initTaskQueuingFnsBase(
    MR::NodeDef*                    nodeDef,
    NetworkDefCompilationInfo*      netDefCompilationInfo,
    NodeTimeStretchModes            timeStretchMode,
    bool                            slerpDeltaTrajectoryTranslation,
    NMP::BasicLogger*               logger);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_SCATTER_BLEND_1D_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
