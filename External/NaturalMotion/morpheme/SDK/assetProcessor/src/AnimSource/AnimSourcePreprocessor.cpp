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
#define _WCTYPE_INLINE_DEFINED
#include <string.h>
#include <sstream>
#include <vector>
#include <algorithm>
#ifdef WIN32
  #include <crtdbg.h>
#endif
#include "XMD/Model.h"
#include "XMD/AnimCycle.h"
#include "XMD/PoseStream.h"
#include "XMD/FCurveStream.h"
#include "XMD/SampledStream.h"

#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrDefines.h"
#include "assetProcessor/AnimSource/AnimSourcePreprocessor.h"
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

namespace AP
{

using namespace XM2;
using namespace XMD;

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetTransformTableBuilderXMD
//----------------------------------------------------------------------------------------------------------------------
uint32_t ChannelSetTableBuilderXMD::calculateMaxNumKeyFrames(
  const XMD::XModel&           xmdAnims,
  const XMD::XAnimCycle&       xmdCycle,
  const std::vector<XMD::XId>& animBoneIDs)
{
  uint32_t numKeyFrames = 0;
  for (uint32_t i = 0; i < animBoneIDs.size(); ++i)
  {
    // Check for a valid animation channel (could be an inserted missing channel).
    if (animBoneIDs[i] == 0xFFFFFFFF)
      continue;
    
    const XMD::XBase* xmdBase = xmdAnims.FindNode(animBoneIDs[i]);
    NMP_VERIFY(xmdBase);

    XMD::XId xmdBoneID = xmdBase->GetID();
    const XMD::XSampledKeys* xmdSampledKeys = xmdCycle.GetBoneKeys(xmdBoneID);
    NMP_VERIFY(xmdSampledKeys);

    // Find max of Rot/Trans keyframes.
    uint32_t numRotKeyFramesThisNode = (uint32_t) xmdSampledKeys->RotationKeys().size();
    uint32_t numTransKeyFramesThisNode = (uint32_t) xmdSampledKeys->TranslationKeys().size();
    uint32_t numKeyFramesThisNode = NMP::maximum(numRotKeyFramesThisNode, numTransKeyFramesThisNode);

    // Get the max over all nodes
    if (numKeyFramesThisNode > numKeyFrames)
    {
      numKeyFrames = numKeyFramesThisNode;
    }
  }

  return numKeyFrames;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t ChannelSetTableBuilderXMD::calculateMaxNumKeyFrames(
  const XMD::XModel&           xmdAnims,
  const XMD::XAnimationTake&   xmdCycle,
  const std::vector<XMD::XId>& animBoneIDs)
{
  uint32_t numKeyFrames = 0;
  for (uint32_t i = 0; i < animBoneIDs.size(); ++i)
  {
    // Check for a valid animation channel (could be an inserted missing channel).
    if (animBoneIDs[i] == 0xFFFFFFFF)
      continue;

    const XMD::XBase* xmdBase = xmdAnims.FindNode(animBoneIDs[i]);
    NMP_VERIFY(xmdBase);

    XMD::XId xmdBoneID = xmdBase->GetID();
    const XMD::XAnimatedNode* xmdSampledKeys = xmdCycle.GetNodeAnimationById(xmdBoneID);
    NMP_VERIFY(xmdSampledKeys);

    // Find max of Rot/Trans keyframes.
    uint32_t numKeyFramesThisNode = 1;
    if (xmdSampledKeys)
    {
      uint32_t numRotKeyFramesThisNode = 1;
      const XMD::XAnimatedAttribute* rotation_attr = xmdSampledKeys->GetAttributeById(XMD::XBone::kRotation);
      if (rotation_attr && rotation_attr->GetSampledStream())
      {
        numRotKeyFramesThisNode = rotation_attr->GetSampledStream()->GetNumElemtents();
      }

      uint32_t numTransKeyFramesThisNode = 1;
      const XMD::XAnimatedAttribute* translate_attr = xmdSampledKeys->GetAttributeById(XMD::XBone::kTranslation);
      if (translate_attr && translate_attr->GetSampledStream())
      {
        numTransKeyFramesThisNode = translate_attr->GetSampledStream()->GetNumElemtents();
      }

      numKeyFramesThisNode = NMP::maximum(numRotKeyFramesThisNode, numTransKeyFramesThisNode);
    }

    // Get the max over all nodes
    if (numKeyFramesThisNode > numKeyFrames)
    {
      numKeyFrames = numKeyFramesThisNode;
    }
  }

  return numKeyFrames;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTableBuilderXMD::initChannelQuat(
  uint32_t                 channelNumFrames,
  NMP::Quat*               channelQuat,
  const XMD::XBone*        bone,
  const XMD::XSampledKeys* keys)
{
  NMP_VERIFY(channelNumFrames > 0);
  NMP_VERIFY(channelQuat);
  NMP_VERIFY(bone);
  NMP_VERIFY(keys);

  uint32_t numKeyframes = (uint32_t)keys->RotationKeys().size();
  NMP_VERIFY(numKeyframes > 0);
  NMP_VERIFY(channelNumFrames >= numKeyframes);

  XQuaternion pre = bone->GetJointOrient();
  XQuaternion post = bone->GetRotationOrient();
  XQuaternionArray new_rots;
  new_rots.resize(numKeyframes);
  XQuaternionArray::const_iterator ita = keys->RotationKeys().begin();
  XQuaternionArray::iterator it = new_rots.begin();
  XQuaternionArray::const_iterator end = keys->RotationKeys().end();
  for (; ita != end; ++ita, ++it)
  {
    it->mul(post, (*ita));
    *it *= pre;
  }

  // Ensure that the first quaternion keyframe has positive w component. This
  // is a useful fact when uncompressing an unvarying quat channel with zero
  // stored samples
  if (new_rots[0].w < 0.0f)
  {
    new_rots[0].x = -new_rots[0].x;
    new_rots[0].y = -new_rots[0].y;
    new_rots[0].z = -new_rots[0].z;
    new_rots[0].w = -new_rots[0].w;
  }

  // Ensure that consecutive quaternions are in the same hemisphere
  for (uint32_t i = 1; i < numKeyframes; i++)
  {
    float dot = new_rots[i-1].x * new_rots[i].x +
                new_rots[i-1].y * new_rots[i].y +
                new_rots[i-1].z * new_rots[i].z +
                new_rots[i-1].w * new_rots[i].w;
    if (dot < 0.0f)
    {
      new_rots[i].x = -new_rots[i].x;
      new_rots[i].y = -new_rots[i].y;
      new_rots[i].z = -new_rots[i].z;
      new_rots[i].w = -new_rots[i].w;
    }
  }

  // Set the quaternion data from the channel requirements
  NMP::Quat q;
  uint32_t frameIndx;
  for (frameIndx = 0; frameIndx < numKeyframes; ++frameIndx)
  {
    q.x = new_rots[frameIndx].x;
    q.y = new_rots[frameIndx].y;
    q.z = new_rots[frameIndx].z;
    q.w = new_rots[frameIndx].w;
    channelQuat[frameIndx] = q;
  }

  // Write any remaining frames
  for (; frameIndx < channelNumFrames; ++frameIndx)
  {
    channelQuat[frameIndx] = q;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTableBuilderXMD::initChannelQuat(
  uint32_t                       channelNumFrames,
  NMP::Quat*                     channelQuat,
  const acAnimInfo&              NMP_UNUSED(animInfo),
  const XMD::XBone*              bone,
  const XMD::XAnimatedAttribute* keys)
{
  NMP_VERIFY(channelNumFrames > 0);
  NMP_VERIFY(channelQuat);
  NMP_VERIFY(keys);
  NMP_VERIFY(bone);

  const XQuaternion& xJointOrient = bone->GetJointOrient();
  const XQuaternion& xRotationOrient = bone->GetRotationOrient();
  NMP::Quat pre(xJointOrient.x, xJointOrient.y, xJointOrient.z, xJointOrient.w);
  NMP::Quat post(xRotationOrient.x, xRotationOrient.y, xRotationOrient.z, xRotationOrient.w);

  // ensure keys are valid (can be null) and they have key data streams
  if (keys && keys->GetNumStreams())
  {
    // check for a single key
    if (keys->GetPoseStream())
    {
      // Set the quaternion data from the channel requirements
      XQuaternion xq = keys->GetPoseStream()->AsQuaternion();
      NMP::Quat q(xq.x, xq.y, xq.z, xq.w);

      // apply pre and post rotation
      q = pre * q * post;

      // Ensure that the first quaternion keyframe has positive w component. This
      // is a useful fact when uncompressing an unvarying quat channel with zero
      // stored samples
      if (q.w < 0.0f)
      {
        q = -q;
      }

      // Write the frames
      for (uint32_t i = 0; i < channelNumFrames; ++i)
      {
        channelQuat[i] = q;
      }
    }
    else if (keys->GetSampledStream())   // always prefer the sampled keys to FCurves
    {
      const XMD::XSampledStream* stream = keys->GetSampledStream();
      uint32_t actualNumFrames = stream->GetNumElemtents();
      NMP_VERIFY(actualNumFrames <= channelNumFrames);

      XQuaternion q0 = stream->AsQuaternion(0);
      NMP::Quat q(q0.x, q0.y, q0.z, q0.w);

      // apply pre and post rotation
      q = pre * q * post;

      // Ensure that the first quaternion keyframe has positive w component. This
      // is a useful fact when uncompressing an unvarying quat channel with zero
      // stored samples
      if (q.w < 0.0f)
      {
        q = -q;
      }

      // Write the frames
      channelQuat[0] = q;
      for (uint32_t i = 1; i < actualNumFrames; ++i)
      {
        XQuaternion xQuat = stream->AsQuaternion(i);
        q.setXYZW(xQuat.x, xQuat.y, xQuat.z, xQuat.w);

        // apply pre and post rotation
        q = pre * q * post;

        // Ensure that consecutive quaternions are in the same hemisphere
        if (q.dot(channelQuat[i-1]) < 0.0f)
        {
          q = -q;
        }

        // Set the channel quat
        channelQuat[i] = q;
      }

      // Write any remaining frames
      for (uint32_t i = actualNumFrames; i < channelNumFrames; ++i)
      {
        channelQuat[i] = q;
      }
    }
    else if (keys->GetFCurveStream())
    {
      NMP_THROW_ERROR("Error processing f-curve data.  Only sampled data is supported");
    }
  }
  else
  {
    // no key frame data for this rotation. use default value.
    XQuaternion xQuat = bone->GetRotation(true);
    NMP::Quat q(xQuat.x, xQuat.y, xQuat.z, xQuat.w);

    // apply pre and post rotation
    q = pre * q * post;

    // Ensure that consecutive quaternions are in the same hemisphere
    if (q.w < 0.0f)
    {
      q = -q;
    }

    // Write the frames
    for (uint32_t i = 0; i < channelNumFrames; ++i)
    {
      channelQuat[i] = q;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTableBuilderXMD::initChannelPos(
  uint32_t                 channelNumFrames,
  NMP::Vector3*            channelPos,
  const XMD::XBone*        NMP_UNUSED(bone),
  const XMD::XSampledKeys* keys)
{
  NMP_VERIFY(channelNumFrames > 0);
  NMP_VERIFY(channelPos);
  NMP_VERIFY(keys);

  const XVector3Array& positions = keys->TranslationKeys();
  uint32_t numKeyframes = (uint32_t)positions.size();
  NMP_VERIFY(numKeyframes > 0);

  // Write the sampled frames
  NMP::Vector3 p(NMP::Vector3::InitZero);
  for (uint32_t i = 0; i < numKeyframes; ++i)
  {
    p.set(positions[i].x, positions[i].y, positions[i].z);
    channelPos[i] = p;
  }

  // Write any remaining frames
  for (uint32_t i = numKeyframes; i < channelNumFrames; ++i)
  {
    channelPos[i] = p;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTableBuilderXMD::initChannelPos(
  uint32_t                       channelNumFrames,
  NMP::Vector3*                  channelPos,
  const acAnimInfo&              animInfo,
  const XMD::XBone*              bone,
  const XMD::XAnimatedAttribute* keys)
{
  NMP_VERIFY(channelNumFrames > 0);
  NMP_VERIFY(channelPos);
  NMP_VERIFY(bone);

  // ensure keys are valid (can be null) and they have key data streams
  if (keys && keys->GetNumStreams())
  {
    // check for a single key
    if (keys->GetPoseStream())
    {
      const XMD::XPoseStream* pstream = keys->GetPoseStream();
      if (pstream)
      {
        XVector3 xp = pstream->AsVector3();
        NMP::Vector3 p(xp.x, xp.y, xp.z, 0.0f);

        // Write the frames
        for (uint32_t i = 0; i < channelNumFrames; ++i)
        {
          channelPos[i] = p;
        }
      }
    }
    else if (keys->GetSampledStream())   // always prefer the sampled keys to FCurves
    {

      const XMD::XSampledStream* stream = keys->GetSampledStream();
      uint32_t actualNumFrames = stream->GetNumElemtents();
      NMP_VERIFY(actualNumFrames <= channelNumFrames);

      // Write the frames
      NMP::Vector3 p(NMP::Vector3::InitZero);
      for (uint32_t i = 0; i < actualNumFrames; ++i)
      {
        XVector3 xp = stream->AsVector3(i);
        p.set(xp.x, xp.y, xp.z);
        channelPos[i] = p;
      }

      // Write any remaining frames
      for (uint32_t i = actualNumFrames; i < channelNumFrames; ++i)
      {
        channelPos[i] = p;
      }
    }
    else if (keys->GetFCurveStream())    // if f-curves are the only data available, we'll use them
    {                                    // (samples are generally safer since transforms can be controlled by expressions,
                                         //  ik or other constraints - which won't be transferred into reliable FCurve data)
      const XMD::XFCurveStream* fcstream = keys->GetFCurveStream();
      if (fcstream)
      {
        const XMD::XFCurve* xcurve = fcstream->GetCurve(0);
        const XMD::XFCurve* ycurve = fcstream->GetCurve(1);
        const XMD::XFCurve* zcurve = fcstream->GetCurve(2);

        XReal start = animInfo.m_StartTime;
        XReal end   = animInfo.m_EndTime;
        XReal incr  = (end - start) / (animInfo.m_NumFrames - 1);
        XReal x = bone->GetTranslation(true).x;
        XReal y = bone->GetTranslation(true).y;
        XReal z = bone->GetTranslation(true).z;

        for (uint32_t i = 0; i < animInfo.m_NumFrames; ++i)
        {
          XReal time = start + incr * i;
          if (xcurve) xcurve->Evaluate(time, x);
          if (xcurve) ycurve->Evaluate(time, y);
          if (xcurve) zcurve->Evaluate(time, z);

          channelPos[i].set(x, y, z);
        }

        // Write any remaining frames
        NMP::Vector3 p = channelPos[animInfo.m_NumFrames-1];
        for (uint32_t i = animInfo.m_NumFrames; i < channelNumFrames; ++i)
        {
          channelPos[i] = p;
        }
      }
    }
  }
  // otherwise just use the default value from the XBone
  else
  {
    const XVector3& p = bone->GetTranslation();
    for (uint32_t i = 0; i < channelNumFrames; ++i)
    {
      channelPos[i].set(p.x, p.y, p.z);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
ChannelSetTable* ChannelSetTableBuilderXMD::init(
  NMP::Memory::Resource        memDesc,
  const XMD::XModel&           xmdAnims,
  const acAnimInfo&            animInfo,
  const std::vector<XMD::XId>& animBoneIDs,
  float                        scale,
  const MR::AnimRigDef*        rig,
  const MR::RigToAnimEntryMap* rigToAnimEntryMap)
{
  NMP_VERIFY(scale > 0.0f);
  NMP_VERIFY(rig);
  NMP_VERIFY(rigToAnimEntryMap);
  ChannelSetTable* result = NULL;

  if (animInfo.m_TakeIndex == (uint32_t)acAnimInfo::kBindPoseTakeIndex)
  {
    uint32_t numChannelSets = (uint32_t)animBoneIDs.size();
    uint32_t channelNumKeyFrames = 2;

    // Initialise the channel set table
    result = AP::ChannelSetTable::init(memDesc, numChannelSets, channelNumKeyFrames);
    NMP_VERIFY(result);

    for (uint32_t i = 0; i < numChannelSets; ++i)
    {
      uint16_t animChannelIndex = (uint16_t)i;
      uint16_t rigChannelIndex;
      if (rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex))
      {
        // Set the quat channel from the bind pose
        NMP::Quat bindPoseQuat = *(rig->getBindPoseBoneQuat(rigChannelIndex));
        result->setChannelQuatKeys(i, bindPoseQuat);

        // Set the pos channel from the bind pose. Note that the bind pose position
        // data has already been scaled correctly.
        NMP::Vector3 bindPosePos = *(rig->getBindPoseBonePos(rigChannelIndex));
        result->setChannelPosKeys(i, bindPosePos);
      }
      else
      {
        NMP_ASSERT_FAIL();
      }
    }
  }
  else if (animInfo.m_TakeIndex < xmdAnims.NumAnimCycles())
  {
    // Find the take that we want to process within the XMD data
    const XMD::XAnimCycle* xmdCycle = xmdAnims.GetAnimCycle(animInfo.m_TakeIndex);
    NMP_VERIFY(xmdCycle);

    // Number of channel sets
    uint32_t numChannelSets = (uint32_t)animBoneIDs.size();

    // Maximum number of frames
    uint32_t actualNumKeyframes = ChannelSetTableBuilderXMD::calculateMaxNumKeyFrames(xmdAnims, *xmdCycle, animBoneIDs);
    // Runtime animations require at least two frames
    uint32_t channelNumKeyFrames = actualNumKeyframes;
    if (actualNumKeyframes < 2)
    {
      channelNumKeyFrames = NMP::maximum(animInfo.m_NumFrames, static_cast<uint32_t>(2));
    }

    // Initialise the channel set table
    result = AP::ChannelSetTable::init(memDesc, numChannelSets, channelNumKeyFrames);
    NMP_VERIFY(result);

    //---------------------
    // Compute the channel set data
    for (uint32_t i = 0; i < numChannelSets; ++i)
    {
      // Check for a valid animation channel (could be an inserted missing channel).
      if (animBoneIDs[i] != 0xFFFFFFFF)
      {
        // Get the XMD Bone and sampled keys
        const XMD::XBase* xmdBase = xmdAnims.FindNode(animBoneIDs[i]);
        NMP_VERIFY(xmdBase);
        XMD::XId xmdBoneID = xmdBase->GetID();
        const XMD::XSampledKeys* xmdSampledKeys = xmdCycle->GetBoneKeys(xmdBoneID);
        const XMD::XBone* xmdBone = xmdBase->HasFn<XMD::XBone>();

        // Quat channel
        ChannelSetTableBuilderXMD::initChannelQuat(
          channelNumKeyFrames,
          result->getChannelQuat(i),
          xmdBone,
          xmdSampledKeys);

        // Pos channel
        ChannelSetTableBuilderXMD::initChannelPos(
          channelNumKeyFrames,
          result->getChannelPos(i),
          xmdBone,
          xmdSampledKeys);
          
        // Scale the position channel
        result->scaleChannelPos(i, scale);
      }
      else
      {
        uint16_t animChannelIndex = (uint16_t)i;
        uint16_t rigChannelIndex;
        if (rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex))
        {
          // Set the quat channel from the bind pose
          NMP::Quat bindPoseQuat = *(rig->getBindPoseBoneQuat(rigChannelIndex));
          result->setChannelQuatKeys(i, bindPoseQuat);

          // Set the pos channel from the bind pose. Note that the bind pose position
          // data has already been scaled correctly.
          NMP::Vector3 bindPosePos = *(rig->getBindPoseBonePos(rigChannelIndex));
          result->setChannelPosKeys(i, bindPosePos);
        }
        else
        {
          NMP_ASSERT_FAIL();
        }
      }
    }
  }
  else
  {
    // Move index past the old take types
    XMD::XU32 idx = animInfo.m_TakeIndex - xmdAnims.NumAnimCycles();
    XMD::XList takes;
    xmdAnims.List(takes, XMD::XFn::AnimationTake);
    NMP_VERIFY(idx < takes.size());

    XMD::XAnimationTake* xmdCycle = takes[idx]->HasFn<XMD::XAnimationTake>();
    NMP_VERIFY(xmdCycle);

    // Number of channel sets
    uint32_t numChannelSets = (uint32_t)animBoneIDs.size();

    // Maximum number of frames
    uint32_t actualNumKeyframes = ChannelSetTableBuilderXMD::calculateMaxNumKeyFrames(xmdAnims, *xmdCycle, animBoneIDs);
    // Runtime animations require at least two frames
    uint32_t channelNumKeyFrames = actualNumKeyframes;
    if (actualNumKeyframes < 2)
    {
      channelNumKeyFrames = NMP::maximum(animInfo.m_NumFrames, static_cast<uint32_t>(2));
    }

    // Initialise the channel set table
    result = AP::ChannelSetTable::init(memDesc, numChannelSets, channelNumKeyFrames);
    NMP_VERIFY(result);

    //---------------------
    // Compute the channel set data
    for (uint32_t i = 0; i < numChannelSets; ++i)
    {
      // Check for a valid animation channel (could be an inserted missing channel).
      if (animBoneIDs[i] != 0xFFFFFFFF)
      {
    
        // Get the XMD Bone and sampled keys
        const XMD::XBase* xmdBase = xmdAnims.FindNode(animBoneIDs[i]);
        NMP_VERIFY(xmdBase);
        const XMD::XAnimatedNode* xmdSampledKeys = xmdCycle->GetNodeAnimationById(animBoneIDs[i]);
        const XMD::XBone* xmdBone = xmdBase->HasFn<XMD::XBone>();
        NMP_VERIFY(xmdBone);

        // Quat channel
        ChannelSetTableBuilderXMD::initChannelQuat(
          channelNumKeyFrames,
          result->getChannelQuat(i),
          animInfo,
          xmdBone,
          xmdSampledKeys->GetAttributeById(XMD::XBone::kRotation));

        // Pos channel
        ChannelSetTableBuilderXMD::initChannelPos(
          channelNumKeyFrames,
          result->getChannelPos(i),
          animInfo,
          xmdBone,
          xmdSampledKeys->GetAttributeById(XMD::XBone::kTranslation));
          
        // Scale the position channel
        result->scaleChannelPos(i, scale);
      }
      else
      {
        uint16_t animChannelIndex = (uint16_t)i;
        uint16_t rigChannelIndex;
        if (rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex))
        {
          // Set the quat channel from the bind pose
          NMP::Quat bindPoseQuat = *(rig->getBindPoseBoneQuat(rigChannelIndex));
          result->setChannelQuatKeys(i, bindPoseQuat);

          // Set the pos channel from the bind pose. Note that the bind pose position
          // data has already been scaled correctly.
          NMP::Vector3 bindPosePos = *(rig->getBindPoseBonePos(rigChannelIndex));
          result->setChannelPosKeys(i, bindPosePos);
        }
        else
        {
          NMP_ASSERT_FAIL();
        }
      }
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// StringTableBuilderXMD Functions
//----------------------------------------------------------------------------------------------------------------------
uint32_t StringTableBuilderXMD::getDataLength(
  const MR::AnimRigDef*        rig,
  const MR::RigToAnimEntryMap* rigToAnimEntryMap)
{
  uint32_t numAnimChannelSets = rigToAnimEntryMap->getNumEntries();

  size_t namesTableSize = 0;
  for (uint32_t i = 0; i < numAnimChannelSets; ++i)
  {
    uint16_t animChannelIndex = (uint16_t)i;
    uint16_t rigChannelIndex;
    if (rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex))
    {
      const char* boneName = rig->getBoneName(rigChannelIndex);
      NMP_VERIFY(boneName);
      namesTableSize += (strlen(boneName) + 1);
    }
    else
    {
      NMP_ASSERT_FAIL();
    }
  }

  return (uint32_t)namesTableSize;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format StringTableBuilderXMD::getMemoryRequirements(
  const MR::AnimRigDef*        rig,
  const MR::RigToAnimEntryMap* rigToAnimEntryMap)
{
  uint32_t dataLength = StringTableBuilderXMD::getDataLength(rig, rigToAnimEntryMap);
  uint32_t numAnimChannelSets = rigToAnimEntryMap->getNumEntries();
  return NMP::OrderedStringTable::getMemoryRequirements(numAnimChannelSets, dataLength);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::OrderedStringTable* StringTableBuilderXMD::init(
  NMP::Memory::Resource        resource,
  const MR::AnimRigDef*        rig,
  const MR::RigToAnimEntryMap* rigToAnimEntryMap)
{
  uint32_t numAnimChannelSets = rigToAnimEntryMap->getNumEntries();
  uint32_t namesTableSize = StringTableBuilderXMD::getDataLength(rig, rigToAnimEntryMap);

  // Create temporary memory for the data arrays
  uint32_t* offsets = (uint32_t*)NMPMemoryAlloc(sizeof(uint32_t) * numAnimChannelSets);
  NMP_ASSERT(offsets);
  char* tableData = (char*)NMPMemoryAlloc(namesTableSize);
  NMP_ASSERT(tableData);

  // Generate the data arrays
  size_t offset = 0;
  for (uint32_t i = 0; i < numAnimChannelSets; ++i)
  {
    uint16_t animChannelIndex = (uint16_t)i;
    uint16_t rigChannelIndex;
    rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
    const char* currentBoneName = rig->getBoneName(rigChannelIndex);
    NMP_VERIFY(currentBoneName);
    offsets[i] = (uint32_t) offset;
    strcpy(&tableData[offset], currentBoneName);
    offset += (strlen(currentBoneName) + 1);
  }

  // Initialise the string table
  NMP::OrderedStringTable* namesTable = NMP::OrderedStringTable::init(
                                   resource,
                                   numAnimChannelSets,
                                   offsets,
                                   tableData,
                                   namesTableSize);
  // Tidy up
  NMP::Memory::memFree(offsets);
  NMP::Memory::memFree(tableData);

  return namesTable;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceUncompressedBuilderXMD Functions
//----------------------------------------------------------------------------------------------------------------------
AnimSourceUncompressed* AnimSourceUncompressedBuilderXMD::init(
  const XMD::XModel&           xmdAnims,
  const acAnimInfo&            animInfo,
  const std::vector<XMD::XId>& animBoneIDs,
  const MR::AnimRigDef*        rig,
  const MR::RigToAnimEntryMap* rigToAnimEntryMap,
  float                        scale,
  const AnimSourceUncompressedOptions& animSourceOptions)
{
  NMP::Memory::Format memReqs;
  NMP::Memory::Resource memRes;

  //---------------------
  // Information
  uint32_t numAnimChannelSets = (uint32_t)animBoneIDs.size();
  float animSampleRate = 0;
  uint32_t actualNumAnimKeyframes = 0;

  // Find the take that we want to process within the XMD data
  if (animInfo.m_TakeIndex == (uint32_t)acAnimInfo::kBindPoseTakeIndex)
  {
    animSampleRate = (float)acAnimInfo::kBindPoseFPS;
    actualNumAnimKeyframes = 2; // Runtime animations require at least two frames
  }
  else if (animInfo.m_TakeIndex < xmdAnims.NumAnimCycles())
  {
    const XMD::XAnimCycle* xmdCycle = xmdAnims.GetAnimCycle(animInfo.m_TakeIndex);
    animSampleRate = (float)xmdCycle->GetFramesPerSecond();

    actualNumAnimKeyframes = ChannelSetTableBuilderXMD::calculateMaxNumKeyFrames(
                               xmdAnims,
                               *xmdCycle,
                               animBoneIDs);
  }
  else
  {
    XMD::XList takes;
    xmdAnims.List(takes, XMD::XFn::AnimationTake);
    uint32_t takeId = animInfo.m_TakeIndex - xmdAnims.NumAnimCycles();
    NMP_VERIFY(takeId < takes.size());

    const XMD::XAnimationTake* xmdTake = takes[takeId]->HasFn<XMD::XAnimationTake>();
    animSampleRate = (float)xmdTake->GetFramesPerSecond();

    actualNumAnimKeyframes = ChannelSetTableBuilderXMD::calculateMaxNumKeyFrames(
                               xmdAnims,
                               *xmdTake,
                               animBoneIDs);
  }

  // Runtime animations require at least two frames
  uint32_t maxNumAnimKeyframes = actualNumAnimKeyframes;
  if (actualNumAnimKeyframes < 2)
  {
    maxNumAnimKeyframes = NMP::maximum(animInfo.m_NumFrames, static_cast<uint32_t>(2));
  }

  //---------------------
  // Build the animation channel sets
  memReqs = AP::ChannelSetTable::getMemoryRequirements(numAnimChannelSets, maxNumAnimKeyframes);

  memRes = NMPMemoryAllocateFromFormat(memReqs);

  ChannelSetTable* animChannelSets = ChannelSetTableBuilderXMD::init(
                                       memRes,
                                       xmdAnims,
                                       animInfo,
                                       animBoneIDs,
                                       scale,
                                       rig,
                                       rigToAnimEntryMap);

  //---------------------
  // Build the channel set names table
  memReqs = StringTableBuilderXMD::getMemoryRequirements(rig, rigToAnimEntryMap);

  memRes = NMPMemoryAllocateFromFormat(memReqs);

  NMP::OrderedStringTable* channelNames = StringTableBuilderXMD::init(
                                     memRes,
                                     rig,
                                     rigToAnimEntryMap);

  //---------------------
  // Build the uncompressed animation source
  memReqs = AnimSourceUncompressedBuilder::getMemoryRequirements(
              rig,
              rigToAnimEntryMap,
              animChannelSets,
              channelNames);

  memRes = NMPMemoryAllocateFromFormat(memReqs);

  AnimSourceUncompressed* anim = AnimSourceUncompressedBuilder::init(
                                   memRes,
                                   rig,
                                   rigToAnimEntryMap,
                                   animChannelSets,
                                   channelNames,
                                   animSampleRate,
                                   animSourceOptions);

  //---------------------
  // Tidy up the temporary workspace memory
  NMP::Memory::memFree(animChannelSets);
  NMP::Memory::memFree(channelNames);

  return anim;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
