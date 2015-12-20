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
#ifndef MR_SCATTERED_DATA_UTILS_H
#define MR_SCATTERED_DATA_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMMathUtils.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Profiling
#define NM_PREDICTION_PROFILINGx
#if defined(PROFILE) && defined(NM_PREDICTION_PROFILING)
 #if defined(NM_HOST_X360)
  #define PREDICTION_PROFILE_BEGIN(name) \
    PIXBeginNamedEvent(0, name); \
    NM_BEGIN_PROFILING(name)
  #define PREDICTION_PROFILE_BEGIN_CONTEXT(name) \
    PIXBeginNamedEvent(0, name); \
    NM_CONTEXT_PROFILING(name)
  #define PREDICTION_PROFILE_END() \
    PIXEndNamedEvent(); \
    NM_END_PROFILING()
 #elif defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  #define PREDICTION_PROFILE_BEGIN(name) NM_BEGIN_PROFILING(name)
  #define PREDICTION_PROFILE_BEGIN_CONTEXT(name) NM_CONTEXT_PROFILING(name)
  #define PREDICTION_PROFILE_END() NM_END_PROFILING()
 #endif
#else
 #define PREDICTION_PROFILE_BEGIN(name) {}
 #define PREDICTION_PROFILE_BEGIN_CONTEXT(name) {}
 #define PREDICTION_PROFILE_END() {}
#endif


namespace MR
{

namespace ScatteredData
{

//----------------------------------------------------------------------------------------------------------------------
// Tolerances
static const float gEps = 1e-6f;                // Floating point precision tolerance
static const float gAbsCellTol = 1e-3f;         // The grid space cell tolerance
static const float gMinCellTol = -1e-3f;        // Min-cell point tolerance [0 : 1]
static const float gMaxCellTol = 1.0f + 1e-3f;  // Max-cell point tolerance [0 : 1]
static const float gMidCellTol = 0.5f + 1e-3f;  // Mid-cell point tolerance [0 : 0.5]
static const float gLambda = 1.0f + 0.001f;     // Smoothing factor for computing the search direction
static const float gAABBTol = 1e-4f;            // Tolerance for checking if a point lies within the AABB

//----------------------------------------------------------------------------------------------------------------------
/// \name Array sizes
/// Allow us to define fixed-size arrays on the stack despite the number of dimensions being variable - use of
/// alloca is ruled out on Vita. Increase these numbers if more dimensions or sample components are likely.
/// @{
#define SCATTERED_DATA_MAX_DIM 5
#define SCATTERED_DATA_MAX_CELL_VERTICES (1 << SCATTERED_DATA_MAX_DIM)
#define SCATTERED_DATA_MAX_CELLFACET_VERTICES (1 << (SCATTERED_DATA_MAX_DIM - 1))
#define SCATTERED_DATA_MAX_SAMPLE_COMPONENTS 16

// Vector size for linear algebra
#define SCATTERED_DATA_VECTOR_SIZE 8
NM_ASSERT_COMPILE_TIME(SCATTERED_DATA_VECTOR_SIZE >= SCATTERED_DATA_MAX_DIM);
/// @}

//----------------------------------------------------------------------------------------------------------------------
// Axis-Aligned Bounding Box functions
//----------------------------------------------------------------------------------------------------------------------
void aabbFromVertices(
  uint32_t numDimensions,         ///< IN: the dimensionality of the AABB
  const float* const* vertices,   ///< IN: the set of working vertices to test
  float* aabbMinVals,             ///< OUT: the AABB min values
  float* aabbMaxVals              ///< OUT: the AABB max values
  );

//----------------------------------------------------------------------------------------------------------------------
bool aabbFromVertices(
  uint32_t numDimensions,         // IN: the dimensionality of the AABB
  const float* const* vertices,   // IN: the set of working vertices to test
  const bool* valid,              // IN: the set of working valid vertices
  float* aabbMinVals,             // OUT: the AABB min values
  float* aabbMaxVals              // OUT: the AABB max values
  );

//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsAnotherAABB(
  uint32_t numDimensions, ///< IN: the dimensionality of the AABB
  float* aabbMinValsA,    ///< IN: the AABB min values for the first AABB
  float* aabbMaxValsA,    ///< IN: the AABB max values for the first AABB
  float* aabbMinValsB,    ///< IN: the AABB min values for the second AABB
  float* aabbMaxValsB     ///< IN: the AABB max values for the second AABB
  );

//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsAABBFromVertices(
  uint32_t numDimensions,         ///< IN: the dimensionality of the AABB
  const float* aabbMinVals,       ///< IN: the AABB min values
  const float* aabbMaxVals,       ///< IN: the AABB max values
  const float* const* vertices    ///< IN: the set of working vertices to test
  );

//----------------------------------------------------------------------------------------------------------------------
bool aabbContainsAnyVertex(
  uint32_t numDimensions,         ///< IN: the dimensionality of the AABB
  const float* aabbMinVals,       ///< IN: the AABB min values
  const float* aabbMaxVals,       ///< IN: the AABB max values
  const float* const* vertices    ///< IN: the set of working vertices to test
  );

bool aabbContainsAnyVertex(
  uint32_t numDimensions,         // IN: the dimensionality of the AABB
  const float* aabbMinVals,       // IN: the AABB min values
  const float* aabbMaxVals,       // IN: the AABB max values
  const float* const* vertices,   // IN: the set of working vertices to test
  const bool* valid               // IN: the set of working valid vertices
  );

//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsRay(
  uint32_t numDimensions,             ///< IN: the dimensionality of the AABB
  const float* aabbMinVals,           ///< IN: the AABB min values
  const float* aabbMaxVals,           ///< IN: the AABB max values
  uint32_t numRayComponents,          ///< IN: the number of non-zero ray components to test
  const uint32_t* rayComponentOrder,  ///< IN: vector of ray component indices to test
  const float* projectionOrigin,      ///< IN: the origin of the projection
  const float* projectionDirection    ///< IN: the direction of the ray
  );

//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsRayClosestPoint(
  uint32_t numDimensions,             ///< IN: the dimensionality of the AABB
  const float* aabbMinVals,           ///< IN: the AABB min values
  const float* aabbMaxVals,           ///< IN: the AABB max values
  const float* projectionOrigin,      ///< IN: the origin of the projection
  const float* queryPoint,            ///< IN: the query point through which to project the ray
  uint32_t& whichAABBDim,             ///< OUT: whichDim of the AABB identifies the intersecting facet
  uint32_t& whichAABBBoundary,        ///< OUT: whichBoundary of the AABB identifies the intersecting facet
  float* projectedAABBPoint           ///< OUT: the projected point intersecting the AABB facet
  );

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function tests whether the NDCell intersects the AABB.
/// The algorithm iteratively bisects the set of vertex interpolants, keeping the sub-cell
/// set whose resampled vertices have an AABB that intersects the query AABB (the one closest
/// to the query AABB centre). Iteration stops when a resampled vertex lies inside the
/// query AABB, none of the resampled vertex AABBs intersect the query AABB or the maximum
/// number of iterations has been exceeded.
//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsNDCell(
  uint32_t numDimensions,         ///< IN: the dimensionality of the vertex samples and AABB
  const float* aabbMinVals,       ///< IN: the AABB min values
  const float* aabbMaxVals,       ///< IN: the AABB max values
  const float* const* vertices,   ///< IN: the vertices of the NDCell
  float* const* verticesA         ///< TEMP: working memory for the resampled NDCell vertices
  );

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function tests whether the NDCell facet intersects the AABB by projecting the vertices
/// onto the specified hyper-plane (either projecting through a point or in a direction). The
/// compromised dimension of the AABB is assumed to be the last component, so you must ensure
/// that the vertex and query AABB components have been permuted accordingly.
///
/// \param numDimensions - The dimensionality of the vertex samples. There are 2^(N-1) vertex
///   samples in the input NDCell facet vertices.
///
/// \param aabbMinVals - The AABB min values: (N-1)d
///
/// \param aabbMaxVals - The AABB max values: (N-1)d
///
/// \param vertices - The vertices of the NDCell facet:
///   There are 2^(N-1) vertices, each with N components.
///
/// \param verticesA - Working memory for the resampled NDCell facet vertices:
///   There are 2^(N-1) vertices, each with (N-1) components.
///
/// \param validA - Working memory for the valid NDCell facet vertices: There are 2^(N-1) entries.
///
/// \param tmK - The projection coefficients (3-vector) [Kf, Kp, Kw] that describe how each vertex is projected
///   onto the AABB hyper-plane.
///
///   For parallel projection:
///     Kf = 1, Kp = 0, Kw = 1
///
///   For central projection:
///     Kf = the absolute distance between AABB and projection centre in the compromise dimension.
///     Kp = -1 or 1 depending on the sign of the difference between projection centre and AABB in
///          the compromise dimension. This is used to determine if a vertex lies on the same side
///          of the projection as the AABB hyper-plane. Points that do not lay in the forward projection
///          direction are not tested for intersection.
///     Kw = 0
//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsProjectedNDCellFacet(
  uint32_t numDimensions,
  const float* aabbMinVals,
  const float* aabbMaxVals,
  const float* const* vertices,
  float* const* verticesA,
  bool* validA,
  const float* tmK
  );

//----------------------------------------------------------------------------------------------------------------------
// Multi-linear interpolation
//----------------------------------------------------------------------------------------------------------------------

/// \brief Informs the caller what happened during an interpolation query.
enum InterpolationReturnVal
{
  INTERPOLATION_SUCCESS = 0,
  INTERPOLATION_PROJECTED,
  INTERPOLATION_FAILED
};

enum NDCellSolverReturnVal
{
  kNDCellSolveInterpolated,   ///< The solution is inside the NDCell, and we converged on this solution.

  kNDCellSolveExtrapolated,   ///< The solution is outside of the NDCell, and we converged on this solution.
                              ///< Extrapolation must be enabled for the solver to iterate until convergence
                              ///< when the multi-linear weights are outside of the NDCell.

  kNDCellSolveOutside,        ///< The solution is outside of the NDCell, but we didn't make any further
                              ///< iterations to converge on this solution. If extrapolation is disabled then
                              ///< the solver stops iterating when it detects that the solution is outside.
                              ///< The computed result values contain the last valid estimate.

  kNDCellSolveFailed,         ///< The solver was unable to compute a valid solution. Typically, the NDCell
                              ///< has a degenerate configuration (i.e. coincident or colinear vertex points),
                              ///< or the solver exceeded the maximum number of iterations. While the interior
                              ///< of a well defined annotation cell can be interpolated, not all of the exterior
                              ///< space can be reached by multi-linear interpolation (unless the sample vertices
                              ///< form a rectangular structure, the locus of interpolation tends to butterfly
                              ///< about some focal point). This may present a problem for extrapolation if the
                              ///< query point lies within the unreachable space.

  kNDCellSolverReturnValueInvalid = 0xFFFFFFFF
};

enum NDCellSolverStatus
{
  kAnalyticSolution,          ///< 1D and 2D solutions for the multi-linear cooeficients can be computed analytically
  kConvergedCostTol,          ///< The iterative solver converged within the query point cost tolerance
  kConvergedDeltaParamTol,    ///< The iterative solver converged within the multi-linear cooeficient parameter tolerance
  kDivergedOutsideNDCell,     ///< The iterative solver diverged to multi-linear cooeficients that lay outside the NDCell.
  kHessianSingular,           ///< The iterative solver hit a singularity at the current multi-linear cooeficients (Cooincident points, etc)
  kTerminatedMaxIterations,   ///< The iterative solver terminated because it did not converge to a solution within the specified number of iterations.

  kNDCellSolverStatusInvalid = 0xFFFFFFFF
};

//----------------------------------------------------------------------------------------------------------------------
class NDCellSolverData
{
public:
  // Initialisation
  NM_INLINE void init(
    uint32_t maxIterations,
    bool enableExtrapolation,
    float* initialInterpolants);
  
  // Input attributes
  NM_INLINE uint32_t getMaxIterations() const { return m_maxIterations; }
  NM_INLINE void setMaxIterations(uint32_t maxIterations) { m_maxIterations = maxIterations; }

  NM_INLINE bool getEnableExtrapolation() const { return m_enableExtrapolation; }
  NM_INLINE void setEnableExtrapolation(bool enableExtrapolation) { m_enableExtrapolation = enableExtrapolation; }

  NM_INLINE const float* getInitialInterpolants() const { return m_initialInterpolants; }
  NM_INLINE void setInitialInterpolants(const float* initialInterpolants) { m_initialInterpolants = initialInterpolants; }

  // Result data
  NM_INLINE uint32_t getNumIterations() const { return m_numIterations; }
  NM_INLINE void setNumIterations(uint32_t numIterations) { m_numIterations = numIterations; }
  NM_INLINE void incrementNumIterations() { m_numIterations++; }

  NM_INLINE NDCellSolverStatus getSolverStatus() const { return m_status; }
  NM_INLINE void setSolverStatus(NDCellSolverStatus status) { m_status = status; }

protected:
  // Input attributes
  uint32_t            m_maxIterations;
  bool                m_enableExtrapolation;
  const float*        m_initialInterpolants;  ///< For the iterative solver. If this is NULL then the interpolants
                                              ///< are initialised at the centre of the NDCell (all values are 0.5)

  // Result data
  NDCellSolverStatus  m_status;
  uint32_t            m_numIterations;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDCellSolverData::init(
  uint32_t maxIterations,
  bool enableExtrapolation,
  float* initialInterpolants)
{
  m_maxIterations = maxIterations;
  m_enableExtrapolation = enableExtrapolation;
  m_initialInterpolants = initialInterpolants;
}

//----------------------------------------------------------------------------------------------------------------------
// Functor prototypes
typedef void (*computeMultilinearInterpFn)(
  uint32_t              numDimensions,
  uint32_t              numSampleComponents,
  float*                x,
  const float*          interpolants,
  const float* const*   vertices);

typedef NDCellSolverReturnVal (*multilinearCellCoeffsFn)(
  uint32_t              numDimensions,
  const float*          x,
  float*                interpolants,
  const float* const*   vertices,
  NDCellSolverData*     solverData);

typedef bool (*multilinearCellCoeffsSearchDirFn)(
  uint32_t numDimensions,
  float dt[SCATTERED_DATA_VECTOR_SIZE],
  const float J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  const float Jtr[SCATTERED_DATA_VECTOR_SIZE]);

typedef float (*multilinearCellCoeffsJacFn)(
  uint32_t              numDimensions,
  const float*          x,
  const float*          tn,
  const float* const*   vertices,
  float                 J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  float                 r[SCATTERED_DATA_VECTOR_SIZE]);


//----------------------------------------------------------------------------------------------------------------------
bool insideNDCell(
  uint32_t      numDimensions,
  const float*  ts);

//----------------------------------------------------------------------------------------------------------------------
void computeMultilinearInterp1D(
  uint32_t              numDimensions,
  uint32_t              numSampleComponents,
  float*                x,
  const float*          ts,
  const float* const*   vertices);

void computeMultilinearInterp2D(
  uint32_t              numDimensions,
  uint32_t              numSampleComponents,
  float*                x,
  const float*          ts,
  const float* const*   vertices);

void computeMultilinearInterp3D(
  uint32_t              numDimensions,
  uint32_t              numSampleComponents,
  float*                x,
  const float*          ts,
  const float* const*   vertices);

void computeMultilinearInterpND(
  uint32_t              numDimensions,
  uint32_t              numSampleComponents,
  float*                x,
  const float*          ts,
  const float* const*   vertices);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Analytical solution to the 1D case of the multi-linear cell coefficient calculation.
/// This function performs extrapolation by default.
NDCellSolverReturnVal multilinearCellCoeffs1D(
  uint32_t              numDimensions,
  const float*          x,
  float*                ts,
  const float* const*   vertices,
  NDCellSolverData*     solverData);

/// \brief Analytical solution to the 2D case of the multi-linear cell coefficient calculation.
/// This function performs extrapolation by default.
NDCellSolverReturnVal multilinearCellCoeffs2D(
  uint32_t              numDimensions,
  const float*          x,
  float*                ts,
  const float* const*   vertices,
  NDCellSolverData*     solverData);

/// \brief Iterative solution for the ND case of the multi-linear cell coefficient calculation.
/// This function uses the solver data structure to modify the behaviour of the solve. Use the
/// attributes of this structure to flag whether the solver should perform extrapolation when
/// the solution is outside of the NDCell.
NDCellSolverReturnVal multilinearCellCoeffsND(
  uint32_t              numDimensions,
  const float*          x,
  float*                ts,
  const float* const*   vertices,
  NDCellSolverData*     solverData);


//----------------------------------------------------------------------------------------------------------------------
bool multilinearCellCoeffs1DSearchDir(
  uint32_t numDimensions,
  float dt[SCATTERED_DATA_VECTOR_SIZE],
  const float J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  const float Jtr[SCATTERED_DATA_VECTOR_SIZE]);

bool multilinearCellCoeffs2DSearchDir(
  uint32_t numDimensions,
  float dt[SCATTERED_DATA_VECTOR_SIZE],
  const float J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  const float Jtr[SCATTERED_DATA_VECTOR_SIZE]);

bool multilinearCellCoeffs3DSearchDir(
  uint32_t numDimensions,
  float dt[SCATTERED_DATA_VECTOR_SIZE],
  const float J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  const float Jtr[SCATTERED_DATA_VECTOR_SIZE]);

bool multilinearCellCoeffsNDSearchDir(
  uint32_t numDimensions,
  float dt[SCATTERED_DATA_VECTOR_SIZE],
  const float J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  const float Jtr[SCATTERED_DATA_VECTOR_SIZE]);

//----------------------------------------------------------------------------------------------------------------------
float multilinearCellCoeffs1DJac(
  uint32_t              numDimensions,
  const float*          x,
  const float*          tn,
  const float* const*   vertices,
  float                 J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  float                 r[SCATTERED_DATA_VECTOR_SIZE]);

float multilinearCellCoeffs2DJac(
  uint32_t              numDimensions,
  const float*          x,
  const float*          tn,
  const float* const*   vertices,
  float                 J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  float                 r[SCATTERED_DATA_VECTOR_SIZE]);

float multilinearCellCoeffs3DJac(
  uint32_t              numDimensions,
  const float*          x,
  const float*          tn,
  const float* const*   vertices,
  float                 J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  float                 r[SCATTERED_DATA_VECTOR_SIZE]);

float multilinearCellCoeffsNDJac(
  uint32_t              numDimensions,
  const float*          x,
  const float*          tn,
  const float* const*   vertices,
  float                 J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  float                 r[SCATTERED_DATA_VECTOR_SIZE]);

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredDataManager
/// \brief A manager to handle scattered data interpolation efficiently
//----------------------------------------------------------------------------------------------------------------------
class ScatteredDataManager
{
public:
  // Structure containing table lookup data and interpolation API
  class DataBlock
  {
  public:
    computeMultilinearInterpFn        m_multilinearInterpFn;
    multilinearCellCoeffsFn           m_multilinearCellCoeffsFn;
    multilinearCellCoeffsSearchDirFn  m_multilinearCellCoeffsSearchDirFn;
    multilinearCellCoeffsJacFn        m_multilinearCellCoeffsJacFn;
  };

  NM_INLINE static const DataBlock& getDataBlock(uint32_t numDimensions);

private:
  // Manager data blocks
  static const DataBlock sm_dataBlocks[SCATTERED_DATA_MAX_DIM];
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const ScatteredDataManager::DataBlock&
ScatteredDataManager::getDataBlock(uint32_t numDimensions)
{
  NMP_ASSERT(numDimensions > 0 && numDimensions <= SCATTERED_DATA_MAX_DIM);
  return sm_dataBlocks[numDimensions - 1];
}

} // namespace ScatteredData

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_SCATTERED_DATA_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
