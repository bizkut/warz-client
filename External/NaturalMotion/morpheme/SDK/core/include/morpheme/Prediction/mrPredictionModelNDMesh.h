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
#ifndef MR_PREDICTION_MODEL_ND_MESH_H
#define MR_PREDICTION_MODEL_ND_MESH_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribData.h"
#include "morpheme/Prediction/mrNetworkPredictionDef.h"
#include "morpheme/Prediction/mrNDMesh.h"
#include "morpheme/Prediction/mrNDMeshAPSearchMap.h"
#include "morpheme/Prediction/mrNDMeshAPResampleMap.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PredictionModelNDMesh
/// \brief Prediction model that uses an NDMesh to store and look up analysed Network parameters
///  (generally motion values such as velocity, distance etc) and network control parameter relationships.
/// \see MR::PredictionModelDef
///
/// AP = Analysed Network Parameter, CP = Network Control Parameter.
/// This model type requires a 1 to 1 relationship between control params and analysed params, so their counts must be equal.
/// This class wraps a ScatteredData::SpatialMap which is a lot more generic.
//----------------------------------------------------------------------------------------------------------------------
class PredictionModelNDMesh : public PredictionModelDef
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Serialisation
  /// \brief  Functions to relocating the data in memory
  //--------------------------------------------------------------------------------------------------------------------
  //@{
  static void dislocate(PredictionModelDef* target);
  static void locate(PredictionModelDef* target);
  static PredictionModelNDMesh* relocate(PredictionModelDef* target);
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Forward Prediction Map: CP -> AP
  /// \brief  Functions to handle fast lookup queries from control parameters to analysis properties
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  // NDMesh - A regularly sampled N-dimensional mesh, whose vertex samples contain the N-d regularly sampled
  // controlling parameters in the first N components, and the analysed properties in the remaining M components.
  // The annotation of cells in this NDMesh forms the basis for interpolating query points between controlling
  // parameter and analysed property component spaces.
  //
  // Independent variables - The components of the vertex samples used as the query point when determining the
  // containing cell and vertex interpolation weights.

  /// \brief Fast lookup map using the control components as the independent variables for interpolation.
  /// The cell to interpolate is found with a fast lookup using the grid of regularly sampled controlling
  /// components. The vertex interpolation weights are computed from the position of the control parameter
  /// components within the containing cell.
  ///
  /// \param componentsSize The number of components per vertex sample. This is used for validation.
  ///
  /// \param components The sample vector containing the control parameters in the first numDimensions
  /// entries and the analysed properties in the remaining entries. On input the control parameter components
  /// are used as the query point for the cell lookup. On output the control parameter components are
  /// replaced with its projected position if the query point lies outside of the regular sample grid, while
  /// the analysed property entries are set with the interpolated cell vertex components.
  ScatteredData::InterpolationReturnVal calculateAPsFromCPs(
    uint32_t          componentsSize,
    float*            components
    ) const;
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Inverse Prediction Map: AP -> CP
  /// \brief  Functions to handle inverse prediction queries from analysis properties to control parameters
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Inverse search map using a sub-set of the analysed properties as the independent variables for
  /// interpolation. The cell to interpolate is found via a search through the analysed component space. The
  /// search algorithm uses simple bounding box containment to quickly determine the initial set of candidate
  /// cells. The vertex interpolation weights are computed from the queryPoint position within the containing
  /// cell. Slower than using lookup, but is a very general solution.
  ///
  /// \param componentsSize The number of components per vertex sample. This is used for validation.
  ///
  /// \param components The sample vector containing the control parameters in the first numDimensions
  /// entries and the analysed properties in the remaining entries. On input the analysed property
  /// components specified by the independentVarIndices array are used as the query point for interpolation.
  /// On output these analysed property components are replaced with the projected values if the query point
  /// lies outside of the annotated sample space, while the remaining entries of the vector are set with the
  /// interpolated cell vertex components.
  ///
  /// \param independentVarsSize The number of components in the query point. This is used for validation
  /// and should be the same as numDimensions.
  ///
  /// \param independentVarIndices A numDimensions array of indices identifying which components of a vertex
  /// sample form the queryPoint of independent variables.
  ///
  /// \param enableProjection A flag indicating whether the function should project the query point back
  /// onto the annotated sample space if it lies outside. The function will just return a fail code otherwise.
  ///
  /// \param projectionParams The projection options. If this is NULL and projection is enabled then the
  /// independent variables are sacrificed in ascending order.
  ScatteredData::InterpolationReturnVal calculateCPsFromAPs(
    uint32_t          componentsSize,
    float*            components,
    uint32_t          independentVarsSize,
    const uint32_t*   independentVarIndices,
    bool              enableProjection = false,
    const ScatteredData::Projection* projectionParams = 0
    ) const;

  /// \brief AP Search map variation of the inverse prediction model query.
  ///
  /// \param componentsSize The number of components per vertex sample. This is used for validation.
  ///
  /// \param components The sample vector containing the control parameters in the first numDimensions
  /// entries and the analysed properties in the remaining entries. On input the analysed property
  /// components specified by the independentVarIndices array are used as the query point for interpolation.
  /// On output these analysed property components are replaced with the projected values if the query point
  /// lies outside of the annotated sample space, while the remaining entries of the vector are set with the
  /// interpolated cell vertex components.
  ///
  /// \param independentVarsSize The number of components in the query point. This is used for validation
  /// and should be the same as numDimensions.
  ///
  /// \param independentVarIndices A numDimensions array of indices identifying which components of a vertex
  /// sample form the queryPoint of independent variables.
  ///
  /// \param ndMeshQueryResult The query result data structure
  ///
  /// \param enableProjection A flag indicating whether the function should project the query point back
  /// onto the annotated sample space if it lies outside. The function will just return a fail code otherwise.
  ///
  /// \param projectionParams The projection options. If this is NULL and projection is enabled then the
  /// independent variables are sacrificed in ascending order.
  ScatteredData::InterpolationReturnVal calculateCPsFromAPsUsingAPSearchMap(
    uint32_t            componentsSize,
    float*              components,
    uint32_t            independentVarsSize,
    const uint32_t*     independentVarIndices,
    ScatteredData::NDMeshQueryResult* ndMeshQueryResult,
    bool                enableProjection = false,
    const ScatteredData::Projection* projectionParams = 0
    ) const;

  /// \brief AP Resample map variation of the inverse prediction model query.
  ///
  /// \param resampleMapIndex The index of the resample map to use
  ///
  /// \param componentsSize The number of components per vertex sample. This is used for validation.
  ///
  /// \param components The sample vector containing the control parameters in the first numDimensions
  /// entries and the analysed properties in the remaining entries. On input the analysed property
  /// components specified by the independentVarIndices array are used as the query point for interpolation.
  /// On output these analysed property components are replaced with the projected values if the query point
  /// lies outside of the annotated sample space, while the remaining entries of the vector are set with the
  /// interpolated cell vertex components.
  ///
  /// \param ndMeshQueryResult The query result data structure
  ///
  /// \param enableProjection A flag indicating whether the function should project the query point back
  /// onto the annotated sample space if it lies outside. The function will just return a fail code otherwise.
  ///
  /// \param projectionParams The projection options. If this is NULL and projection is enabled then the
  /// independent variables are sacrificed in ascending order.
  ScatteredData::InterpolationReturnVal calculateCPsFromAPsUsingAPResampleMap(
    uint32_t resampleMapIndex,
    uint32_t componentSize,
    float* components,
    ScatteredData::NDMeshQueryResult* ndMeshQueryResult,
    bool enableProjection = false,
    const ScatteredData::Projection* projectionParams = 0
    ) const;

  /// \brief Get the minimum and maximum values of one of the independent variables given the values of the others.
  /// The function returns true if the query point could be projected onto the sample space.
  ///
  /// \param independentVarsSize The number of components in the query point. This is used for validation
  ///   and should be the same as numDimensions.
  ///
  /// \param independentVarIndices A numDimensions array of indices identifying which components of a vertex
  ///   sample form the queryPoint of independent variables.
  ///
  /// \param independentVars A numDimensions array of independent variable values. These values are held
  ///   fixed while the specified variable can change to bring the query point into the sample space. The
  ///   query point is projected to find both the minimum and maximum extents of the sample space.
  ///
  /// \param whichQueryVariable The index of the independent variable for which you are querying the range.
  ///   Note this is an index into the independentVars array, not into the vertex sample data.
  ///
  /// \param minimum The minimum extent of the specified independent variable.
  ///
  /// \param maximum The maximum extent of the specified independent variable.
  bool calculateRange(
    uint32_t            independentVarsSize,
    const uint32_t*     independentVarIndices,
    const float*        independentVars,
    uint32_t            whichQueryVariable,
    float&              minimum,
    float&              maximum
    ) const;

  /// \brief AP Search map variation of the range query function
  ///
  /// \param independentVarsSize The number of components in the query point. This is used for validation
  ///   and should be the same as numDimensions.
  ///
  /// \param independentVarIndices A numDimensions array of indices identifying which components of a vertex
  ///   sample form the queryPoint of independent variables.
  ///
  /// \param independentVars A numDimensions array of independent variable values. These values are held
  ///   fixed while the specified variable can change to bring the query point into the sample space. The
  ///   query point is projected to find both the minimum and maximum extents of the sample space.
  ///
  /// \param whichQueryVariable The index of the independent variable for which you are querying the range.
  ///   Note this is an index into the independentVars array, not into the vertex sample data.
  ///
  /// \param minimum The minimum extent of the specified independent variable.
  ///
  /// \param maximum The maximum extent of the specified independent variable.
  ///
  /// \param ndMeshQueryResultRear The rear NDMesh query result
  ///
  /// \param ndMeshQueryResultFore The fore NDMesh query result
  NM_INLINE bool calculateRangeUsingAPSearchMap(
    uint32_t            independentVarsSize,
    const uint32_t*     independentVarIndices,
    const float*        independentVars,
    uint32_t            whichQueryVariable,
    float&              minimum,
    float&              maximum,
    ScatteredData::NDMeshQueryResult* ndMeshQueryResultRear,
    ScatteredData::NDMeshQueryResult* ndMeshQueryResultFore
    ) const;

  /// \brief AP Resample map variation of the range query function
  ///
  /// \param resampleMapIndex The index of the resample map to use
  ///
  /// \param independentVarsSize The number of components in the query point. This is used for validation
  ///   and should be the same as numDimensions.
  ///
  /// \param independentVars A numDimensions array of independent variable values. These values are held
  ///   fixed while the specified variable can change to bring the query point into the sample space. The
  ///   query point is projected to find both the minimum and maximum extents of the sample space.
  ///
  /// \param whichQueryVariable The index of the independent variable for which you are querying the range.
  ///   Note this is an index into the independentVars array, not into the vertex sample data.
  ///
  /// \param minimum The minimum extent of the specified independent variable.
  ///
  /// \param maximum The maximum extent of the specified independent variable.
  ///
  /// \param ndMeshQueryResultRear The rear NDMesh query result
  ///
  /// \param ndMeshQueryResultFore The fore NDMesh query result
  NM_INLINE bool calculateRangeUsingAPResampleMap(
    uint32_t            resampleMapIndex,
    uint32_t            independentVarsSize,
    const float*        independentVars,
    uint32_t            whichQueryVariable,
    float&              minimum,
    float&              maximum,
    ScatteredData::NDMeshQueryResult* ndMeshQueryResultRear,
    ScatteredData::NDMeshQueryResult* ndMeshQueryResultFore
    ) const;
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex functions
  /// \brief  Functions for accessing NDMesh vertex data
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief Recover the sample centre of the NDMesh (Barycentre of the vertex samples)
  ///
  /// \param componentsSize The component buffer size - should be the same as the number of components per sample.
  ///
  /// \param components The buffer for the sample centre components
  NM_INLINE void getSampleCentre(
    uint32_t          componentsSize,
    float*            components) const;

  /// \brief Recover the sample centre of the NDMesh (Barycentre of the vertex samples)
  ///
  /// \param outputComponentsSize The component buffer size
  ///
  /// \param outputComponentIndices The indices of the vertex components to recover
  ///
  /// \param outputComponentValues The buffer for the sample centre components
  NM_INLINE void getSampleCentre(
    uint32_t        outputComponentsSize,
    float*          outputComponentValues,
    const uint32_t* outputComponentIndices) const;

  /// \brief Recover the Axis Aligned Bounding Box for the NDMesh sample data
  ///
  /// \param componentsSize The component buffer size - should be the same as the number of components per sample.
  ///
  /// \param aabbMinVals The buffer for the AABB min values
  ///
  /// \param aabbMaxVals The buffer for the AABB max values
  NM_INLINE void getSampleAABB(
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
  NM_INLINE void getSampleAABB(
    uint32_t        numSampleComponents,
    const uint32_t* whichSampleComponents,
    float*          aabbComponentMinVals,
    float*          aabbComponentMaxVals) const;
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Accessors and Modifiers
  /// \brief  Functions for accessing general information about the NDMesh
  //--------------------------------------------------------------------------------------------------------------------
  //@{

  /// \brief The number of dimensions, useful for writing code that works for a variety of models.
  NM_INLINE uint32_t getNumDimensions() const { return m_nDMesh->getNumDimensions(); }

  /// \brief How many components make up every sample.
  NM_INLINE uint32_t getNumComponentsPerSample() const { return m_nDMesh->getNumComponentsPerSample(); }

  /// \brief How many samples are there in total.
  NM_INLINE uint32_t getNumSamples() const { return m_nDMesh->getNumSamples(); }

  /// \brief How many samples have been taken in each dimension.
  NM_INLINE const uint32_t* getSampleCountsPerDimension() const { return m_nDMesh->getSampleCountsPerDimension(); }

  /// \brief The static type ID of PredictionModelNDMesh.
  NM_INLINE static PredictionType getClassPredictionType() { return PREDICTMODELTYPE_ND_MESH; }

  /// \brief Get the ND mesh
  NM_INLINE const ScatteredData::NDMesh* getNDMesh() const { return m_nDMesh; }

  /// \brief Does this prediction model contain an AP search map for AP -> CP prediction
  NM_INLINE bool hasAPSearchMap() const { return m_apSearchMap != NULL; }

  /// \brief Return the AP search map for AP -> CP prediction
  NM_INLINE const ScatteredData::NDMeshAPSearchMap* getAPSearchMap() const { return m_apSearchMap; }

  /// \brief How many resample maps does this prediction model contain
  NM_INLINE uint32_t getNumAPResampleMaps() const { return m_numAPResampleMaps; }

  /// \brief Get the AP resample map at the index position
  NM_INLINE const ScatteredData::NDMeshAPResampleMap* getAPResampleMap(uint32_t mapIndex) const;
  //@}

protected:
  NM_INLINE PredictionModelNDMesh() { m_type = PREDICTMODELTYPE_ND_MESH; m_name = NULL; }

  uint32_t                                m_numAPResampleMaps;
  ScatteredData::NDMesh*                  m_nDMesh;
  ScatteredData::NDMeshAPSearchMap*       m_apSearchMap;
  ScatteredData::NDMeshAPResampleMap**    m_apResampleMaps;
};


//----------------------------------------------------------------------------------------------------------------------
// PredictionModelNDMesh inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool PredictionModelNDMesh::calculateRangeUsingAPSearchMap(
  uint32_t            NMP_USED_FOR_ASSERTS(independentVarsSize),
  const uint32_t*     independentVarIndices,
  const float*        independentVars,
  uint32_t            whichQueryVariable,
  float&              minimum,
  float&              maximum,
  ScatteredData::NDMeshQueryResult* ndMeshQueryResultRear,
  ScatteredData::NDMeshQueryResult* ndMeshQueryResultFore) const
{
  NMP_ASSERT(m_nDMesh);
  NMP_ASSERT_MSG(
    m_apSearchMap,
    "The prediction model does not contain an AP search map.");

  NMP_ASSERT(independentVarsSize == m_nDMesh->getNumDimensions());
  return m_apSearchMap->getRangeInDimension(
    m_nDMesh,
    independentVars,
    independentVarIndices,
    whichQueryVariable,
    minimum,
    maximum,
    ndMeshQueryResultRear,
    ndMeshQueryResultFore);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool PredictionModelNDMesh::calculateRangeUsingAPResampleMap(
  uint32_t            resampleMapIndex,
  uint32_t            NMP_USED_FOR_ASSERTS(independentVarsSize),
  const float*        independentVars,
  uint32_t            whichQueryVariable,
  float&              minimum,
  float&              maximum,
  ScatteredData::NDMeshQueryResult* ndMeshQueryResultRear,
  ScatteredData::NDMeshQueryResult* ndMeshQueryResultFore) const
{
  const ScatteredData::NDMeshAPResampleMap* resampleMap = getAPResampleMap(resampleMapIndex);
  NMP_ASSERT(resampleMap);
  NMP_ASSERT(independentVarsSize == resampleMap->getNumDimensions());
  return resampleMap->getRangeInDimension(
    m_nDMesh,
    independentVars,
    whichQueryVariable,
    minimum,
    maximum,
    ndMeshQueryResultRear,
    ndMeshQueryResultFore);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PredictionModelNDMesh::getSampleCentre(
  uint32_t          componentsSize,
  float*            components) const
{
  NMP_ASSERT(m_nDMesh);
  m_nDMesh->getSampleCentre(componentsSize, components);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PredictionModelNDMesh::getSampleCentre(
  uint32_t        outputComponentsSize,
  float*          outputComponentValues,
  const uint32_t* outputComponentIndices) const
{
  NMP_ASSERT(m_nDMesh);
  m_nDMesh->getSampleCentre(outputComponentsSize, outputComponentIndices, outputComponentValues);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PredictionModelNDMesh::getSampleAABB(
  uint32_t          componentsSize,
  float*            aabbMinVals,
  float*            aabbMaxVals) const
{
  NMP_ASSERT(m_nDMesh);
  m_nDMesh->getSampleAABB(componentsSize, aabbMinVals, aabbMaxVals);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PredictionModelNDMesh::getSampleAABB(
  uint32_t        numSampleComponents,
  const uint32_t* whichSampleComponents,
  float*          aabbComponentMinVals,
  float*          aabbComponentMaxVals) const
{
  NMP_ASSERT(m_nDMesh);
  m_nDMesh->getSampleAABB(numSampleComponents, whichSampleComponents, aabbComponentMinVals, aabbComponentMaxVals);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const ScatteredData::NDMeshAPResampleMap*
PredictionModelNDMesh::getAPResampleMap(uint32_t mapIndex) const
{
  NMP_ASSERT_MSG(
    mapIndex < m_numAPResampleMaps,
    "The prediction model does not contain a resample map at index %d",
    mapIndex);
  return m_apResampleMaps[mapIndex];
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PREDICTION_MODEL_ND_MESH_H
//----------------------------------------------------------------------------------------------------------------------
