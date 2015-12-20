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
#ifndef MR_ANIMATION_SOURCE_HANDLE_H
#define MR_ANIMATION_SOURCE_HANDLE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix34.h"
#include "morpheme/mrRig.h"
#include "morpheme/AnimSource/mrAnimSource.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AnimationSourceHandle
/// \brief Utility class for the MorphemeConnect animation browser to be able to preview animations without having to
/// create a full Network instance in the runtime target.
/// \ingroup CoreModule
//
// MORPH-21336: Move this class to the RuntimeTarget as it is only necessary as a connect utility.
//----------------------------------------------------------------------------------------------------------------------
class AnimationSourceHandle
{
public:
  AnimationSourceHandle();
  ~AnimationSourceHandle();

  bool openAnimation(unsigned char* data, uint32_t dataLength, const char* typeExtension);
  void setRig(MR::AnimRigDef* rig);
  const MR::AnimRigDef* getRig() const;
  void setRigToAnimMap(MR::RigToAnimMap* rigToAnimMap);
  const MR::RigToAnimMap* getRigToAnimMap() const;
  float getDuration() const;
  bool setTime(float time);
  float getTime() const;
  uint32_t getChannelCount() const;
  const NMP::PosQuat* getChannelData() const;
  void getTrajectory(NMP::Quat& orientation, NMP::Vector3& translation) const;

private:
  void clear();

  MR::AnimSourceBase* m_animation;
  unsigned char*      m_buffer;
  NMP::PosQuat*       m_localTransforms;
  uint32_t            m_transformCount;
  float               m_time;
  MR::AnimRigDef*     m_rig;
  MR::RigToAnimMap*   m_rigToAnimMap;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIMATION_SOURCE_HANDLE_H
//----------------------------------------------------------------------------------------------------------------------
