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
#include "sceneObjectRecordManager.h"

#include "morpheme/mrNetwork.h"
#include "comms/sceneObject.h"

#include "iControllerMgr.h"
#include "networkInstanceRecordManager.h"
#include "defaultPhysicsMgr.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// SceneObjectRecord
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
SceneObjectRecord* SceneObjectRecord::create(
  MCOMMS::SceneObjectID           id,
  uint32_t                        numAttributes,
  const MCOMMS::Attribute* const* attributes,
  MCOMMS::Connection* owner)
{
  // Create object.
  MCOMMS::SceneObject* const object = MCOMMS::SceneObject::allocAndCreate(id, numAttributes);

  // Allocate space for each attribute and set to the new object.
  for (uint32_t attributeIndex = 0 ; attributeIndex < numAttributes; ++attributeIndex)
  {
    const MCOMMS::Attribute* const attribute = attributes[attributeIndex];
    const size_t dataSize = attribute->getDataSize();

    void* const data = NMPMemoryAllocAligned(dataSize, 16);
    NMP_ASSERT(data);

    MCOMMS::Attribute* const objectAttribute = object->getAttribute(attributeIndex);
    objectAttribute->init((*attribute->getDescriptor()), data);
    objectAttribute->copyDataFrom(attributes[attributeIndex]);
  }

  // Create object record.
  void* sceneObjectRecordMemPtr = NMPMemoryAlloc(sizeof(SceneObjectRecord));
  NMP_ASSERT(sceneObjectRecordMemPtr);
  SceneObjectRecord* const sceneObjectRecord = new(sceneObjectRecordMemPtr) SceneObjectRecord(owner, object);

  // ...
  return sceneObjectRecord;
}

//----------------------------------------------------------------------------------------------------------------------
void SceneObjectRecord::releaseAndDestroy()
{
  // Destroy the data held by each attribute.
  const uint32_t numAttributes = m_object->getNumAttributes();
  for (uint32_t attributeIndex = 0; attributeIndex < numAttributes; ++attributeIndex)
  {
    NMP::Memory::memFree(m_object->getAttribute(attributeIndex)->getData());
  }

  // Destroy the object.
  m_object->destroyAndFree();

  // Destroy the object record.
  this->~SceneObjectRecord();
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
void SceneObjectRecord::update(
  float NMP_UNUSED(deltaTime),
  const NetworkInstanceRecordManager* networkInstanceManager,
  const IControllerMgr* controllerMgr)
{
  // There is only something to do if this object has a network instance ID associated with it.
  MCOMMS::Attribute* const instanceIDAttribute = m_object->getAttribute(MCOMMS::Attribute::SEMANTIC_INSTANCEID);
  if (!instanceIDAttribute)
  {
    return;
  }

  // Find the instance record for this object.
  const MCOMMS::InstanceID instanceID = *(MCOMMS::InstanceID*)instanceIDAttribute->getData();
  NetworkInstanceRecord* const instanceRecord = networkInstanceManager->findInstanceRecord(instanceID);
  if (!instanceRecord)
  {
    NMP_MSG("SceneObject with ID %d has invalid network instance", m_object->getSceneObjectID());
    return;
  }

  MR::Network* const instance = instanceRecord->getNetwork();
  if (!instance)
  {
    NMP_MSG("SceneObject with ID %d has invalid network instance", m_object->getSceneObjectID());
    return;
  }

  // Update each of the appropriate attributes on this object
  const uint32_t numAttributes = m_object->getNumAttributes();
  for (uint32_t attributeIndex = 0; attributeIndex < numAttributes; ++attributeIndex)
  {
    MCOMMS::Attribute* const attribute = m_object->getAttribute(attributeIndex);

    switch (attribute->getSemantic())
    {
    case MCOMMS::Attribute::SEMANTIC_BONE_MATRICES:
      {
        NMP_ASSERT_MSG(
          (attribute->getDataType() == MCOMMS::Attribute::DATATYPE_MATRIX),
          "Attribute with semantic 'SEMANTIC_BONE_MATRICES' on scene object (id=%u) has invalid data type!",
          m_object->getSceneObjectID());

        NMP_ASSERT_MSG(
          (attribute->getDataCount() >= instanceRecord->getNetwork()->getCurrentTransformCount()),
          "Scene object (id=%u) has insufficient space for %u transforms!",
          m_object->getSceneObjectID(), instanceRecord->getNetwork()->getCurrentTransformCount());

        NMP::Matrix34* const transforms = (NMP::Matrix34*)attribute->getData();
        const size_t numTransforms = attribute->getDataCount();

        if (numTransforms >= instance->getCurrentTransformCount())
        {
          instanceRecord->computeLocalTransforms((uint32_t)numTransforms, transforms);
        }
      }
      break;

    case MCOMMS::Attribute::SEMANTIC_TRANSFORM:
      {
        NMP_ASSERT_MSG(
          (attribute->getDataType() == MCOMMS::Attribute::DATATYPE_MATRIX),
          "Attribute with semantic 'SEMANTIC_TRANSFORM' on scene object (id=%u) has invalid data type!",
          m_object->getSceneObjectID());

        NMP::Matrix34* const transform = (NMP::Matrix34*)attribute->getData();
        transform->initialise(controllerMgr->getOrientation(instanceID), controllerMgr->getPosition(instanceID));
      }
      break;

    case MCOMMS::Attribute::SEMANTIC_PARENT_BONE_ID:
      {
        NMP_ASSERT_MSG(
          (attribute->getDataType() == MCOMMS::Attribute::DATATYPE_INT),
          "Attribute with semantic 'SEMANTIC_PARENT_BONE_ID' on scene object (id=%u) has invalid data type!",
          m_object->getSceneObjectID());

        int32_t* const parentIndices = (int32_t*)attribute->getData();
        const uint32_t numParentIndices = (uint32_t)attribute->getDataCount();
        const uint32_t numCurrentTransforms = instance->getCurrentTransformCount();

        if (numParentIndices >= numCurrentTransforms)
        {
          const MR::AnimRigDef* const currentRig = instance->getActiveRig();
          for (uint32_t i = 0; i != numCurrentTransforms; ++i)
          {
            parentIndices[i] = currentRig->getParentBoneIndex(i);
          }

          for (uint32_t i = numCurrentTransforms; i != numParentIndices; ++i)
          {
            parentIndices[i] = -1;
          }
        }
      }
      break;

    default:
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
SceneObjectRecord::SceneObjectRecord(MCOMMS::Connection* owner, MCOMMS::SceneObject* object) :
  m_owner(owner),
  m_object(object)
{
}

//----------------------------------------------------------------------------------------------------------------------
// SceneObjectRecordManager
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
SceneObjectRecordManager::SceneObjectRecordManager() :
  m_sceneObjects(),
  m_cachedSceneObject()
{
  resetCache();
}

//----------------------------------------------------------------------------------------------------------------------
SceneObjectRecordManager::~SceneObjectRecordManager()
{
  NMP_ASSERT(m_sceneObjects.empty());
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::SceneObject* SceneObjectRecordManager::findSceneObject(MCOMMS::SceneObjectID objectID) const
{
  // Check cached (MRU) value first ...
  if (m_cachedSceneObject.first == objectID)
  {
    return m_cachedSceneObject.second->getObject();
  }

  // ... then search whole map
  SceneObjectMap::const_iterator it = m_sceneObjects.find(objectID);

  if (it == m_sceneObjects.end())
  {
    return NULL;
  }
  else
  {
    m_cachedSceneObject.first = objectID;
    m_cachedSceneObject.second = it->second;

    return it->second->getObject();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SceneObjectRecordManager::addSceneObjectRecord(SceneObjectRecord* sceneObjectRecord)
{
  if (!findSceneObject(sceneObjectRecord->getObject()->getSceneObjectID()))
  {
    m_sceneObjects.insert(SceneObjectMapEntry(sceneObjectRecord->getObject()->getSceneObjectID(), sceneObjectRecord));
  }
  else
  {
    NMP_ASSERT(findSceneObject(sceneObjectRecord->getObject()->getSceneObjectID()) == sceneObjectRecord->getObject());
  }
}

//----------------------------------------------------------------------------------------------------------------------
SceneObjectRecord* SceneObjectRecordManager::removeSceneObjectRecord(MCOMMS::SceneObjectID objectID)
{
  if (m_cachedSceneObject.first == objectID)
  {
    resetCache();
  }

  SceneObjectMap::iterator it = m_sceneObjects.find(objectID);

  if (it == m_sceneObjects.end())
  {
    NMP_ASSERT_FAIL_MSG("Attempt to destroy a scene object that was already destroyed!");
    return NULL;
  }
  else
  {
    SceneObjectRecord* const object = it->second;

    m_sceneObjects.erase(it);

    return object;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SceneObjectRecordManager::destorySceneObjectRecords(MCOMMS::InstanceID instanceID)
{
  const SceneObjectMap::iterator end = m_sceneObjects.end();
  SceneObjectMap::iterator it = m_sceneObjects.begin();

  while (it != end)
  {
    MCOMMS::SceneObject* const sceneObject = it->second->getObject();
    MCOMMS::Attribute* const attribute = sceneObject->getAttribute(MCOMMS::Attribute::SEMANTIC_INSTANCEID);

    if (!attribute)
    {
      ++it;
    }
    else
    {
      const MCOMMS::InstanceID objectInstanceID = *(MCOMMS::InstanceID*)attribute->getData();
      if (objectInstanceID == instanceID)
      {
        it->second->releaseAndDestroy();
        m_sceneObjects.erase(it++);
      }
      else
      {
        ++it;
      }
    }
  }

  resetCache();
}

//----------------------------------------------------------------------------------------------------------------------
void SceneObjectRecordManager::destorySceneObjectRecords(MCOMMS::Connection* connection, DefaultPhysicsMgr& physicsManager)
{
  const SceneObjectMap::iterator end = m_sceneObjects.end();
  SceneObjectMap::iterator it = m_sceneObjects.begin();

  while (it != end)
  {
    if (it->second->getOwner() == connection)
    {
      physicsManager.destroyPhysicsBody(it->first);

      it->second->releaseAndDestroy();
      m_sceneObjects.erase(it++);
    }
    else
    {
      ++it;
    }
  }

  resetCache();
}

//----------------------------------------------------------------------------------------------------------------------
void SceneObjectRecordManager::updateSceneObjectRecords(
  float deltaTime,
  const NetworkInstanceRecordManager* networkInstanceManager,
  const IControllerMgr* controllerMgr)
{
  const SceneObjectMap::const_iterator end = m_sceneObjects.end();
  for (SceneObjectMap::const_iterator it = m_sceneObjects.begin(); it != end; ++it)
  {
    SceneObjectRecord* const sceneObject = (*it).second;
    sceneObject->update(deltaTime, networkInstanceManager, controllerMgr);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SceneObjectRecordManager::resetCache()
{
  m_cachedSceneObject.first = 0XFFFFFFFE;
  m_cachedSceneObject.second = NULL;
}
