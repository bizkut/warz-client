// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_NDMESH_SEARCH_MAP_H
#define MR_NDMESH_SEARCH_MAP_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "morpheme/Prediction/mrNDMesh.h"
#include "morpheme/Prediction/mrNDMeshQueryData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

namespace ScatteredData
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::NDMeshAPSearchMap
/// \brief Bounding box tree for fast searching of the NDMesh. This structure is efficient for mapping
/// analysis properties back to control control parameters, for projection and calculating component ranges
/// within the analysed property space.
//----------------------------------------------------------------------------------------------------------------------
class NDMeshAPSearchMap
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Serialisation
  /// \brief  Functions to relocating the data in memory
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Prepare data for another platform
  static void dislocate(NDMeshAPSearchMap* target);

  /// \brief Prepare data for the local platform
  static void locate(NDMeshAPSearchMap* target);

  /// \brief Prepare data after being moved on the local platform
  static NDMeshAPSearchMap* relocate(void*& ptr);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inverse Prediction Map: AP -> CP
  /// \brief  Functions to handle inverse prediction queries from analysis properties to control parameters
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  // NDMesh - A regularly sampled N-dimensional mesh, whose vertex samples contain the N-d regularly sampled
  // controlling parameters in the first N components, and the analysed properties in the remaining M components.
  // The annotation of cells in this NDMesh forms the basis for interpolating query points between controlling
  // parameter and analysed property component spaces.

  // Independent variables - The components of the vertex samples used as the query point when determining the
  // containing cell and vertex interpolation weights.

  /// \brief Inverse search map using the analysed properties as independent variables for interpolation.
  /// The cell to interpolate is found via a search through the analysed component space. The search algorithm
  /// uses simple bounding box containment to quickly determine the initial set of candidate cells. The vertex
  /// interpolation weights are computed from the queryPoint position within the containing cell. The output
  /// vector is interpolated from the specified components of the cell's vertex samples identified by
  /// the outputComponentIndices array. Slower than using lookup, but is a very general solution.
  ///
  /// \param nDMesh The NDMesh on which the search map has been compiled for
  ///
  /// \param queryPoint An m_numDimensions array of values identifying the independent variables for interpolation.
  ///   This vector may be modified by projection if the queryPoint lies outside of the cell annotations in
  ///   analysed component space.
  ///
  /// \param queryPointComponentIndices An m_numDimensions array of indices identifying which components of a
  ///   vertex sample form the independent variables of the queryPoint. Note: The algorithm will work if the query
  ///   components are the same as the controlling parameters, however this is much slower than using mapFromCPs
  ///   instead.
  ///
  /// \param numOutputSampleComponents The number of components to interpolate.
  ///
  /// \param outputSampleComponentIndices Which components of the cell's vertex samples to interpolate.
  ///
  /// \param outputSampleComponents The resulting vector of interpolated sample component values. The elements
  ///   of this vector are interpolated from the components of the cell's vertex samples identified by the
  ///   outputComponentIndices array.
  ///
  /// \param enableProjection Flag indicating whether projection should be performed if the query point
  ///   lies outside of the NDCell annotations.
  ///
  /// \param projectionParams The projection method and options to use when the queryPoint lies outside of
  ///   the cell annotations in analysed component space.
  InterpolationReturnVal mapFromAPs(
    const NDMesh*       nDMesh,
    float*              queryPoint,
    const uint32_t*     queryPointComponentIndices,
    NDMeshQueryResult*  ndMeshQueryResult,
    uint32_t            numOutputSampleComponents,
    const uint32_t*     outputSampleComponentIndices,
    float*              outputSampleComponents,
    bool                enableProjection,
    const Projection*   projectionParams = 0
    ) const;

  /// \brief Function finds the NDCell containing the query point and interpolates the required output components.
  ///
  /// \param nDMesh The NDMesh on which the search map has been compiled for
  ///
  /// \param queryPoint An m_numDimensions array of values identifying a point in the sample space using the sample
  ///   components identified by the queryPointComponentIndices array as the independent variables. On input these
  ///   components form the independent variables.
  ///
  /// \param queryPointComponentIndices An m_numDimensions array of indices identifying which components of a
  ///   vertex sample form the independent variables of the queryPoint.
  ///
  /// \param ndMeshQueryResult The query result data for the NDCell interpolation.
  ///
  /// \param numOutputSampleComponents The number of components to interpolate.
  ///
  /// \param outputSampleComponentIndices Which components of the cell's vertex samples to interpolate.
  ///
  /// \param outputSampleComponents The resulting vector of interpolated sample component values. The elements
  ///   of this vector are interpolated from the components of the cell's vertex samples identified by the
  ///   outputComponentIndices array.
  bool interpolate(
    const NDMesh*       nDMesh,
    const float*        queryPoint,
    const uint32_t*     queryPointComponentIndices,
    NDMeshQueryResult*  ndMeshQueryResult,
    uint32_t            numOutputSampleComponents = 0,
    const uint32_t*     outputSampleComponentIndices = 0,
    float*              outputSampleComponents = 0
    ) const;

  /// \brief Project the query point onto the nearest point on the boundary of the sample space by
  /// sacrificing the specified dimension. i.e. projection along one of the principal axis directions.
  ///
  /// \param nDMesh The NDMesh on which the search map has been compiled for
  ///
  /// \param queryPoint An m_numDimensions array of values identifying a point in the sample space using the sample
  ///   components identified by the queryPointComponentIndices array as the independent variables. On input these
  ///   components form the independent variables, while on output these components are overwritten with the nearest
  ///   projected intersection point on the boundary.
  ///
  /// \param queryPointComponentIndices An m_numDimensions array of indices identifying which components of a
  ///   vertex sample form the independent variables of the queryPoint.
  ///
  /// \param whichVariableToCompromise An index into the queryPoint array identifying which independent variable can change
  ///   to bring the query point into the sample space; equivalently, the projection ray is the line through the query point
  ///   along that dimension.
  ///   NOTE that whichVariableToCompromise is an index into the queryPoint array, not into the vertex sample array - so for
  ///   instance if the queryPointComponentIndices array identifies the independent variables as the sample components {0, 1, 5},
  ///   and it is the third independent variable that is allowed to be compromised, whichVariableToCompromise is 2, NOT 5.
  ///
  /// \param ndMeshQueryResult The query result data for the projection onto the NDCell facet.
  ///
  /// \param numOutputSampleComponents The number of components to interpolate.
  ///
  /// \param outputSampleComponentIndices Which components of the cell's vertex samples to interpolate.
  ///
  /// \param outputSampleComponents The resulting vector of interpolated sample component values. The elements
  ///   of this vector are interpolated from the components of the cell's vertex samples identified by the
  ///   outputComponentIndices array.
  bool projectAlongOneDimension(
    const NDMesh*       nDMesh,
    float*              queryPoint,
    const uint32_t*     queryPointComponentIndices,
    uint32_t            whichVariableToCompromise,
    NDMeshQueryResult*  ndMeshQueryResult,
    uint32_t            numOutputSampleComponents = 0,
    const uint32_t*     outputSampleComponentIndices = 0,
    float*              outputSampleComponents = 0
    ) const;

  /// \brief Project the query point onto the nearest point on the boundary of the sample space by
  /// sacrificing the components along the specified projection direction.
  ///
  /// \param queryPoint An m_numDimensions array of values identifying a point in the sample space using the sample
  ///   components identified by the queryPointComponentIndices array as the independent variables. On input these
  ///   components form the independent variables, while on output these components are overwritten with the nearest
  ///   projected intersection point on the boundary.
  ///
  /// \param queryPointComponentIndices An m_numDimensions array of indices identifying which components of a
  ///   vertex sample form the independent variables of the queryPoint.
  ///
  /// \param projectionCentre An m_numDimensions array of values identifying a point in the sample space using the
  ///   sample components identified by the queryPointComponentIndices array as the target for projection. The
  ///   projection vector is formed as the line joining both the query and target points.
  ///
  /// \param ndMeshQueryResult The query result data for the projection onto the NDCell facet.
  ///
  /// \param numOutputSampleComponents The number of components to interpolate.
  ///
  /// \param outputSampleComponentIndices Which components of the cell's vertex samples to interpolate.
  ///
  /// \param outputSampleComponents The resulting vector of interpolated sample component values. The elements
  ///   of this vector are interpolated from the components of the cell's vertex samples identified by the
  ///   outputComponentIndices array.
  bool projectThroughPoint(
    const NDMesh*       nDMesh,
    float*              queryPoint,
    const uint32_t*     queryPointComponentIndices,
    const float*        projectionCentre,
    NDMeshQueryResult*  ndMeshQueryResult,
    uint32_t            numOutputSampleComponents = 0,
    const uint32_t*     outputSampleComponentIndices = 0,
    float*              outputSampleComponents = 0
    ) const;

  /// \brief Get the minimum and maximum values of one of the independent variables fixing the values of the others.
  ///
  /// \param nDMesh The NDMesh on which the search map has been compiled for
  ///
  /// \param queryPoint An m_numDimensions array of values identifying a point in the sample space using the sample
  ///   components identified by the queryPointComponentIndices array as the independent variables.
  ///
  /// \param queryPointComponentIndices An m_numDimensions array of indices identifying which components of a
  ///   vertex sample form the queryPoint of independent variables.
  ///
  /// \param whichQueryVariable The index of the independent variable for which you are querying the range.
  ///   Note this is an index into the queryPoint array, not into the vertex sample data.
  ///
  /// \param minimum The minimum extent of the specified independent variable.
  ///
  /// \param maximum The maximum extent of the specified independent variable.
  ///
  /// \param ndMeshQueryResultRear The rear NDMesh query result
  ///
  /// \param ndMeshQueryResultFore The fore NDMesh query result
  bool getRangeInDimension(
    const NDMesh*       nDMesh,
    const float*        queryPoint,
    const uint32_t*     queryPointComponentIndices,
    uint32_t            whichQueryVariable,
    float&              minimum,
    float&              maximum,
    NDMeshQueryResult*  ndMeshQueryResultRear,
    NDMeshQueryResult*  ndMeshQueryResultFore
    ) const;
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   AABB query
  /// \brief  Functions for querying the search map for AABB intersection
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Recover the list of NDMesh cells that intersect the input AABB. The function returns the
  /// number of leaf nodes intersecting the AABB.
  ///
  /// \param nDMesh The NDMesh on which the search map has been compiled for
  ///
  /// \param queryPointComponentIndices A numDimensions array of indices identifying which components
  /// of a vertex sample form the AABB independent variables.
  ///
  /// \param aabbComponentMinVals A numDimensions array of AABB min components
  ///
  /// \param aabbComponentMaxVals A numDimensions array of AABB max components
  ///
  /// \param nodeEntriesSize The maximum number of node entries available for the result.
  ///
  /// \param nodeEntries The result entry array
  NM_INLINE uint32_t getNDCellsIntersectingQueryAABBInterpolate(
    const NDMesh*     nDMesh,
    const uint32_t*   queryPointComponentIndices,
    const float*      aabbComponentMinVals,
    const float*      aabbComponentMaxVals,
    uint32_t          nodeEntriesSize,
    NDCellEntry*      nodeEntries
    ) const;

  /// \brief Recover the list of NDMesh cell facets that project onto the specified hyper-plane
  /// and intersect the AABB on it. The function returns the number of leaf nodes in the foreground
  /// that intersect the AABB.
  ///
  /// \param nDMesh The NDMesh on which the search map has been compiled for
  ///
  /// \param queryPointComponentIndices A numDimensions array of indices identifying which components
  /// of a vertex sample form the AABB independent variables.
  ///
  /// \param whichAABBDim Which dim of the AABB is fixed to form the (N-1)d hyper-plane.
  ///   This is also the dimension that is compromised for projection.
  ///
  /// \param aabbComponentMinVals the query AABB min values Nd. whichDim and whichBoundary describe
  ///   which is the fixed component in the query
  ///
  /// \param aabbComponentMaxVals the query AABB max values Nd. whichDim and whichBoundary describe
  ///   which is the fixed component in the query
  ///
  /// \param nodeEntriesSize The maximum number of node entries available for the result.
  ///
  /// \param nodeEntries The result entry array
  NM_INLINE uint32_t getNDCellFacetsIntersectingQueryAABBProjectAlongOneDimension(
    const NDMesh*     nDMesh,
    const uint32_t*   queryPointComponentIndices,
    uint32_t          whichAABBDim,
    const float*      aabbComponentMinVals,
    const float*      aabbComponentMaxVals,
    uint32_t          nodeEntriesSize,
    NDCellFacetEntry* nodeEntries
    ) const;

  /// \brief Recover the list of NDMesh cell facets that project onto the specified hyper-plane
  /// and intersect the AABB on it. The function returns the number of leaf nodes in the foreground
  /// that intersect the AABB.
  ///
  /// \param nDMesh The NDMesh on which the search map has been compiled for
  ///
  /// \param projectionCentre The point through which all ray-casts originate
  ///
  /// \param queryPointComponentIndices A numDimensions array of indices identifying which components
  /// of a vertex sample form the AABB independent variables.
  ///
  /// \param whichAABBDim Which dim of the AABB is fixed to form the (N-1)d hyper-plane.
  ///
  /// \param aabbComponentMinVals the query AABB min values Nd. whichDim and whichBoundary describe
  ///   which is the fixed component in the query
  ///
  /// \param aabbComponentMaxVals the query AABB max values Nd. whichDim and whichBoundary describe
  ///   which is the fixed component in the query
  ///
  /// \param nodeEntriesSize The maximum number of node entries available for the result.
  ///
  /// \param nodeEntries The result entry array
  NM_INLINE uint32_t getNDCellFacetsIntersectingQueryAABBProjectThroughPoint(
    const NDMesh*     nDMesh,
    const float*      projectionCentre,
    const uint32_t*   queryPointComponentIndices,
    uint32_t          whichAABBDim,
    const float*      aabbComponentMinVals,
    const float*      aabbComponentMaxVals,
    uint32_t          nodeEntriesSize,
    NDCellFacetEntry* nodeEntries
    ) const;
  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the APSearchMap
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE uint32_t getNumDimensions() const { return m_numDimensions; }
  NM_INLINE uint32_t getNumComponentsPerSample() const { return m_numComponentsPerSample; }
  //@}

protected:

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Internal helper functions
  /// \brief  Functions to handle recursive search map queries
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE uint32_t getNumNodes() const;
  NM_INLINE bool isLeafNode(uint32_t nodeID) const;
  NM_INLINE bool isBranchNode(uint32_t nodeID) const;
  NM_INLINE uint32_t getBranchPartitionDim(uint32_t nodeID) const;
  NM_INLINE uint32_t getBranchNodeLeft(uint32_t nodeID) const;
  NM_INLINE uint32_t getBranchNodeRight(uint32_t nodeID) const;

  NM_INLINE bool isEdgeElement(uint32_t nodeID) const;

  /// \brief An interpolation helper function that is called recursively to test if the query point
  /// lies within each search map node's AABB.
  ///
  /// \param nodeID The index of the search map node currently being tested
  ///
  /// \param resultData The interpolation query state data containing the query point, workspace
  ///   memory for the vertex samples, and the solver data. The query result is set with the
  ///   multi-linear weights if the point point lies within the NDCell.
  bool interpolate(
    uint32_t nodeID,
    InterpolateResultQuery* resultData) const;

  /// \brief A ray projection helper function that is called recursively to test for intersection with
  /// each search map node's AABB.
  ///
  /// \param nodeID The index of the search map node currently being tested
  ///
  /// \param resultData The projection query state data containing the query point, workspace
  ///   memory for the vertex samples, and the solver data. The query result is updated with the
  ///   multi-linear weights if the point projects onto an NDCell facet.
  ///
  /// \param projectionBasis A basis frame identifying the query point origin, projection direction
  ///   and its orthogonal complement of directions.
  bool projectRayThroughPoint(
    uint32_t nodeID,
    ProjectRayResultQuery* resultData,
    const ProjectionBasis* projectionBasis) const;

  /// \brief A ray projection helper function that is called recursively to test for intersection with
  /// each search map node's AABB.
  ///
  /// \param nodeID The index of the search map node currently being tested
  ///
  /// \param resultData The projection query state data containing the query point, workspace
  ///   memory for the vertex samples, and the solver data. The query result is updated with the
  ///   multi-linear weights if the point projects onto an NDCell facet.
  ///
  /// \param projectionBasis A basis frame identifying the query point origin, projection direction
  ///   and its orthogonal complement of directions.
  bool projectRayAlongOneDimension(
    uint32_t nodeID,
    ProjectRayResultQuery* resultData,
    const ProjectionBasis* projectionBasis) const;

  /// \brief An interpolation helper function that is called recursively to test if the query AABB
  /// intersects each search map node's AABB.
  ///
  /// \param nodeID The index of the search map node currently being tested
  ///
  /// \param resultData The interpolation query state data containing the query AABB, workspace
  ///   memory for the vertex samples.
  void getNDCellIntersectingQueryAABBInterpolate(
    uint32_t nodeID,
    AABBIntersectionResult* resultData) const;

  /// \brief A ray projection helper function that is called recursively to test if each search map
  /// node's AABB intersects the queryy AABB by projection.
  ///
  /// \param nodeID The index of the search map node currently being tested
  ///
  /// \param resultData The projection query state data containing the query AABB, workspace
  ///   memory for the vertex samples.
  void getNDCellFacetIntersectingQueryAABBProjectAlongOneDimension(
    uint32_t nodeID,
    AABBProjectionResult* resultData) const;

  /// \brief A ray projection helper function that is called recursively to test if each search map
  /// node's AABB intersects the queryy AABB by projection.
  ///
  /// \param nodeID The index of the search map node currently being tested
  ///
  /// \param resultData The projection query state data containing the query AABB, workspace
  ///   memory for the vertex samples.
  void getNDCellFacetIntersectingQueryAABBProjectThroughPoint(
    uint32_t nodeID,
    AABBProjectionResult* resultData) const;
  //@}

protected:
  /// \brief The number of dimensions (controlling parameters).
  uint32_t        m_numDimensions;

  /// \brief The number of analysed properties
  uint32_t        m_numAnalysedProperties;

  /// \brief The number of components CPs + APs that make up a vertex sample
  uint32_t        m_numComponentsPerSample;

  /// \brief The number of branch nodes that partition the sample space
  uint32_t        m_numBranchNodes;

  /// \brief The number of leaf nodes that partition the sample space
  uint32_t        m_numLeafNodes;

  /// \brief Offsets for determining if an AABB node touches the external boundary.
  /// Nodes are re-ordered in the asset compiler so that branch nodes that touch
  /// the external boundary appear first in the list, and leaf nodes touching the
  /// boundary appear last in the list.
  uint32_t        m_edgeBranchNodeOffset;
  uint32_t        m_edgeLeafNodeOffset;

  /// \brief The AABBs that partition the sample space
  uint16_t*       m_aabbMinVals;    // All nodes
  uint16_t*       m_aabbMaxVals;    // All nodes

  /// \brief The branch node children
  uint8_t*        m_branchPartitionDims;
  uint16_t*       m_left;           // Branch nodes only
  uint16_t*       m_right;          // Branch nodes only
};


//----------------------------------------------------------------------------------------------------------------------
// NDMeshAPSearchMap inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshAPSearchMap::getNumNodes() const
{
  return m_numBranchNodes + m_numLeafNodes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NDMeshAPSearchMap::isLeafNode(uint32_t nodeID) const
{
  NMP_ASSERT(nodeID < getNumNodes());
  return nodeID >= m_numBranchNodes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NDMeshAPSearchMap::isBranchNode(uint32_t nodeID) const
{
  NMP_ASSERT(nodeID < getNumNodes());
  return nodeID < m_numBranchNodes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshAPSearchMap::getBranchPartitionDim(uint32_t nodeID) const
{
  NMP_ASSERT(nodeID < m_numBranchNodes);
  return m_branchPartitionDims[nodeID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshAPSearchMap::getBranchNodeLeft(uint32_t nodeID) const
{
  NMP_ASSERT(nodeID < m_numBranchNodes);
  return m_left[nodeID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshAPSearchMap::getBranchNodeRight(uint32_t nodeID) const
{
  NMP_ASSERT(nodeID < m_numBranchNodes);
  return m_right[nodeID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NDMeshAPSearchMap::isEdgeElement(uint32_t nodeID) const
{
  return nodeID < m_edgeBranchNodeOffset || nodeID >= m_edgeLeafNodeOffset;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshAPSearchMap::getNDCellsIntersectingQueryAABBInterpolate(
  const NDMesh*     nDMesh,
  const uint32_t*   queryPointComponentIndices,
  const float*      aabbComponentMinVals,
  const float*      aabbComponentMaxVals,
  uint32_t          nodeEntriesSize,
  NDCellEntry*      nodeEntries) const
{
  // Apply the transform that maps the query AABBs into the normalised space
  float aabbComponentMinValsNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    aabbComponentMinVals,
    aabbComponentMinValsNorm);

  float aabbComponentMaxValsNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    aabbComponentMaxVals,
    aabbComponentMaxValsNorm);

  AABBIntersectionResult resultData;
  resultData.init(
    nDMesh,
    queryPointComponentIndices,
    aabbComponentMinValsNorm,
    aabbComponentMaxValsNorm,
    nodeEntriesSize,
    nodeEntries);

  getNDCellIntersectingQueryAABBInterpolate(0, &resultData);

  return resultData.getNodeEntryCount();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshAPSearchMap::getNDCellFacetsIntersectingQueryAABBProjectAlongOneDimension(
  const NDMesh*     nDMesh,
  const uint32_t*   queryPointComponentIndices,
  uint32_t          whichAABBDim,
  const float*      aabbComponentMinVals,
  const float*      aabbComponentMaxVals,
  uint32_t          nodeEntriesSize,
  NDCellFacetEntry* nodeEntries) const
{
  // Apply the transform that maps the query AABBs into the normalised space
  float aabbComponentMinValsNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    aabbComponentMinVals,
    aabbComponentMinValsNorm);

  float aabbComponentMaxValsNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    aabbComponentMaxVals,
    aabbComponentMaxValsNorm);

  AABBProjectionResult resultData;
  resultData.initAlongDimension(
    nDMesh,
    queryPointComponentIndices,
    whichAABBDim,
    aabbComponentMinValsNorm,
    aabbComponentMaxValsNorm,
    nodeEntriesSize,
    nodeEntries);

  getNDCellFacetIntersectingQueryAABBProjectAlongOneDimension(0, &resultData);

  return resultData.getNodeEntryCount();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshAPSearchMap::getNDCellFacetsIntersectingQueryAABBProjectThroughPoint(
  const NDMesh*     nDMesh,
  const float*      projectionCentre,
  const uint32_t*   queryPointComponentIndices,
  uint32_t          whichAABBDim,
  const float*      aabbComponentMinVals,
  const float*      aabbComponentMaxVals,
  uint32_t          nodeEntriesSize,
  NDCellFacetEntry* nodeEntries) const
{
  // Apply the transform that maps the projection centre into the normalised space
  float projectionCentreNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    projectionCentre,
    projectionCentreNorm);

  // Apply the transform that maps the query AABBs into the normalised space
  float aabbComponentMinValsNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    aabbComponentMinVals,
    aabbComponentMinValsNorm);

  float aabbComponentMaxValsNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    aabbComponentMaxVals,
    aabbComponentMaxValsNorm);

  // Compute the intersection test
  AABBProjectionResult resultData;
  resultData.initThroughPoint(
    nDMesh,
    projectionCentreNorm,
    queryPointComponentIndices,
    whichAABBDim,
    aabbComponentMinValsNorm,
    aabbComponentMaxValsNorm,
    nodeEntriesSize,
    nodeEntries);

  getNDCellFacetIntersectingQueryAABBProjectThroughPoint(0, &resultData);

  return resultData.getNodeEntryCount();
}

} // namespace ScatteredData

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NDMESH_SEARCH_MAP_H
//----------------------------------------------------------------------------------------------------------------------
