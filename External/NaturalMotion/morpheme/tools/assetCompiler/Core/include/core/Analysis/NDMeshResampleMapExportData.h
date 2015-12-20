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
#ifndef AP_NDMESH_RESAMPLEMAP_EXPORTDATA_H
#define AP_NDMESH_RESAMPLEMAP_EXPORTDATA_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "morpheme/Prediction/mrNDMeshAPSearchMap.h"
#include "export/mcExport.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
  class PredictionModelNDMesh;
  class PackedArrayUInt32;
}

namespace AP
{

// Forward declaration
class ControlParameterRegularSampleGrid;

//----------------------------------------------------------------------------------------------------------------------
/// \class NDMeshResampleMapGridExportData
/// \brief Intermediate data representing a resampling grid for export to connect
//----------------------------------------------------------------------------------------------------------------------
class NDMeshResampleMapGridExportData
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // Initialisation
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numDimensions,
    const uint32_t* sampleCounts);

  static NDMeshResampleMapGridExportData* init(
    NMP::Memory::Resource& resource,
    uint32_t numDimensions,
    const uint32_t* sampleCounts);

  static NDMeshResampleMapGridExportData* create(
    uint32_t numDimensions,
    const uint32_t* sampleCounts);

  //--------------------------------------------------------------------------------------------------------------------
  // Accessors and Modifiers
  NM_INLINE uint32_t getNumDimensions() const;
  NM_INLINE uint32_t getNumVertexSamples() const;
  NM_INLINE uint32_t getNumCellSamples() const;

  NM_INLINE const float* getStartRanges() const;
  NM_INLINE const float* getEndRanges() const;
  NM_INLINE float getStartRange(uint32_t i) const;
  NM_INLINE float getEndRange(uint32_t i) const;
  NM_INLINE void setStartRange(uint32_t i, float value);
  NM_INLINE void setEndRange(uint32_t i, float value);

  NM_INLINE const uint32_t* getSampleCountsPerDimension() const;
  NM_INLINE uint32_t getSampleCountForDimension(uint32_t i) const;

  NM_INLINE const uint32_t* getVertexBlockSizes() const;
  NM_INLINE const uint32_t* getCellBlockSizes() const;

  NM_INLINE uint32_t coordinateToVertexAddress(
    const uint32_t* coordinate) const;

  NM_INLINE uint32_t coordinateToCellAddress(
    const uint32_t* coordinate) const;

  NM_INLINE const uint32_t* getWhichQueryComponentIndices() const;

  NM_INLINE void setWhichQueryComponentIndices(
    uint32_t numComponents,
    const uint32_t* whichQueryComponentIndices);

  NM_INLINE float getGridVertexComponent(
    uint32_t whichDim,
    uint32_t coord) const;

  NM_INLINE void getGridVertexFromCoordinate(
    const uint32_t* vertexCoord,
    float* vertexSample) const;

  NM_INLINE void getGridCellAABBComponent(
    uint32_t whichDim,
    uint32_t coord,
    float& aabbComponentMinVal,
    float& aabbComponentMaxVal) const;

  NM_INLINE void getGridCellAABBFromCoordinate(
    const uint32_t* topLeftCoord,
    float* aabbComponentMinVals,
    float* aabbComponentMaxVals) const;

  /// \brief Recover the specified component of the sample AABB
  NM_INLINE float getSampleAABBComponent(
    uint32_t whichDim,
    uint32_t whichBoundary) const;

private:
  // Regular grid resampling information
  uint32_t      m_numDimensions;
  uint32_t      m_numVertexSamples;   // i.e. NumSamples0 * NumSamples1 * ...
  uint32_t      m_numCellSamples;     // i.e. (NumSamples0 - 1) * (NumSamples1 - 1) * ...

  uint32_t*     m_nDVertexBlockSizes; // Nd block sizes for the resample map vertex samples
  uint32_t*     m_nDCellBlockSizes;   // Nd block sizes for the resample map NDCell flags

  // Array of query component indices that correspond to the complementary dimensions
  uint32_t*     m_whichQueryComponentIndices;

  // Analysis property resample grid
  uint32_t*     m_sampleCountsPerDimension;
  float*        m_startRanges;
  float*        m_endRanges;
};

//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampleMapExportData inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampleMapGridExportData::getNumDimensions() const
{
  return m_numDimensions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampleMapGridExportData::getNumVertexSamples() const
{
  return m_numVertexSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampleMapGridExportData::getNumCellSamples() const
{
  return m_numCellSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* NDMeshResampleMapGridExportData::getStartRanges() const
{
  return m_startRanges;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* NDMeshResampleMapGridExportData::getEndRanges() const
{
  return m_endRanges;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NDMeshResampleMapGridExportData::getStartRange(uint32_t i) const
{
  NMP_VERIFY(i < m_numDimensions);
  return m_startRanges[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NDMeshResampleMapGridExportData::getEndRange(uint32_t i) const
{
  NMP_VERIFY(i < m_numDimensions);
  return m_endRanges[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshResampleMapGridExportData::setStartRange(uint32_t i, float value)
{
  NMP_VERIFY(i < m_numDimensions);
  m_startRanges[i] = value;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshResampleMapGridExportData::setEndRange(uint32_t i, float value)
{
  NMP_VERIFY(i < m_numDimensions);
  m_endRanges[i] = value;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* NDMeshResampleMapGridExportData::getSampleCountsPerDimension() const
{
  return m_sampleCountsPerDimension;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampleMapGridExportData::getSampleCountForDimension(uint32_t i) const
{
  NMP_VERIFY(i < m_numDimensions);
  return m_sampleCountsPerDimension[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* NDMeshResampleMapGridExportData::getVertexBlockSizes() const
{
  return m_nDVertexBlockSizes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* NDMeshResampleMapGridExportData::getCellBlockSizes() const
{
  return m_nDCellBlockSizes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampleMapGridExportData::coordinateToVertexAddress(
  const uint32_t* coordinate) const
{
  NMP_VERIFY(coordinate);
  uint32_t index = 0;
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    NMP_VERIFY(coordinate[dim] < m_sampleCountsPerDimension[dim]);
    index += coordinate[dim] * m_nDVertexBlockSizes[dim];
  }
  return index;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampleMapGridExportData::coordinateToCellAddress(
  const uint32_t* coordinate) const
{
  NMP_VERIFY(coordinate);
  uint32_t index = 0;
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    NMP_VERIFY(coordinate[dim] < m_sampleCountsPerDimension[dim] - 1);
    index += coordinate[dim] * m_nDCellBlockSizes[dim];
  }
  return index;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* NDMeshResampleMapGridExportData::getWhichQueryComponentIndices() const
{
  return m_whichQueryComponentIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshResampleMapGridExportData::setWhichQueryComponentIndices(
  uint32_t numComponents,
  const uint32_t* whichQueryComponentIndices)
{
  NMP_VERIFY(numComponents == m_numDimensions);
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    m_whichQueryComponentIndices[i] = whichQueryComponentIndices[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NDMeshResampleMapGridExportData::getGridVertexComponent(
  uint32_t whichDim,
  uint32_t coord) const
{
  NMP_VERIFY(whichDim < m_numDimensions);
  NMP_VERIFY(coord < m_sampleCountsPerDimension[whichDim]);
  float alpha = (float)coord / (float)(m_sampleCountsPerDimension[whichDim] - 1);
  float value = (1.0f - alpha) * m_startRanges[whichDim] + alpha * m_endRanges[whichDim];
  return value;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshResampleMapGridExportData::getGridVertexFromCoordinate(
  const uint32_t* vertexCoord,
  float* vertexSample) const
{
  NMP_VERIFY(vertexCoord);
  NMP_VERIFY(vertexSample);
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    vertexSample[i] = getGridVertexComponent(i, vertexCoord[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshResampleMapGridExportData::getGridCellAABBComponent(
  uint32_t whichDim,
  uint32_t coord,
  float& aabbComponentMinVal,
  float& aabbComponentMaxVal) const
{
  NMP_VERIFY(whichDim < m_numDimensions);
  NMP_VERIFY(coord < m_sampleCountsPerDimension[whichDim] - 1);

  float scale = 1.0f / (float)(m_sampleCountsPerDimension[whichDim] - 1);

  // Min value
  float alpha = (float)coord * scale;
  float valueA = (1.0f - alpha) * m_startRanges[whichDim] + alpha * m_endRanges[whichDim];

  // Max value
  float beta = (float)(coord + 1) * scale;
  float valueB = (1.0f - beta) * m_startRanges[whichDim] + beta * m_endRanges[whichDim];

  // AABB component
  aabbComponentMinVal = NMP::minimum(valueA, valueB);
  aabbComponentMaxVal = NMP::maximum(valueA, valueB);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshResampleMapGridExportData::getGridCellAABBFromCoordinate(
  const uint32_t* topLeftCoord,
  float* aabbComponentMinVals,
  float* aabbComponentMaxVals) const
{
  NMP_VERIFY(topLeftCoord);
  NMP_VERIFY(aabbComponentMinVals);
  NMP_VERIFY(aabbComponentMaxVals);
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    getGridCellAABBComponent(i, topLeftCoord[i], aabbComponentMinVals[i], aabbComponentMaxVals[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NDMeshResampleMapGridExportData::getSampleAABBComponent(
  uint32_t whichDim,
  uint32_t whichBoundary) const
{
  NMP_VERIFY(whichDim < m_numDimensions);
  NMP_VERIFY(whichBoundary == 0 || whichBoundary == 1);
  float values[2] =
  {
    m_startRanges[whichDim],
    m_endRanges[whichDim]
  };
  return values[whichBoundary];
}

//----------------------------------------------------------------------------------------------------------------------
/// \class NDMeshResampleMapCPSampleExportData
/// \brief Intermediate data representing a resampling grid for export to connect
//----------------------------------------------------------------------------------------------------------------------
class NDMeshResampleMapCPSampleExportData
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // Initialisation
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numDimensions,
    const uint32_t* sampleCounts,
    uint32_t numComponentsPerSample);

  static NDMeshResampleMapCPSampleExportData* init(
    NMP::Memory::Resource& resource,
    uint32_t numDimensions,
    const uint32_t* sampleCounts,
    uint32_t numComponentsPerSample);

  static NDMeshResampleMapCPSampleExportData* create(
    uint32_t numDimensions,
    const uint32_t* sampleCounts,
    uint32_t numComponentsPerSample);

  //--------------------------------------------------------------------------------------------------------------------
  // Accessors and Modifiers
  NM_INLINE uint32_t getNumDimensions() const;

  NM_INLINE uint32_t getNumComponentsPerSample() const;

  NM_INLINE const float* const* getVertexCPSamples() const;
  NM_INLINE float* const* getVertexCPSamples();
  NM_INLINE const float* getVertexCPSamples(uint32_t index) const;
  NM_INLINE float* getVertexCPSamples(uint32_t index);

  NM_INLINE const bool* getCellFlags() const;
  NM_INLINE bool* getCellFlags();

protected:
  // Sample information
  uint32_t      m_numDimensions;
  uint32_t      m_numComponentsPerSample;

  // Control parameters mapped from APs
  float**       m_vertexCPSamples;

  // Grid cell flags
  bool*         m_cellFlags;
};

//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampleMapCPSampleExportData inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampleMapCPSampleExportData::getNumDimensions() const
{
  return m_numDimensions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampleMapCPSampleExportData::getNumComponentsPerSample() const
{
  return m_numComponentsPerSample;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* const* NDMeshResampleMapCPSampleExportData::getVertexCPSamples() const
{
  return m_vertexCPSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float* const* NDMeshResampleMapCPSampleExportData::getVertexCPSamples()
{
  return m_vertexCPSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* NDMeshResampleMapCPSampleExportData::getVertexCPSamples(uint32_t index) const
{
  NMP_VERIFY(index < m_numComponentsPerSample);
  return m_vertexCPSamples[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float* NDMeshResampleMapCPSampleExportData::getVertexCPSamples(uint32_t index)
{
  NMP_VERIFY(index < m_numComponentsPerSample);
  return m_vertexCPSamples[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const bool* NDMeshResampleMapCPSampleExportData::getCellFlags() const
{
  return m_cellFlags;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool* NDMeshResampleMapCPSampleExportData::getCellFlags()
{
  return m_cellFlags;
}


//----------------------------------------------------------------------------------------------------------------------
/// \class NDMeshResampleMapCPSampleQuantisationData
/// \brief Intermediate data representing a quantised set of CP vertex samples
/// for resmaple map interpolation.
//----------------------------------------------------------------------------------------------------------------------
class NDMeshResampleMapCPSampleQuantisationData
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // Initialisation
  static NMP::Memory::Format getMemoryRequirements(
    const NDMeshResampleMapGridExportData* interpolationGrid,
    uint32_t numBitsPerSample);

  static NDMeshResampleMapCPSampleQuantisationData* init(
    NMP::Memory::Resource& resource,
    const NDMeshResampleMapGridExportData* interpolationGrid,
    uint32_t numBitsPerSample);

  static NDMeshResampleMapCPSampleQuantisationData* create(
    const NDMeshResampleMapGridExportData* interpolationGrid,
    const NDMeshResampleMapCPSampleExportData* interpolationData,
    uint32_t numBitsPerSample);

  void releaseAndDestroy();

  NM_INLINE uint32_t getNumDimensions() const { return m_numDimensions; }
  NM_INLINE uint32_t getNumVertexSamples() const { return m_numVertexSamples; }
  NM_INLINE const float* getQMinValues() const { return m_qMinValues; }
  NM_INLINE const float* getQMaxValues() const { return m_qMaxValues; }
  NM_INLINE const float* getStepSizes() const { return m_stepSizes; }
  NM_INLINE const MR::PackedArrayUInt32* getVertexCPSamples() const { return m_vertexCPSamples; }

private:
  uint32_t                m_numDimensions;
  uint32_t                m_numVertexSamples;

  float*                  m_qMinValues;
  float*                  m_qMaxValues;
  float*                  m_stepSizes;
  float*                  m_recipStepSizes;

  MR::PackedArrayUInt32*  m_vertexCPSamples;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NDMeshResampleMapProjectionExportData
/// \brief Intermediate data representing a resampling grid for export to connect
//----------------------------------------------------------------------------------------------------------------------
class NDMeshResampleMapProjectionExportData
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // Initialisation
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numCellSamples,
    uint32_t numEntries);

  static NDMeshResampleMapProjectionExportData* init(
    NMP::Memory::Resource& resource,
    uint32_t numCellSamples,
    uint32_t numEntries);

  static NDMeshResampleMapProjectionExportData* create(
    uint32_t numCellSamples,
    uint32_t numEntries);

  void releaseAndDestroy();

  //--------------------------------------------------------------------------------------------------------------------
  // Accessors and Modifiers
  NM_INLINE uint32_t getNumCellSamples() const { return m_numCellSamples; }
  NM_INLINE uint32_t getNumEntries() const { return m_numEntries; }

  NM_INLINE const uint32_t* getCellEntryCounts() const { return m_cellEntryCounts; }
  NM_INLINE uint32_t* getCellEntryCounts() { return m_cellEntryCounts; }

  NM_INLINE const uint32_t* getEntriesCA() const { return m_entriesCA; }
  NM_INLINE uint32_t* getEntriesCA() { return m_entriesCA; }

  NM_INLINE const uint32_t* getEntriesWD() const { return m_entriesWD; }
  NM_INLINE uint32_t* getEntriesWD() { return m_entriesWD; }

  NM_INLINE const uint32_t* getEntriesWB() const { return m_entriesWB; }
  NM_INLINE uint32_t* getEntriesWB() { return m_entriesWB; }

protected:
  uint32_t    m_numCellSamples;   ///< The number of resample grid cells
  uint32_t    m_numEntries;       ///< The total number of table entries
  uint32_t*   m_cellEntryCounts;  ///< Entry counts corresponding to each resample grid cell
  uint32_t*   m_entriesCA;        ///< Cell address entries
  uint32_t*   m_entriesWD;        ///< WhichDim flag entries
  uint32_t*   m_entriesWB;        ///< WhichBoundary flag entries
};


//----------------------------------------------------------------------------------------------------------------------
/// \class NDMeshResampleGridExtrapolationData
//----------------------------------------------------------------------------------------------------------------------
class NDMeshResampleGridExtrapolationData
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numDimensions,
    uint32_t maxNumEntries);

  static NDMeshResampleGridExtrapolationData* init(
    NMP::Memory::Resource& resource,
    uint32_t numDimensions,
    uint32_t maxNumEntries);

  static NDMeshResampleGridExtrapolationData* create(
    uint32_t numDimensions,
    uint32_t maxNumEntries);

  void releaseAndDestroy();

  void reset();

  void addEntry(
    const float* cpComponents,
    const float* ts);

  void computeWeightedAverage(float* cpComponents);

  NM_INLINE uint32_t getNumEntries() const { return m_numEntries; }

private:
  uint32_t  m_numDimensions;
  uint32_t  m_maxNumEntries;
  uint32_t  m_numEntries;

  float**   m_cpComponentEntries;
  float*    m_costEntries;
  float*    m_weightEntries;
};


//----------------------------------------------------------------------------------------------------------------------
/// \class NDMeshResampleGridInterpolationData
/// \brief Intermediate data for compiling the resample grid interpolation data
//----------------------------------------------------------------------------------------------------------------------
class NDMeshResampleGridInterpolationData
{
public:
  struct VertexBinEntry
  {
    bool  m_vertexValid;              ///< Flag indicating if the resample vertex is valid.
    uint32_t  m_numGridCellEntries;   ///< The number of grid cells containing annotation intersection
                                      ///< data that are connected to the resample vertex.
    uint32_t* m_gridCellAddresses;    ///< The list of grid cells with intersection data connected to the
                                      ///< resample vertex. We store the flat address of the grid cell rather
                                      ///< than the coordinate.
  };

  struct GridCellBinEntry
  {
    uint32_t  m_numNodeEntries;       ///< The number of annotation cells that intersect the resample cell.
    MR::ScatteredData::NDCellEntry* m_nodeEntries;  ///< Array of intersection result data corresponding to
                                      ///< the annotation cells that intersect the resample cell. This is
                                      ///< externally allocated memory that should be deleted in releaseAndDestroy.
  };

public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numDimensions,
    uint32_t numVertexSamples,
    uint32_t numGridCells);

  static NDMeshResampleGridInterpolationData* init(
    NMP::Memory::Resource& resource,
    uint32_t numDimensions,
    uint32_t numVertexSamples,
    uint32_t numGridCells);

  static NDMeshResampleGridInterpolationData* create(
    uint32_t numDimensions,
    uint32_t numVertexSamples,
    uint32_t numGridCells);

  void releaseAndDestroy();

public:
  // General information
  uint32_t      m_numDimensions;
  uint32_t      m_numVertexSamples;   // i.e. NumSamples0 * NumSamples1 * ...
  uint32_t      m_numGridCells;       // i.e. (NumSamples0 - 1) * (NumSamples1 - 1) * ...

  // Per resample vertex compilation information
  VertexBinEntry*   m_vertexData;

  // Per resample cell compilation information
  GridCellBinEntry* m_gridCellData;
};


//----------------------------------------------------------------------------------------------------------------------
/// \class NDMeshResampleGridProjectionData
/// \brief Intermediate data for compiling the resample grid projection data
//----------------------------------------------------------------------------------------------------------------------
class NDMeshResampleGridProjectionData
{
public:
  struct GridCellBinEntry
  {
    uint32_t  m_numNodeEntries;       ///< The number of annotation cells that intersect the resample cell.
    MR::ScatteredData::NDCellFacetEntry* m_nodeEntries;  ///< Array of intersection result data corresponding to
                                      ///< the annotation cells that intersect the resample cell. This is
                                      ///< externally allocated memory that should be deleted in releaseAndDestroy.
  };

public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numDimensions,
    uint32_t numGridCells);

  static NDMeshResampleGridProjectionData* init(
    NMP::Memory::Resource& resource,
    uint32_t numDimensions,
    uint32_t numGridCells);

  static NDMeshResampleGridProjectionData* create(
    uint32_t numDimensions,
    uint32_t numGridCells);

  void releaseAndDestroy();

public:
  // General information
  uint32_t      m_numDimensions;
  uint32_t      m_numGridCells;       // i.e. (NumSamples0 - 1) * (NumSamples1 - 1) * ...

  // Per resample cell compilation information
  GridCellBinEntry* m_gridCellData;
};


//----------------------------------------------------------------------------------------------------------------------
/// \class NDMeshResampleMapExportData
//----------------------------------------------------------------------------------------------------------------------
class NDMeshResampleMapExportData
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // Construction / Destruction
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numDimensions,
    uint32_t numAPQueryComplementComponentIndices,
    const uint32_t* sampleCounts);

  static NDMeshResampleMapExportData* init(
    NMP::Memory::Resource& resource,
    uint32_t numDimensions,
    uint32_t numAPQueryComplementComponentIndices,
    const uint32_t* sampleCounts,
    uint32_t numBitsPerSample,
    bool buildProjectThroughSampleCentreMap);

  // For creating the resample maps for analysis
  static NDMeshResampleMapExportData* createForAnalysis(
    const ME::AnalysisNodeExport* resampleGridExport,
    const ME::AnalysisNodeExport* ndMeshNodeExport);

  static NDMeshResampleMapExportData* createForAnalysisUsingDefaults(
    const ME::AnalysisNodeExport* ndMeshNodeExport,
    const ControlParameterRegularSampleGrid* regularSampleGrid);

  // For creating an intermediate format from the NDMesh export while building
  // the final prediction model binary
  static NDMeshResampleMapExportData* createForPreview(
    const ME::AnalysisNodeExport* ndMeshNodeExport,
    uint32_t resampleMapIndex,
    MR::AnimSetIndex animSetIndex);

  static uint32_t findNDMeshNodeAPIndex(
    const ME::AnalysisNodeExport* ndMeshNodeExport,
    const char* apPathIn);

  void releaseAndDestroy();


  //--------------------------------------------------------------------------------------------------------------------
  // Processing
  void analyse(const MR::PredictionModelNDMesh* ndMeshModel);

  //--------------------------------------------------------------------------------------------------------------------
  // Serialisation
  static void readExportData(
    uint32_t resampleMapIndex,
    const ME::DataBlockExport* ndMeshNodeExportDataBlock,
    uint32_t bufferSize,
    uint32_t& numDimensions,
    uint32_t& numAnalysisProperties,
    uint32_t& numBitsPerSample,
    bool& buildProjectThroughSampleCentreMap,
    uint32_t* sampleCounts,
    uint32_t* apQueryComponentIndices);

  void readPerSetExportData(
    uint32_t resampleMapIndex,
    MR::AnimSetIndex animSetIndex,
    const ME::DataBlockExport* dataBlock);

  void writeExportData(
    uint32_t resampleMapIndex,
    ME::DataBlockExport* resultData) const;

  void writePerSetExportData(
    uint32_t resampleMapIndex,
    MR::AnimSetIndex animSetIndex,
    ME::DataBlockExport* resultData) const;

  void tidyPerSetExportData();

  ///--------------------------------------------------------------------------------------------------------------------
  // Accessors and Modifiers
  NM_INLINE uint32_t getNumDimensions() const; // i.e. the number of control parameters

  NM_INLINE const uint32_t* getAPQueryComponentIndices() const;
  NM_INLINE uint32_t getAPQueryComponentIndex(uint32_t i) const;
  NM_INLINE void setAPQueryComponentIndex(uint32_t i, uint32_t value);
  NM_INLINE void setAPQueryComponentIndices(uint32_t numDimensions, const uint32_t* apComponentIndices);

  NM_INLINE uint32_t getNumAPQueryComplementComponentIndices() const; // i.e. the number of analysis properties
  NM_INLINE const uint32_t* getAPQueryComplementComponentIndices() const;

  NM_INLINE const NDMeshResampleMapGridExportData* getInterpolationGrid() const;
  NM_INLINE NDMeshResampleMapGridExportData* getInterpolationGrid();

  NM_INLINE const NDMeshResampleMapCPSampleExportData* getInterpolationData() const;
  NM_INLINE NDMeshResampleMapCPSampleExportData* getInterpolationData();

  NM_INLINE const NDMeshResampleMapCPSampleQuantisationData* getInterpolationQuantisationData() const;
  NM_INLINE NDMeshResampleMapCPSampleQuantisationData* getInterpolationQuantisationData();

  NM_INLINE const NDMeshResampleMapGridExportData* getProjectionGrid(uint32_t whichDim) const;
  NM_INLINE NDMeshResampleMapGridExportData* getProjectionGrid(uint32_t whichDim);

  NM_INLINE const NDMeshResampleMapProjectionExportData* getProjectAlongDimensionData(uint32_t whichDim) const;
  NM_INLINE NDMeshResampleMapProjectionExportData* getProjectAlongDimensionData(uint32_t whichDim);
  NM_INLINE void setProjectAlongDimensionData(uint32_t whichDim, NDMeshResampleMapProjectionExportData* data);

  NM_INLINE const NDMeshResampleMapProjectionExportData* getProjectThroughPointData(uint32_t whichDim, uint32_t whichBoundary) const;
  NM_INLINE NDMeshResampleMapProjectionExportData* getProjectThroughPointData(uint32_t whichDim, uint32_t whichBoundary);
  NM_INLINE void setProjectThroughPointData(uint32_t whichDim, uint32_t whichBoundary, NDMeshResampleMapProjectionExportData* data);

  NM_INLINE uint32_t getNumBitsPerSample() const { return m_numBitsPerSample; }
  NM_INLINE bool getBuildProjectThroughSampleCentreMap() const { return m_buildProjectThroughSampleCentreMap; }

  void setSampleAABB(
    uint32_t numDimensions,
    const float* aabbComponentMinVals,
    const float* aabbComponentMaxVals);

  void calculateAPQueryComplementComponentIndices();

protected:
  //---------------------------
  // Interpolation grid data
  void calculateInterpolationGridVertexSamples(
    const MR::PredictionModelNDMesh* ndMeshModel,
    NDMeshResampleGridInterpolationData* tempData);

  void calculateNDCellsIntersectingInterpolationGrid(
    const MR::PredictionModelNDMesh* ndMeshModel,
    NDMeshResampleGridInterpolationData* tempData);

  void calculateInterpolationGridVertexConnections(
    NDMeshResampleGridInterpolationData* tempData);

  void extrapolateInterpolationGridVertexSamples(
    const MR::PredictionModelNDMesh* ndMeshModel,
    NDMeshResampleGridInterpolationData* tempData);

  void calculateInterpolationGridCellFlags(
    NDMeshResampleGridInterpolationData* tempData);

  //---------------------------
  // Project along dimension grid data
  void calculateNDCellsIntersectingProjectionGridProjectAlongDimension(
    uint32_t whichDim,
    const MR::PredictionModelNDMesh* ndMeshModel,
    NDMeshResampleGridProjectionData* tempData);

  void compileProjectAlongDimensionExportData(
    uint32_t whichDim,
    const MR::PredictionModelNDMesh* ndMeshModel,
    NDMeshResampleGridProjectionData* tempData);

  //---------------------------
  // Project through point grid data
  void calculateNDCellsIntersectingProjectionGridProjectThroughPoint(
    const float* projectionCentre,
    uint32_t whichDim,
    uint32_t whichBoundary,
    const MR::PredictionModelNDMesh* ndMeshModel,
    NDMeshResampleGridProjectionData* tempData);

  void compileProjectThroughPointExportData(
    uint32_t whichDim,
    uint32_t whichBoundary,
    const MR::PredictionModelNDMesh* ndMeshModel,
    NDMeshResampleGridProjectionData* tempData);

  //---------------------------
  enum SetNDCellFacetEntryBufferStrategy
  {
    kClosestToFurthest,
    kStaggeredRearAndFore
  };

  void setNDCellFacetEntryBufferFromDistance(
    const MR::ScatteredData::NDMesh* nDMesh,
    const MR::ScatteredData::ProjectionBasis* projectionBasis,
    MR::ScatteredData::ProjectRayResult* projectionResult,
    uint32_t numNodeEntries,
    const MR::ScatteredData::NDCellFacetEntry* nodeEntriesSrc,
    MR::ScatteredData::NDCellFacetEntry* nodeEntriesDst,
    SetNDCellFacetEntryBufferStrategy strategy);

  //---------------------------
  void compileQuantisationData();

private:
  // Options
  uint32_t      m_numBitsPerSample;
  bool          m_buildProjectThroughSampleCentreMap;

  // Sample information
  uint32_t      m_numDimensions;
  uint32_t      m_numAPQueryComplementComponentIndices;
  uint32_t*     m_apQueryComponentIndices;
  uint32_t*     m_apQueryComplementComponentIndices;

  // Interpolation query resample map
  NDMeshResampleMapGridExportData*            m_interpolationGrid;
  NDMeshResampleMapCPSampleExportData*        m_interpolationData;
  NDMeshResampleMapCPSampleQuantisationData*  m_interpolationQuantisationData;

  // Projection query resample maps
  NDMeshResampleMapGridExportData**           m_projectionGrids;
  NDMeshResampleMapProjectionExportData**     m_projectAlongDimensionData;
  NDMeshResampleMapProjectionExportData**     m_projectThroughPointData[2]; // Boundary [0,1]
};

//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampleMapExportData inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampleMapExportData::getNumDimensions() const
{
  return m_numDimensions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* NDMeshResampleMapExportData::getAPQueryComponentIndices() const
{
  return m_apQueryComponentIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampleMapExportData::getAPQueryComponentIndex(uint32_t i) const
{
  NMP_VERIFY(i < m_numDimensions);
  return m_apQueryComponentIndices[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshResampleMapExportData::setAPQueryComponentIndex(uint32_t i, uint32_t value)
{
  NMP_VERIFY(i < m_numDimensions);
  m_apQueryComponentIndices[i] = value;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshResampleMapExportData::setAPQueryComponentIndices(uint32_t numDimensions, const uint32_t* apComponentIndices)
{
  NMP_VERIFY(numDimensions == m_numDimensions);
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    m_apQueryComponentIndices[i] = apComponentIndices[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampleMapExportData::getNumAPQueryComplementComponentIndices() const
{
  return m_numAPQueryComplementComponentIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* NDMeshResampleMapExportData::getAPQueryComplementComponentIndices() const
{
  return m_apQueryComplementComponentIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NDMeshResampleMapGridExportData* NDMeshResampleMapExportData::getInterpolationGrid() const
{
  return m_interpolationGrid;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NDMeshResampleMapGridExportData* NDMeshResampleMapExportData::getInterpolationGrid()
{
  return m_interpolationGrid;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NDMeshResampleMapCPSampleExportData* NDMeshResampleMapExportData::getInterpolationData() const
{
  return m_interpolationData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NDMeshResampleMapCPSampleExportData* NDMeshResampleMapExportData::getInterpolationData()
{
  return m_interpolationData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NDMeshResampleMapCPSampleQuantisationData*
NDMeshResampleMapExportData::getInterpolationQuantisationData() const
{
  return m_interpolationQuantisationData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NDMeshResampleMapCPSampleQuantisationData*
NDMeshResampleMapExportData::getInterpolationQuantisationData()
{
  return m_interpolationQuantisationData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NDMeshResampleMapGridExportData* NDMeshResampleMapExportData::getProjectionGrid(uint32_t dim) const
{
  NMP_VERIFY(dim < m_numDimensions);
  return m_projectionGrids[dim];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NDMeshResampleMapGridExportData* NDMeshResampleMapExportData::getProjectionGrid(uint32_t whichDim)
{
  NMP_VERIFY(whichDim < m_numDimensions);
  return m_projectionGrids[whichDim];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NDMeshResampleMapProjectionExportData* NDMeshResampleMapExportData::getProjectAlongDimensionData(uint32_t whichDim) const
{
  NMP_VERIFY(whichDim < m_numDimensions);
  return m_projectAlongDimensionData[whichDim];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NDMeshResampleMapProjectionExportData* NDMeshResampleMapExportData::getProjectAlongDimensionData(uint32_t whichDim)
{
  NMP_VERIFY(whichDim < m_numDimensions);
  return m_projectAlongDimensionData[whichDim];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshResampleMapExportData::setProjectAlongDimensionData(uint32_t whichDim, NDMeshResampleMapProjectionExportData* data)
{
  NMP_VERIFY(whichDim < m_numDimensions);
  NMP_VERIFY_MSG(
    !m_projectAlongDimensionData[whichDim],
    "Trying to set a ProjectAlongDimension data entry that has already been set");
  m_projectAlongDimensionData[whichDim] = data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NDMeshResampleMapProjectionExportData*
NDMeshResampleMapExportData::getProjectThroughPointData(uint32_t whichDim, uint32_t whichBoundary) const
{
  NMP_VERIFY(whichDim < m_numDimensions);
  NMP_VERIFY(whichBoundary < 2);
  return m_projectThroughPointData[whichBoundary][whichDim];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NDMeshResampleMapProjectionExportData*
NDMeshResampleMapExportData::getProjectThroughPointData(uint32_t whichDim, uint32_t whichBoundary)
{
  NMP_VERIFY(whichDim < m_numDimensions);
  NMP_VERIFY(whichBoundary < 2);
  return m_projectThroughPointData[whichBoundary][whichDim];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshResampleMapExportData::setProjectThroughPointData(
  uint32_t whichDim,
  uint32_t whichBoundary,
  NDMeshResampleMapProjectionExportData* data)
{
  NMP_VERIFY(whichDim < m_numDimensions);
  NMP_VERIFY(whichBoundary < 2);
  NMP_VERIFY_MSG(
    !m_projectThroughPointData[whichBoundary][whichDim],
    "Trying to set a ProjectThroughPoint data entry that has already been set");
  m_projectThroughPointData[whichBoundary][whichDim] = data;
}

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_NDMESH_RESAMPLEMAP_EXPORTDATA_H
//----------------------------------------------------------------------------------------------------------------------
