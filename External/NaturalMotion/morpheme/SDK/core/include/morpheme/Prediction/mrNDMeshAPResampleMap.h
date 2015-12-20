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
#ifndef MR_NDMESH_RESAMPLE_MAP_H
#define MR_NDMESH_RESAMPLE_MAP_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBitArray.h"
#include "morpheme/mrPackedArrayUint32.h"
#include "morpheme/Prediction/mrNDMesh.h"
#include "morpheme/Prediction/mrNDMeshQueryData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

namespace ScatteredData
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::NDMeshAPResampleMapProjectionData
/// \brief Packed array data for the projection grid
//----------------------------------------------------------------------------------------------------------------------
class NDMeshAPResampleMapProjectionData
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Serialisation
  /// \brief  Functions to relocating the data in memory
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Prepare data for another platform
  static void dislocate(NDMeshAPResampleMapProjectionData* target);

  /// \brief Prepare data for the local platform
  static void locate(NDMeshAPResampleMapProjectionData* target);

  /// \brief Prepare data after being moved on the local platform
  static NDMeshAPResampleMapProjectionData* relocate(void*& ptr);
  //@}

public:
  PackedArrayUInt32*  m_cellEntryOffsets;
  PackedArrayUInt32*  m_cellEntries;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatteredData::NDMeshAPResampleMap
/// \brief A fast lookup map between analysis properties and control parameters
/// created by resampling the analysis property space.
//----------------------------------------------------------------------------------------------------------------------
class NDMeshAPResampleMap
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Serialisation
  /// \brief  Functions to relocating the data in memory
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Prepare data for another platform
  static void dislocate(NDMeshAPResampleMap* target);

  /// \brief Prepare data for the local platform
  static void locate(NDMeshAPResampleMap* target);

  /// \brief Prepare data after being moved on the local platform
  static NDMeshAPResampleMap* relocate(void*& ptr);
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

  /// \brief Checks to see if the query options are compatible with this resample map
  ///
  /// \param apQueryComponentIndices An m_numDimensions array of indices identifying which components of a
  ///   vertex sample form the independent variables of the queryPoint.
  ///
  /// \param projectionParams The projection method and options to use when the queryPoint lies outside of
  ///   the cell annotations in analysed component space.
  bool isValidQuery(
    const uint32_t* apQueryComponentIndices,
    const ScatteredData::Projection* projectionParams
    ) const;

  /// \brief Inverse resample map using the analysed properties as independent variables for interpolation.
  /// The cell to interpolate is found via a fast lookup and guided search through the analysed component space.
  /// The vertex interpolation weights are computed from the queryPoint position within the containing cell. The
  /// output vector is interpolated from the specified components of the cell's vertex samples identified by
  /// the outputComponentIndices array.
  ///
  /// \param nDMesh The NDMesh on which the search map has been compiled for
  ///
  /// \param queryPoint An m_numDimensions array of values identifying the independent variables for interpolation.
  ///   This vector may be modified by projection if the queryPoint lies outside of the cell annotations in
  ///   analysed component space.
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
    NDMeshQueryResult*  ndMeshQueryResult,
    uint32_t            numOutputSampleComponents,
    const uint32_t*     outputSampleComponentIndices,
    float*              outputSampleComponents,
    bool                enableProjection,
    const Projection*   projectionParams = 0
    ) const;

  /// \brief Get the minimum and maximum values of one of the independent variables fixing the values of the others.
  ///
  /// \param nDMesh The NDMesh on which the search map has been compiled for
  ///
  /// \param queryPoint An m_numDimensions array of values identifying a point in the sample space using the sample
  ///   components identified by the queryPointComponentIndices array as the independent variables.
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
    uint32_t            whichQueryVariable,
    float&              minimum,
    float&              maximum,
    NDMeshQueryResult*  ndMeshQueryResultRear,
    NDMeshQueryResult*  ndMeshQueryResultFore
    ) const;
  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the APResampleMap
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  NM_INLINE uint32_t getNumDimensions() const { return m_numDimensions; }

  NM_INLINE uint32_t getNumAPQueryComplementComponentIndices() const { return m_numAPQueryComplementComponentIndices; }

  NM_INLINE const uint32_t* getAPQueryComponentIndices() const { return m_apQueryComponentIndices; }

  NM_INLINE const uint32_t* getAPQueryComplementComponentIndices() const { return m_apQueryComplementComponentIndices; }

  NM_INLINE const uint32_t* getSampleCountsPerDimension() const { return m_sampleCountsPerDimension; }

  NM_INLINE const float* getAPGridStartValues() const { return m_apGridStartValues; }

  NM_INLINE const float* getAPGridEndValues() const { return m_apGridEndValues; }

  /// \brief Recover the CP components for the resample map cell
  ///
  /// \param topLeftCoord The coordinate of the top-left corner of the ND resample grid cell
  ///
  /// \param vertexComponents A buffer for the vertex components
  void getInterpolationCellVertexComponents(
    const uint32_t* topLeftCoord,
    float* const* vertexComponents) const;
  //@}

protected:
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Internal helper functions
  /// \brief  Functions to handle resample map queries
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Function finds the NDCell containing the query point and interpolates the required output components.
  ///
  /// \param nDMesh The NDMesh on which the search map has been compiled for
  ///
  /// \param queryPoint An m_numDimensions array of values identifying a point in the sample space using the sample
  ///   components identified by the queryPointComponentIndices array as the independent variables. On input these
  ///   components form the independent variables.
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
    NDMeshQueryResult*  ndMeshQueryResult,
    uint32_t            numOutputSampleComponents,
    const uint32_t*     outputSampleComponentIndices,
    float*              outputSampleComponents
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
    uint32_t            whichVariableToCompromise,
    NDMeshQueryResult*  ndMeshQueryResult,
    uint32_t            numOutputSampleComponents,
    const uint32_t*     outputSampleComponentIndices,
    float*              outputSampleComponents
    ) const;

  /// \brief Project the query point onto the nearest point on the boundary of the sample space by
  /// sacrificing the components along the specified projection direction.
  ///
  /// \param queryPoint An m_numDimensions array of values identifying a point in the sample space using the sample
  ///   components identified by the queryPointComponentIndices array as the independent variables. On input these
  ///   components form the independent variables, while on output these components are overwritten with the nearest
  ///   projected intersection point on the boundary.
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
    NDMeshQueryResult*  ndMeshQueryResult,
    uint32_t            numOutputSampleComponents,
    const uint32_t*     outputSampleComponentIndices,
    float*              outputSampleComponents
    ) const;

  /// \brief Recover the ND resample grid cell and interpolants for the AP query point
  ///
  /// \pararm apQueryPoint An m_numDimensions array of values identifying a point in the sample space using
  ///   the sample components identified by the queryPointComponentIndices array as the independent variables.
  ///
  /// \param apTopLeftCoord The coordinate of the top-left corner of the ND resample grid cell
  ///
  /// \param apInterpolants The interpolants for the query point within the ND resample grid cell
  bool interpolateMultilinearWeights(
    const float*        apQueryPoint,               // IN (Nd)
    uint32_t*           apTopLeftCoord,             // OUT (Nd)
    float*              apInterpolants              // OUT (Nd)
    ) const;

  /// \brief Recover the (N-1)d projection resample grid cell and interpolants for the AP query point
  ///
  /// \pararm apQueryPoint An m_numDimensions array of values identifying a point in the sample space using
  ///   the sample components identified by the queryPointComponentIndices array as the independent variables.
  ///
  /// \param whichVariableToCompromise An index into the queryPoint array identifying which independent variable can change
  ///   to bring the query point into the sample space.
  ///
  /// \param apTopLeftCoord The (N-1)d coordinate of the top-left corner of the ND projection resample grid cell
  bool projectAlongDimCellCoordinates(
    const float*        apQueryPoint,               // IN (function permutes Nd -> N-1d)
    uint32_t            whichVariableToCompromise,  // IN (encodes the projection dimension)
    uint32_t*           apTopLeftCoord              // OUT (N-1)d
    ) const;

  /// \brief Recover the (N-1)d projection resample grid cell and interpolants for the AP query point
  ///
  /// \pararm apQueryPoint An m_numDimensions array of values identifying a point in the sample space using
  ///   the sample components identified by the queryPointComponentIndices array as the independent variables.
  ///
  /// \param projectionCentre An m_numDimensions array of values identifying a point in the sample space using the
  ///   sample components identified by the queryPointComponentIndices array as the target for projection. The
  ///   projection vector is formed as the line joining both the query and target points.
  ///
  /// \param whichAABBDim Which dim of the projection AABB to use for the query resample map.
  ///
  /// \param whichAABBBoundary Which near or far boundary to use for the query resample map.
  ///
  /// \param apTopLeftCoord The (N-1)d coordinate of the top-left corner of the ND projection resample grid cell
  bool projectThroughPointCellCoordinates(
    const float*        apQueryPoint,               // IN: (function permutes Nd -> N-1d)
    const float*        projectionCentre,           // IN:
    uint32_t&           whichAABBDim,               // OUT: 
    uint32_t&           whichAABBBoundary,          // OUT:
    uint32_t*           apTopLeftCoord              // OUT:
    ) const;

  /// \brief Unpack the projection cell entries for the corresponding projection resample grid cell
  ///
  /// \param whichVariableToCompromise Which dimension of the query point to sacrifice for projection.
  ///
  /// \param apTopLeftCoord The (N-1)d coordinate of the top-left corner of the ND projection resample grid cell
  ///
  /// \param maxNumEntries The maximum number of entries for the output buffers
  ///
  /// \param cpCellAddresses The buffer of cell address entries
  ///
  /// \param whichDimFlags he buffer of whichDim flags
  ///
  /// \param whichBoundaryFlags The buffer of whichBoundry flags
  uint32_t getProjectionAlongDimensionCellEntries(
    uint32_t          whichVariableToCompromise,  // IN:
    const uint32_t*   apTopLeftCoord,             // IN:
    uint32_t          maxNumEntries,              // IN:
    uint32_t*         cpCellAddresses,            // OUT:
    uint32_t*         whichDimFlags,              // OUT:
    uint32_t*         whichBoundaryFlags          // OUT:
    ) const;

  /// \brief Unpack the projection cell entries for the corresponding projection resample grid cell
  ///
  /// \param whichDim whichDim identifier for the projection grid hyper-plane
  ///
  /// \param whichBoundary whichBoundary identifier for the projection grid hyper-plane
  ///
  /// \param apTopLeftCoord The (N-1)d coordinate of the top-left corner of the ND projection resample grid cell
  ///
  /// \param maxNumEntries The maximum number of entries for the output buffers
  ///
  /// \param cpCellAddresses The buffer of cell address entries
  ///
  /// \param whichDimFlags he buffer of whichDim flags
  ///
  /// \param whichBoundaryFlags The buffer of whichBoundry flags
  uint32_t getProjectionThroughPointCellEntries(
    uint32_t          whichDim,                   ///< IN:
    uint32_t          whichBoundary,              ///< IN:
    const uint32_t*   apTopLeftCoord,             ///< IN:
    uint32_t          maxNumEntries,              ///< IN:
    uint32_t*         cpCellAddresses,            ///< OUT:
    uint32_t*         whichDimFlags,              ///< OUT:
    uint32_t*         whichBoundaryFlags          ///< OUT:
    ) const;
  //@}

protected:
  //------------------------------
  /// \brief Number of dimensions (number of controlling parameters).
  uint32_t            m_numDimensions;
  uint32_t            m_numAPQueryComplementComponentIndices;

  /// \brief The component indices of the analysis properties that make up the query point
  uint32_t*           m_apQueryComponentIndices;

  /// \brief The complement of the analysis property query point component indices
  uint32_t*           m_apQueryComplementComponentIndices;

  /// \brief The sample counts in each dimension
  uint32_t*           m_sampleCountsPerDimension;

  /// \brief For grid data, the starting value for the variables in each dimension.
  float*              m_apGridStartValues;

  /// \brief For grid data, the end value for the variables in each dimension.
  float*              m_apGridEndValues;

  /// \brief For grid data, the reciprocal of the sample period (space between each sample in each dimension).
  float*              m_apGridSampleScales;

  //------------------------------
  // Interpolation resample map
  uint32_t*           m_interpolationVertexBlockSizes;
  uint32_t*           m_interpolationCellBlockSizes;

  float*              m_interpolationVertexCPScales;            // Quantisation scales
  float*              m_interpolationVertexCPOffsets;           // Quantisation offsets

  PackedArrayUInt32*  m_interpolationVertexCPSamples;           // Resampled control parameter vertices for the interpolation grid
  NMP::BitArray*      m_interpolationCellFlags;                 // Bit-array of valid interpolation cell entries

  //------------------------------
  // Projection resample maps
  uint32_t**          m_projectionWhichQueryComponentIndices;   // A table of (N-1)d arrays encoding which query component indices correspond to the complement
                                                                // dimensions. These are arrays of indices into the m_apQueryComponentIndices component vector.
  uint32_t**          m_projectionCellBlockSizes;               // A table if (N-1)d arrays containing the data block strides for the projection resample maps.

  // Projection along dimension resample map data
  NDMeshAPResampleMapProjectionData** m_projectionAlongDimensionData;

  // Projection through point resample map data
  bool                m_projectionThroughPointEnable;
  NDMeshAPResampleMapProjectionData** m_projectionThroughPointData[2];
};

} // namespace ScatteredData

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NDMESH_RESAMPLE_MAP_H
//----------------------------------------------------------------------------------------------------------------------
