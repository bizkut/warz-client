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
#include "Analysis/NDMeshAPSearchMapBuilder.h"
#include "NMNumerics/NMUniformQuantisation.h"
#include "NMPlatform/NMSystem.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// NDMeshBBTree::Node
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshBBTree::Node::getMemoryRequirements(
  uint32_t numDimensions,
  uint32_t numComponentsPerSample)
{
  // Header
  NMP::Memory::Format result(sizeof(NDMeshBBTree::Node), NMP_NATURAL_TYPE_ALIGNMENT);

  // Bounding box
  NMP::Memory::Format memReqsBB(sizeof(float) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqsBB * 2);

  // Coordinates
  NMP::Memory::Format memReqsCoords(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsCoords;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshBBTree::Node* NDMeshBBTree::Node::init(
  NMP::Memory::Resource& memRes,
  uint32_t               numDimensions,
  uint32_t               numComponentsPerSample)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshBBTree::Node), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshBBTree::Node* result = (NDMeshBBTree::Node*)memRes.alignAndIncrement(memReqsHdr);

  result->m_isEdgeElement = false;
  result->m_childIndex[0] = 0;
  result->m_childIndex[1] = 0;

  // Bounding box
  NMP::Memory::Format memReqsBB(sizeof(float) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_aabbMinVals = (float*)memRes.alignAndIncrement(memReqsBB);
  result->m_aabbMaxVals = (float*)memRes.alignAndIncrement(memReqsBB);

  // Coordinates
  NMP::Memory::Format memReqsCoords(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_coordinates = (uint32_t*)memRes.alignAndIncrement(memReqsCoords);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// NDMeshBBTree
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshBBTree::getMemoryRequirements(
  const MR::ScatteredData::NDMesh* nDMesh)
{
  NMP_VERIFY(nDMesh);

  // Header
  NMP::Memory::Format result(sizeof(NDMeshBBTree), NMP_NATURAL_TYPE_ALIGNMENT);

  uint32_t numDimensions = nDMesh->getNumDimensions();
  uint32_t numComponentsPerSample = nDMesh->getNumComponentsPerSample();
  const uint32_t* sampleCountsPerDimension = nDMesh->getSampleCountsPerDimension();

  uint32_t numCells, numNodes;
  calculateNodeMetrics(
    numDimensions,
    sampleCountsPerDimension,
    numCells,
    numNodes);

  // Node pointers table
  NMP::Memory::Format memReqsNodePtrs(sizeof(Node*) * numNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsNodePtrs;

  // Nodes
  NMP::Memory::Format memReqsNode = Node::getMemoryRequirements(numDimensions, numComponentsPerSample);
  result += (memReqsNode * numNodes);

  // Sample centre
  NMP::Memory::Format memReqsCentre(sizeof(float) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsCentre;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshBBTree* NDMeshBBTree::init(
  NMP::Memory::Resource& memRes,
  const MR::ScatteredData::NDMesh* nDMesh)
{
  NMP_VERIFY(nDMesh);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshBBTree), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshBBTree* result = (NDMeshBBTree*)memRes.alignAndIncrement(memReqsHdr);

  uint32_t numDimensions = nDMesh->getNumDimensions();
  uint32_t numComponentsPerSample = nDMesh->getNumComponentsPerSample();
  const uint32_t* sampleCountsPerDimension = nDMesh->getSampleCountsPerDimension();

  result->m_numDimensions = numDimensions;
  result->m_numComponentsPerSample = numComponentsPerSample;
  calculateNodeMetrics(
    numDimensions,
    sampleCountsPerDimension,
    result->m_numCells,
    result->m_numNodes);

  // Node pointers table
  NMP::Memory::Format memReqsNodePtrs(sizeof(Node*) * result->m_numNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_nodes = (Node**)memRes.alignAndIncrement(memReqsNodePtrs);

  // Nodes
  for (uint32_t i = 0; i < result->m_numNodes; ++i)
  {
    result->m_nodes[i] = Node::init(memRes, numDimensions, numComponentsPerSample);
  }

  // Sample centre
  NMP::Memory::Format memReqsCentre(sizeof(float) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_sampleCentre = (float*)memRes.alignAndIncrement(memReqsCentre);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshBBTree* NDMeshBBTree::create(const MR::ScatteredData::NDMesh* nDMesh)
{
  NMP_VERIFY(nDMesh);

  //-------------------------------
  NMP::Memory::Format memReqs = NDMeshBBTree::getMemoryRequirements(nDMesh);

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  NDMeshBBTree* result = NDMeshBBTree::init(memRes, nDMesh);

  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "The NDMeshBBTree did not initialise all the allocated memory: %d bytes remain.",
    memRes.format.size);

  //-------------------------------
  // Kick off a recursive filling of the tree via the root node.
  uint32_t numDimensions = nDMesh->getNumDimensions();
  const uint32_t* sampleCountsPerDimension = nDMesh->getSampleCountsPerDimension();

  uint32_t cellSizes[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    cellSizes[dim] = sampleCountsPerDimension[dim] - 1;
  }
  uint32_t coordinates[SCATTERED_DATA_MAX_DIM];
  memset(coordinates, 0, sizeof(uint32_t) * numDimensions);

  uint32_t nodeIndex = result->fill(nDMesh, 0, cellSizes, coordinates);
  NMP_VERIFY(nodeIndex == result->m_numNodes);

  //-------------------------------
  // Sample centre
  uint32_t numComponentsPerSample = result->m_numComponentsPerSample;
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
    result->m_sampleCentre[i] = 0.0f;

  float vertex[SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
  uint32_t numSamples = nDMesh->getNumSamples();
  for (uint32_t vertexSampleIndex = 0; vertexSampleIndex < numSamples; ++vertexSampleIndex)
  {
    nDMesh->getVertex(vertexSampleIndex, vertex);
    for (uint32_t i = 0; i < numComponentsPerSample; ++i)
      result->m_sampleCentre[i] += vertex[i];
  }

  float fNumSamples = (float)numSamples;
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
  {
    result->m_sampleCentre[i] /= fNumSamples;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshBBTree::calculateNodeMetrics(
  uint32_t numDimensions,
  const uint32_t* sampleCountsPerDimension,
  uint32_t& numCells,
  uint32_t& numNodes)
{
  NMP_ASSERT(numDimensions > 0);
  NMP_ASSERT(sampleCountsPerDimension);

  // Calculate the total number of cells, which is like the total number of samples but
  // one fewer in each dimension.
  numCells = 1;
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    numCells *= sampleCountsPerDimension[dim] - 1;
  }

  // This is a binary tree, with the leaf layer having numCells entries.  That means there
  // are in total 2*numCells - 1 entries or nodes.
  numNodes = 2 * numCells - 1;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NDMeshBBTree::fill(    
  const MR::ScatteredData::NDMesh* nDMesh,
  uint32_t nodeIndex,
  const uint32_t* cellSizes,
  const uint32_t* coordinates)
{
  NMP_VERIFY(nDMesh);
  NMP_VERIFY(cellSizes);
  NMP_VERIFY(coordinates);
  NMP_VERIFY(nodeIndex < m_numNodes);

  uint32_t resultIndex = nodeIndex;
  Node* target = m_nodes[nodeIndex];
  uint32_t numDimensions = nDMesh->getNumDimensions();
  uint32_t numComponentsPerSample = nDMesh->getNumComponentsPerSample();
  const uint32_t* samplesPerDimension = nDMesh->getSampleCountsPerDimension();

  // Store the coordinates passed in.
  memcpy(target->m_coordinates, coordinates, sizeof(uint32_t) * numDimensions);

  // Am I a leaf element (size 1 in all dimensions)?
  bool isLeafElement = true;
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    NMP_ASSERT(cellSizes[dim] >= 1);
    if (cellSizes[dim] > 1)
    {
      isLeafElement = false;
      break;
    }
  }

  // If I'm a leaf element, compute my bounding box information from the sample data.
  if (isLeafElement)
  {
    // Check bounds.  Coordinates along the far edge are valid samples but not valid cells - they represent
    // cells outside the sample space.
#ifdef NM_DEBUG
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      NMP_VERIFY(coordinates[dim] < (samplesPerDimension[dim] - 1));
    }
#endif

    // Recover the topLeft cell vertex
    float vertexData[SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
    nDMesh->getVertex(coordinates, vertexData);

    // Initialise the maxima and minima for this sample.
    for (uint32_t i = 0; i < numComponentsPerSample; ++i)
    {
      target->m_aabbMinVals[i] = vertexData[i];
      target->m_aabbMaxVals[i] = vertexData[i];
    }

    // Fill the array of vertex samples for this cell and compute the maximum and minimum values in each
    // dimension.
    uint32_t numVertices = 1 << numDimensions;
    for (uint32_t vertex = 1; vertex < numVertices; ++vertex)
    {
      // Convert vertex index into a coordinate offset from the 'top left' of the cell for each
      // dimension.
      uint32_t vertexCoord[SCATTERED_DATA_MAX_DIM];
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        // Offset coordinate by 1 or 0, depending on whether the bit for this element is 1 or 0.
        // We need to make a special case of degenerate dimensions.
        uint32_t offset = (vertex >> dim) & 1;
        vertexCoord[dim] = coordinates[dim] + offset;
      }

      // Recover the cell vertex
      nDMesh->getVertex(vertexCoord, vertexData);

      // Update the maximum and minimum.
      for (uint32_t i = 0; i < numComponentsPerSample; ++i)
      {
        target->m_aabbMinVals[i] = NMP::minimum(target->m_aabbMinVals[i], vertexData[i]);
        target->m_aabbMaxVals[i] = NMP::maximum(target->m_aabbMaxVals[i], vertexData[i]);
      }
    }

    // Be sure to set the child nodes to 0 to mark this as a leaf.
    target->m_branchPartitionDim = 0;
    target->m_childIndex[0] = 0;
    target->m_childIndex[1] = 0;

    // The return value is the next node in the tree
    resultIndex = nodeIndex + 1;
  }

  // Otherwise, fill the child data and use to get my bounding box.
  else
  {
    // Which dimension am I going to halve to create the next partition?  The one with the most cells.
    uint32_t nextDimension = 0;
    uint32_t largestSize = cellSizes[0];
    for (uint32_t dim = 1; dim < numDimensions; ++dim)
    {
      if (cellSizes[dim] > largestSize)
      {
        largestSize = cellSizes[dim];
        nextDimension = dim;
      }
    }
    target->m_branchPartitionDim = nextDimension;
    uint32_t nextCellSizeRight = cellSizes[nextDimension] >> 1;
    uint32_t nextCellSizeLeft = cellSizes[nextDimension] - nextCellSizeRight; // Remainder in here

    // Calculate the sizes for the first child by halving the size in that dimension and rounding up.
    uint32_t childCellSizes[SCATTERED_DATA_MAX_DIM];
    memcpy(childCellSizes, cellSizes, sizeof(uint32_t) * numDimensions);
    childCellSizes[nextDimension] = nextCellSizeLeft;

    // First child has same coordinates.
    uint32_t childCoordinates[SCATTERED_DATA_MAX_DIM];
    memcpy(childCoordinates, coordinates, sizeof(uint32_t) * numDimensions);

    // Get the location of the first child, which directly follows this node.
    target->m_childIndex[0] = nodeIndex + 1;

    // Fill the first child - retrieve the location of the second child, which directly follows the first child and
    // all its descendants.
    target->m_childIndex[1] = fill(
      nDMesh,
      target->m_childIndex[0],
      childCellSizes,
      childCoordinates);

    // Second child's coordinates are got by adding the size of the first child in the relevant dimension.
    childCoordinates[nextDimension] += nextCellSizeLeft;
    childCellSizes[nextDimension] = nextCellSizeRight;

    // Fill the second child and get the location of the next element, which directly follows the second child and
    // all its descendants.
    resultIndex = fill(
      nDMesh,
      target->m_childIndex[1],
      childCellSizes,
      childCoordinates);

    // Compute my bounding box from my children.
    for (uint32_t i = 0; i < numComponentsPerSample; ++i)
    {
      Node* child0 = m_nodes[target->m_childIndex[0]];
      Node* child1 = m_nodes[target->m_childIndex[1]];
      target->m_aabbMinVals[i] = NMP::minimum(child0->m_aabbMinVals[i], child1->m_aabbMinVals[i]);
      target->m_aabbMaxVals[i] = NMP::maximum(child0->m_aabbMaxVals[i], child1->m_aabbMaxVals[i]);
    }
  }

  // Am I an edge element?  I am if my coordinates are 0 or samplesPerDimension[dim] - cellSizes[dim] - 1.
  target->m_isEdgeElement = false;
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    uint32_t numCellsInDimension = samplesPerDimension[dim] - 1;
    if (coordinates[dim] == 0 || coordinates[dim] == (numCellsInDimension - cellSizes[dim]))
    {
      target->m_isEdgeElement = true;
      break;
    }
  }

  NMP_ASSERT(resultIndex != nodeIndex);
  return resultIndex;
}


//----------------------------------------------------------------------------------------------------------------------
// NDMeshAPSearchMapBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshAPSearchMapBuilder::getMemoryRequirements(
  const NDMeshBBTree* nDMeshBBTree)
{
  NMP_VERIFY(nDMeshBBTree);

  // Header
  NMP::Memory::Format result(sizeof(MR::ScatteredData::NDMeshAPSearchMap), NMP_NATURAL_TYPE_ALIGNMENT);

  uint32_t numDimensions = nDMeshBBTree->m_numDimensions;
  uint32_t numComponentsPerSample = nDMeshBBTree->m_numComponentsPerSample;
  uint32_t numAnalysedProperties = numComponentsPerSample - numDimensions;

  // AABBs
  NMP::Memory::Format memReqsBB(sizeof(uint16_t) * numAnalysedProperties, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsBB *= (nDMeshBBTree->m_numNodes * 2);
  result += memReqsBB;

  // Branch node children
  uint32_t numBranchNodes = nDMeshBBTree->getNumBranchNodes();
  if (numBranchNodes > 0)
  {
    NMP::Memory::Format memReqsBPDims(sizeof(uint8_t) * numBranchNodes, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsBPDims;

    NMP::Memory::Format memReqsChild(sizeof(uint16_t) * numBranchNodes, NMP_NATURAL_TYPE_ALIGNMENT);
    result += (memReqsChild * 2);
  }

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::ScatteredData::NDMeshAPSearchMap* NDMeshAPSearchMapBuilder::init(
  NMP::Memory::Resource& memRes,
  const NDMeshBBTree* nDMeshBBTree)
{
  NMP_VERIFY(nDMeshBBTree);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(MR::ScatteredData::NDMeshAPSearchMap), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshAPSearchMapBuilder* result = (NDMeshAPSearchMapBuilder*)memRes.alignAndIncrement(memReqsHdr);

  NMP_VERIFY(nDMeshBBTree->m_numNodes > 0);
  uint32_t numBranchNodes = nDMeshBBTree->getNumBranchNodes();
  uint32_t numLeafNodes = nDMeshBBTree->getNumLeafNodes();
  uint32_t numDimensions = nDMeshBBTree->m_numDimensions;
  uint32_t numComponentsPerSample = nDMeshBBTree->m_numComponentsPerSample;
  uint32_t numAnalysedProperties = numComponentsPerSample - numDimensions;
 
  result->m_numDimensions = numDimensions;
  result->m_numAnalysedProperties = numAnalysedProperties;
  result->m_numComponentsPerSample = numComponentsPerSample;
  result->m_numBranchNodes = numBranchNodes;
  result->m_numLeafNodes = numLeafNodes;

  // AABBs
  NMP::Memory::Format memReqsBB(sizeof(uint16_t) * numAnalysedProperties, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsBB *= nDMeshBBTree->m_numNodes;
  result->m_aabbMinVals = (uint16_t*)memRes.alignAndIncrement(memReqsBB);
  result->m_aabbMaxVals = (uint16_t*)memRes.alignAndIncrement(memReqsBB);

  // Branch node children
  if (numBranchNodes > 0)
  {
    NMP::Memory::Format memReqsBPDims(sizeof(uint8_t) * numBranchNodes, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_branchPartitionDims = (uint8_t*)memRes.alignAndIncrement(memReqsBPDims);

    NMP::Memory::Format memReqsChild(sizeof(uint16_t) * numBranchNodes, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_left = (uint16_t*)memRes.alignAndIncrement(memReqsChild);
    result->m_right = (uint16_t*)memRes.alignAndIncrement(memReqsChild);
  }
  else
  {
    result->m_branchPartitionDims = NULL;
    result->m_left = NULL;
    result->m_right = NULL;
  }

  // Multiple of the data alignment
  memRes.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource NDMeshAPSearchMapBuilder::create(
  const MR::ScatteredData::NDMesh* nDMesh,
  const NDMeshQuantisationData* quantisationData,
  const NDMeshBBTree* nDMeshBBTree)
{
  NMP_VERIFY(nDMesh);
  NMP_VERIFY(quantisationData);
  NMP_VERIFY(nDMeshBBTree);

  //-------------------------------
  NMP::Memory::Format memReqs = NDMeshAPSearchMapBuilder::getMemoryRequirements(nDMeshBBTree);

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  NMP::Memory::Resource result = memRes;
  ZeroMemory(memRes.ptr, memRes.format.size);

  NDMeshAPSearchMapBuilder* target = (NDMeshAPSearchMapBuilder*)NDMeshAPSearchMapBuilder::init(memRes, nDMeshBBTree);

  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "The NDMeshAPSearchMapBuilder did not initialise all the allocated memory: %d bytes remain.",
    memRes.format.size);

  //-------------------------------
  // Compute the index re-mapping of the nodes so that all branch nodes appear before leaf nodes.
  uint32_t numNodes = nDMeshBBTree->m_numNodes;
  NMP::Memory::Format memReqReMap(sizeof(uint32_t) * numNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResReMapA = NMPMemoryAllocateFromFormat(memReqReMap);
  NMP::Memory::Resource memResReMapB = NMPMemoryAllocateFromFormat(memReqReMap);
  uint32_t* reMapA = (uint32_t*)memResReMapA.alignAndIncrement(memReqReMap);
  uint32_t* reMapB = (uint32_t*)memResReMapB.alignAndIncrement(memReqReMap);

  // Branch nodes
  uint32_t numBranchNodes = 0;
  for (uint32_t i = 0; i < numNodes; ++i)
  {
    if (nDMeshBBTree->m_nodes[i]->isBranchNode())
    {
      reMapA[numBranchNodes] = i; // Map from reordered buffer to source buffer (for consecutive
                                  // elements in the reordered buffer, where do nodes come from in
                                  // the source buffer)
      numBranchNodes++;
    }
  }

  // Leaf nodes
  uint32_t index = numBranchNodes;
  for (uint32_t i = 0; i < numNodes; ++i)
  {
    if (nDMeshBBTree->m_nodes[i]->isLeafNode())
    {
      reMapA[index] = i;    // Map from reordered buffer to source buffer (for consecutive
                            // elements in the reordered buffer, where do nodes come from in
                            // the source buffer)
      index++;
    }
  }
  NMP_VERIFY(index == numNodes);

  //-------------------------------
  // Reorder the branch nodes so that AABBs that touch the external boundary appear first in the
  // list. This will be used to quickly test for external AABBs at runtime.
  for (index = 0; index < numBranchNodes; ++index)
  {
    uint32_t indxSrc1 = reMapA[index];
    if (!nDMeshBBTree->m_nodes[indxSrc1]->isEdgeElement())
    {
      // Find the next boundary branch node in the buffer
      uint32_t indexDst = index;
      for (uint32_t j = index + 1; j < numBranchNodes; ++j)
      {
        uint32_t indxSrc2 = reMapA[j];
        if (nDMeshBBTree->m_nodes[indxSrc2]->isEdgeElement())
        {
          indexDst = j;
          break;
        }
      }

      // Check if there are any remaining boundary branch nodes in the buffer
      if (indexDst == index)
        break;

      // Swap the branch nodes
      reMapA[index] = reMapA[indexDst];
      reMapA[indexDst] = indxSrc1;
    }
  }
  target->m_edgeBranchNodeOffset = index;

  // Reorder the leaf nodes so that NDCells that touch the external boundary appear last in the
  // list. This will be used to quickly test for external cells at runtime.
  for (index = numBranchNodes; index < numNodes; ++index)
  {
    // Sort all internal leaf node cells before boundary cells
    uint32_t indxSrc1 = reMapA[index];
    if (nDMeshBBTree->m_nodes[indxSrc1]->isEdgeElement())
    {
      // Try and swap this boundary leaf node with one of the proceeding nodes in the reordered buffer
      uint32_t indexDst = index;
      for (uint32_t j = index + 1; j < numNodes; ++j)
      {
        uint32_t indxSrc2 = reMapA[j];
        if (!nDMeshBBTree->m_nodes[indxSrc2]->isEdgeElement())
        {
          indexDst = j;
          break;
        }
      }

      // Check if there are any remaining internal leaf nodes in the buffer
      if (indexDst == index)
        break;

      // Swap the leaf nodes
      reMapA[index] = reMapA[indexDst];
      reMapA[indexDst] = indxSrc1;
    }
  }
  NMP_VERIFY(index < numNodes);
  target->m_edgeLeafNodeOffset = index;

  //-------------------------------
  // Compute the map from source buffer to reordered buffer (i.e. for consecutive
  // elements in the source buffer, where do nodes go to in the reordered buffer)
  for (uint32_t i = 0; i < numNodes; ++i)
  {
    reMapB[i] = 0xFFFFFFFF;
  }
  for (uint32_t i = 0; i < numNodes; ++i)
  {
    uint32_t indexSrc = reMapA[i];
    NMP_VERIFY(reMapB[indexSrc] == 0xFFFFFFFF); // Check that this entry hasn't been set before
    reMapB[indexSrc] = i;
  }

  // Re-map the branch nodes
  for (uint32_t nodeID = 0; nodeID < numBranchNodes; ++nodeID)
  {
    // Find the next branch node
    uint32_t branchNodeIndex = reMapA[nodeID];
    NMP_VERIFY(branchNodeIndex < numNodes);
    const NDMeshBBTree::Node* branchNodeSrc = nDMeshBBTree->m_nodes[branchNodeIndex];

    // Compute the indices of the re-mapped child nodes
    uint32_t childIndex0 = branchNodeSrc->m_childIndex[0];
    NMP_VERIFY(childIndex0 < numNodes);
    childIndex0 = reMapB[childIndex0];
    NMP_VERIFY(childIndex0 < numNodes);

    uint32_t childIndex1 = branchNodeSrc->m_childIndex[1];
    NMP_VERIFY(childIndex1 < numNodes);
    childIndex1 = reMapB[childIndex1];
    NMP_VERIFY(childIndex1 < numNodes);

    // Bounding box
    uint32_t offset = nodeID * target->m_numAnalysedProperties;
    uint16_t* aabbMinVals = &target->m_aabbMinVals[offset];
    uint16_t* aabbMaxVals = &target->m_aabbMaxVals[offset];
    for (uint32_t apIndex = 0; apIndex < target->m_numAnalysedProperties; ++apIndex)
    {
      uint32_t index = apIndex + target->m_numDimensions;

      uint32_t qValue;
      qValue = NMP::UniformQuantisation::quantiseFloor(
        quantisationData->m_qMinValues[index],
        quantisationData->m_qMaxValues[index],
        quantisationData->m_recipStepSizes[index],
        branchNodeSrc->m_aabbMinVals[index]);
      aabbMinVals[apIndex] = (uint16_t)qValue;

      qValue = NMP::UniformQuantisation::quantiseCeil(
        quantisationData->m_qMinValues[index],
        quantisationData->m_qMaxValues[index],
        quantisationData->m_recipStepSizes[index],
        branchNodeSrc->m_aabbMaxVals[index]);
      aabbMaxVals[apIndex] = (uint16_t)qValue;
    }

    // Branch node children
    target->m_branchPartitionDims[nodeID] = (uint8_t)branchNodeSrc->m_branchPartitionDim;
    target->m_left[nodeID] = (uint16_t)childIndex0;
    target->m_right[nodeID] = (uint16_t)childIndex1;
  }

  // Re-map the leaf nodes
  for (uint32_t nodeID = numBranchNodes; nodeID < numNodes; ++nodeID)
  {
    // Find the next leaf node
    uint32_t leafNodeIndex = reMapA[nodeID];
    NMP_VERIFY(leafNodeIndex < numNodes);
    const NDMeshBBTree::Node* leafNodeSrc = nDMeshBBTree->m_nodes[leafNodeIndex];

    // Bounding box
    uint32_t offset = nodeID * target->m_numAnalysedProperties;
    uint16_t* aabbMinVals = &target->m_aabbMinVals[offset];
    uint16_t* aabbMaxVals = &target->m_aabbMaxVals[offset];
    for (uint32_t apIndex = 0; apIndex < target->m_numAnalysedProperties; ++apIndex)
    {
      uint32_t index = apIndex + target->m_numDimensions;

      uint32_t qValue;
      qValue = NMP::UniformQuantisation::quantiseFloor(
        quantisationData->m_qMinValues[index],
        quantisationData->m_qMaxValues[index],
        quantisationData->m_recipStepSizes[index],
        leafNodeSrc->m_aabbMinVals[index]);
      aabbMinVals[apIndex] = (uint16_t)qValue;

      qValue = NMP::UniformQuantisation::quantiseCeil(
        quantisationData->m_qMinValues[index],
        quantisationData->m_qMaxValues[index],
        quantisationData->m_recipStepSizes[index],
        leafNodeSrc->m_aabbMaxVals[index]);
      aabbMaxVals[apIndex] = (uint16_t)qValue;
    }
  }

  //-------------------------------
  // Tidy up
  NMP::Memory::memFree(reMapA);
  NMP::Memory::memFree(reMapB);

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
