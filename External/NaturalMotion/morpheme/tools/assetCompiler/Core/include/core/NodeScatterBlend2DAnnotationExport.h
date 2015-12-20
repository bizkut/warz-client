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
#ifndef MR_NODE_SCATTER_BLEND_2D_ANNOTATION_EXPORT_H
#define MR_NODE_SCATTER_BLEND_2D_ANNOTATION_EXPORT_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "morpheme/Nodes/mrScatterBlend2DUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ME
{
class DataBlockExport;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangleSubSampleData
//----------------------------------------------------------------------------------------------------------------------
class SB2DTriangleSubSampleData
{
public:
  SB2DTriangleSubSampleData();
  ~SB2DTriangleSubSampleData();

  void init(uint32_t numTriangleSubDivisions);

  void release();

  void readDataBlock(const ME::DataBlockExport* exportDataBlock, const MR::NodeID* sourceNodeLookup, uint32_t sourceNodeCount);
  void writeDataBlock(ME::DataBlockExport* exportDataBlock);

  /// \brief Permutes the sub-sample data stored in triangleSubSampleData to
  /// have the desired vertex order
  void permute(
    const uint32_t* desiredVertexOrderIDs,
    const SB2DTriangleSubSampleData& triangleSubSampleData);

  bool isVertexOrderingCollinear() const;

  NM_INLINE void clear();

  NM_INLINE uint32_t getNumTriangleSubDivisions() const;
  NM_INLINE uint32_t getNumSubSamplesPerTriangle() const;

  /// \brief Get the triangle vertex IDs
  NM_INLINE const uint32_t* getTriangleVertexIDs() const;

  NM_INLINE void setTriangleVertexIDs(
    const uint32_t* triangleVertexIDs);

  NM_INLINE void getTriangleVertexIDs(
    uint32_t& vertexAID,
    uint32_t& vertexBID,
    uint32_t& vertexCID) const;

  /// \brief Set the triangle vertex IDs
  NM_INLINE void setTriangleVertexIDs(
    uint32_t vertexAID,
    uint32_t vertexBID,
    uint32_t vertexCID);

  /// \brief Get the triangle sub-sample at the index position
  NM_INLINE void getTriangleSubSample(
    uint32_t triangleSubSampleIndex,
    float& x,
    float& y) const;

  /// \brief Set the triangle sub-sample at the index position
  NM_INLINE void setTriangleSubSample(
    uint32_t triangleSubSampleIndex,
    float x,
    float y);

  NM_INLINE bool getTriangleSubSampleValidFlag(
    uint32_t triangleSubSampleIndex) const;

  NM_INLINE void setTriangleSubSampleValidFlag(
    uint32_t triangleSubSampleIndex,
    bool validFlag);

  NM_INLINE void setTriangleSubSampleValidFlags(bool validFlag);

  NM_INLINE void toggleTriangleSubSampleValidFlags();

  NM_INLINE float* getTriangleSubSamplesX();
  NM_INLINE float* getTriangleSubSamplesY();

private:
  uint32_t                      m_numTriangleSubDivisions;        ///< The number of triangle sub-divisions
  uint32_t                      m_numSubSamplesPerTriangle;       ///< The size of the sub-sample data arrays

  // Triangle data
  uint32_t                      m_triangleVertexIDs[3];           ///< Vertex source location indices
  float*                        m_triangleSubSamples[2];          ///< Sub-sample data
  bool*                         m_triangleSubSampleValidFlags;    ///< Sub-sample data valid flags
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationExportDataUnpacked
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotationExportDataUnpacked
{
public:
  SB2DAnnotationExportDataUnpacked();
  ~SB2DAnnotationExportDataUnpacked();

  void release();

  /// \brief Initialise the export data
  void init(
    uint32_t numTriangleSubDivisions,
    uint32_t numSubSamplesPerTriangle,
    uint32_t numVertices,
    uint32_t numTriangles);

  void writeFile(const char* filename) const;

  /// \brief Removes all the vertices from the export data
  void removeAllVertices();

  /// \brief Removes all the triangles from the export data
  void removeAllTriangles();

  void getTriangleSubSampleData(
    uint32_t triangleIndex,
    SB2DTriangleSubSampleData& triangleSubSampleData) const;

  void setTriangleSubSampleData(
    uint32_t triangleIndex,
    const SB2DTriangleSubSampleData& triangleSubSampleData);

  /// \brief Return the number of triangle sub-divisions.
  NM_INLINE uint32_t getNumTriangleSubDivisions() const;

  /// \brief Return the number of sub-samples per triangle.
  NM_INLINE uint32_t getNumSubSamplesPerTriangle() const;

  /// \brief Return the number of vertices in the annotation.
  NM_INLINE uint32_t getNumVertices() const;

  /// \brief Return the number of triangles in the annotation
  NM_INLINE uint32_t getNumTriangles() const;

  /// \brief Get the vertex at the index position.
  NM_INLINE void getVertex(
    uint32_t vertexIndex,
    float& x,
    float& y) const;

  /// \brief Set the vertex at the index position.
  NM_INLINE void setVertex(
    uint32_t vertexIndex,
    float x,
    float y);

  /// \brief Get the vertex valid flag at the index position
  NM_INLINE bool getVertexValidFlag(
    uint32_t vertexIndex) const;

  /// \brief Set the vertex valid flag at the index position
  NM_INLINE void setVertexValidFlag(
    uint32_t vertexIndex,
    bool validFlag);

  /// \brief Get the triangle vertex IDs
  NM_INLINE void getTriangleVertexIDs(
    uint32_t triangleIndex,
    uint32_t& vertexAID,
    uint32_t& vertexBID,
    uint32_t& vertexCID) const;

  /// \brief Set the triangle vertex IDs
  NM_INLINE void setTriangleVertexIDs(
    uint32_t triangleIndex,
    uint32_t vertexAID,
    uint32_t vertexBID,
    uint32_t vertexCID);

  /// \brief Get the triangle sub-sample at the index position
  NM_INLINE void getTriangleSubSample(
    uint32_t triangleIndex,
    uint32_t triangleSubSampleIndex,
    float& x,
    float& y) const;

  /// \brief Set the triangle sub-sample at the index position
  NM_INLINE void setTriangleSubSample(
    uint32_t triangleIndex,
    uint32_t triangleSubSampleIndex,
    float x,
    float y);

  /// \brief Get the triangle valid flag at the index position
  NM_INLINE bool getTriangleSubSampleValidFlag(
    uint32_t triangleIndex,
    uint32_t triangleSubSampleIndex) const;

  /// \brief Set the triangle valid flag at the index position
  NM_INLINE void setTriangleSubSampleValidFlag(
    uint32_t triangleIndex,
    uint32_t triangleSubSampleIndex,
    bool validFlag);

  /// \brief get access to the samples data arrays
  NM_INLINE const float* getVerticesX() const;
  NM_INLINE const float* getVerticesY() const;
  NM_INLINE float* getVerticesX();
  NM_INLINE float* getVerticesY();

  NM_INLINE const bool* getVertexValidFlags() const;
  NM_INLINE bool* getVertexValidFlags();

  /// \brief get access to the samples data arrays
  NM_INLINE const float* getTriangleSubSamplesX() const;
  NM_INLINE const float* getTriangleSubSamplesY() const;
  NM_INLINE float* getTriangleSubSamplesX();
  NM_INLINE float* getTriangleSubSamplesY();

  NM_INLINE const uint32_t* getTriangleVertexAIDs() const;
  NM_INLINE const uint32_t* getTriangleVertexBIDs() const;
  NM_INLINE const uint32_t* getTriangleVertexCIDs() const;
  NM_INLINE uint32_t* getTriangleVertexAIDs();
  NM_INLINE uint32_t* getTriangleVertexBIDs();
  NM_INLINE uint32_t* getTriangleVertexCIDs();

  NM_INLINE const bool* getTriangleValidFlags() const;
  NM_INLINE bool* getTriangleValidFlags(); 

private:
  // Information
  uint32_t      m_numTriangleSubDivisions;     ///< The number of triangle sub-divisions used for sub-sampling.
  uint32_t      m_numSubSamplesPerTriangle;    ///< The number of sub-samples per triangle.
  uint32_t      m_numVertices;                 ///< The number of annotated vertices
  uint32_t      m_numTriangles;                ///< The number of annotated triangles

  // Vertex data
  float*        m_vertices[2];
  bool*         m_vertexValidFlags;

  // Triangle data
  uint32_t*     m_triangleVertexIDs[3];
  float*        m_triangleSubSamples[2];
  bool*         m_triangleValidFlags;
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationExportDataPacked
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotationExportDataPacked
{
public:
  SB2DAnnotationExportDataPacked();
  ~SB2DAnnotationExportDataPacked();

  void release();

  /// \brief Initialise the export data
  void init(
    uint32_t numTriangleSubDivisions,
    uint32_t numSubSamplesPerTriangle,
    uint32_t numVertices,
    uint32_t numTriangles,
    uint32_t numSubSamples,
    uint32_t numExteriorEdgeFacets);
    
  void writeFile(const char* filename) const;

  /// \brief Removes all the samples from the export data
  void removeAllSamples();

  /// \brief Removes all the triangles from the export data
  void removeAllTriangles();

  /// \brief Return the number of triangle sub-divisions.
  NM_INLINE uint32_t getNumTriangleSubDivisions() const;

  /// \brief Return the number of sub-samples per triangle.
  NM_INLINE uint32_t getNumSubSamplesPerTriangle() const;

  /// \brief Return the number of vertices in the annotation.
  NM_INLINE uint32_t getNumVertices() const;

  /// \brief Return the number of triangles in the annotation
  NM_INLINE uint32_t getNumTriangles() const;

  /// \brief Return the number of unique sub-samples within the annotation.
  /// i.e. The full set of samples that sub-divide the triangles, less the
  /// annotation vertices and the duplicate edges & triangle facets.
  NM_INLINE uint32_t getNumSubSamples() const;

  /// \brief Return the number of unique samples within the annotation.
  /// i.e. The full set of samples that sub-divide the triangles, less any
  /// duplicate edge & triangle facet sub-samples.
  NM_INLINE uint32_t getNumSamples() const;

  /// \brief Get the sample at the index position. Note that the annotation
  /// vertices are stored in the first m_numVertices positions.
  NM_INLINE void getSample(
    uint32_t index,
    float& x,
    float& y) const;

  /// \brief Set the sample at the index position. Note that the annotation
  /// vertices are stored in the first m_numVertices positions.
  NM_INLINE void setSample(
    uint32_t index,
    float x,
    float y);

  /// \brief get access to the samples data arrays
  NM_INLINE const float* getSamplesX() const;
  NM_INLINE const float* getSamplesY() const;
  NM_INLINE float* getSamplesX();
  NM_INLINE float* getSamplesY();

  /// \brief Triangle sub-sample indices
  NM_INLINE uint32_t getNumTriangleSampleIndices() const;
  NM_INLINE const uint32_t* getTriangleSampleIndices() const;
  NM_INLINE uint32_t* getTriangleSampleIndices();

  NM_INLINE void getTriangleSubSampleIndex(
    uint32_t triangleIndex,
    uint32_t triangleSubSampleIndex,
    uint32_t& value) const;

  NM_INLINE void setTriangleSubSampleIndex(
    uint32_t triangleIndex,
    uint32_t triangleSubSampleIndex,
    uint32_t value);

  /// \brief Recover the sample indices corresponding to the major vertices of the triangle
  NM_INLINE void getTriangleSubSampleIndices(
    uint32_t triangleIndex,
    uint32_t& sampleIndexA,
    uint32_t& sampleIndexB,
    uint32_t& sampleIndexC) const;

  NM_INLINE bool isValidTriangleSampleIndices() const;

  /// \brief Exterior triangle facets
  NM_INLINE uint32_t getNumExteriorEdgeFacets() const;
  NM_INLINE const uint32_t* getExteriorTriangleIndices() const;
  NM_INLINE const uint32_t* getExteriorEdgeFacetIndices() const;
  NM_INLINE uint32_t* getExteriorTriangleIndices();
  NM_INLINE uint32_t* getExteriorEdgeFacetIndices();

  NM_INLINE void getExteriorEdgeFacet(
    uint32_t entryIndex,
    uint32_t& exteriorTriangleIndex,
    uint32_t& exteriorEdgeFacetIndex) const;

  NM_INLINE void setExteriorEdgeFacet(
    uint32_t entryIndex,
    uint32_t exteriorTriangleIndex,
    uint32_t exteriorEdgeFacetIndex);

  /// \brief Annotation valid flags
  NM_INLINE bool getAnnotationValidFlags() const;

  NM_INLINE void setAnnotationValidFlags(
    bool validFlags);

  // Sub-division triangles
  NM_INLINE uint32_t getNumSubSimplexesPerTriangle() const;

  NM_INLINE void getSubTriangleSubSampleIndices(
    uint32_t triangleIndex,
    uint32_t subDivTriangleIndex,
    uint32_t& sampleIndexA,
    uint32_t& sampleIndexB,
    uint32_t& sampleIndexC) const;

private:
  // Information
  uint32_t      m_numTriangleSubDivisions;     ///< The number of triangle sub-divisions used for sub-sampling.
  uint32_t      m_numSubSamplesPerTriangle;    ///< The number of sub-samples per triangle.
  uint32_t      m_numVertices;                    ///< The number of annotated vertices
  uint32_t      m_numTriangles;                ///< The number of annotated triangles
  uint32_t      m_numSubSamples;                  ///< The number of unique sub-division samples excluding the annotation vertices.  
  uint32_t      m_numExteriorEdgeFacets;      ///< The number of exterior triangle facets for the annotation.
  bool          m_annotationValidFlags;           ///< Flag determining if the annotation data is valid

  // Sample data
  uint32_t*     m_triangleSampleIndices;
  float*        m_samples[2];

  // Exterior triangle facets: (triangle id, facet id) pairs
  uint32_t*     m_exteriorTriangleIndices;     ///< Triangle ids
  uint32_t*     m_exteriorEdgeFacetIndices;   ///< Triangle facet ids. Index (0-3) into m_triangleFacetIndices.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::SB2DAnnotationSubSampleRangeData
/// \brief This structure stores the range information for the annotation sub-samples
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotationSubSampleRangeData
{
public:
  SB2DAnnotationSubSampleRangeData();
  ~SB2DAnnotationSubSampleRangeData();

public:
  float m_samplesRangeMin[2];
  float m_samplesRangeMax[2];
  float m_samplesCentre[2];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::SB2DAnnotationBSPTreeExportData
/// \brief This structure is used to store the data required for binary
/// space partitioning the annotation data in a flattened memory space.
/// This structure is used for converting the binary tree representation
/// to a flat memory representation for export to connect.
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotationBSPTreeExportData
{
public:
  SB2DAnnotationBSPTreeExportData();
  ~SB2DAnnotationBSPTreeExportData();

  void init(
    uint32_t numBranchNodes,
    uint32_t numLeafNodes,
    uint32_t numLeafNodeEntries,
    uint32_t numSubSimplexesPerTriangle);

  void init(const ME::DataBlockExport* dataBlockToRead);

  void release();

  void writeDataBlock(ME::DataBlockExport* dataBlockToWrite);

  void writeFile(const char* filename) const;

  uint32_t computeMaxNumSubTrianglesInPartition() const;

  NM_INLINE bool getValidFlag() const;
  NM_INLINE void setValidFlag(bool flag);

  NM_INLINE uint32_t getNumSubSimplexesPerTriangle() const;

  NM_INLINE uint32_t getNumBranchNodes() const;
  NM_INLINE uint32_t getNumLeafNodes() const;
  NM_INLINE uint32_t getNumNodes() const;

  NM_INLINE const uint32_t* getBranchNodeLeft() const;
  NM_INLINE uint32_t* getBranchNodeLeft();

  NM_INLINE const uint32_t* getBranchNodeRight() const;
  NM_INLINE uint32_t* getBranchNodeRight();

  NM_INLINE const uint32_t* getBranchNodeHyperPlaneSampleIndicesA() const;
  NM_INLINE uint32_t* getBranchNodeHyperPlaneSampleIndicesA();

  NM_INLINE const uint32_t* getBranchNodeHyperPlaneSampleIndicesB() const;
  NM_INLINE uint32_t* getBranchNodeHyperPlaneSampleIndicesB();

  NM_INLINE const uint32_t* getLeafNodeEntryOffsets() const;
  NM_INLINE uint32_t* getLeafNodeEntryOffsets();

  NM_INLINE uint32_t getNumLeafNodeEntries() const;
  NM_INLINE const uint32_t* getLeafNodeTriangleIndices() const;
  NM_INLINE uint32_t* getLeafNodeTriangleIndices();

  NM_INLINE uint32_t getleafNodeSubTriangleFlagsSize() const;
  NM_INLINE const bool* getLeafNodeSubTriangleFlags() const;
  NM_INLINE bool* getLeafNodeSubTriangleFlags();

  NM_INLINE const bool* getLeafNodeSubTriangleFlags(uint32_t entryIndex) const;
  NM_INLINE bool* getLeafNodeSubTriangleFlags(uint32_t entryIndex);

private:
  // Annotation complete flag
  bool        m_validFlag;

  // Flattened BSP data
  uint32_t    m_bspNumBranchNodes;
  uint32_t    m_bspNumLeafNodes;
  uint32_t    m_bspNumLeafNodeEntries;
  uint32_t    m_bspNumSubSimplexesPerTriangle;

  uint32_t*   m_bspBranchNodeLeft;                ///< Index of the BSP node left of the partition.
  uint32_t*   m_bspBranchNodeRight;               ///< Index of the BSP node right of the partition.
                                                  ///< before leaf nodes, this vector is shorter than m_bspLeft)  
  uint32_t*   m_bspBranchNodeHyperPlaneSampleIndices[2];  ///< Two edge vertex sample indices that construct the hyper-plane
  uint32_t*   m_bspLeafNodeEntryOffsets;          ///< Entry offsets for the leaf node data. An extra element is added to this array
                                                  ///< so that the entry counts can be recovered by forward difference.
  uint32_t*   m_bspLeafNodeTriangleIndices;       ///< Triangle indices corresponding to each leaf partition.
  bool*       m_bspLeafNodeSubTriangleFlags;      ///< Sub-triangle indices corresponding to each leaf partition (Encoded as flags).
};


//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangleSubSampleData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleData::clear()
{
  for (uint32_t i = 0; i < m_numSubSamplesPerTriangle; ++i)
  {
    m_triangleSubSamples[0][i] = 0.0f;
    m_triangleSubSamples[1][i] = 0.0f;
    m_triangleSubSampleValidFlags[i] = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DTriangleSubSampleData::getNumTriangleSubDivisions() const
{
  return m_numTriangleSubDivisions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DTriangleSubSampleData::getNumSubSamplesPerTriangle() const
{
  return m_numSubSamplesPerTriangle;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DTriangleSubSampleData::getTriangleVertexIDs() const
{
  return m_triangleVertexIDs;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleData::setTriangleVertexIDs(
  const uint32_t* triangleVertexIDs)
{
  for (uint32_t i = 0; i < 3; ++i)
    m_triangleVertexIDs[i] = triangleVertexIDs[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleData::getTriangleVertexIDs(
  uint32_t& vertexAID,
  uint32_t& vertexBID,
  uint32_t& vertexCID) const
{
  vertexAID = m_triangleVertexIDs[0];
  vertexBID = m_triangleVertexIDs[1];
  vertexCID = m_triangleVertexIDs[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleData::setTriangleVertexIDs(
  uint32_t vertexAID,
  uint32_t vertexBID,
  uint32_t vertexCID)
{
  m_triangleVertexIDs[0] = vertexAID;
  m_triangleVertexIDs[1] = vertexBID;
  m_triangleVertexIDs[2] = vertexCID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleData::getTriangleSubSample(
  uint32_t triangleSubSampleIndex,
  float& x,
  float& y) const
{
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  x = m_triangleSubSamples[0][triangleSubSampleIndex];
  y = m_triangleSubSamples[1][triangleSubSampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleData::setTriangleSubSample(
  uint32_t triangleSubSampleIndex,
  float x,
  float y)
{
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  m_triangleSubSamples[0][triangleSubSampleIndex] = x;
  m_triangleSubSamples[1][triangleSubSampleIndex] = y;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB2DTriangleSubSampleData::getTriangleSubSampleValidFlag(
  uint32_t triangleSubSampleIndex) const
{
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  return m_triangleSubSampleValidFlags[triangleSubSampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleData::setTriangleSubSampleValidFlag(
  uint32_t triangleSubSampleIndex,
  bool validFlag)
{
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  m_triangleSubSampleValidFlags[triangleSubSampleIndex] = validFlag;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleData::setTriangleSubSampleValidFlags(bool validFlag)
{
  for (uint32_t i = 0; i < m_numSubSamplesPerTriangle; ++i)
  {
    m_triangleSubSampleValidFlags[i] = validFlag;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleData::toggleTriangleSubSampleValidFlags()
{
  for (uint32_t i = 0; i < m_numSubSamplesPerTriangle; ++i)
    m_triangleSubSampleValidFlags[i] = !m_triangleSubSampleValidFlags[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float* SB2DTriangleSubSampleData::getTriangleSubSamplesX()
{
  return m_triangleSubSamples[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float* SB2DTriangleSubSampleData::getTriangleSubSamplesY()
{
  return m_triangleSubSamples[1];
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationExportDataUnpacked inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataUnpacked::getNumTriangleSubDivisions() const
{
  return m_numTriangleSubDivisions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataUnpacked::getNumSubSamplesPerTriangle() const
{
  return m_numSubSamplesPerTriangle;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataUnpacked::getNumVertices() const
{
  return m_numVertices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataUnpacked::getNumTriangles() const
{
  return m_numTriangles;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataUnpacked::getVertex(
  uint32_t vertexIndex,
  float& x,
  float& y) const
{
  NMP_VERIFY(vertexIndex < m_numVertices);
  x = m_vertices[0][vertexIndex];
  y = m_vertices[1][vertexIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataUnpacked::setVertex(
  uint32_t vertexIndex,
  float x,
  float y)
{
  NMP_VERIFY(vertexIndex < m_numVertices);
  m_vertices[0][vertexIndex] = x;
  m_vertices[1][vertexIndex] = y;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB2DAnnotationExportDataUnpacked::getVertexValidFlag(
  uint32_t vertexIndex) const
{
  NMP_VERIFY(vertexIndex < m_numVertices);
  return m_vertexValidFlags[vertexIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataUnpacked::setVertexValidFlag(
  uint32_t vertexIndex,
  bool validFlag)
{
  NMP_VERIFY(vertexIndex < m_numVertices);
  m_vertexValidFlags[vertexIndex] = validFlag;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataUnpacked::getTriangleSubSample(
  uint32_t triangleIndex,
  uint32_t triangleSubSampleIndex,
  float& x,
  float& y) const
{
  NMP_VERIFY(triangleIndex < m_numTriangles);
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  uint32_t index = triangleIndex * m_numSubSamplesPerTriangle + triangleSubSampleIndex;
  x = m_triangleSubSamples[0][index];
  y = m_triangleSubSamples[1][index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataUnpacked::setTriangleSubSample(
  uint32_t triangleIndex,
  uint32_t triangleSubSampleIndex,
  float x,
  float y)
{
  NMP_VERIFY(triangleIndex < m_numTriangles);
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  uint32_t index = triangleIndex * m_numSubSamplesPerTriangle + triangleSubSampleIndex;
  m_triangleSubSamples[0][index] = x;
  m_triangleSubSamples[1][index] = y;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataUnpacked::getTriangleVertexIDs(
  uint32_t triangleIndex,
  uint32_t& vertexAID,
  uint32_t& vertexBID,
  uint32_t& vertexCID) const
{
  NMP_VERIFY(triangleIndex < m_numTriangles);
  vertexAID = m_triangleVertexIDs[0][triangleIndex];
  vertexBID = m_triangleVertexIDs[1][triangleIndex];
  vertexCID = m_triangleVertexIDs[2][triangleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataUnpacked::setTriangleVertexIDs(
  uint32_t triangleIndex,
  uint32_t vertexAID,
  uint32_t vertexBID,
  uint32_t vertexCID)
{
  NMP_VERIFY(triangleIndex < m_numTriangles);
  m_triangleVertexIDs[0][triangleIndex] = vertexAID;
  m_triangleVertexIDs[1][triangleIndex] = vertexBID;
  m_triangleVertexIDs[2][triangleIndex] = vertexCID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB2DAnnotationExportDataUnpacked::getTriangleSubSampleValidFlag(
  uint32_t triangleIndex,
  uint32_t triangleSubSampleIndex) const
{
  NMP_VERIFY(triangleIndex < m_numTriangles);
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  uint32_t index = triangleIndex * m_numSubSamplesPerTriangle + triangleSubSampleIndex;
  return m_triangleValidFlags[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataUnpacked::setTriangleSubSampleValidFlag(
  uint32_t triangleIndex,
  uint32_t triangleSubSampleIndex,
  bool validFlag)
{
  NMP_VERIFY(triangleIndex < m_numTriangles);
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  uint32_t index = triangleIndex * m_numSubSamplesPerTriangle + triangleSubSampleIndex;
  m_triangleValidFlags[index] = validFlag;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* SB2DAnnotationExportDataUnpacked::getVerticesX() const
{
  return m_vertices[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* SB2DAnnotationExportDataUnpacked::getVerticesY() const
{
  return m_vertices[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float* SB2DAnnotationExportDataUnpacked::getVerticesX()
{
  return m_vertices[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float* SB2DAnnotationExportDataUnpacked::getVerticesY()
{
  return m_vertices[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const bool* SB2DAnnotationExportDataUnpacked::getVertexValidFlags() const
{
  return m_vertexValidFlags;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool* SB2DAnnotationExportDataUnpacked::getVertexValidFlags()
{
  return m_vertexValidFlags;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* SB2DAnnotationExportDataUnpacked::getTriangleSubSamplesX() const
{
  return m_triangleSubSamples[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* SB2DAnnotationExportDataUnpacked::getTriangleSubSamplesY() const
{
  return m_triangleSubSamples[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float* SB2DAnnotationExportDataUnpacked::getTriangleSubSamplesX()
{
  return m_triangleSubSamples[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float* SB2DAnnotationExportDataUnpacked::getTriangleSubSamplesY()
{
  return m_triangleSubSamples[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationExportDataUnpacked::getTriangleVertexAIDs() const
{
  return m_triangleVertexIDs[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationExportDataUnpacked::getTriangleVertexBIDs() const
{
  return m_triangleVertexIDs[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationExportDataUnpacked::getTriangleVertexCIDs() const
{
  return m_triangleVertexIDs[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationExportDataUnpacked::getTriangleVertexAIDs()
{
  return m_triangleVertexIDs[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationExportDataUnpacked::getTriangleVertexBIDs()
{
  return m_triangleVertexIDs[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationExportDataUnpacked::getTriangleVertexCIDs()
{
  return m_triangleVertexIDs[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const bool* SB2DAnnotationExportDataUnpacked::getTriangleValidFlags() const
{
  return m_triangleValidFlags;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool* SB2DAnnotationExportDataUnpacked::getTriangleValidFlags()
{
  return m_triangleValidFlags;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationExportDataPacked inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataPacked::getNumTriangleSubDivisions() const
{
  return m_numTriangleSubDivisions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataPacked::getNumSubSamplesPerTriangle() const
{
  return m_numSubSamplesPerTriangle;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataPacked::getNumVertices() const
{
  return m_numVertices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataPacked::getNumTriangles() const
{
  return m_numTriangles;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataPacked::getNumSubSamples() const
{
  return m_numSubSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataPacked::getNumSamples() const
{
  return m_numVertices + m_numSubSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataPacked::getSample(
  uint32_t index,
  float& x,
  float& y) const
{
  NMP_VERIFY(index < getNumSamples());
  x = m_samples[0][index];
  y = m_samples[1][index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataPacked::setSample(
  uint32_t index,
  float x,
  float y)
{
  NMP_VERIFY(index < getNumSamples());
  m_samples[0][index] = x;
  m_samples[1][index] = y;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* SB2DAnnotationExportDataPacked::getSamplesX() const
{
  return m_samples[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* SB2DAnnotationExportDataPacked::getSamplesY() const
{
  return m_samples[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float* SB2DAnnotationExportDataPacked::getSamplesX()
{
  return m_samples[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float* SB2DAnnotationExportDataPacked::getSamplesY()
{
  return m_samples[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataPacked::getNumTriangleSampleIndices() const
{
  uint32_t numTriangleSubSampleIndices = m_numSubSamplesPerTriangle * m_numTriangles;
  return numTriangleSubSampleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationExportDataPacked::getTriangleSampleIndices() const
{
  return m_triangleSampleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationExportDataPacked::getTriangleSampleIndices()
{
  return m_triangleSampleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataPacked::getTriangleSubSampleIndex(
  uint32_t triangleIndex,
  uint32_t triangleSubSampleIndex,
  uint32_t& value) const
{
  NMP_VERIFY(triangleIndex < m_numTriangles);
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  uint32_t index = triangleIndex * m_numSubSamplesPerTriangle + triangleSubSampleIndex;
  value = m_triangleSampleIndices[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataPacked::setTriangleSubSampleIndex(
  uint32_t triangleIndex,
  uint32_t triangleSubSampleIndex,
  uint32_t value)
{
  NMP_VERIFY(triangleIndex < m_numTriangles);
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  uint32_t index = triangleIndex * m_numSubSamplesPerTriangle + triangleSubSampleIndex;
  m_triangleSampleIndices[index] = value;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataPacked::getTriangleSubSampleIndices(
  uint32_t triangleIndex,
  uint32_t& sampleIndexA,
  uint32_t& sampleIndexB,
  uint32_t& sampleIndexC) const
{
  // Get the triangle sub-sample indices corresponding to the major vertices A, B, C
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(m_numTriangleSubDivisions);
  const uint16_t* vertexSubSampleIndices = subDivisionDataBlock.getVertexSubSampleIndices();
  NMP_VERIFY(vertexSubSampleIndices);

  // Get the triangle sub-sample index map
  NMP_VERIFY(triangleIndex < m_numTriangles);
  const uint32_t* triangleSampleIndices = &m_triangleSampleIndices[triangleIndex * m_numSubSamplesPerTriangle];

  // Samples
  sampleIndexA = triangleSampleIndices[vertexSubSampleIndices[0]];
  sampleIndexB = triangleSampleIndices[vertexSubSampleIndices[1]];
  sampleIndexC = triangleSampleIndices[vertexSubSampleIndices[2]];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB2DAnnotationExportDataPacked::isValidTriangleSampleIndices() const
{
  uint32_t numSamples = m_numVertices + m_numSubSamples;
  uint32_t numTriangleSubSampleIndices = m_numSubSamplesPerTriangle * m_numTriangles;

  for (uint32_t i = 0; i < numTriangleSubSampleIndices; ++i)
  {
    if (m_triangleSampleIndices[i] >= numSamples)
      return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataPacked::getNumExteriorEdgeFacets() const
{
  return m_numExteriorEdgeFacets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationExportDataPacked::getExteriorTriangleIndices() const
{
  return m_exteriorTriangleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationExportDataPacked::getExteriorEdgeFacetIndices() const
{
  return m_exteriorEdgeFacetIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationExportDataPacked::getExteriorTriangleIndices()
{
  return m_exteriorTriangleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationExportDataPacked::getExteriorEdgeFacetIndices()
{
  return m_exteriorEdgeFacetIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataPacked::getExteriorEdgeFacet(
  uint32_t entryIndex,
  uint32_t& exteriorTriangleIndex,
  uint32_t& exteriorEdgeFacetIndex) const
{
  NMP_VERIFY(entryIndex < m_numExteriorEdgeFacets);
  exteriorTriangleIndex = m_exteriorTriangleIndices[entryIndex];
  exteriorEdgeFacetIndex = m_exteriorEdgeFacetIndices[entryIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataPacked::setExteriorEdgeFacet(
  uint32_t entryIndex,
  uint32_t exteriorTriangleIndex,
  uint32_t exteriorEdgeFacetIndex)
{
  NMP_VERIFY(entryIndex < m_numExteriorEdgeFacets);
  m_exteriorTriangleIndices[entryIndex] = exteriorTriangleIndex;
  m_exteriorEdgeFacetIndices[entryIndex] = exteriorEdgeFacetIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB2DAnnotationExportDataPacked::getAnnotationValidFlags() const
{
  return m_annotationValidFlags;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataPacked::setAnnotationValidFlags(bool validFlags)
{
  m_annotationValidFlags = validFlags;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationExportDataPacked::getNumSubSimplexesPerTriangle() const
{
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(m_numTriangleSubDivisions);

  return subDivisionDataBlock.getNumSubSimplexesPerTriangle();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationExportDataPacked::getSubTriangleSubSampleIndices(
  uint32_t triangleIndex,
  uint32_t subDivTriangleIndex,
  uint32_t& sampleIndexA,
  uint32_t& sampleIndexB,
  uint32_t& sampleIndexC) const
{
  // Get the triangle sub-sample index map
  uint16_t triangleSubSampleIndex;
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(m_numTriangleSubDivisions);
  NMP_VERIFY(triangleIndex < m_numTriangles);
  NMP_VERIFY(subDivTriangleIndex < subDivisionDataBlock.getNumSubSimplexesPerTriangle());
  const uint32_t* triangleSampleIndices = &m_triangleSampleIndices[triangleIndex * m_numSubSamplesPerTriangle];

  // Sample A
  triangleSubSampleIndex = subDivisionDataBlock.m_subDivisionTriangleIndices[0][subDivTriangleIndex];
  sampleIndexA = triangleSampleIndices[triangleSubSampleIndex];
  // Sample B
  triangleSubSampleIndex = subDivisionDataBlock.m_subDivisionTriangleIndices[1][subDivTriangleIndex];
  sampleIndexB = triangleSampleIndices[triangleSubSampleIndex];
  // Sample C
  triangleSubSampleIndex = subDivisionDataBlock.m_subDivisionTriangleIndices[2][subDivTriangleIndex];
  sampleIndexC = triangleSampleIndices[triangleSubSampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeExportData inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB2DAnnotationBSPTreeExportData::getValidFlag() const
{
  return m_validFlag;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeExportData::setValidFlag(bool flag)
{
  m_validFlag = flag;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeExportData::getNumSubSimplexesPerTriangle() const
{
  return m_bspNumSubSimplexesPerTriangle;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeExportData::getNumBranchNodes() const
{
  return m_bspNumBranchNodes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeExportData::getNumLeafNodes() const
{
  return m_bspNumLeafNodes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeExportData::getNumNodes() const
{
  return m_bspNumBranchNodes + m_bspNumLeafNodes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeExportData::getNumLeafNodeEntries() const
{
  return m_bspNumLeafNodeEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationBSPTreeExportData::getBranchNodeLeft() const
{
  return m_bspBranchNodeLeft;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationBSPTreeExportData::getBranchNodeLeft()
{
  return m_bspBranchNodeLeft;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationBSPTreeExportData::getBranchNodeRight() const
{
  return m_bspBranchNodeRight;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationBSPTreeExportData::getBranchNodeRight()
{
  return m_bspBranchNodeRight;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationBSPTreeExportData::getBranchNodeHyperPlaneSampleIndicesA() const
{
  return m_bspBranchNodeHyperPlaneSampleIndices[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationBSPTreeExportData::getBranchNodeHyperPlaneSampleIndicesA()
{
  return m_bspBranchNodeHyperPlaneSampleIndices[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationBSPTreeExportData::getBranchNodeHyperPlaneSampleIndicesB() const
{
  return m_bspBranchNodeHyperPlaneSampleIndices[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationBSPTreeExportData::getBranchNodeHyperPlaneSampleIndicesB()
{
  return m_bspBranchNodeHyperPlaneSampleIndices[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationBSPTreeExportData::getLeafNodeEntryOffsets() const
{
  return m_bspLeafNodeEntryOffsets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationBSPTreeExportData::getLeafNodeEntryOffsets()
{
  return m_bspLeafNodeEntryOffsets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DAnnotationBSPTreeExportData::getLeafNodeTriangleIndices() const
{
  return m_bspLeafNodeTriangleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DAnnotationBSPTreeExportData::getLeafNodeTriangleIndices()
{
  return m_bspLeafNodeTriangleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeExportData::getleafNodeSubTriangleFlagsSize() const
{
  return m_bspNumSubSimplexesPerTriangle * m_bspNumLeafNodeEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const bool* SB2DAnnotationBSPTreeExportData::getLeafNodeSubTriangleFlags() const
{
  return m_bspLeafNodeSubTriangleFlags;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool* SB2DAnnotationBSPTreeExportData::getLeafNodeSubTriangleFlags()
{
  return m_bspLeafNodeSubTriangleFlags;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const bool* SB2DAnnotationBSPTreeExportData::getLeafNodeSubTriangleFlags(uint32_t entryIndex) const
{
  NMP_VERIFY(entryIndex < m_bspNumLeafNodeEntries);
  return &m_bspLeafNodeSubTriangleFlags[m_bspNumSubSimplexesPerTriangle * entryIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool* SB2DAnnotationBSPTreeExportData::getLeafNodeSubTriangleFlags(uint32_t entryIndex)
{
  NMP_VERIFY(entryIndex < m_bspNumLeafNodeEntries);
  return &m_bspLeafNodeSubTriangleFlags[m_bspNumSubSimplexesPerTriangle * entryIndex];
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_SCATTER_BLEND_2D_ANNOTATION_EXPORT_H
//----------------------------------------------------------------------------------------------------------------------
