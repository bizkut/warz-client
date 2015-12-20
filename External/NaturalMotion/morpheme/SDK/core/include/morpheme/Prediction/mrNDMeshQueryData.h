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
#ifndef MR_NDMESH_QUERYDATA_H
#define MR_NDMESH_QUERYDATA_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "morpheme/Prediction/mrScatteredDataUtils.h"
#include "morpheme/Prediction/mrNDMesh.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

namespace ScatteredData
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::NDMeshQueryResult
/// \brief NDCell interpolation result data returned from NDMesh queries.
//----------------------------------------------------------------------------------------------------------------------
class NDMeshQueryResult
{
public:
  enum InterpolationType
  {
    kNDCell = 0,
    kNDCellFacet,
    kNDCellInvalid
  };

  NM_INLINE void init();

public:
  InterpolationType       m_interpolationType;
  uint32_t                m_topLeftCoord[SCATTERED_DATA_MAX_DIM];
  uint32_t                m_whichDim;
  uint32_t                m_whichBoundary;
  float                   m_interpolants[SCATTERED_DATA_MAX_DIM];
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshQueryResult::init()
{
  m_interpolationType = kNDCellInvalid;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::Projection
/// \brief Parameters for projection of outliers onto the sample space.
//----------------------------------------------------------------------------------------------------------------------
class Projection
{
public:
  /// \brief Which of the various kind of projection to use.
  enum Method
  {
    ProjectAlongDimension,
    ProjectThroughPoint,
    ProjectThroughSampleCentre
  };

public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inititialisation
  /// \brief  Functions to initialising the projection options
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  Projection() {}

  /// \brief Initialisation for ProjectAlongDimension type projection
  ///
  /// \param numVariablesSacrifice The number of different projection directions to try
  ///
  /// \param orderOfVariablesSacrifice The order in which to try directions for projection.
  ///   Note these are indices into the independentVars array, not into the vertex sample data.
  Projection(
    uint32_t numVariablesSacrifice,
    const uint32_t* orderOfVariablesSacrifice);

  /// \brief Initialisation for ProjectThroughPoint type projection
  ///
  /// \param numDimensions The number of dimensions for the query
  ///
  /// \param projectionCentre The array of values representing the projection centre.
  ///   Note these are components in the independentVars array, not in the vertex sample data.
  Projection(
    uint32_t numDimensions,
    const float* projectionCentre);

  /// \brief Initialisation for ProjectThroughSampleCentre type projection
  ///
  /// \param numDimensions The number of dimensions for the query
  Projection(
    uint32_t numDimensions);

  /// \brief Initialisation for ProjectAlongDimension type projection
  ///
  /// \param numVariablesSacrifice The number of different projection directions to try
  ///
  /// \param orderOfVariablesSacrifice The order in which to try directions for projection.
  ///   Note these are indices into the independentVars array, not into the vertex sample data.
  NM_INLINE void init(
    uint32_t numVariablesSacrifice,
    const uint32_t* orderOfVariablesSacrifice);

  /// \brief Initialisation for ProjectThroughPoint type projection
  ///
  /// \param numDimensions The number of dimensions for the query
  ///
  /// \param projectionCentre The array of values representing the projection centre.
  ///   Note these are components in the independentVars array, not in the vertex sample data.
  NM_INLINE void init(
    uint32_t numDimensions,
    const float* projectionCentre);

  /// \brief Initialisation for ProjectThroughSampleCentre type projection
  ///
  /// \param numDimensions The number of dimensions for the query
  NM_INLINE void init(
    uint32_t numDimensions);
  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the projection
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE Method getMethod() const { return m_method; }

  NM_INLINE uint32_t getNumComponents() const { return m_numComponents; }

  NM_INLINE const float* getProjectionCentre() const;

  NM_INLINE const uint32_t* getOrderOfVariablesSacrifice() const;
  NM_INLINE uint32_t getVariableToSacrifice(uint32_t i) const;
  //@}

protected:
  /// \brief The projection method
  Method    m_method;

  /// \brief The number of dimensions for the projection centre or the number of variables to sacrifice.
  uint32_t  m_numComponents;

  /// \brief For ProjectThroughPoint, the point.
  float     m_projectionCentre[SCATTERED_DATA_MAX_DIM];

  /// \brief For ProjectAlongDimension, allows user to define which values can vary in what priority order.
  uint32_t  m_orderOfVariablesSacrifice[SCATTERED_DATA_MAX_DIM];
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Projection::init(
  uint32_t numVariablesSacrifice,
  const uint32_t* orderOfVariablesSacrifice)
{
  NMP_ASSERT(numVariablesSacrifice <= SCATTERED_DATA_MAX_DIM);
  NMP_ASSERT(orderOfVariablesSacrifice);

  m_method = ProjectAlongDimension;
  m_numComponents = numVariablesSacrifice;
  for (uint32_t i = 0; i < numVariablesSacrifice; ++i)
  {
    NMP_ASSERT(orderOfVariablesSacrifice[i] < SCATTERED_DATA_MAX_DIM);
    m_orderOfVariablesSacrifice[i] = orderOfVariablesSacrifice[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Projection::init(
  uint32_t numDimensions,
  const float* projectionCentre)
{
  NMP_ASSERT(numDimensions <= SCATTERED_DATA_MAX_DIM);
  NMP_ASSERT(projectionCentre);

  m_method = ProjectThroughPoint;
  m_numComponents = numDimensions;
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    m_projectionCentre[dim] = projectionCentre[dim];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Projection::init(
  uint32_t numDimensions)
{
  NMP_ASSERT(numDimensions <= SCATTERED_DATA_MAX_DIM);
  m_method = ProjectThroughSampleCentre;
  m_numComponents = numDimensions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* Projection::getProjectionCentre() const
{
  NMP_ASSERT(m_method == ProjectThroughPoint);
  return m_projectionCentre;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* Projection::getOrderOfVariablesSacrifice() const
{
  NMP_ASSERT(m_method == ProjectAlongDimension);
  return m_orderOfVariablesSacrifice;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t Projection::getVariableToSacrifice(uint32_t i) const
{
  NMP_ASSERT(m_method == ProjectAlongDimension);
  NMP_ASSERT(i < m_numComponents);
  return m_orderOfVariablesSacrifice[i];
}


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::InterpolateResult
/// \brief State data for the interpolation of a query point within the NDMesh
//----------------------------------------------------------------------------------------------------------------------
class InterpolateResult
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inititialisation
  /// \brief  Functions to initialising the interpolation options
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Initialisation for ProjectAlongDimension type projection
  ///
  /// \param numDimensions The number of dimensions for the query
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param queryPoint An numDimensions array of values identifying the independent controlling components.
  ///
  /// \param enableExtrapolation A flag determining whether queries to the solver should extrapolate
  ///   the result if the query point lies outside the NDCell.
  void init(
    uint32_t        numDimensions,
    const uint32_t* queryPointComponentIndices,
    const float*    queryPoint,
    bool            enableExtrapolation);
  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the interpolation
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE uint32_t getNumDimensions() const { return m_numDimensions; }

  NM_INLINE uint32_t getNumCellVertices() const { return m_numCellVertices; }

  NM_INLINE const uint32_t* getQueryPointComponentIndices() const { return m_queryPointComponentIndices; }

  NM_INLINE const float* getQueryPoint() const { return m_queryPoint; }

  NM_INLINE float* const* getVertexComponents() { return m_vertexComponents; }

  NM_INLINE NDCellSolverData* getSolverData() { return &m_solverData; }
  //@}

protected:
  // Input data
  uint32_t          m_numDimensions;
  uint32_t          m_numCellVertices;
  const uint32_t*   m_queryPointComponentIndices;
  const float*      m_queryPoint;

  // NDCell vertex sample data
  float             m_vertexData[SCATTERED_DATA_MAX_CELL_VERTICES][SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
  float*            m_vertexComponents[SCATTERED_DATA_MAX_CELL_VERTICES];

  // Solver data
  NDCellSolverData  m_solverData;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::InterpolateResultQuery
/// \brief State data for the interpolation of a query point within the NDMesh
//----------------------------------------------------------------------------------------------------------------------
class InterpolateResultQuery : public InterpolateResult
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inititialisation
  /// \brief  Functions to initialising the interpolation options
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Initialisation for ProjectAlongDimension type projection
  ///
  /// \param nDMesh The source NDMesh to use for the query
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param queryPoint An numDimensions array of values identifying the independent controlling components.
  ///
  /// \param ndMeshQueryResult The NDCell coordinates and multi-linear weights for the interpolated result
  NM_INLINE void init(
    const NDMesh*   nDMesh,
    const uint32_t* queryPointComponentIndices,
    const float*    queryPoint,
    NDMeshQueryResult* ndMeshQueryResult);

  /// \brief Initialisation of the search map state data
  NM_INLINE void initState();
  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the interpolation
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE const NDMesh* getNDMesh() const { return m_nDMesh; }

  NM_INLINE const NDMeshQueryResult* getNDMeshQueryResult() const { return m_ndMeshQueryResult; }
  NM_INLINE NDMeshQueryResult* getNDMeshQueryResult() { return m_ndMeshQueryResult; }

  NM_INLINE uint32_t* getCoordinates() { return m_coordinates; }
  NM_INLINE uint32_t* getCellSizes() { return m_cellSizes; }
  //@}

protected:
  // Query data
  const NDMesh*       m_nDMesh;

  // Cell interpolation information
  NDMeshQueryResult*  m_ndMeshQueryResult;

  // State data for the NDCell search
  uint32_t            m_coordinates[SCATTERED_DATA_MAX_DIM];
  uint32_t            m_cellSizes[SCATTERED_DATA_MAX_DIM];
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void InterpolateResultQuery::init(
  const NDMesh*       nDMesh,
  const uint32_t*     queryPointComponentIndices,
  const float*        queryPoint,
  NDMeshQueryResult*  ndMeshQueryResult)
{
  NMP_ASSERT(ndMeshQueryResult);

  // Initialise the base data
  NMP_ASSERT(nDMesh);
  uint32_t numDimensions = nDMesh->getNumDimensions();
  InterpolateResult::init(
    numDimensions,
    queryPointComponentIndices,
    queryPoint,
    false);

  // Query data
  m_nDMesh = nDMesh;
  m_ndMeshQueryResult = ndMeshQueryResult;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void InterpolateResultQuery::initState()
{
  // State data for the NDCell search
  NMP_ASSERT(m_nDMesh);
  const uint32_t* sampleCountsPerDimension = m_nDMesh->getSampleCountsPerDimension();
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    m_coordinates[dim] = 0;
    m_cellSizes[dim] = sampleCountsPerDimension[dim] - 1;
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::ProjectionBasis
/// \brief An orthonormal set of basis vectors for projection ray queries
//----------------------------------------------------------------------------------------------------------------------
class ProjectionBasis
{
public:
  enum ProjectionBasisType
  {
    kProjectAlongOneDimension,
    kProjectInDirection
  };

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inititialisation
  /// \brief  Functions to initialising the projection basis
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief This variant initialises the projection basis for projection along one
  /// of the principal axis directions. It also assumes that the origin for the
  /// projection basis lies at zero.
  ///
  /// \param numDimensions The number of dimensions for the query
  ///
  /// \param whichVariableToCompromise An index into the queryPoint array identifying which independent
  ///   variable to sacrifice for projection back onto the NDMesh sample space.
  void initAlongDimension(
    uint32_t        numDimensions,
    uint32_t        whichVariableToCompromise);

  /// \brief This variant initialises the projection basis for projection along one
  /// of the principal axis directions. The origin for the projection basis is set to
  /// the query point.
  ///
  /// \param numDimensions The number of dimensions for the query
  ///
  /// \param queryPoint Used as the origin for the projection basis
  ///
  /// \param whichVariableToCompromise An index into the queryPoint array identifying which independent
  ///   variable to sacrifice for projection back onto the NDMesh sample space.
  void initAlongDimension(
    uint32_t        numDimensions,
    const float*    queryPoint,
    uint32_t        whichVariableToCompromise);

  /// \brief This variant only initialises the principal projection direction. It does
  /// not recover the complementary set of basis directions.
  ///
  /// \param numDimensions The number of dimensions for the query
  ///
  /// \param queryPoint Used as the origin for the projection basis
  ///
  /// \param projectionCentre A point which the projection direction must pass through
  void initThroughPointProjectionDirectionOnly(
    uint32_t        numDimensions,
    const float*    queryPoint,
    const float*    projectionCentre);

  /// \brief This variant initialises the full set of projection basis directions.
  ///
  /// \param numDimensions The number of dimensions for the query
  ///
  /// \param queryPoint Used as the origin for the projection basis
  ///
  /// \param projectionCentre A point which the projection direction must pass through
  void initThroughPoint(
    uint32_t        numDimensions,
    const float*    queryPoint,
    const float*    projectionCentre);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Projection query
  /// \brief  Functions for transforming data according to the projection basis
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Transforms a buffer of vertices by the projection basis
  ///
  /// \param numVertices The number of vertices to transform according to the projection basis
  ///
  /// \param inputVertexComponents The input vertex buffer
  ///
  /// \param outputVertexComponents The output vertex buffer (can be the same as inputVertexComponents)
  void transform(
    uint32_t numVertices,
    const float* const* inputVertexComponents,
    float* const* outputVertexComponents
    ) const;

  /// \brief Calculates the component distance of the vertex from the projection origin
  ///   along the direction of projection.
  ///
  /// \param vertex The query point
  float distanceAlongProjectionDirection(
    const float* vertex
    ) const;
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the projection basis
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  NM_INLINE uint32_t getNumDimensions() const { return m_numDimensions; }

  NM_INLINE uint32_t getNumRayComponents() const { return m_numRayComponents; }
  NM_INLINE const uint32_t* getRayComponentOrder() const { return m_rayComponentOrder; }

  NM_INLINE uint32_t getWhichVariableToCompromise() const;

  NM_INLINE const float* getProjectionOrigin() const { return m_projectionOrigin; }
  NM_INLINE const float* getProjectionDirection() const { return m_projectionBasis[m_numDimensions - 1]; }
  NM_INLINE const float* getProjectionBasisVector(uint32_t index) const;
  //@}

protected:
  // General information
  ProjectionBasisType m_projectionBasisType;
  uint32_t            m_numDimensions;

  // The order of the principal ray vector components to test against the AABBs.
  // i.e. if the query ray has direction [0, 1, 0] then any of the zero components
  // do not contribute to the intersection test. We re-order the components of the
  // vector based on their magnitude, and during the query only process the non-zero entries.
  uint32_t            m_numRayComponents;
  uint32_t            m_rayComponentOrder[SCATTERED_DATA_MAX_DIM];

  // The projection origin. i.e the query point
  float               m_projectionOrigin[SCATTERED_DATA_MAX_DIM];

  // Projection basis: The first vector represents the projection direction,
  // while the remaining vectors are the orthogonal complement (Used for projection
  // through point).
  float               m_projectionDist; // The distance from the query point to the projection centre
  float               m_projectionBasisData[SCATTERED_DATA_MAX_DIM][SCATTERED_DATA_MAX_DIM];
  float*              m_projectionBasis[SCATTERED_DATA_MAX_DIM];
};


//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t ProjectionBasis::getWhichVariableToCompromise() const
{
  NMP_ASSERT_MSG(
    m_projectionBasisType == kProjectAlongOneDimension,
    "whichVariableToCompromise is only available for projectAlongDimension queries");
  NMP_ASSERT(m_numRayComponents < m_numDimensions);
  return m_rayComponentOrder[m_numRayComponents];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* ProjectionBasis::getProjectionBasisVector(uint32_t index) const
{
  NMP_ASSERT(index < m_numDimensions);
  return m_projectionBasis[index];
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::ProjectRayResult
/// \brief State data for the projection of a query point into the NDMesh
//----------------------------------------------------------------------------------------------------------------------
class ProjectRayResult
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inititialisation
  /// \brief  Functions to initialising the projection options
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Initialises the project ray result data
  ///
  /// \param numDimensions The number of dimensions for the query
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param enableExtrapolation A flag determining whether queries to the solver should extrapolate
  ///   the result if the query point lies outside the NDCell facet.
  void init(
    uint32_t        numDimensions,
    const uint32_t* queryPointComponentIndices,
    bool            enableExtrapolation);
  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Projection query
  /// \brief  Functions for querying data according to the projection basis
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Computes the AABB of the transformed vertex data in the basis directions
  /// complementary to the projection direction, then checks to see if the zero point
  /// (i.e. the projection origin) lies inside this AABB.
  NM_INLINE bool aabbFromTransformedVerticesContainsZeroPoint() const;
  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the projection result data
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE uint32_t getNumDimensions() const { return m_numDimensions; }

  NM_INLINE uint32_t getNumFacetVertices() const { return m_numFacetVertices; }

  NM_INLINE const uint32_t* getQueryPointComponentIndices() const { return m_queryPointComponentIndices; }

  NM_INLINE const float* getZeroVector() const { return m_zeroVector; }

  NM_INLINE float* const* getFacetVertexComponents() { return m_facetVertexComponents; }
  NM_INLINE float* const* getTransformedFacetVertexComponents() { return m_transformedFacetVertexComponents; }

  NM_INLINE NDCellSolverData* getSolverData() { return &m_solverData; }
  //@}

protected:
  // Input data
  uint32_t          m_numDimensions;
  uint32_t          m_numFacetVertices;
  const uint32_t*   m_queryPointComponentIndices;

  // Intermediate data
  float             m_zeroVector[SCATTERED_DATA_MAX_DIM];

  // The NDCell facet vertex sample data
  float             m_facetVertexData[SCATTERED_DATA_MAX_CELLFACET_VERTICES][SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
  float*            m_facetVertexComponents[SCATTERED_DATA_MAX_CELLFACET_VERTICES];

  // The NDCell facet vertex sample data transformed into the projection basis frame
  float             m_transformedFacetVertexData[SCATTERED_DATA_MAX_CELLFACET_VERTICES][SCATTERED_DATA_MAX_DIM];
  float*            m_transformedFacetVertexComponents[SCATTERED_DATA_MAX_CELLFACET_VERTICES];

  // Solver data
  NDCellSolverData  m_solverData;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool ProjectRayResult::aabbFromTransformedVerticesContainsZeroPoint() const
{
  uint32_t M = m_numDimensions - 1;
  for (uint32_t dim = 0; dim < M; ++dim)
  {
    // Check if there are any projected vertex samples left of the query point (zero)
    bool status = true;
    for (uint32_t vertex = 0; vertex < m_numFacetVertices; ++vertex)
    {
      if (m_transformedFacetVertexComponents[vertex][dim] < gAABBTol)
      {
        status = false;
        break;
      }
    }
    if (status)
    {
      return false;
    }

    // Check if there are any projected vertex samples right of the query point (zero)
    status = true;
    for (uint32_t vertex = 0; vertex < m_numFacetVertices; ++vertex)
    {
      if (m_transformedFacetVertexComponents[vertex][dim] > -gAABBTol)
      {
        status = false;
        break;
      }
    }
    if (status)
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Callback functions for projection
//----------------------------------------------------------------------------------------------------------------------
class ProjectRayResultQuery;

typedef bool (*projectRayPreUpdateFn)(
  ProjectRayResultQuery* target);

typedef void (*projectRayPostUpdateFn)(
  ProjectRayResultQuery* target,
  float dist,
  const uint32_t* topLeftCoord,
  uint32_t whichDim,
  uint32_t whichBoundary,
  const float* interpolants,
  const float* projectedPoint);

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::ProjectRayResultQuery
/// \brief State data for the projection of a query point into the NDMesh
//----------------------------------------------------------------------------------------------------------------------
class ProjectRayResultQuery : public ProjectRayResult
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inititialisation
  /// \brief  Functions to initialising the projection query result data
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Initialises the project ray result query data
  ///
  /// \param nDMesh The source NDMesh to use for the query
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param preUpdateFn the user defined pre-update function.
  ///
  /// \param postUpdateFn the user defined post-update function
  NM_INLINE void init(
    const NDMesh*   nDMesh,
    const uint32_t* queryPointComponentIndices,
    projectRayPreUpdateFn preUpdateFn,
    projectRayPostUpdateFn postUpdateFn);

  /// \brief Initialisation of the search map state data
  NM_INLINE void initState();
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Update functions
  /// \brief  Functions for updating the distance metrics
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE bool preUpdate();

  NM_INLINE void postUpdate(
    float           dist,
    const uint32_t* topLeftCoord,
    uint32_t        whichDim,
    uint32_t        whichBoundary,
    const float*    interpolants,
    const float*    projectedPoint);
  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the state data
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE const NDMesh* getNDMesh() const { return m_nDMesh; }

  NM_INLINE const uint32_t* getCoordinates() const { return m_coordinates; }
  NM_INLINE uint32_t* getCoordinates() { return m_coordinates; }

  NM_INLINE const uint32_t* getCellSizes() const { return m_cellSizes; }
  NM_INLINE uint32_t* getCellSizes() { return m_cellSizes; }
  //@}

protected:
  // Query data
  const NDMesh*           m_nDMesh;

  // State data for the APSearchMap
  uint32_t                m_coordinates[SCATTERED_DATA_MAX_DIM];
  uint32_t                m_cellSizes[SCATTERED_DATA_MAX_DIM];

  // Callback function
  projectRayPreUpdateFn   m_preUpdateFn;
  projectRayPostUpdateFn  m_postUpdateFn;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ProjectRayResultQuery::init(
  const NDMesh*   nDMesh,
  const uint32_t* queryPointComponentIndices,
  projectRayPreUpdateFn preUpdateFn,
  projectRayPostUpdateFn postUpdateFn)
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(preUpdateFn);
  NMP_ASSERT(postUpdateFn);

  // Initialise the base data
  ProjectRayResult::init(
    nDMesh->getNumDimensions(),
    queryPointComponentIndices,
    false);

  // Query data
  m_nDMesh = nDMesh;
  m_preUpdateFn = preUpdateFn;
  m_postUpdateFn = postUpdateFn;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ProjectRayResultQuery::initState()
{
  // State data for the NDCell search
  NMP_ASSERT(m_nDMesh);
  const uint32_t* sampleCountsPerDimension = m_nDMesh->getSampleCountsPerDimension();
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    m_coordinates[dim] = 0;
    m_cellSizes[dim] = sampleCountsPerDimension[dim] - 1;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool ProjectRayResultQuery::preUpdate()
{
  NMP_ASSERT(m_preUpdateFn);
  return m_preUpdateFn(this);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ProjectRayResultQuery::postUpdate(
  float dist,
  const uint32_t* topLeftCoord,
  uint32_t whichDim,
  uint32_t whichBoundary,
  const float* interpolants,
  const float* projectedPoint)
{
  NMP_ASSERT(m_postUpdateFn);
  m_postUpdateFn(
    this,
    dist,
    topLeftCoord,
    whichDim,
    whichBoundary,
    interpolants,
    projectedPoint);
}


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::ProjectRayResultClosest
/// \brief Distance metric data for the projection of a query point into the NDMesh. The
/// query metric calculates the closest projected query point that projects onto the NDMesh.
//----------------------------------------------------------------------------------------------------------------------
class ProjectRayResultClosest : public ProjectRayResultQuery
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inititialisation
  /// \brief  Functions to initialising the distance metric data
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Initialises the project ray result query data
  ///
  /// \param nDMesh The source NDMesh to use for the query
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param ndMeshQueryResult The query result data for the projection onto the NDCell facet.
  NM_INLINE void initClosest(
    const NDMesh*       nDMesh,
    const uint32_t*     queryPointComponentIndices,
    NDMeshQueryResult*  ndMeshQueryResult);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Update functions
  /// \brief  Functions for updating the distance metrics
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  static bool preUpdateClosest(
    ProjectRayResultQuery* target);

  static void postUpdateClosest(
    ProjectRayResultQuery* target,
    float             dist,
    const uint32_t*   topLeftCoord,
    uint32_t          whichDim,
    uint32_t          whichBoundary,
    const float*      interpolants,
    const float*      projectedPoint);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the state data
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE float* getProjectedPoint() { return m_projectedPoint; }
  NM_INLINE void getProjectedPoint(float* components) const;
  NM_INLINE uint32_t getCount() const { return m_count; }

  NM_INLINE const NDMeshQueryResult* getNDMeshQueryResult() const { return m_ndMeshQueryResult; }
  NM_INLINE NDMeshQueryResult* getNDMeshQueryResult() { return m_ndMeshQueryResult; }
  //@}

protected:
  // Projected query point
  float               m_projectedPoint[SCATTERED_DATA_MAX_DIM];
  float               m_bestDistance;
  uint32_t            m_count;

  // Closest projected point cell facet information
  NDMeshQueryResult*  m_ndMeshQueryResult;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ProjectRayResultClosest::initClosest(
  const NDMesh*       nDMesh,
  const uint32_t*     queryPointComponentIndices,
  NDMeshQueryResult*  ndMeshQueryResult)
{
  NMP_ASSERT(ndMeshQueryResult);

  // Initialise the base class
  ProjectRayResultQuery::init(
    nDMesh,
    queryPointComponentIndices,
    preUpdateClosest,
    postUpdateClosest);

  // Initialisation
  m_bestDistance = FLT_MAX;
  m_count = 0;
  m_ndMeshQueryResult = ndMeshQueryResult;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ProjectRayResultClosest::getProjectedPoint(float* components) const
{
  NMP_ASSERT(components);
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    components[i] = m_projectedPoint[i];
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::ProjectRayResultExtremes
/// \brief Distance metric data for the projection of a query point into the NDMesh. The
/// query metric calculates the min and max extremes of all projected query points that project
/// onto the NDMesh.
//----------------------------------------------------------------------------------------------------------------------
class ProjectRayResultExtremes : public ProjectRayResultQuery
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inititialisation
  /// \brief  Functions to initialising the distance metric data
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Initialises the project ray result query data
  ///
  /// \param nDMesh The source NDMesh to use for the query
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param ndMeshQueryResultRear The rear query result data for the projection onto the NDCell facet.
  ///
  /// \param ndMeshQueryResultFore The fore query result data for the projection onto the NDCell facet.
  NM_INLINE void initExtremes(
    const NDMesh*       nDMesh,
    const uint32_t*     queryPointComponentIndices,
    NDMeshQueryResult*  ndMeshQueryResultRear,
    NDMeshQueryResult*  ndMeshQueryResultFore);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Update functions
  /// \brief  Functions for updating the distance metrics
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  static bool preUpdateExtremes(
    ProjectRayResultQuery* target);

  static void postUpdateExtremes(
    ProjectRayResultQuery* target,
    float             dist,
    const uint32_t*   topLeftCoord,
    uint32_t          whichDim,
    uint32_t          whichBoundary,
    const float*      interpolants,
    const float*      projectedPoint);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the state data
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE const float* getProjectedPointRear() const { return m_projectedPointRear; }
  NM_INLINE const float* getProjectedPointFore() const { return m_projectedPointFore; }
  NM_INLINE uint32_t getCount() const { return m_count; }

  NM_INLINE const NDMeshQueryResult* getNDMeshQueryResultRear() const { return m_ndMeshQueryResultRear; }
  NM_INLINE NDMeshQueryResult* getNDMeshQueryResultRear() { return m_ndMeshQueryResultRear; }

  NM_INLINE const NDMeshQueryResult* getNDMeshQueryResultFore() const { return m_ndMeshQueryResultFore; }
  NM_INLINE NDMeshQueryResult* getNDMeshQueryResultFore() { return m_ndMeshQueryResultFore; }
  //@}

protected:
  // Projected query point
  float               m_projectedPointRear[SCATTERED_DATA_MAX_DIM];
  float               m_projectedPointFore[SCATTERED_DATA_MAX_DIM];
  float               m_bestDistanceRear;
  float               m_bestDistanceFore;
  uint32_t            m_count;

  // Projected point cell facet information
  NDMeshQueryResult*  m_ndMeshQueryResultRear;
  NDMeshQueryResult*  m_ndMeshQueryResultFore;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ProjectRayResultExtremes::initExtremes(
  const NDMesh*       nDMesh,
  const uint32_t*     queryPointComponentIndices,
  NDMeshQueryResult*  ndMeshQueryResultRear,
  NDMeshQueryResult*  ndMeshQueryResultFore)
{
  NMP_ASSERT(ndMeshQueryResultRear);
  NMP_ASSERT(ndMeshQueryResultFore);

  // Initialise the base class
  ProjectRayResultQuery::init(
    nDMesh,
    queryPointComponentIndices,
    preUpdateExtremes,
    postUpdateExtremes);

  // Initialisation
  m_bestDistanceRear = FLT_MAX;
  m_bestDistanceFore = -FLT_MAX;
  m_count = 0;
  m_ndMeshQueryResultRear = ndMeshQueryResultRear;
  m_ndMeshQueryResultFore = ndMeshQueryResultFore;
}


//----------------------------------------------------------------------------------------------------------------------
// AABB Query state data
//----------------------------------------------------------------------------------------------------------------------
struct NDCellEntry
{
  uint32_t  m_nodeID;                                 ///< The cell node id for the entry
  uint32_t  m_topLeftCoord[SCATTERED_DATA_MAX_DIM];   ///< The coordinate of the cell entry
};

//----------------------------------------------------------------------------------------------------------------------
struct NDCellFacetEntry
{
  uint32_t  m_nodeID;                                 ///< The cell node id for the entry
  uint32_t  m_topLeftCoord[SCATTERED_DATA_MAX_DIM];   ///< The coordinate of the cell entry
  uint32_t  m_whichDimFlags;                          ///< A bitmask determining which dimensions of the cell are valid
  uint32_t  m_whichBoundaryFlags;                     ///< A bitmask determining which boundaries of the cell are valid
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::AABBIntersectionResult
/// \brief State data for the AABB intersection query within the NDMesh
//----------------------------------------------------------------------------------------------------------------------
class AABBIntersectionResult
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inititialisation
  /// \brief  Functions to initialising the AABB intersection data
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Initialises the AABB result data
  ///
  /// \param nDMesh The source NDMesh to use for the query
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param aabbComponentMinVals The minimum components of the query AABB
  ///
  /// \param aabbComponentMaxVals The maximum components of the query AABB
  ///
  /// \param maxNumNodeEntries The maximum number of buffer entries
  ///
  /// \param nodeEntries The entry buffer used for setting results
  void init(
    const NDMesh*     nDMesh,
    const uint32_t*   queryPointComponentIndices,
    const float*      aabbComponentMinVals,
    const float*      aabbComponentMaxVals,
    uint32_t          maxNumNodeEntries,
    NDCellEntry*      nodeEntries);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Query functions
  /// \brief  Functions for calculating intersection information
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Calculates if the NDCell identified by the coordinates intersects the query AABB
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND cell
  NM_INLINE bool queryAABBIntersectsNDCell(
    const uint32_t* topLeftCoord);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the state data
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE void addEntry(
    uint32_t nodeID,
    const uint32_t* topLeftCoord);

  NM_INLINE const NDMesh* getNDMesh() const { return m_nDMesh; }

  NM_INLINE const uint32_t* getQueryPointComponentIndices() const { return m_queryPointComponentIndices; }
  NM_INLINE const float* getQueryAABBMinVals() const { return m_queryAABBMinVals; }
  NM_INLINE const float* getQueryAABBMaxVals() const { return m_queryAABBMaxVals; }

  NM_INLINE uint32_t getNodeEntryCount() const { return m_nodeEntryCount; }
  NM_INLINE const NDCellEntry* getNodeEntries() const { return m_nodeEntries; }

  NM_INLINE const uint32_t* getCoordinates() const { return m_coordinates; }
  NM_INLINE uint32_t* getCoordinates() { return m_coordinates; }

  NM_INLINE const uint32_t* getCellSizes() const { return m_cellSizes; }
  NM_INLINE uint32_t* getCellSizes() { return m_cellSizes; }

  NM_INLINE float* const* getVertexComponents() { return m_vertexComponents; }

  NM_INLINE float* const* getVertexComponentsTemp() { return m_vertexComponentsTemp; }
  //@}

protected:
  // Query data
  const NDMesh*     m_nDMesh;
  const uint32_t*   m_queryPointComponentIndices;
  const float*      m_queryAABBMinVals;
  const float*      m_queryAABBMaxVals;

  // Result data
  uint32_t          m_maxNumNodeEntries;
  uint32_t          m_nodeEntryCount;
  NDCellEntry*      m_nodeEntries;

  // NDCell coordinates
  uint32_t          m_coordinates[SCATTERED_DATA_MAX_DIM];
  uint32_t          m_cellSizes[SCATTERED_DATA_MAX_DIM];

  // The NDCell vertex sample data
  float             m_vertexData[SCATTERED_DATA_MAX_CELL_VERTICES][SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
  float*            m_vertexComponents[SCATTERED_DATA_MAX_CELL_VERTICES];

  // Temporary data for AABB intersection
  float             m_vertexDataTemp[SCATTERED_DATA_MAX_CELL_VERTICES][SCATTERED_DATA_MAX_DIM];
  float*            m_vertexComponentsTemp[SCATTERED_DATA_MAX_CELL_VERTICES];
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool AABBIntersectionResult::queryAABBIntersectsNDCell(
  const uint32_t* topLeftCoord)
{
  uint32_t numDimensions = m_nDMesh->getNumDimensions();

  // Retrieve the input NDCell vertex sample data
  m_nDMesh->getNDCellVertexComponents(
    topLeftCoord,
    numDimensions,
    m_queryPointComponentIndices,
    m_nDMesh->getScalesQuantisedToNorm(),
    m_nDMesh->getOffsetsQuantisedToNorm(),
    m_vertexComponents);

  // Check for intersection
  return ScatteredData::aabbIntersectsNDCell(
    numDimensions,
    m_queryAABBMinVals,
    m_queryAABBMaxVals,
    m_vertexComponents,
    m_vertexComponentsTemp);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AABBIntersectionResult::addEntry(
  uint32_t nodeID,
  const uint32_t* topLeftCoord)
{
  NMP_ASSERT_MSG(
    m_nodeEntryCount < m_maxNumNodeEntries,
    "Insufficient space in the intersection result buffer to append a new entry. Increase the number of samples to decrease the number of likely intersections.");

  NMP_ASSERT(m_nDMesh->isValidCellCoordinate(topLeftCoord));
  uint32_t numDimensions = m_nDMesh->getNumDimensions();

  if (m_nodeEntryCount < m_maxNumNodeEntries)
  {
    NDCellEntry& entry = m_nodeEntries[m_nodeEntryCount];
    entry.m_nodeID = nodeID;
    for (uint32_t i = 0; i < numDimensions; ++i)
    {
      entry.m_topLeftCoord[i] = topLeftCoord[i];
    }
    m_nodeEntryCount++;
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::AABBProjectionResult
/// \brief State data for the AABB projection query within the NDMesh
//----------------------------------------------------------------------------------------------------------------------
class AABBProjectionResult
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inititialisation
  /// \brief  Functions to initialising the AABB intersection data
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Initialises the AABB result data for projection along dimension type queries
  ///
  /// \param nDMesh The source NDMesh to use for the query
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param whichAABBDim Which dim of the AABB is fixed to form the (N-1)d hyper-plane.
  ///   This is also the dimension that is compromised for projection.
  ///
  /// \param aabbComponentMinVals The minimum components of the query AABB
  ///
  /// \param aabbComponentMaxVals The maximum components of the query AABB
  ///
  /// \param maxNumNodeEntries The maximum number of buffer entries
  ///
  /// \param nodeEntries The entry buffer used for setting results
  void initAlongDimension(
    const NDMesh*     nDMesh,
    const uint32_t*   queryPointComponentIndices,
    uint32_t          whichAABBDim,
    const float*      aabbComponentMinVals,
    const float*      aabbComponentMaxVals,
    uint32_t          maxNumNodeEntries,
    NDCellFacetEntry* nodeEntries);

  /// \brief Initialises the AABB result data for projection through point type queries
  ///
  /// \param nDMesh The source NDMesh to use for the query
  ///
  /// \param projectionCentre The point through which all ray-casts originate
  ///
  /// \param queryPointComponentIndices The components of the vertex samples used as the query point
  ///
  /// \param whichAABBDim Which dim of the AABB is fixed to form the (N-1)d hyper-plane.
  ///
  /// \param aabbComponentMinVals The minimum components of the query AABB
  ///
  /// \param aabbComponentMaxVals The maximum components of the query AABB
  ///
  /// \param maxNumNodeEntries The maximum number of buffer entries
  ///
  /// \param nodeEntries The entry buffer used for setting results
  void initThroughPoint(
    const NDMesh*     nDMesh,
    const float*      projectionCentre,
    const uint32_t*   queryPointComponentIndices,
    uint32_t          whichAABBDim,
    const float*      aabbComponentMinVals,
    const float*      aabbComponentMaxVals,
    uint32_t          maxNumNodeEntries,
    NDCellFacetEntry* nodeEntries);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Query functions
  /// \brief  Functions for calculating intersection information
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Calculates if the NDCell facet identified by the coordinates intersects the
  /// query AABB via projection
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND cell
  ///
  /// \param whichDim The dimension of the NDCell identifying the cell facet
  ///
  /// \param whichBoundary Index specifying the near-side or far-side facet for that dimension
  bool queryAABBIntersectsNDCellFacet(
    const uint32_t* topLeftCoord,
    uint32_t whichDim,
    uint32_t whichBoundary);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the state data
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE void addEntry(
    uint32_t nodeID,
    const uint32_t* topLeftCoord,
    uint32_t whichDimFlags,
    uint32_t whichBoundaryFlags);

  NM_INLINE const NDMesh* getNDMesh() const { return m_nDMesh; }

  NM_INLINE const uint32_t* getQueryPointComponentIndices() const { return m_queryPointComponentIndices; }
  NM_INLINE const float* getQueryAABBMinVals() const { return m_queryAABBMinVals; }
  NM_INLINE const float* getQueryAABBMaxVals() const { return m_queryAABBMaxVals; }

  NM_INLINE uint32_t getWhichAABBDim() const { return m_whichAABBDim; }

  NM_INLINE uint32_t getNodeEntryCount() const { return m_nodeEntryCount; }

  NM_INLINE const NDCellFacetEntry* getNodeEntries() const { return m_nodeEntries; }

  NM_INLINE const uint32_t* getPermutationOrder() const { return m_tmP; }

  NM_INLINE const uint32_t* getCoordinates() const { return m_coordinates; }
  NM_INLINE uint32_t* getCoordinates() { return m_coordinates; }

  NM_INLINE const uint32_t* getCellSizes() const { return m_cellSizes; }
  NM_INLINE uint32_t* getCellSizes() { return m_cellSizes; }

  NM_INLINE const float* getMidPointInterpolants() const { return m_midPointInterpolants; }
  //@}

protected:
  // Query data
  const NDMesh*     m_nDMesh;
  const uint32_t*   m_queryPointComponentIndices;
  const float*      m_queryAABBMinVals;
  const float*      m_queryAABBMaxVals;
  uint32_t          m_whichAABBDim;                 ///< Which dim of the AABB components is compromised

  // Result data
  uint32_t          m_maxNumNodeEntries;
  uint32_t          m_nodeEntryCount;
  NDCellFacetEntry* m_nodeEntries;

  // Intermediate data
  float             m_tmT[SCATTERED_DATA_MAX_DIM];  ///< Transform that moves the projection centre to the origin
  uint32_t          m_tmP[SCATTERED_DATA_MAX_DIM];  ///< Transform that re-orders the vertex components so that the
                                                    ///< dimension for compromise is moved to the last element
  float             m_tmK[3];                       ///< The projection coefficients (3-vector) [Kf, Kp, Kw] that describe
                                                    ///< how each vertex is projected onto the AABB hyper-plane.
  float             m_queryAABBMinValsAdj[SCATTERED_DATA_MAX_DIM]; ///< Permuted AABB min values
  float             m_queryAABBMaxValsAdj[SCATTERED_DATA_MAX_DIM]; ///< Permuted AABB max values
  float             m_midPointInterpolants[SCATTERED_DATA_MAX_DIM];

  // NDCell coordinates
  uint32_t          m_coordinates[SCATTERED_DATA_MAX_DIM];
  uint32_t          m_cellSizes[SCATTERED_DATA_MAX_DIM];

  // The NDCell facet vertex sample data
  float             m_facetVertexData[SCATTERED_DATA_MAX_CELLFACET_VERTICES][SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
  float*            m_facetVertexComponents[SCATTERED_DATA_MAX_CELLFACET_VERTICES];

  // The permuted facet vertex sample data
  float             m_facetVertexDataAdj[SCATTERED_DATA_MAX_CELLFACET_VERTICES][SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
  float*            m_facetVertexComponentsAdj[SCATTERED_DATA_MAX_CELLFACET_VERTICES];

  // Temporary data for AABB intersection
  float             m_facetVertexDataTemp[SCATTERED_DATA_MAX_CELLFACET_VERTICES][SCATTERED_DATA_MAX_DIM];
  float*            m_facetVertexComponentsTemp[SCATTERED_DATA_MAX_CELLFACET_VERTICES];
  bool              m_facetVertexValidTemp[SCATTERED_DATA_MAX_CELLFACET_VERTICES];
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AABBProjectionResult::addEntry(
  uint32_t nodeID,
  const uint32_t* topLeftCoord,
  uint32_t whichDimFlags,
  uint32_t whichBoundaryFlags)
{
  NMP_ASSERT_MSG(
    m_nodeEntryCount < m_maxNumNodeEntries,
    "Insufficient space in the intersection result buffer to append a new entry. Increase the number of samples to decrease the number of likely intersections.");

  NMP_ASSERT(m_nDMesh->isValidCellCoordinate(topLeftCoord));
  NMP_ASSERT(whichDimFlags != 0);
  NMP_ASSERT(whichDimFlags < (1 << SCATTERED_DATA_MAX_DIM));
  NMP_ASSERT(whichBoundaryFlags < (1 << SCATTERED_DATA_MAX_DIM));
  uint32_t numDimensions = m_nDMesh->getNumDimensions();

  if (m_nodeEntryCount < m_maxNumNodeEntries)
  {
    NDCellFacetEntry& entry = m_nodeEntries[m_nodeEntryCount];
    entry.m_nodeID = nodeID;
    for (uint32_t i = 0; i < numDimensions; ++i)
    {
      entry.m_topLeftCoord[i] = topLeftCoord[i];
    }
    entry.m_whichDimFlags = whichDimFlags;
    entry.m_whichBoundaryFlags = whichBoundaryFlags;

    m_nodeEntryCount++;
  }
}

} // namespace ScatteredData

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NDMESH_QUERYDATA_H
//----------------------------------------------------------------------------------------------------------------------
