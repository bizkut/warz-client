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
#ifndef MR_ND_MESH_AP_SEARCH_MAP_BUILDER_H
#define MR_ND_MESH_AP_SEARCH_MAP_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/Prediction/mrNDMesh.h"
#include "morpheme/Prediction/mrNDMeshAPSearchMap.h"
#include "Analysis/NDMeshQuantisationData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NDMeshBBTree
/// \brief An intermediate representation of the scale space bounding box tree used during asset compilation.
//----------------------------------------------------------------------------------------------------------------------
class NDMeshBBTree
{
public:
  class Node
  {
  public:
    static NMP::Memory::Format getMemoryRequirements(
      uint32_t numDimensions,
      uint32_t numComponentsPerSample);

    static Node* init(
      NMP::Memory::Resource& memRes,
      uint32_t               numDimensions,
      uint32_t               numComponentsPerSample);

    NM_INLINE bool isBranchNode() const { return m_childIndex[0] != 0; }
    NM_INLINE bool isLeafNode() const { return m_childIndex[0] == 0; }

    NM_INLINE bool isEdgeElement() const { return m_isEdgeElement; }

  public:
    /// \brief A flag used to signal if any of the edge facets corresponding to this cell group partition
    /// touch the external edge of the annotation grid. This allows us to ignore internal elements when
    /// doing projection.
    bool      m_isEdgeElement;

    /// \brief The dimension that is partitioned for branch nodes. Not used for leaf nodes.
    uint32_t  m_branchPartitionDim;

    /// \brief The branches from this node, 0 if this is a leaf node (size 1 in all dimensions).
    uint32_t  m_childIndex[2];

    /// \brief The actual bounding box data for this node.
    float*    m_aabbMinVals;
    float*    m_aabbMaxVals;

    /// \brief The coordinates of the vertex at the top left (lowest coordinate in all dimensions) of the region
    /// bounded by this node.
    uint32_t* m_coordinates;
  };

public:
  static NMP::Memory::Format getMemoryRequirements(
    const MR::ScatteredData::NDMesh* nDMesh);

  static NDMeshBBTree* init(
    NMP::Memory::Resource& memRes,
    const MR::ScatteredData::NDMesh* nDMesh);

  static NDMeshBBTree* create(
    const MR::ScatteredData::NDMesh* nDMesh);

  NM_INLINE uint32_t getNumLeafNodes() const;

  NM_INLINE uint32_t getNumBranchNodes() const;

protected:
  static void calculateNodeMetrics(
    uint32_t numDimensions,
    const uint32_t* sampleCountsPerDimension,
    uint32_t& numCells,
    uint32_t& numNodes);

  uint32_t fill(    
    const MR::ScatteredData::NDMesh* nDMesh,
    uint32_t nodeIndex,
    const uint32_t* cellSizes,
    const uint32_t* coordinates);

public:  
  uint32_t    m_numDimensions;
  uint32_t    m_numComponentsPerSample;
  uint32_t    m_numCells;
  uint32_t    m_numNodes;

  Node**      m_nodes;
  float*      m_sampleCentre;
};

//----------------------------------------------------------------------------------------------------------------------
// NDMeshBBTree inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshBBTree::getNumLeafNodes() const
{
  return m_numCells;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshBBTree::getNumBranchNodes() const
{
  return m_numCells - 1;
}


//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NDMeshAPSearchMapBuilder
/// \brief For construction of MR::ScatteredData::NDMeshAPSearchMap in the asset compiler.
//----------------------------------------------------------------------------------------------------------------------
class NDMeshAPSearchMapBuilder : public MR::ScatteredData::NDMeshAPSearchMap
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    const NDMeshBBTree* nDMeshBBTree);

  static MR::ScatteredData::NDMeshAPSearchMap* init(
    NMP::Memory::Resource& memRes,
    const NDMeshBBTree* nDMeshBBTree);

  static NMP::Memory::Resource create(
    const MR::ScatteredData::NDMesh* nDMesh,
    const NDMeshQuantisationData* quantisationData,
    const NDMeshBBTree* nDMeshBBTree);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ND_MESH_AP_SEARCH_MAP_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
