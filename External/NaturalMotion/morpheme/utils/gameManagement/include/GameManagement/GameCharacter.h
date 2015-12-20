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
  #pragma warning (disable: 4324) // Turn off 'structure was padded due to __declspec(align())' warning.
#endif

#ifdef _MSC_VER
  #pragma once
#endif
#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrDispatcher.h"
#include "morpheme/mrNetwork.h"
#include "GameCharacterDef.h"
#include <string>
#include <list>
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::Character
/// \brief Wraps a morpheme Network instance and the information required to update and run an individual character.
///
/// This is an uninstantiable base class.
/// A Character is instantiated from a GameCharacterDef.
/// In a game scenario a Character Class could also store data for a character instance (current health/ammo etc).
//----------------------------------------------------------------------------------------------------------------------
NMP_ALIGN_PREFIX(16) class Character
{
protected:

  /// \brief Constructor is protected because this is a base class that is not directly instantiable.
  Character():
    m_useOverrideDeltaTransform(false),
    m_name(NULL),
    m_externalIdentifier(0xFFFFFFFF),
    m_characterDef(NULL),
    m_net(NULL),
    m_worldTransforms(NULL),
    m_animRigDef(NULL),
    m_temporaryMemoryAllocator(NULL),
    m_internalTemporaryMemoryAllocator(false),
    m_persistentMemoryAllocator(NULL)
    {
      m_overrideDeltaTranslation = NMP::Vector3::InitZero;
      m_overrideDeltaOrientation = NMP::Quat::kIdentity;
    }
  virtual ~Character() {}

public:

  /// \brief Deinitialise and deallocate a Character instance. Inherited classes must implement this function.
  ///
  /// Note that it is virtual rather than static to make management of lists of Characters of varying types easier.
  /// Freeing the instance memory must be the last task performed in this function.
  virtual void destroy() = 0;
  
  /// \brief Updates the world transforms from the local space root network output.
  void updateWorldTransforms(const NMP::Matrix34& rootTransform);
  
  /// \brief Runs an update step to initialise the network.
  ///
  /// The initial update with a delta time of zero will initialise active connections etc. It is important that the
  /// initial active animation set is set before the first update so the resulting transform output matches the active
  /// rig. This should be called exactly once after the network was created.
  void runInitialisingUpdateStep();

  /// \see MR::Network::broadcastMessage()
  bool broadcastRequestMessage(MR::MessageID messageID, bool status);

  /// \see MR::Network::setControlParameter()
  void setFloatControlParameter(MR::NodeID cpNodeID, float value);

  //----------------------------
  // Accessors
  const char* getName() const { return m_name->c_str(); }
  void setName(const char* const name) { *m_name = name; }

  MR::Network* getNetwork() const { return m_net; }
  const CharacterDef* getCharacterDef() const { return m_characterDef; }

  uint32_t getExternalIdentifier() const { return m_externalIdentifier; }
  void setExternalIdentifier(uint32_t value) { m_externalIdentifier = value; }

  void setCharacterController(MR::CharacterControllerInterface* cc) { m_net->setCharacterController(cc); }
  
  /// \brief Returns the world transforms as evaluated by the most recent network update and the rig it was created for.
  const NMP::DataBuffer* getWorldTransforms(const MR::AnimRigDef*& animRig) const { animRig = m_animRigDef; return m_worldTransforms; }

  /// \brief When updating the characters new world position if we have set to use the override delta transform then these values are used,
  /// otherwise the values calculated and stored in the Network are used.
  void setUseDeltaTransformOverride(bool use) { m_useOverrideDeltaTransform = use;}

  /// \brief Delta transform values to use when updating this frames world position of the character. 
  /// Only used if m_useOverrideDeltaTransform otherwise the values calculated and stored in the Network are used.
  void setDeltaTransformOverride(NMP::Vector3& overrideDeltaTranslation, NMP::Quat& overrideDeltaOrientation);  
  
protected:

  /// \brief Allocate and initialise the members of this base class.
  ///
  /// To be used as a utility by the create function of inherited classes.
  bool initBaseMembers(
    const char*               name,
    CharacterDef*             characterDef,
    MR::AnimSetIndex          initialAnimSetIndex,
    NMP::TempMemoryAllocator* temporaryMemoryAllocator,
    uint32_t                  externalIdentifier = 0xFFFFFFFF);

  /// \brief Deinitialise and deallocate the members of this base class.
  ///
  /// To be used as a utility by the destroy function of inherited classes.
  void termBaseMembers();
  
  /// \brief Initialise the temporary memory allocator.
  ///
  /// Create our own if one is not passed in for us to use.
  void initTemporaryMemoryAllocator(
    NMP::TempMemoryAllocator* temporaryMemoryAllocator);

  /// \brief If we own our own temporary memory allocator free it.
  void termTemporaryMemoryAllocator();

  /// \brief Initialise the persistent memory allocator.
  ///
  /// Each character has its own persistent allocator.
  void initPersistentMemoryAllocator();

  /// \brief Execute the queued Network and continue until the execution result is not MR::EXECUTE_RESULT_IN_PROGRESS. 
  ///
  /// - requiredWaitingTaskID: If on completion there is a waiting task then it must be of ID requiredWaitingTaskID 
  ///    (unless requiredWaitingTaskID == TASK_ID_UNSPECIFIED). If the requiredWaitingTaskID if TASK_ID_UNSPECIFIED
  ///    then there must be no resulting external task.
  /// - requiredExecuteResult: The execution result must be of ID requiredExecuteResult 
  ///    (unless requiredExecuteResult == EXECUTE_RESULT_INVALID).
  MR::Task* executeNetworkWhileInProgress(
    MR::TaskID        requiredWaitingTaskID,
    MR::ExecuteResult requiredExecuteResult = MR::EXECUTE_RESULT_INVALID); 

protected:

  /// If during the update of the character you wish to amend the delta transform computed by the Network
  /// set m_useOverrideDeltaTransform to true and these delta transform values will be used rather than
  /// the values computed and stored in the Network instance.
  /// Example usage would be to:
  ///  + update the Network to MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER.
  ///  + get the Networks delta transform values (getTranslationChange() & getOrientationChange()).
  ///  + amend these values and store them here.
  ///  + finish Network update.
  NMP::Vector3 m_overrideDeltaTranslation;
  NMP::Quat    m_overrideDeltaOrientation; 
  bool         m_useOverrideDeltaTransform;

  std::string*     m_name;               ///< The name of this character instance.
  uint32_t         m_externalIdentifier; ///< Can be used to store a unique identifier for this character instance. 
                                         ///<  Management of this value is entirely up to the user of this class, 
                                         ///<  but it is initialised to 0xFFFFFFFF on Character creation.
  CharacterDef*    m_characterDef;       ///< The character definition that this class is an instance of.

  MR::Network*     m_net;                ///< The network instance for this character.

  NMP::DataBuffer* m_worldTransforms;    ///< The transforms buffer (pose) of the character.
  MR::AnimRigDef*  m_animRigDef;         ///< The currently active animation rig for this network.

  /// If we need to create our own temporary memory allocator this is the size we will make it.
  static const size_t DEFAULT_TEMP_ALLOCATOR_SIZE = 512 * 1024;

  /// A memory allocator that can either be created and managed locally or provided to us.
  NMP::TempMemoryAllocator* m_temporaryMemoryAllocator;
  bool                      m_internalTemporaryMemoryAllocator; ///< Is this our own or a provided allocator

  /// A memory allocator created by and local to this Character instance.
  NMP::MemoryAllocator*     m_persistentMemoryAllocator;

  /// Network update execute result from the last Network::update() call we made.
  MR::ExecuteResult         m_executeResult;
};

//C_ASSERT(sizeof(Character) == 32);

//C_ASSERT(offsetof(Character, m_overrideDeltaTranslation)== (sizeof(void*) + 8));
//C_ASSERT(offsetof(Character, m_overrideDeltaOrientation)== (sizeof(void*) + 8 + 16));
//C_ASSERT(offsetof(Character, wat)==16);

//----------------------------------------------------------------------------------------------------------------------
/// Typedef for a list of Character pointers.
typedef std::list<Character*> CharacterList;

//----------------------------------------------------------------------------------------------------------------------
/// Typedef for a list of CharacterDef pointers.
typedef std::list<CharacterDef*> CharacterDefList;

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_H
//----------------------------------------------------------------------------------------------------------------------

