// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_NETWORKINTERFACE_H
#define NM_NETWORKINTERFACE_H

#include "NMPlatform/NMMemory.h"
#include "erBehaviour.h"

namespace ER
{

class Module;

//----------------------------------------------------------------------------------------------------------------------
/// This is just a little container arrangement that means that auto-generated NetworkInterfaces can allocate all the
/// defs they need without needing two allocation calls.
//----------------------------------------------------------------------------------------------------------------------
struct BehaviourDefsBase
{
  virtual ~BehaviourDefsBase() {};
  virtual unsigned int  getNumBehaviourDefs() const = 0;
  virtual BehaviourDef* getBehaviourDef(unsigned int index) const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
template <unsigned int Size>
struct BehaviourDefs : public BehaviourDefsBase
{
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(BehaviourDefsBase) + sizeof(BehaviourDef*) * Size, NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  virtual BehaviourDef* getBehaviourDef(unsigned int index) const NM_OVERRIDE { NMP_ASSERT(index < Size); return m_defs[index]; }
  virtual unsigned int getNumBehaviourDefs() const NM_OVERRIDE { return Size; }

  BehaviourDef* m_defs[Size];
};

//----------------------------------------------------------------------------------------------------------------------
/// Interface to a behaviour network library.
//----------------------------------------------------------------------------------------------------------------------
class NetworkInterface
{

public:

  NetworkInterface() : m_defs(0), m_ownsMemorySystem(false) {}
  virtual ~NetworkInterface() {}

  // call getDesc() to see if this is a network you want to / can use
  struct Description
  {
    uint32_t    m_uid;
    uint8_t     m_numLimbsSupported;
  };

  virtual const Description& getDesc() const = 0;

  /// Call init() on derived instances to build instances of behaviour defs
  virtual void init() = 0;

  /// Returns managed array of BehaviourDefs describing all available behaviours, instantiated during ::init()
  virtual BehaviourDefsBase* getBehaviourDefs() const { NMP_ASSERT(m_defs); return m_defs; }

  /// Creates an instance of the behaviour network, returns root module
  virtual ER::RootModule* createNetwork() const = 0;

  /// Destroy an instance of the behaviour network
  virtual bool destroyNetwork(ER::Module** rootModuleOut) const = 0;

  /// Call term to tidy up, shut down memory subsystem, delete behaviourdef instances
  virtual void term()
  {
    // Shutdown the memory system only if owned.
    if (m_ownsMemorySystem)
      NMP::Memory::shutdown();
  }

protected:
  BehaviourDefsBase*   m_defs;
  bool                 m_ownsMemorySystem;

}; // class NetworkInterface

} // namespace ER

#endif // NM_NETWORKINTERFACE_H
