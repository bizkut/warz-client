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
#ifndef MR_NODE_SCATTER_BLEND_BUILDER_H
#define MR_NODE_SCATTER_BLEND_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"
#include "assetProcessor/BlendNodeBuilderUtils.h"
//----------------------------------------------------------------------------------------------------------------------

// nonstandard extension used : nameless struct/union
#ifdef NM_COMPILER_MSVC
#pragma warning (push)
#pragma warning (disable : 4201)
#endif

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
class SB2DAnnotationExportDataPacked;
class SB2DAnnotationBSPTreeExportData;
class SB2DAnnotationSubSampleRangeData;
class SB2DAnnotation;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ScatterBlendExportInfo
/// \brief A structure to contain the scatter blend header information (blending options etc) that
/// are exported from connect.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class ScatterBlendExportInfo
{
public:
  ScatterBlendExportInfo() {}
  ~ScatterBlendExportInfo() {}

  void readHeaderInfo(
    const ME::NodeExport* nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor* processor);

public:
  // Projection
  NMP::Vector3                        m_projectionVector;
  bool                                m_projectionUseCentre;
  MR::ScatterBlend2DProjectionMode    m_projectionMode;

  // Display scales (perAnimSet): Used in InAxisForFacet projection to scale the
  // scatter blend data to a similar aspect ratio to the scatter blend viewport.
  MR::AnimSetIndex                    m_numAnimSets;
  float                               m_displayScalesX[8];
  float                               m_displayScalesY[8];

  // Unit scaling: Converts ControlParameter space units to ScatterBlend units
  float                               m_unitScaleX;
  float                               m_unitScaleY;

  // Other information
  const char*                         m_nodeName;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeScatterBlendPreInitData
/// \brief A structure to contain the compiled binary ready attribute data for the scatter blend node.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeScatterBlendPreInitData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numAnimSets);
  static NodeScatterBlendPreInitData* init(NMP::Memory::Resource& resource, uint32_t numAnimSets);
  static NodeScatterBlendPreInitData* createAndInit(uint32_t numAnimSets);

  NodeScatterBlendPreInitData() {}
  ~NodeScatterBlendPreInitData() {}

  void releaseAndDestroy();

public:
  bool                          m_exportForAnalysis;    ///< Mode of operation flag: Analysis or preview

  uint32_t                      m_numAnimSets;          ///< The number of animation sets
  MR::AttribDataHandle*         m_attribDataHandles;    ///< The compiled animation set attribute data
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeScatterBlendBuilder
/// \brief Builder for the scatter blend node
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeScatterBlendBuilder : public NodeDefBuilder
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

  //---------------------------
  // Node builder helper functions
  //
  static void loadScatterBlendVertexSources(
    MR::AnimSetIndex                animSetIndex,
    const ME::NodeExport*           nodeDefExport,    
    std::vector<MR::NodeID>&        sourceNodeIDs,
    SB2DAnnotationExportDataPacked& annotationExportData);

  static bool loadScatterBlendTriangleAnnotations(
    MR::AnimSetIndex                      animSetIndex,
    const ME::NodeExport*                 nodeDefExport,
    AssetProcessor*                       processor,
    const SB2DAnnotationExportDataPacked& annotationExportData,
    SB2DAnnotation&                       annotation,
    std::vector<MR::NodeID>&              sourceNodeIDs,
    bool                                  loadForAnalysis);

  static const ME::AnalysisNodeExport* findScatterBlendBSPTree(
    MR::AnimSetIndex                      animSetIndex,
    const ME::NodeExport*                 nodeDefExport);

  static bool loadScatterBlendBSPTree(
    const ME::AnalysisNodeExport* bspAnalysisNode,
    const SB2DAnnotationExportDataPacked& annotationExportData,
    SB2DAnnotationBSPTreeExportData&      annotationBSPTreeExportData);

  static void computeRangeOfAnnotationSubSamples(
    const ME::NodeExport* nodeDefExport,
    AssetProcessor* processor,
    const SB2DAnnotationExportDataPacked& annotationExportData,
    SB2DAnnotationSubSampleRangeData& annotationRangeData);

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
    NodeScatterBlendPreInitData*    preInitData,
    const ME::NodeExport*           nodeDefExport);

  static MR::AttribDataHandle createAndInitAttribDataScatterBlend2DDef(
    MR::AnimSetIndex animSetIndex,
    const ScatterBlendExportInfo& scatterBlendExportInfo,
    const SB2DAnnotationExportDataPacked& annotationExportData,
    const SB2DAnnotationBSPTreeExportData& annotationBSPTreeExportData,
    const SB2DAnnotationSubSampleRangeData& annotationRangeData);

  /// \brief Initialisation function for asset compilation of the offline sub-sample data
  static MR::NodeDef* initForAnalysis(
    NMP::Memory::Resource&          memRes,
    NetworkDefCompilationInfo*      netDefCompilationInfo,
    NodeScatterBlendPreInitData*    preInitData,
    const ME::NodeExport*           nodeDefExport,
    AssetProcessor*                 processor);

  /// \brief Initialisation function for the fully annotated attribute data
  static MR::NodeDef* initForPreview(
    NMP::Memory::Resource&          memRes,
    NetworkDefCompilationInfo*      netDefCompilationInfo,
    NodeScatterBlendPreInitData*    preInitData,
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
#endif // MR_NODE_SCATTER_BLEND_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
