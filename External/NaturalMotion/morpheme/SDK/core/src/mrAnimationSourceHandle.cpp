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
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBuffer.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrAnimationSourceHandle.h"
#include "morpheme/AnimSource/mrAnimSource.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AnimationSourceHandle::AnimationSourceHandle() :
  m_animation(NULL),
  m_buffer(NULL),
  m_localTransforms(NULL),
  m_transformCount(0),
  m_rig(NULL),
  m_rigToAnimMap(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
AnimationSourceHandle::~AnimationSourceHandle()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSourceHandle::clear()
{
  if (m_animation)
  {
    NMP::Memory::memFree(m_animation);
    m_animation = NULL;
  }

  if (m_buffer)
  {
    // MORPH-21358: Currently m_animation is deleting this data
    //delete [] m_buffer;
  }
  m_buffer = NULL;
  if (m_localTransforms)
  {
    NMP::Memory::memFree(m_localTransforms);
    m_localTransforms = NULL;
  }
  m_transformCount = 0;

  if (m_rig)
  {
    NMP::Memory::memFree(m_rig);
    m_rig = NULL;
  }

  if (m_rigToAnimMap)
  {
    NMP::Memory::memFree(m_rigToAnimMap);
    m_rigToAnimMap = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimationSourceHandle::openAnimation(unsigned char* data, uint32_t dataLength, const char* typeExtension)
{
  clear();
  if (data && dataLength > 0)
  {
    // This memory will be deleted by the animation
    m_buffer = (unsigned char*) NMPMemoryAllocAligned(sizeof(unsigned char) * dataLength, NMP_VECTOR_ALIGNMENT);
    NMP_ASSERT(m_buffer);

    memcpy(m_buffer, data, dataLength);

    // Get the registered animation format string
    const Manager::AnimationFormatRegistryEntry* animRegistryEntry =
      Manager::getInstance().findAnimationFormatRegistryEntry(typeExtension);
    if (animRegistryEntry)
    {
      // Locate the animation
      m_animation = reinterpret_cast<MR::AnimSourceBase*> (m_buffer);
      if (!m_animation->isLocated())
        animRegistryEntry->m_locateAnimFormatFn(m_animation);
    }
    else
    {
      NMP_ASSERT_FAIL();
    }

    // Tidy up
    if (m_animation)
    {
      return true;
    }
    else
    {
      clear();
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSourceHandle::setRig(MR::AnimRigDef* rig)
{
  // Clear out previous rig and rig to anim map if present.
  if (m_rig)
  {
    NMP::Memory::memFree(m_rig);
    m_rig = NULL;
  }

  m_rig = rig;

  // Free any existing transform buffer.
  if (m_localTransforms)
  {
    NMP::Memory::memFree(m_localTransforms);
    m_localTransforms = NULL;
  }

  // Allocate a new transform buffer for the new rig.
  m_transformCount = m_rig->getNumBones();
  m_localTransforms = (NMP::PosQuat*)NMPMemoryAlloc(sizeof(NMP::PosQuat) * m_transformCount );
  NMP_ASSERT(m_localTransforms);
}

//----------------------------------------------------------------------------------------------------------------------
const MR::AnimRigDef* AnimationSourceHandle::getRig() const
{
  return m_rig;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSourceHandle::setRigToAnimMap(MR::RigToAnimMap* rigToAnimMap)
{
  NMP_ASSERT(rigToAnimMap);

  if (m_rigToAnimMap)
  {
    NMP::Memory::memFree(m_rigToAnimMap);
    m_rigToAnimMap = NULL;
  }

  m_rigToAnimMap = rigToAnimMap;
}

//----------------------------------------------------------------------------------------------------------------------
const MR::RigToAnimMap* AnimationSourceHandle::getRigToAnimMap() const
{
  return m_rigToAnimMap;
}

//----------------------------------------------------------------------------------------------------------------------
float AnimationSourceHandle::getDuration() const
{
  if (m_animation)
  {
    return m_animation->animGetDuration();
  }
  return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimationSourceHandle::setTime(float time)
{
  // Make sure we don't try to set the animation at a playback position outside of its range.
  m_time = NMP::clampValue(time, 0.0f, m_animation->animGetDuration());

  // Ensure rig and transforms initialized.
  NMP_ASSERT(m_transformCount > 0);
  NMP_ASSERT(m_rigToAnimMap);
  if (m_transformCount > 0 && m_rigToAnimMap)
  {
    // Buffers to store process buffer output.
    NMP::HeapAllocator allocator;
    NMP::Memory::Format buffMemReqs = NMP::DataBuffer::getPosQuatMemoryRequirements(m_transformCount);
    NMP::DataBuffer* transformBuffer = NMP::DataBuffer::createPosQuat(&allocator, buffMemReqs, m_transformCount);

    // propagate anim buffer.
    m_animation->animComputeAtTime(
      m_time,
      m_rig,
      m_rigToAnimMap,
      0, // outputSubsetSize
      0, // outputSubsetArray
      transformBuffer,
      &allocator);

    // Compute remaining transforms.
    NMP::Matrix34   processTransform;
    for (uint32_t i = 0; i < m_transformCount; ++i)
    {
      // Copy in the transforms from the bind pose for any bones that are missing animation tracks
      if (!transformBuffer->hasChannel(i))
      {
        const NMP::Vector3* rigBoneBindPosePos = m_rig->getBindPoseBonePos(i);
        const NMP::Quat* rigBoneBindPoseQuat = m_rig->getBindPoseBoneQuat(i);

        // This channel has not been computed by the current morpheme state, so set it to identity for now.
        transformBuffer->setPosQuatChannelPos(i, *rigBoneBindPosePos);
        transformBuffer->setPosQuatChannelQuat(i, *rigBoneBindPoseQuat);
      }

      m_localTransforms[i].m_pos = *(NMP::Vector3*)transformBuffer->getPosQuatChannelPos(i);
      m_localTransforms[i].m_quat = *(NMP::Quat*)transformBuffer->getPosQuatChannelQuat(i);
    }

    allocator.memFree(transformBuffer);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
float AnimationSourceHandle::getTime() const
{
  return m_time;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimationSourceHandle::getChannelCount() const
{
  return m_transformCount;
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::PosQuat* AnimationSourceHandle::getChannelData() const
{
  return m_localTransforms;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSourceHandle::getTrajectory(NMP::Quat& orientation, NMP::Vector3& translation) const
{
  const MR::TrajectorySourceBase* trajectoryControl = m_animation->animGetTrajectorySourceData();
  if (trajectoryControl)
  {
    trajectoryControl->trajComputeTrajectoryTransformAtTime(m_time, orientation, translation);
  }
  else
  {
    orientation.identity();
    translation.setToZero();
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
