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
#include "Analysis/ScatterBlend2DBSPTreeAnalyser.h"
#include "NMNumerics/NMNumericUtils.h"
#include "XMD/Model.h"
#include "export/mcExportXml.h"
#include "assetProcessor/AssetProcessor.h"
#include "Analysis/XMDNetworkControlTake.h"
#include "NodeScatterBlendBuilder.h"
#include "NodeScatterBlend2DAnnotation.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool ScatterBlend2DBSPTreeAnalyser::analyse(
  AP::AssetProcessor* processor,
  const ME::AnalysisTaskExport* task,
  ME::DataBlockExport* resultData)
{
  NMP_VERIFY_MSG(task->getNumAnalysisNodes() == 1, "Expected 1 input analysisNode");

  // get the exported analysisNode data
  const ME::AnalysisNodeExport* stat = task->getAnalysisNode(0);
  NMP_VERIFY(strcmp(stat->getTypeName(), "ScatterBlend2DBSPTreeAnalyser") == 0);

  const ME::DataBlockExport* statDataBlock = stat->getDataBlock();
  NMP_VERIFY(statDataBlock);

  // read the location of the network to calculate the BSP for
  std::string exportedNetworkPath;
  statDataBlock->readString(exportedNetworkPath, "ExportedNetworkPath");

  uint32_t animSetIndex = 0xFFFFFFFF;
  statDataBlock->readUInt(animSetIndex, "AnalysisAnimSetIndex");
  NMP_VERIFY(animSetIndex != 0xFFFFFFFF);


  // attempt to load the exported asset
  ME::ExportFactoryXML exportFactory;
  ME::AssetExport* loadedAsset = exportFactory.loadAsset(exportedNetworkPath.c_str());
  NMP_VERIFY(loadedAsset);

  // find the scatter blend export
  ME::NetworkDefExport* networkExport = dynamic_cast<ME::NetworkDefExport*>(loadedAsset);
  NMP_VERIFY(networkExport);

  ME::NodeExport* nodeDefExport = networkExport->getNode(networkExport->getRootNodeID());
  NMP_VERIFY(nodeDefExport);
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_SCATTER_BLEND_2D);

  // Read the vertex samples from the export data
  std::vector<MR::NodeID> sourceNodeIDs;
  SB2DAnnotationExportDataPacked annotationVertexData;
  NodeScatterBlendBuilder::loadScatterBlendVertexSources(
    (MR::AnimSetIndex)animSetIndex,
    nodeDefExport,
    sourceNodeIDs,
    annotationVertexData);

  // Initialise the annotation graph with the vertex samples
  SB2DAnnotation annotation;
  annotation.setAnnotation(annotationVertexData);

  // Read the triangle annotations from the export data
  bool status = NodeScatterBlendBuilder::loadScatterBlendTriangleAnnotations(
    (MR::AnimSetIndex)animSetIndex,
    nodeDefExport,
    processor,
    annotationVertexData,
    annotation,
    sourceNodeIDs,
    true);

  NMP_VERIFY_MSG(
    status,
    "In ScatterBlend2DBSPTreeAnalyser: Failed to load the triangle annotations from exported network.");

  // Convert the annotation graph into the packed export format
  SB2DAnnotationExportDataPacked annotationExportData;
  annotation.getAnnotation(annotationExportData);

  // Compute the binary space partitioning of the sub-division triangles
  SB2DAnnotationBSPTree annotationBSPTree;
  annotationBSPTree.computeBSPTree(annotationExportData);

  // Convert the BSP tree into the packed export format
  SB2DAnnotationBSPTreeExportData annotationBSPTreeExportData;
  annotationBSPTree.getBSPTree(annotationBSPTreeExportData);

  // Write the export data to the analysis results
  annotationBSPTreeExportData.writeDataBlock(resultData);

  return true;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
