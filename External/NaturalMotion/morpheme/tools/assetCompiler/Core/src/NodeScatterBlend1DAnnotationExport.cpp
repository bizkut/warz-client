// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "NodeScatterBlend1DAnnotationExport.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// SB1DAnnotationExportDataPacked
//----------------------------------------------------------------------------------------------------------------------
SB1DAnnotationExportDataPacked::SB1DAnnotationExportDataPacked() :
  m_numSubDivisions(0),
  m_numSubSamplesPerLine(0),
  m_numVertices(0),
  m_numLines(0),
  m_numSubSamples(0),
  m_annotationValidFlags(false),
  m_samples(NULL),
  m_vertexSourceIndices(NULL),
  m_lineSampleIndices(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
SB1DAnnotationExportDataPacked::~SB1DAnnotationExportDataPacked()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void SB1DAnnotationExportDataPacked::release()
{
  m_numSubDivisions = 0;
  m_numSubSamplesPerLine = 0;
  m_numVertices = 0;
  m_numLines = 0;
  m_numSubSamples = 0;
  m_annotationValidFlags = false;

  if (m_samples)
  {
    delete[] m_samples;
    m_samples = NULL;
  }

  if (m_vertexSourceIndices)
  {
    delete[] m_vertexSourceIndices;
    m_vertexSourceIndices = NULL;
  }

  if (m_lineSampleIndices)
  {
    delete[] m_lineSampleIndices;
    m_lineSampleIndices = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB1DAnnotationExportDataPacked::init(
  uint32_t numSubDivisions,
  uint32_t numSubSamplesPerLine,
  uint32_t numVertices,
  uint32_t numLines,
  uint32_t numSubSamples)
{
  NMP_VERIFY(m_numSubSamplesPerLine == 0); // Must have been released
  NMP_VERIFY(numSubDivisions <= 2); // We don't support more sub-divisions

  // Information
  m_numSubDivisions = numSubDivisions;
  m_numSubSamplesPerLine = numSubSamplesPerLine;
  m_numVertices = numVertices;
  m_numLines = numLines;
  m_numSubSamples = numSubSamples;

  //---------------------------
  // Allocate the memory for the samples data
  uint32_t numSamples = numVertices + numSubSamples;
  if (numSamples > 0)
  {
    m_samples = new float[numSamples];
    // Clear the sample data
    for (uint32_t k = 0; k < numSamples; ++k)
    {
      m_samples[k] = 0.0f;
    }
  }

  // Allocate the memory for the vertex source indices
  if (numVertices > 0)
  {
    m_vertexSourceIndices = new uint32_t[numVertices];
    // Clear the indices
    for (uint32_t i = 0; i < numVertices; ++i)
    {
      m_vertexSourceIndices[i] = 0;
    }
  }

  // Allocate the memory for the line segment data
  if (numLines > 0)
  {
    NMP_VERIFY(numSubSamplesPerLine > 0);
    uint32_t numLineSubSampleIndices = numSubSamplesPerLine * numLines;
    m_lineSampleIndices = new uint32_t[numLineSubSampleIndices];

    // Invalidate the tetrahedron sub-sample indices
    for (uint32_t i = 0; i < numLineSubSampleIndices; ++i)
      m_lineSampleIndices[i] = 0xFFFFFFFF;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SB1DAnnotationExportDataPacked::writeFile(const char* filename) const
{
  NMP_VERIFY(filename);
  FILE* fp = fopen(filename, "w");
  if (fp)
  {
    fprintf(fp, "m_numSubDivisions = %d\n", m_numSubDivisions);
    fprintf(fp, "m_numSubSamplesPerLine = %d\n", m_numSubSamplesPerLine);
    fprintf(fp, "m_numVertices = %d\n", m_numVertices);
    fprintf(fp, "m_numLines = %d\n", m_numLines);
    fprintf(fp, "m_numSubSamples = %d\n", m_numSubSamples);

    //---------------------------
    fprintf(fp, "\nSamples:\n");
    uint32_t numSamples = getNumSamples();
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      float val = getSample(i);
      fprintf(fp, "%f\n", val);
    }

    //---------------------------
    fprintf(fp, "\nVertexSourceIndices:\n");
    for (uint32_t i = 0; i < m_numVertices; ++i)
    {
      uint32_t val = getVertexSourceIndex(i);
      fprintf(fp, "%d\n", val);
    }

    //---------------------------
    fprintf(fp, "\nLineSampleIndices:\n");
    uint32_t numLines = getNumLines();
    if (numLines > 0)
    {
      for (uint32_t lineIndex = 0; lineIndex < numLines; ++lineIndex)
      {
        for( uint32_t lineSubSampleIndex = 0; lineSubSampleIndex < m_numSubSamplesPerLine; ++lineSubSampleIndex)
        {
          uint32_t sampleIndex = getLineSubSampleIndex(
            lineIndex,
            lineSubSampleIndex);
          fprintf(fp, "%d ", sampleIndex);
        }
        fprintf(fp, "\n");
      }
    }

    fclose(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SB1DAnnotationSubSampleRangeData
//----------------------------------------------------------------------------------------------------------------------
SB1DAnnotationSubSampleRangeData::SB1DAnnotationSubSampleRangeData() :
  m_samplesRangeMin(0.0f),
  m_samplesRangeMax(0.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
SB1DAnnotationSubSampleRangeData::~SB1DAnnotationSubSampleRangeData()
{
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
