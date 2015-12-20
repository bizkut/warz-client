// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_BODYDEF_H
#define NM_BODYDEF_H


namespace NMP
{
  class DataBuffer;
}

namespace MR
{
  class AnimRigDef;
  class PhysicsRigDef;
}

namespace ER
{

class Body;
class LimbDef;

//----------------------------------------------------------------------------------------------------------------------
/// Definition unique per body type. Each body type is 1 topology.
//----------------------------------------------------------------------------------------------------------------------
class BodyDef
{
public:

  struct SelfAvoidance
  {
    float radius;
  };

  bool locate();
  bool dislocate();

  MR::PhysicsRigDef* getPhysicsRigDef() const { return m_physicsRigDef; }
  void setPhysicsRigDef(MR::PhysicsRigDef* physicsRigDef) { m_physicsRigDef = (MR::PhysicsRigDef*)physicsRigDef; }
  MR::AnimRigDef* getRig() const { return m_rigRef; }
  void setRig(MR::AnimRigDef* rig) { m_rigRef = rig; }

  MR::AnimRigDef* m_rigRef;
  MR::PhysicsRigDef* m_physicsRigDef;
  uint32_t m_numLimbs;
  uint32_t m_rootLimbIndex;
  NMP::DataBuffer* m_defaultPoseAnimData;
  LimbDef** m_limbDefs;

  uint32_t m_numArmLimbs;
  uint32_t m_numHeadLimbs;
  uint32_t m_numLegLimbs;
  uint32_t m_numSpineLimbs;

  SelfAvoidance m_selfAvoidance;

  int32_t getFirstArmLimbIndex() const { return 0; }
  int32_t getFirstHeadLimbIndex() const { return m_numArmLimbs; }
  int32_t getFirstLegLimbIndex() const { return m_numArmLimbs + m_numHeadLimbs; }
  int32_t getFirstSpineLimbIndex() const { return m_numArmLimbs + m_numHeadLimbs + m_numLegLimbs; }

  int32_t getLastArmLimbIndex() const { return m_numArmLimbs - 1; }
  int32_t getLastHeadLimbIndex() const { return m_numArmLimbs + m_numHeadLimbs - 1; }
  int32_t getLastLegLimbIndex() const { return m_numArmLimbs + m_numHeadLimbs + m_numLegLimbs - 1; }
  int32_t getLastSpineLimbIndex() const { return m_numArmLimbs + m_numHeadLimbs + m_numLegLimbs + m_numSpineLimbs - 1; }

  uint32_t getLimbIndexFromName(const char* limbName, bool matchPartial = false) const;
  const char* getLimbNameFromIndex(uint16_t i) const;
  uint32_t getLimbNames(char* names, uint16_t nameLength) const;
  uint32_t getNumLimbs() const { return m_numLimbs; }

protected:
  friend class Body;
};

//----------------------------------------------------------------------------------------------------------------------
bool locateBodyDef(uint32_t assetType, void* assetMemory);

}

#endif // NM_BODYDEF_H
