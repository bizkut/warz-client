// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_BEHAVIOUR_H
#define NM_BEHAVIOUR_H

#include "erCharacter.h"

namespace MR
{
  struct BehaviourAnimationData;
}

namespace ER
{

class Body;
class Behaviour;
class Module;

//----------------------------------------------------------------------------------------------------------------------
/// Unique per behaviour, per body type (ie for rig based tweaking)... how would we author this? seems hard
/// Definition of behaviour, and used to create instances
//----------------------------------------------------------------------------------------------------------------------
class BehaviourDef
{
public:

  struct ModuleToEnable
  {
    unsigned int  m_moduleIndex;
    bool          m_enableWithChildren;
  };

  virtual ~BehaviourDef() {}
  Behaviour* createInstance();
  virtual Behaviour* newInstance() const = 0;
  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// General interface to any behaviour
//----------------------------------------------------------------------------------------------------------------------
class Behaviour
{
public:
  virtual ~Behaviour() {}
  virtual void create(BehaviourDef* behaviourDef);
  virtual void initialise(Character* owner);
  virtual void destroy();
  virtual bool isAnimationRequired(int32_t animationID) const = 0;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) = 0;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) = 0;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) = 0;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) = 0;
  virtual void handleEmittedMessages(uint32_t& messages) = 0;
  virtual void start();
  virtual void stop();
  virtual const char* getBehaviourName() const = 0;
  virtual int getBehaviourID() const = 0;
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) = 0;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) = 0;
  virtual int getHighestPriority();
  virtual void setHighestPriority(int32_t priority);
  // This function is overwritten by the children of this class that have output control parameters.
  // Behaviour class calls this function on itself therefore it is not pure virtual.
  virtual void clearOutputControlParamsData() {};

  BehaviourDef* getDefinition() const { return m_definition; }
  bool getEnabled() const { return m_enabled > 0; }

protected:
  const Character* m_owner;
  BehaviourDef* m_definition;
  int32_t m_priority;
  int32_t m_enabled; // int so works when on multiple enables and disables
};

}
#endif // NM_BEHAVIOUR_H
