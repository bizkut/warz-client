// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

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
#ifndef MCOMMS_LIVE_LINK_SCENE_OBJECT_MANAGER_H
#define MCOMMS_LIVE_LINK_SCENE_OBJECT_MANAGER_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMapContainer.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

class LiveLinkNetworkManager;
class LiveLinkSceneObjectManager;

//----------------------------------------------------------------------------------------------------------------------
class GameSceneObject
{
public:
  GameSceneObject();
  virtual ~GameSceneObject();

  /// \brief Setup the managed scene object.
  void init(MCOMMS::SceneObject* sceneObject);

  /// \brief Implements the update scheme for derived objects.
  virtual void update(float deltaTime);

  /// \brief Returns the managed scene object.
  NM_INLINE MCOMMS::SceneObject* getObject() const
  {
    return m_sceneObject;
  }

protected:
  MCOMMS::SceneObject* m_sceneObject;
};

typedef NMP::MapContainer<MCOMMS::SceneObjectID, GameSceneObject*> GameSceneObjectMap;

//----------------------------------------------------------------------------------------------------------------------
/// \class GameControllerSceneObject
/// \brief Implements the scene object that represents a character controller
///
//----------------------------------------------------------------------------------------------------------------------
class GameControllerSceneObject : public GameSceneObject
{
public:
  GameControllerSceneObject();

  /// \brief
  bool initSceneObject(
    LiveLinkSceneObjectManager* sceneObjectManager, 
    MCOMMS::InstanceID instanceID,
    const NMP::Vector3& characterPosition);

  /// \brief
  virtual void update(float deltaTime) NM_OVERRIDE;

private:
  uint8_t pad[2];

  /// \brief 
  MR::Network* m_gameCharacter;

  /// \brief
  NMP::Vector3 m_controllerOffset;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class GameCharacterSceneObject
/// \brief Scene object used to show skins in connect.
///
//----------------------------------------------------------------------------------------------------------------------
class GameCharacterSceneObject : public GameSceneObject
{
public:
  GameCharacterSceneObject();

  /// \brief maxPathLen specifies the maximum length used to describe a skin path.
  bool initSceneObject(
    LiveLinkSceneObjectManager* sceneObjectManager, 
    MCOMMS::InstanceID instanceID, 
    const char* skinName,
    uint32_t maxPathLen = 512);

  /// \brief Updates the dynamic attributes of the object.
  virtual void update(float deltaTime) NM_OVERRIDE;

  /// \brief
  void setSkin(const char* skinName);

private:
  /// \brief
  uint32_t computeLocalTransforms(
    MR::Network* network, 
    uint32_t NMP_USED_FOR_ASSERTS(numOutputBufferEntries),
    NMP::Matrix34* outputBuffer) const;

  /// \brief
  MR::Network* m_gameCharacter;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class GameEnvironmentSceneObject
/// \brief Base class for environment scene objects.
///
//----------------------------------------------------------------------------------------------------------------------
class GameEnvironmentSceneObject : public GameSceneObject
{
public:
  GameEnvironmentSceneObject();

  /// \brief Retrieves the scene object's current transform attribute.
  const NMP::Matrix34& getTransform() const;

  /// \brief Updates the scene object's transform attribute.
  void setTransform(const NMP::Matrix34& transform);
};

//----------------------------------------------------------------------------------------------------------------------
/// \class GameMeshSceneObject
/// \brief Environment scene object for meshes.
///
//----------------------------------------------------------------------------------------------------------------------
class GameMeshSceneObject : public GameEnvironmentSceneObject
{
public:
  GameMeshSceneObject();

  class MeshDesc
  {
  public:
    MeshDesc() :
      initialTransform(NMP::Matrix34::kIdentity),
      motionEnabled(false),
      pointBuffer(0),
      numPoints(0),
      indexBuffer(0),
      numIndices(0),
      clockWise(false)
    {
    }

    NMP::Matrix34 initialTransform;
    bool motionEnabled;
    NMP::Vector3* pointBuffer;
    uint32_t numPoints;
    int* indexBuffer;
    uint32_t numIndices;
    bool clockWise;
  };

  bool initSceneObject(
    LiveLinkSceneObjectManager* sceneObjectManager,
    const MeshDesc& meshDesc);
};

//----------------------------------------------------------------------------------------------------------------------
/// \class GameShapeSceneObject
/// \brief Environment scene object for basic shapes.
///
//----------------------------------------------------------------------------------------------------------------------
class GameShapeSceneObject : public GameEnvironmentSceneObject
{
public:
  GameShapeSceneObject();

  class ShapeDesc
  {
  public:
    ShapeDesc() :
      initialTransform(NMP::Matrix34::kIdentity),
      motionEnabled(false),
      shapeType(kShapeBox)
    {
      params[0] = params[1] = params[2] = 1;
    }

    enum ShapeType 
    {
      kShapeBox,
      kShapeCylinder,
      kShapeCapsule,
      kShapeSphere
    };
  public:
    /// \brief Initial transform for the shape.
    NMP::Matrix34 initialTransform;

    /// \brief 
    bool motionEnabled;

    /// \brief Dimensional parameters for the shape.
    // Box: params = { len, depth, height }
    // Cylinder: params = { radius, height }
    // Capsule: params = { radius, height }
    // Sphere: params = { radius }
    float params[3];
    ShapeType shapeType;
  };

  /// \brief
  bool initSceneObject(
    LiveLinkSceneObjectManager* sceneObjectManager,
    const ShapeDesc& shapeDesc);
};

//----------------------------------------------------------------------------------------------------------------------
class AttributesVectorContainer : public NMP::VectorContainer<MCOMMS::Attribute*>
{
public:
  void resetValues()
  {
    for (AttributesVectorContainer::iterator iter = begin(); iter != end(); ++iter)
    {
      (*iter) = 0;
    }
  }

  void destroyAttributes()
  {
    for (AttributesVectorContainer::iterator iter = begin(); iter != end(); ++iter)
    {
      MCOMMS::Attribute* attribute = (*iter);
      if (!attribute)
        continue;

      NMP::Memory::memFree((void*)(*iter)->getData());
      MCOMMS::Attribute::destroy((*iter));
    }
  }

  static NM_INLINE AttributesVectorContainer* init(NMP::Memory::Resource& resource, uint32_t maxEntries)
  {
    AttributesVectorContainer* result = 
        (AttributesVectorContainer*)NMP::VectorContainer<MCOMMS::Attribute*>::init(resource, maxEntries);
    result->resetValues();
    return result;
  }

private:
  AttributesVectorContainer();
  ~AttributesVectorContainer();
};

//----------------------------------------------------------------------------------------------------------------------
class LiveLinkSceneObjectManager : public MCOMMS::SceneObjectManagementInterface
{
public:
  LiveLinkSceneObjectManager(LiveLinkNetworkManager* networkManager, uint32_t maxNumSceneObjects);
  virtual ~LiveLinkSceneObjectManager();

  /// \brief Checks if the implementation of this interface supports creation of scene objects.
  /// The COMMS layer uses this information to communicate runtime capabilities to morpheme:connect.
  virtual bool canCreateSceneObjects() const NM_OVERRIDE
  {
    return false;
  }

  /// \brief Checks if the implementation of this interface permits editing of scene objects. As this cannot be enforced
  /// through the implementation of the interface (scene object attributes can be accessed directly on the same) this
  /// capability relies on COMMS respecting the implementations preference.
  virtual bool canEditSceneObjects() const NM_OVERRIDE
  {
    return false;
  }

  /// \brief Returns the number of scene objects available on the runtime target.
  virtual uint32_t getNumSceneObjects() const NM_OVERRIDE;

  /// \brief Requests that a scene object be added with the given attributes. The validity of the given attributes is
  /// only guaranteed for the duration of the call, implementations must create local copies of attributes retained for
  /// later use.
  virtual MCOMMS::SceneObject* createSceneObject(
    uint32_t NMP_UNUSED(numAttributes),
    const MCOMMS::Attribute* const* NMP_UNUSED(attributes),
    MCOMMS::Connection* NMP_UNUSED(owner),
    MCOMMS::SceneObject *& NMP_UNUSED(createdCharacterController)) NM_OVERRIDE
  {
    return NULL;
  }

  /// \brief Signals the application the the specified object is no longer
  /// required by connect.
  virtual bool destroySceneObject(
    MCOMMS::SceneObjectID NMP_UNUSED(objectID),
    MCOMMS::SceneObjectID& NMP_UNUSED(destroyedCharacterController)) NM_OVERRIDE
  {
    return false;
  }

  /// \brief Stores up to the given maximum number of runtime scene object in the provided list (in no particular order)
  /// and returns the number of scene objects available.
  virtual uint32_t getSceneObjects(MCOMMS::SceneObject** objects, uint32_t maxObjects) const NM_OVERRIDE;

  /// \brief Returns the scene object associated with the given ID or NULL.
  virtual MCOMMS::SceneObject* getSceneObject(MCOMMS::SceneObjectID objectID) const NM_OVERRIDE;

  /// \brief Called when a connection is closed.
  virtual void onConnectionClosed(MCOMMS::Connection* connection) NM_OVERRIDE;

public:
  /// \brief
  bool publishSceneObject(GameSceneObject* gameSceneObject);

  /// \brief
  bool revokeSceneObject(GameSceneObject* gameSceneObject);

public:
  /// \brief
  NM_INLINE LiveLinkNetworkManager* getNetworkManager() const { return m_networkManager; }

  /// \brief
  MCOMMS::SceneObject* buildSceneObject(uint32_t numAttributes, AttributesVectorContainer* attributes);

  /// \brief
  void updateSceneObjects(float deltaTime);

  /// \brief
  MCOMMS::SceneObjectID getUnusedSceneObjectID();

protected:
  LiveLinkNetworkManager* m_networkManager;
  /// \brief Returns the scene object associated with the given ID or NULL is such a record does not exist.
  MCOMMS::SceneObject* findSceneObject(MCOMMS::SceneObjectID objectID) const;

  GameSceneObjectMap*  m_sceneObjects;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_LIVE_LINK_SCENE_OBJECT_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
