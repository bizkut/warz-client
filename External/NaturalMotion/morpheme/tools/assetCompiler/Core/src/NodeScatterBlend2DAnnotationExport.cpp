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
#include "NodeScatterBlend2DAnnotationExport.h"
#include "export/mcExport.h"
#include "assetProcessor/NodeBuilderUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangleSubSampleData
//----------------------------------------------------------------------------------------------------------------------
SB2DTriangleSubSampleData::SB2DTriangleSubSampleData() :
m_numTriangleSubDivisions(0),
m_numSubSamplesPerTriangle(0),
m_triangleSubSampleValidFlags(NULL)
{
  for (uint32_t i = 0; i < 3; ++i)
    m_triangleVertexIDs[i] = 0;

  for (uint32_t i = 0; i < 2; ++i)
    m_triangleSubSamples[i] = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DTriangleSubSampleData::~SB2DTriangleSubSampleData()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangleSubSampleData::init(uint32_t numTriangleSubDivisions)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(numTriangleSubDivisions);

  m_numTriangleSubDivisions = numTriangleSubDivisions;
  m_numSubSamplesPerTriangle = subDivisionDataBlock.getNumSubSamplesPerTriangle();

  // Triangle data
  m_triangleSubSamples[0] = new float[m_numSubSamplesPerTriangle];
  m_triangleSubSamples[1] = new float[m_numSubSamplesPerTriangle];
  m_triangleSubSampleValidFlags = new bool[m_numSubSamplesPerTriangle];

  // Clear the data
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangleSubSampleData::release()
{
  m_numTriangleSubDivisions = 0;
  m_numSubSamplesPerTriangle = 0;

  if (m_triangleSubSampleValidFlags)
  {
    delete[] m_triangleSubSampleValidFlags;
    m_triangleSubSampleValidFlags = NULL;
  }

  for (uint32_t i = 0; i < 2; ++i)
  {
    if (m_triangleSubSamples[i])
    {
      delete[] m_triangleSubSamples[i];
      m_triangleSubSamples[i] = NULL;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangleSubSampleData::readDataBlock(
  const ME::DataBlockExport* exportDataBlock,
  const MR::NodeID* sourceNodeLookup,
  uint32_t sourceNodeCount)
{
  NMP_VERIFY(exportDataBlock);


  MR::NodeID nodeIndexes[3];
  // Annotation vertex source indices
  readNodeID(exportDataBlock, "AnnotationLocationID0", nodeIndexes[0]);
  readNodeID(exportDataBlock, "AnnotationLocationID1", nodeIndexes[1]);
  readNodeID(exportDataBlock, "AnnotationLocationID2", nodeIndexes[2]);

  // find the vertex ids from the exported node ids
  for (uint32_t location = 0; location < 3; ++location)
  {
    for (uint16_t childIndex = 0; childIndex < sourceNodeCount; ++childIndex)
    {
      MR::NodeID childId = sourceNodeLookup[childIndex];

      if(nodeIndexes[location] == childId)
      {
        m_triangleVertexIDs[location] = childIndex;
      }
    }
  }

  bool status = false;

  // flags are currently not supported/passed anywhere in the scatter blend
  uint32_t flags;
  // Annotation vertex source indices
  status = exportDataBlock->readUInt(
    flags,
    "AnnotationLocationFlags0");
  NMP_VERIFY(status);
  NMP_VERIFY(flags == 0);

  status = exportDataBlock->readUInt(
    flags,
    "AnnotationLocationFlags1");
  NMP_VERIFY(status);
  NMP_VERIFY(flags == 0);

  status = exportDataBlock->readUInt(
    flags,
    "AnnotationLocationFlags2");
  NMP_VERIFY(status);
  NMP_VERIFY(flags == 0);

  // Check if the data is dirty. Note that the dirty data could contain
  // arrays that are a different size to the expected data
  bool dirtyFlag = true;
  status = exportDataBlock->readBool(
    dirtyFlag,
    "DirtyFlag");
  NMP_VERIFY(status);

  if (!dirtyFlag)
  {
    size_t length;

    // Verify the export data is the same format as the stored data
    uint32_t numSubDivisions = 0;
    status = exportDataBlock->readUInt(numSubDivisions, "NumSubDivisions");
    NMP_VERIFY(status && numSubDivisions < 3);
    NMP_VERIFY_MSG(
      numSubDivisions == m_numTriangleSubDivisions,
      "Triangle sub-sample export data is inconsistent with the sub-division level of the annotation data");

    bool statusX = exportDataBlock->readFloatArray(
      m_triangleSubSamples[0],
      m_numSubSamplesPerTriangle,
      length,
      "Property0");

    bool statusY = exportDataBlock->readFloatArray(
      m_triangleSubSamples[1],
      m_numSubSamplesPerTriangle,
      length,
      "Property1");

    setTriangleSubSampleValidFlags(statusX && statusY);
  }
  else
  {
    clear();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangleSubSampleData::writeDataBlock(ME::DataBlockExport* exportDataBlock)
{
  NMP_VERIFY(exportDataBlock);
  exportDataBlock->writeUInt(m_numTriangleSubDivisions, "NumSubDivisions");

  exportDataBlock->writeUInt(m_triangleVertexIDs[0], "AnnotationLocation0");
  exportDataBlock->writeUInt(m_triangleVertexIDs[1], "AnnotationLocation1");
  exportDataBlock->writeUInt(m_triangleVertexIDs[2], "AnnotationLocation2");

  exportDataBlock->writeFloatArray(m_triangleSubSamples[0], m_numSubSamplesPerTriangle, "Property0");
  exportDataBlock->writeFloatArray(m_triangleSubSamples[1], m_numSubSamplesPerTriangle, "Property1");
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DTriangleSubSampleData::permute(
  const uint32_t* desiredVertexOrderIDs,
  const SB2DTriangleSubSampleData& triangleSubSampleData)
{
  NMP_VERIFY(triangleSubSampleData.m_numTriangleSubDivisions == m_numTriangleSubDivisions);

  // Get the sub-sample permutation map for the desired vertex ordering.
  // i.e. the forward mapping of components in the source to the destination.
  NMP_VERIFY(desiredVertexOrderIDs);
  const uint32_t* srcVertexIDs = triangleSubSampleData.getTriangleVertexIDs();
  uint32_t permutationIndex = MR::ScatterBlend2DSubSampleManager::getTriangleVertexOrderPermutation(
    srcVertexIDs[0],
    srcVertexIDs[1],
    srcVertexIDs[2],
    desiredVertexOrderIDs[0],
    desiredVertexOrderIDs[1],
    desiredVertexOrderIDs[2]);

  // Permute the source data to obtain the required vertex ordering
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(triangleSubSampleData.getNumTriangleSubDivisions());
  const uint16_t* permutationMap = subDivisionDataBlock.getTriangleSubSamplePermutationMap(permutationIndex);

  // Vertex IDs
  m_triangleVertexIDs[0] = desiredVertexOrderIDs[0];
  m_triangleVertexIDs[1] = desiredVertexOrderIDs[1];
  m_triangleVertexIDs[2] = desiredVertexOrderIDs[2];

  // Triangle data
  for (uint32_t i = 0; i < m_numSubSamplesPerTriangle; ++i)
  {
    uint32_t index = permutationMap[i];
    NMP_VERIFY(index < m_numSubSamplesPerTriangle);
    m_triangleSubSamples[0][index] = triangleSubSampleData.m_triangleSubSamples[0][i];
    m_triangleSubSamples[1][index] = triangleSubSampleData.m_triangleSubSamples[1][i];
    m_triangleSubSampleValidFlags[index] = triangleSubSampleData.m_triangleSubSampleValidFlags[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SB2DTriangleSubSampleData::isVertexOrderingCollinear() const
{
  const float* samplesX = m_triangleSubSamples[0];
  const float* samplesY = m_triangleSubSamples[1];

  uint32_t indexA = m_triangleVertexIDs[0];
  uint32_t indexB = m_triangleVertexIDs[1];
  uint32_t indexC = m_triangleVertexIDs[2];

  // Relative vectors AB, AC
  float Ux = samplesX[indexB] - samplesX[indexA];
  float Uy = samplesY[indexB] - samplesY[indexA];
  float Vx = samplesX[indexC] - samplesX[indexA];
  float Vy = samplesY[indexC] - samplesY[indexA];

  // Check the Z component of the cross product
  float CPz = Ux * Vy - Vx * Uy;
  return fabs(CPz) < FLT_EPSILON;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationExportDataUnpacked
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationExportDataUnpacked::SB2DAnnotationExportDataUnpacked() :
  m_numTriangleSubDivisions(0),
  m_numSubSamplesPerTriangle(0),
  m_numVertices(0),
  m_numTriangles(0)
{
  for (uint32_t i = 0; i < 2; ++i)
    m_vertices[i] = NULL;

  m_vertexValidFlags = NULL;

  for (uint32_t i = 0; i < 2; ++i)
    m_triangleSubSamples[i] = NULL;

  for (uint32_t i = 0; i < 3; ++i)
    m_triangleVertexIDs[i] = NULL;

  m_triangleValidFlags = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationExportDataUnpacked::~SB2DAnnotationExportDataUnpacked()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataUnpacked::release()
{
  m_numTriangleSubDivisions = 0;
  m_numSubSamplesPerTriangle = 0;

  removeAllVertices();
  removeAllTriangles();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataUnpacked::init(
  uint32_t numTriangleSubDivisions,
  uint32_t numSubSamplesPerTriangle,
  uint32_t numVertices,
  uint32_t numTriangles)
{
  NMP_VERIFY(m_numSubSamplesPerTriangle == 0); // Must have been released
  NMP_VERIFY(numTriangleSubDivisions <= 2); // We don't support more sub-divisions

  // Information
  m_numTriangleSubDivisions = numTriangleSubDivisions;
  m_numSubSamplesPerTriangle = numSubSamplesPerTriangle;
  m_numVertices = numVertices;
  m_numTriangles = numTriangles;

  //---------------------------
  // Allocate the memory for the vertex data
  if (numVertices > 0)
  {
    for (uint32_t i = 0; i < 2; ++i)
    {
      m_vertices[i] = new float[numVertices];

      // Clear the sample data
      for (uint32_t k = 0; k < numVertices; ++k)
      {
        m_vertices[i][k] = 0.0f;
      }
    }

    m_vertexValidFlags = new bool[numVertices];

    // Clear the valid flag data
    for (uint32_t k = 0; k < numVertices; ++k)
      m_vertexValidFlags[k] = false;
  }

  //---------------------------
  // Allocate the memory for the triangle data
  if (numTriangles > 0)
  {
    NMP_VERIFY(m_numSubSamplesPerTriangle > 0);
    uint32_t numTriangleSubSamples = numSubSamplesPerTriangle * numTriangles;
    for (uint32_t i = 0; i < 2; ++i)
    {
      m_triangleSubSamples[i] = new float[numTriangleSubSamples];

      // Clear the sample data
      for (uint32_t k = 0; k < numTriangleSubSamples; ++k)
      {
        m_triangleSubSamples[i][k] = 0.0f;
      }
    }

    for (uint32_t i = 0; i < 3; ++i)
    {
      m_triangleVertexIDs[i] = new uint32_t[numTriangles];

      // Clear the data
      for (uint32_t k = 0; k < numTriangles; ++k)
      {
        m_triangleVertexIDs[i][k] = 0;
      }
    }

    m_triangleValidFlags = new bool[numTriangleSubSamples];

    // Clear the valid flag data
    for (uint32_t k = 0; k < numTriangleSubSamples; ++k)
      m_triangleValidFlags[k] = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataUnpacked::writeFile(const char* filename) const
{
  NMP_VERIFY(filename);
  FILE* fp = fopen(filename, "w");
  if (fp)
  {
    fprintf(fp, "m_numTriangleSubDivisions = %d\n", m_numTriangleSubDivisions);
    fprintf(fp, "m_numSubSamplesPerTriangle = %d\n", m_numSubSamplesPerTriangle);
    fprintf(fp, "m_numVertices = %d\n", m_numVertices);
    fprintf(fp, "m_numTriangles = %d\n", m_numTriangles);

    //---------------------------
    fprintf(fp, "\nVertices:\n");
    for (uint32_t i = 0; i < m_numVertices; ++i)
    {
      float x, y;
      getVertex(i, x, y);
      fprintf(fp, "%f %f\n", x, y);
    }

    fprintf(fp, "\nVertexValidFlags:\n");
    for (uint32_t i = 0; i < m_numVertices; ++i)
    {
      bool validFlag = getVertexValidFlag(i);
      fprintf(fp, "%d\n", validFlag);
    }

    //---------------------------
    fprintf(fp, "\nTriangleVertexIDs:\n");
    for (uint32_t triangleIndex = 0; triangleIndex < m_numTriangles; ++triangleIndex)
    {
      uint32_t vertexAID, vertexBID, vertexCID;
      getTriangleVertexIDs(
        triangleIndex,
        vertexAID,
        vertexBID,
        vertexCID);
      fprintf(fp, "%d %d %d\n", vertexAID, vertexBID, vertexCID);
    }

    fprintf(fp, "\nTriangleSubSamples:\n");
    for (uint32_t triangleIndex = 0; triangleIndex < m_numTriangles; ++triangleIndex)
    {
      fprintf(fp, "\nTriangle %d:\n", triangleIndex);
      for( uint32_t triangleSubSampleIndex = 0; triangleSubSampleIndex < m_numSubSamplesPerTriangle; ++triangleSubSampleIndex)
      {
        float x, y;
        getTriangleSubSample(
          triangleIndex,
          triangleSubSampleIndex,
          x, y);
        fprintf(fp, "%f %f\n", x, y);
      }
    }

    fprintf(fp, "\nTriangleValidFlags:\n");
    for (uint32_t triangleIndex = 0; triangleIndex < m_numTriangles; ++triangleIndex)
    {
      fprintf(fp, "\nTriangle %d:\n", triangleIndex);
      for( uint32_t triangleSubSampleIndex = 0; triangleSubSampleIndex < m_numSubSamplesPerTriangle; ++triangleSubSampleIndex)
      {
        bool validFlag = getTriangleSubSampleValidFlag(triangleIndex, triangleSubSampleIndex);
        fprintf(fp, "%d\n", validFlag);
      }
    }

    fclose(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataUnpacked::removeAllVertices()
{
  m_numVertices = 0;

  for (uint32_t i = 0; i < 2; ++i)
  {
    if (m_vertices[i])
    {
      delete[] m_vertices[i];
      m_vertices[i] = NULL;
    }
  }

  if (m_vertexValidFlags)
  {
    delete[] m_vertexValidFlags;
    m_vertexValidFlags = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataUnpacked::removeAllTriangles()
{
  m_numTriangles = 0;

  for (uint32_t i = 0; i < 2; ++i)
  {
    if (m_triangleSubSamples[i])
    {
      delete[] m_triangleSubSamples[i];
      m_triangleSubSamples[i] = NULL;
    }
  }

  for (uint32_t i = 0; i < 3; ++i)
  {
    if (m_triangleVertexIDs[i])
    {
      delete[] m_triangleVertexIDs[i];
      m_triangleVertexIDs[i] = NULL;
    }
  }

  if (m_triangleValidFlags)
  {
    delete[] m_triangleValidFlags;
    m_triangleValidFlags = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataUnpacked::getTriangleSubSampleData(
  uint32_t triangleIndex,
  SB2DTriangleSubSampleData& triangleSubSampleData) const
{
  NMP_VERIFY(triangleSubSampleData.getNumTriangleSubDivisions() == m_numTriangleSubDivisions);
  NMP_VERIFY(triangleIndex < m_numTriangles);

  uint32_t vertexAIndex, vertexBIndex, vertexCIndex;
  getTriangleVertexIDs(triangleIndex, vertexAIndex, vertexBIndex, vertexCIndex);
  triangleSubSampleData.setTriangleVertexIDs(vertexAIndex, vertexBIndex, vertexCIndex);

  for (uint32_t i = 0; i < m_numSubSamplesPerTriangle; ++i)
  {
    bool validFlag = getTriangleSubSampleValidFlag(triangleIndex, i);
    triangleSubSampleData.setTriangleSubSampleValidFlag(i, validFlag);

    float x, y;
    getTriangleSubSample(triangleIndex, i, x, y);
    triangleSubSampleData.setTriangleSubSample(i, x, y);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataUnpacked::setTriangleSubSampleData(
  uint32_t triangleIndex,
  const SB2DTriangleSubSampleData& triangleSubSampleData)
{
  NMP_VERIFY(triangleSubSampleData.getNumTriangleSubDivisions() == m_numTriangleSubDivisions);
  NMP_VERIFY(triangleIndex < m_numTriangles);

  uint32_t vertexAIndex, vertexBIndex, vertexCIndex;
  triangleSubSampleData.getTriangleVertexIDs(vertexAIndex, vertexBIndex, vertexCIndex);
  setTriangleVertexIDs(triangleIndex, vertexAIndex, vertexBIndex, vertexCIndex);

  for (uint32_t i = 0; i < m_numSubSamplesPerTriangle; ++i)
  {
    bool validFlag = triangleSubSampleData.getTriangleSubSampleValidFlag(i);
    setTriangleSubSampleValidFlag(triangleIndex, i, validFlag);

    float x, y;
    triangleSubSampleData.getTriangleSubSample(i, x, y);
    setTriangleSubSample(triangleIndex, i, x, y);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationExportDataPacked
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationExportDataPacked::SB2DAnnotationExportDataPacked() :
  m_numTriangleSubDivisions(0),
  m_numVertices(0),
  m_numTriangles(0),
  m_numSubSamples(0),
  m_numExteriorEdgeFacets(0),
  m_numSubSamplesPerTriangle(0),
  m_triangleSampleIndices(NULL),
  m_exteriorTriangleIndices(NULL),
  m_exteriorEdgeFacetIndices(NULL)
{
  for (uint32_t i = 0; i < 2; ++i)
    m_samples[i] = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationExportDataPacked::~SB2DAnnotationExportDataPacked()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataPacked::release()
{
  m_numTriangleSubDivisions = 0;
  m_numSubSamplesPerTriangle = 0;

  removeAllSamples();
  removeAllTriangles();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataPacked::init(
  uint32_t numTriangleSubDivisions,
  uint32_t numSubSamplesPerTriangle,
  uint32_t numVertices,
  uint32_t numTriangles,
  uint32_t numSubSamples,
  uint32_t numExteriorEdgeFacets)
{
  NMP_VERIFY(m_numSubSamplesPerTriangle == 0); // Must have been released
  NMP_VERIFY(numTriangleSubDivisions <= 2); // We don't support more sub-divisions

  // Information
  m_numTriangleSubDivisions = numTriangleSubDivisions;
  m_numSubSamplesPerTriangle = numSubSamplesPerTriangle;
  m_numVertices = numVertices;
  m_numTriangles = numTriangles;
  m_numSubSamples = numSubSamples;
  m_numExteriorEdgeFacets = numExteriorEdgeFacets;

  //---------------------------
  // Allocate the memory for the samples data
  uint32_t numTriangleSubSampleIndices = m_numSubSamplesPerTriangle * m_numTriangles;
  if (numTriangleSubSampleIndices > 0)
  {
    m_triangleSampleIndices = new uint32_t[numTriangleSubSampleIndices];
    
    // Invalidate the triangle sub-sample indices
    for (uint32_t i = 0; i < numTriangleSubSampleIndices; ++i)
      m_triangleSampleIndices[i] = 0xFFFFFFFF;
  }

  uint32_t numSamples = numVertices + numSubSamples;
  if (numSamples > 0)
  {
    for (uint32_t i = 0; i < 2; ++i)
    {
      m_samples[i] = new float[numSamples];
      
      // Clear the sample data
      for (uint32_t k = 0; k < numSamples; ++k)
      {
        m_samples[i][k] = 0.0f;
      }
    }
  }

  //---------------------------
  // Exterior triangle facets
  if (numExteriorEdgeFacets > 0)
  {
    m_exteriorTriangleIndices = new uint32_t[numExteriorEdgeFacets];
    m_exteriorEdgeFacetIndices = new uint32_t[numExteriorEdgeFacets];
    for (uint32_t i = 0; i < m_numExteriorEdgeFacets; ++i)
    {
      m_exteriorTriangleIndices[i] = 0;
      m_exteriorEdgeFacetIndices[i] = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataPacked::writeFile(const char* filename) const
{
  NMP_VERIFY(filename);
  FILE* fp = fopen(filename, "w");
  if (fp)
  {
    fprintf(fp, "m_numTriangleSubDivisions = %d\n", m_numTriangleSubDivisions);
    fprintf(fp, "m_numSubSamplesPerTriangle = %d\n", m_numSubSamplesPerTriangle);
    fprintf(fp, "m_numVertices = %d\n", m_numVertices);
    fprintf(fp, "m_numTriangles = %d\n", m_numTriangles);
    fprintf(fp, "m_numSubSamples = %d\n", m_numSubSamples);
    fprintf(fp, "m_numExteriorEdgeFacets = %d\n", m_numExteriorEdgeFacets);

    //---------------------------
    fprintf(fp, "\nSamples:\n");
    uint32_t numSamples = getNumSamples();
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      float x, y;
      getSample(i, x, y);
      fprintf(fp, "%f %f\n", x, y);
    }
    
    //---------------------------
    fprintf(fp, "\nTriangleSampleIndices:\n");
    for (uint32_t triangleIndex = 0; triangleIndex < m_numTriangles; ++triangleIndex)
    {
      for( uint32_t triangleSubSampleIndex = 0; triangleSubSampleIndex < m_numSubSamplesPerTriangle; ++triangleSubSampleIndex)
      {
        uint32_t sampleIndex;
        getTriangleSubSampleIndex(
          triangleIndex,
          triangleSubSampleIndex,
          sampleIndex);
        fprintf(fp, "%d ", sampleIndex);
      }
      fprintf(fp, "\n");
    }

    //---------------------------
    fprintf(fp, "\nExteriorEdgeFacets: (triangle id, facet id)\n");
    for (uint32_t i = 0; i < m_numExteriorEdgeFacets; ++i)
    {
      uint32_t exteriorTriangleIndex;
      uint32_t exteriorEdgeFacetIndex;
      getExteriorEdgeFacet(
        i,
        exteriorTriangleIndex,
        exteriorEdgeFacetIndex);
      fprintf(fp, "%d %d\n", exteriorTriangleIndex, exteriorEdgeFacetIndex);
    }

    fclose(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataPacked::removeAllSamples()
{
  m_numVertices = 0;
  
  for (uint32_t i = 0; i < 2; ++i)
  {
    if (m_samples[i])
    {
      delete[] m_samples[i];
      m_samples[i] = NULL;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationExportDataPacked::removeAllTriangles()
{
  m_numTriangles = 0;
  m_numSubSamples = 0;
  m_numExteriorEdgeFacets = 0;

  if (m_triangleSampleIndices)
  {
    delete[] m_triangleSampleIndices;
    m_triangleSampleIndices = NULL;
  }

  if (m_exteriorTriangleIndices)
  {
    delete[] m_exteriorTriangleIndices;
    m_exteriorTriangleIndices = NULL;
  }

  if (m_exteriorEdgeFacetIndices)
  {
    delete[] m_exteriorEdgeFacetIndices;
    m_exteriorEdgeFacetIndices = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationSubSampleRangeData
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationSubSampleRangeData::SB2DAnnotationSubSampleRangeData()
{
  for (uint32_t i = 0; i < 2; ++i)
  {
    m_samplesRangeMin[i] = 0;
    m_samplesRangeMax[i] = 0;
    m_samplesCentre[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationSubSampleRangeData::~SB2DAnnotationSubSampleRangeData()
{
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeExportData
//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeExportData::SB2DAnnotationBSPTreeExportData() :
  m_validFlag(false),
  m_bspNumBranchNodes(0),
  m_bspNumLeafNodes(0),
  m_bspNumLeafNodeEntries(0),
  m_bspNumSubSimplexesPerTriangle(0),
  m_bspBranchNodeLeft(NULL),
  m_bspBranchNodeRight(NULL),
  m_bspLeafNodeEntryOffsets(NULL),
  m_bspLeafNodeTriangleIndices(NULL),
  m_bspLeafNodeSubTriangleFlags(NULL)
{
  m_bspBranchNodeHyperPlaneSampleIndices[0] = NULL;
  m_bspBranchNodeHyperPlaneSampleIndices[1] = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
SB2DAnnotationBSPTreeExportData::~SB2DAnnotationBSPTreeExportData()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeExportData::init(
  uint32_t numBranchNodes,
  uint32_t numLeafNodes,
  uint32_t numLeafNodeEntries,
  uint32_t numSubSimplexesPerTriangle)
{
  NMP_VERIFY(!m_bspBranchNodeLeft); // Must be released

  m_bspNumBranchNodes = numBranchNodes;
  m_bspNumLeafNodes = numLeafNodes;
  m_bspNumLeafNodeEntries = numLeafNodeEntries;
  m_bspNumSubSimplexesPerTriangle = numSubSimplexesPerTriangle;

  if (numLeafNodes == 0)
    return;

  //---------------------------
  NMP_VERIFY(numSubSimplexesPerTriangle > 0);
  NMP_VERIFY(numLeafNodes > 0);
  NMP_VERIFY(numLeafNodeEntries > 0);
  uint32_t numSubTriangleFlags = numLeafNodeEntries * numSubSimplexesPerTriangle;

  if (numBranchNodes > 0)
  {
    m_bspBranchNodeLeft = new uint32_t[numBranchNodes];
    m_bspBranchNodeRight = new uint32_t[numBranchNodes];
    m_bspBranchNodeHyperPlaneSampleIndices[0] = new uint32_t[numBranchNodes];
    m_bspBranchNodeHyperPlaneSampleIndices[1] = new uint32_t[numBranchNodes];
  }

  m_bspLeafNodeEntryOffsets = new uint32_t[numLeafNodes + 1];
  m_bspLeafNodeTriangleIndices = new uint32_t[numLeafNodeEntries];
  m_bspLeafNodeSubTriangleFlags = new bool[numSubTriangleFlags];

  //---------------------------
  // Clear the data
  for (uint32_t i = 0; i < numBranchNodes; ++i)
  {
    m_bspBranchNodeLeft[i] = 0;
    m_bspBranchNodeRight[i] = 0;
    m_bspBranchNodeHyperPlaneSampleIndices[0][i] = 0;
    m_bspBranchNodeHyperPlaneSampleIndices[1][i] = 0;
  }

  for (uint32_t i = 0; i <= numLeafNodes; ++i)
    m_bspLeafNodeEntryOffsets[i] = 0;

  for (uint32_t i = 0; i < numLeafNodeEntries; ++i)
    m_bspLeafNodeTriangleIndices[i] = 0;

  for (uint32_t i = 0; i < numSubTriangleFlags; ++i)
    m_bspLeafNodeSubTriangleFlags[i] = false;
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeExportData::init(const ME::DataBlockExport* dataBlockToRead)
{
  uint32_t numBranches = 0;
  uint32_t numLeafNodes = 0;
  uint32_t numLeafNodeEntries = 0;
  uint32_t numSubSimplexPerTriangle = 0;

  dataBlockToRead->readUInt(numBranches, "NumBranchNodes");
  dataBlockToRead->readUInt(numLeafNodes, "NumLeafNodes");
  dataBlockToRead->readUInt(numLeafNodeEntries, "NumLeafNodeEntries");
  dataBlockToRead->readUInt(numSubSimplexPerTriangle, "NumSubSimplexesPerTriangle");
  NMP_VERIFY_MSG(
    numLeafNodes > 0,
    "The BSP tree has no leaf nodes");
  NMP_VERIFY_MSG(
    numSubSimplexPerTriangle > 0,
    "The BSP tree has an invalid number of sub-divisions");

  size_t actualLength = 0;

  // defer to the main init function
  init(numBranches, numLeafNodes, numLeafNodeEntries, numSubSimplexPerTriangle);

  if(m_bspNumBranchNodes > 0)
  {
    dataBlockToRead->readUIntArray(m_bspBranchNodeLeft, m_bspNumBranchNodes, actualLength, "BranchNodeLeft");
    NMP_VERIFY(actualLength == m_bspNumBranchNodes);
    dataBlockToRead->readUIntArray(m_bspBranchNodeRight, m_bspNumBranchNodes, actualLength, "BranchNodeRight");
    NMP_VERIFY(actualLength == m_bspNumBranchNodes);

    dataBlockToRead->readUIntArray(m_bspBranchNodeHyperPlaneSampleIndices[0], m_bspNumBranchNodes, actualLength, "BranchNodeHyperPlaneSampleIndices0");
    NMP_VERIFY(actualLength == m_bspNumBranchNodes);
    dataBlockToRead->readUIntArray(m_bspBranchNodeHyperPlaneSampleIndices[1], m_bspNumBranchNodes, actualLength, "BranchNodeHyperPlaneSampleIndices1");
    NMP_VERIFY(actualLength == m_bspNumBranchNodes);
  }

  if(m_bspNumLeafNodes > 0)
  {
    // NOTE: An extra element is added to this array so that the entry counts can be recovered by forward difference.
    dataBlockToRead->readUIntArray(m_bspLeafNodeEntryOffsets, m_bspNumLeafNodes + 1, actualLength, "LeafNodeEntryOffsets");
    NMP_VERIFY(actualLength == m_bspNumLeafNodes + 1);
  }

  if(m_bspNumLeafNodeEntries > 0)
  {
    dataBlockToRead->readUIntArray(m_bspLeafNodeTriangleIndices, m_bspNumLeafNodeEntries, actualLength, "LeafNodeTriangleIndices");
    NMP_VERIFY(actualLength == m_bspNumLeafNodeEntries);
  }

  if(m_bspNumSubSimplexesPerTriangle > 0)
  {
    char nameBuffer[256];

    // Leaf node sub-triangle flags
    for (uint32_t i = 0; i < m_bspNumLeafNodeEntries; ++i)
    {
      sprintf_s(nameBuffer, 255, "LeafNodeSubTriangleFlags_%d", i);

      bool* subTriangleFlags = getLeafNodeSubTriangleFlags(i);
      dataBlockToRead->readBoolArray(subTriangleFlags, m_bspNumSubSimplexesPerTriangle, actualLength, nameBuffer);
      NMP_VERIFY(actualLength == m_bspNumSubSimplexesPerTriangle);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeExportData::release()
{
  m_validFlag = false;
  m_bspNumBranchNodes = 0;
  m_bspNumLeafNodes = 0;
  m_bspNumLeafNodeEntries = 0;
  m_bspNumSubSimplexesPerTriangle = 0;

  if (m_bspBranchNodeLeft)
  {
    delete[] m_bspBranchNodeLeft;
    m_bspBranchNodeLeft = NULL;
  }

  if (m_bspBranchNodeRight)
  {
    delete[] m_bspBranchNodeRight;
    m_bspBranchNodeRight = NULL;
  }

  for (uint32_t i = 0; i < 2; ++i)
  {
    if (m_bspBranchNodeHyperPlaneSampleIndices[i])
    {
      delete[] m_bspBranchNodeHyperPlaneSampleIndices[i];
      m_bspBranchNodeHyperPlaneSampleIndices[i] = NULL;
    }
  }

  if (m_bspLeafNodeEntryOffsets)
  {
    delete[] m_bspLeafNodeEntryOffsets;
    m_bspLeafNodeEntryOffsets = NULL;
  }

  if (m_bspLeafNodeTriangleIndices)
  {
    delete[] m_bspLeafNodeTriangleIndices;
    m_bspLeafNodeTriangleIndices = NULL;
  }

  if (m_bspLeafNodeSubTriangleFlags)
  {
    delete[] m_bspLeafNodeSubTriangleFlags;
    m_bspLeafNodeSubTriangleFlags = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeExportData::writeDataBlock(ME::DataBlockExport* dataBlockToWrite)
{
  NMP_VERIFY(dataBlockToWrite);

  // Information
  dataBlockToWrite->writeUInt(m_bspNumBranchNodes, "NumBranchNodes");
  dataBlockToWrite->writeUInt(m_bspNumLeafNodes, "NumLeafNodes");
  dataBlockToWrite->writeUInt(m_bspNumLeafNodeEntries, "NumLeafNodeEntries");
  dataBlockToWrite->writeUInt(m_bspNumSubSimplexesPerTriangle, "NumSubSimplexesPerTriangle");

  // Left
  dataBlockToWrite->writeUIntArray(m_bspBranchNodeLeft, m_bspNumBranchNodes, "BranchNodeLeft");

  // Right
  dataBlockToWrite->writeUIntArray(m_bspBranchNodeRight, m_bspNumBranchNodes, "BranchNodeRight");

  // Hyper planes
  dataBlockToWrite->writeUIntArray(m_bspBranchNodeHyperPlaneSampleIndices[0], m_bspNumBranchNodes, "BranchNodeHyperPlaneSampleIndices0");
  dataBlockToWrite->writeUIntArray(m_bspBranchNodeHyperPlaneSampleIndices[1], m_bspNumBranchNodes, "BranchNodeHyperPlaneSampleIndices1");

  // Leaf node entry offsets. NOTE: An extra element is added to this array so that the entry
  // counts can be recovered by forward difference.
  dataBlockToWrite->writeUIntArray(m_bspLeafNodeEntryOffsets, m_bspNumLeafNodes + 1, "LeafNodeEntryOffsets");

  // Leaf node triangle indices
  dataBlockToWrite->writeUIntArray(m_bspLeafNodeTriangleIndices, m_bspNumLeafNodeEntries, "LeafNodeTriangleIndices");

  char nameBuffer[256];

  // Leaf node sub-triangle flags
  for (uint32_t i = 0; i < m_bspNumLeafNodeEntries; ++i)
  {
    sprintf_s(nameBuffer, 255, "LeafNodeSubTriangleFlags_%d", i);

    const bool* subTriangleFlags = getLeafNodeSubTriangleFlags(i);
    dataBlockToWrite->writeBoolArray(subTriangleFlags, m_bspNumSubSimplexesPerTriangle, nameBuffer);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB2DAnnotationBSPTreeExportData::writeFile(const char* filename) const
{
  NMP_VERIFY(filename);
  FILE* fp = fopen(filename, "w");
  if (fp)
  {
    // Information
    fprintf(fp, "m_bspNumBranchNodes = %d\n", m_bspNumBranchNodes);
    fprintf(fp, "m_bspNumLeafNodes = %d\n", m_bspNumLeafNodes);
    fprintf(fp, "m_bspNumLeafNodeEntries = %d\n", m_bspNumLeafNodeEntries);
    fprintf(fp, "m_bspNumSubSimplexesPerTriangle = %d\n", m_bspNumSubSimplexesPerTriangle);

    // Left
    fprintf(fp, "\nm_bspBranchNodeLeft\n");
    for (uint32_t i = 0; i < m_bspNumBranchNodes; ++i)
    {
      fprintf(fp, "%d ", m_bspBranchNodeLeft[i]);
    }
    fprintf(fp, "\n");

    // Right
    fprintf(fp, "\nm_bspBranchNodeRight\n");
    for (uint32_t i = 0; i < m_bspNumBranchNodes; ++i)
    {
      fprintf(fp, "%d ", m_bspBranchNodeRight[i]);
    }
    fprintf(fp, "\n");

    // Hyper plane
    fprintf(fp, "\nm_bspBranchNodeHyperPlaneSampleIndices\n");
    for (uint32_t i = 0; i < m_bspNumBranchNodes; ++i)
    {
      fprintf(fp, "%d ", m_bspBranchNodeHyperPlaneSampleIndices[0][i]);
    }
    fprintf(fp, "\n");
    for (uint32_t i = 0; i < m_bspNumBranchNodes; ++i)
    {
      fprintf(fp, "%d ", m_bspBranchNodeHyperPlaneSampleIndices[1][i]);
    }
    fprintf(fp, "\n");

    // Leaf node entry offsets
    fprintf(fp, "\nm_bspLeafNodeEntryOffsets\n");
    for (uint32_t i = 0; i <= m_bspNumLeafNodes; ++i)
    {
      fprintf(fp, "%d ", m_bspLeafNodeEntryOffsets[i]);
    }
    fprintf(fp, "\n");

    // Leaf node triangle indices
    fprintf(fp, "\nm_bspLeafNodeTriangleIndices\n");
    for (uint32_t i = 0; i < m_bspNumLeafNodeEntries; ++i)
    {
      fprintf(fp, "%d ", m_bspLeafNodeTriangleIndices[i]);
    }
    fprintf(fp, "\n");

    // Leaf node sub-triangle flags
    fprintf(fp, "\nm_bspLeafNodeSubTriangleFlags\n");
    for (uint32_t i = 0; i < m_bspNumLeafNodeEntries; ++i)
    {
      const bool* subTriangleFlags = getLeafNodeSubTriangleFlags(i);
      for (uint32_t k = 0; k < m_bspNumSubSimplexesPerTriangle; ++k)
      {
        fprintf(fp, "%d ", (uint32_t)subTriangleFlags[k]);
      }
      fprintf(fp, "\n");
    }

    fclose(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SB2DAnnotationBSPTreeExportData::computeMaxNumSubTrianglesInPartition() const
{
  uint32_t maxTriangleSearchSize = 0;
  for (uint32_t entryIndex = 0; entryIndex < m_bspNumLeafNodes; ++entryIndex)
  {
    // Compute the search size for this partition
    uint32_t triangleSearchSize = 0 ;
    uint32_t nodeEntryOffsetsBegin = m_bspLeafNodeEntryOffsets[entryIndex];
    uint32_t nodeEntryOffsetsEnd = m_bspLeafNodeEntryOffsets[entryIndex + 1];
    for (uint32_t nodeEntryOffset = nodeEntryOffsetsBegin; nodeEntryOffset < nodeEntryOffsetsEnd; ++nodeEntryOffset)
    {
      const bool* subTriangleFlags = getLeafNodeSubTriangleFlags(nodeEntryOffset);
      for (uint32_t i = 0; i < m_bspNumSubSimplexesPerTriangle; ++i)
      {
        if (subTriangleFlags[i])
          triangleSearchSize++;
      }
    }

    // Update the max search size
    maxTriangleSearchSize = NMP::maximum(maxTriangleSearchSize, triangleSearchSize);
  }

  return maxTriangleSearchSize;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
