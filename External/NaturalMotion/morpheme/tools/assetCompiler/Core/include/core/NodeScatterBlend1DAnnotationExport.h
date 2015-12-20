// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_NODE_SCATTER_BLEND_1D_ANNOTATION_EXPORT_H
#define MR_NODE_SCATTER_BLEND_1D_ANNOTATION_EXPORT_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// SB1DAnnotationExportDataPacked
//----------------------------------------------------------------------------------------------------------------------
class SB1DAnnotationExportDataPacked
{
public:
  SB1DAnnotationExportDataPacked();
  ~SB1DAnnotationExportDataPacked();

  void release();

  /// \brief Initialise the export data
  void init(
    uint32_t numSubDivisions,
    uint32_t numSubSamplesPerLine,
    uint32_t numVertices,
    uint32_t numLines,
    uint32_t numSubSamples);

  void writeFile(const char* filename) const;

  /// \brief Return the number of sub-divisions.
  NM_INLINE uint32_t getNumSubDivisions() const;

  /// \brief Return the number of sub-samples per line.
  NM_INLINE uint32_t getNumSubSamplesPerLine() const;

  /// \brief Return the number of vertices in the annotation.
  NM_INLINE uint32_t getNumVertices() const;

  /// \brief Return the number of lines in the annotation
  NM_INLINE uint32_t getNumLines() const;

  /// \brief Return the number of unique sub-samples within the annotation.
  /// i.e. The full set of samples that sub-divide the lines, less the
  /// annotation vertices.
  NM_INLINE uint32_t getNumSubSamples() const;

  /// \brief Return the number of unique samples within the annotation.
  NM_INLINE uint32_t getNumSamples() const;

  /// \brief Get the sample at the index position. Note that the annotation
  /// vertices are stored in the first m_numVertices positions.
  NM_INLINE float getSample(
    uint32_t index) const;

  /// \brief Set the sample at the index position. Note that the annotation
  /// vertices are stored in the first m_numVertices positions.
  NM_INLINE void setSample(
    uint32_t index,
    float val);

  /// \brief get access to the samples data arrays
  NM_INLINE const float* getSamples() const;
  NM_INLINE float* getSamples();

  /// \brief get access to the vertex source indices
  NM_INLINE const uint32_t* getVertexSourceIndices() const;
  NM_INLINE uint32_t* getVertexSourceIndices();

  NM_INLINE uint32_t getVertexSourceIndex(
    uint32_t index) const;

  NM_INLINE void setVertexSourceIndex(
    uint32_t index,
    uint32_t val);

  /// \brief Line sub-sample indices
  NM_INLINE uint32_t getNumLineSampleIndices() const;
  NM_INLINE const uint32_t* getLineSampleIndices() const;
  NM_INLINE uint32_t* getLineSampleIndices();

  NM_INLINE uint32_t getLineSubSampleIndex(
    uint32_t lineIndex,
    uint32_t lineSubSampleIndex) const;

  NM_INLINE void setLineSubSampleIndex(
    uint32_t lineIndex,
    uint32_t lineSubSampleIndex,
    uint32_t value);

  /// \brief Recover the sample indices corresponding to the major vertices of the line
  NM_INLINE void getLineSubSampleIndices(
    uint32_t lineIndex,
    uint32_t& sampleIndexA,
    uint32_t& sampleIndexB) const;

  /// \brief Annotation valid flags
  NM_INLINE bool getAnnotationValidFlags() const;

  NM_INLINE void setAnnotationValidFlags(
    bool validFlags);

private:
  // Information
  uint32_t      m_numSubDivisions;                ///< The number of tetrahedron sub-divisions used for sub-sampling.
  uint32_t      m_numSubSamplesPerLine;           ///< The number of sub-samples per tetrahedron.
  uint32_t      m_numVertices;                    ///< The number of annotated vertices
  uint32_t      m_numLines;                       ///< The number of annotated line segments
  uint32_t      m_numSubSamples;                  ///< The number of unique sub-division samples excluding the annotation vertices.
  bool          m_annotationValidFlags;           ///< Flag determining if the annotation data is valid

  // Sample data
  float*        m_samples;                        ///< Vertex source and line sub-samples
  uint32_t*     m_vertexSourceIndices;            ///< The pin indices for the vertex sources
  uint32_t*     m_lineSampleIndices;              ///< The numLines by numSubSamplesPerLine table of sub-samples for each line.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::SB1DAnnotationSubSampleRangeData
/// \brief This structure stores the range information for the annotation sub-samples
//----------------------------------------------------------------------------------------------------------------------
class SB1DAnnotationSubSampleRangeData
{
public:
  SB1DAnnotationSubSampleRangeData();
  ~SB1DAnnotationSubSampleRangeData();

  NM_INLINE void clear()
  {
    m_samplesRangeMin = 0.0f;
    m_samplesRangeMax = 0.0f;
    m_samplesCentre = 0.0f;
  }

public:
  float m_samplesRangeMin;
  float m_samplesRangeMax;
  float m_samplesCentre;
};

//----------------------------------------------------------------------------------------------------------------------
// SB1DAnnotationExportDataPacked inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB1DAnnotationExportDataPacked::getNumSubDivisions() const
{
  return m_numSubDivisions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB1DAnnotationExportDataPacked::getNumSubSamplesPerLine() const
{
  return m_numSubSamplesPerLine;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB1DAnnotationExportDataPacked::getNumVertices() const
{
  return m_numVertices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB1DAnnotationExportDataPacked::getNumLines() const
{
  return m_numLines;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB1DAnnotationExportDataPacked::getNumSubSamples() const
{
  return m_numSubSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB1DAnnotationExportDataPacked::getNumSamples() const
{
  return m_numVertices + m_numSubSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float SB1DAnnotationExportDataPacked::getSample(
  uint32_t index) const
{
  NMP_VERIFY(index < getNumSamples());
  return m_samples[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB1DAnnotationExportDataPacked::setSample(
  uint32_t index,
  float val)
{
  NMP_VERIFY(index < getNumSamples());
  m_samples[index] = val;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* SB1DAnnotationExportDataPacked::getSamples() const
{
  return m_samples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float* SB1DAnnotationExportDataPacked::getSamples()
{
  return m_samples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB1DAnnotationExportDataPacked::getVertexSourceIndices() const
{
  return m_vertexSourceIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB1DAnnotationExportDataPacked::getVertexSourceIndices()
{
  return m_vertexSourceIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB1DAnnotationExportDataPacked::getVertexSourceIndex(
  uint32_t index) const
{
  NMP_VERIFY(index < m_numVertices);
  return m_vertexSourceIndices[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB1DAnnotationExportDataPacked::setVertexSourceIndex(
  uint32_t index,
  uint32_t val)
{
  NMP_VERIFY(index < m_numVertices);
  m_vertexSourceIndices[index] = val;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB1DAnnotationExportDataPacked::getNumLineSampleIndices() const
{
  uint32_t numLineSubSampleIndices = m_numSubSamplesPerLine * m_numLines;
  return numLineSubSampleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* SB1DAnnotationExportDataPacked::getLineSampleIndices() const
{
  return m_lineSampleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* SB1DAnnotationExportDataPacked::getLineSampleIndices()
{
  return m_lineSampleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SB1DAnnotationExportDataPacked::getLineSubSampleIndex(
  uint32_t lineIndex,
  uint32_t lineSubSampleIndex) const
{
  NMP_VERIFY(lineIndex < getNumLines());
  NMP_VERIFY(lineSubSampleIndex < m_numSubSamplesPerLine);
  uint32_t index = lineIndex * m_numSubSamplesPerLine + lineSubSampleIndex;
  return m_lineSampleIndices[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB1DAnnotationExportDataPacked::setLineSubSampleIndex(
  uint32_t lineIndex,
  uint32_t lineSubSampleIndex,
  uint32_t value)
{
  NMP_VERIFY(lineIndex < getNumLines());
  NMP_VERIFY(lineSubSampleIndex < m_numSubSamplesPerLine);
  uint32_t index = lineIndex * m_numSubSamplesPerLine + lineSubSampleIndex;
  m_lineSampleIndices[index] = value;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB1DAnnotationExportDataPacked::getLineSubSampleIndices(
  uint32_t lineIndex,
  uint32_t& sampleIndexA,
  uint32_t& sampleIndexB) const
{
  // Get the line sub-sample index map
  NMP_VERIFY(lineIndex < getNumLines());
  const uint32_t* lineSampleIndices = &m_lineSampleIndices[lineIndex * m_numSubSamplesPerLine];

  // Samples
  sampleIndexA = lineSampleIndices[0];
  sampleIndexB = lineSampleIndices[m_numSubSamplesPerLine - 1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool SB1DAnnotationExportDataPacked::getAnnotationValidFlags() const
{
  return m_annotationValidFlags;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SB1DAnnotationExportDataPacked::setAnnotationValidFlags(bool validFlags)
{
  m_annotationValidFlags = validFlags;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_SCATTER_BLEND_1D_ANNOTATION_EXPORT_H
//----------------------------------------------------------------------------------------------------------------------
