// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef GAME_PHYSX2_MANAGER_H
#define GAME_PHYSX2_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMStringTable.h"
#include "physics/PhysX2/mrPhysicsScenePhysX2.h"
#include "mrPhysX2Includes.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::PhysX2Manager
/// \brief Provides simplified access to the PhysX SDK.
///
/// Allows creation and management of a single simple PhysX scene and is content.
/// A game would likely implement PhysX management for itself.
//----------------------------------------------------------------------------------------------------------------------
class PhysX2Manager
{
public:

  PhysX2Manager():
    m_physXSDK(NULL),
    m_physXScene(NULL),
    m_physXCharacterControllerManager(NULL)
  {
  };
  ~PhysX2Manager() 
  {
    // You must have called term to shut down properly.
    NMP_ASSERT(!m_physXSDK && !m_physXScene && !m_physXCharacterControllerManager);
  };

  /// \brief Setup the PhysX SDK
  void init(
#ifdef NM_HOST_CELL_PPU
    CellSpurs* spursObject /// Used to initialise PhysX with spurs.
                           /// Supplied either direct from the application or with NMP::SPUManager::getSpursObject() once AnimSystemModulePhysX2 has been initiliased.
#endif // NM_HOST_CELL_PPU  
    );

  /// \brief Shut down the PhysX SDK.
  void term();

  // Create and initialise the PhysX scene and character controller manager.
  NxScene* createScene(const NMP::Vector3* gravity = NULL);
  
  /// \brief Destroy the PhysX scene and character controller manager.
  void destroyScene();

  /// \brief Execute a frame of the PhysX global physics simulation.
  void simulate(float deltaTime);

  /// \brief Performs a global update of the PhysX character controller manager.
  ///  Handles response to any changes in the physic scene or character controllers.
  void updateCharacterControllerManager();

  /// \brief Create a PhysX actor object to act as a ground plane for the scene.
  /// 
  ///  Note that keeping track of what actors have been created and destroyed is up to the caller.
  NxActor* createPhysicsObjectGroundPlane(
    float               groundHeight = 0.0f,  ///< From zero.
    float               planeWidth = 2000.0f, ///< Plane dimension.
    const NMP::Vector3* planeNormal = NULL);  ///< Assumes inverse to gravity direction of not provided;

  /// \brief Create a PhysX actor box, that is an immovable.
  ///
  ///  If a rotation is provided it will be used otherwise the box will be world axis aligned.
  ///  Note that keeping track of what actors have been created and destroyed is up to the caller.
  NxActor* createPhysicsObjectStaticBox(
    const NMP::Vector3& dimensions,
    const NMP::Vector3& position,
    const NMP::Quat*    rotation);
  
  /// \brief Generic function for creation of PhysX actor objects.
  ///  Note that keeping track of what actors have been created and destroyed is up to the caller.
  NxActor* createPhysicsObject(
    uint32_t             geometryType,
    bool                 isDynamic,
    bool                 hasCollision,
    const NMP::Vector3&  halfDimensions,
    const NMP::Vector3&  position,
    const NMP::Quat&     orientation,
    float                density,
    NxTriangleMeshShape* triMeshGeom = NULL);

  /// \brief Destroy the specified actor object.
  ///  Note that keeping track of what actors have been created and destroyed is up to the caller.
  void destroyPhysicsObject(NxActor* actor);

  /// \brief A function supplied to the loadBundle function that verifies that the bundle was built with the expected
  ///  asset compiler plugins and that they are specified in the correct order.
  static bool validatePluginList(const NMP::OrderedStringTable& pluginList);

  // Accessors.
  NM_INLINE NxPhysicsSDK* getPhysXSDK() { return m_physXSDK; }
  NM_INLINE NxScene* getPhysXScene() { return m_physXScene; }
  NM_INLINE NxControllerManager* getPhysXCharacterControllerManager() { return m_physXCharacterControllerManager; }
  NM_INLINE NxMaterial* getPhysXSceneDefaultMaterial();
  
protected:
 
  //----------------------------------------------------------------------------------------------------------------------
  /// \class UserOutputStream
  /// \brief Implementation of an NxUserOutputStream to allow us to catch and print out any messages or errors from PhysX.
  //----------------------------------------------------------------------------------------------------------------------
  class UserOutputStream: public NxUserOutputStream
  {
    virtual void reportError(NxErrorCode code, const char* message, const char* file, int line);

    virtual NxAssertResponse reportAssertViolation(const char* message, const char* file, int line);

    virtual void print(const char * message);
  };
  
  NxPhysicsSDK*        m_physXSDK;                        ///< Pointer to the singleton instance of the PhysX SDK.
  NxScene*             m_physXScene;                      ///< 
  NxControllerManager* m_physXCharacterControllerManager; ///< Manages an array of character controllers.
  UserOutputStream     m_physXOutputStream;               ///< For catching and printing out any messages or errors from PhysX.
};

//----------------------------------------------------------------------------------------------------------------------
// Inline functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NxMaterial* PhysX2Manager::getPhysXSceneDefaultMaterial()
{
  // Make sure that the SDK and scene are initialised.
  NMP_ASSERT(m_physXSDK && m_physXScene);

  // Material index 0 on the scene is our default material for the scene.
  return m_physXScene->getMaterialFromIndex(0);
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_PHYSX2_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------


