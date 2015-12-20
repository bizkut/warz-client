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
#ifndef MR_NDMESH_H
#define MR_NDMESH_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "morpheme/Prediction/mrScatteredDataUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

namespace ScatteredData
{

//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
//----------------------------------------------------------------------------------------------------------------------
class NDMeshQueryResult;
class InterpolateResultQuery;
class ProjectionBasis;
class ProjectRayResultQuery;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::NDMesh
/// \brief A regularly sampled N-dimensional mesh, whose vertex samples contain the N-d regularly sampled
/// controlling parameters in the first N components, and the analysed properties in the remaining M components.
/// The annotation of cells in this NDMesh forms the basis for interpolating query points between controlling
/// parameter and analysed property component spaces.
//----------------------------------------------------------------------------------------------------------------------
class NDMesh
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Serialisation
  /// \brief  Functions to relocating the data in memory
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Prepare data for another platform
  static void dislocate(NDMesh* target);

  /// \brief Prepare data for the local platform
  static void locate(NDMesh* target);

  /// \brief Prepare data after being moved on the local platform
  static NDMesh* relocate(void*& ptr);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Forward Prediction Map: CP -> AP
  /// \brief  Functions to handle fast lookup queries from control parameters to analysis properties
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  // Independent variables - The components of the vertex samples used as the query point when determining the
  // containing cell and vertex interpolation weights.

  /// \brief Fast lookup map using the control components as the independent variables for interpolation.
  /// The cell to interpolate is found with a fast lookup using the grid of regularly sampled controlling components.
  /// The output vector is interpolated from the specified components of the cell's vertex samples identified by
  /// the outputComponentIndices array. The vertex interpolation weights are computed from the queryPoint position
  /// within the containing cell.
  ///
  /// \param queryPoint An m_numDimensions array of values identifying the independent controlling components.
  ///   This vector may be modified by projection if the queryPoint lies outside of the regular sample grid.
  ///
  /// \param numOutputComponents The number of components to interpolate.
  ///
  /// \param outputComponentIndices Which components of the cell's vertex samples to interpolate.
  ///
  /// \param outputComponentValues The resulting vector of interpolated sample component values. The elements
  ///   of this vector are interpolated from the components of the cell's vertex samples identified by the
  ///   outputComponentIndices array.
  InterpolationReturnVal mapFromCPs(
    float*          queryPoint,
    uint32_t        numOutputComponents,
    const uint32_t* outputComponentIndices,
    float*          outputComponentValues
    ) const;

  /// \brief Calculate the multi-linear weights for the query point within the regular sample grid
  ///
  /// \param queryPoint An m_numDimensions array of values identifying the independent controlling components.
  ///   This vector may be modified by projection if the queryPoint lies outside of the regular sample grid.
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND cell
  ///
  /// \param ts The interpolants of the query point within the ND cell
  InterpolationReturnVal multilinearWeights(
    float*      queryPoint,   // IN / OUT
    uint32_t*   topLeftCoord, // OUT
    float*      ts            // OUT
    ) const;
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inverse Prediction Map: AP -> CP
  /// \brief  Functions to handle inverse prediction queries from analysis properties to control parameters
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Calculates whether the query point lies within the NDCell and determines the
  /// corresponding set of multi-linear weights. The function only tests the specified NDCell.
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND cell
  ///
  /// \param resultData The interpolation query state data containing the query point, workspace
  ///   memory for the vertex samples, and the solver data. The query result is set with the
  ///   multi-linear weights if the point point lies within the NDCell.
  bool interpolate(
    const uint32_t* topLeftCoord,
    InterpolateResultQuery* resultData
    ) const;

  /// \brief Calculates whether the query point lies within an NDCell and determines
  /// the corresponding set of multi-linear weights. The function searches the local
  /// neighbourhood of NDCells starting from the initial cell coordinate and interpolants.
  ///
  /// \param initTopLeftCoord The initial coordinate of the top-left corner of the ND cell
  ///   for the guided search.
  ///
  /// \param initInterpolants The initial interpolants for the guided search.
  ///
  /// \param resultData The interpolation query state data containing the query point, workspace
  ///   memory for the vertex samples, and the solver data. The query result is set with the
  ///   multi-linear weights if the point point lies within an NDCell.
  bool interpolateUsingGuidedSearch(
    const uint32_t* initTopLeftCoord,
    const float* initInterpolants,
    InterpolateResultQuery* resultData
    ) const;

  /// \brief Calculates whether the query point can be projected along the specified direction onto
  /// the NDCell facet. The function calculates the corresponding set of multi-linear weights, and
  /// only tests the specified NDCell facet.
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND cell
  ///
  /// \param whichDim The dimension of the NDCell identifying the cell facet
  ///
  /// \param whichBoundary Index specifying the near-side or far-side facet for that dimension
  ///
  /// \param resultData The projection query state data containing the query point, workspace
  ///   memory for the vertex samples, and the solver data. The query result is updated with the
  ///   multi-linear weights if the point projects onto the NDCell facet
  ///
  /// \param projectionBasis A basis frame identifying the query point origin, projection direction
  ///   and its orthogonal complement of directions.
  bool projectRay(
    const uint32_t* topLeftCoord,
    uint32_t whichDim,
    uint32_t whichBoundary,
    ProjectRayResultQuery* resultData,
    const ProjectionBasis* projectionBasis
    ) const;

  /// \brief A closest point projection query for the 1D prediction model case.
  ///
  /// \param queryPoint The 1D query value of independent parameters. This value is modified
  ///   by the function.
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param ndMeshQueryResult The NDCell facet coordinates and multi-linear weights for the projected result
  ///
  /// \param numOutputComponents The number of components to interpolate.
  ///
  /// \param outputComponentIndices Which components of the cell's vertex samples to interpolate.
  ///
  /// \param outputComponentValues The resulting vector of interpolated sample component values. The elements
  ///   of this vector are interpolated from the components of the cell's vertex samples identified by the
  ///   outputComponentIndices array.
  void projection1DClosest(
    float*          queryPoint,
    const uint32_t* queryPointComponentIndices,
    NDMeshQueryResult* ndMeshQueryResult,
    uint32_t        numOutputSampleComponents,
    const uint32_t* outputSampleComponentIndices,
    float*          outputSampleComponents
    ) const;

  /// \brief A near and far projection query for the 1D prediction model case suitable for calculating ranges.
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param ndMeshQueryResultRear The near-side NDCell facet coordinates and multi-linear weights for the projection query.
  ///
  /// \param ndMeshQueryResultFore The far-side NDCell facet coordinates and multi-linear weights for the projection query.
  ///
  /// \param valueRear The near-side projected result value.
  ///
  /// \param valueFore The far-side projected result value.
  void projection1DExtremes(
    const uint32_t*     queryPointComponentIndices,
    NDMeshQueryResult*  ndMeshQueryResultRear,
    NDMeshQueryResult*  ndMeshQueryResultFore,
    float&              valueRear,
    float&              valueFore
    ) const;
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Extrapolation: AP -> CP
  /// \brief  Functions to handle extrapolation queries from analysis properties to control parameters
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Compute CPs from APs using a specified NDMesh cell to extrapolate the sample data.
  /// Not all of the AP space can be spanned by multi-linear interpolation of the NDCell. The
  /// function returns false if the query point lies outside of this spanning space.
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND cell
  ///
  /// \param queryPoint The query point to extrapolate from the NDCell
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param interpolants The multi-linear weights required for the extrapolation. This can be used
  ///   as an input/output parameter to provide an initial estimate for the solve, otherwise iterative
  ///   solves start from the cell mid-point.
  ///   
  /// \param useInitialInterpolants A flag indicating if the interpolants parameter is used to provide
  ///   an initial estimate for the solver.
  ///
  /// \param numOutputComponents The number of components to interpolate.
  ///
  /// \param outputComponentIndices Which components of the cell's vertex samples to interpolate.
  ///
  /// \param outputComponentValues The resulting vector of interpolated sample component values. The elements
  ///   of this vector are interpolated from the components of the cell's vertex samples identified by the
  ///   outputComponentIndices array. 
  bool extrapolateCPs(
    const uint32_t*   topLeftCoord,
    const float*      queryPoint,
    const uint32_t*   queryPointComponentIndices,
    float*            interpolants,
    bool              useInitialInterpolants,
    uint32_t          numOutputComponents,
    const uint32_t*   outputComponentIndices,
    float*            outputComponentValues
    ) const;
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex functions
  /// \brief  Functions for accessing NDMesh vertex data
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Determines whether the NDCell facet is located on the external boundary of the NDMesh grid.
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND cell
  ///
  /// \param whichDim The dimension of the NDCell identifying the cell facet
  ///
  /// \param whichBoundary Index specifying the near-side or far-side facet for that dimension
  NM_INLINE bool isNDCellBoundaryFacet(
    const uint32_t* topLeftCoord,
    uint32_t whichDim,
    uint32_t whichBoundary) const;

  /// \brief Retrieves the (min, max) of the specified control parameter sample positions from
  /// the regular sample grid.
  ///
  /// \param whichCPComponent Which control parameter to recover the information for
  ///
  /// \param coordLeft sample grid coordinate for the first query position
  ///
  /// \param coordRight sample grid coordinate for the second query position
  ///
  /// \param The result AABB min value
  ///
  /// \param The result AABB max value
  NM_INLINE void getControlComponentAABB(
    uint32_t whichCPComponent,
    uint32_t coordLeft,
    uint32_t coordRight,
    float& aabbMinValue,
    float& aabbMaxValue) const;

  /// \brief Recovers the CPs + APs components of the specified vertex
  ///
  /// \param coordinate The coordinate within the NDMesh of the vertex to recover
  ///
  /// \param vertex A buffer for the vertex data
  NM_INLINE void getVertex(
    const uint32_t* coordinate,
    float* vertex) const;

  /// \brief Recovers the requested components of the specified vertex
  ///
  /// \param coordinate The coordinate within the NDMesh of the vertex to recover
  ///
  /// \param numSampleComponents The number of vertex components to recover
  ///
  /// \param whichSampleComponents Which components of the vertex to recover
  ///
  /// \param vertexComponents A buffer for the vertex components
  NM_INLINE void getVertexComponents(
    const uint32_t* coordinate,
    uint32_t numSampleComponents,
    const uint32_t* whichSampleComponents,
    float* vertexComponents) const;

  /// \brief Recovers the CPs + APs components of the specified vertex
  ///
  /// \param vertexSampleIndex A flat address identifying which vertex within the NDMesh to recover
  ///
  /// \param vertex A buffer for the vertex data
  void getVertex(
    uint32_t vertexSampleIndex,
    float* vertex) const;

  /// \brief Recovers the requested components of the specified vertex
  ///
  /// \param vertexSampleIndex A flat address identifying which vertex within the NDMesh to recover
  ///
  /// \param numSampleComponents The number of vertex components to recover
  ///
  /// \param whichSampleComponents Which components of the vertex to recover
  ///
  /// \param vertexComponents A buffer for the vertex components
  void getVertexComponents(
    uint32_t vertexSampleIndex,
    uint32_t numSampleComponents,
    const uint32_t* whichSampleComponents,
    float* vertexComponents) const;

  /// \brief Recovers the vertex components for the NDCell using the specified
  /// quantisation scale, offset transform. Specifying the quantisation transform
  /// allows you to recover the vertices in the real analysed or a normalised space.
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND cell
  ///
  /// \param numSampleComponents The number of vertex components to recover
  ///
  /// \param whichSampleComponents Which components of the vertex to recover
  ///
  /// \param qScales The quantisation scales vector
  ///
  /// \param qOffsets The quantisation offsets vector
  ///
  /// \param vertexComponents A buffer for the vertex components
  void getNDCellVertexComponents(
    const uint32_t* topLeftCoord,
    uint32_t numSampleComponents,
    const uint32_t* whichSampleComponents,
    const float* qScales,
    const float* qOffsets,
    float* const* vertexComponents) const;

  /// \brief Recovers the vertex components for the NDCell.
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND cell
  ///
  /// \param numSampleComponents The number of vertex components to recover
  ///
  /// \param whichSampleComponents Which components of the vertex to recover
  ///
  /// \param vertexComponents A buffer for the vertex components
  NM_INLINE void getNDCellVertexComponents(
    const uint32_t* topLeftCoord,
    uint32_t numSampleComponents,
    const uint32_t* whichSampleComponents,
    float* const* vertexComponents) const;

  /// \brief Recovers the vertex components for the NDCell facet using the specified
  /// quantisation scale, offset transform. Specifying the quantisation transform
  /// allows you to recover the vertices in the real analysed or a normalised space.
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND cell
  ///
  /// \param whichDim The dimension of the NDCell identifying the cell facet
  ///
  /// \param whichBoundary Index specifying the near-side or far-side facet for that dimension
  ///
  /// \param numSampleComponents The number of vertex components to recover
  ///
  /// \param whichSampleComponents Which components of the vertex to recover
  ///
  /// \param qScales The quantisation scales vector
  ///
  /// \param qOffsets The quantisation offsets vector
  ///
  /// \param vertexComponents A buffer for the vertex components
  void getNDCellFacetVertexComponents(
    const uint32_t* topLeftCoord,
    uint32_t whichDim,
    uint32_t whichBoundary,
    uint32_t numSampleComponents,
    const uint32_t* whichSampleComponents,
    const float* qScales,
    const float* qOffsets,
    float* const* vertexComponents) const;

  /// \brief Recovers the vertex components for the NDCell facet
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND cell
  ///
  /// \param whichDim The dimension of the NDCell identifying the cell facet
  ///
  /// \param whichBoundary Index specifying the near-side or far-side facet for that dimension
  ///
  /// \param numSampleComponents The number of vertex components to recover
  ///
  /// \param whichSampleComponents Which components of the vertex to recover
  ///
  /// \param vertexComponents A buffer for the vertex components
  NM_INLINE void getNDCellFacetVertexComponents(
    const uint32_t* topLeftCoord,
    uint32_t whichDim,
    uint32_t whichBoundary,
    uint32_t numSampleComponents,
    const uint32_t* whichSampleComponents,
    float* const* vertexComponents) const;

  /// \brief Recover the sample centre of the NDMesh (Barycentre of the vertex samples)
  ///
  /// \param componentsSize The component buffer size - should be the same as the number of components per sample.
  ///
  /// \param components The buffer for the sample centre components
  void getSampleCentre(
    uint32_t          componentsSize,
    float*            components) const;

  /// \brief Recover the sample centre of the NDMesh (Barycentre of the vertex samples)
  ///
  /// \param outputComponentsSize The component buffer size
  ///
  /// \param outputComponentIndices The indices of the vertex components to recover
  ///
  /// \param outputComponentValues The buffer for the sample centre components
  void getSampleCentre(
    uint32_t          outputComponentsSize,
    const uint32_t*   outputComponentIndices,
    float*            outputComponentValues) const;

  /// \brief Recover the Axis Aligned Bounding Box for the NDMesh sample data
  ///
  /// \param componentsSize The component buffer size - should be the same as the number of components per sample.
  ///
  /// \param aabbMinVals The buffer for the AABB min values
  ///
  /// \param aabbMaxVals The buffer for the AABB max values
  void getSampleAABB(
    uint32_t          componentsSize,
    float*            aabbMinVals,
    float*            aabbMaxVals) const;

  /// \brief Recover the Axis Aligned Bounding Box for the NDMesh sample data
  ///
  /// \param numSampleComponents The component buffer size
  ///
  /// \param queryPointComponentIndices The indices of the vertex components to recover
  ///
  /// \param aabbComponentMinVals The buffer for the AABB min values
  ///
  /// \param aabbComponentMaxVals The buffer for the AABB max values
  void getSampleAABB(
    uint32_t          numSampleComponents,
    const uint32_t*   queryPointComponentIndices,
    float*            aabbComponentMinVals,
    float*            aabbComponentMaxVals) const;
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the NDMesh
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  NM_INLINE uint32_t getNumDimensions() const { return m_numDimensions; }
  NM_INLINE uint32_t getNumVerticesPerNDCell() const { return 1 << m_numDimensions; }
  NM_INLINE uint32_t getNumVerticesPerNDCellFacet() const { return 1 << (m_numDimensions - 1); }

  NM_INLINE uint32_t getNumAnalysisProperties() const { return m_numAnalysisProperties; }
  NM_INLINE uint32_t getNumComponentsPerSample() const { return m_numComponentsPerSample; }
  NM_INLINE uint32_t getNumSamples() const { return m_numSamples; }
  NM_INLINE const uint32_t* getSampleCountsPerDimension() const { return m_sampleCountsPerDimension; }

  NM_INLINE const float* getControlComponentStartValues() const { return m_controlComponentStartValues; }
  NM_INLINE const float* getControlComponentEndValues() const { return m_controlComponentEndValues; }

  NM_INLINE const float* getScalesQuantisedToReal() const { return m_scalesQuantisedToReal; }
  NM_INLINE const float* getOffsetsQuantisedToReal() const { return m_offsetsQuantisedToReal; }

  NM_INLINE const float* getScalesRealToNorm() const { return m_scalesRealToNorm; }
  NM_INLINE const float* getOffsetsRealToNorm() const { return m_offsetsRealToNorm; }

  NM_INLINE const float* getScalesQuantisedToNorm() const { return m_scalesQuantisedToNorm; }
  NM_INLINE const float* getOffsetsQuantisedToNorm() const { return m_offsetsQuantisedToNorm; }

  /// \brief Transforms a query point from real space to the normalised coordinate space
  ///
  /// \param numComponents The number of components in the query point
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param queryPointIn The input query point in real space
  ///
  /// \param queryPointOut The transformed query point in normalised space (can be the same as queryPointIn)
  NM_INLINE void transformRealToNorm(
    uint32_t numComponents,
    const uint32_t* queryPointComponentIndices,
    const float* queryPointIn,
    float* queryPointOut
    ) const;

  /// \brief Get the indices of all analysis property components within a vertex sample. i.e. i + numCPs
  NM_INLINE const uint32_t* getAnalysisPropertyComponentIndices() const { return m_analysisPropertyComponentIndices; }

  /// \brief Get the indices of the vertex sample components that are not in the query set.
  /// The function returns the number of complementary component indices.
  ///
  /// \param numQueryComponents The number of number of components in the query point
  ///
  /// \param queryComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param queryComponentComplement The output buffer in which to recover the complement to the query indices
  NM_INLINE uint32_t getComplementaryQueryComponentIndices(
    uint32_t          numQueryComponents,
    const uint32_t*   queryComponentIndices,
    uint32_t*         queryComponentComplement
    ) const;

  /// \brief Convert an N-D grid sample coordinate to a flat sample address
  NM_INLINE uint32_t coordinateToAddress(
    const uint32_t* coordinate) const;

  /// \brief Convert a flat sample address to a N-D grid sample coordinate
  NM_INLINE void coordinateFromAddress(
    uint32_t* coordinate,
    uint32_t address) const;

  /// \brief Checks whether the sample coordinate lays within the NDMesh
  NM_INLINE bool isValidVertexCoordinate(
    const uint32_t* coordinate) const;

  /// \brief Checks whether the cell topLeft coordinate lays within the NDMesh
  NM_INLINE bool isValidCellCoordinate(
    const uint32_t* topLeftCoord) const;

  /// \brief Recovers the full N-d multi-linear weights from the query result,
  /// converting the (N-1)d cell facet representation appropriately.
  ///
  /// \param ndMeshQueryResult The query result data structure
  ///
  /// \param interpolants The output Nd multi-linear weights
  bool ndCellInterpolants(
    const NDMeshQueryResult* ndMeshQueryResult,
    float* interpolants
    ) const;
  //@}

protected:
  /// \brief Number of dimensions (number of controlling parameters).
  uint32_t    m_numDimensions;

  /// \brief Number of analysis properties
  uint32_t    m_numAnalysisProperties;

  /// \brief The number of components CPs + APs that make up a vertex sample
  uint32_t    m_numComponentsPerSample;

  /// \brief Total number of vertex samples in the dataset.
  uint32_t    m_numSamples;

  /// \brief For grid data, the number of samples in each dimension.
  uint32_t*   m_sampleCountsPerDimension;

  /// \brief Block size array needed for traversing the sample grid. This encodes the
  /// data components strides to convert a cell coordinate to a flattened sample index.
  uint32_t*   m_nDBlockSizes;

  /// \brief For grid data, the starting value for the variables in each dimension.
  float*      m_controlComponentStartValues;

  /// \brief For grid data, the end value for the variables in each dimension.
  float*      m_controlComponentEndValues;

  /// \brief For grid data, the reciprocal of the sample period (space between each sample in each dimension).
  float*      m_controlComponentSampleScales;

  /// \brief Normalisation transformation scale and offset for the vertex samples (CPs + APs).
  /// This is used during inverse prediction query to normalise the query AP components to a
  /// suitable range to avoid issues with numerical error.
  float*      m_scalesRealToNorm;
  float*      m_offsetsRealToNorm;
  float*      m_scalesQuantisedToNorm;
  float*      m_offsetsQuantisedToNorm;

  /// \brief Quantisation scale and offset information for the vertex samples (CPs + APs).
  float*      m_scalesQuantisedToReal;
  float*      m_offsetsQuantisedToReal;

  /// \brief The block of memory storing the quantised vertex data samples (CPs + APs).
  uint16_t*   m_vertexSamples;

  /// \brief Definition data for convenience
  uint32_t*   m_analysisPropertyComponentIndices;

  /// \brief The Barycentre (weighted average) of the vertex sample data
  uint16_t*   m_sampleCentre;
};


//----------------------------------------------------------------------------------------------------------------------
// NDMesh inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMesh::transformRealToNorm(
  uint32_t numComponents,
  const uint32_t* queryPointComponentIndices,
  const float* queryPointIn,
  float* queryPointOut) const
{
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(queryPointIn);
  NMP_ASSERT(queryPointOut);

  for (uint32_t i = 0; i < numComponents; ++i)
  {
    uint32_t index = queryPointComponentIndices[i];
    NMP_ASSERT(index < m_numComponentsPerSample);
    queryPointOut[i] = m_scalesRealToNorm[index] * queryPointIn[i] + m_offsetsRealToNorm[index];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMesh::getComplementaryQueryComponentIndices(
  uint32_t          numQueryComponents,
  const uint32_t*   queryComponentIndices,
  uint32_t*         queryComponentComplement) const
{
  bool isIndependentVar[SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
  for (uint32_t i = 0; i < m_numComponentsPerSample; ++i)
  {
    isIndependentVar[i] = false;
  }

  // Determine which components to interpolate
  for (uint32_t i = 0; i < numQueryComponents; ++i)
  {
    isIndependentVar[queryComponentIndices[i]] = true;
  }
  uint32_t outputComponentsSize = 0;
  for (uint32_t i = 0; i < m_numComponentsPerSample; ++i)
  {
    if (!isIndependentVar[i])
    {
      queryComponentComplement[outputComponentsSize] = i;
      ++outputComponentsSize;
    }
  }

  return outputComponentsSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMesh::coordinateToAddress(
  const uint32_t* coordinate) const
{
  NMP_ASSERT(coordinate);
  uint32_t index = 0;
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    NMP_ASSERT(coordinate[dim] < m_sampleCountsPerDimension[dim]);
    index += coordinate[dim] * m_nDBlockSizes[dim];
  }
  return index;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMesh::coordinateFromAddress(
  uint32_t* coordinate,
  uint32_t address) const
{
  NMP_ASSERT(coordinate);
  for (uint32_t dim = m_numDimensions - 1; dim > 0; --dim)
  {
    coordinate[dim] = address / m_nDBlockSizes[dim];
    NMP_ASSERT(coordinate[dim] < m_sampleCountsPerDimension[dim]);
    address -= (coordinate[dim] * m_nDBlockSizes[dim]);
  }
  coordinate[0] = address;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NDMesh::isValidVertexCoordinate(
  const uint32_t* coordinate) const
{
  NMP_ASSERT(coordinate);
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    NMP_ASSERT(m_sampleCountsPerDimension[dim] > 1);
    if (coordinate[dim] >= m_sampleCountsPerDimension[dim])
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NDMesh::isValidCellCoordinate(
  const uint32_t* topLeftCoord) const
{
  NMP_ASSERT(topLeftCoord);
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    NMP_ASSERT(m_sampleCountsPerDimension[dim] > 1);
    uint32_t lastCount = m_sampleCountsPerDimension[dim] - 1;
    if (topLeftCoord[dim] >= lastCount)
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NDMesh::isNDCellBoundaryFacet(
  const uint32_t* topLeftCoord,
  uint32_t whichDim,
  uint32_t whichBoundary) const
{
  NMP_ASSERT(topLeftCoord);
  NMP_ASSERT(whichDim < m_numDimensions);
  NMP_ASSERT(whichBoundary <= 1); // 0 or 1
  NMP_ASSERT(m_sampleCountsPerDimension[whichDim] > 1);

  NMP_ASSERT(topLeftCoord[whichDim] < m_sampleCountsPerDimension[whichDim] - 1);
  uint32_t boundaryCoord = topLeftCoord[whichDim] + whichBoundary;
  if (boundaryCoord == 0 || boundaryCoord == m_sampleCountsPerDimension[whichDim] - 1)
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMesh::getControlComponentAABB(
  uint32_t whichCPComponent,
  uint32_t coordLeft,
  uint32_t coordRight,
  float& aabbMinValue,
  float& aabbMaxValue) const
{
  NMP_ASSERT(whichCPComponent < m_numDimensions);
  NMP_ASSERT(coordLeft < m_sampleCountsPerDimension[whichCPComponent]);
  NMP_ASSERT(coordRight < m_sampleCountsPerDimension[whichCPComponent]);

  float Vs = m_controlComponentStartValues[whichCPComponent];
  float Ve = m_controlComponentEndValues[whichCPComponent];
  float scale = (Ve - Vs) / (m_sampleCountsPerDimension[whichCPComponent] - 1);

  float leftValue = Vs + coordLeft * scale;
  float rightValue = Vs + coordRight * scale;

  aabbMinValue = NMP::minimum(leftValue, rightValue);
  aabbMaxValue = NMP::maximum(leftValue, rightValue);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMesh::getVertex(
  const uint32_t* coordinate,
  float* vertex) const
{
  uint32_t vertexSampleIndex = coordinateToAddress(coordinate);
  getVertex(vertexSampleIndex, vertex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMesh::getVertexComponents(
  const uint32_t* coordinate,
  uint32_t numSampleComponents,
  const uint32_t* whichSampleComponents,
  float* vertexComponents) const
{
  uint32_t vertexSampleIndex = coordinateToAddress(coordinate);
  getVertexComponents(
    vertexSampleIndex,
    numSampleComponents,
    whichSampleComponents,
    vertexComponents);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMesh::getNDCellVertexComponents(
  const uint32_t* topLeftCoord,
  uint32_t numSampleComponents,
  const uint32_t* whichSampleComponents,
  float* const* vertexComponents) const
{
  getNDCellVertexComponents(
    topLeftCoord,
    numSampleComponents,
    whichSampleComponents,
    m_scalesQuantisedToReal,
    m_offsetsQuantisedToReal,
    vertexComponents);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMesh::getNDCellFacetVertexComponents(
  const uint32_t* topLeftCoord,
  uint32_t whichDim,
  uint32_t whichBoundary,
  uint32_t numSampleComponents,
  const uint32_t* whichSampleComponents,
  float* const* vertexComponents) const
{
  getNDCellFacetVertexComponents(
    topLeftCoord,
    whichDim,
    whichBoundary,
    numSampleComponents,
    whichSampleComponents,
    m_scalesQuantisedToReal,
    m_offsetsQuantisedToReal,
    vertexComponents);
}

} // namespace ScatteredData

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NDMESH_H
//----------------------------------------------------------------------------------------------------------------------
