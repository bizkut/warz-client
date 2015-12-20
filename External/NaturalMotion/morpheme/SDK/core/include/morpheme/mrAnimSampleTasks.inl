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
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
typedef void (*ComputeTransformsAtTimeFunc)(
  const AnimSourceBase* sourceAnimation,       ///< IN: Animation to sample.
  float                 time,                  ///< IN: Time at which to sample the animation (seconds).
  const AnimRigDef*     rig,                   ///< IN: Hierarchy and bind poses (not used)
  const RigToAnimMap*   mapFromRigToAnim,      ///< IN: So that results from sampled anim channels can be
                                               ///<     stored in the correct rig bone ordered transform buffer slot.
  uint32_t              outputSubsetSize,      ///< IN: \see outputSubsetArray
  const uint16_t*       outputSubsetArray,     ///< IN: Channel set values will only be calculated if
                                               ///<     output indexes appear both in this array and the 
                                               ///<     mapping array. Ignored if size is zero.
  NMP::DataBuffer*      outputTransformBuffer, ///< OUT: Calculated transform data is stored and returned
                                               ///<      in this provided set.
  NMP::MemoryAllocator* allocator);

//----------------------------------------------------------------------------------------------------------------------
typedef void (*ComputeTrajectoryAtTimeFunc)(
  const TrajectorySourceBase* trajectoryControl,
  float                       bufferPlaybackPos,
  NMP::Quat&                  resultQuat,
  NMP::Vector3&               resultPos);

//----------------------------------------------------------------------------------------------------------------------
template <typename AnimSourceType>
NM_INLINE void subTaskSampleTransformsFromAnimSource(
  Dispatcher::TaskParameters* parameters,
  AnimType NMP_USED_FOR_ASSERTS(animType),
  ComputeTransformsAtTimeFunc computeTransformsFunc,
  bool transformComputationRequiresRig)
{
  AttribDataPlaybackPos* animSamplePos = parameters->getInputAttrib<AttribDataPlaybackPos>(0, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS);
  AttribDataSourceAnim* sourceAnim = parameters->getInputAttrib<AttribDataSourceAnim>(1, ATTRIB_SEMANTIC_SOURCE_ANIM);

  uint32_t outputParamIndex;
  AnimRigDef* rigDef;
  if (transformComputationRequiresRig)
  {
    rigDef = parameters->getInputAttrib<AttribDataRig>(2, ATTRIB_SEMANTIC_RIG)->m_rig;
    outputParamIndex = 3;
  }
  else
  {
    outputParamIndex = 2;
    rigDef = NULL;
  }

#ifdef NM_HOST_CELL_SPU
  AttribDataSourceAnim::prepAnimForSPU(sourceAnim, parameters->m_dispatcher->getTempMemoryAllocator());
  AttribDataSourceAnim::prepRigToAnimMapForSPU(sourceAnim, parameters->m_dispatcher->getTempMemoryAllocator());

  // Sort out all internal pointers in the anim.
  AnimSourceType* animSource = (AnimSourceType*)sourceAnim->m_anim;
  animSource->relocate();
#endif // NM_HOST_CELL_SPU

  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(
    outputParamIndex,
    sourceAnim->m_rigToAnimMap->getNumRigBones(), ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  NMP_ASSERT(sourceAnim->m_anim && sourceAnim->m_anim->getType() == animType);

  // Update the transforms themselves
  computeTransformsFunc(
    sourceAnim->m_anim,
    animSamplePos->m_currentPosAdj,
    rigDef,
    sourceAnim->m_rigToAnimMap,
    0,
    NULL,
    transforms->m_transformBuffer,
    parameters->m_dispatcher->getTempMemoryAllocator());
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void UpdateDeltaTrajectories(
  NMP::Vector3*          trajectoryDeltaPos,
  NMP::Quat*             trajectoryAttitude,
  AttribDataSourceAnim*  sourceAnim,
  AttribDataPlaybackPos* animSamplePos,
  AttribDataBool*        loopedOnLastUpdate,
  NMP::Quat&             lastUpdateTrajectoryQuaternion,
  NMP::Vector3&          lastUpdateTrajectoryTranslation,
  NMP::Quat&             currentUpdateTrajectoryAttitude,
  NMP::Vector3&          currentUpdateTrajectoryTranslation)
{ 
  if (animSamplePos->m_setWithAbs)
  {
    //--------------------------------
    // On the first update we only initialise and deltas will be set to zero.
    trajectoryDeltaPos->setToZero();
    trajectoryAttitude->identity();
  }
  else
  {
    if (!loopedOnLastUpdate->m_value)
    {
      //--------------------------------
      // Subtract the last frames transform from this frames transform to get the delta transform.

      // -- Translation --
      *trajectoryDeltaPos = currentUpdateTrajectoryTranslation - lastUpdateTrajectoryTranslation;
      // Delta translation is relative to the starting attitude (m_LastUpdateTrajectoryQuaternion)
      *trajectoryDeltaPos =
        lastUpdateTrajectoryQuaternion.inverseRotateVector(*trajectoryDeltaPos);

      // -- Rotation --
      // Invert last rotation.
      NMP::Quat inverseLastAttitude;
      inverseLastAttitude = lastUpdateTrajectoryQuaternion;
      inverseLastAttitude.conjugate();
      // multiply the 2 quats to get the rotational difference.
      *trajectoryAttitude = inverseLastAttitude * currentUpdateTrajectoryAttitude;
    }
    else
    {
      //--------------------------------
      // The animation has looped so we need to offset the distance between start
      // and end so we don't get a big jump in the roots position

      // If playing backwards we need to swap the start and end transform values.
      NMP::Vector3  trajectoryEndTranslation;
      NMP::Quat     trajectoryEndRotation;
      NMP::Vector3  trajectoryStartTranslation;
      NMP::Quat     trajectoryStartRotation;
      if (sourceAnim->m_playBackwards)
      {
        trajectoryEndRotation       = sourceAnim->m_transformAtStartQuat;
        trajectoryEndTranslation    = sourceAnim->m_transformAtStartPos;
        trajectoryStartRotation     = sourceAnim->m_transformAtEndQuat;
        trajectoryStartTranslation  = sourceAnim->m_transformAtEndPos;
      }
      else
      {
        trajectoryEndRotation       = sourceAnim->m_transformAtEndQuat;
        trajectoryEndTranslation    = sourceAnim->m_transformAtEndPos;
        trajectoryStartRotation     = sourceAnim->m_transformAtStartQuat;
        trajectoryStartTranslation  = sourceAnim->m_transformAtStartPos;
      }

      // -- End section translation --
      NMP::Vector3    startSectionTransDelta;
      NMP::Vector3    endSectionTransDelta;
      NMP::Quat       startSectionAttDelta;
      NMP::Quat       endSectionAttDelta;
      endSectionTransDelta = trajectoryEndTranslation - lastUpdateTrajectoryTranslation;
      endSectionTransDelta = lastUpdateTrajectoryQuaternion.inverseRotateVector(endSectionTransDelta);

      // -- Start section translation --
      startSectionTransDelta = currentUpdateTrajectoryTranslation - trajectoryStartTranslation;
      startSectionTransDelta = trajectoryStartRotation.inverseRotateVector(startSectionTransDelta);

      // -- End section rotation --
      NMP::Quat inverseLastAttitude;
      inverseLastAttitude = lastUpdateTrajectoryQuaternion;
      inverseLastAttitude.conjugate();
      endSectionAttDelta = inverseLastAttitude * trajectoryEndRotation;

      // -- Start section rotation --
      inverseLastAttitude = trajectoryStartRotation;
      inverseLastAttitude.conjugate();
      startSectionAttDelta = inverseLastAttitude * currentUpdateTrajectoryAttitude;

      // -- Add the start and end sections to get the delta transform.
      *trajectoryAttitude = startSectionAttDelta * endSectionAttDelta;
      *trajectoryDeltaPos = startSectionTransDelta + endSectionTransDelta;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <typename TrajectorySourceType>
NM_INLINE void subTaskUpdateDeltaTrajectoryFromTrajectorySource(
  Dispatcher::TaskParameters* parameters,
  AnimType NMP_USED_FOR_ASSERTS(animType),
  ComputeTrajectoryAtTimeFunc computeTrajectoryFunc)
{
  AttribDataTrajectoryDeltaTransform* trajectoryDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataSourceAnim* sourceAnim = parameters->getInputAttrib<AttribDataSourceAnim>(1, ATTRIB_SEMANTIC_SOURCE_ANIM);
  AttribDataPlaybackPos* animSamplePos = parameters->getInputAttrib<AttribDataPlaybackPos>(2, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS);
  AttribDataBool* loopedOnLastUpdate = parameters->getInputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE);

  if (!sourceAnim->m_sourceTrajectoryChannel)
  {
    // This animation does not contain a channel for the trajectory bone so it cannot contribute to trajectory deltas.
    trajectoryDeltaTransform->m_deltaAtt.identity();
    trajectoryDeltaTransform->m_deltaPos.setToZero();
    trajectoryDeltaTransform->m_filteredOut = true;
  }
  else
  {
#ifdef NM_HOST_CELL_SPU
    NMP_ASSERT(parameters->m_dispatcher);
    AttribDataSourceAnim::prepTrajectoryForSPU(sourceAnim, parameters->m_dispatcher->getTempMemoryAllocator());

    // Sort out all internal pointers in the trajectory channel.
    TrajectorySourceType* trajectorySource = (TrajectorySourceType*)sourceAnim->m_sourceTrajectoryChannel;
    trajectorySource->relocate();
#endif // NM_HOST_CELL_SPU

    NMP_ASSERT(sourceAnim->m_sourceTrajectoryChannel->getType() == animType);

    // Compute the root transform at the last update time (for calculating delta transform).
    NMP::Quat     lastUpdateTrajectoryQuaternion;
    NMP::Vector3  lastUpdateTrajectoryTranslation;
    computeTrajectoryFunc(
      sourceAnim->m_sourceTrajectoryChannel,
      animSamplePos->m_previousPosAdj,
      lastUpdateTrajectoryQuaternion,
      lastUpdateTrajectoryTranslation);

    // Get trajectory channels current transform.
    NMP::Quat     currentUpdateTrajectoryAttitude;
    NMP::Vector3  currentUpdateTrajectoryTranslation;
    computeTrajectoryFunc(
      sourceAnim->m_sourceTrajectoryChannel,
      animSamplePos->m_currentPosAdj,
      currentUpdateTrajectoryAttitude,
      currentUpdateTrajectoryTranslation);

    UpdateDeltaTrajectories(
      &trajectoryDeltaTransform->m_deltaPos,
      &trajectoryDeltaTransform->m_deltaAtt,
      sourceAnim,
      animSamplePos,
      loopedOnLastUpdate,
      lastUpdateTrajectoryQuaternion,
      lastUpdateTrajectoryTranslation,
      currentUpdateTrajectoryAttitude,
      currentUpdateTrajectoryTranslation);

    trajectoryDeltaTransform->m_filteredOut = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <typename AnimSourceType, typename TrajectorySourceType>
NM_INLINE void subTaskUpdateDeltaTrajectoryAndTransformsFromSource(
  Dispatcher::TaskParameters* parameters,
  AnimType NMP_USED_FOR_ASSERTS(animType),
  ComputeTransformsAtTimeFunc computeTransformsFunc,
  ComputeTrajectoryAtTimeFunc computeTrajectoryFunc,
  bool transformComputationRequiresRig)
{
  AttribDataSourceAnim* sourceAnim = parameters->getInputAttrib<AttribDataSourceAnim>(1, ATTRIB_SEMANTIC_SOURCE_ANIM);
  AttribDataPlaybackPos* animSamplePos = parameters->getInputAttrib<AttribDataPlaybackPos>(2, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS);
  AttribDataBool* loopedOnLastUpdate = parameters->getInputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE);
  AnimRigDef* rig = 0;
  if (transformComputationRequiresRig)
  {
    rig = parameters->getInputAttrib<AttribDataRig>(4, ATTRIB_SEMANTIC_RIG)->m_rig;
  }

#ifdef NM_HOST_CELL_SPU
  AttribDataSourceAnim::prepAnimForSPU(sourceAnim, parameters->m_dispatcher->getTempMemoryAllocator());
  AttribDataSourceAnim::prepRigToAnimMapForSPU(sourceAnim, parameters->m_dispatcher->getTempMemoryAllocator());

  // Sort out all internal pointers in the anim.
  AnimSourceType* animSource = (AnimSourceType*)sourceAnim->m_anim;
  animSource->relocate();
#endif // NM_HOST_CELL_SPU

  const uint32_t numRigBones = sourceAnim->m_rigToAnimMap->getNumRigBones();
  AttribDataTransformBuffer* trajectoryDeltaTransformBuffer = parameters->createOutputAttribReplaceTransformBuffer(
    0,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    numRigBones);
  NMP_ASSERT(sourceAnim->m_anim && sourceAnim->m_anim->getType() == animType);
  NMP::DataBuffer* transforms = trajectoryDeltaTransformBuffer->m_transformBuffer;

  // Update the transforms themselves
  computeTransformsFunc(
    sourceAnim->m_anim,
    animSamplePos->m_currentPosAdj,
    rig,
    sourceAnim->m_rigToAnimMap,
    0,
    NULL,
    transforms,
    parameters->m_dispatcher->getTempMemoryAllocator());

  if (!sourceAnim->m_sourceTrajectoryChannel)
  {
    // This animation does not contain a channel for the trajectory bone so it cannot contribute to trajectory deltas.
    transforms->getChannelQuat(0)->identity();
    transforms->getChannelPos(0)->setToZero();
    transforms->setChannelUnused(0);
  }
  else
  {
#ifdef NM_HOST_CELL_SPU
    AttribDataSourceAnim::prepTrajectoryForSPU(sourceAnim, parameters->m_dispatcher->getTempMemoryAllocator());

    // Sort out all internal pointers in the trajectory channel.
    TrajectorySourceType* trajectorySource = (TrajectorySourceType*)sourceAnim->m_sourceTrajectoryChannel;
    trajectorySource->relocate();
#endif // NM_HOST_CELL_SPU

    NMP_ASSERT(sourceAnim->m_sourceTrajectoryChannel->getType() == animType);

    // Compute the root transform at the last update time (for calculating delta transform).
    NMP::Quat     lastUpdateTrajectoryQuaternion;
    NMP::Vector3  lastUpdateTrajectoryTranslation;
    computeTrajectoryFunc(
      sourceAnim->m_sourceTrajectoryChannel,
      animSamplePos->m_previousPosAdj,
      lastUpdateTrajectoryQuaternion,
      lastUpdateTrajectoryTranslation);

    // Get trajectory channels current transform.
    NMP::Quat     currentUpdateTrajectoryAttitude;
    NMP::Vector3  currentUpdateTrajectoryTranslation;
    computeTrajectoryFunc(
      sourceAnim->m_sourceTrajectoryChannel,
      animSamplePos->m_currentPosAdj,
      currentUpdateTrajectoryAttitude,
      currentUpdateTrajectoryTranslation);

    // Now sample the trajectory delta using the format specific sample function
    UpdateDeltaTrajectories(
      transforms->getChannelPos(0),
      transforms->getChannelQuat(0),
      sourceAnim,
      animSamplePos,
      loopedOnLastUpdate,
      lastUpdateTrajectoryQuaternion,
      lastUpdateTrajectoryTranslation,
      currentUpdateTrajectoryAttitude,
      currentUpdateTrajectoryTranslation);

    transforms->setChannelUsed(0);
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
