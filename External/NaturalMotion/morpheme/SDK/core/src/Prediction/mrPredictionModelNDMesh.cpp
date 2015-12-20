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
#include "morpheme/Prediction/mrScatteredDataUtils.h"
#include "morpheme/Prediction/mrPredictionModelNDMesh.h"
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
ScatteredData::InterpolationReturnVal PredictionModelNDMesh::calculateAPsFromCPs(
  uint32_t  NMP_USED_FOR_ASSERTS(componentsSize),
  float*    components) const
{
  NMP_ASSERT(m_nDMesh);
  uint32_t numDimensions = m_nDMesh->getNumDimensions();
  uint32_t numAnalysisProperties = m_nDMesh->getNumAnalysisProperties();
  NMP_ASSERT(componentsSize == numDimensions + numAnalysisProperties);
  NMP_ASSERT(components);
  const uint32_t* analysisPropertyIndices = m_nDMesh->getAnalysisPropertyComponentIndices();
  float* outputComponentValues = &components[numDimensions];

  // Interpolate
  ScatteredData::InterpolationReturnVal result;
  result = m_nDMesh->mapFromCPs(
    components,
    numAnalysisProperties,
    analysisPropertyIndices,
    outputComponentValues);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
ScatteredData::InterpolationReturnVal PredictionModelNDMesh::calculateCPsFromAPs(
  uint32_t          componentsSize,
  float*            components,
  uint32_t          independentVarsSize,
  const uint32_t*   independentVarIndices,
  bool              enableProjection,
  const ScatteredData::Projection* projectionParams) const
{
  ScatteredData::NDMeshQueryResult ndMeshQueryResult;

  //------------------------------
  // Find a resample map that supports the query options
  for (uint32_t resampleMapIndex = 0; resampleMapIndex < m_numAPResampleMaps; ++resampleMapIndex)
  {
    const ScatteredData::NDMeshAPResampleMap* apResampleMap = m_apResampleMaps[resampleMapIndex];
    NMP_ASSERT(apResampleMap);
    NMP_ASSERT(apResampleMap->getNumDimensions() == independentVarsSize);

    if (apResampleMap->isValidQuery(independentVarIndices, projectionParams))
    {
      return calculateCPsFromAPsUsingAPResampleMap(
        resampleMapIndex,
        componentsSize,
        components,
        &ndMeshQueryResult,
        enableProjection,
        projectionParams);
    }
  }

  //------------------------------
  // Perform a slow search using the AP Search map
  if (m_apSearchMap)
  {
    return calculateCPsFromAPsUsingAPSearchMap(
      componentsSize,
      components,
      independentVarsSize,
      independentVarIndices,
      &ndMeshQueryResult,
      enableProjection,
      projectionParams);
  }

  return ScatteredData::INTERPOLATION_FAILED;
}

//----------------------------------------------------------------------------------------------------------------------
ScatteredData::InterpolationReturnVal PredictionModelNDMesh::calculateCPsFromAPsUsingAPSearchMap(
  uint32_t            NMP_USED_FOR_ASSERTS(componentsSize),
  float*              components,
  uint32_t            NMP_USED_FOR_ASSERTS(independentVarsSize),
  const uint32_t*     independentVarIndices,
  ScatteredData::NDMeshQueryResult* ndMeshQueryResult,
  bool                enableProjection,
  const ScatteredData::Projection* projectionParams) const
{
  NMP_ASSERT(m_nDMesh);
  NMP_ASSERT_MSG(
    m_apSearchMap,
    "The prediction model does not contain an AP search map.");

  uint32_t numDimensions = m_nDMesh->getNumDimensions();
  uint32_t numComponentsPerSample = m_nDMesh->getNumComponentsPerSample();  
  NMP_ASSERT(componentsSize == numComponentsPerSample);
  NMP_ASSERT(componentsSize <= SCATTERED_DATA_MAX_SAMPLE_COMPONENTS);
  NMP_ASSERT(components);
  NMP_ASSERT(independentVarsSize == numDimensions);

  //------------------------------
  // Build the workspace component vectors
  float independentVars[SCATTERED_DATA_MAX_DIM];
  float outputComponentValues[SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
  uint32_t outputComponentIndices[SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
  bool isIndependentVar[SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];

  // Determine which components to interpolate
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
  {
    isIndependentVar[i] = false;
  }
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    independentVars[i] = components[independentVarIndices[i]];
    isIndependentVar[independentVarIndices[i]] = true;
  }
  uint32_t outputComponentsSize = 0;
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
  {
    if (!isIndependentVar[i])
    {
      outputComponentIndices[outputComponentsSize] = i;
      ++outputComponentsSize;
    }
  }
  NMP_ASSERT(outputComponentsSize > 0);

  //------------------------------
  // Interpolate
  ScatteredData::InterpolationReturnVal result;
  result = m_apSearchMap->mapFromAPs(
    m_nDMesh,               // IN
    independentVars,        // IN / OUT: Query components
    independentVarIndices,  // IN
    ndMeshQueryResult,      // OUT: the interpolation query data
    outputComponentsSize,   // IN
    outputComponentIndices, // IN
    outputComponentValues,  // OUT: Complementary sample components
    enableProjection,       // IN
    projectionParams);      // IN

  //------------------------------
  // Write the result data
  if (result != ScatteredData::INTERPOLATION_FAILED)
  {
    // Overwrite input variables in case they were projected.
    for (uint32_t i = 0; i < numDimensions; ++i)
    {
      components[independentVarIndices[i]] = independentVars[i];
    }

    // Copy the output variables back into the original array.
    for (uint32_t i = 0; i < outputComponentsSize; ++i)
    {
      components[outputComponentIndices[i]] = outputComponentValues[i];
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
ScatteredData::InterpolationReturnVal PredictionModelNDMesh::calculateCPsFromAPsUsingAPResampleMap(
  uint32_t  resampleMapIndex,
  uint32_t  NMP_USED_FOR_ASSERTS(componentsSize),
  float*    components,
  ScatteredData::NDMeshQueryResult* ndMeshQueryResult,
  bool      enableProjection,
  const ScatteredData::Projection* projectionParams) const
{  
  const ScatteredData::NDMeshAPResampleMap* resampleMap = getAPResampleMap(resampleMapIndex);
  NMP_ASSERT(resampleMap);
  const uint32_t* independentVarIndices = resampleMap->getAPQueryComponentIndices();
  NMP_ASSERT(independentVarIndices);
  const uint32_t* outputComponentIndices = resampleMap->getAPQueryComplementComponentIndices();
  NMP_ASSERT(outputComponentIndices);
  uint32_t outputComponentsSize = resampleMap->getNumAPQueryComplementComponentIndices();
  NMP_ASSERT(outputComponentsSize > 0);
  float outputComponentValues[SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];

  NMP_ASSERT(m_nDMesh);
  uint32_t numDimensions = m_nDMesh->getNumDimensions();
  NMP_ASSERT(componentsSize == m_nDMesh->getNumComponentsPerSample());
  NMP_ASSERT(componentsSize <= SCATTERED_DATA_MAX_SAMPLE_COMPONENTS);
  NMP_ASSERT(components);

  //------------------------------
  // Recover the query point
  float independentVars[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    independentVars[i] = components[independentVarIndices[i]];
  }

  //------------------------------
  // Interpolate
  ScatteredData::InterpolationReturnVal result;
  result = resampleMap->mapFromAPs(
    m_nDMesh,               // IN
    independentVars,        // IN / OUT: Query components
    ndMeshQueryResult,      // OUT: the interpolation query data
    outputComponentsSize,   // IN
    outputComponentIndices, // IN
    outputComponentValues,  // OUT: Complementary sample components
    enableProjection,       // IN
    projectionParams);      // IN

  //------------------------------
  // Write the result data
  if (result != ScatteredData::INTERPOLATION_FAILED)
  {
    // Overwrite input variables in case they were projected.
    for (uint32_t i = 0; i < numDimensions; ++i)
    {
      components[independentVarIndices[i]] = independentVars[i];
    }

    // Copy the output variables back into the original array.
    for (uint32_t i = 0; i < outputComponentsSize; ++i)
    {
      components[outputComponentIndices[i]] = outputComponentValues[i];
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool PredictionModelNDMesh::calculateRange(
  uint32_t          NMP_USED_FOR_ASSERTS(independentVarsSize),
  const uint32_t*   independentVarIndices,
  const float*      independentVars,
  uint32_t          whichQueryVariable,
  float&            minimum,
  float&            maximum) const
{
  ScatteredData::NDMeshQueryResult ndMeshQueryResultRear;
  ScatteredData::NDMeshQueryResult ndMeshQueryResultFore;

  //------------------------------
  // Find a resample map that supports the query options
  for (uint32_t resampleMapIndex = 0; resampleMapIndex < m_numAPResampleMaps; ++resampleMapIndex)
  {
    const ScatteredData::NDMeshAPResampleMap* apResampleMap = m_apResampleMaps[resampleMapIndex];
    NMP_ASSERT(apResampleMap);
    NMP_ASSERT(apResampleMap->getNumDimensions() == independentVarsSize);

    if (apResampleMap->isValidQuery(independentVarIndices, NULL)) // Supply NULL projection options since projection along the principal directions is always possible
    {
      return apResampleMap->getRangeInDimension(
        m_nDMesh,
        independentVars,
        whichQueryVariable,
        minimum,
        maximum,
        &ndMeshQueryResultRear,
        &ndMeshQueryResultFore);
    }
  }

  //------------------------------
  // Perform a slow search using the AP Search map
  if (m_apSearchMap)
  {
    return m_apSearchMap->getRangeInDimension(
      m_nDMesh,
      independentVars,
      independentVarIndices,
      whichQueryVariable,
      minimum,
      maximum,
      &ndMeshQueryResultRear,
      &ndMeshQueryResultFore);
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void PredictionModelNDMesh::dislocate(PredictionModelDef* target)
{
  PredictionModelNDMesh* def = (PredictionModelNDMesh*)target;

  // AP resample maps
  if (def->m_apResampleMaps)
  {
    for (uint32_t i = 0; i < def->m_numAPResampleMaps; ++i)
    {
      NMP_ASSERT(def->m_apResampleMaps[i]);
      ScatteredData::NDMeshAPResampleMap::dislocate(def->m_apResampleMaps[i]);
      UNFIX_SWAP_PTR_RELATIVE(ScatteredData::NDMeshAPResampleMap, def->m_apResampleMaps[i], target);
    }
    UNFIX_SWAP_PTR_RELATIVE(ScatteredData::NDMeshAPResampleMap*, def->m_apResampleMaps, target);
  }

  // AP search map
  if (def->m_apSearchMap)
  {
    ScatteredData::NDMeshAPSearchMap::dislocate(def->m_apSearchMap);
    UNFIX_SWAP_PTR_RELATIVE(ScatteredData::NDMeshAPSearchMap, def->m_apSearchMap, target);
  }

  // ND Mesh
  ScatteredData::NDMesh::dislocate(def->m_nDMesh);
  UNFIX_SWAP_PTR_RELATIVE(ScatteredData::NDMesh, def->m_nDMesh, target);

  // Header
  NMP::endianSwap(def->m_numAPResampleMaps);
  target->dislocate();
}

//----------------------------------------------------------------------------------------------------------------------
void PredictionModelNDMesh::locate(PredictionModelDef* target)
{
  PredictionModelNDMesh* def = (PredictionModelNDMesh*)target;

  // Header
  target->locate();
  NMP::endianSwap(def->m_numAPResampleMaps);

  // ND Mesh
  REFIX_SWAP_PTR_RELATIVE(ScatteredData::NDMesh, def->m_nDMesh, target);
  ScatteredData::NDMesh::locate(def->m_nDMesh);

  // AP search map
  if (def->m_apSearchMap)
  {
    REFIX_SWAP_PTR_RELATIVE(ScatteredData::NDMeshAPSearchMap, def->m_apSearchMap, target);
    ScatteredData::NDMeshAPSearchMap::locate(def->m_apSearchMap);
  }

  // AP resample maps
  if (def->m_apResampleMaps)
  {
    REFIX_SWAP_PTR_RELATIVE(ScatteredData::NDMeshAPResampleMap*, def->m_apResampleMaps, target);
    for (uint32_t i = 0; i < def->m_numAPResampleMaps; ++i)
    {
      NMP_ASSERT(def->m_apResampleMaps[i]);
      REFIX_SWAP_PTR_RELATIVE(ScatteredData::NDMeshAPResampleMap, def->m_apResampleMaps[i], target);
      ScatteredData::NDMeshAPResampleMap::locate(def->m_apResampleMaps[i]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
PredictionModelNDMesh* PredictionModelNDMesh::relocate(PredictionModelDef* target)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(MR::PredictionModelNDMesh), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP_ASSERT(NMP_IS_ALIGNED(target, memReqsHdr.alignment));
  void* ptr = target;
  PredictionModelNDMesh* result = (PredictionModelNDMesh*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  // NOTE: The prediction model name points to an external string and is set after relocation.

  // ND Mesh
  result->m_nDMesh = ScatteredData::NDMesh::relocate(ptr);

  // AP search map
  if (result->m_apSearchMap)
  {
    result->m_apSearchMap = ScatteredData::NDMeshAPSearchMap::relocate(ptr);
  }

  // AP resample maps
  if (result->m_numAPResampleMaps > 0)
  {
    NMP::Memory::Format memReqsRMTable(
      sizeof(ScatteredData::NDMeshAPResampleMap*) * result->m_numAPResampleMaps,
      NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_apResampleMaps = (ScatteredData::NDMeshAPResampleMap**)NMP::Memory::alignAndIncrement(ptr, memReqsRMTable);

    for (uint32_t i = 0; i < result->m_numAPResampleMaps; ++i)
    {
      result->m_apResampleMaps[i] = ScatteredData::NDMeshAPResampleMap::relocate(ptr);
    }
  }

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
