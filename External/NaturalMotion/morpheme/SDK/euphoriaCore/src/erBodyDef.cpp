// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "morpheme/mrManager.h"
#include "morpheme/mrRig.h"

#include "euphoria/erLimbDef.h"
#include "euphoria/erBodyDef.h"
#include "euphoria/erBody.h"
#include "euphoria/erEuphoriaLogger.h"

#include "NMPlatform/NMMemory.h"

#include "physics/mrPhysicsRigDef.h"

#include <string.h>

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
bool BodyDef::locate()
{
  NMP::endianSwap(m_physicsRigDef);
  m_physicsRigDef =
    (MR::PhysicsRigDef*)MR::Manager::getInstance().getObjectPtrFromObjectID((MR::ObjectID)m_physicsRigDef);

  NMP::endianSwap(m_rigRef);
  m_rigRef = (MR::AnimRigDef*)MR::Manager::getInstance().getObjectPtrFromObjectID((MR::ObjectID)m_rigRef);

  NMP::endianSwap(m_rootLimbIndex);
  NMP::endianSwap(m_numLimbs);

  NMP::endianSwap(m_numArmLimbs);
  NMP::endianSwap(m_numHeadLimbs);
  NMP::endianSwap(m_numLegLimbs);
  NMP::endianSwap(m_numSpineLimbs);

  NMP::endianSwap(m_selfAvoidance.radius);

  REFIX_SWAP_PTR(NMP::DataBuffer, m_defaultPoseAnimData);
  m_defaultPoseAnimData->locate();

  REFIX_SWAP_PTR(LimbDef*, m_limbDefs);

  for (uint32_t i = 0; i < m_numLimbs; ++i)
  {
    REFIX_SWAP_PTR(LimbDef, m_limbDefs[i]);

    m_limbDefs[i]->locate();
    m_limbDefs[i]->m_bodyDef = this;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool BodyDef::dislocate()
{
  m_physicsRigDef = (MR::PhysicsRigDef*)MR::Manager::getInstance().getObjectIDFromObjectPtr(m_physicsRigDef);
  NMP::endianSwap(m_physicsRigDef);

  m_rigRef = (MR::AnimRigDef*)MR::Manager::getInstance().getObjectIDFromObjectPtr(m_rigRef);
  NMP::endianSwap(m_rigRef);

  NMP::endianSwap(m_selfAvoidance.radius);

  NMP::endianSwap(m_rootLimbIndex);
  NMP::endianSwap(m_numArmLimbs);
  NMP::endianSwap(m_numHeadLimbs);
  NMP::endianSwap(m_numLegLimbs);
  NMP::endianSwap(m_numSpineLimbs);

  m_defaultPoseAnimData->dislocate();
  UNFIX_SWAP_PTR(NMP::DataBuffer, m_defaultPoseAnimData);

  for (uint32_t i = 0; i < m_numLimbs; ++i)
  {
    m_limbDefs[i]->dislocate();
    UNFIX_SWAP_PTR(LimbDef, m_limbDefs[i]);
  }
  UNFIX_SWAP_PTR(LimbDef*, m_limbDefs);

  NMP::endianSwap(m_numLimbs);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t BodyDef::getLimbIndexFromName(const char* limbName, bool matchPartial) const
{
  for (uint32_t i = 0 ; i < m_numLimbs; ++i)
  {
    const LimbDef* limb = m_limbDefs[i];
    if (matchPartial)
    {
      if (strstr(limbName, limb->m_name) != NULL)
        return (int32_t)i;
    }
    else
    {
      if (strcmp(limbName, limb->m_name) == 0)
        return (int32_t)i;
    }
  }
  return 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
const char* BodyDef::getLimbNameFromIndex(uint16_t i) const
{
  NMP_ASSERT(i < m_numLimbs);

  return i < m_numLimbs ? m_limbDefs[i]->m_name : (char*)0;
}

uint32_t BodyDef::getLimbNames(char* names, uint16_t nameLength) const
{
  for (uint16_t i = 0; i < m_numLimbs; ++i)
  {
    NMP_STRNCPY_S(&names[nameLength * i], nameLength, getLimbNameFromIndex(i));
  }
  return m_numLimbs;
}

//----------------------------------------------------------------------------------------------------------------------
bool locateBodyDef(uint32_t NMP_USED_FOR_ASSERTS(assetType), void* assetMemory)
{
  NMP_ASSERT(assetType == MR::Manager::kAsset_BodyDef);
  ER::BodyDef* bodyDef = (ER::BodyDef*)assetMemory;
  return bodyDef->locate();
}

} // namespace

//----------------------------------------------------------------------------------------------------------------------
