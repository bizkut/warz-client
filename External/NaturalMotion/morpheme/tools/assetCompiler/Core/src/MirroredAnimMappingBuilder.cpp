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
#include "MirroredAnimMappingBuilder.h"

#include "XMD/Model.h"
#include "XMD/AnimCycle.h"

#include "morpheme/mrAttribData.h"
#include <algorithm>
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
void convertBindPoseToWorldSpace(const ME::RigExport* rig, NMP::Quat* rotations, NMP::Vector3* translations)
{
  // Compute the character space transforms
  for (uint32_t i = 0; i < rig->getNumJoints(); ++i)
  {
    // if invalid, ignore...
    uint32_t parentIndex = rig->getJoint(i)->getParentIndex();
    if (parentIndex == 0xFFFFFFFF)
      continue;

    rotations[i] = rotations[parentIndex] * rotations[i];
    translations[i] = translations[parentIndex] + rotations[parentIndex].rotateVector(translations[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void convertBindPoseToLocalSpace(const ME::RigExport* rig, NMP::Quat* rotations, NMP::Vector3* translations)
{
  // start with kids and convert them to local space before the parents.
  for (uint32_t i = rig->getNumJoints(); i > 0; --i)
  {
    // the joint index
    uint32_t jointIdx = i - 1;

    // get the world space position / quaternion
    NMP::Matrix34 worldTM(rotations[jointIdx], translations[jointIdx]);

    // If child then LocalTransform = WorldTranform * ParentInverseWorldTranform
    uint32_t parentIdx = rig->getJoint(jointIdx)->getParentIndex();
    if (parentIdx != 0xFFFFFFFF)
    {
      NMP::Matrix34 parentTMInv(rotations[parentIdx], translations[parentIdx]);
      parentTMInv.invert();
      worldTM *= parentTMInv;
    }

    // store local space values for joint
    rotations[jointIdx] = worldTM.toQuat();
    translations[jointIdx] = worldTM.translation();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void mirrorTransforms(
  const uint32_t      mirrorAxis,
  const uint32_t      numJoints,
  const NMP::Quat*    rotations,
  const NMP::Vector3* translations,
  NMP::Quat*          outRotations,
  NMP::Vector3*       outTranslations)
{
  uint32_t rx = (mirrorAxis + 1) % 3;
  uint32_t ry = (mirrorAxis + 2) % 3;

  // Compute the character space transforms
  for (uint32_t i = 0; i < numJoints; ++i)
  {
    // grab pos quat
    outRotations[i] = rotations[i];
    outTranslations[i] = translations[i];

    // mirror
    (&outRotations[i].x)[rx] = -(&outRotations[i].x)[rx];
    (&outRotations[i].x)[ry] = -(&outRotations[i].x)[ry];
    outTranslations[i].f[mirrorAxis] = -outTranslations[i].f[mirrorAxis];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void modifyQuatsByBlendFrame(const uint32_t numQuats, NMP::Quat* quats, const NMP::Quat& frameOrient)
{
  for (uint32_t i = 0; i != numQuats; ++i)
  {
    NMP::Quat q;
    q.multiply(frameOrient, quats[i]);
    quats[i] = q;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void MirroredAnimMappingBuilder::init(
  MR::AttribDataMirroredAnimMapping* animMapping,
  const ME::RigExport*               rigExport,
  const ME::AnimationSetExport*      animSetExport)
{
  // Init mirror plane.
  animMapping->m_axis = rigExport->getMirrorPlane();
  std::map<uint32_t, uint32_t> leftToRight;
  std::vector<uint32_t> unmapped;

  // start by filling a set of ID's up in the vector
  for (uint32_t i = 0; i != rigExport->getNumJoints(); ++i)
  {
    unmapped.push_back(i);
  }

  // build the map of left to right bone indices to be swapped when mirroring.
  unsigned int count = rigExport->getMirrorMappingCount();
  for (unsigned int i = 0; i != count; ++i)
  {
    unsigned int first, second;
    rigExport->getMirrorMapping(i, first, second);
    leftToRight.insert(std::make_pair(static_cast<uint32_t>(first), static_cast<uint32_t>(second)));

    // remove the 2 bones from the unmapped array
    unmapped.erase(std::find(unmapped.begin(), unmapped.end(), first));
    if (first != second)
    {
      unmapped.erase(std::find(unmapped.begin(), unmapped.end(), second));
    }
  }

  // Now sort out the rotation offsets between bind pose and the mirrored bind pose.
  {
    // Allocate buffers to store the world space joint transforms.
    NMP::Quat* rotations = (NMP::Quat*)NMPMemoryAllocAligned(sizeof(NMP::Quat) * rigExport->getNumJoints(), NMP_VECTOR_ALIGNMENT);
    NMP_ASSERT(rotations);
    NMP::Vector3* positions = (NMP::Vector3*)NMPMemoryAllocAligned(sizeof(NMP::Vector3) * rigExport->getNumJoints(), NMP_VECTOR_ALIGNMENT);
    NMP_ASSERT(positions);

    // Grab bind pose from rig.
    for (uint32_t i = 0; i != rigExport->getNumJoints(); ++i)
    {
      rigExport->getJoint(i)->getRotation(rotations[i].x, rotations[i].y, rotations[i].z, rotations[i].w);
      rigExport->getJoint(i)->getTranslation(positions[i].x, positions[i].y, positions[i].z);
    }

    // Convert to world space.
    convertBindPoseToWorldSpace(rigExport, rotations, positions);

    // Allocate buffers to store the mirrored world space joint transforms.
    NMP::Quat* outRotations = (NMP::Quat*)NMPMemoryAllocAligned(sizeof(NMP::Quat) * rigExport->getNumJoints(), NMP_VECTOR_ALIGNMENT);
    NMP_ASSERT(outRotations);
    NMP::Vector3* outPositions = (NMP::Vector3*)NMPMemoryAllocAligned(sizeof(NMP::Vector3) * rigExport->getNumJoints(), NMP_VECTOR_ALIGNMENT);
    NMP_ASSERT(outPositions);

    // Mirror in the desired axis.
    mirrorTransforms(rigExport->getMirrorPlane(), rigExport->getNumJoints(), rotations, positions, outRotations, outPositions);

    // Can throw away the translation results.
    NMP::Memory::memFree(positions);
    NMP::Memory::memFree(outPositions);

    for (uint32_t boneIndex = 0; boneIndex != rigExport->getNumJoints(); ++boneIndex)
    {
      rotations[boneIndex].conjugate();
    }

    // initialise the mapping data
    {
      animMapping->m_numBones = rigExport->getNumJoints();
      std::map<uint32_t, uint32_t>::iterator it = leftToRight.begin();
      uint32_t i = 0;
      for (; it != leftToRight.end(); ++it, ++i)
      {
        // To get the rotational offset.
        animMapping->m_boneMappings[i].leftIndex = it->first;
        animMapping->m_boneMappings[i].rightIndex = it->second;
        animMapping->m_quatOffsets[it->first] = rotations[it->second] * outRotations[it->first];
        animMapping->m_quatOffsets[it->second] = rotations[it->first] * outRotations[it->second];
      }
    }

    // now determine the offsets between all of the non-swapped joints.
    uint32_t nJoints = (uint32_t)unmapped.size();
    for (uint32_t i = 0; i != nJoints; ++i)
    {
      // store orient offset for the unmapped quat
      animMapping->m_quatOffsets[unmapped[i]] = NMP::Quat(NMP::Quat::kIdentity);
    }

    // Finally clean up after ourselves.
    NMP::Memory::memFree(rotations);
    NMP::Memory::memFree(outRotations);
  }

  // Now sort out the track id's.
  {
    animMapping->m_numTrackIds = animSetExport->getNumEventTrackMirrorMappings(); ;
    for (uint32_t i = 0; i != animMapping->m_numTrackIds; ++i)
    {
      int32_t firstTrack, secondTrack;
      animSetExport->getEventTrackMirrorMapping(i, firstTrack, secondTrack);
      animMapping->m_trackIds[i].leftIndex = static_cast<uint32_t>(firstTrack);
      animMapping->m_trackIds[i].rightIndex = static_cast<uint32_t>(secondTrack);
    }
  }

  // Now sort out the event id's.
  {
    animMapping->m_numEventIds = animSetExport->getNumEventUserDataMirrorMappings();
    for (uint32_t i = 0; i != animMapping->m_numEventIds; ++i)
    {
      int firstTrack, secondTrack;
      animSetExport->getEventUserDataMirrorMapping(i, firstTrack, secondTrack);
      animMapping->m_eventIds[i].leftIndex = static_cast<uint32_t>(firstTrack);
      animMapping->m_eventIds[i].rightIndex = static_cast<uint32_t>(secondTrack);
    }
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
