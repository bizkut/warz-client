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
#include "morpheme/Nodes/mrNodeOperatorNoiseGen.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

// Flags for Noise Gen options (needs to match up with OperatorNoiseGen.lua)
static const uint32_t NOISE_GEN_FLAGS_NOISE   = 0x01;
static const uint32_t NOISE_GEN_FLAGS_SAWWAVE = 0x02;
static const uint32_t NOISE_GEN_FLAGS_EASE    = 0x08; // rather than 0x04 so as to be consistent with morpheme 1.3

// Noise generating functions
static float NoiseGenEase(float t);
static float NoiseGenSaw(float t);
static float NoiseGenNoise(float t, const AttribDataNoiseGenDef* const noiseGenData);

//----------------------------------------------------------------------------------------------------------------------
static bool updateNoiseGen(
  NodeDef*         node,
  AttribDataFloat* input,
  AttribDataFloat* output)
{
  // Get the node's data that we setup from the builder.
  const AttribDataNoiseGenDef* const noiseGen = node->getAttribData<AttribDataNoiseGenDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  
  // Based on the flags, generate the noise signal from the input value.
  float result = input->m_value;
  if (noiseGen->m_flags & NOISE_GEN_FLAGS_NOISE)
  {
    result = NoiseGenNoise((result + noiseGen->m_timeOffset) * noiseGen->m_noiseSawFrequency, noiseGen);
  }

  if (noiseGen->m_flags & NOISE_GEN_FLAGS_SAWWAVE)
  {
    result = NoiseGenSaw(result * noiseGen->m_noiseSawFrequency);
  }

  if (noiseGen->m_flags & NOISE_GEN_FLAGS_EASE)
  {
    result = NoiseGenEase(result);
  }

  output->m_value = result;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorNoiseGenOutputCPUpdateFloat(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  AttribDataFloat* inputCPFloat = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(0), animSet);
  
  // Get the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataFloat* outputCPFloat = outputCPPin->getAttribData<AttribDataFloat>();

  // Perform the actual task
  updateNoiseGen(node, inputCPFloat, outputCPFloat);

  return outputCPFloat;
}

//----------------------------------------------------------------------------------------------------------------------
static float NoiseGenNoise(float t, const AttribDataNoiseGenDef* const noiseGenData)
{
  // MORPH-21349: Replace with a proper noise generator.

  // Make local copies of the tables
  const float* const table1 = noiseGenData->m_table1;
  const float* const table2 = noiseGenData->m_table2;
  const float* const table3 = noiseGenData->m_table3;

  const uint32_t table1Size = AttribDataNoiseGenDef::TABLE_1_SIZE;
  const uint32_t table2Size = AttribDataNoiseGenDef::TABLE_2_SIZE;
  const uint32_t table3Size = AttribDataNoiseGenDef::TABLE_3_SIZE;

  const uint32_t repeatLength = table1Size * table2Size * table3Size;

  const float index = t * repeatLength;
  const uint32_t i = (uint32_t) index;
  const float weight = index - (float)i;
  const float invWeight = 1 - weight;

  const uint32_t i1_a =  i    % table1Size;
  const uint32_t i1_b = (i + 1) % table1Size;
  const uint32_t i2_a =  i    % table2Size;
  const uint32_t i2_b = (i + 1) % table2Size;
  const uint32_t i3_a =  i    % table3Size;
  const uint32_t i3_b = (i + 1) % table3Size;

  float noise = (invWeight * (table1[i1_a] + table2[i2_a] + table3[i3_a])) +
                (weight * (table1[i1_b] + table2[i2_b] + table3[i3_b]));

  noise /= 3.0f;

  return noise;
}

//----------------------------------------------------------------------------------------------------------------------
static float NoiseGenSaw(float t)
{
  const float remainder = fmodf(t, 1);

  // MORPH-21350: This should be done with fsel where possible
  if (remainder > 0.5f)
  {
    return 2.0f * (1 - remainder);
  }
  return 2.0f * remainder;
}

//----------------------------------------------------------------------------------------------------------------------
static float NoiseGenEase(float t)
{
  const float half = 0.5f;
  return half * (sinf(NM_PI * (t - half)) + 1.0f);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
