// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_MODULE_H
#define NM_MODULE_H

#include "euphoria/erCharacter.h"
#include "erDebugDraw.h"

#define EUPHORIA_PROFILINGx
#define EUPHORIA_PROFILING_DETAILEDx

namespace MR
{
struct PhysicsSerialisationBuffer;
}

namespace ER
{

class Junction;
class ModuleCon;

// ---------------------------------------------------------------------------------------------------------------------
// the data allocator is a system that ensures that all input/output/etc data is stored in
// separate contiguous blocks; it is derived from by a code-generated class that will have
// knowledge about how much space is required for each block (and thus doesn't pollute any
// include dependencies). Note there is no symmetric 'free' function, as we don't ever need
// to free the data blocks until the network is torn down, at which point term() is called
// to free the allocated memory.
//
class ModuleDataAllocator
{
public:

  virtual ~ModuleDataAllocator() {}

  // ordering does not need to match the module ordering
  enum Section
  {
    Data,
    Inputs,
    UserInputs,
    Outputs,
    FeedbackInputs,
    FeedbackOutputs,
    NumSections
  };

  // allocate space for allocation (must allocate 128-byte-aligned)
  virtual void init() = 0;

  // returns a chunk of memory for the given section (performs no actual memory allocation)
  // 'alignment' refers to the required type alignment, the returned block will be aligned to this value
  virtual void* alloc(ER::ModuleDataAllocator::Section sct, size_t size, size_t alignment) = 0;

  // memset's the section to zero
  virtual void clear(ER::ModuleDataAllocator::Section sct) = 0;

  // throw away allocated memory
  virtual void term() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// The Module is the base type that the entire behaviour network derives from
///
/// very few constraints are enforced here, though the auto-code generation will add restricted ways of using this
/// even so, modules are very flexible. The desired constraints are that they:
/// 1. obey the rules that allow their children to be run in parallel
/// 2. apply a combiner scheme from competing (parallel) inputs
/// 3. implement construction so variables are never uninitialised
class Module
{
public:

  Module(ER::ModuleDataAllocator* NMP_UNUSED(mdAllocator), ER::ModuleCon* moduleCon) :
    m_moduleCon(moduleCon),
    m_childIndex(-1),
    m_manifestIndex(-1)
  {
  }

  virtual ~Module();

  virtual void clearAllData() = 0;

  virtual void create(Module* NMP_UNUSED(parent), int childIndex = -1) { m_childIndex = childIndex; }
  virtual void entry() = 0;

  virtual int32_t getNumChildren() const = 0;
  virtual Module* getChild(int32_t index) const = 0;
  virtual const char* getChildName(int32_t index) const = 0;

  /// This will save all aspects of the body network state except for which modules are enabled
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) = 0;
  /// This will restore all aspects of the network state, except for which modules are enabled
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) = 0;

  // This clears all the data in this module and calls initialise on all its children
  void initialise();

  bool storeStateChildren(MR::PhysicsSerialisationBuffer& savedState);
  bool restoreStateChildren(MR::PhysicsSerialisationBuffer& savedState);

  ModuleCon* getConnections() { return m_moduleCon; }
  virtual const Module* getOwner() const { return 0; }
  virtual const char* getClassName() const { return 0; }

  NM_INLINE int getChildIndex() const { return m_childIndex; }
  NM_INLINE int getManifestIndex() const { return m_manifestIndex; }

  // called from the generated root module during construction
  NM_INLINE void assignManifestIndex(int index) { NMP_ASSERT(m_manifestIndex == -1); m_manifestIndex = index; }

  // Step up the owner hierarchy until we find the root (MyNetwork).
  const RootModule* getRootModule() const;

  // Gets all the module names (active or not). numModules should be set to 0, and on return will
  // contain the number of unique module names. The name of each modules parent is also returned
  // (with the same index as the associated module name).
  void getModuleNames(char* names, char* parentNames, int32_t nameLength, int32_t& numModules);

protected:

  ModuleCon* m_moduleCon;

private:

  int m_childIndex;     // if in an array of child modules, which index into the array this instance is. -1 if not in an array
  int m_manifestIndex;  // entry from NetworkManifest::Modules, a unique index into the master module table

  Module();
};

//----------------------------------------------------------------------------------------------------------------------
class ModuleCon
{
public:
  virtual ~ModuleCon() {}

  virtual void create(Module* module, Module* owner) = 0;

  // Combiner functions are optionally overridden by subclasses with relevant
  // connections and hence the need for an associated combiner. Otherwise they 
  // are not so the following are provided as a set of no-op defaults.
  //
  virtual void combineInputs(ER::Module* NMP_UNUSED(modulePtr))         {}
  virtual void combineOutputs(ER::Module* NMP_UNUSED(modulePtr))        {}
  virtual void combineFeedbackInputs(ER::Module* NMP_UNUSED(modulePtr)) {}
  virtual void combineFeedbackOutputs(ER::Module* NMP_UNUSED(modulePtr)){}
};

//----------------------------------------------------------------------------------------------------------------------
class RootModule : public Module
{
public:

  RootModule();

  int32_t getAllDebugEnabledModuleNames(char* names, int nameLength) const;
  int32_t getAllEnabledModules(Module** modules);
  bool isModuleEnabled(const char* name);
  uint32_t getNumModules() const { return m_numModules; }

  void setCharacter(ER::Character* myCharacter);
  ER::Character* getCharacter() const {return m_character;}

  virtual void update(float timeStep) = 0;
  virtual void feedback(float timeStep) = 0;

  virtual void initLimbIndices() = 0;

  // to be derived from the generated classes, will return the
  // generated data allocator class
  virtual ER::ModuleDataAllocator* getDataAllocator() = 0;

  const char* getModuleName(uint32_t moduleIndex) const { return m_allModules[moduleIndex]->getClassName(); }
  Module* getModule(uint32_t moduleIndex) const { return m_allModules[moduleIndex]; }

  // control of module enabling / disabling
  void enable(uint32_t moduleIndex);
  void disable(uint32_t moduleIndex);
  void enableOwners(uint32_t moduleIndex);
  void disableOwners(uint32_t moduleIndex);
  void enableModuleAndChildren(uint32_t moduleIndex);
  void disableModuleAndChildren(uint32_t moduleIndex);
  void setDebugInterface(MR::InstanceDebugInterface* debugInterface);
  MR::InstanceDebugInterface* getDebugInterface() const { return m_debugInterface; }

  // The network's random number generator seed is initialised to a value that will be different for
  // all characters and from run to run. If the game wants determinism then they can subsequently
  // call setNetworkRandomSeed(). 
  inline void setNetworkRandomSeed(uint32_t seed) { m_networkSeed = seed; }
  // This gets the network random seed and then modifies it, so that if modules contain RNGs and
  // start at the same time, their random numbers won't be identical, but also if the seed is
  // initialised to a constant then the behaviour can be made deterministic.
  inline uint32_t getAndBumpNetworkRandomSeed() const { return m_networkSeed++; }

  NM_INLINE bool isEnabled(uint32_t moduleIndex) const
  {
    NMP_ASSERT(moduleIndex < m_numModules);
    const ModuleEnableState& mes = m_moduleEnableStates[moduleIndex];
    return (mes.m_enabled && mes.m_enabledByOwner);
  }

#if defined(MR_OUTPUT_DEBUGGING)
  void getDebugEnabledModuleNames(char* names, int nameLength, int& numModules) const;
#endif

  NM_INLINE void setEnabledByOwner(uint32_t moduleIndex, bool enabled)
  {
    NMP_ASSERT(moduleIndex < m_numModules);
    ModuleEnableState& mes = m_moduleEnableStates[moduleIndex];
    mes.m_enabledByOwner = enabled;
  }
  NM_INLINE bool isEnabledByOwner(uint32_t moduleIndex) const
  {
    NMP_ASSERT(moduleIndex < m_numModules);
    const ModuleEnableState& mes = m_moduleEnableStates[moduleIndex];
    return mes.m_enabledByOwner != 0;
  }

  void getEnabledModules(Module** modules, int& numModules);

  // auto-generated execution code
  virtual void executeNetworkUpdate(float timeStep) = 0;
  virtual void executeNetworkFeedback(float timeStep) = 0;

protected:

  MR::InstanceDebugInterface* m_debugInterface;

  ER::Character*  m_character;

  // flattened list of all modules held in this network
  ER::Module**     m_allModules;
  uint32_t         m_numModules;
  mutable uint32_t m_networkSeed;

  struct ModuleEnableState
  {
    int32_t m_enabled;
    // This flag cleared (by MyNetwork) on limbs that exist in the network but not in the rig. This
    // is just so they can be discarded from updates etc.
    bool m_enabledByOwner;
  };
  ModuleEnableState* m_moduleEnableStates;
};
}
#endif // NM_MODULE_H
