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
#include "comms/liveLinkDataManager.h"
#include "comms/liveLinkNetworkManager.h"
#include "comms/liveLinkSceneObjectManager.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
GameSceneObject::GameSceneObject() :
  m_sceneObject(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
GameSceneObject::~GameSceneObject()
{
  if (m_sceneObject)
  {
    // Destroy the data held by each attribute.
    const uint32_t numAttributes = m_sceneObject->getNumAttributes();
    for (uint32_t attributeIndex = 0; attributeIndex < numAttributes; ++attributeIndex)
    {
      NMP::Memory::memFree(m_sceneObject->getAttribute(attributeIndex)->getData());
    }

    // Destroy the object.
    m_sceneObject->destroyAndFree();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GameSceneObject::init(MCOMMS::SceneObject* sceneObject)
{
  m_sceneObject = sceneObject;
}

//----------------------------------------------------------------------------------------------------------------------
void GameSceneObject::update(float NMP_UNUSED(deltaTime))
{
  // Most scene objects don't need an update implementation.
}

//----------------------------------------------------------------------------------------------------------------------
GameControllerSceneObject::GameControllerSceneObject() :
  m_gameCharacter(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool GameControllerSceneObject::initSceneObject(
  LiveLinkSceneObjectManager* sceneObjectManager, 
  MCOMMS::InstanceID instanceID,
  const NMP::Vector3& characterPosition)
{
  const char* networkName = 0;

  // Initialize the internal pointer to the game character.
  m_gameCharacter = sceneObjectManager->getNetworkManager()->findNetworkByInstanceID(instanceID, networkName);
  if (!m_gameCharacter)
  {
    // Can't find game character for this network instance id.
    return false;
  }

  MR::Network* network = m_gameCharacter;

  MR::CharacterControllerDef::ShapeType shape = network->getActiveCharacterControllerDef()->getShape();
  NMP::Colour colour = network->getActiveCharacterControllerDef()->getColour();
  float height = network->getActiveCharacterControllerDef()->getHeight();
  float width = network->getActiveCharacterControllerDef()->getWidth();
  float depth = network->getActiveCharacterControllerDef()->getDepth();
  float radius = network->getActiveCharacterControllerDef()->getRadius();
  float skinWidth = network->getActiveCharacterControllerDef()->getSkinWidth();

  const NMP::Vector3 worldUpDirection(0, 1, 0);
  m_controllerOffset = worldUpDirection * (height * 0.5f + radius + skinWidth);
  NMP::Vector3 position = characterPosition + m_controllerOffset;

  // Initialise the attributes
  // Note that the max number of attributes must be specified accurately, as the vector container has a fixed max size.
  uint32_t maxNumAttributes = 10;
  uint32_t numAttributes = 0;
  NMP::Memory::Format attributesFormat = AttributesVectorContainer::getMemoryRequirements(maxNumAttributes);
  NMP::Memory::Resource attributesRes = NMPMemoryAllocateFromFormat(attributesFormat);
  AttributesVectorContainer* attributes = AttributesVectorContainer::init(attributesRes, maxNumAttributes);

  attributes->push_back(
    MCOMMS::Attribute::createAttribute(
    "CharacterController",
    MCOMMS::Attribute::SEMANTIC_NAME));
  ++numAttributes;

  NMP::Matrix34 transform(NMP::Matrix34Identity());
  transform.translation() = position;
  attributes->push_back(
    MCOMMS::Attribute::createAttribute(
    transform,
    MCOMMS::Attribute::SEMANTIC_TRANSFORM,
    true));
  ++numAttributes;

  attributes->push_back(
    MCOMMS::Attribute::createAttribute(
    1, // Controller Enabled
    MCOMMS::Attribute::SEMANTIC_ACTIVE_PHYSICS_OBJECT,
    true));
  ++numAttributes;

  switch (shape)
  {
  case MR::CharacterControllerDef::CapsuleShape:
  default:
    attributes->push_back(
      MCOMMS::Attribute::createAttribute(
      MCOMMS::Attribute::CONTROLLER_SHAPE_CAPSULE,
      MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
    break;
  case MR::CharacterControllerDef::CylinderShape:
    attributes->push_back(
      MCOMMS::Attribute::createAttribute(
      MCOMMS::Attribute::CONTROLLER_SHAPE_CYLINDER,
      MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
    break;
  case MR::CharacterControllerDef::BoxShape:
    attributes->push_back(
      MCOMMS::Attribute::createAttribute(
      MCOMMS::Attribute::CONTROLLER_SHAPE_BOX,
      MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
    break;
  case MR::CharacterControllerDef::SphereShape:
    attributes->push_back(
      MCOMMS::Attribute::createAttribute(
      MCOMMS::Attribute::CONTROLLER_SHAPE_SPHERE,
      MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
    break;
  }
  ++numAttributes;

  attributes->push_back(
    MCOMMS::Attribute::createAttribute(
    height,
    MCOMMS::Attribute::SEMANTIC_HEIGHT,
    true));
  ++numAttributes;

  attributes->push_back(
    MCOMMS::Attribute::createAttribute(
    width,
    MCOMMS::Attribute::SEMANTIC_LENGTH,
    true));
  ++numAttributes;

  attributes->push_back(
    MCOMMS::Attribute::createAttribute(
    depth,
    MCOMMS::Attribute::SEMANTIC_DEPTH,
    true));
  ++numAttributes;

  attributes->push_back(
    MCOMMS::Attribute::createAttribute(
    radius,
    MCOMMS::Attribute::SEMANTIC_RADIUS,
    true));
  ++numAttributes;

  attributes->push_back(
    MCOMMS::Attribute::createAttribute(
    skinWidth,
    MCOMMS::Attribute::SEMANTIC_SKIN_WIDTH));
  ++numAttributes;

  attributes->push_back(
    MCOMMS::Attribute::createAttribute(
    colour,
    MCOMMS::Attribute::SEMANTIC_COLOUR));
  ++numAttributes;

  // Create the scene object. The manager assigns a unique id to objects.
  // A copy of the attributes is stored internally.
  MCOMMS::SceneObject* controllerSceneObject = sceneObjectManager->buildSceneObject(numAttributes, attributes);

  // We have the ownership of these attributes so need to take care of destroying them.
  attributes->destroyAttributes();

  NMP::Memory::memFree(attributes);

  GameSceneObject::init(controllerSceneObject);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
GameCharacterSceneObject::GameCharacterSceneObject() :
  m_gameCharacter(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
void GameControllerSceneObject::update(float NMP_UNUSED(deltaTime))
{
  NMP_ASSERT(m_sceneObject && m_gameCharacter);

  MR::Network* network = m_gameCharacter;
  const NMP::Matrix34& worldTransform = network->getCharacterPropertiesWorldRootTransform();
  NMP::Matrix34 controllerTransform = worldTransform;
  controllerTransform.translation() += m_controllerOffset;

  // Update the transform attribute
  const uint32_t numAttributes = m_sceneObject->getNumAttributes();
  for (uint32_t attributeIndex = 0; attributeIndex < numAttributes; ++attributeIndex)
  {
    MCOMMS::Attribute* const attribute = m_sceneObject->getAttribute(attributeIndex);

    switch (attribute->getSemantic())
    {
    case MCOMMS::Attribute::SEMANTIC_TRANSFORM:
      {
        NMP_ASSERT_MSG(
          (attribute->getDataType() == MCOMMS::Attribute::DATATYPE_MATRIX),
          "Attribute with semantic 'SEMANTIC_TRANSFORM' on scene object (id=%u) has invalid data type!",
          m_sceneObject->getSceneObjectID());

        NMP::Matrix34* const transform = (NMP::Matrix34*)attribute->getData();
        transform->initialise(controllerTransform);
      }
      break;
    default:
      break;
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
bool GameCharacterSceneObject::initSceneObject(
  LiveLinkSceneObjectManager* sceneObjectManager, 
  MCOMMS::InstanceID instanceID,
  const char* skinName,
  uint32_t maxPathLen)
{
  const char* networkName = 0;

  // Initialize the internal pointer to the game character.
  m_gameCharacter = sceneObjectManager->getNetworkManager()->findNetworkByInstanceID(instanceID, networkName);
  if (!m_gameCharacter)
  {
    // Can't find game character for this network instance id.
    return false;
  }

  MR::Network* network = m_gameCharacter;
  uint32_t transformCount = network->getCurrentTransformCount();

  // Initialise the attributes
  // Note that the max number of attributes must be specified accurately, as the vector container has a fixed max size.
  const uint32_t maxNumAttributes = 4;
  uint32_t numAttributes = 0;
  NMP::Memory::Format attributesFormat = AttributesVectorContainer::getMemoryRequirements(maxNumAttributes);
  NMP::Memory::Resource attributesRes = NMPMemoryAllocateFromFormat(attributesFormat);
  AttributesVectorContainer* attributes = AttributesVectorContainer::init(attributesRes, maxNumAttributes);

  MCOMMS::Attribute* instanceIDAttr = MCOMMS::Attribute::createAttribute(
    instanceID,
    MCOMMS::Attribute::SEMANTIC_INSTANCEID,
    false);
  attributes->push_back(instanceIDAttr);
  ++numAttributes;

  NMP::Matrix34 transform(NMP::Matrix34::kIdentity);
  MCOMMS::Attribute* transformAttr = MCOMMS::Attribute::createAttribute(
    transform,
    MCOMMS::Attribute::SEMANTIC_TRANSFORM,
    true);
  attributes->push_back(transformAttr);
  ++numAttributes;


  NMP::Matrix34* matrices = 
    (NMP::Matrix34*)
      NMPMemoryAllocAligned(sizeof(NMP::Matrix34) * transformCount, NMP_VECTOR_ALIGNMENT);

  for (uint32_t i = 0; i != transformCount; ++i)
  {
    matrices[i].identity();
  }

  MCOMMS::Attribute* boneMatrices = MCOMMS::Attribute::createAttribute(
    matrices,
    transformCount,
    MCOMMS::Attribute::SEMANTIC_BONE_MATRICES, true);
  NMP::Memory::memFree(matrices);
  attributes->push_back(boneMatrices);
  ++numAttributes;

  // Create a larger attribute than the actual string hint length.
  char* largeSkinName = (char*)NMPMemoryAllocAligned(sizeof(char) * maxPathLen, 4);
  largeSkinName[maxPathLen - 1] = 0;
  MCOMMS::Attribute* skinHint =
    MCOMMS::Attribute::createAttribute(
    largeSkinName,
    MCOMMS::Attribute::SEMANTIC_PATH_HINT,
    true);
  attributes->push_back(skinHint);
  ++numAttributes;

  NMP_STRNCPY_S((char*)skinHint->getData(), maxPathLen, skinName);

  // Create the scene object. The manager assigns a unique id to objects.
  // A copy of the attributes is stored internally.
  MCOMMS::SceneObject* characterSceneObject = sceneObjectManager->buildSceneObject(numAttributes, attributes);

  // We have the ownership of these attributes so need to take care of destroying them.
  attributes->destroyAttributes();

  NMP::Memory::memFree(attributes);
  NMP::Memory::memFree(largeSkinName);

  GameSceneObject::init(characterSceneObject);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t GameCharacterSceneObject::computeLocalTransforms(
  MR::Network* network,
  uint32_t NMP_USED_FOR_ASSERTS(numOutputBufferEntries),
  NMP::Matrix34* outputBuffer) const
{
  NMP_ASSERT(outputBuffer);
  const uint32_t resultTransformCount = network->getCurrentTransformCount();
  NMP::DataBuffer* sourceBuffer = network->getTransforms();

  NMP_ASSERT(numOutputBufferEntries >= resultTransformCount);

  if (sourceBuffer && resultTransformCount > 0)
  {
    const NMP::Matrix34& worldTransform = network->getCharacterPropertiesWorldRootTransform();
    outputBuffer[0].initialise(worldTransform);

    // Generate output local transform matrices from the network pos quat buffer.
    for (uint32_t i = 1; i < sourceBuffer->getLength(); ++i)
    {
      outputBuffer[i].initialise(*sourceBuffer->getPosQuatChannelQuat(i), *sourceBuffer->getPosQuatChannelPos(i));
    }

    return resultTransformCount;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void GameCharacterSceneObject::update(float NMP_UNUSED(deltaTime))
{
  NMP_ASSERT(m_gameCharacter && m_sceneObject);
  
  MR::Network* network = m_gameCharacter;

  const uint32_t numAttributes = m_sceneObject->getNumAttributes();
  for (uint32_t attributeIndex = 0; attributeIndex < numAttributes; ++attributeIndex)
  {
    MCOMMS::Attribute* const attribute = m_sceneObject->getAttribute(attributeIndex);

    switch (attribute->getSemantic())
    {
    case MCOMMS::Attribute::SEMANTIC_BONE_MATRICES:
      {
        NMP_ASSERT_MSG(
          (attribute->getDataType() == MCOMMS::Attribute::DATATYPE_MATRIX),
          "Attribute with semantic 'SEMANTIC_BONE_MATRICES' on scene object (id=%u) has invalid data type!",
          m_sceneObject->getSceneObjectID());

        NMP_ASSERT_MSG(
          (attribute->getDataCount() >= network->getCurrentTransformCount()),
          "Scene object (id=%u) has insufficient space for %u transforms!",
          m_sceneObject->getSceneObjectID(), network->getCurrentTransformCount());

        NMP::Matrix34* const transforms = (NMP::Matrix34*)attribute->getData();
        const size_t numTransforms = attribute->getDataCount();

        if (numTransforms >= network->getCurrentTransformCount())
        {
          computeLocalTransforms(network, (uint32_t)numTransforms, transforms);
        }
      }
      break;

    case MCOMMS::Attribute::SEMANTIC_PARENT_BONE_ID:
      {
        NMP_ASSERT_MSG(
          (attribute->getDataType() == MCOMMS::Attribute::DATATYPE_INT),
          "Attribute with semantic 'SEMANTIC_PARENT_BONE_ID' on scene object (id=%u) has invalid data type!",
          m_sceneObject->getSceneObjectID());

        int32_t* const parentIndices = (int32_t*)attribute->getData();
        const uint32_t numParentIndices = (uint32_t)attribute->getDataCount();
        const uint32_t numCurrentTransforms = network->getCurrentTransformCount();

        if (numParentIndices >= numCurrentTransforms)
        {
          const MR::AnimRigDef* const currentRig = network->getActiveRig();
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
void GameCharacterSceneObject::setSkin(const char* skinName)
{
  NMP_ASSERT(m_sceneObject);
  MCOMMS::Attribute* const skinHintAttribute = m_sceneObject->getAttribute(MCOMMS::Attribute::SEMANTIC_PATH_HINT);

  if (skinHintAttribute)
  {
    NMP_ASSERT(strlen(skinName) + 1 <= skinHintAttribute->getDataSize());
    NMP_STRNCPY_S(((char*)skinHintAttribute->getData()), skinHintAttribute->getDataSize(), skinName);
  }
}

//----------------------------------------------------------------------------------------------------------------------
GameEnvironmentSceneObject::GameEnvironmentSceneObject()
{
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Matrix34& GameEnvironmentSceneObject::getTransform() const
{
  NMP_ASSERT(m_sceneObject);
  MCOMMS::Attribute* transformAttribute = m_sceneObject->getAttribute(MCOMMS::Attribute::SEMANTIC_TRANSFORM);
  NMP_ASSERT(transformAttribute);
  const NMP::Matrix34& transform = *((NMP::Matrix34*)transformAttribute->getData());
  return transform;
}

//----------------------------------------------------------------------------------------------------------------------
void GameEnvironmentSceneObject::setTransform(const NMP::Matrix34& transform)
{
  NMP_ASSERT(m_sceneObject);
  MCOMMS::Attribute* transformAttribute = m_sceneObject->getAttribute(MCOMMS::Attribute::SEMANTIC_TRANSFORM);
  NMP_ASSERT(transformAttribute);
  *((NMP::Matrix34*)transformAttribute->getData()) = transform;
}

//----------------------------------------------------------------------------------------------------------------------
GameShapeSceneObject::GameShapeSceneObject()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool GameShapeSceneObject::initSceneObject(
  LiveLinkSceneObjectManager* sceneObjectManager, 
  const GameShapeSceneObject::ShapeDesc& shapeDesc)
{
  // Initialise the attributes
  // Note that the max number of attributes must be specified accurately, as the vector container has a fixed max size.
  const uint32_t maxNumAttributes = 7;
  uint32_t numAttributes = 0;
  NMP::Memory::Format attributesFormat = AttributesVectorContainer::getMemoryRequirements(maxNumAttributes);
  NMP::Memory::Resource attributesRes = NMPMemoryAllocateFromFormat(attributesFormat);
  AttributesVectorContainer* attributes = AttributesVectorContainer::init(attributesRes, maxNumAttributes);

  const NMP::Matrix34& transform = shapeDesc.initialTransform;

  MCOMMS::Attribute* transformAttr = MCOMMS::Attribute::createAttribute(
    transform,
    MCOMMS::Attribute::SEMANTIC_TRANSFORM,
    true);
  attributes->push_back(transformAttr);
  ++numAttributes;

  MCOMMS::Attribute* physicsObjectIDAttr = MCOMMS::Attribute::createAttribute(
    0xFFFFFFFF,
    MCOMMS::Attribute::SEMANTIC_PHYSICS_OBJECT_ID,
    true);
  attributes->push_back(physicsObjectIDAttr);
  ++numAttributes;

  bool motionEnabled = shapeDesc.motionEnabled;

  MCOMMS::Attribute* isDynamicObjectAttr = MCOMMS::Attribute::createAttribute(
    static_cast<uint32_t>((motionEnabled) ? 1 : 0),
    MCOMMS::Attribute::SEMANTIC_DYNAMIC_PHYSICS_OBJECT);
  attributes->push_back(isDynamicObjectAttr);
  ++numAttributes;

  // set the shape specific attributes
  ShapeDesc::ShapeType shapeType = shapeDesc.shapeType;
  switch (shapeType)
  {
    case ShapeDesc::kShapeBox:
    {
      // Box: params = { len, depth, height }
      MCOMMS::Attribute* shapeTypeAttr = MCOMMS::Attribute::createAttribute(
        MCOMMS::Attribute::PHYSICS_SHAPE_BOX,
        MCOMMS::Attribute::SEMANTIC_PHYSICS_SHAPE_TYPE,
        false);
      attributes->push_back(shapeTypeAttr);
      ++numAttributes;

      float value = shapeDesc.params[0];

      MCOMMS::Attribute* lengthAttr = MCOMMS::Attribute::createAttribute(
        value, 
        MCOMMS::Attribute::SEMANTIC_LENGTH);
      attributes->push_back(lengthAttr);
      ++numAttributes;

      value = shapeDesc.params[1];

      MCOMMS::Attribute* depthAttr = MCOMMS::Attribute::createAttribute(
        value,
        MCOMMS::Attribute::SEMANTIC_DEPTH);
      attributes->push_back(depthAttr);
      ++numAttributes;

      value = shapeDesc.params[2];

      MCOMMS::Attribute* heightAttr = MCOMMS::Attribute::createAttribute(
        value,
        MCOMMS::Attribute::SEMANTIC_HEIGHT);
      attributes->push_back(heightAttr);
      ++numAttributes;
    }
    break;
    case ShapeDesc::kShapeCylinder:
    {
      // Cylinder: params = { radius, height }
      MCOMMS::Attribute* shapeTypeAttr = MCOMMS::Attribute::createAttribute(
        MCOMMS::Attribute::PHYSICS_SHAPE_CAPSULE,
        MCOMMS::Attribute::SEMANTIC_PHYSICS_SHAPE_TYPE,
        false);
      attributes->push_back(shapeTypeAttr);
      ++numAttributes;

      float value = shapeDesc.params[0];

      MCOMMS::Attribute* radiusAttr = MCOMMS::Attribute::createAttribute(
        value,
        MCOMMS::Attribute::SEMANTIC_RADIUS);
      attributes->push_back(radiusAttr);
      ++numAttributes;

      value = shapeDesc.params[1];

      MCOMMS::Attribute* heightAttr = MCOMMS::Attribute::createAttribute(
        value,
        MCOMMS::Attribute::SEMANTIC_HEIGHT);
      attributes->push_back(heightAttr);
      ++numAttributes;
    }
    break;
    case ShapeDesc::kShapeCapsule:
    {
      // Capsule: params = { radius, height }
      MCOMMS::Attribute* shapeTypeAttr = MCOMMS::Attribute::createAttribute(
        MCOMMS::Attribute::PHYSICS_SHAPE_CAPSULE,
        MCOMMS::Attribute::SEMANTIC_PHYSICS_SHAPE_TYPE,
        false);
      attributes->push_back(shapeTypeAttr);
      ++numAttributes;

      float value = shapeDesc.params[0];

      MCOMMS::Attribute* radiusAttr = MCOMMS::Attribute::createAttribute(
        value,
        MCOMMS::Attribute::SEMANTIC_RADIUS);
      attributes->push_back(radiusAttr);
      ++numAttributes;

      // !!! In the SceneGraph, the Length attribute is used to specify the "height" of the capsule.
      value = shapeDesc.params[1];

      MCOMMS::Attribute* heightAttr = MCOMMS::Attribute::createAttribute(
        value,
        MCOMMS::Attribute::SEMANTIC_HEIGHT);
      attributes->push_back(heightAttr);
      ++numAttributes;
    }
    break;
    case ShapeDesc::kShapeSphere:
    {
      // Sphere: params = { radius }
      MCOMMS::Attribute* shapeTypeAttr = MCOMMS::Attribute::createAttribute(
        MCOMMS::Attribute::PHYSICS_SHAPE_SPHERE,
        MCOMMS::Attribute::SEMANTIC_PHYSICS_SHAPE_TYPE,
        false);
      attributes->push_back(shapeTypeAttr);
      ++numAttributes;

      float value = shapeDesc.params[0];

      MCOMMS::Attribute* radiusAttr = MCOMMS::Attribute::createAttribute(
        value,
        MCOMMS::Attribute::SEMANTIC_RADIUS);
      attributes->push_back(radiusAttr);
      ++numAttributes;
    }
    break;
    default:
      // Invalid shape type.
      NMP_ASSERT_FAIL();
  }

  // Create the scene object. The manager assigns a unique id to objects.
  // A copy of the attributes is stored internally.
  MCOMMS::SceneObject* shapeSceneObject = sceneObjectManager->buildSceneObject(numAttributes, attributes);

  // We have the ownership of these attributes so need to take care of destroying them.
  attributes->destroyAttributes();

  NMP::Memory::memFree(attributes);

  GameSceneObject::init(shapeSceneObject);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
GameMeshSceneObject::GameMeshSceneObject()
{

}

//----------------------------------------------------------------------------------------------------------------------
bool GameMeshSceneObject::initSceneObject(
  LiveLinkSceneObjectManager* sceneObjectManager,
  const MeshDesc& meshDesc)
{
  NMP_ASSERT(meshDesc.numPoints > 0 && meshDesc.pointBuffer &&
             meshDesc.numIndices > 0 && meshDesc.indexBuffer);

  // Initialise the attributes
  const uint32_t maxNumAttributes = 7;
  uint32_t numAttributes = 0;
  NMP::Memory::Format attributesFormat = AttributesVectorContainer::getMemoryRequirements(maxNumAttributes);
  NMP::Memory::Resource attributesRes = NMPMemoryAllocateFromFormat(attributesFormat);
  AttributesVectorContainer* attributes = AttributesVectorContainer::init(attributesRes, maxNumAttributes);

  const NMP::Matrix34& transform = meshDesc.initialTransform;

  MCOMMS::Attribute* transformAttr = MCOMMS::Attribute::createAttribute(
    transform,
    MCOMMS::Attribute::SEMANTIC_TRANSFORM,
    true);
  attributes->push_back(transformAttr);
  ++numAttributes;

  MCOMMS::Attribute* physicsObjectIDAttr = MCOMMS::Attribute::createAttribute(
    0xFFFFFFFF,
    MCOMMS::Attribute::SEMANTIC_PHYSICS_OBJECT_ID,
    true);
  attributes->push_back(physicsObjectIDAttr);
  ++numAttributes;

  bool motionEnabled = meshDesc.motionEnabled;

  MCOMMS::Attribute* isDynamicObjectAttr = MCOMMS::Attribute::createAttribute(
    static_cast<uint32_t>((motionEnabled) ? 1 : 0),
    MCOMMS::Attribute::SEMANTIC_DYNAMIC_PHYSICS_OBJECT);
  attributes->push_back(isDynamicObjectAttr);
  ++numAttributes;

  // set the mesh specific attributes
  MCOMMS::Attribute::PhysicsShapeType shapeType = MCOMMS::Attribute::PHYSICS_SHAPE_MESH;
  if (motionEnabled)
  {
    shapeType = MCOMMS::Attribute::PHYSICS_SHAPE_CONVEX_HULL;
  }

  MCOMMS::Attribute* shapeTypeAttr = MCOMMS::Attribute::createAttribute(
    shapeType,
    MCOMMS::Attribute::SEMANTIC_PHYSICS_SHAPE_TYPE,
    false);
  attributes->push_back(shapeTypeAttr);
  ++numAttributes;

  // Add the points. The attribute allocates memory for the vertices.
  MCOMMS::Attribute* vertsAttr = MCOMMS::Attribute::createAttribute(
    meshDesc.pointBuffer,
    meshDesc.numPoints,
    MCOMMS::Attribute::SEMANTIC_VERTEX_POSITIONS);
  attributes->push_back(vertsAttr);
  ++numAttributes;

  // Add the indices
  MCOMMS::Attribute* indexAttr = MCOMMS::Attribute::createAttribute(
    meshDesc.indexBuffer,
    meshDesc.numIndices,
    MCOMMS::Attribute::SEMANTIC_VERTEX_INDICES);
  attributes->push_back(indexAttr);
  ++numAttributes;

  MCOMMS::Attribute::VerticesWindingType windingType = 
    meshDesc.clockWise ? MCOMMS::Attribute::VERTICES_WINDING_CLOCKWISE : MCOMMS::Attribute::VERTICES_WINDING_COUNTERCLOCKWISE;

  // set the winding attribute
  MCOMMS::Attribute* windingAttr = MCOMMS::Attribute::createAttribute(
    windingType,
    MCOMMS::Attribute::SEMANTIC_VERTICES_WINDING,
    false);
  attributes->push_back(windingAttr);
  ++numAttributes;

  // Create the scene object. The manager assigns a unique id to objects.
  // A copy of the attributes is stored internally.
  MCOMMS::SceneObject* shapeSceneObject = sceneObjectManager->buildSceneObject(numAttributes, attributes);

  // We have the ownership of these attributes so need to take care of destroying them.
  attributes->destroyAttributes();

  NMP::Memory::memFree(attributes);

  GameSceneObject::init(shapeSceneObject);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
LiveLinkSceneObjectManager::LiveLinkSceneObjectManager(LiveLinkNetworkManager* networkManager, uint32_t maxNumSceneObjects) :
  m_networkManager(networkManager)
{
  NMP::Memory::Format sceneObjectsFormat = GameSceneObjectMap::getMemoryRequirements(maxNumSceneObjects);
  NMP::Memory::Resource sceneObjectsRes = NMPMemoryAllocateFromFormat(sceneObjectsFormat);
  m_sceneObjects = GameSceneObjectMap::init(sceneObjectsRes, maxNumSceneObjects);
}

//----------------------------------------------------------------------------------------------------------------------
LiveLinkSceneObjectManager::~LiveLinkSceneObjectManager()
{
  // Scene objects should have been revoked and freed at this point.
  NMP_ASSERT(m_sceneObjects->size() == 0);

  NMP::Memory::memFree(m_sceneObjects);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t LiveLinkSceneObjectManager::getNumSceneObjects() const
{
  return m_sceneObjects->size();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t LiveLinkSceneObjectManager::getSceneObjects(MCOMMS::SceneObject** objects, uint32_t maxObjects) const
{
  const GameSceneObjectMap::iterator end = m_sceneObjects->end();
  GameSceneObjectMap::iterator it = m_sceneObjects->begin();

  for (uint32_t i = 0; (it != end) && (i < maxObjects); ++i, ++it)
  {
    objects[i] = (*it).value->getObject();
  }

  return m_sceneObjects->size();
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::SceneObject* LiveLinkSceneObjectManager::getSceneObject(MCOMMS::SceneObjectID objectID) const
{
  GameSceneObjectMap::iterator it = m_sceneObjects->find(objectID);

  if (it == m_sceneObjects->end())
  {
    return 0;
  }

  return (*it).value->getObject();
}

//----------------------------------------------------------------------------------------------------------------------
void LiveLinkSceneObjectManager::onConnectionClosed(MCOMMS::Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkSceneObjectManager::publishSceneObject(GameSceneObject* gameSceneObject)
{
  if (!gameSceneObject || !gameSceneObject->getObject())
  {
    return false;
  }

  m_sceneObjects->insert(gameSceneObject->getObject()->getSceneObjectID(), gameSceneObject);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkSceneObjectManager::revokeSceneObject(GameSceneObject* gameSceneObject)
{
  if (!gameSceneObject || !gameSceneObject->getObject())
  {
    return false;
  }

  GameSceneObjectMap::iterator iter = m_sceneObjects->find(gameSceneObject->getObject()->getSceneObjectID());
  if (iter != m_sceneObjects->end())
  {
    m_sceneObjects->erase(iter);
    return true;
  }

  // Return false if we couldn't find the object.
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::SceneObject* LiveLinkSceneObjectManager::findSceneObject(MCOMMS::SceneObjectID objectID) const
{
  GameSceneObjectMap::iterator it = m_sceneObjects->find(objectID);

  if (it == m_sceneObjects->end())
  {
    return NULL;
  }

  return (*it).value->getObject();
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::SceneObject* LiveLinkSceneObjectManager::buildSceneObject(uint32_t numAttributes, AttributesVectorContainer* attributes)
{
  NMP_ASSERT(attributes);

  // Assign unique id
  MCOMMS::SceneObjectID id = getUnusedSceneObjectID();

  // Create object. Use 'numAttributes' as the vector container may not be full.
  MCOMMS::SceneObject* const sceneObject = MCOMMS::SceneObject::allocAndCreate(id, numAttributes);

  // Allocate space for each attribute and set to the new object.
  uint32_t attributeIndex = 0;
  for (AttributesVectorContainer::iterator iter = attributes->begin(); iter != attributes->end(); ++iter)
  {
    const MCOMMS::Attribute* const attribute = *(iter);
    const size_t dataSize = attribute->getDataSize();

    void* const data = NMPMemoryAllocAligned(dataSize, 16);
    NMP_ASSERT(data);

    MCOMMS::Attribute* const objectAttribute = sceneObject->getAttribute(attributeIndex);
    objectAttribute->init((*attribute->getDescriptor()), data);
    objectAttribute->copyDataFrom(attribute);
    ++attributeIndex;
  }

  return sceneObject;
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::SceneObjectID LiveLinkSceneObjectManager::getUnusedSceneObjectID()
{
  static uint32_t nextObjectID = 0;
  return nextObjectID++;
}

//----------------------------------------------------------------------------------------------------------------------
void LiveLinkSceneObjectManager::updateSceneObjects(float deltaTime)
{
  for (GameSceneObjectMap::iterator iter = m_sceneObjects->begin(); iter != m_sceneObjects->end(); ++iter)
  {
    (*iter).value->update(deltaTime);
  }
}

} // namespace COMMS
