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
#include "NMPlatform/NMMathUtils.h"
#include "NodeScatterBlend2DAnnotation.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Edge facet indices: A, B arrays for each of the 3 edges opposite the corresponding triangle vertex.
static const uint32_t g_edgeIndicesA[3] = {1, 2, 0};
static const uint32_t g_edgeIndicesB[3] = {2, 0, 1};

//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangleElement
//----------------------------------------------------------------------------------------------------------------------
SB2DTriangleElement::SB2DTriangleElement(
  SB2DTriangleElementType elementType,
  uint32_t numElementSubSamples)
{
  // Initialise the data
  NMP_VERIFY(elementType < SB2DNumTriangleElementTypes);
  m_elementType = elementType;

  for (uint32_t i = 0; i < 3; ++i)
    m_elementVertices[i] = NULL;

  // Homogeneous sub-sample point
  for (uint32_t i = 0; i < 3; ++i)
    m_elementSubSamples[i] = NULL;

  for (uint32_t i = 0; i < 3; ++i)
    m_elementSubSampleWeights[i] = NULL;

  // Allocate the sample and weight data
  m_numElementSubSamples = numElementSubSamples;
  if (m_numElementSubSamples > 0)
  {
    for (uint32_t i = 0; i < 3; ++i)
    {
      m_elementSubSamples[i] = new float[numElementSubSamples];

      // Clear the data
      for (uint32_t k = 0; k < numElementSubSamples; ++k)
      {
        m_elementSubSamples[i][k] = 0.0f;
      }
    }

    for (uint32_t i = 0; i <= (uint32_t)elementType; ++i)
    {
      m_elementSubSampleWeights[i] = new float[numElementSubSamples];

      // Clear the data
      for (uint32_t k = 0; k < numElementSubSamples; ++k)
      {
        m_elementSubSampleWeights[i][k] = 0.0f;
      }
    }

    m_elementSubSampleValidFlags = new bool[numElementSubSamples];
    // Clear the data
    for (uint32_t k = 0; k < numElementSubSamples; ++k)
    {
      m_elementSubSampleValidFlags[k] = false;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
SB2DTriangleElement::~SB2DTriangleElement()
{
  for (uint32_t i = 0; i < 3; ++i)
  {
    if (m_elementSubSamples[i])
      delete[] m_elementSubSamples[i];
  }

  for (uint32_t i = 0; i < 3; ++i)
  {
    if (m_elementSubSampleWeights[i])
      delete[] m_elementSubSampleWeights[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangleSubSampleElementMap
//----------------------------------------------------------------------------------------------------------------------
SB2DTriangleSubSampleElementMap::SB2DTriangleSubSampleElementMap() :
  m_numTriangleSubDivisions(0),
  m_numSubSamplesPerTriangle(0),
  m_tempIndexBuffer(NULL),
  m_triangleElementTypes(NULL),
  m_triangleElementIndices(NULL),
  m_elementSubSampleIndices(NULL)
{
  for (uint32_t i = 0; i < 3; ++i)
    m_triangleVertexIDs[i] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DTriangleSubSampleElementMap::~SB2DTriangleSubSampleElementMap()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangleSubSampleElementMap::init(uint32_t numTriangleSubDivisions)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(numTriangleSubDivisions);

  m_numTriangleSubDivisions = numTriangleSubDivisions;
  m_numSubSamplesPerTriangle = subDivisionDataBlock.getNumSubSamplesPerTriangle();

  // Temporary index buffers
  m_tempIndexBuffer = new uint32_t[m_numSubSamplesPerTriangle];

  // Triangle element map
  m_triangleElementTypes = new SB2DTriangleElementType[m_numSubSamplesPerTriangle];
  m_triangleElementIndices = new uint32_t[m_numSubSamplesPerTriangle];
  m_elementSubSampleIndices = new uint32_t[m_numSubSamplesPerTriangle];
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangleSubSampleElementMap::release()
{
  m_numTriangleSubDivisions = 0;
  m_numSubSamplesPerTriangle = 0;

  if (m_tempIndexBuffer)
  {
    delete[] m_tempIndexBuffer;
    m_tempIndexBuffer = NULL;
  }

  if (m_triangleElementTypes)
  {
    delete[] m_triangleElementTypes;
    m_triangleElementTypes = NULL;
  }

  if (m_triangleElementIndices)
  {
    delete[] m_triangleElementIndices;
    m_triangleElementIndices = NULL;
  }

  if (m_elementSubSampleIndices)
  {
    delete[] m_elementSubSampleIndices;
    m_elementSubSampleIndices = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DVertex
//----------------------------------------------------------------------------------------------------------------------
SB2DVertex::SB2DVertex() : SB2DTriangleElement(kSB2DVertexElementType, 1)
{
  m_vertexID = 0;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DVertex::~SB2DVertex()
{
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DEdge
//----------------------------------------------------------------------------------------------------------------------
SB2DEdge::SB2DEdge(uint32_t numElementSubSamples) : SB2DTriangleElement(kSB2DEdgeElementType, numElementSubSamples)
{
}

//----------------------------------------------------------------------------------------------------------------------
SB2DEdge::~SB2DEdge()
{
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangle
//----------------------------------------------------------------------------------------------------------------------
SB2DTriangle::SB2DTriangle(uint32_t numElementSubSamples) : SB2DTriangleElement(kSB2DTriangleElementType, numElementSubSamples)
{
  m_triangleID = 0;
  
  for (uint32_t i = 0; i < 3; ++i)
    m_edges[i] = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DTriangle::~SB2DTriangle()
{
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangle::getEdgeVertices(
  uint32_t edgeIndex,
  SB2DVertex*& vertexA,
  SB2DVertex*& vertexB) const
{
  NMP_VERIFY(edgeIndex < 3);
  vertexA = m_elementVertices[ g_edgeIndicesA[edgeIndex] ];
  vertexB = m_elementVertices[ g_edgeIndicesB[edgeIndex] ];
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangle::getTriangleVertices(
  SB2DVertex*& vertexA,
  SB2DVertex*& vertexB,
  SB2DVertex*& vertexC) const
{
  vertexA = m_elementVertices[0];
  vertexB = m_elementVertices[1];
  vertexC = m_elementVertices[2];
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DTriangle::isVertexOrderingCCW(
  float Ax,
  float Ay,
  float Bx,
  float By,
  float Cx,
  float Cy)
{
  // Relative vectors AB, AC
  float Ux = Bx - Ax;
  float Uy = By - Ay;
  float Vx = Cx - Ax;
  float Vy = Cy - Ay;

  // Check the Z component of the cross product
  float CPz = Ux * Vy - Vx * Uy;
  return CPz >= 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DTriangle::isVertexOrderingCollinear(
  float Ax,
  float Ay,
  float Bx,
  float By,
  float Cx,
  float Cy)
{
  // Relative vectors AB, AC
  float Ux = Bx - Ax;
  float Uy = By - Ay;
  float Vx = Cx - Ax;
  float Vy = Cy - Ay;

  // Check the Z component of the cross product
  float CPz = Ux * Vy - Vx * Uy;
  return fabs(CPz) < FLT_EPSILON;
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DTriangle::getVertexOrderingCCW(
  uint32_t numVertices,               // IN: The number of vertices
  const float* samplesX,              // IN: X component array of vertex samples.
  const float* samplesY,              // IN: Y component array of vertex samples.
  const uint32_t* vertexIndicesIn,    // IN: Indices within the sample arrays for the triangle's A, B, C vertices.
  uint32_t* vertexIndicesOut)         // OUT: Permuted triangle vertex indices that have a counter-clockwise geometry.
{
  NMP_VERIFY(samplesX);
  NMP_VERIFY(samplesY);
  NMP_VERIFY(vertexIndicesIn);
  NMP_VERIFY(vertexIndicesOut);
  NMP_VERIFY_MSG(
    vertexIndicesIn[0] < numVertices &&
    vertexIndicesIn[1] < numVertices &&
    vertexIndicesIn[2] < numVertices,
    "Invalid triangle annotion vertex indices");

  uint32_t indexA = vertexIndicesIn[0];
  uint32_t indexB = vertexIndicesIn[1];
  uint32_t indexC = vertexIndicesIn[2];

  // Determine if ABC is counter-clockwise
  if (isVertexOrderingCCW(
    samplesX[indexA],
    samplesY[indexA],
    samplesX[indexB],
    samplesY[indexB],
    samplesX[indexC],
    samplesY[indexC]))
  {
    vertexIndicesOut[0] = indexA;
    vertexIndicesOut[1] = indexB;
    vertexIndicesOut[2] = indexC;
  }
  else
  {
    // Swap vertex order B -> C
    vertexIndicesOut[0] = indexA;
    vertexIndicesOut[1] = indexC;
    vertexIndicesOut[2] = indexB;
  }

  // Check if vertices lay in a line
  bool isCollinear = isVertexOrderingCollinear(
    samplesX[indexA],
    samplesY[indexA],
    samplesX[indexB],
    samplesY[indexB],
    samplesX[indexC],
    samplesY[indexC]);

  return isCollinear;
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DTriangle::isValidVertexOrdering() const
{
  // Recover the triangle vertices
  NMP::Vector3 X[3];
  for (uint32_t i = 0; i < 3; ++i)
  {
    NMP_VERIFY(m_elementVertices[i]);
    float x, y;
    m_elementVertices[i]->getElementSubSampleInhomogeneous(0, x, y);
    X[i].set(x, y, 0.0f);
  }

  // Check ABC forms a counter-clockwise geometry (z component of cross product)
  return isVertexOrderingCCW(
    X[0].x,
    X[0].y,
    X[1].x,
    X[1].y,
    X[2].x,
    X[2].y);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SB2DTriangle::getTriangleSubSampleIndexForVertexElement(
  uint32_t numTriangleSubDivisions,
  const SB2DVertex* vertex) const
{
  NMP_VERIFY(vertex);

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(numTriangleSubDivisions);

  // Get the vertex index for the input vertex
  uint32_t vertexIndex = getVertexIndex(vertex);
  NMP_VERIFY(vertexIndex != 0xFFFFFFFF);

  // Get the corresponding triangle local sub-sample index
  const uint16_t* vertexSubSampleIndices = subDivisionDataBlock.getVertexSubSampleIndices();
  NMP_VERIFY(vertexSubSampleIndices);

  return vertexSubSampleIndices[vertexIndex];
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangle::getTriangleSubSampleIndicesForEdgeElement(
  uint32_t numTriangleSubDivisions,
  const SB2DEdge* edge,
  uint32_t* triangleSubSampleIndices) const
{
  NMP_VERIFY(edge);
  NMP_VERIFY(triangleSubSampleIndices);

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(numTriangleSubDivisions);

  uint32_t numInteriorSubSamplesPerEdge = subDivisionDataBlock.getNumInteriorSubSamplesPerEdge();

  // Get the edge index for the input edge
  uint32_t edgeIndex = getEdgeIndex(edge);
  NMP_VERIFY(edgeIndex != 0xFFFFFFFF);

  // Edge vertex order of the triangle
  SB2DVertex* vertexA;
  SB2DVertex* vertexB;
  getEdgeVertices(
    edgeIndex,
    vertexA,
    vertexB);
  uint32_t destVertexAID = vertexA->getVertexID();
  uint32_t destVertexBID = vertexB->getVertexID();

  // Edge vertex order of the stored edge element sub-samples
  vertexA = edge->getElementVertex(0);
  vertexB = edge->getElementVertex(1);
  uint32_t srcVertexAID = vertexA->getVertexID();
  uint32_t srcVertexBID = vertexB->getVertexID();

  // Compute the sub-sample permutation for the difference in the vertex order between
  // the stored and desired elements.
  uint32_t permutationIndex = MR::ScatterBlend2DSubSampleManager::getInteriorEdgeVertexOrderPermutation(
    srcVertexAID,
    srcVertexBID,
    destVertexAID,
    destVertexBID);

  // These could be NULL
  const uint16_t* interiorEdgeSubSamplePermutationMap = subDivisionDataBlock.getInteriorEdgeSubSamplePermutationMap(permutationIndex);
  const uint16_t* interiorEdgeSubSampleIndices = subDivisionDataBlock.getInteriorEdgeSubSampleIndices(edgeIndex);

  // Get the corresponding triangle local sub-sample indices
  for (uint32_t i = 0; i < numInteriorSubSamplesPerEdge; ++i)
  {
    uint32_t permutedElementSubSampleIndex = interiorEdgeSubSamplePermutationMap[i];
    NMP_VERIFY(permutedElementSubSampleIndex < numInteriorSubSamplesPerEdge);
    triangleSubSampleIndices[i] = interiorEdgeSubSampleIndices[permutedElementSubSampleIndex];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangle::getTriangleSubSampleIndicesForTriangleElement(
  uint32_t numTriangleSubDivisions,
  uint32_t* triangleSubSampleIndices) const
{
  NMP_VERIFY(triangleSubSampleIndices);

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(numTriangleSubDivisions);

  uint32_t numInteriorSubSamplesPerTriangle = subDivisionDataBlock.getNumInteriorSubSamplesPerTriangle();

  // This could be NULL
  const uint16_t* interiorTriangleSubSampleIndices = subDivisionDataBlock.getInteriorTriangleSubSampleIndices();

  for (uint32_t i = 0; i < numInteriorSubSamplesPerTriangle; ++i)
  {
    triangleSubSampleIndices[i] = interiorTriangleSubSampleIndices[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangle::getTriangleSubSampleElementMap(
  SB2DTriangleSubSampleElementMap& elementMap) const
{
  uint32_t numTriangleSubDivisions = elementMap.getNumTriangleSubDivisions();
  uint32_t* tempIndexBuffer = elementMap.getTempIndexBuffer();

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(numTriangleSubDivisions);

  uint32_t numInteriorSubSamplesPerEdge = subDivisionDataBlock.getNumInteriorSubSamplesPerEdge();
  uint32_t numInteriorSubSamplesPerTriangle = subDivisionDataBlock.getNumInteriorSubSamplesPerTriangle();

  //---------------------------
#ifdef NM_DEBUG
  elementMap.clearElementMap();
#endif

  // Vertex elements
  for (uint32_t elementIndex = 0; elementIndex < 3; ++elementIndex)
  {
    SB2DVertex* vertex = getElementVertex(elementIndex);
    uint32_t triangleSubSampleIndex = getTriangleSubSampleIndexForVertexElement(
      numTriangleSubDivisions,
      vertex);
    elementMap.setElementMapEntry(
      triangleSubSampleIndex,
      kSB2DVertexElementType,
      elementIndex,
      0);
  }

  // Interior edge elements
  for (uint32_t elementIndex = 0; elementIndex < 3; ++elementIndex)
  {
    SB2DEdge* edge = getEdgeElement(elementIndex);

    // Compute the triangle sub-sample indices corresponding to the
    // sub-sample indices of the edge element
    getTriangleSubSampleIndicesForEdgeElement(
      numTriangleSubDivisions,
      edge,
      tempIndexBuffer);

    for (uint32_t i = 0; i < numInteriorSubSamplesPerEdge; ++i)
    {
      elementMap.setElementMapEntry(
        tempIndexBuffer[i],
        kSB2DEdgeElementType,
        elementIndex,
        i);
    }
  }

  // Interior triangle element
  getTriangleSubSampleIndicesForTriangleElement(
    numTriangleSubDivisions,
    tempIndexBuffer);

  // Set the triangle sub-sample indices
  for (uint32_t i = 0; i < numInteriorSubSamplesPerTriangle; ++i)
  {
    elementMap.setElementMapEntry(
      tempIndexBuffer[i],
      kSB2DTriangleElementType,
      0,
      i);
  }

#ifdef NM_DEBUG
  NMP_VERIFY(elementMap.isValidElementMap());
#endif

  //---------------------------
  uint32_t vertexIDs[3];
  getTriangleVertexIDs(
    vertexIDs[0],
    vertexIDs[1],
    vertexIDs[2]);
  elementMap.setTriangleVertexIDs(vertexIDs);
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangle::getTriangleSubSampleData(
  const SB2DTriangleSubSampleElementMap& elementMap,
  SB2DTriangleSubSampleData& triangleSubSampleData) const
{
  // Vertex IDs
  triangleSubSampleData.setTriangleVertexIDs(elementMap.getTriangleVertexIDs());

  uint32_t numSubSamplesPerTriangle = elementMap.getNumSubSamplesPerTriangle();
  for (uint32_t i = 0; i < numSubSamplesPerTriangle; ++i)
  {
    // Get the element map entry
    SB2DTriangleElementType triangleElementType;
    uint32_t triangleElementIndex;
    uint32_t elementSubSampleIndex;
    elementMap.getElementMapEntry(
      i,
      triangleElementType,
      triangleElementIndex,
      elementSubSampleIndex);

    // Get the sub-sample data from the triangle element
    float x, y;
    x = y = 0.0f;
    bool validFlag = false;
    switch (triangleElementType)
    {
    case kSB2DTriangleElementType:
      {
        validFlag = getElementSubSampleValidFlag(elementSubSampleIndex);
        if (validFlag)
          getElementSubSampleInhomogeneous(elementSubSampleIndex, x, y);        
      }
      break;

    case kSB2DEdgeElementType:
      {
        const SB2DEdge* edge = getEdgeElement(triangleElementIndex);        
        validFlag = edge->getElementSubSampleValidFlag(elementSubSampleIndex);
        if (validFlag)
          edge->getElementSubSampleInhomogeneous(elementSubSampleIndex, x, y);
      }
      break;

    case kSB2DVertexElementType:
      {
        const SB2DVertex* vertex = getElementVertex(triangleElementIndex);        
        validFlag = vertex->getElementSubSampleValidFlag(elementSubSampleIndex);
        if (validFlag)
          vertex->getElementSubSampleInhomogeneous(elementSubSampleIndex, x, y);
      }
      break;
    }

    // Set the triangle sub-sample data
    triangleSubSampleData.setTriangleSubSample(i, x, y);
    triangleSubSampleData.setTriangleSubSampleValidFlag(i, validFlag);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangle::setTriangleSubSampleData(
  const SB2DTriangleSubSampleElementMap& elementMap,
  const SB2DTriangleSubSampleData& triangleSubSampleData)
{
  uint32_t numSubSamplesPerTriangle = elementMap.getNumSubSamplesPerTriangle();
  for (uint32_t i = 0; i < numSubSamplesPerTriangle; ++i)
  {
    // Get the element map entry
    SB2DTriangleElementType triangleElementType;
    uint32_t triangleElementIndex;
    uint32_t elementSubSampleIndex;
    elementMap.getElementMapEntry(
      i,
      triangleElementType,
      triangleElementIndex,
      elementSubSampleIndex);

    // Get the triangle sub-sample data
    float x, y;
    triangleSubSampleData.getTriangleSubSample(i, x, y);
    bool validFlag = triangleSubSampleData.getTriangleSubSampleValidFlag(i);

    // Add the sub-sample data to the triangle element
    switch (triangleElementType)
    {
    case kSB2DTriangleElementType:
      {
        if (validFlag)
        {
          setElementSubSampleInhomogeneous(elementSubSampleIndex, x, y);
          setElementSubSampleValidFlag(elementSubSampleIndex, validFlag);
        }        
      }
      break;

    case kSB2DEdgeElementType:
      {
        SB2DEdge* edge = getEdgeElement(triangleElementIndex);
        if (validFlag)
        {
          edge->setElementSubSampleInhomogeneous(elementSubSampleIndex, x, y);
          edge->setElementSubSampleValidFlag(elementSubSampleIndex, validFlag);
        }
      }
      break;

    case kSB2DVertexElementType:
      {
        SB2DVertex* vertex = getElementVertex(triangleElementIndex);
        if (validFlag)
        {
          vertex->setElementSubSampleInhomogeneous(elementSubSampleIndex, x, y);
          vertex->setElementSubSampleValidFlag(elementSubSampleIndex, validFlag);
        }
      }
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangle::addTriangleSubSampleData(
  const SB2DTriangleSubSampleElementMap& elementMap,
  const SB2DTriangleSubSampleData& triangleSubSampleData)
{
  uint32_t numSubSamplesPerTriangle = elementMap.getNumSubSamplesPerTriangle();
  for (uint32_t i = 0; i < numSubSamplesPerTriangle; ++i)
  {
    // Get the element map entry
    SB2DTriangleElementType triangleElementType;
    uint32_t triangleElementIndex;
    uint32_t elementSubSampleIndex;
    elementMap.getElementMapEntry(
      i,
      triangleElementType,
      triangleElementIndex,
      elementSubSampleIndex);

    // Get the triangle sub-sample data
    float x, y;
    triangleSubSampleData.getTriangleSubSample(i, x, y);
    bool validFlag = triangleSubSampleData.getTriangleSubSampleValidFlag(i);

    // Add the sub-sample data to the triangle element
    switch (triangleElementType)
    {
    case kSB2DTriangleElementType:
      {
        if (validFlag)
        {
          addElementSubSampleInhomogeneous(elementSubSampleIndex, x, y);
          setElementSubSampleValidFlag(elementSubSampleIndex, validFlag);
        }        
      }
      break;

    case kSB2DEdgeElementType:
      {
        SB2DEdge* edge = getEdgeElement(triangleElementIndex);
        if (validFlag)
        {
          edge->addElementSubSampleInhomogeneous(elementSubSampleIndex, x, y);
          edge->setElementSubSampleValidFlag(elementSubSampleIndex, validFlag);
        }
      }
      break;

    case kSB2DVertexElementType:
      {
        SB2DVertex* vertex = getElementVertex(triangleElementIndex);
        if (validFlag)
        {
          vertex->addElementSubSampleInhomogeneous(elementSubSampleIndex, x, y);
          vertex->setElementSubSampleValidFlag(elementSubSampleIndex, validFlag);
        }
      }
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangle::setValidFlags(bool validFlag)
{
  for (uint32_t elementIndex = 0; elementIndex < 3; ++elementIndex)
  {
    SB2DVertex* vertex = getElementVertex(elementIndex);
    vertex->setElementSubSampleValidFlags(validFlag);
  }

  for (uint32_t elementIndex = 0; elementIndex < 3; ++elementIndex)
  {
    SB2DEdge* edge = getEdgeElement(elementIndex);
    edge->setElementSubSampleValidFlags(validFlag);
  }

  setElementSubSampleValidFlags(validFlag);
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DTriangle::getValidFlags() const
{
  for (uint32_t elementIndex = 0; elementIndex < 3; ++elementIndex)
  {
    SB2DVertex* vertex = getElementVertex(elementIndex);
    if (!vertex->getElementSubSampleValidFlags())
      return false;
  }

  for (uint32_t elementIndex = 0; elementIndex < 3; ++elementIndex)
  {
    SB2DEdge* edge = getEdgeElement(elementIndex);
    if (!edge->getElementSubSampleValidFlags())
      return false;
  }

  return getElementSubSampleValidFlags();
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotation
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotation::SB2DAnnotation() : m_numTriangleSubDivisions(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotation::~SB2DAnnotation()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::release()
{
  //---------------------------
  // Vertices
  std::list<SB2DVertex*>::iterator vit;
  for (vit = m_vertexList.begin(); vit != m_vertexList.end(); ++vit)
  {
    SB2DVertex* vertex = *vit;
    delete vertex;
  }
  m_vertexList.clear();
  
  //---------------------------
  // Edge connections
  std::list<SB2DEdge*>::iterator eit;
  for (eit = m_edgeList.begin(); eit != m_edgeList.end(); ++eit)
  {
    SB2DEdge* edge = *eit;
    delete edge;
  }
  m_edgeList.clear();

  //---------------------------
  // Triangles
  std::list<SB2DTriangle*>::iterator tit;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    SB2DTriangle* triangle = *tit;
    delete triangle;
  }
  m_triangleList.clear();

  //---------------------------
  // Sub-samples
  m_numTriangleSubDivisions = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::initAnnotation(
  uint32_t numTriangleSubDivisions)
{
  // Release the currently stored data in the annotation graph
  release();

  // Sub-samples
  NMP_VERIFY(numTriangleSubDivisions < 3);
  m_numTriangleSubDivisions = numTriangleSubDivisions;
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::getAnnotation(SB2DAnnotationExportDataPacked& exportData) const
{
  // Release any currently stored export data
  exportData.release();

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(m_numTriangleSubDivisions);

  //---------------------------
  // Initialise the export data
  uint32_t numSubSamplesPerTriangle = subDivisionDataBlock.getNumSubSamplesPerTriangle();
  uint32_t numInteriorSubSamplesPerEdge = subDivisionDataBlock.getNumInteriorSubSamplesPerEdge();
  uint32_t numInteriorSubSamplesPerTriangle = subDivisionDataBlock.getNumInteriorSubSamplesPerTriangle();

  uint32_t numVertices = getNumVertices();
  uint32_t numUniqueEdges = getNumUniqueEdges();
  uint32_t numTriangles = getNumTriangles();
  uint32_t numSubSamples = calculateNumSubSamples(); // All unique sub-samples excluding the annotation vertices
  uint32_t numExteriorEdgeFacets = calculateNumExteriorEdgeFacets();

  exportData.init(
    m_numTriangleSubDivisions,
    numSubSamplesPerTriangle,
    numVertices,
    numTriangles,
    numSubSamples,
    numExteriorEdgeFacets);
  uint32_t numSamples = exportData.getNumSamples();

  //---------------------------
  // Element sub-sample arrays
  std::vector<uint32_t> triangleSubSampleIndices;
  triangleSubSampleIndices.resize(numSubSamplesPerTriangle);

  //---------------------------
  // Triangle vertex sub-sample data
  uint32_t index;
  uint32_t sampleIndex = 0;

  std::vector<uint32_t> vertexSubSampleOffsets;
  vertexSubSampleOffsets.resize(numVertices);
  std::list<SB2DVertex*>::const_iterator vit;
  sampleIndex = 0;
  for (vit = m_vertexList.begin(); vit != m_vertexList.end(); ++vit)
  {
    const SB2DVertex* vertex = *vit;
    vertexSubSampleOffsets[sampleIndex] = sampleIndex;

    float x, y;
    vertex->getElementSubSampleInhomogeneous(0, x, y);
    exportData.setSample(
      sampleIndex,
      x, y);

    NMP_VERIFY(vertex->getNumElementSubSamples() == 1);
    ++sampleIndex;
  }

  //---------------------------
  // Triangle interior edge sub-sample data
  std::vector<uint32_t> edgeSubSampleOffsets;
  edgeSubSampleOffsets.resize(numUniqueEdges);
  std::list<SB2DEdge*>::const_iterator eit;
  index = 0;
  for (eit = m_edgeList.begin(); eit != m_edgeList.end(); ++eit, ++index)
  {
    const SB2DEdge* edge = *eit;
    edgeSubSampleOffsets[index] = sampleIndex;

    float x, y;
    uint32_t numElementSubSamples = edge->getNumElementSubSamples();
    for (uint32_t i = 0; i < numElementSubSamples; ++i, ++sampleIndex)
    {
      edge->getElementSubSampleInhomogeneous(i, x, y);
      exportData.setSample(
        sampleIndex,
        x, y);
    }
  }

  //---------------------------
  // Triangle interior sub-sample data
  std::vector<uint32_t> triangleSubSampleOffsets;
  triangleSubSampleOffsets.resize(numTriangles);
  std::list<SB2DTriangle*>::const_iterator tit;
  index = 0;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit, ++index)
  {
    const SB2DTriangle* triangle = *tit;
    triangleSubSampleOffsets[index] = sampleIndex;

    float x, y;
    uint32_t numElementSubSamples = triangle->getNumElementSubSamples();
    for (uint32_t i = 0; i < numElementSubSamples; ++i, ++sampleIndex)
    {
      triangle->getElementSubSampleInhomogeneous(i, x, y);
      exportData.setSample(
        sampleIndex,
        x, y);
    }
  }

  //---------------------------
  // Triangle sub-sample indices
  uint32_t triangleIndex = 0;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit, ++triangleIndex)
  {
    // Get the triangle vertex IDs
    const SB2DTriangle* triangle = *tit;
    NMP_VERIFY(triangle);

    //---------------------------
    // Vertex sub-samples
    uint32_t subSampleIndex = 0xFFFFFFFF;
    for (uint32_t elementIndex = 0; elementIndex < 3; ++elementIndex)
    {
      // Find the index of the vertex within the global flat list
      const SB2DVertex* vertex = triangle->getElementVertex(elementIndex);
      NMP_VERIFY(isValidVertex(vertex));
      uint32_t vertexIndex = 0;
      for (vit = m_vertexList.begin(); vit != m_vertexList.end(); ++vit, ++vertexIndex)
      {
        const SB2DVertex* V = *vit;
        if (vertex == V)
          break;
      }
      NMP_VERIFY(vertexIndex != (uint32_t)m_vertexList.size());

      // Get the corresponding triangle local sub-sample index
      uint32_t triangleSubSampleIndex = triangle->getTriangleSubSampleIndexForVertexElement(
        m_numTriangleSubDivisions,
        vertex);

      // Set the triangle sub-sample index
      subSampleIndex = vertexSubSampleOffsets[vertexIndex];
      NMP_VERIFY(subSampleIndex < numVertices);
      exportData.setTriangleSubSampleIndex(
        triangleIndex,
        triangleSubSampleIndex,
        subSampleIndex);
    }

    //---------------------------
    // Interior edge sub-samples
    for (uint32_t elementIndex = 0; elementIndex < 3; ++elementIndex)
    {
      // Find the index of the edge within the global flat list
      const SB2DEdge* edge = triangle->getEdgeElement(elementIndex);
      NMP_VERIFY(isValidEdge(edge));
      uint32_t edgeIndex = 0;
      for (eit = m_edgeList.begin(); eit != m_edgeList.end(); ++eit, ++edgeIndex)
      {
        const SB2DEdge* E = *eit;
        if (edge == E)
          break;
      }
      NMP_VERIFY(edgeIndex != (uint32_t)m_edgeList.size());

      // Compute the triangle sub-sample indices corresponding to the
      // sub-sample indices of the edge element
      triangle->getTriangleSubSampleIndicesForEdgeElement(
        m_numTriangleSubDivisions,
        edge,
        &triangleSubSampleIndices[0]);

      // Set the triangle sub-sample indices
      for (uint32_t i = 0; i < numInteriorSubSamplesPerEdge; ++i)
      {
        subSampleIndex = edgeSubSampleOffsets[edgeIndex] + i;
        NMP_VERIFY(subSampleIndex < numSamples);
        exportData.setTriangleSubSampleIndex(
          triangleIndex,
          triangleSubSampleIndices[i],
          subSampleIndex);
      }
    }

    //---------------------------
    // Interior triangle sub-samples
    triangle->getTriangleSubSampleIndicesForTriangleElement(
      m_numTriangleSubDivisions,
      &triangleSubSampleIndices[0]);

    // Set the triangle sub-sample indices
    for (uint32_t i = 0; i < numInteriorSubSamplesPerTriangle; ++i)
    {
      subSampleIndex = triangleSubSampleOffsets[triangleIndex] + i;
      NMP_VERIFY(subSampleIndex < numSamples);
      exportData.setTriangleSubSampleIndex(
        triangleIndex,
        triangleSubSampleIndices[i],
        subSampleIndex);
    }
  }

  // Validate the export data sub-samples
  NMP_VERIFY(exportData.isValidTriangleSampleIndices());

  //---------------------------
  // Exterior edge facets
  uint32_t entryIndex = 0;
  triangleIndex = 0;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit, ++triangleIndex)
  {
    const SB2DTriangle* triangle = *tit;
    for (uint32_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
    {
      const SB2DEdge* edge = triangle->getEdgeElement(edgeIndex);
      NMP_VERIFY(edge);
      uint32_t numTriangleConnections = edge->getNumTriangleConnections();
      NMP_VERIFY(numTriangleConnections > 0);

      // Check for an external triangle facet
      if (numTriangleConnections == 1)
      {
        exportData.setExteriorEdgeFacet(
          entryIndex,
          triangleIndex,
          edgeIndex);

        ++entryIndex;
      }
    }
  }
  NMP_VERIFY(entryIndex == numExteriorEdgeFacets);

  //---------------------------
  // Annotation valid flags
  bool annotationValidFlag = true;
  uint32_t numUnconnectedVertices = getNumUnconnectedVertices();
  if (numTriangles == 0 || numVertices == 0 || numUnconnectedVertices > 0)
  {
    annotationValidFlag = false;
  }
  exportData.setAnnotationValidFlags(annotationValidFlag);
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::getAnnotation(SB2DAnnotationExportDataUnpacked& exportData) const
{
  // Release any currently stored export data
  exportData.release();

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(m_numTriangleSubDivisions);

  //---------------------------
  // Initialise the export data
  uint32_t numSubSamplesPerTriangle = subDivisionDataBlock.getNumSubSamplesPerTriangle();
  uint32_t numVertices = getNumVertices();
  uint32_t numTriangles = getNumTriangles();

  exportData.init(
    m_numTriangleSubDivisions,
    numSubSamplesPerTriangle,
    numVertices,
    numTriangles);

  //---------------------------
  // Triangle sub-sample element map
  SB2DTriangleSubSampleElementMap elementMap;
  SB2DTriangleSubSampleData triangleSubSampleData;
  elementMap.init(m_numTriangleSubDivisions);
  triangleSubSampleData.init(m_numTriangleSubDivisions);

  //---------------------------
  // Vertex sample data
  std::list<SB2DVertex*>::const_iterator vit;
  uint32_t sampleIndex = 0;
  for (vit = m_vertexList.begin(); vit != m_vertexList.end(); ++vit, ++sampleIndex)
  {
    const SB2DVertex* vertex = *vit;

    bool isValidFlag = vertex->getElementSubSampleValidFlag(0);
    if (isValidFlag)
    {
      float x, y;
      vertex->getElementSubSampleInhomogeneous(0, x, y);
      exportData.setVertex(
        sampleIndex,
        x, y);
      exportData.setVertexValidFlag(
        sampleIndex,
        isValidFlag);
    }
  }

  //---------------------------
  // Triangle sample data
  std::list<SB2DTriangle*>::const_iterator tit;
  uint32_t triangleIndex = 0;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit, ++triangleIndex)
  {
    // Get the triangle vertex IDs
    const SB2DTriangle* triangle = *tit;
    NMP_VERIFY(triangle);

    // Build the triangle sub-sample map for this triangle's configuration of the
    // stored vertex, edge and triangle facet elements.
    triangle->getTriangleSubSampleElementMap(elementMap);

    // Get the valid triangle sub-sample data
    triangle->getTriangleSubSampleData(
      elementMap,
      triangleSubSampleData);

    // Set the triangle vertex IDs
    uint32_t triangleVertexIDs[3];
    triangleSubSampleData.getTriangleVertexIDs(
      triangleVertexIDs[0],
      triangleVertexIDs[1],
      triangleVertexIDs[2]);
    exportData.setTriangleVertexIDs(
      triangleIndex,
      triangleVertexIDs[0],
      triangleVertexIDs[1],
      triangleVertexIDs[2]);

    // Set the triangle sub-sample export data
    for (uint32_t i = 0; i < numSubSamplesPerTriangle; ++i)
    {
      float x, y;
      triangleSubSampleData.getTriangleSubSample(i, x, y);
      exportData.setTriangleSubSample(triangleIndex, i, x, y);

      bool isValidFlag = triangleSubSampleData.getTriangleSubSampleValidFlag(i);
      exportData.setTriangleSubSampleValidFlag(triangleIndex, i, isValidFlag);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::setAnnotation(
  const SB2DAnnotationExportDataPacked& exportData)
{
  //---------------------------
  // Init the annotation graph
  initAnnotation(exportData.getNumTriangleSubDivisions());

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(m_numTriangleSubDivisions);

  uint32_t numSubSamplesPerTriangle = subDivisionDataBlock.getNumSubSamplesPerTriangle();
  uint32_t numInteriorSubSamplesPerEdge = subDivisionDataBlock.getNumInteriorSubSamplesPerEdge();
  uint32_t numInteriorSubSamplesPerTriangle = subDivisionDataBlock.getNumInteriorSubSamplesPerTriangle();

  //---------------------------
  // Element sub-sample arrays
  std::vector<uint32_t> triangleSubSampleIndices;
  triangleSubSampleIndices.resize(numSubSamplesPerTriangle);

  //---------------------------
  // Add the vertices
  uint32_t numVertices = exportData.getNumVertices();
  for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
  {
    // Get the vertex information
    float x, y;
    exportData.getSample(vertexIndex, x, y);
    
    // Add a new vertex to the annotation graph
    SB2DVertex* vertex = addVertex(vertexIndex);
    vertex->setElementSubSampleInhomogeneous(0, x, y);
    vertex->setElementSubSampleValidFlag(0, true);
  }

  //---------------------------
  // Add the triangles
  uint32_t numTriangles = exportData.getNumTriangles();
  for (uint32_t triangleIndex = 0; triangleIndex < numTriangles; ++triangleIndex)
  {
    // Get the triangle vertex sub-sample IDs from the export data
    uint32_t triangleVertexIDs[3];
    for (uint32_t i = 0; i < 3; ++i)
    {
      uint32_t triangleSubSampleIndex = subDivisionDataBlock.getVertexSubSampleIndex(i);
      exportData.getTriangleSubSampleIndex(
        triangleIndex,
        triangleSubSampleIndex,
        triangleVertexIDs[i]);
      NMP_VERIFY(triangleVertexIDs[i] < numVertices);
    }

    // Add a new triangle to the annotation graph
    SB2DTriangle* triangle = addTriangle(triangleVertexIDs);
    NMP_VERIFY(triangle);

    //---------------------------
    // Interior edge sub-samples
    for (uint32_t elementIndex = 0; elementIndex < 3; ++elementIndex)
    {
      SB2DEdge* edge = triangle->getEdgeElement(elementIndex);
      NMP_VERIFY(isValidEdge(edge));

      // Compute the triangle sub-sample indices corresponding to the
      // sub-sample indices of the edge element
      triangle->getTriangleSubSampleIndicesForEdgeElement(
        m_numTriangleSubDivisions,
        edge,
        &triangleSubSampleIndices[0]);

      for (uint32_t i = 0; i < numInteriorSubSamplesPerEdge; ++i)
      {
        // Get the sub-sample index within the global flat list
        uint32_t subSampleIndex;
        exportData.getTriangleSubSampleIndex(
          triangleIndex,
          triangleSubSampleIndices[i],
          subSampleIndex);

        // Get the sub-sample data
        float x, y;
        exportData.getSample(
          subSampleIndex,
          x, y);

        // Set the edge sub-sample data
        edge->setElementSubSampleInhomogeneous(i, x, y);
        edge->setElementSubSampleValidFlag(i, true);
      }
    }

    //---------------------------
    // Interior triangle sub-samples
    triangle->getTriangleSubSampleIndicesForTriangleElement(
      m_numTriangleSubDivisions,
      &triangleSubSampleIndices[0]);

    // Set the triangle sub-sample indices
    for (uint32_t i = 0; i < numInteriorSubSamplesPerTriangle; ++i)
    {
      // Get the sub-sample index within the global flat list
      uint32_t subSampleIndex;
      exportData.getTriangleSubSampleIndex(
        triangleIndex,
        triangleSubSampleIndices[i],
        subSampleIndex);

      // Get the sub-sample data
      float x, y;
      exportData.getSample(
        subSampleIndex,
        x, y);

      // Set the triangle sub-sample data
      triangle->setElementSubSampleInhomogeneous(i, x, y);
      triangle->setElementSubSampleValidFlag(i, true);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::setAnnotation(
  const SB2DAnnotationExportDataUnpacked& exportData)
{
  //---------------------------
  // Init the annotation graph
  initAnnotation(exportData.getNumTriangleSubDivisions());

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(m_numTriangleSubDivisions);

  uint32_t numSubSamplesPerTriangle = subDivisionDataBlock.getNumSubSamplesPerTriangle();

  //---------------------------
  // Triangle sub-sample element map
  SB2DTriangleSubSampleElementMap elementMap;
  SB2DTriangleSubSampleData triangleSubSampleData;
  elementMap.init(m_numTriangleSubDivisions);
  triangleSubSampleData.init(m_numTriangleSubDivisions);

  //---------------------------
  // Add the vertices
  uint32_t numVertices = exportData.getNumVertices();
  for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
  {
    // Add a new vertex to the annotation graph.
    SB2DVertex* vertex = addVertex(vertexIndex);

    if (exportData.getVertexValidFlag(vertexIndex))
    {
      float x, y;
      exportData.getVertex(vertexIndex, x, y);

      vertex->setElementSubSampleInhomogeneous(0, x, y);
      vertex->setElementSubSampleValidFlag(0, true);
    }
  }

  //---------------------------
  // Add the triangles
  uint32_t numTriangles = exportData.getNumTriangles();
  for (uint32_t triangleIndex = 0; triangleIndex < numTriangles; ++triangleIndex)
  {
    // Get the triangle vertex sub-sample IDs from the export data
    uint32_t triangleVertexIDs[3];
    exportData.getTriangleVertexIDs(
      triangleIndex,
      triangleVertexIDs[0],
      triangleVertexIDs[1],
      triangleVertexIDs[2]);

    // Add a new triangle to the annotation graph
    SB2DTriangle* triangle = addTriangle(triangleVertexIDs);
    NMP_VERIFY(triangle);

    // Build the triangle sub-sample map for this triangle's configuration of the
    // stored vertex, edge and triangle facet elements.
    triangle->getTriangleSubSampleElementMap(elementMap);

    // Get the triangle sub-sample data from the export
    for (uint32_t i = 0; i < numSubSamplesPerTriangle; ++i)
    {
      float x, y;
      exportData.getTriangleSubSample(
        triangleIndex,
        i,
        x, y);
      triangleSubSampleData.setTriangleSubSample(
        i,
        x, y);

      bool validFlag = exportData.getTriangleSubSampleValidFlag(
        triangleIndex,
        i);
      triangleSubSampleData.setTriangleSubSampleValidFlag(
        i,
        validFlag);
    }

    // Add the valid triangle sub-sample data. Note that we add the sub-sample data
    // as homogeneous vectors to the annotation to average the duplicate vertex, edge and
    // triangle facet entries.
    triangle->addTriangleSubSampleData(
      elementMap,
      triangleSubSampleData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
SB2DVertex* SB2DAnnotation::getVertex(uint32_t ID) const
{
  std::list<SB2DVertex*>::const_iterator vit;
  for (vit = m_vertexList.begin(); vit != m_vertexList.end(); ++vit)
  {
    SB2DVertex* vertex = *vit;
    if (vertex->getVertexID() == ID)
      return vertex;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DTriangle* SB2DAnnotation::getTriangle(const uint32_t* vertexIDs) const
{
  NMP_VERIFY(vertexIDs);
  std::list<SB2DTriangle*>::const_iterator tit;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    SB2DTriangle* triangle = *tit;
    
    // Check the vertices match
    if (triangle->getElementVertexWithID(vertexIDs[0]) &&
        triangle->getElementVertexWithID(vertexIDs[1]) &&
        triangle->getElementVertexWithID(vertexIDs[2]))
    {
      return triangle;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DEdge* SB2DAnnotation::getEdge(const uint32_t* vertexIDs) const
{
  NMP_VERIFY(vertexIDs);
  std::list<SB2DEdge*>::const_iterator eit;
  for (eit = m_edgeList.begin(); eit != m_edgeList.end(); ++eit)
  {
    SB2DEdge* edge = *eit;
    
    // Check the vertices match
    if (edge->getElementVertexWithID(vertexIDs[0]) &&
        edge->getElementVertexWithID(vertexIDs[1]))
    {
      return edge;
    }
    
  }
  
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DVertex* SB2DAnnotation::addVertex(uint32_t ID)
{
  float subSampleWeights[1] = {1.0f};

  // Create and add a vertex to the annotation graph
  NMP_VERIFY(!getVertex(ID));
  SB2DVertex* vertex = new SB2DVertex;
  vertex->setElementVertex(0, vertex);
  vertex->setVertexID(ID);
  vertex->setElementSubSampleWeight(0, subSampleWeights);
  m_vertexList.push_back(vertex);

  return vertex;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DTriangle* SB2DAnnotation::addTriangle(const uint32_t* vertexIDs)
{
  NMP_VERIFY(!getTriangle(vertexIDs));

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(m_numTriangleSubDivisions);

  uint32_t numInteriorSubSamplesPerEdge = subDivisionDataBlock.getNumInteriorSubSamplesPerEdge();
  uint32_t numInteriorSubSamplesPerTriangle = subDivisionDataBlock.getNumInteriorSubSamplesPerTriangle();

  // Find the vertices specified by their IDs
  SB2DVertex* vertices[3];
  for (uint32_t i = 0; i < 3; ++i)
  {
    vertices[i] = getVertex(vertexIDs[i]);
    if (!vertices[i])
      return NULL;
  }

  // TODO: get a unique triangle ID from the annotation
  uint32_t triangleID = 0;

  //---------------------------
  // Create and add a new triangle to the annotation graph
  SB2DTriangle* triangle = new SB2DTriangle(numInteriorSubSamplesPerTriangle);
  triangle->setTriangleID(triangleID);
  for (uint32_t i = 0; i < 3; ++i)
    triangle->setElementVertex(i, vertices[i]);
  m_triangleList.push_back(triangle);
  
  // Update the interior triangle sub-sample weights
  for (uint32_t elementSubSampleIndex = 0; elementSubSampleIndex < numInteriorSubSamplesPerTriangle; ++elementSubSampleIndex)
  {
    // Get the weights for the interior triangle sub-samples
    uint32_t triangleSubSampleIndex = subDivisionDataBlock.getInteriorTriangleSubSampleIndex(elementSubSampleIndex);

    float subSampleWeights[3];
    subDivisionDataBlock.getTriangleSubSampleWeights(
      triangleSubSampleIndex,
      subSampleWeights[0],
      subSampleWeights[1],
      subSampleWeights[2]);
    triangle->setElementSubSampleWeight(
      elementSubSampleIndex,
      subSampleWeights);
  }

  //---------------------------
  // Update the unique vertex connections
  for (uint32_t i = 0; i < 3; ++i)
    vertices[i]->appendTriangleConnection(triangle);

  //---------------------------
  // Update the unique edge connections
  for (uint32_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
  {
    // Get the edge vertices
    SB2DVertex* edgeVertexA;
    SB2DVertex* edgeVertexB;
    triangle->getEdgeVertices(
      edgeIndex,
      edgeVertexA,
      edgeVertexB);
    NMP_VERIFY(edgeVertexA != edgeVertexB);

    // Check if the edge already exists in the annotation graph
    uint32_t edgeVertexIDs[2];
    edgeVertexIDs[0] = edgeVertexA->getVertexID();
    edgeVertexIDs[1] = edgeVertexB->getVertexID();
    SB2DEdge* edge = getEdge(edgeVertexIDs);

    if (!edge)
    {
      // Create and add a new edge to the annotation graph
      edge = new SB2DEdge(numInteriorSubSamplesPerEdge);
      edge->setElementVertex(0, edgeVertexA);
      edge->setElementVertex(1, edgeVertexB);
      m_edgeList.push_back(edge);

      // Update the interior edge sub-sample weights
      for (uint32_t elementSubSampleIndex = 0; elementSubSampleIndex < numInteriorSubSamplesPerEdge; ++elementSubSampleIndex)
      {
        // Get the weights for the interior triangle sub-samples
        float subSampleWeights[2];
        subDivisionDataBlock.getInteriorEdgeSubSampleWeight(
          elementSubSampleIndex,
          subSampleWeights[0],
          subSampleWeights[1]);
        edge->setElementSubSampleWeight(
          elementSubSampleIndex,
          subSampleWeights);
      }
    }

    // Update the connections
    NMP_VERIFY(!edge->isConnectedToTriangle(triangle));
    edge->appendTriangleConnection(triangle);

    NMP_VERIFY(!triangle->getEdgeElement(edgeIndex));
    triangle->setEdgeElement(edgeIndex, edge);
  }

  return triangle;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DTriangle* SB2DAnnotation::addTriangle(const SB2DTriangleSubSampleData* triangleSubSampleData)
{
  NMP_VERIFY(triangleSubSampleData);
  SB2DTriangle* triangle = addTriangle(triangleSubSampleData->getTriangleVertexIDs());
  if (triangle)
  {
    // Build the triangle sub-sample map for this triangle's configuration of the
    // stored vertex, edge and triangle facet elements.
    SB2DTriangleSubSampleElementMap elementMap;
    elementMap.init(triangleSubSampleData->getNumTriangleSubDivisions());
    triangle->getTriangleSubSampleElementMap(elementMap);

    // Add the valid triangle sub-sample data. Note that we add the sub-sample data
    // as homogeneous vectors to the annotation to average the duplicate vertex, edge and
    // triangle facet entries.
    triangle->addTriangleSubSampleData(
      elementMap,
      *triangleSubSampleData);
  }

  return triangle;
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::removeVertex(SB2DVertex* vertex)
{
  NMP_VERIFY(vertex);

  // Find the vertex within the list
  std::list<SB2DVertex*>::const_iterator vit;
  for (vit = m_vertexList.begin(); vit != m_vertexList.end(); ++vit)
  {
    SB2DVertex* V = *vit;
    if (V == vertex)
    {
      // Remove all triangles that are connected to the vertex
      uint32_t numConnections = vertex->getNumTriangleConnections();
      for (uint32_t i = 0; i < numConnections; ++i)
      {
        SB2DTriangle* triangle = vertex->getFrontTriangleConnection();
        removeTriangle(triangle);
      }

      // Remove and delete the vertex
      delete vertex;
      m_vertexList.erase(vit);
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::removeTriangle(SB2DTriangle* triangle)
{
  // Find the triangle in the list
  NMP_VERIFY(triangle);
  std::list<SB2DTriangle*>::iterator tit;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    SB2DTriangle* T = *tit;
    if (T == triangle)
    {
      // Update the vertex connections
      for (uint32_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
      {
        // Remove the triangle from the vertex connections
        SB2DVertex* vertex = triangle->getElementVertex(vertexIndex);
        NMP_VERIFY(vertex);
        NMP_VERIFY(vertex->isConnectedToTriangle(triangle));
        vertex->removeTriangleConnection(triangle);
      }

      // Update the edge connections
      for (uint32_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
      {
        // Remove the triangle from the edge connections. We will delete
        // any edges later if they contain no remaining connections.
        SB2DEdge* edge = triangle->getEdgeElement(edgeIndex);
        NMP_VERIFY(edge);
        NMP_VERIFY(edge->isConnectedToTriangle(triangle));
        edge->removeTriangleConnection(triangle);
      }

      // Remove and delete the triangle
      delete triangle;
      m_triangleList.erase(tit);
      
      //---------------------------
      // Remove any edges that have no remaining connections
      std::list<SB2DEdge*>::iterator eit = m_edgeList.begin();
      while (eit != m_edgeList.end())
      {
        SB2DEdge* edge = *eit;
        uint32_t numTriangleConnections = edge->getNumTriangleConnections();
        if (numTriangleConnections == 0)
        {
          delete edge;
          eit = m_edgeList.erase(eit);
        }
        else
        {
          ++eit;
        }
      }
      
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::getSingletonTriangles(std::list<SB2DTriangle*>& triangleList) const
{
  NMP_VERIFY(triangleList.empty());

  // A correctly segmented annotation should contain triangles whose
  // facets are only shared by its neighbours.
  std::list<SB2DTriangle*>::const_iterator tit;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    SB2DTriangle* triangle = *tit;

    // Recover the triangle connection counts
    uint32_t triangleConnectionCounts[3];
    for (uint32_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
    {
      SB2DEdge* edge = triangle->getEdgeElement(edgeIndex);
      NMP_VERIFY(edge);
      triangleConnectionCounts[edgeIndex] = edge->getNumTriangleConnections();
    }

    // Check for a singleton triangle
    if (triangleConnectionCounts[0] == 1 &&
        triangleConnectionCounts[1] == 1 &&
        triangleConnectionCounts[2] == 1)
    {
      triangleList.push_back(triangle);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::getRedundantTriangles(std::list<SB2DTriangle*>& triangleList) const
{
  NMP_VERIFY(triangleList.empty());

  // A correctly segmented annotation should contain triangles whose
  // facets are only shared by its neighbours. Annotating the space automatically
  // using groups can cause redundant triangles to be added to concave edges.
  // We can detect these overlapping triangles since the facet connection counts
  // will be greater than two. We can remove redundant triangles as long as the
  // removal does not expose additional external facets.
  std::list<SB2DTriangle*>::const_iterator tit;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    SB2DTriangle* triangle = *tit;

    // Recover the triangle connection counts
    uint32_t triangleConnectionCounts[3];
    for (uint32_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
    {
      SB2DEdge* edge = triangle->getEdgeElement(edgeIndex);
      NMP_VERIFY(edge);
      triangleConnectionCounts[edgeIndex] = edge->getNumTriangleConnections();
    }

    // Check for a singleton triangle
    bool canRemove = true;
    if (triangleConnectionCounts[0] == 1 &&
        triangleConnectionCounts[1] == 1 &&
        triangleConnectionCounts[2] == 1)
    {
      canRemove = false;
    }
    else
    {
      // We can remove triangles that have edge facets with only a single connection count
      // (it is an external facet), or facets with connection counts > 2 (overlapping facets).
      if (triangleConnectionCounts[0] == 2 ||
          triangleConnectionCounts[1] == 2 ||
          triangleConnectionCounts[2] == 2)
      {
        // We can't expose additional external facets
        canRemove = false;
      }
    }

    // Add the triangle to the list
    if (canRemove)
    {
      triangleList.push_back(triangle);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SB2DAnnotation::removeRedundantTriangles()
{
  std::list<SB2DTriangle*> redundantTriangles;
  getRedundantTriangles(redundantTriangles);
  uint32_t result = (uint32_t)redundantTriangles.size();

  std::list<SB2DTriangle*>::iterator tit;
  for (tit = redundantTriangles.begin(); tit != redundantTriangles.end(); ++tit)
  {
    removeTriangle(*tit);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::removeAllTrianglesFromAnnotation()
{
  // Delete all edges
  std::list<SB2DEdge*>::iterator eit;
  for (eit = m_edgeList.begin(); eit != m_edgeList.end(); ++eit)
  {
    SB2DEdge* edge = *eit;
    delete edge;
  }

  // Delete all triangles
  std::list<SB2DTriangle*>::iterator tit;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    SB2DTriangle* triangle = *tit;
    delete triangle;
  }

  // Wipe down all the vertex connections
  std::list<SB2DVertex*>::iterator vit;
  for (vit = m_vertexList.begin(); vit != m_vertexList.end(); ++vit)
  {
    SB2DVertex* vertex = *vit;
    vertex->removeAllTriangleConnections();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::removeTrianglesInVertexSetFromAnnotation(
  uint32_t numVertices,
  const uint32_t* triangleVertexIDs)
{
  // Triangle interior sub-sample data
  std::list<SB2DTriangle*>::iterator tit = m_triangleList.begin();
  while (tit != m_triangleList.end())
  {
    // Get the triangle vertex IDs
    SB2DTriangle* triangle = *tit;
    bool canRemove = true;
    for (uint32_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
    {
      SB2DVertex* vertex = triangle->getElementVertex(vertexIndex);
      uint32_t vertexID = vertex->getVertexID();
      bool found = false;
      for (uint32_t index = 0; index < numVertices; ++index)
      {
        if (vertexID == triangleVertexIDs[index])
        {
          found = true;
          break;
        }
      }
      if (!found)
      {
        canRemove = false; // Vertex did not match
        break;
      }
    }

    // Update the iterator before possibly removing the triangle
    ++tit;

    // Check if the triangle vertices matched
    if (canRemove)
      removeTriangle(triangle);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SB2DAnnotation::calculateNumSubSamples() const
{
  uint32_t numSubSamples = 0;

  // Triangle interior edge sub-sample data
  std::list<SB2DEdge*>::const_iterator eit;
  for (eit = m_edgeList.begin(); eit != m_edgeList.end(); ++eit)
  {
    SB2DEdge* edge = *eit;
    numSubSamples += edge->getNumElementSubSamples();
  }

  // Triangle interior sub-sample data
  std::list<SB2DTriangle*>::const_iterator tit;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    SB2DTriangle* triangle = *tit;
    numSubSamples += triangle->getNumElementSubSamples();
  }

  return numSubSamples;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SB2DAnnotation::calculateNumExteriorEdgeFacets() const
{
  uint32_t numExteriorEdgeFacets = 0;
  std::list<SB2DTriangle*>::const_iterator tit = m_triangleList.begin();
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    const SB2DTriangle* triangle = *tit;
    for (uint32_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
    {
      const SB2DEdge* edge = triangle->getEdgeElement(edgeIndex);
      NMP_VERIFY(edge);
      uint32_t numTriangleConnections = edge->getNumTriangleConnections();
      NMP_VERIFY(numTriangleConnections > 0);

      // Check for an external edge facet
      if (numTriangleConnections == 1)
      {
        ++numExteriorEdgeFacets;
      }
    }
  }

  return numExteriorEdgeFacets;
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotation::getExteriorTriangles(
  std::list<SB2DTriangle*>& triangleList) const
{
  std::list<SB2DTriangle*>::const_iterator tit = m_triangleList.begin();
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    SB2DTriangle* triangle = *tit;
    for (uint32_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
    {
      const SB2DEdge* edge = triangle->getEdgeElement(edgeIndex);
      NMP_VERIFY(edge);
      uint32_t numTriangleConnections = edge->getNumTriangleConnections();
      NMP_VERIFY(numTriangleConnections > 0);

      // Check for an external edge facet
      if (numTriangleConnections == 1)
      {
        triangleList.push_back(triangle);
        break;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DAnnotation::isValidVertex(const SB2DVertex* vertex) const
{
  std::list<SB2DVertex*>::const_iterator vit;
  for (vit = m_vertexList.begin(); vit != m_vertexList.end(); ++vit)
  {
    SB2DVertex* V = *vit;
    NMP_VERIFY(V);
    if (vertex == V)
      return true;
  }
  
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DAnnotation::isValidEdge(const SB2DEdge* edge) const
{
  std::list<SB2DEdge*>::const_iterator eit;
  for (eit = m_edgeList.begin(); eit != m_edgeList.end(); ++eit)
  {
    const SB2DEdge* E = *eit;
    NMP_VERIFY(E);
    if (edge == E)
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DAnnotation::isValidTriangle(const SB2DTriangle* triangle) const
{
  std::list<SB2DTriangle*>::const_iterator tit;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    const SB2DTriangle* T = *tit;
    NMP_VERIFY(T);
    if (triangle == T)
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DAnnotation::isValidElementSubSampleData() const
{
  //---------------------------
  // Vertex samples
  std::list<SB2DVertex*>::const_iterator vit;
  for (vit = m_vertexList.begin(); vit != m_vertexList.end(); ++vit)
  {
    const SB2DVertex* vertex = *vit;
    NMP_VERIFY(vertex);
    uint32_t numElementSubSamples = vertex->getNumElementSubSamples();
    if (numElementSubSamples > 0 && !vertex->getElementSubSampleValidFlags())
    {
      return false;
    }
  }

  //---------------------------
  // Interior edge sub-samples
  std::list<SB2DEdge*>::const_iterator eit;
  for (eit = m_edgeList.begin(); eit != m_edgeList.end(); ++eit)
  {
    // Check if the edge data is valid
    SB2DEdge* edge = *eit;
    NMP_VERIFY(edge);
    uint32_t numElementSubSamples = edge->getNumElementSubSamples();
    if (numElementSubSamples > 0 && !edge->getElementSubSampleValidFlags())
    {
      return false;
    }
  }

  //---------------------------
  // Interior triangle sub-samples
  std::list<SB2DTriangle*>::const_iterator tit;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    // Check if the triangle data is valid
    const SB2DTriangle* triangle = *tit;
    NMP_VERIFY(triangle);
    uint32_t numElementSubSamples = triangle->getNumElementSubSamples();
    if (numElementSubSamples > 0 && !triangle->getElementSubSampleValidFlags())
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DAnnotation::isValidVertexOrdering() const
{
  std::list<SB2DTriangle*>::const_iterator tit;
  for (tit = m_triangleList.begin(); tit != m_triangleList.end(); ++tit)
  {
    // Check if the triangle data is valid
    const SB2DTriangle* triangle = *tit;
    NMP_VERIFY(triangle);
    if (!triangle->isValidVertexOrdering())
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DAnnotation::isValidTriangleConnectivity() const
{
  // Check for only a single triangle in the annotation
  if (getNumTriangles() > 1)
  {
    // All triangles must be connected together
    std::list<SB2DTriangle*> triangleList;
    getSingletonTriangles(triangleList);
    return triangleList.empty();
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SB2DAnnotation::getNumUnconnectedVertices() const
{
  std::list<SB2DVertex*>::const_iterator vit;
  uint32_t count = 0;
  for (vit = m_vertexList.begin(); vit != m_vertexList.end(); ++vit)
  {
    const SB2DVertex* vertex = *vit;
    uint32_t numTriangleConnections = vertex->getNumTriangleConnections();
    if (numTriangleConnections == 0)
      count++;
  }
  return count;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeClassifier
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeClassifierEntries::SB2DAnnotationBSPTreeClassifierEntries() :
  m_numTriangleEntries(0),
  m_numHyperPlaneEntries(0),
  m_classEntries(NULL),
  m_leftCounts(NULL),
  m_bothCounts(NULL),
  m_rightCounts(NULL),
  m_incorrectCounts(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeClassifierEntries::~SB2DAnnotationBSPTreeClassifierEntries()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeClassifierEntries::init(
  uint32_t numHyperPlaneEntries,
  uint32_t numTriangleEntries)
{
  NMP_VERIFY(!m_classEntries); // Should have been released
  m_numTriangleEntries = numTriangleEntries;
  m_numHyperPlaneEntries = numHyperPlaneEntries;

  uint32_t numClassEntries = numHyperPlaneEntries * numTriangleEntries;
  m_classEntries = new int8_t[numClassEntries];
  m_leftCounts = new uint16_t[numHyperPlaneEntries];
  m_bothCounts = new uint16_t[numHyperPlaneEntries];
  m_rightCounts = new uint16_t[numHyperPlaneEntries];
  m_incorrectCounts = new uint16_t[numHyperPlaneEntries];
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeClassifierEntries::release()
{
  m_numTriangleEntries = 0;
  m_numHyperPlaneEntries = 0;

  if (m_classEntries)
  {
    delete[] m_classEntries;
    m_classEntries = NULL;
  }

  if (m_leftCounts)
  {
    delete[] m_leftCounts;
    m_leftCounts = NULL;
  }

  if (m_bothCounts)
  {
    delete[] m_bothCounts;
    m_bothCounts = NULL;
  }

  if (m_rightCounts)
  {
    delete[] m_rightCounts;
    m_rightCounts = NULL;
  }

  if (m_incorrectCounts)
  {
    delete[] m_incorrectCounts;
    m_incorrectCounts = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeClassifierEntries::clear()
{
  uint32_t numClassEntries = m_numHyperPlaneEntries * m_numTriangleEntries;
  for (uint32_t i = 0; i < numClassEntries; ++i)
    m_classEntries[i] = 0;

  for (uint32_t i = 0; i < m_numHyperPlaneEntries; ++i)
  {
    m_leftCounts[i] = 0;
    m_bothCounts[i] = 0;
    m_rightCounts[i] = 0;
    m_incorrectCounts[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeSampleEntries
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeSampleEntries::SB2DAnnotationBSPTreeSampleEntries() :
  m_numSampleEntries(0),
  m_samplesX(NULL),
  m_samplesY(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeSampleEntries::~SB2DAnnotationBSPTreeSampleEntries()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeSampleEntries::init(uint32_t numSampleEntries)
{
  m_numSampleEntries = numSampleEntries;
  if (numSampleEntries > 0)
  {
    m_samplesX = new float[numSampleEntries];
    m_samplesY = new float[numSampleEntries];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeSampleEntries::release()
{
  if (m_samplesX)
  {
    delete[] m_samplesX;
    m_samplesX = NULL;
  }

  if (m_samplesY)
  {
    delete[] m_samplesY;
    m_samplesY = NULL;
  }

  m_numSampleEntries = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeSampleEntries::clear()
{
  for (uint32_t i = 0; i < m_numSampleEntries; ++i)
  {
    m_samplesX[i] = 0.0f;
    m_samplesY[i] = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeClassifier
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeTriangleEntries::SB2DAnnotationBSPTreeTriangleEntries()
{
  m_numTrianglesEntries = 0;

  for (uint32_t k = 0; k < 3; ++k)
  {
    m_verticesX[k] = NULL;
    m_verticesY[k] = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeTriangleEntries::~SB2DAnnotationBSPTreeTriangleEntries()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeTriangleEntries::init(uint32_t numTriangleEntries)
{
  m_numTrianglesEntries = numTriangleEntries;
  if (numTriangleEntries > 0)
  {
    for (uint32_t k = 0; k < 3; ++k)
    {
      NMP_VERIFY(!m_verticesX[k]);
      NMP_VERIFY(!m_verticesY[k]);
      m_verticesX[k] = new float[numTriangleEntries];
      m_verticesY[k] = new float[numTriangleEntries];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeTriangleEntries::release()
{
  m_numTrianglesEntries = 0;

  for (uint32_t k = 0; k < 3; ++k)
  {
    if (m_verticesX[k])
    {
      delete[] m_verticesX[k];
      m_verticesX[k] = NULL;
    }

    if (m_verticesY[k])
    {
      delete[] m_verticesY[k];
      m_verticesY[k] = NULL;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeTriangleEntries::clear()
{
  for (uint32_t k = 0; k < 3; ++k)
  {
    for (uint32_t i = 0; i < m_numTrianglesEntries; ++i)
    {
      m_verticesX[k][i] = 0.0f;
      m_verticesY[k][i] = 0.0f;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeHyperPlaneEntries
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeHyperPlaneEntries::SB2DAnnotationBSPTreeHyperPlaneEntries()
{
  m_numHyperPlaneEntries = 0;
  for (uint32_t k = 0; k < 3; ++k)
  {
    m_hyperPlanes[k] = NULL;
  }

  for (uint32_t k = 0; k < 2; ++k)
  {
    m_hyperPlaneSampleIndices[k] = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeHyperPlaneEntries::~SB2DAnnotationBSPTreeHyperPlaneEntries()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeHyperPlaneEntries::init(uint32_t numHyperPlaneEntries)
{
  m_numHyperPlaneEntries = numHyperPlaneEntries;
  if (numHyperPlaneEntries > 0)
  {
    for (uint32_t k = 0; k < 3; ++k)
    {
      NMP_VERIFY(!m_hyperPlanes[k]);
      m_hyperPlanes[k] = new float[numHyperPlaneEntries];
    }

    for (uint32_t k = 0; k < 2; ++k)
    {
      NMP_VERIFY(!m_hyperPlaneSampleIndices[k]);
      m_hyperPlaneSampleIndices[k] = new uint16_t[numHyperPlaneEntries];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeHyperPlaneEntries::release()
{
  m_numHyperPlaneEntries = 0;
  for (uint32_t k = 0; k < 3; ++k)
  {
    if (m_hyperPlanes[k])
    {
      delete[] m_hyperPlanes[k];
      m_hyperPlanes[k] = NULL;
    }
  }

  for (uint32_t k = 0; k < 2; ++k)
  {
    if (m_hyperPlaneSampleIndices[k])
    {
      delete[] m_hyperPlaneSampleIndices[k];
      m_hyperPlaneSampleIndices[k] = NULL;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeHyperPlaneEntries::clear()
{
  for (uint32_t k = 0; k < 3; ++k)
  {
    for (uint32_t i = 0; i < m_numHyperPlaneEntries; ++i)
    {
      m_hyperPlanes[k][i] = 0.0f;
    }
  }

  for (uint32_t k = 0; k < 2; ++k)
  {
    for (uint32_t i = 0; i < m_numHyperPlaneEntries; ++i)
    {
      m_hyperPlaneSampleIndices[k][i] = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeClassifier::SB2DAnnotationBSPTreeClassifier() :
  m_numTriangles(0),
  m_numSubSimplexesPerTriangle(0),
  m_triangleEntries(NULL),
  m_subTriangleEntries(NULL),
  m_samplesEntries(NULL),
  m_hyperPlaneEntries(NULL),
  m_triangleClassEntries(NULL),
  m_subTriangleClassEntries(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeClassifier::~SB2DAnnotationBSPTreeClassifier()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeClassifier::init(
  const SB2DAnnotationExportDataPacked& exportdata)
{
  // Release the old data
  release();

  //---------------------------
  // Initialise the classifier
  m_numTriangles = exportdata.getNumTriangles();
  m_numSubSimplexesPerTriangle = exportdata.getNumSubSimplexesPerTriangle();
  uint32_t numSubSamplesPerTriangle = exportdata.getNumSubSamplesPerTriangle();
  uint32_t numHyperPlanes = 3 * m_numTriangles;

  m_triangleEntries = new SB2DAnnotationBSPTreeTriangleEntries;
  m_triangleEntries->init(m_numTriangles);
  m_triangleEntries->clear();

  m_subTriangleEntries = new SB2DAnnotationBSPTreeTriangleEntries*[m_numTriangles];
  for (uint32_t i = 0; i < m_numTriangles; ++i)
  {
    m_subTriangleEntries[i] = new SB2DAnnotationBSPTreeTriangleEntries;
    m_subTriangleEntries[i]->init(m_numSubSimplexesPerTriangle);
    m_subTriangleEntries[i]->clear();
  }

  // Triangle centroids
  m_samplesEntries = new SB2DAnnotationBSPTreeSampleEntries;
  m_samplesEntries->init(m_numTriangles);
  m_samplesEntries->clear();

  m_hyperPlaneEntries = new SB2DAnnotationBSPTreeHyperPlaneEntries;
  m_hyperPlaneEntries->init(numHyperPlanes);
  m_hyperPlaneEntries->clear();

  m_triangleClassEntries = new SB2DAnnotationBSPTreeClassifierEntries;
  m_triangleClassEntries->init(numHyperPlanes, m_numTriangles);
  m_triangleClassEntries->clear();

  m_subTriangleClassEntries = new SB2DAnnotationBSPTreeClassifierEntries*[m_numTriangles];
  for (uint32_t i = 0; i < m_numTriangles; ++i)
  {
    m_subTriangleClassEntries[i] = new SB2DAnnotationBSPTreeClassifierEntries;
    m_subTriangleClassEntries[i]->init(numHyperPlanes, m_numSubSimplexesPerTriangle);
    m_subTriangleClassEntries[i]->clear();
  }

  //---------------------------
  // Initialise the hyper plane entries. Note that we limit the hyper-plane sectioning to
  // planes cutting the major vertices of each parent triangle. This ensures that partitions
  // contain sub-triangles mostly from the same parent triangle. If we use all possible
  // hyper-planes from all the sub-triangles then:
  // 1) We run the risk of computing partitions that fragment the parent triangles quite badly.
  //    Highly fragmented triangles will require more memory in the runtime binary attrib data.
  // 2) We will need a lot of memory during the classification stage to determine suitable
  //    partitions.
  // 3) A lot of processing will be required during classification to test which side
  //    of the hyper-planes each of the sub-triangles lay. Again, most of this processing time
  //    is expended determining optimal sub-triangle sets that fragment the parent triangles
  //    (a property that we would like to avoid).
  uint32_t hyperPlaneEntryIndex = 0;
  for (uint32_t triangleIndex = 0; triangleIndex < m_numTriangles; ++triangleIndex)
  {
    NMP::Vector3 A, B, C;
    NMP::Vector3 N;
    float mag, recipMag;
    A.setToZero();
    B.setToZero();
    C.setToZero();
    N.setToZero();

    // Get the sample indices for the triangle's major vertices
    uint32_t sampleIndices[3];
    exportdata.getTriangleSubSampleIndices(
      triangleIndex,
      sampleIndices[0],
      sampleIndices[1],
      sampleIndices[2]);

    // Set the triangle entry
    exportdata.getSample(sampleIndices[0], A.x, A.y);
    exportdata.getSample(sampleIndices[1], B.x, B.y);
    exportdata.getSample(sampleIndices[2], C.x, C.y);
    m_triangleEntries->setTriangleEntry(triangleIndex, A, B, C);

    //---------------------------
    // BC hyper-plane
    N.x = B.y - C.y;
    N.y = C.x - B.x;
    N.w = B.x * C.y - C.x * B.y;
    mag = sqrtf(N.x * N.x + N.y * N.y);
    NMP_VERIFY(mag > 0.0f);
    recipMag = 1.0f / mag;
    N.x *= recipMag;
    N.y *= recipMag;
    N.w *= recipMag;
    m_hyperPlaneEntries->setHyperPlaneEntry(
      hyperPlaneEntryIndex,
      N);
    m_hyperPlaneEntries->setHyperPlaneSampleIndices(
      hyperPlaneEntryIndex,
      (uint16_t)sampleIndices[1],
      (uint16_t)sampleIndices[2]);
    hyperPlaneEntryIndex++;

    //---------------------------
    // CA hyper-plane
    N.x = C.y - A.y;
    N.y = A.x - C.x;
    N.w = C.x * A.y - A.x * C.y;
    mag = sqrtf(N.x * N.x + N.y * N.y);
    NMP_VERIFY(mag > 0.0f);
    recipMag = 1.0f / mag;
    N.x *= recipMag;
    N.y *= recipMag;
    N.w *= recipMag;
    m_hyperPlaneEntries->setHyperPlaneEntry(
      hyperPlaneEntryIndex,
      N);
    m_hyperPlaneEntries->setHyperPlaneSampleIndices(
      hyperPlaneEntryIndex,
      (uint16_t)sampleIndices[2],
      (uint16_t)sampleIndices[0]);
    hyperPlaneEntryIndex++;

    //---------------------------
    // AB hyper-plane
    N.x = A.y - B.y;
    N.y = B.x - A.x;
    N.w = A.x * B.y - B.x * A.y;
    mag = sqrtf(N.x * N.x + N.y * N.y);
    NMP_VERIFY(mag > 0.0f);
    recipMag = 1.0f / mag;
    N.x *= recipMag;
    N.y *= recipMag;
    N.w *= recipMag;
    m_hyperPlaneEntries->setHyperPlaneEntry(
      hyperPlaneEntryIndex,
      N);
    m_hyperPlaneEntries->setHyperPlaneSampleIndices(
      hyperPlaneEntryIndex,
      (uint16_t)sampleIndices[0],
      (uint16_t)sampleIndices[1]);
    hyperPlaneEntryIndex++;

    //---------------------------
    // Iterate over the sub-triangles
    SB2DAnnotationBSPTreeTriangleEntries* subTriangleEntries = m_subTriangleEntries[triangleIndex];
    for (uint32_t subDivTriangleIndex = 0; subDivTriangleIndex < m_numSubSimplexesPerTriangle; ++subDivTriangleIndex)
    {
      // Get the sample indices for the sub-triangle
      exportdata.getSubTriangleSubSampleIndices(
        triangleIndex,
        subDivTriangleIndex,
        sampleIndices[0],
        sampleIndices[1],
        sampleIndices[2]);

      // Set the triangle entry
      exportdata.getSample(sampleIndices[0], A.x, A.y);
      exportdata.getSample(sampleIndices[1], B.x, B.y);
      exportdata.getSample(sampleIndices[2], C.x, C.y);
      subTriangleEntries->setTriangleEntry(subDivTriangleIndex, A, B, C);
    }

    // Compute the triangle centroid
    NMP::Vector3 X;
    X.setToZero();
    for (uint32_t triangleSubSampleIndex = 0; triangleSubSampleIndex < numSubSamplesPerTriangle; ++triangleSubSampleIndex)
    {
      uint32_t sampleIndex;
      exportdata.getTriangleSubSampleIndex(
        triangleIndex,
        triangleSubSampleIndex,
        sampleIndex);
      exportdata.getSample(sampleIndex, A.x, A.y);
      X += A;
    }
    X /= (float)m_numTriangles;
    m_samplesEntries->setSampleEntry(triangleIndex, X);
  }

  //---------------------------
  // Classify the partitioning of the sub-division triangles using the hyper-planes
  for (hyperPlaneEntryIndex = 0; hyperPlaneEntryIndex < numHyperPlanes; ++hyperPlaneEntryIndex)
  {
    SB2DAnnotationBSPTreeTriangleEntries* subTriangleEntries;
    SB2DAnnotationBSPTreeClassifierEntries* subTriangleClassEntries;

    // Get the hyper-plane entry
    NMP::Vector3 N, P;
    m_hyperPlaneEntries->getHyperPlaneEntry(hyperPlaneEntryIndex, N);

    // Iterate over the triangles
    for (uint32_t triangleIndex = 0; triangleIndex < m_numTriangles; ++triangleIndex)
    {
      subTriangleEntries = m_subTriangleEntries[triangleIndex];
      subTriangleClassEntries = m_subTriangleClassEntries[triangleIndex];

      // Compute the actual class label of the triangle based on which side of the hyper-plane
      // the centroid lies.
      int8_t triangleLabel;
      m_samplesEntries->getSampleEntry(triangleIndex, P);
      float d = P.x * N.x + P.y * N.y + N.w;
      if (d < 0.0f)
        triangleLabel = -1;
      else
        triangleLabel = 1;

      //---------------------------
      // Iterate over the sub-division triangles
      uint16_t leftCount = 0;
      uint16_t bothCount = 0;
      uint16_t rightCount = 0;
      uint16_t incorrectCount = 0;
      for (uint32_t subTriangleIndex = 0; subTriangleIndex < m_numSubSimplexesPerTriangle; ++subTriangleIndex)
      {
        // Get the sub-division triangle entry
        NMP::Vector3 X[3];
        subTriangleEntries->getTriangleEntry(subTriangleIndex, X[0], X[1], X[2]);

        // Compute the partition metric
        float metric[3];
        for (uint32_t i = 0; i < 3; ++i)
        {
          metric[i] = X[i].x * N.x + X[i].y * N.y + N.w;
        }

        // Set the classification entry
        int8_t entry;
        const float tol = 1e-5f;
        if (metric[0] <= tol && metric[1] <= tol && metric[2] <= tol)
        {
          entry = -1; // Left
          leftCount++;
        }
        else if (metric[0] >= -tol && metric[1] >= -tol && metric[2] >= -tol)
        {
          entry = 1; // Right
          rightCount++;
        }
        else
        {
          entry = 0; // Both
          bothCount++;
        }

        if (entry != triangleLabel)
          incorrectCount++;

        subTriangleClassEntries->setClassEntry(
          hyperPlaneEntryIndex,
          subTriangleIndex,
          entry);
      }
      subTriangleClassEntries->setLeftCount(hyperPlaneEntryIndex, leftCount);
      subTriangleClassEntries->setBothCount(hyperPlaneEntryIndex, bothCount);
      subTriangleClassEntries->setRightCount(hyperPlaneEntryIndex, rightCount);
      subTriangleClassEntries->setIncorrectCount(hyperPlaneEntryIndex, incorrectCount);
    }

    //---------------------------
    // Compute the parent triangle class entry
    for (uint32_t triangleIndex = 0; triangleIndex < m_numTriangles; ++triangleIndex)
    {
      subTriangleClassEntries = m_subTriangleClassEntries[triangleIndex];
      int8_t triangleClassEntry = subTriangleClassEntries->getClassEntry(hyperPlaneEntryIndex, 0);

      for (uint32_t subTriangleIndex = 1; subTriangleIndex < m_numSubSimplexesPerTriangle; ++subTriangleIndex)
      {
        // All sub-triangle class entries must be the same to classify the parent
        // entry as left or right, otherwise the parent is marked as both.
        int8_t subTriangleClassEntry = subTriangleClassEntries->getClassEntry(hyperPlaneEntryIndex, subTriangleIndex);
        if (subTriangleClassEntry != triangleClassEntry)
          triangleClassEntry = 0; // Both
      }

      m_triangleClassEntries->setClassEntry(
        hyperPlaneEntryIndex,
        triangleIndex,
        triangleClassEntry);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeClassifier::release()
{
  if (m_triangleEntries)
  {
    delete m_triangleEntries;
    m_triangleEntries = NULL;
  }

  if (m_subTriangleEntries)
  {
    for (uint32_t i = 0; i < m_numTriangles; ++i)
    {
      NMP_VERIFY(m_subTriangleEntries[i]);
      delete m_subTriangleEntries[i];
    }
    delete[] m_subTriangleEntries;
    m_subTriangleEntries = NULL;
  }

  if (m_samplesEntries)
  {
    delete m_samplesEntries;
    m_samplesEntries = NULL;
  }

  if (m_hyperPlaneEntries)
  {
    delete m_hyperPlaneEntries;
    m_hyperPlaneEntries = NULL;
  }

  if (m_triangleClassEntries)
  {
    delete m_triangleClassEntries;
    m_triangleClassEntries = NULL;
  }

  if (m_subTriangleClassEntries)
  {
    for (uint32_t i = 0; i < m_numTriangles; ++i)
    {
      NMP_VERIFY(m_subTriangleClassEntries[i]);
      delete m_subTriangleClassEntries[i];
    }
    delete[] m_subTriangleClassEntries;
    m_subTriangleClassEntries = NULL;
  }

  m_numTriangles = 0;
  m_numSubSimplexesPerTriangle = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreePartitionEntry
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreePartitionEntry::SB2DAnnotationBSPTreePartitionEntry(uint32_t numSubTriangleIndices)
{
  m_triangleIndex = 0;
  m_numSubTriangleIndices = numSubTriangleIndices;

  if (numSubTriangleIndices > 0)
  {
    m_subTriangleIndices = new uint32_t[numSubTriangleIndices];
  }
  else
  {
    m_subTriangleIndices = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreePartitionEntry::~SB2DAnnotationBSPTreePartitionEntry()
{
  if (m_subTriangleIndices)
  {
    delete[] m_subTriangleIndices;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTree::BSPNodeMetrics
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTree::BSPNodeMetrics::BSPNodeMetrics()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTree::BSPNodeMetrics::clear()
{
  m_numNodes = 0;
  m_numBranchNodes = 0;
  m_numLeafNodes = 0;
  m_numLeafNodeEntries = 0;

  m_minNumEntries = UINT_MAX;
  m_maxNumEntries = 0;
  m_maxDepth = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTree
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTree::SB2DAnnotationBSPTree() :
  m_validFlag(false),
  m_numTriangles(0),
  m_numSubSimplexesPerTriangle(0),
  m_bspTree(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTree::~SB2DAnnotationBSPTree()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTree::release()
{
  m_validFlag = false;
  m_numTriangles = 0;
  m_numSubSimplexesPerTriangle = 0;

  if (m_bspTree)
  {
    releaseBSPNode(m_bspTree);
    m_bspTree = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTree::getBSPTree(
  SB2DAnnotationBSPTreeExportData& exportData) const
{
  // Release the old export data
  exportData.release();  
  if (!m_bspTree)
    return;

  // Calculate the BSP metrics
  SB2DAnnotationBSPTree::BSPNodeMetrics nodeMetrics;
  calculateBSPMetrics(nodeMetrics, 0, m_bspTree);

  // Initialise the BSP export data
  exportData.init(
    nodeMetrics.m_numBranchNodes,
    nodeMetrics.m_numLeafNodes,
    nodeMetrics.m_numLeafNodeEntries,
    m_numSubSimplexesPerTriangle);

  // Flatten the BSP Tree data
  SB2DAnnotationBSPTree::BSPNodeMetrics state;
  doGetBSPTree(exportData, state, m_bspTree);

  // Set the valid flags
  exportData.setValidFlag(m_validFlag);
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTree::setBSPTree(
  const SB2DAnnotationBSPTreeExportData& exportData)
{
  // Release the old annotation BSP data
  release();
  m_validFlag = exportData.getValidFlag();

  // Unflatten the BSP Tree data
  SB2DAnnotationBSPTree::BSPNodeMetrics state;
  m_bspTree = doSetBSPTree(exportData, state, 0);
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTree::computeBSPTree(const SB2DAnnotationExportDataPacked& exportdata)
{
  // Release the old BSP tree
  release();
  m_numTriangles = exportdata.getNumTriangles();
  m_numSubSimplexesPerTriangle = exportdata.getNumSubSimplexesPerTriangle();
  if (m_numTriangles == 0)
    return;

  uint32_t numTriangleSubDivisions = exportdata.getNumTriangleSubDivisions();

  // Initialise the BSP classifier
  SB2DAnnotationBSPTreeClassifier classifier;
  classifier.init(exportdata);

  // Build the initial (triangle, sub-triangle) index pairs
  std::vector<SB2DAnnotationBSPTreePartitionEntry*> partitionEntries;
  partitionEntries.resize(m_numTriangles);
  for (uint32_t triangleIndex = 0; triangleIndex < m_numTriangles; ++triangleIndex)
  {
    SB2DAnnotationBSPTreePartitionEntry* partitionEntry = new SB2DAnnotationBSPTreePartitionEntry(m_numSubSimplexesPerTriangle);
    partitionEntries[triangleIndex] = partitionEntry;

    partitionEntry->m_triangleIndex = triangleIndex;
    for (uint32_t i = 0; i < m_numSubSimplexesPerTriangle; ++i)
    {
      partitionEntry->m_subTriangleIndices[i] = i;
    }    
  }

  // Recursively cut the set of sub-division triangles until there are fewer triangles
  // in the set than the partition limit. Note computing BSP trees using a metric that purely
  // balances the partitions generates a lot of duplicate triangles in both left and right sets.
  float costAlphas[3] = {0.333f, 0.666f, 1.0f};
  NMP::Memory::Format memReqsBSP[3];
  BSPNode* bspNodes[3];

  for (uint32_t i = 0; i < 3; ++i)
  {
    // Compute the BSP tree with the current cost weight
    bspNodes[i] = new BSPNode;
    doComputeBSPTree(
      bspNodes[i],
      classifier,
      costAlphas[i],
      partitionEntries);

    // Calculate the BSP metrics
    SB2DAnnotationBSPTree::BSPNodeMetrics nodeMetrics;
    calculateBSPMetrics(nodeMetrics, 0, bspNodes[i]);

    // Calculate the memory requirements for the attrib data
    memReqsBSP[i] = MR::AttribDataScatterBlend2DDef::getBSPMemoryRequirements(
      numTriangleSubDivisions,
      m_numTriangles,
      nodeMetrics.m_numBranchNodes,
      nodeMetrics.m_numLeafNodes,
      nodeMetrics.m_numLeafNodeEntries);
  }

  // Choose the BSP tree with the smallest memory requirements
  uint32_t minIndex = 0;
  for (uint32_t i = 1; i < 3; ++i)
  {
    if (memReqsBSP[i].size < memReqsBSP[minIndex].size)
    {
      minIndex = i;
    }
  }

  // Release the other BSP trees
  m_bspTree = bspNodes[minIndex];
  for (uint32_t i = 0; i < 3; ++i)
  {
    if (i != minIndex)
    {
      releaseBSPNode(bspNodes[i]);
    }
  }

  // Tidy up the partition entries
  tidyBSPTreePartitionEntries(partitionEntries);

  // Build the initial triangle entry set
  m_validFlag = true;
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTree::tidyBSPTreePartitionEntries(std::vector<SB2DAnnotationBSPTreePartitionEntry*>& partitionEntries)
{
  uint32_t numPartiationEntries = (uint32_t)partitionEntries.size();
  for (uint32_t entryIndex = 0; entryIndex < numPartiationEntries; ++entryIndex)
  {
    const SB2DAnnotationBSPTreePartitionEntry* partitionEntry = partitionEntries[entryIndex];
    delete partitionEntry;
  }
  partitionEntries.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTree::releaseBSPNode(SB2DAnnotationBSPTree::BSPNode* node)
{
  NMP_VERIFY(node);
  if (node->m_left)
  {
    releaseBSPNode(node->m_left);
    releaseBSPNode(node->m_right);
  }
  else
  {
    if (node->m_triangleIndices)
      delete[] node->m_triangleIndices;
    if (node->m_subTriangleIndices)
      delete[] node->m_subTriangleIndices;
  }
  delete node;
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTree::doComputeBSPTree(
  SB2DAnnotationBSPTree::BSPNode* node,
  SB2DAnnotationBSPTreeClassifier& classifier,
  float costAlpha,
  const std::vector<SB2DAnnotationBSPTreePartitionEntry*>& partitionEntries)
{
  NMP_VERIFY(node);

  // Initialise the BSP node data
  ZeroMemory(node, sizeof(SB2DAnnotationBSPTree::BSPNode));

  //---------------------------
  // Compute the total number of sub-triangles in the input partition entry set
  uint32_t numPartiationEntries = (uint32_t)partitionEntries.size();
  NMP_VERIFY(numPartiationEntries > 0);
  uint32_t partitionEntryCount = 0;
  for (uint32_t entryIndex = 0; entryIndex < numPartiationEntries; ++entryIndex)
  {
    const SB2DAnnotationBSPTreePartitionEntry* partitionEntry = partitionEntries[entryIndex];
    NMP_VERIFY(partitionEntry);
    NMP_VERIFY(partitionEntry->m_numSubTriangleIndices > 0);
    partitionEntryCount += partitionEntry->m_numSubTriangleIndices;
  }
  NMP_VERIFY(partitionEntryCount > 0);

  const uint32_t packetLimit = 4; // Smallest number of sub-triangle indices we should partition the data into
  if (partitionEntryCount > packetLimit)
  {
    SB2DAnnotationBSPTreeClassifierEntries* triangleClassEntries = classifier.getTriangleClassEntries();

    //---------------------------
    // Recompute the parent triangle class entries
    uint32_t numHyperPlaneEntries = triangleClassEntries->getNumHyperPlaneEntries();
    for (uint32_t hyperPlaneIndex = 0; hyperPlaneIndex < numHyperPlaneEntries; ++hyperPlaneIndex)
    {
      // Iterate over the triangles in the partition
      uint16_t leftCount = 0;
      uint16_t bothCount = 0;
      uint16_t rightCount = 0;
      uint16_t incorrectCount = 0;
      for (uint32_t entryIndex = 0; entryIndex < numPartiationEntries; ++entryIndex)
      {
        const SB2DAnnotationBSPTreePartitionEntry* partitionEntry = partitionEntries[entryIndex];
        NMP_VERIFY(partitionEntry);
        const SB2DAnnotationBSPTreeClassifierEntries* subTriangleClassEntries = classifier.getSubTriangleClassEntries(partitionEntry->m_triangleIndex);
        NMP_VERIFY(subTriangleClassEntries);
        NMP_VERIFY(partitionEntry->m_numSubTriangleIndices > 0);

        // First entry in the set
        uint32_t subTriangleIndex = partitionEntry->m_subTriangleIndices[0];
        int8_t triangleClassEntry = subTriangleClassEntries->getClassEntry(hyperPlaneIndex, subTriangleIndex);
        // Remaining entries (must be the same to classify the parent entry as left or right,
        // otherwise the parent is marked as both).
        for (uint32_t index = 1; index < partitionEntry->m_numSubTriangleIndices; ++index)
        {
          subTriangleIndex = partitionEntry->m_subTriangleIndices[index];
          int8_t subTriangleClassEntry = subTriangleClassEntries->getClassEntry(hyperPlaneIndex, subTriangleIndex);
          if (subTriangleClassEntry != triangleClassEntry)
            triangleClassEntry = 0; // Both
        }

        // Left, both, right
        if (triangleClassEntry < 0)
        {
          leftCount++;
        }
        else if (triangleClassEntry > 0)
        {
          rightCount++;
        }
        else
        {
          bothCount++;
        }

        // Incorrect
        incorrectCount += subTriangleClassEntries->getIncorrectCount(hyperPlaneIndex);
      }

      // Set the triangle class entries
      triangleClassEntries->setLeftCount(hyperPlaneIndex, leftCount);
      triangleClassEntries->setBothCount(hyperPlaneIndex, bothCount);
      triangleClassEntries->setRightCount(hyperPlaneIndex, rightCount);
      triangleClassEntries->setIncorrectCount(hyperPlaneIndex, incorrectCount);
    }

    //---------------------------
    // Compute the hyper-plane that best splits the triangle partition entry set into two halves.
    uint32_t minCostIndex = 0xFFFFFFFF;
    uint32_t minCounts[4] = {0, 0, 0, 0}; // left, both, right, incorrect
    float minCost = FLT_MAX;
    for (uint32_t hyperPlaneEntryIndex = 0; hyperPlaneEntryIndex < numHyperPlaneEntries; ++hyperPlaneEntryIndex)
    {
      uint32_t counts[4];
      counts[0] = triangleClassEntries->getLeftCount(hyperPlaneEntryIndex);
      counts[1] = triangleClassEntries->getBothCount(hyperPlaneEntryIndex);
      counts[2] = triangleClassEntries->getRightCount(hyperPlaneEntryIndex);
      counts[3] = triangleClassEntries->getIncorrectCount(hyperPlaneEntryIndex);

      // Only consider hyper-planes that actually partition the data
      if (counts[0] > 0 && counts[2] > 0)
      {
        // We are guaranteed to partition the data into two smaller sets, so we must now
        // consider how badly to fragment these sets. Partitioning the sets evenly using
        // a cost metric abs(left - right) generates highly fragmented triangle sets
        // with many duplicates. There is a high memory cost in the attrib data for storing
        // this. It is best to minimise the amount of triangle entry duplication.
        float diff = NMP::nmfabs((float)counts[0] - (float)counts[2]);
        float cost = (1.0f - costAlpha) * diff + costAlpha * (float)counts[1];

        // Update the minimum cost
        if (cost < minCost || (NMP::nmfabs(cost - minCost) < 1e-5f && counts[3] < minCounts[3]))
        {
          minCostIndex = hyperPlaneEntryIndex;
          minCost = cost;
          minCounts[0] = counts[0];
          minCounts[1] = counts[1];
          minCounts[2] = counts[2];
          minCounts[3] = counts[3];
        }
      }
    }

    //---------------------------
    // Check if we can adequately partition the data. Note that there may be circumstances
    // (especially for skinny edge triangles, whose sub-samples fold in on itself) where
    // we can't partition any further. It may also be inefficient to effectively partition a
    // small number of triangles. Instead we should just create a leaf node for the data.
    if (minCostIndex != 0xFFFFFFFF)
    {
      // Set the BSP hyper plane
      SB2DAnnotationBSPTreeHyperPlaneEntries* hyperPlaneEntries = classifier.getHyperPlaneEntries();
      NMP_VERIFY(hyperPlaneEntries);
      uint16_t hyperPlaneSampleIndices[2];
      hyperPlaneEntries->getHyperPlaneSampleIndices(
        minCostIndex,
        hyperPlaneSampleIndices[0],
        hyperPlaneSampleIndices[1]);
      node->m_hyperPlaneSampleIndices[0] = hyperPlaneSampleIndices[0];
      node->m_hyperPlaneSampleIndices[1] = hyperPlaneSampleIndices[1];

      //---------------------------
      // LEFT
      node->m_left = new BSPNode;
      std::vector<SB2DAnnotationBSPTreePartitionEntry*> leftSet;
      leftSet.reserve(numPartiationEntries);
      for (uint32_t entryIndex = 0; entryIndex < numPartiationEntries; ++entryIndex)
      {
        const SB2DAnnotationBSPTreePartitionEntry* partitionEntry = partitionEntries[entryIndex];
        NMP_VERIFY(partitionEntry);
        const SB2DAnnotationBSPTreeClassifierEntries* subTriangleClassEntries = classifier.getSubTriangleClassEntries(partitionEntry->m_triangleIndex);
        NMP_VERIFY(subTriangleClassEntries);
        uint32_t numEntries = 0;
        for (uint32_t i = 0; i < partitionEntry->m_numSubTriangleIndices; ++i)
        {
          uint32_t subTriangleIndex = partitionEntry->m_subTriangleIndices[i];
          int8_t entry = subTriangleClassEntries->getClassEntry(minCostIndex, subTriangleIndex);
          if (entry <= 0)
            numEntries++;
        }
        if (numEntries > 0)
        {
          SB2DAnnotationBSPTreePartitionEntry* leftPartition = new SB2DAnnotationBSPTreePartitionEntry(numEntries);
          leftSet.push_back(leftPartition);

          leftPartition->m_triangleIndex = partitionEntry->m_triangleIndex;
          numEntries = 0;
          for (uint32_t i = 0; i < partitionEntry->m_numSubTriangleIndices; ++i)
          {
            uint32_t subTriangleIndex = partitionEntry->m_subTriangleIndices[i];
            int8_t entry = subTriangleClassEntries->getClassEntry(minCostIndex, subTriangleIndex);
            if (entry <= 0)
            {
              leftPartition->m_subTriangleIndices[numEntries] = subTriangleIndex;
              numEntries++;
            }
          }
        }
      }
      NMP_VERIFY(!leftSet.empty());
      doComputeBSPTree(
        node->m_left,
        classifier,
        costAlpha,
        leftSet);
      tidyBSPTreePartitionEntries(leftSet);

      //---------------------------
      // RIGHT
      node->m_right = new BSPNode;
      std::vector<SB2DAnnotationBSPTreePartitionEntry*> rightSet;
      rightSet.reserve(numPartiationEntries);
      for (uint32_t entryIndex = 0; entryIndex < numPartiationEntries; ++entryIndex)
      {
        const SB2DAnnotationBSPTreePartitionEntry* partitionEntry = partitionEntries[entryIndex];
        NMP_VERIFY(partitionEntry);
        const SB2DAnnotationBSPTreeClassifierEntries* subTriangleClassEntries = classifier.getSubTriangleClassEntries(partitionEntry->m_triangleIndex);
        NMP_VERIFY(subTriangleClassEntries);
        uint32_t numEntries = 0;
        for (uint32_t i = 0; i < partitionEntry->m_numSubTriangleIndices; ++i)
        {
          uint32_t subTriangleIndex = partitionEntry->m_subTriangleIndices[i];
          int8_t entry = subTriangleClassEntries->getClassEntry(minCostIndex, subTriangleIndex);
          if (entry >= 0)
            numEntries++;
        }
        if (numEntries > 0)
        {
          SB2DAnnotationBSPTreePartitionEntry* rightPartition = new SB2DAnnotationBSPTreePartitionEntry(numEntries);
          rightSet.push_back(rightPartition);

          rightPartition->m_triangleIndex = partitionEntry->m_triangleIndex;
          numEntries = 0;
          for (uint32_t i = 0; i < partitionEntry->m_numSubTriangleIndices; ++i)
          {
            uint32_t subTriangleIndex = partitionEntry->m_subTriangleIndices[i];
            int8_t entry = subTriangleClassEntries->getClassEntry(minCostIndex, subTriangleIndex);
            if (entry >= 0)
            {
              rightPartition->m_subTriangleIndices[numEntries] = subTriangleIndex;
              numEntries++;
            }
          }
        }
      }
      NMP_VERIFY(!rightSet.empty());
      doComputeBSPTree(
        node->m_right,
        classifier,
        costAlpha,
        rightSet);
      tidyBSPTreePartitionEntries(rightSet);

      return;
    }
  }

  //---------------------------
  // Leaf node
  node->m_numEntries = partitionEntryCount;
  node->m_triangleIndices = new uint32_t[partitionEntryCount];
  node->m_subTriangleIndices = new uint32_t[partitionEntryCount];

  // Set the flattened entry indices
  partitionEntryCount = 0;
  for (uint32_t entryIndex = 0; entryIndex < numPartiationEntries; ++entryIndex)
  {
    const SB2DAnnotationBSPTreePartitionEntry* partitionEntry = partitionEntries[entryIndex];
    NMP_VERIFY(partitionEntry);

    for (uint32_t i = 0; i < partitionEntry->m_numSubTriangleIndices; ++i)
    {
      node->m_triangleIndices[partitionEntryCount] = partitionEntry->m_triangleIndex;
      node->m_subTriangleIndices[partitionEntryCount] = partitionEntry->m_subTriangleIndices[i];
      partitionEntryCount++;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTree::writeBSPNode(
  FILE* fp,
  uint32_t depth,
  const BSPNode* node)
{
  NMP_VERIFY(fp);
  NMP_VERIFY(node);

  // Indent text
  char buffer[256];
  uint32_t indentSize = 2 * depth;
  for (uint32_t i = 0; i < indentSize; ++i)
    buffer[i] = ' ';
  buffer[indentSize] = 0;

  if (node->m_left)
  {
    // Branch
    fprintf(fp,
      "%sm_hyperPlaneSampleIndices = %d %d\n",
      buffer,
      node->m_hyperPlaneSampleIndices[0],
      node->m_hyperPlaneSampleIndices[1]);

    writeBSPNode(fp, depth + 1, node->m_left);
    writeBSPNode(fp, depth + 1, node->m_right);
  }
  else
  {
    // Leaf
    fprintf(fp, "%sm_numEntries = %d\n", buffer, node->m_numEntries);
    fprintf(fp, "%sm_triangleIndices = ", buffer);
    for (uint32_t i = 0; i < node->m_numEntries; ++i)
      fprintf(fp, "%d ", node->m_triangleIndices[i]);
    fprintf(fp, "\n%sm_subTriangleIndices = ", buffer);
    for (uint32_t i = 0; i < node->m_numEntries; ++i)
      fprintf(fp, "%d ", node->m_subTriangleIndices[i]);
    fprintf(fp, "\n");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTree::writeBSPTree(const char* filename) const
{
  NMP_VERIFY(filename);
  FILE* fp = fopen(filename, "w");
  if (fp)
  {
    SB2DAnnotationBSPTree::BSPNodeMetrics nodeMetrics;
    calculateBSPMetrics(nodeMetrics, 0, m_bspTree);
    fprintf(fp, "numBranchNodes = %d\n", nodeMetrics.m_numBranchNodes);
    fprintf(fp, "numLeafNodes = %d\n", nodeMetrics.m_numLeafNodes);
    fprintf(fp, "minNumEntries = %d\n", nodeMetrics.m_minNumEntries);
    fprintf(fp, "maxNumEntries = %d\n", nodeMetrics.m_maxNumEntries);
    fprintf(fp, "maxDepth = %d\n", nodeMetrics.m_maxDepth);
    fprintf(fp, "\n");

    writeBSPNode(fp, 0, m_bspTree);
    fclose(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTree::calculateBSPMetrics(
  SB2DAnnotationBSPTree::BSPNodeMetrics& nodeMetrics,
  uint32_t depth,
  const BSPNode* node)
{
  NMP_VERIFY(node);
  nodeMetrics.m_numNodes++;

  if (node->m_left)
  {
    nodeMetrics.m_numBranchNodes++;
    calculateBSPMetrics(nodeMetrics, depth + 1, node->m_left);
    calculateBSPMetrics(nodeMetrics, depth + 1, node->m_right);
  }
  else
  {
    // Calculate the number of different triangles within the partition.
    // This bit of code assumes that the entries have been sorted.
    NMP_VERIFY(node->m_numEntries > 0);
    nodeMetrics.m_numLeafNodeEntries++;
    for (uint32_t i = 1; i < node->m_numEntries; ++i)
    {
      if (node->m_triangleIndices[i] != node->m_triangleIndices[i - 1])
        nodeMetrics.m_numLeafNodeEntries++;
    }

    nodeMetrics.m_numLeafNodes++;

    // Update node metrics
    nodeMetrics.m_minNumEntries = NMP::minimum(nodeMetrics.m_minNumEntries, node->m_numEntries);
    nodeMetrics.m_maxNumEntries = NMP::maximum(nodeMetrics.m_maxNumEntries, node->m_numEntries);
    nodeMetrics.m_maxDepth = NMP::maximum(nodeMetrics.m_maxDepth, depth);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SB2DAnnotationBSPTree::doGetBSPTree(
  SB2DAnnotationBSPTreeExportData& exportData,
  BSPNodeMetrics& state,
  const BSPNode* node) const
{
  uint32_t nodeID;
  NMP_VERIFY(node);
  state.m_numNodes++;

  if (node->m_left)
  {
    // Branch nodes are stored before leaf nodes in the packed data
    nodeID = state.m_numBranchNodes;
    state.m_numBranchNodes++;

    // Update the BSP branch export data
    uint32_t* branchNodeLeft = exportData.getBranchNodeLeft();
    uint32_t* branchNodeRight = exportData.getBranchNodeRight();
    uint32_t* branchNodeHyperPlaneSampleIndicesA = exportData.getBranchNodeHyperPlaneSampleIndicesA();
    uint32_t* branchNodeHyperPlaneSampleIndicesB = exportData.getBranchNodeHyperPlaneSampleIndicesB();

    // Hyper-plane
    branchNodeHyperPlaneSampleIndicesA[nodeID] = node->m_hyperPlaneSampleIndices[0];
    branchNodeHyperPlaneSampleIndicesB[nodeID] = node->m_hyperPlaneSampleIndices[1];

    // Left
    branchNodeLeft[nodeID] = doGetBSPTree(
      exportData,
      state,
      node->m_left);

    // Right
    branchNodeRight[nodeID] = doGetBSPTree(
      exportData,
      state,
      node->m_right);
  }
  else
  {
    // Leaf nodes are stored after branch nodes in the packed data
    nodeID = exportData.getNumBranchNodes() + state.m_numLeafNodes;

    // Update the BSP leaf export data
    NMP_VERIFY(node->m_numEntries > 0);
    uint32_t* leafNodeEntryOffsets = exportData.getLeafNodeEntryOffsets();
    uint32_t* leafNodeTriangleIndices = exportData.getLeafNodeTriangleIndices();

    // First entry
    bool* leafNodeSubTriangleFlags = exportData.getLeafNodeSubTriangleFlags(state.m_numLeafNodeEntries);
    uint32_t triangleIndex = node->m_triangleIndices[0];
    NMP_VERIFY(triangleIndex < m_numTriangles);
    uint32_t subTriangleIndex = node->m_subTriangleIndices[0];
    NMP_VERIFY(subTriangleIndex < m_numSubSimplexesPerTriangle);
    leafNodeEntryOffsets[state.m_numLeafNodes] = state.m_numLeafNodeEntries;
    leafNodeTriangleIndices[state.m_numLeafNodeEntries] = triangleIndex;
    leafNodeSubTriangleFlags[subTriangleIndex] = true;
    state.m_numLeafNodeEntries++;

    // Remaining entries
    for (uint32_t i = 1; i < node->m_numEntries; ++i)
    {
      triangleIndex = node->m_triangleIndices[i];
      subTriangleIndex = node->m_subTriangleIndices[i];

      // Add a new leaf node entry if we have a different triangle
      if (triangleIndex != node->m_triangleIndices[i - 1])
      {        
        leafNodeSubTriangleFlags = exportData.getLeafNodeSubTriangleFlags(state.m_numLeafNodeEntries);
        leafNodeTriangleIndices[state.m_numLeafNodeEntries] = triangleIndex;
        state.m_numLeafNodeEntries++;
      }

      // Update the sub-triangle entry
      leafNodeSubTriangleFlags[subTriangleIndex] = true;
    }

    // +1 update for forward difference
    state.m_numLeafNodes++;
    leafNodeEntryOffsets[state.m_numLeafNodes] = state.m_numLeafNodeEntries;
  }

  return nodeID;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTree::BSPNode* SB2DAnnotationBSPTree::doSetBSPTree(
  const SB2DAnnotationBSPTreeExportData& exportData,
  BSPNodeMetrics& state,
  uint32_t nodeID)
{
  NMP_VERIFY(nodeID < exportData.getNumNodes());
  BSPNode* node = new BSPNode;
  ZeroMemory(node, sizeof(BSPNode));
  state.m_numNodes++;

  if (nodeID < exportData.getNumBranchNodes())
  {
    // Branch nodes are stored before leaf nodes in the packed data
    nodeID = state.m_numBranchNodes;
    state.m_numBranchNodes++;

    // Update the BSP branch export data
    const uint32_t* nodeLeft = exportData.getBranchNodeLeft();
    const uint32_t* nodeRight = exportData.getBranchNodeRight();
    const uint32_t* branchNodeHyperPlaneSampleIndicesA = exportData.getBranchNodeHyperPlaneSampleIndicesA();
    const uint32_t* branchNodeHyperPlaneSampleIndicesB = exportData.getBranchNodeHyperPlaneSampleIndicesB();

    // Hyper-plane
    node->m_hyperPlaneSampleIndices[0] = branchNodeHyperPlaneSampleIndicesA[nodeID];
    node->m_hyperPlaneSampleIndices[1] = branchNodeHyperPlaneSampleIndicesB[nodeID];

    // Left
    node->m_left = doSetBSPTree(
      exportData,
      state,
      nodeLeft[nodeID]);

    // Right
    node->m_right = doSetBSPTree(
      exportData,
      state,
      nodeRight[nodeID]);
  }
  else
  {
    // Leaf nodes are stored after branch nodes in the packed data
    nodeID -= exportData.getNumBranchNodes();

    // Update the BSP leaf export data
    uint32_t numSubSimplexesPerTriangle = exportData.getNumSubSimplexesPerTriangle();
    const uint32_t* leafNodeEntryOffsets = exportData.getLeafNodeEntryOffsets();
    const uint32_t* leafNodeTriangleIndices = exportData.getLeafNodeTriangleIndices();

    // Calculate the number of entries
    uint32_t nodeEntryOffsetsBegin = leafNodeEntryOffsets[nodeID];
    uint32_t nodeEntryOffsetsEnd = leafNodeEntryOffsets[nodeID + 1];
    uint32_t numEntries = 0;
    for (uint32_t nodeEntryOffset = nodeEntryOffsetsBegin; nodeEntryOffset < nodeEntryOffsetsEnd; ++nodeEntryOffset)
    {
      const bool* leafNodeSubTriangleFlags = exportData.getLeafNodeSubTriangleFlags(nodeEntryOffset);
      for (uint32_t subTriangleIndex = 0; subTriangleIndex < numSubSimplexesPerTriangle; ++subTriangleIndex)
      {
        if (leafNodeSubTriangleFlags[subTriangleIndex])
        {
          numEntries++;
        }
      }
    }
    NMP_VERIFY(numEntries > 0);
    node->m_numEntries = numEntries;
    node->m_triangleIndices = new uint32_t[node->m_numEntries];
    node->m_subTriangleIndices = new uint32_t[node->m_numEntries];

    // Set the entries
    numEntries = 0;
    for (uint32_t nodeEntryOffset = nodeEntryOffsetsBegin; nodeEntryOffset < nodeEntryOffsetsEnd; ++nodeEntryOffset)
    {
      uint32_t triangleIndex = leafNodeTriangleIndices[nodeEntryOffset];
      const bool* leafNodeSubTriangleFlags = exportData.getLeafNodeSubTriangleFlags(nodeEntryOffset);
      for (uint32_t subTriangleIndex = 0; subTriangleIndex < numSubSimplexesPerTriangle; ++subTriangleIndex)
      {
        if (leafNodeSubTriangleFlags[subTriangleIndex])
        {
          node->m_triangleIndices[numEntries] = triangleIndex;
          node->m_subTriangleIndices[numEntries] = subTriangleIndex;
          numEntries++;
        }
      }
    }
  }

  return node;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
