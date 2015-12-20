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
#ifndef GAME_PHYSX3_MANAGER_H
#define GAME_PHYSX3_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMStringTable.h"
#include "physics/PhysX3/mrPhysicsScenePhysX3.h"
#include <vector>
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::PhysX3Manager
/// \brief Provides simplified access to the PhysX SDK.
///
/// Allows creation and management of a single simple PhysX scene and is content.
/// A game would likely implement PhysX management for itself.
//----------------------------------------------------------------------------------------------------------------------
class PhysX3Manager
{
public:

  PhysX3Manager():
    m_physXFoundation(NULL),
    m_physXProfileZoneManager(NULL),
    m_physXSDK(NULL),
    m_physXCooking(NULL),
    m_defaultPhysXMaterial(NULL),
    m_physXScene(NULL),
    m_physXCpuDispatcher(NULL),
    m_physXCharacterControllerManager(NULL),
    m_defaultPhysXCharacterControllerMaterial(NULL)
  {
    m_physXVisualDebuggerFilename[0] = '\0';
#ifdef NM_HOST_CELL_PPU
    m_spursSimulationTaskset = NULL;
    m_spursQueryTaskset = NULL;
    m_physXSpuDispatcher = NULL;
#endif // NM_HOST_CELL_PPU
  }
  ~PhysX3Manager() 
  {
    // You must have called term to shut down properly.
    NMP_ASSERT(
            !m_physXFoundation &&
            !m_physXProfileZoneManager &&
            !m_physXSDK &&
            !m_physXCooking &&
            !m_defaultPhysXMaterial &&
            !m_physXScene &&
            !m_physXCpuDispatcher &&
            !m_physXCharacterControllerManager &&
            !m_defaultPhysXCharacterControllerMaterial);
  }
  

  //--------------------------------------
  // SDK

  /// \brief Setup the PhysX SDK.
  void init(
#ifdef NM_HOST_CELL_PPU
    CellSpurs2* spursObject,
#endif // NM_HOST_CELL_PPU
    float physicsToleranceScale = 1.0f);

  /// \brief Shut down the PhysX SDK.
  void term();

  /// \brief If NULL connection to the visual debugger will be attempted over TCP/IP,
  ///  otherwise connection will be via this file (writing data straight to the file system).
  void setPhysXVisualDebuggerFileName(const char* fileName);

  /// \brief Attempt to make a connection to the visual debugger, either via TCP/IP or via the file system.
  bool connectToPhysXVisualDebugger();

  /// \brief Turns physics object descriptions in to efficient runtime ready formats.
  physx::PxCooking* getPxCooking() { return m_physXCooking; }

  //--------------------------------------
  // Scene

  // Create and initialise the PhysX scene and character controller manager.
  physx::PxScene* createScene(const NMP::Vector3* gravity = NULL);
  
  /// \brief Destroy the PhysX scene and character controller manager.
  void destroyScene();

  /// \brief Execute a frame of the PhysX global physics simulation.
  void simulate(float deltaTime);

  /// \brief Create a PhysX actor object to act as a ground plane for the scene.
  /// 
  ///  Note that keeping track of what actors have been created and destroyed is up to the caller.
  physx::PxRigidActor* createPhysicsObjectGroundPlane(
    float               groundHeight = 0.0f,  ///< From zero.
    float               planeWidth = 2000.0f, ///< Plane dimension.
    const NMP::Vector3* planeNormal = NULL);  ///< Assumes inverse to gravity direction of not provided;
  
  /// \brief Create a PhysX actor box, that is an immovable.
  ///
  ///  If a rotation is provided it will be used otherwise the box will be world axis aligned.
  ///  Note that keeping track of what actors have been created and destroyed is up to the caller.
  ///  Note uses the default material.
  physx::PxRigidActor* createPhysicsObjectStaticBox(
    const NMP::Vector3& dimensions,
    const NMP::Vector3& position,
    const NMP::Quat*    rotation);
  
  /// \brief Generic function for creation of a PhysX actor of specified geometry type (box, capsule, sphere).
  ///
  ///  Note that keeping track of what actors have been created and destroyed is up to the caller.
  ///  If a material is not provided an internally managed material will be created using the parameters provided.
  physx::PxRigidActor* createPhysicsObject(
    physx::PxGeometryType::Enum geomType,
    physx::PxGeometry*          geometry,
    bool                        dynamic,
    const NMP::Vector3&         position,
    const NMP::Quat&            rotation,
    float                       density,
    bool                        hasCollision,
    physx::PxMaterial*          material,
    float                       staticFriction,
    float                       dynamicFriction,
    float                       skinWidth,
    float                       restitution,
    float                       sleepThreshold,
    float                       maxAngularVelocity,
    float                       linearDamping,
    float                       angularDamping);

  /// \brief Destroy the specified actor object.
  ///  Note that keeping track of what actors have been created and destroyed is up to the caller.
  void destroyPhysicsObject(physx::PxRigidActor *actor);

  /// \brief A function supplied to the loadBundle function that verifies that the bundle was built with the expected
  ///  asset compiler plugins and that they are specified in the correct order.
  static bool validatePluginList(const NMP::OrderedStringTable& pluginList);

  // Accessors.
  NM_INLINE physx::PxPhysics* getPhysXSDK() { return m_physXSDK; }
  NM_INLINE physx::PxScene* getPhysXScene() { return m_physXScene; }
  NM_INLINE physx::PxMaterial* getPhysXSceneDefaultMaterial();
  NM_INLINE physx::PxControllerManager* getPhysXCharacterControllerManager() { return m_physXCharacterControllerManager; }
  NM_INLINE physx::PxMaterial* getDefaultPhysXCharacterControllerMaterial();

protected:

  //---------------------------------------------------
  /// \class ErrorStream 
  /// \brief Our debug message handling class allowing us to handle debug output from the PhysX library.
  ///
  /// Implements the physX abstract interface class physx::PxErrorCallback.
  //---------------------------------------------------
  class ErrorStream : public physx::PxErrorCallback
  {
  public:
    void reportError(physx::PxErrorCode::Enum e, const char* message, const char* file, int line);
    void print(const char* message);
  };

  //---------------------------------------------------
  /// \class PhysXAllocator 
  /// \brief Our memory allocator to be used internally by the PhysX library.
  ///
  /// Implements the physX abstract interface class physx::PxAllocatorCallback.
  //---------------------------------------------------
  class PhysXAllocator : public physx::PxAllocatorCallback
  {
    void* allocate(size_t size, const char*, const char*, int);
    void deallocate(void* ptr);
  };

  //---------------------------------------------------
  // SDK core
  physx::PxFoundation*            m_physXFoundation;         ///< Foundation SDK singleton class
  class PhysXAllocator            m_physXAllocator;          ///< Our memory allocator to be used internally by the PhysX library.
  class ErrorStream               m_physXErrorStream;        ///< Our debug message handling class allowing us to handle debug output from the PhysX library.
  physx::PxProfileZoneManager*    m_physXProfileZoneManager; ///< 
  physx::PxPhysics*               m_physXSDK;                ///< Pointer to our single instance of the PhysX SDK.
  physx::PxCooking*               m_physXCooking;            ///< Turns physics object descriptions in to efficient runtime ready formats.
  physx::PxMaterial*              m_defaultPhysXMaterial;    ///< 
  
  static const uint32_t           sm_maxFilenameLength = 256;
  char                            m_physXVisualDebuggerFilename[sm_maxFilenameLength]; ///< If set to '\0' connection will attempted over TCP/IP,
                                                                                       ///< otherwise connection will be via this file (writing data straight to the file system).
#ifdef NM_HOST_CELL_PPU
  CellSpursTaskset2*              m_spursSimulationTaskset;  ///< Spurs Taskset instance for PhysX simulation tasks.
  CellSpursTaskset2*              m_spursQueryTaskset;       ///< Spurs Taskset instance for PhysX query tasks.
#endif // NM_HOST_CELL_PPU

  physx::PxControllerManager*     m_physXCharacterControllerManager;         ///< Manages all character controllers from all scenes.
  physx::PxMaterial*              m_defaultPhysXCharacterControllerMaterial; ///< Used when creating character controllers.
  
  //---------------------------------------------------
  // Scene
  physx::PxScene*                 m_physXScene;            ///< 
  physx::PxDefaultCpuDispatcher*  m_physXCpuDispatcher;    ///< A CPU task dispatcher. Controls processing of the simulation over multiple threads.
#ifdef NM_HOST_CELL_PPU
  PxDefaultSpuDispatcher*         m_physXSpuDispatcher;    ///< Controls processing of the simulation over multiple SPUs.
#endif // NM_HOST_CELL_PPU

  float                           m_physicsToleranceScale; ///< The value that the default physics tolerances are 
                                                           ///<  multiplied by when the SDK is created.
  MR::MorphemePhysX3ContactModifyCallback m_contactModifyCallback; ///< Registered with PhysX to reduce the suddenness of penetration resolution in character self-contacts.  

  std::vector<physx::PxMaterial*> m_materials;             ///< Materials created by this manager during construction of objects by this manager.
                                                           ///< These materials are destroyed on scene destruction.
};

//----------------------------------------------------------------------------------------------------------------------
// Inline functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE physx::PxMaterial* PhysX3Manager::getPhysXSceneDefaultMaterial()
{
  NMP_ASSERT(m_defaultPhysXMaterial); 
  return m_defaultPhysXMaterial;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE physx::PxMaterial* PhysX3Manager::getDefaultPhysXCharacterControllerMaterial()
{
  NMP_ASSERT(m_defaultPhysXCharacterControllerMaterial); 
  return m_defaultPhysXCharacterControllerMaterial;
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_PHYSX3_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------


