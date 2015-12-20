// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "euphoria/erCharacterDef.h"
#include "euphoria/erBody.h"
#include "euphoria/erEuphoriaLogger.h"

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
static BodyDef* getBodyDefFromNetwork(MR::NetworkDef* networkDef, MR::AnimSetIndex animSetIndex)
{
  BodyDef* result = NULL;
  MR::AttribData* bodyDefAD = networkDef->getAttribData((MR::AttribDataSemantic)ATTRIB_SEMANTIC_BODY_DEF, 0, animSetIndex);
  NMP_ASSERT(bodyDefAD && bodyDefAD->getType() == ATTRIB_TYPE_BODY_DEF);
  result = ((AttribDataBodyDef*)bodyDefAD)->m_bodyDef;
  NMP_ASSERT(result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterDef::create(MR::NetworkDef* networkDef, ER::BehaviourDefsBase* behaviourDefs)
{
  EUPHORIA_LOG_ENTER_FUNC();
  m_behaviourDefs = behaviourDefs;
  m_networkDef = networkDef;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterDef::destroy()
{
  EUPHORIA_LOG_ENTER_FUNC();
}

//----------------------------------------------------------------------------------------------------------------------
Character* CharacterDef::createInstance(
  MR::AnimRigDef*             rig,
  MR::PhysicsRig*             physicsRig,
  ER::RootModule*             module,
  uint32_t                    collisionIgnoreGroups,
  bool                        addPartsToEuphoria,
  MR::InstanceDebugInterface* debugInterface)
{
  EUPHORIA_LOG_ENTER_FUNC();
  MR::AnimSetIndex animSetIndex;
  for (animSetIndex = 0; animSetIndex < m_networkDef->getNumAnimSets(); animSetIndex++)
  {
    if (m_networkDef->getRig(animSetIndex) == rig)
      break;
  }
  NMP_ASSERT(animSetIndex < m_networkDef->getNumAnimSets()); // can't find body def for this rig
  ER::Character* character = (ER::Character*) NMPMemoryAlloc(sizeof(ER::Character));
  NMP_ASSERT(character);
  new(character) ER::Character();
  character->create(this, module);

  ER::Body* body = Body::createInstance(
    getBodyDefFromNetwork(m_networkDef, animSetIndex),
    physicsRig,
    animSetIndex,
    addPartsToEuphoria);
  NMP_ASSERT(body);

  character->initialise(collisionIgnoreGroups, body, debugInterface);
  return character;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterDef::destroyInstance(Character* character)
{
  EUPHORIA_LOG_ENTER_FUNC();
  NMP_ASSERT(character); // can't destroy a null pointer

  Body::destroyInstance(&character->getBody());
  character->destroy();
  deleteInstance(character);

  return;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterDef::deleteInstance(Character* character)
{
  character->~Character();
  NMP::Memory::memFree(character);
}

} // namespace ER