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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"
#include "comms/mcomms.h"
#include "comms/sceneObject.h"
#include "comms/packet.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
uint32_t SceneObject::getMemorySize(uint32_t numAttributes)
{
  return (sizeof(SceneObject) + (sizeof(Attribute) * numAttributes));
}

//----------------------------------------------------------------------------------------------------------------------
SceneObject* SceneObject::init(void* mem, SceneObjectID objectID, uint32_t numAttributes)
{
  SceneObject* object = reinterpret_cast<SceneObject*>(mem);

  object->m_objectID = objectID;
  object->m_numAttributes = numAttributes;

  if (numAttributes != 0)
  {
    memset(object->getAttribute(0), 0, sizeof(Attribute) * numAttributes);
  }

  object->m_firstUpdate = 1;

  return object;
}

//----------------------------------------------------------------------------------------------------------------------
void SceneObject::destroy()
{
  // Nothing to do
}

//----------------------------------------------------------------------------------------------------------------------
SceneObject* SceneObject::allocAndCreate(SceneObjectID objectID, uint32_t numAttributes)
{
  uint32_t memSize = getMemorySize(numAttributes);
  void* objectMem = NMPMemoryAllocAligned(memSize, sizeof(void*));
  NMP_ASSERT(objectMem);

  return init(objectMem, objectID, numAttributes);
}

//----------------------------------------------------------------------------------------------------------------------
void SceneObject::destroyAndFree()
{
  destroy();
  NMP::Memory::memFree(this);
}

} // namespace MCCOMMS

//----------------------------------------------------------------------------------------------------------------------
