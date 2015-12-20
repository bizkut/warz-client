// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
#ifndef NM_CHARACTER_DEF_H
#define NM_CHARACTER_DEF_H

#include "erCharacter.h"

namespace MR
{
  class NetworkDef;
  class AnimRigDef;
  class PhysicsRig;
  class InstanceDebugInterface;
}

namespace ER
{

class RootModule;
struct BehaviourDefsBase;

//----------------------------------------------------------------------------------------------------------------------
/// one per character rig (including all LODs). Responsible also for creating instances
//----------------------------------------------------------------------------------------------------------------------
class CharacterDef
{
  friend class Character;
public:
  CharacterDef() {}
  virtual ~CharacterDef() {}

  virtual void create(MR::NetworkDef* networkDef, ER::BehaviourDefsBase* behaviourDefs);
  virtual void destroy();

  virtual Character* createInstance(
    MR::AnimRigDef*             rig,
    MR::PhysicsRig*             physicsRig,
    ER::RootModule*             module,
    uint32_t                    collisionIgnoreGroups,
    bool                        addPartsToEuphoria,
    MR::InstanceDebugInterface* debugInterface);

  virtual void destroyInstance(Character* character);

protected:
  void deleteInstance(Character* character);

  BehaviourDefsBase* m_behaviourDefs; // these could be referenced so many characterDefs share behaviourDefs
  MR::NetworkDef* m_networkDef;
}; // class CharacterDef

} // namespace ER

#endif
