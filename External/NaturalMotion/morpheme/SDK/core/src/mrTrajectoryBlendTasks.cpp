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
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrBlendOps.h"
#ifdef NM_HOST_CELL_SPU
  #include "morphemeSPU/mrDispatcherSPU.h"
  #include "NMPlatform/ps3/NMSPUDMAController.h"
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
// Blend2 tasks
//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
// Disable 'Conditional expression is constant' warning, template parameters are deliberately compile-time constants.
  #pragma warning(push)
  #pragma warning(disable: 4127)
#endif // _MSC_VER
template <Blend2WeightingType _BlendWeighting, Blend2QuatBlendFunc _QuatBlendFunc, Blend2PosUpdateType _PosUpdateType>
MR_TRAJECTORY_BLEND_TASKS_FORCEINLINE
static void Blend2TrajectoryDeltaTransforms(Dispatcher::TaskParameters* parameters)
{
  AttribDataTrajectoryDeltaTransform* myTrajectoryDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataTrajectoryDeltaTransform* source0TrajectoryDeltaTransform = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataTrajectoryDeltaTransform* source1TrajectoryDeltaTransform = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);
  AttribDataFeatherBlend2ChannelAlphas* boneWeights = NULL;

  if (_BlendWeighting == BLEND2_WEIGHTING_PER_BONE)
  {
    boneWeights = parameters->getInputAttrib<AttribDataFeatherBlend2ChannelAlphas>(4, ATTRIB_SEMANTIC_BONE_WEIGHTS);
  }

  Blend2TrajectoryDeltaTransforms<_BlendWeighting, _QuatBlendFunc, _PosUpdateType>(
    &myTrajectoryDeltaTransform->m_deltaPos,
    &myTrajectoryDeltaTransform->m_deltaAtt,
    &myTrajectoryDeltaTransform->m_filteredOut,
    &source0TrajectoryDeltaTransform->m_deltaPos,
    &source0TrajectoryDeltaTransform->m_deltaAtt,
    source0TrajectoryDeltaTransform->m_filteredOut,
    &source1TrajectoryDeltaTransform->m_deltaPos,
    &source1TrajectoryDeltaTransform->m_deltaAtt,
    source1TrajectoryDeltaTransform->m_filteredOut,
    blendWeights->m_trajectoryAndTransformsWeights[0],
    boneWeights);
}
#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryDeltaTransformsAddAttAddPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::additiveBlendQuats, BLEND2_ADD_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryDeltaTransformsSubtractAttSubtractPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::subtractiveBlendQuats, BLEND2_SUBTRACT_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryDeltaTransformsInterpAttSubtractPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::interpBlendQuats, BLEND2_SUBTRACT_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryDeltaTransformsSubtractAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::subtractiveBlendQuats, BLEND2_LERP_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryDeltaTransformsAddAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::additiveBlendQuats, BLEND2_LERP_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryDeltaTransformsAddAttSlerpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::additiveBlendQuats, BLEND2_SLERP_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryDeltaTransformsSubtractAttSlerpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::subtractiveBlendQuats, BLEND2_SLERP_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryDeltaTransformsInterpAttAddPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::interpBlendQuats, BLEND2_ADD_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryDeltaTransformsInterpAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::interpBlendQuats, BLEND2_LERP_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryDeltaTransformsInterpAttSlerpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
// Feather blend2 tasks
//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaTransformsAddAttAddPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::additiveBlendQuats, BLEND2_ADD_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaTransformsAddAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::additiveBlendQuats, BLEND2_LERP_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaTransformsAddAttSlerpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::additiveBlendQuats, BLEND2_SLERP_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaTransformsInterpAttAddPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::interpBlendQuats, BLEND2_ADD_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaTransformsInterpAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::interpBlendQuats, BLEND2_LERP_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaTransformsInterpAttSlerpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
// Blend2x2 tasks
//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2x2TrajectoryDeltaTransformsInterpAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  AttribDataTrajectoryDeltaTransform* trajectoryDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataTrajectoryDeltaTransform* sourceTrajectoryDeltaTransform[4] =
  {
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM),
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM),
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM),
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(4, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM)
  };
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(5, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 2);

  // Four way blend implemented as three two way blends
  NMP::Quat interpQuatTmp01, interpQuatTmp23;
  NMP::Vector3 interpVecTmp01, interpVecTmp23;
  bool filteredOut01, filteredOut23;

  BlendOps::blend2PartialInterpQuatInterpPos(
    &interpQuatTmp01,
    &interpVecTmp01,
    &filteredOut01,
    &sourceTrajectoryDeltaTransform[0]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[0]->m_deltaPos,
    sourceTrajectoryDeltaTransform[0]->m_filteredOut,
    &sourceTrajectoryDeltaTransform[1]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[1]->m_deltaPos,
    sourceTrajectoryDeltaTransform[1]->m_filteredOut,
    blendWeights->m_trajectoryAndTransformsWeights[0]);
  BlendOps::blend2PartialInterpQuatInterpPos(
    &interpQuatTmp23,
    &interpVecTmp23,
    &filteredOut23,
    &sourceTrajectoryDeltaTransform[2]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[2]->m_deltaPos,
    sourceTrajectoryDeltaTransform[2]->m_filteredOut,
    &sourceTrajectoryDeltaTransform[3]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[3]->m_deltaPos,
    sourceTrajectoryDeltaTransform[3]->m_filteredOut,
    blendWeights->m_trajectoryAndTransformsWeights[0]);
  BlendOps::blend2PartialInterpQuatInterpPos(
    &trajectoryDeltaTransform->m_deltaAtt,
    &trajectoryDeltaTransform->m_deltaPos,
    &trajectoryDeltaTransform->m_filteredOut,
    &interpQuatTmp01,
    &interpVecTmp01,
    filteredOut01,
    &interpQuatTmp23,
    &interpVecTmp23,
    filteredOut23,
    blendWeights->m_trajectoryAndTransformsWeights[1]);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2x2TrajectoryDeltaTransformsInterpAttSlerpPos(Dispatcher::TaskParameters* parameters)
{
  AttribDataTrajectoryDeltaTransform* trajectoryDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataTrajectoryDeltaTransform* sourceTrajectoryDeltaTransform[4] =
  {
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM),
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM),
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM),
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(4, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM)
  };
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(5, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 2);

  // Four way blend implemented as three two way blends
  NMP::Quat interpQuatTmp01, interpQuatTmp23;
  NMP::Vector3 interpVecTmp01, interpVecTmp23;
  bool filteredOut01, filteredOut23;

  BlendOps::blend2PartialInterpQuatSlerpPos(
    &interpQuatTmp01,
    &interpVecTmp01,
    &filteredOut01,
    &sourceTrajectoryDeltaTransform[0]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[0]->m_deltaPos,
    sourceTrajectoryDeltaTransform[0]->m_filteredOut,
    &sourceTrajectoryDeltaTransform[1]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[1]->m_deltaPos,
    sourceTrajectoryDeltaTransform[1]->m_filteredOut,
    blendWeights->m_trajectoryAndTransformsWeights[0]);
  BlendOps::blend2PartialInterpQuatSlerpPos(
    &interpQuatTmp23,
    &interpVecTmp23,
    &filteredOut23,
    &sourceTrajectoryDeltaTransform[2]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[2]->m_deltaPos,
    sourceTrajectoryDeltaTransform[2]->m_filteredOut,
    &sourceTrajectoryDeltaTransform[3]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[3]->m_deltaPos,
    sourceTrajectoryDeltaTransform[3]->m_filteredOut,
    blendWeights->m_trajectoryAndTransformsWeights[0]);
  BlendOps::blend2PartialInterpQuatSlerpPos(
    &trajectoryDeltaTransform->m_deltaAtt,
    &trajectoryDeltaTransform->m_deltaPos,
    &trajectoryDeltaTransform->m_filteredOut,
    &interpQuatTmp01,
    &interpVecTmp01,
    filteredOut01,
    &interpQuatTmp23,
    &interpVecTmp23,
    filteredOut23,
    blendWeights->m_trajectoryAndTransformsWeights[1]);
}

//----------------------------------------------------------------------------------------------------------------------
// Triangle blend tasks
//----------------------------------------------------------------------------------------------------------------------
void TaskTriangleBlendTrajectoryDeltaTransformsInterpAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  AttribDataTrajectoryDeltaTransform* trajectoryDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  const AttribDataTrajectoryDeltaTransform* sourceTrajectoryDeltaTransform[3] =
  {
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM),
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM),
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM)
  };
  const AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 2);

  // Triangle blend implemented as a pair of two way blends
  NMP::Quat interpQuatTmp;
  NMP::Vector3 interpVecTmp;
  bool filteredOutTmp;

  BlendOps::blend2PartialInterpQuatInterpPos(
    &interpQuatTmp,
    &interpVecTmp,
    &filteredOutTmp,
    &sourceTrajectoryDeltaTransform[0]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[0]->m_deltaPos,
    sourceTrajectoryDeltaTransform[0]->m_filteredOut,
    &sourceTrajectoryDeltaTransform[1]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[1]->m_deltaPos,
    sourceTrajectoryDeltaTransform[1]->m_filteredOut,
    blendWeights->m_trajectoryAndTransformsWeights[0]);
  BlendOps::blend2PartialInterpQuatInterpPos(
    &trajectoryDeltaTransform->m_deltaAtt,
    &trajectoryDeltaTransform->m_deltaPos,
    &trajectoryDeltaTransform->m_filteredOut,
    &interpQuatTmp,
    &interpVecTmp,
    filteredOutTmp,
    &sourceTrajectoryDeltaTransform[2]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[2]->m_deltaPos,
    sourceTrajectoryDeltaTransform[2]->m_filteredOut,
    blendWeights->m_trajectoryAndTransformsWeights[1]);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTriangleBlendTrajectoryDeltaTransformsInterpAttSlerpPos(Dispatcher::TaskParameters* parameters)
{
  AttribDataTrajectoryDeltaTransform* trajectoryDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  const AttribDataTrajectoryDeltaTransform* sourceTrajectoryDeltaTransform[3] =
  {
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM),
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM),
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM)
  };
  const AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 2);

  // Triangle blend implemented as a pair of two way blends
  NMP::Quat interpQuatTmp;
  NMP::Vector3 interpVecTmp;
  bool filteredOutTmp;

  BlendOps::blend2PartialInterpQuatSlerpPos(
    &interpQuatTmp,
    &interpVecTmp,
    &filteredOutTmp,
    &sourceTrajectoryDeltaTransform[0]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[0]->m_deltaPos,
    sourceTrajectoryDeltaTransform[0]->m_filteredOut,
    &sourceTrajectoryDeltaTransform[1]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[1]->m_deltaPos,
    sourceTrajectoryDeltaTransform[1]->m_filteredOut,
    blendWeights->m_trajectoryAndTransformsWeights[0]);
  BlendOps::blend2PartialInterpQuatSlerpPos(
    &trajectoryDeltaTransform->m_deltaAtt,
    &trajectoryDeltaTransform->m_deltaPos,
    &trajectoryDeltaTransform->m_filteredOut,
    &interpQuatTmp,
    &interpVecTmp,
    filteredOutTmp,
    &sourceTrajectoryDeltaTransform[2]->m_deltaAtt,
    &sourceTrajectoryDeltaTransform[2]->m_deltaPos,
    sourceTrajectoryDeltaTransform[2]->m_filteredOut,
    blendWeights->m_trajectoryAndTransformsWeights[1]);
}

//----------------------------------------------------------------------------------------------------------------------
// Blend all tasks
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void subTaskBlendAllTrajectoryDeltaTransformsInterpAttPos(
  Dispatcher::TaskParameters* parameters,
  bool slerpTrajectory)
{
  // Output delta trajectory
  AttribDataTrajectoryDeltaTransform* myTrajectoryDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  myTrajectoryDeltaTransform->m_filteredOut = false;
  
  // Template helper so we don't have to duplicate all this code for the slerp / lerp case
  // Get temp allocator. Note: requires that this is a FastHeapAllocator.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  uint32_t parameterIndex = 0;
  // Get number of connections
  uint32_t numConnections = parameters->m_numParameters - 2;

  // We can optimize for different counts of filtered delta trajectory inputs
  uint32_t numFilteredOut = 0;
  // Source delta trajectories
  NMP::Memory::Format memReq(sizeof(AttribDataTrajectoryDeltaTransform*) * numConnections, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memRes = NMPAllocatorAllocateFromFormat(childAllocator, memReq);
  AttribDataTrajectoryDeltaTransform** sourceTrajectoryDeltaTransform = (AttribDataTrajectoryDeltaTransform**)memRes.ptr;
  for (uint32_t i = 0; i < numConnections; ++i)
  {
    sourceTrajectoryDeltaTransform[i] = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(++parameterIndex, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
    if (sourceTrajectoryDeltaTransform[i]->m_filteredOut)
      numFilteredOut++;
  };

  // Blend weights
  AttribDataFloatArray* weights = parameters->getInputAttrib<AttribDataFloatArray>(++parameterIndex, ATTRIB_SEMANTIC_CP_FLOAT);

  // Classify cases depending on filtered inputs
  if (numFilteredOut == numConnections) // All sources have been filtered out, empty output
  {
      myTrajectoryDeltaTransform->m_deltaAtt.identity();
      myTrajectoryDeltaTransform->m_deltaPos.setToZero();
      myTrajectoryDeltaTransform->m_filteredOut = true;
  }
  else if ( numFilteredOut == numConnections-1) // One source present, just copy
  {
    for (uint32_t i = 0; i < numConnections; i++)
    {
      if (sourceTrajectoryDeltaTransform[i]->m_filteredOut == false)
      {
        myTrajectoryDeltaTransform->m_deltaAtt = sourceTrajectoryDeltaTransform[i]->m_deltaAtt;
        myTrajectoryDeltaTransform->m_deltaPos = sourceTrajectoryDeltaTransform[i]->m_deltaPos;
        break;
      }
    }
  }
  else
  {
    uint32_t sourceIndex = 0;
    NMP::Quat interpQuatTmp;
    NMP::Vector3 interpVecTmp;
    float weight = 0.0f;
    float accumulatedWeight = 0.0f;
    // Find the first of the present sources
    while ((sourceIndex < numConnections) && (sourceTrajectoryDeltaTransform[sourceIndex]->m_filteredOut))
    {
      ++sourceIndex;
    }
    interpQuatTmp = sourceTrajectoryDeltaTransform[sourceIndex]->m_deltaAtt;
    interpVecTmp = sourceTrajectoryDeltaTransform[sourceIndex]->m_deltaPos;
    accumulatedWeight += weights->m_values[sourceIndex];
    ++sourceIndex;
    while (sourceIndex < numConnections)
    {
      // Find next of the present sources
      while ((sourceIndex < numConnections) && (sourceTrajectoryDeltaTransform[sourceIndex]->m_filteredOut))
      {
        ++sourceIndex;
      }
      // normalised blend weight
      weight = accumulatedWeight / (accumulatedWeight + weights->m_values[sourceIndex]);
      // Perform the actual trajectory blending
      BlendOps::interpBlendQuats(
                &myTrajectoryDeltaTransform->m_deltaAtt,
                &interpQuatTmp,
                &sourceTrajectoryDeltaTransform[sourceIndex]->m_deltaAtt,
                1.0f - weight);
      if (slerpTrajectory)
      {
        myTrajectoryDeltaTransform->m_deltaPos.slerp(
                                                interpVecTmp,
                                                sourceTrajectoryDeltaTransform[sourceIndex]->m_deltaPos,
                                                1.0f - weight);
      }
      else
      {
        myTrajectoryDeltaTransform->m_deltaPos.lerp(
                                                interpVecTmp,
                                                sourceTrajectoryDeltaTransform[sourceIndex]->m_deltaPos,
                                                1.0f - weight);
      }
      interpQuatTmp = myTrajectoryDeltaTransform->m_deltaAtt;
      interpVecTmp = myTrajectoryDeltaTransform->m_deltaPos;
      accumulatedWeight += weights->m_values[sourceIndex];
      ++sourceIndex;
    }
  }
  
  //--------------------------------------------
  // Release working memory
  childAllocator->memFree(sourceTrajectoryDeltaTransform);
  tempAllocator->destroyChildAllocator(childAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlendAllTrajectoryDeltaTransformsInterpAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  subTaskBlendAllTrajectoryDeltaTransformsInterpAttPos(parameters, false);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlendAllTrajectoryDeltaTransformsInterpAttSlerpPos(Dispatcher::TaskParameters* parameters)
{
  subTaskBlendAllTrajectoryDeltaTransformsInterpAttPos(parameters, true);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
