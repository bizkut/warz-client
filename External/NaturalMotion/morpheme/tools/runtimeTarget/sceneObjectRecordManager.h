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
#ifndef NM_RTT_SCENEOBJECTRECORDMANAGER_H
#define NM_RTT_SCENEOBJECTRECORDMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/sceneObject.h"
#include <map>
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
class Connection;
}

class IControllerMgr;
class DefaultPhysicsMgr;
class NetworkInstanceRecordManager;

//----------------------------------------------------------------------------------------------------------------------
/// \class SceneObjectRecord
/// \brief Decorates a scene object with additional information as required by the runtime target to operate.
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class SceneObjectRecord
{
public:

  // Creates a record and scene object from the given attributes.
  static SceneObjectRecord* create(
    MCOMMS::SceneObjectID           id,
    uint32_t                        numAttributes,
    const MCOMMS::Attribute* const* attributes,
    MCOMMS::Connection*             owner);

  // Destroys the scene object and the record instance. Do not use the record after calling this method!
  void releaseAndDestroy();

  MCOMMS::Connection* getOwner() const { return m_owner; }
  MCOMMS::SceneObject* getObject() const { return m_object; }

  void update(
    float deltaTime,
    const NetworkInstanceRecordManager* networkInstanceManager,
    const IControllerMgr* controllerMgr);

  /// \brief This custom assignment operator is required as the compiler cannot generate the default assignment operator
  ///  due to const members.
  SceneObjectRecord & operator= (const SceneObjectRecord & other) {*this = other; return *this;}

private:

  MCOMMS::Connection*   const m_owner;
  MCOMMS::SceneObject*  const m_object;

  SceneObjectRecord(MCOMMS::Connection* owner, MCOMMS::SceneObject* object);
};

typedef std::map<MCOMMS::SceneObjectID, SceneObjectRecord*> SceneObjectMap;
typedef std::pair<MCOMMS::SceneObjectID, SceneObjectRecord*> SceneObjectMapEntry;

//----------------------------------------------------------------------------------------------------------------------
/// \class SceneObjectRecordManager
/// \brief Keeps track of the set of scene objects on the runtime target.
/// \ingroup RuntimeTarget
///
/// This class simply keeps track of the set of scene objects for use across the various runtime target components.
/// Scene objects are generally created/destroyed by DefaultSceneObjectMgr but DefaultControllerMgr will also create
/// scene objects for the character controllers.
//----------------------------------------------------------------------------------------------------------------------
class SceneObjectRecordManager
{
public:

  SceneObjectRecordManager();
  ~SceneObjectRecordManager();

  /// \brief Returns the scene object associated with the given ID or NULL is such a record does not exist.
  MCOMMS::SceneObject* findSceneObject(MCOMMS::SceneObjectID objectID) const;

  /// \brief Adds the given scene object record to the map.
  void addSceneObjectRecord(SceneObjectRecord* sceneObjectRecord);

  /// \brief Removes a scene object record indicated by the given ID.
  SceneObjectRecord* removeSceneObjectRecord(MCOMMS::SceneObjectID objectID);

  /// \briefs Destroys all scene object records associated with the given network instance.
  void destorySceneObjectRecords(MCOMMS::InstanceID instanceID);

  /// \briefs Destroys all scene objects owned by the given connection.
  void destorySceneObjectRecords(MCOMMS::Connection* connection, DefaultPhysicsMgr& physicsManager);

  /// \brief Updates all scene object records.
  void updateSceneObjectRecords(
    float deltaTime,
    const NetworkInstanceRecordManager* networkInstanceManager,
    const IControllerMgr* controllerMgr);

  /// \brief Returns the actual scene object record map.
  SceneObjectMap& getSceneObjectMap() { return m_sceneObjects; }

  /// \brief Returns the actual scene object record map.
  const SceneObjectMap& getSceneObjectMap() const { return m_sceneObjects; }

private:

  SceneObjectMap                m_sceneObjects;
  mutable SceneObjectMapEntry   m_cachedSceneObject;

  void resetCache();
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_RTT_SCENEOBJECTRECORDMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
