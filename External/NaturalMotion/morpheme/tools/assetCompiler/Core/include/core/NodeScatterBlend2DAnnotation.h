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
#ifndef MR_NODE_SCATTER_BLEND_2D_ANNOTATION_H
#define MR_NODE_SCATTER_BLEND_2D_ANNOTATION_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include <list>
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "morpheme/Nodes/mrScatterBlend2DUtils.h"
#include "NodeScatterBlend2DAnnotationExport.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
class SB2DVertex;
class SB2DEdge;
class SB2DTriangle;

//----------------------------------------------------------------------------------------------------------------------
enum SB2DTriangleElementType
{
  kSB2DVertexElementType = 0,
  kSB2DEdgeElementType,
  kSB2DTriangleElementType,

  SB2DNumTriangleElementTypes,
  kSB2DInvalidElementType = 0xFFFFFFFF
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangleElement
//----------------------------------------------------------------------------------------------------------------------
class SB2DTriangleElement
{
public:
  SB2DTriangleElement(
    SB2DTriangleElementType elementType,
    uint32_t numElementSubSamples);

  ~SB2DTriangleElement();

  //---------------------------------------------------------------------
  /// \name   Triangle connectivity
  /// \brief  Functions to manage the triangle connectivity list
  //---------------------------------------------------------------------
  //@{
  NM_INLINE uint32_t getNumTriangleConnections() const;

  NM_INLINE SB2DTriangle* getFrontTriangleConnection() const;

  NM_INLINE const std::list<SB2DTriangle*>& getTriangleConnections() const;

  NM_INLINE void appendTriangleConnection(
    SB2DTriangle* triangle);

  NM_INLINE void removeTriangleConnection(
    SB2DTriangle* triangle);

  NM_INLINE void removeAllTriangleConnections();

  NM_INLINE bool isConnectedToTriangle(
    const SB2DTriangle* triangle) const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Triangle element data
  /// \brief  Functions to manage the triangle element data
  //---------------------------------------------------------------------
  //@{  
  NM_INLINE SB2DTriangleElementType getElementType() const;
  
  NM_INLINE void setElementType(
    SB2DTriangleElementType elementType);
    
  /// \brief Get the triangle vertex with the corresponding ID. The function returns
  /// NULL if none of the connected vertices have the appropriate ID.
  NM_INLINE SB2DVertex* getElementVertexWithID(
    uint32_t vertexID);

  NM_INLINE SB2DVertex* getElementVertex(
    uint32_t index) const;
    
  NM_INLINE void setElementVertex(
    uint32_t index,
    SB2DVertex* elementVertex);
    
  NM_INLINE bool isValidElementVertex(
    const SB2DVertex* vertex) const;

  NM_INLINE uint32_t getNumElementSubSamples() const;

  // Homogeneous element sub-samples
  NM_INLINE void getElementSubSample(
    uint32_t elementSubSampleIndex,
    float& x,
    float& y,
    float& w) const;

  NM_INLINE void setElementSubSample(
    uint32_t elementSubSampleIndex,
    float x,
    float y,
    float w);

  // Inhomogeneous element sub-sample functions
  NM_INLINE void getElementSubSampleInhomogeneous(
    uint32_t elementSubSampleIndex,
    float& x,
    float& y) const;

  NM_INLINE void setElementSubSampleInhomogeneous(
    uint32_t elementSubSampleIndex,
    float x,
    float y);

  NM_INLINE void addElementSubSampleInhomogeneous(
    uint32_t elementSubSampleIndex,
    float x,
    float y);

  NM_INLINE void getElementSubSampleWeight(
    uint32_t elementSubSampleIndex,
    float* subSampleWeights) const;

  NM_INLINE void setElementSubSampleWeight(
    uint32_t elementSubSampleIndex,
    const float* subSampleWeights);

  NM_INLINE bool getElementSubSampleValidFlag(
    uint32_t elementSubSampleIndex) const;

  NM_INLINE void setElementSubSampleValidFlag(
    uint32_t elementSubSampleIndex,
    bool validFlag);

  NM_INLINE bool getElementSubSampleValidFlags() const;

  NM_INLINE void setElementSubSampleValidFlags(
    bool validFlag);
  //@}

protected:
  // The triangles that share the element
  std::list<SB2DTriangle*>      m_triangleConnections;

  // Element information
  SB2DTriangleElementType       m_elementType;
  SB2DVertex*                   m_elementVertices[3];   ///< Vertex order in which the triangle element sub-sample data is stored.

  // Element sub-sample data
  uint32_t                      m_numElementSubSamples;
  float*                        m_elementSubSamples[3]; // Homogeneous point [x, y, w]
  float*                        m_elementSubSampleWeights[3];
  bool*                         m_elementSubSampleValidFlags;
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangleSubSampleElementMap
//----------------------------------------------------------------------------------------------------------------------
class SB2DTriangleSubSampleElementMap
{
public:
  SB2DTriangleSubSampleElementMap();
  ~SB2DTriangleSubSampleElementMap();

  void init(uint32_t numTriangleSubDivisions);

  void release();

  NM_INLINE uint32_t getNumTriangleSubDivisions() const;
  NM_INLINE uint32_t getNumSubSamplesPerTriangle() const;

  NM_INLINE uint32_t* getTempIndexBuffer();

  NM_INLINE void clearElementMap();
  NM_INLINE bool isValidElementMap() const;

  NM_INLINE void getElementMapEntry(
    uint32_t triangleSubSampleIndex,
    SB2DTriangleElementType& triangleElementType,
    uint32_t& triangleElementIndex,
    uint32_t& elementSubSampleIndex) const;

  NM_INLINE void setElementMapEntry(
    uint32_t triangleSubSampleIndex,
    SB2DTriangleElementType triangleElementType,
    uint32_t triangleElementIndex,
    uint32_t elementSubSampleIndex);

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

private:
  uint32_t                      m_numTriangleSubDivisions;
  uint32_t                      m_numSubSamplesPerTriangle;

  // Temporary buffers
  uint32_t*                     m_tempIndexBuffer;

  // Triangle element map
  SB2DTriangleElementType*      m_triangleElementTypes;
  uint32_t*                     m_triangleElementIndices;
  uint32_t*                     m_elementSubSampleIndices;

  // Triangle data
  uint32_t                      m_triangleVertexIDs[3];
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DVertex
//----------------------------------------------------------------------------------------------------------------------
class SB2DVertex : public SB2DTriangleElement
{
public:
  SB2DVertex();
  ~SB2DVertex();

  NM_INLINE uint32_t getVertexID() const;
  NM_INLINE void setVertexID(uint32_t vertexID);

private:
  uint32_t            m_vertexID;
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DEdge
//----------------------------------------------------------------------------------------------------------------------
class SB2DEdge : public SB2DTriangleElement
{
public:
  SB2DEdge(uint32_t numElementSubSamples);
  ~SB2DEdge();
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangle
//----------------------------------------------------------------------------------------------------------------------
class SB2DTriangle : public SB2DTriangleElement
{
public:
  SB2DTriangle(uint32_t numElementSubSamples);
  ~SB2DTriangle();

  //---------------------------------------------------------------------
  /// \name   Information
  /// \brief  Functions to retrieve high level information about the triangle
  //---------------------------------------------------------------------
  //@{
  NM_INLINE uint32_t getTriangleID() const;

  NM_INLINE void setTriangleID(uint32_t triangleID);

  NM_INLINE void getTriangleVertexIDs(
    uint32_t& vertexAID,
    uint32_t& vertexBID,
    uint32_t& vertexCID) const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Vertex connectivity
  /// \brief  Functions to retrieve high level information about the vertex connectivity.
  //---------------------------------------------------------------------
  //@{  
  void getEdgeVertices(
    uint32_t edgeIndex,
    SB2DVertex*& vertexA,
    SB2DVertex*& vertexB) const;
    
  void getTriangleVertices(
    SB2DVertex*& vertexA,
    SB2DVertex*& vertexB,
    SB2DVertex*& vertexC) const;

  /// \brief Function checks if the vertices ABC produce a counter-clockwise geometry.
  /// i.e. the signed area of the triangle is positive.
  static bool isVertexOrderingCCW(
    float Ax,
    float Ay,
    float Bx,
    float By,
    float Cx,
    float Cy);

  /// \brief Function checks if the vertices ABC are all collinear (lay on a line).
  static bool isVertexOrderingCollinear(
    float Ax,
    float Ay,
    float Bx,
    float By,
    float Cx,
    float Cy);

  /// \brief Function recovers a permuted order of the triangle vertex indices that produce a
  /// counter-clockwise geometry. i.e. the signed area of the triangle is positive.
  /// Returns false if the vertices are collinear.
  static bool getVertexOrderingCCW(
    uint32_t numVertices,               ///< IN: The number of vertices
    const float* samplesX,              ///< IN: X component array of vertex samples.
    const float* samplesY,              ///< IN: Y component array of vertex samples.
    const uint32_t* vertexIndicesIn,    ///< IN: Indices within the sample arrays for the triangle's A, B, C vertices.
    uint32_t* vertexIndicesOut          ///< OUT: Permuted triangle vertex indices that have a counter-clockwise geometry.
    );

  /// \brief Function checks if the vertices ABC produce a counter-clockwise geometry.
  /// i.e. the signed area of the triangle is positive.
  bool isValidVertexOrdering() const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Triangle elements
  /// \brief  Functions to allow access to the component elements of the triangle
  //---------------------------------------------------------------------
  //@{

  /// \brief Finds the vertex index corresponding to the specified vertex
  NM_INLINE uint32_t getVertexIndex(
    const SB2DVertex* vertex) const;

  /// \brief Converts sample index in the space of the stored vertex element
  /// to a sub-sample index in the space of the triangle.
  uint32_t getTriangleSubSampleIndexForVertexElement(
    uint32_t numTriangleSubDivisions,
    const SB2DVertex* vertex) const;


  /// \brief Get the triangle edge with the appropriate edge index.
  NM_INLINE SB2DEdge* getEdgeElement(
    uint32_t edgeIndex) const;

  /// \brief Set the triangle edge with the appropriate edge index.
  NM_INLINE void setEdgeElement(
    uint32_t edgeIndex,
    SB2DEdge* edge);

  /// \brief Finds the edge index corresponding to the specified edge
  NM_INLINE uint32_t getEdgeIndex(
      const SB2DEdge* edge) const;

  /// \brief Converts sample indices in the space of the stored edge element
  /// to sub-sample indices in the space of the triangle.
  void getTriangleSubSampleIndicesForEdgeElement(
    uint32_t numTriangleSubDivisions,
    const SB2DEdge* edge,
    uint32_t* triangleSubSampleIndices) const;

  /// \brief Converts sample indices in the space of the stored triangle element (interior)
  /// to sub-sample indices in the space of the triangle.
  void getTriangleSubSampleIndicesForTriangleElement(
    uint32_t numTriangleSubDivisions,
    uint32_t* triangleSubSampleIndices) const;

  /// \brief Computes the map between triangle sub-sample indices to triangle
  /// element sub-samples.
  void getTriangleSubSampleElementMap(
    SB2DTriangleSubSampleElementMap& elementMap) const;

  void getTriangleSubSampleData(
    const SB2DTriangleSubSampleElementMap& elementMap,
    SB2DTriangleSubSampleData& triangleSubSampleData) const;

  void setTriangleSubSampleData(
    const SB2DTriangleSubSampleElementMap& elementMap,
    const SB2DTriangleSubSampleData& triangleSubSampleData);

  void addTriangleSubSampleData(
    const SB2DTriangleSubSampleElementMap& elementMap,
    const SB2DTriangleSubSampleData& triangleSubSampleData);

  /// \brief Function flags all connected triangle elements as having valid data
  void setValidFlags(
    bool validFlag);

  /// \brief Function checks if all connected triangle elements have valid data
  bool getValidFlags() const;
  //@}

private:
  // Identifier
  uint32_t              m_triangleID;
  
  // The edges of the triangle opposite each of the vertices.
  SB2DEdge*             m_edges[3];
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotation
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotation
{
public:
  SB2DAnnotation();
  ~SB2DAnnotation();

  // Information
  NM_INLINE uint32_t getNumVertices() const;
  NM_INLINE uint32_t getNumUniqueEdges() const;
  NM_INLINE uint32_t getNumTriangles() const;

  NM_INLINE uint32_t getNumTriangleSubDivisions() const;

  NM_INLINE const std::list<SB2DVertex*> getVertexList() const;
  NM_INLINE const std::list<SB2DEdge*> getEdgeList() const;
  NM_INLINE const std::list<SB2DTriangle*> getTriangleList() const;

  /// \brief Destroys all data in the annotation graph
  void release();

  /// \brief Initialise an empty annotation with the required sub-division specification
  void initAnnotation(
    uint32_t numTriangleSubDivisions);

  /// \brief Converts the annotation graph into the packed export data format
  void getAnnotation(
    SB2DAnnotationExportDataPacked& exportData) const;

  /// \brief Converts the annotation graph into the unpacked export data format
  void getAnnotation(
    SB2DAnnotationExportDataUnpacked& exportData) const;

  /// \brief Builds the annotation graph from the packed export data format
  void setAnnotation(
    const SB2DAnnotationExportDataPacked& exportData);

  /// \brief Builds the annotation graph from the unpacked export data format
  void setAnnotation(
    const SB2DAnnotationExportDataUnpacked& exportData);

  /// \brief Removes all triangles from the annotation graph, destroying the required
  /// memory for the facet, edge and triangle sub-sample data.
  void removeAllTrianglesFromAnnotation();

  /// \brief Removes all triangles whose vertices are contained within the vertex set,
  /// destroying the required memory for the facet, edge and triangle sub-sample data.
  void removeTrianglesInVertexSetFromAnnotation(
    uint32_t numVertices,
    const uint32_t* triangleVertexIDs);

  /// \brief Return the number of unique sub-samples within the annotation.
  /// i.e. The full set of samples that sub-divide the triangles, less the
  /// annotation vertices and the duplicate edges & triangle facets.
  uint32_t calculateNumSubSamples() const;

  /// \brief Return the number of edge facets that lay on the exterior
  /// boundary of the annotation.
  uint32_t calculateNumExteriorEdgeFacets() const;

  /// \brief Recover the list of exterior triangles
  void getExteriorTriangles(
    std::list<SB2DTriangle*>& triangleList) const;

  // Validation
  bool isValidVertex(const SB2DVertex* vertex) const;
  bool isValidEdge(const SB2DEdge* edge) const;
  bool isValidTriangle(const SB2DTriangle* triangle) const;
  bool isValidElementSubSampleData() const;
  bool isValidVertexOrdering() const;
  bool isValidTriangleConnectivity() const;

  /// \brief Get the vertex with the specified ID within the annotation graph
  SB2DVertex* getVertex(uint32_t ID) const;

  /// \brief Gets the triangle that contains vertices with specified IDs
  /// (vertex ID order is unimportant) within the annotation graph.
  SB2DTriangle* getTriangle(const uint32_t* vertexIDs) const;

  /// \brief Gets the edge that contains vertices with specified IDs
  /// (vertex ID order is unimportant) within the annotation graph.
  SB2DEdge* getEdge(const uint32_t* vertexIDs) const;

  /// \brief Adds a new vertex to the annotation graph with the specified ID.
  SB2DVertex* addVertex(uint32_t ID);

  /// \brief Adds a new triangle to the annotation graph with the vertices
  /// specified by their vertex IDs. 
  SB2DTriangle* addTriangle(const uint32_t* vertexIDs);

  /// \brief Adds a new triangle and sample data to the annotation graph with the vertices
  /// specified by their vertex IDs.
  SB2DTriangle* addTriangle(const SB2DTriangleSubSampleData* triangleSubSampleData);

  /// \brief Removes a vertex from the annotation graph. This also removes the
  /// connected triangles, edges and triangle facets.
  void removeVertex(SB2DVertex* vertex);

  /// \brief Removes a triangle from the annotation graph. This also removes the
  /// connected edges and triangle facets.
  void removeTriangle(SB2DTriangle* triangle);

  /// \brief Function returns the list of singleton triangles in the annotation
  /// that are not connected to any others
  void getSingletonTriangles(
    std::list<SB2DTriangle*>& triangleList) const;

  /// \brief Removes redundant triangles whose non-exterior edge facets are
  /// already connected to other triangles. Removal of a triangle does not
  /// expose additional exterior edge facets.
  void getRedundantTriangles(
    std::list<SB2DTriangle*>& triangleList) const;

  uint32_t removeRedundantTriangles();

  /// \brief Annotation validation
  uint32_t getNumUnconnectedVertices() const;

private:
  // Annotation
  std::list<SB2DVertex*>              m_vertexList;
  std::list<SB2DEdge*>                m_edgeList;
  std::list<SB2DTriangle*>            m_triangleList;

  // Sub-samples
  uint32_t                            m_numTriangleSubDivisions;
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeClassifierEntries
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotationBSPTreeClassifierEntries
{
public:
  SB2DAnnotationBSPTreeClassifierEntries();
  ~SB2DAnnotationBSPTreeClassifierEntries();

  void init(
    uint32_t numHyperPlaneEntries,
    uint32_t numTriangleEntries);

  void release();

  void clear();

  NM_INLINE uint32_t getNumTriangleEntries() const;
  NM_INLINE uint32_t getNumHyperPlaneEntries() const;

  NM_INLINE int8_t getClassEntry(
    uint32_t hyperPlaneEntryIndex,
    uint32_t triangleEntryIndex) const;

  NM_INLINE void setClassEntry(
    uint32_t hyperPlaneEntryIndex,
    uint32_t triangleEntryIndex,
    int8_t entry);

  NM_INLINE uint16_t getLeftCount(
    uint32_t hyperPlaneEntryIndex) const;

  NM_INLINE void setLeftCount(
    uint32_t hyperPlaneEntryIndex,
    uint16_t count);

  NM_INLINE uint16_t getBothCount(
    uint32_t hyperPlaneEntryIndex) const;

  NM_INLINE void setBothCount(
    uint32_t hyperPlaneEntryIndex,
    uint16_t count);

  NM_INLINE uint16_t getRightCount(
    uint32_t hyperPlaneEntryIndex) const;

  NM_INLINE void setRightCount(
    uint32_t hyperPlaneEntryIndex,
    uint16_t count);

  NM_INLINE uint16_t getIncorrectCount(
    uint32_t hyperPlaneEntryIndex) const;

  NM_INLINE void setIncorrectCount(
    uint32_t hyperPlaneEntryIndex,
    uint16_t count);

private:
  uint32_t            m_numTriangleEntries;       ///< The total number of sub-triangles in the annotation.
  uint32_t            m_numHyperPlaneEntries;     ///< The number of pre-determined hyper-planes available to
                                                  ///< partition the sub-triangle data.
  int8_t*             m_classEntries;             ///< A -1 if left, 0 if both, 1 if right
  uint16_t*           m_leftCounts;
  uint16_t*           m_bothCounts;
  uint16_t*           m_rightCounts;
  uint16_t*           m_incorrectCounts;
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeSampleEntries
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotationBSPTreeSampleEntries
{
public:
  SB2DAnnotationBSPTreeSampleEntries();
  ~SB2DAnnotationBSPTreeSampleEntries();

  void init(uint32_t numSampleEntries);

  void release();

  void clear();

  NM_INLINE uint32_t getNumSampleEntries() const;

  NM_INLINE void getSampleEntry(
    uint32_t entryIndex,
    NMP::Vector3& sample) const;

  NM_INLINE void setSampleEntry(
    uint32_t entryIndex,
    const NMP::Vector3& sample);

private:
  uint32_t  m_numSampleEntries;
  float*    m_samplesX;
  float*    m_samplesY;
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeTriangleEntries
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotationBSPTreeTriangleEntries
{
public:
  SB2DAnnotationBSPTreeTriangleEntries();
  ~SB2DAnnotationBSPTreeTriangleEntries();

  void init(uint32_t numTriangleEntries);

  void release();

  void clear();

  NM_INLINE uint32_t getNumTriangleEntries() const;

  NM_INLINE void getTriangleEntry(
    uint32_t entryIndex,
    NMP::Vector3& vertexA,
    NMP::Vector3& vertexB,
    NMP::Vector3& vertexC) const;

  NM_INLINE void setTriangleEntry(
    uint32_t entryIndex,
    const NMP::Vector3& vertexA,
    const NMP::Vector3& vertexB,
    const NMP::Vector3& vertexC);

private:
  uint32_t  m_numTrianglesEntries;
  float*    m_verticesX[3];           ///< X components of the three vertices A, B, C of the triangles
  float*    m_verticesY[3];           ///< Y components of the three vertices A, B, C of the triangles
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeHyperPlaneEntries
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotationBSPTreeHyperPlaneEntries
{
public:
  SB2DAnnotationBSPTreeHyperPlaneEntries();
  ~SB2DAnnotationBSPTreeHyperPlaneEntries();

  void init(uint32_t numHyperPlaneEntries);

  void release();

  void clear();

  NM_INLINE uint32_t getNumHyperPlaneEntries() const;

  NM_INLINE void getHyperPlaneEntry(
    uint32_t entryIndex,
    NMP::Vector3& hyperPlane) const;

  NM_INLINE void setHyperPlaneEntry(
    uint32_t entryIndex,
    const NMP::Vector3& hyperPlane);

  NM_INLINE void getHyperPlaneSampleIndices(
    uint32_t entryIndex,
    uint16_t& sampleAIndex,
    uint16_t& sampleBIndex) const;

  NM_INLINE void setHyperPlaneSampleIndices(
    uint32_t entryIndex,
    uint16_t sampleAIndex,
    uint16_t sampleBIndex);

private:
  uint32_t  m_numHyperPlaneEntries;
  float*    m_hyperPlanes[3];             ///< X, Y, W components of the hyper-planes
  uint16_t* m_hyperPlaneSampleIndices[2]; ///< Vertex sample indices that construct the hyper-plane
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeClassifier
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotationBSPTreeClassifier
{
public:
  SB2DAnnotationBSPTreeClassifier();
  ~SB2DAnnotationBSPTreeClassifier();

  void init(
    const SB2DAnnotationExportDataPacked& exportdata);

  void release();

  NM_INLINE uint32_t getNumTriangles() const;
  NM_INLINE uint32_t getNumSubSimplexesPerTriangle() const;

  NM_INLINE SB2DAnnotationBSPTreeTriangleEntries* getTriangleEntries();
  NM_INLINE SB2DAnnotationBSPTreeHyperPlaneEntries* getHyperPlaneEntries();
  NM_INLINE SB2DAnnotationBSPTreeClassifierEntries* getTriangleClassEntries();
  NM_INLINE SB2DAnnotationBSPTreeClassifierEntries* getSubTriangleClassEntries(uint32_t triangleIndex);

private:
  uint32_t                                  m_numTriangles;
  uint32_t                                  m_numSubSimplexesPerTriangle;
  SB2DAnnotationBSPTreeTriangleEntries*     m_triangleEntries;
  SB2DAnnotationBSPTreeTriangleEntries**    m_subTriangleEntries;
  SB2DAnnotationBSPTreeSampleEntries*       m_samplesEntries;
  SB2DAnnotationBSPTreeHyperPlaneEntries*   m_hyperPlaneEntries;
  SB2DAnnotationBSPTreeClassifierEntries*   m_triangleClassEntries;
  SB2DAnnotationBSPTreeClassifierEntries**  m_subTriangleClassEntries;
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreePartitionEntry
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotationBSPTreePartitionEntry
{
public:
  SB2DAnnotationBSPTreePartitionEntry(uint32_t numSubTriangleIndices);
  ~SB2DAnnotationBSPTreePartitionEntry();

public:
  uint32_t    m_triangleIndex;
  uint32_t    m_numSubTriangleIndices;
  uint32_t*   m_subTriangleIndices;
};

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTree
//----------------------------------------------------------------------------------------------------------------------
class SB2DAnnotationBSPTree
{
public:
  //---------------------------
  class BSPNode
  {
  public:
    // Common node data: The left pointer is used to reference a left child branch node.
    // A NULL pointer indicates that this is a BSP Leaf node.
    BSPNode*        m_left;

    union
    {
      // Branch node data: The vertex sample indices within the annotation export that
      // construct the hyper-plane partitioning the data.
      struct
      {
        BSPNode*    m_right;
        uint32_t    m_hyperPlaneSampleIndices[2];   ///< The sample indices corresponding to the vertices constructing the hyper-plane
      };

      // Leaf node data: The (triangle, sub-triangle) index pairs identifying those
      // triangles that lay within the partition.
      struct
      {
        uint32_t    m_numEntries;
        uint32_t*   m_triangleIndices;
        uint32_t*   m_subTriangleIndices;
      };
    };
  };

  //---------------------------
  class BSPNodeMetrics
  {
  public:
    BSPNodeMetrics();

    void clear();

  public:
    uint32_t m_numNodes;
    uint32_t m_numBranchNodes;
    uint32_t m_numLeafNodes;
    uint32_t m_numLeafNodeEntries;

    uint32_t m_minNumEntries;
    uint32_t m_maxNumEntries;
    uint32_t m_maxDepth;
  };

public:
  SB2DAnnotationBSPTree();
  ~SB2DAnnotationBSPTree();

  void release();

  /// \brief Converts the annotation BSP tree into the export data format
  void getBSPTree(
    SB2DAnnotationBSPTreeExportData& exportData) const;

  /// \brief Builds the annotation BSP tree from the export data format
  void setBSPTree(
    const SB2DAnnotationBSPTreeExportData& exportData);

  void computeBSPTree(
    const SB2DAnnotationExportDataPacked& exportdata);

  void writeBSPTree(
    const char* filename) const;

private:
  static void tidyBSPTreePartitionEntries(
    std::vector<SB2DAnnotationBSPTreePartitionEntry*>& partitionEntries);

  static void releaseBSPNode(BSPNode* node);

  static void doComputeBSPTree(
    BSPNode* node,
    SB2DAnnotationBSPTreeClassifier& classifier,
    float costAlpha,
    const std::vector<SB2DAnnotationBSPTreePartitionEntry*>& partitionEntries);

  static void writeBSPNode(
    FILE* fp,
    uint32_t depth,
    const BSPNode* node);

  static void calculateBSPMetrics(
    BSPNodeMetrics& nodeMetrics,
    uint32_t depth,
    const BSPNode* node);

  uint32_t doGetBSPTree(
    SB2DAnnotationBSPTreeExportData& exportData,
    BSPNodeMetrics& state,
    const BSPNode* node) const;

  BSPNode* doSetBSPTree(
    const SB2DAnnotationBSPTreeExportData& exportData,
    BSPNodeMetrics& state,
    uint32_t nodeID);

private:
  bool      m_validFlag;
  uint32_t  m_numTriangles;
  uint32_t  m_numSubSimplexesPerTriangle;
  BSPNode*  m_bspTree;
};


//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangleElement inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DTriangleElement::getNumTriangleConnections() const
{
  return (uint32_t)m_triangleConnections.size();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE SB2DTriangle* SB2DTriangleElement::getFrontTriangleConnection() const
{
  return m_triangleConnections.front();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const std::list<SB2DTriangle*>& SB2DTriangleElement::getTriangleConnections() const
{
  return m_triangleConnections;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::appendTriangleConnection(
  SB2DTriangle* triangle)
{
  m_triangleConnections.push_back(triangle);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::removeTriangleConnection(
  SB2DTriangle* triangle)
{
  m_triangleConnections.remove(triangle);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::removeAllTriangleConnections()
{
  m_triangleConnections.clear();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB2DTriangleElement::isConnectedToTriangle(
  const SB2DTriangle* triangle) const
{
  std::list<SB2DTriangle*>::const_iterator it;
  for (it = m_triangleConnections.begin(); it != m_triangleConnections.end(); ++it)
  {
    if (*it == triangle)
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE SB2DTriangleElementType SB2DTriangleElement::getElementType() const
{
  return m_elementType;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::setElementType(
  SB2DTriangleElementType elementType)
{
  m_elementType = elementType;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE SB2DVertex* SB2DTriangleElement::getElementVertexWithID(
  uint32_t vertexID)
{
  for (uint32_t i = 0; i <= (uint32_t)m_elementType; ++i)
  {
    if (m_elementVertices[i]->getVertexID() == vertexID)
      return m_elementVertices[i];
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE SB2DVertex* SB2DTriangleElement::getElementVertex(
  uint32_t index) const
{
  NMP_VERIFY(index < 3);
  NMP_VERIFY(index <= (uint32_t)m_elementType);
  return m_elementVertices[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::setElementVertex(
  uint32_t index,
  SB2DVertex* elementVertex)
{
  NMP_VERIFY(index < 3);
  NMP_VERIFY(index <= (uint32_t)m_elementType);
  m_elementVertices[index] = elementVertex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB2DTriangleElement::isValidElementVertex(
  const SB2DVertex* vertex) const
{
  NMP_VERIFY(vertex);
  for (uint32_t i = 0; i <= (uint32_t)m_elementType; ++i)
  {
    if (m_elementVertices[i] == vertex)
      return true;
  }
  
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DTriangleElement::getNumElementSubSamples() const
{
  return m_numElementSubSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::getElementSubSample(
  uint32_t elementSubSampleIndex,
  float& x,
  float& y,
  float& w) const
{
  NMP_VERIFY(elementSubSampleIndex <  m_numElementSubSamples);
  x = m_elementSubSamples[0][elementSubSampleIndex];
  y = m_elementSubSamples[1][elementSubSampleIndex];
  w = m_elementSubSamples[2][elementSubSampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::setElementSubSample(
  uint32_t elementSubSampleIndex,
  float x,
  float y,
  float w)
{
  NMP_VERIFY(elementSubSampleIndex <  m_numElementSubSamples);
  m_elementSubSamples[0][elementSubSampleIndex] = x;
  m_elementSubSamples[1][elementSubSampleIndex] = y;
  m_elementSubSamples[2][elementSubSampleIndex] = w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::getElementSubSampleInhomogeneous(
  uint32_t elementSubSampleIndex,
  float& x,
  float& y) const
{
  NMP_VERIFY(elementSubSampleIndex <  m_numElementSubSamples);
  float v[3];
  v[0] = m_elementSubSamples[0][elementSubSampleIndex];
  v[1] = m_elementSubSamples[1][elementSubSampleIndex];
  v[2] = m_elementSubSamples[2][elementSubSampleIndex];
  if (v[2] != 0.0f)
  {
    x = v[0] / v[2];
    y = v[1] / v[2];
  }
  else
  {
    x = y = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::setElementSubSampleInhomogeneous(
  uint32_t elementSubSampleIndex,
  float x,
  float y)
{
  NMP_VERIFY(elementSubSampleIndex <  m_numElementSubSamples);
  m_elementSubSamples[0][elementSubSampleIndex] = x;
  m_elementSubSamples[1][elementSubSampleIndex] = y;
  m_elementSubSamples[2][elementSubSampleIndex] = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::addElementSubSampleInhomogeneous(
  uint32_t elementSubSampleIndex,
  float x,
  float y)
{
  NMP_VERIFY(elementSubSampleIndex <  m_numElementSubSamples);
  m_elementSubSamples[0][elementSubSampleIndex] += x;
  m_elementSubSamples[1][elementSubSampleIndex] += y;
  m_elementSubSamples[2][elementSubSampleIndex] += 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::getElementSubSampleWeight(
  uint32_t elementSubSampleIndex,
  float* subSampleWeights) const
{
  NMP_VERIFY(elementSubSampleIndex <  m_numElementSubSamples);
  NMP_VERIFY(subSampleWeights);
  for (uint32_t i = 0; i <= (uint32_t)m_elementType; ++i)
  {
    NMP_VERIFY(m_elementSubSampleWeights[i]);
    subSampleWeights[i] = m_elementSubSampleWeights[i][elementSubSampleIndex];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::setElementSubSampleWeight(
  uint32_t elementSubSampleIndex,
  const float* subSampleWeights)
{
  NMP_VERIFY(elementSubSampleIndex <  m_numElementSubSamples);
  NMP_VERIFY(subSampleWeights);
  for (uint32_t i = 0; i <= (uint32_t)m_elementType; ++i)
  {
    NMP_VERIFY(m_elementSubSampleWeights[i]);
    m_elementSubSampleWeights[i][elementSubSampleIndex] = subSampleWeights[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB2DTriangleElement::getElementSubSampleValidFlag(
  uint32_t elementSubSampleIndex) const
{
  NMP_VERIFY(elementSubSampleIndex <  m_numElementSubSamples);
  return m_elementSubSampleValidFlags[elementSubSampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::setElementSubSampleValidFlag(
  uint32_t elementSubSampleIndex,
  bool validFlag)
{
  NMP_VERIFY(elementSubSampleIndex <  m_numElementSubSamples);
  m_elementSubSampleValidFlags[elementSubSampleIndex] = validFlag;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB2DTriangleElement::getElementSubSampleValidFlags() const
{
  for (uint32_t i = 0; i < m_numElementSubSamples; ++i)
  {
    if (!m_elementSubSampleValidFlags[i])
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleElement::setElementSubSampleValidFlags(
  bool validFlag)
{
  for (uint32_t i = 0; i < m_numElementSubSamples; ++i)
    m_elementSubSampleValidFlags[i] = validFlag;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangleSubSampleElementMap inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DTriangleSubSampleElementMap::getNumTriangleSubDivisions() const
{
  return m_numTriangleSubDivisions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DTriangleSubSampleElementMap::getNumSubSamplesPerTriangle() const
{
  return m_numSubSamplesPerTriangle;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleElementMap::clearElementMap()
{
  for (uint32_t i = 0; i < m_numSubSamplesPerTriangle; ++i)
  {
    m_triangleElementTypes[i] = kSB2DInvalidElementType;
    m_triangleElementIndices[i] = 0;
    m_elementSubSampleIndices[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB2DTriangleSubSampleElementMap::isValidElementMap() const
{
  for (uint32_t i = 0; i < m_numSubSamplesPerTriangle; ++i)
  {
    if (m_triangleElementTypes[i] == kSB2DInvalidElementType)
      return false;
    NMP_VERIFY(m_triangleElementTypes[i] < SB2DNumTriangleElementTypes);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB2DTriangleSubSampleElementMap::getTempIndexBuffer()
{
  return m_tempIndexBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleElementMap::getElementMapEntry(
  uint32_t triangleSubSampleIndex,
  SB2DTriangleElementType& triangleElementType,
  uint32_t& triangleElementIndex,
  uint32_t& elementSubSampleIndex) const
{
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  triangleElementType = m_triangleElementTypes[triangleSubSampleIndex];
  triangleElementIndex = m_triangleElementIndices[triangleSubSampleIndex];
  elementSubSampleIndex = m_elementSubSampleIndices[triangleSubSampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleElementMap::setElementMapEntry(
  uint32_t triangleSubSampleIndex,
  SB2DTriangleElementType triangleElementType,
  uint32_t triangleElementIndex,
  uint32_t elementSubSampleIndex)
{
  NMP_VERIFY(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  m_triangleElementTypes[triangleSubSampleIndex] = triangleElementType;
  m_triangleElementIndices[triangleSubSampleIndex] = triangleElementIndex;
  m_elementSubSampleIndices[triangleSubSampleIndex] = elementSubSampleIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB2DTriangleSubSampleElementMap::getTriangleVertexIDs() const
{
  return m_triangleVertexIDs;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleElementMap::setTriangleVertexIDs(
  const uint32_t* triangleVertexIDs)
{
  for (uint32_t i = 0; i < 3; ++i)
    m_triangleVertexIDs[i] = triangleVertexIDs[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleElementMap::getTriangleVertexIDs(
  uint32_t& vertexAID,
  uint32_t& vertexBID,
  uint32_t& vertexCID) const
{
  vertexAID = m_triangleVertexIDs[0];
  vertexBID = m_triangleVertexIDs[1];
  vertexCID = m_triangleVertexIDs[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangleSubSampleElementMap::setTriangleVertexIDs(
  uint32_t vertexAID,
  uint32_t vertexBID,
  uint32_t vertexCID)
{
  m_triangleVertexIDs[0] = vertexAID;
  m_triangleVertexIDs[1] = vertexBID;
  m_triangleVertexIDs[2] = vertexCID;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DVertex inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DVertex::getVertexID() const
{
  return m_vertexID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DVertex::setVertexID(uint32_t vertexID)
{
  m_vertexID = vertexID;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DTriangle inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DTriangle::getTriangleID() const
{
  return m_triangleID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangle::setTriangleID(uint32_t triangleID)
{
  m_triangleID = triangleID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangle::getTriangleVertexIDs(
  uint32_t& vertexAID,
  uint32_t& vertexBID,
  uint32_t& vertexCID) const
{
  vertexAID = m_elementVertices[0]->getVertexID();
  vertexBID = m_elementVertices[1]->getVertexID();
  vertexCID = m_elementVertices[2]->getVertexID();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DTriangle::getVertexIndex(
  const SB2DVertex* vertex) const
{
  for (uint32_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
  {
    if (m_elementVertices[vertexIndex] == vertex)
      return vertexIndex;
  }
  return 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE SB2DEdge* SB2DTriangle::getEdgeElement(
  uint32_t edgeIndex) const
{
  NMP_VERIFY(edgeIndex < 3);
  return m_edges[edgeIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DTriangle::setEdgeElement(
  uint32_t edgeIndex,
  SB2DEdge* edge)
{
  NMP_VERIFY(edgeIndex < 3);
  m_edges[edgeIndex] = edge;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DTriangle::getEdgeIndex(
  const SB2DEdge* edge) const
{
  for (uint32_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
  {
    if (m_edges[edgeIndex] == edge)
      return edgeIndex;
  }
  return 0xFFFFFFFF;
}


//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotation inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotation::getNumVertices() const
{
  return (uint32_t)m_vertexList.size();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotation::getNumTriangles() const
{
  return (uint32_t)m_triangleList.size();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotation::getNumTriangleSubDivisions() const
{
  return m_numTriangleSubDivisions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotation::getNumUniqueEdges() const
{
  return (uint32_t)m_edgeList.size();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const std::list<SB2DVertex*> SB2DAnnotation::getVertexList() const
{
  return m_vertexList;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const std::list<SB2DEdge*> SB2DAnnotation::getEdgeList() const
{
  return m_edgeList;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const std::list<SB2DTriangle*> SB2DAnnotation::getTriangleList() const
{
  return m_triangleList;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeClassifierEntries inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeClassifierEntries::getNumTriangleEntries() const
{
  return m_numTriangleEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeClassifierEntries::getNumHyperPlaneEntries() const
{
  return m_numHyperPlaneEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int8_t SB2DAnnotationBSPTreeClassifierEntries::getClassEntry(
  uint32_t hyperPlaneEntryIndex,
  uint32_t triangleEntryIndex) const
{
  NMP_VERIFY(hyperPlaneEntryIndex < m_numHyperPlaneEntries);
  NMP_VERIFY(triangleEntryIndex < m_numTriangleEntries);
  uint32_t index = hyperPlaneEntryIndex * m_numTriangleEntries + triangleEntryIndex;
  return m_classEntries[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeClassifierEntries::setClassEntry(
  uint32_t hyperPlaneEntryIndex,
  uint32_t triangleEntryIndex,
  int8_t entry)
{
  NMP_VERIFY(hyperPlaneEntryIndex < m_numHyperPlaneEntries);
  NMP_VERIFY(triangleEntryIndex < m_numTriangleEntries);
  uint32_t index = hyperPlaneEntryIndex * m_numTriangleEntries + triangleEntryIndex;
  m_classEntries[index] = entry;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t SB2DAnnotationBSPTreeClassifierEntries::getLeftCount(
  uint32_t hyperPlaneEntryIndex) const
{
  NMP_VERIFY(hyperPlaneEntryIndex < m_numHyperPlaneEntries);
  return m_leftCounts[hyperPlaneEntryIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeClassifierEntries::setLeftCount(
  uint32_t hyperPlaneEntryIndex,
  uint16_t count)
{
  NMP_VERIFY(hyperPlaneEntryIndex < m_numHyperPlaneEntries);
  m_leftCounts[hyperPlaneEntryIndex] = count;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t SB2DAnnotationBSPTreeClassifierEntries::getBothCount(
  uint32_t hyperPlaneEntryIndex) const
{
  NMP_VERIFY(hyperPlaneEntryIndex < m_numHyperPlaneEntries);
  return m_bothCounts[hyperPlaneEntryIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeClassifierEntries::setBothCount(
  uint32_t hyperPlaneEntryIndex,
  uint16_t count)
{
  NMP_VERIFY(hyperPlaneEntryIndex < m_numHyperPlaneEntries);
  m_bothCounts[hyperPlaneEntryIndex] = count;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t SB2DAnnotationBSPTreeClassifierEntries::getRightCount(
  uint32_t hyperPlaneEntryIndex) const
{
  NMP_VERIFY(hyperPlaneEntryIndex < m_numHyperPlaneEntries);
  return m_rightCounts[hyperPlaneEntryIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeClassifierEntries::setRightCount(
  uint32_t hyperPlaneEntryIndex,
  uint16_t count)
{
  NMP_VERIFY(hyperPlaneEntryIndex < m_numHyperPlaneEntries);
  m_rightCounts[hyperPlaneEntryIndex] = count;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t SB2DAnnotationBSPTreeClassifierEntries::getIncorrectCount(
  uint32_t hyperPlaneEntryIndex) const
{
  NMP_VERIFY(hyperPlaneEntryIndex < m_numHyperPlaneEntries);
  return m_incorrectCounts[hyperPlaneEntryIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeClassifierEntries::setIncorrectCount(
  uint32_t hyperPlaneEntryIndex,
  uint16_t count)
{
  NMP_VERIFY(hyperPlaneEntryIndex < m_numHyperPlaneEntries);
  m_incorrectCounts[hyperPlaneEntryIndex] = count;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeSampleEntries inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeSampleEntries::getNumSampleEntries() const
{
  return m_numSampleEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeSampleEntries::getSampleEntry(
  uint32_t entryIndex,
  NMP::Vector3& sample) const
{
  NMP_VERIFY(entryIndex < m_numSampleEntries);
  sample.set(
    m_samplesX[entryIndex],
    m_samplesY[entryIndex],
    0.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeSampleEntries::setSampleEntry(
  uint32_t entryIndex,
  const NMP::Vector3& sample)
{
  NMP_VERIFY(entryIndex < m_numSampleEntries);
  m_samplesX[entryIndex] = sample.x;
  m_samplesY[entryIndex] = sample.y;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeTriangleEntries inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeTriangleEntries::getNumTriangleEntries() const
{
  return m_numTrianglesEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeTriangleEntries::getTriangleEntry(
  uint32_t entryIndex,
  NMP::Vector3& vertexA,
  NMP::Vector3& vertexB,
  NMP::Vector3& vertexC) const
{
  NMP_VERIFY(entryIndex < m_numTrianglesEntries);
  vertexA.set(
    m_verticesX[0][entryIndex],
    m_verticesY[0][entryIndex],
    0.0f);
  vertexB.set(
    m_verticesX[1][entryIndex],
    m_verticesY[1][entryIndex],
    0.0f);
  vertexC.set(
    m_verticesX[2][entryIndex],
    m_verticesY[2][entryIndex],
    0.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeTriangleEntries::setTriangleEntry(
  uint32_t entryIndex,
  const NMP::Vector3& vertexA,
  const NMP::Vector3& vertexB,
  const NMP::Vector3& vertexC)
{
  NMP_VERIFY(entryIndex < m_numTrianglesEntries);
  m_verticesX[0][entryIndex] = vertexA.x;
  m_verticesY[0][entryIndex] = vertexA.y;
  m_verticesX[1][entryIndex] = vertexB.x;
  m_verticesY[1][entryIndex] = vertexB.y;
  m_verticesX[2][entryIndex] = vertexC.x;
  m_verticesY[2][entryIndex] = vertexC.y;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeHyperPlaneEntries inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeHyperPlaneEntries::getNumHyperPlaneEntries() const
{
  return m_numHyperPlaneEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeHyperPlaneEntries::getHyperPlaneEntry(
  uint32_t entryIndex,
  NMP::Vector3& hyperPlane) const
{
  NMP_VERIFY(entryIndex < m_numHyperPlaneEntries);
  hyperPlane.set(
    m_hyperPlanes[0][entryIndex],
    m_hyperPlanes[1][entryIndex],
    0.0f,
    m_hyperPlanes[2][entryIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeHyperPlaneEntries::setHyperPlaneEntry(
  uint32_t entryIndex,
  const NMP::Vector3& hyperPlane)
{
  NMP_VERIFY(entryIndex < m_numHyperPlaneEntries);
  m_hyperPlanes[0][entryIndex] = hyperPlane.x;
  m_hyperPlanes[1][entryIndex] = hyperPlane.y;
  m_hyperPlanes[2][entryIndex] = hyperPlane.w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeHyperPlaneEntries::getHyperPlaneSampleIndices(
  uint32_t entryIndex,
  uint16_t& sampleAIndex,
  uint16_t& sampleBIndex) const
{
  NMP_VERIFY(entryIndex < m_numHyperPlaneEntries);
  sampleAIndex = m_hyperPlaneSampleIndices[0][entryIndex];
  sampleBIndex = m_hyperPlaneSampleIndices[1][entryIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB2DAnnotationBSPTreeHyperPlaneEntries::setHyperPlaneSampleIndices(
  uint32_t entryIndex,
  uint16_t sampleAIndex,
  uint16_t sampleBIndex)
{
  NMP_VERIFY(entryIndex < m_numHyperPlaneEntries);
  m_hyperPlaneSampleIndices[0][entryIndex] = sampleAIndex;
  m_hyperPlaneSampleIndices[1][entryIndex] = sampleBIndex;
}

//----------------------------------------------------------------------------------------------------------------------
// SB2DAnnotationBSPTreeClassifier inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeClassifier::getNumTriangles() const
{
  return m_numTriangles;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB2DAnnotationBSPTreeClassifier::getNumSubSimplexesPerTriangle() const
{
  return m_numSubSimplexesPerTriangle;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE SB2DAnnotationBSPTreeTriangleEntries*
SB2DAnnotationBSPTreeClassifier::getTriangleEntries()
{
  return m_triangleEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE SB2DAnnotationBSPTreeHyperPlaneEntries*
SB2DAnnotationBSPTreeClassifier::getHyperPlaneEntries()
{
  return m_hyperPlaneEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE SB2DAnnotationBSPTreeClassifierEntries*
SB2DAnnotationBSPTreeClassifier::getTriangleClassEntries()
{
  return m_triangleClassEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE SB2DAnnotationBSPTreeClassifierEntries*
SB2DAnnotationBSPTreeClassifier::getSubTriangleClassEntries(uint32_t triangleIndex)
{
  NMP_VERIFY(triangleIndex < m_numTriangles);
  return m_subTriangleClassEntries[triangleIndex];
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_SCATTER_BLEND_2D_ANNOTATION_H
//----------------------------------------------------------------------------------------------------------------------
