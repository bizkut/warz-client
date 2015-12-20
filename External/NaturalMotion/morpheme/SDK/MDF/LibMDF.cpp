// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------

#include "MDFPrecompiled.h"
#include "MDFModuleAnalysis.h"
#include "MDFTypesAnalysis.h"
#include "MDFBehaviourAnalysis.h"
#include "ParserEngine.h"
#include "StringBuilder.h"
#include "MappedIO.h"
#include "Utils.h"
#include "LibMDF.h"
#include "ParserMemory.h"

#include <Shlwapi.h>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
MDFFeedbackInterface::~MDFFeedbackInterface()
{
}

//----------------------------------------------------------------------------------------------------------------------
void MDFFeedbackInterface::logMessage(MDFFeedbackInterface::MessageType mt, const char* msg, ...)
{
  #define OUTPUT_BUFFER_SIZE    (1024)
  static char gOutputBuffer[OUTPUT_BUFFER_SIZE];

  va_list args;
  va_start(args, msg);
  NMP_VSPRINTF(gOutputBuffer, OUTPUT_BUFFER_SIZE-1, msg, args);
  va_end(args);

  onLogMessage(mt, gOutputBuffer);
}


//----------------------------------------------------------------------------------------------------------------------
/**
 * private network state structure, one per MDFNetworkBuilder instance
 */
struct MDFNetworkState
{
  friend class MDFNetworkBuilder;

  enum Configuration
  {
    DefaultContainerSize  = 64            ///< initial size for storage of modules, structures, etc
  };

  MDFNetworkState() : 
    m_networkDB(0), 
    m_phase(lpInitial),
    m_moduleAnalysis(0)
  {
    parserMemoryAllocDirectArgs(NetworkDatabase, m_networkDB, DefaultContainerSize);
    parserMemoryAllocDirectArgs(ModuleAnalysis, m_moduleAnalysis, &m_networkDB->m_keywords, &m_networkDB->m_typesRegistry);
  }

  // the main databases built during processing
  NetworkDatabase            *m_networkDB;

  MDFFeedbackInterface       *m_feedbackInterface;

  
  // phase controls what order the API functions can be called in
  // eg. we can't be loading new types once MDFs have begun to be loaded
  enum LoaderPhase
  {
    lpInitial,
    lpTypesLoaded,
    lpAddingModules,
    lpFinalizedModules,
    lpAddingBehaviours,
    lpSealed
  }                           m_phase;

private:
  // instance of the analysis toolset
  ModuleAnalysis             *m_moduleAnalysis;

  MDFNetworkState(const MDFNetworkState&);
};

//----------------------------------------------------------------------------------------------------------------------
MDFNetworkBuilder::MDFNetworkBuilder() : 
  state(0)
{
  const unsigned int memoryInit_defaultHeapSize = 1024 * 1024 * 4;    // primary heap size
  const unsigned int memoryInit_subHeapSize     = 1024 * 1024 * 4;    // once primary is drained, new heaps will be this big
  mmgr = new ParserMemory(memoryInit_defaultHeapSize, memoryInit_subHeapSize);
}

//----------------------------------------------------------------------------------------------------------------------
MDFNetworkBuilder::~MDFNetworkBuilder()
{
  term();

  delete mmgr;
}

//----------------------------------------------------------------------------------------------------------------------
void MDFNetworkBuilder::init(MDFFeedbackInterface* fbInterface, unsigned int networkUID)
{
  assert(mmgr);
  ActiveMemoryManagerAutoScoped mmgrScope(mmgr);

  state = new MDFNetworkState();
  state->m_networkDB->m_moduleNUID = networkUID;
  state->m_feedbackInterface = fbInterface;
}

//----------------------------------------------------------------------------------------------------------------------
void MDFNetworkBuilder::term()
{
  assert(mmgr);
  ActiveMemoryManagerAutoScoped mmgrScope(mmgr);

  delete state;
  state = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDFNetworkBuilder::addNetworkConstant(const char* cName, int cValue)
{
  if (state == 0)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "init() must be called before working with the MDF network.");
    return false;
  }

  assert(mmgr);
  ActiveMemoryManagerAutoScoped mmgrScope(mmgr);

  if (state->m_phase != MDFNetworkState::lpInitial)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "Constants must be added before any other files are loaded.");
    return false;
  }

  bool inserted = state->m_networkDB->m_constants.find(cName);
  if (inserted)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::Warning, "Duplicate network constant value ('%s') ignored.", cName);
  }
  else
  {
    state->m_networkDB->m_constants.insert(cName, cValue);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDFNetworkBuilder::addTypes(const char* typedefFileName, void* clientUserData)
{
  if (state == 0)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "init() must be called before working with the MDF network.");
    return false;
  }

  assert(mmgr);
  ActiveMemoryManagerAutoScoped mmgrScope(mmgr);

  if (state->m_phase != MDFNetworkState::lpTypesLoaded &&
      state->m_phase != MDFNetworkState::lpInitial)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "Types must be loaded before any MDF data.");
    return false;
  }
  state->m_phase = MDFNetworkState::lpTypesLoaded;

  MDFTypes::Context context;
  context.m_keywords        = &state->m_networkDB->m_keywords;
  context.m_typesRegistry   = &state->m_networkDB->m_typesRegistry;
  context.m_constants       = &state->m_networkDB->m_constants;
  context.m_structMap       = &state->m_networkDB->m_structMap;
  context.m_commentMap      = &state->m_networkDB->m_commentMap;
  context.m_clientUserData  = clientUserData;

  MDFTypes::Parser parseMDFTypes(state->m_feedbackInterface, 0);

  // load 'em
  ParseResult result = parseMDFTypes.runFromFile(typedefFileName, &context);

  return (result == prSuccess);
}

//----------------------------------------------------------------------------------------------------------------------
bool MDFNetworkBuilder::postProcessTypes()
{
  if (state == 0)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "init() must be called before working with the MDF network.");
    return false;
  }

  assert(mmgr);
  ActiveMemoryManagerAutoScoped mmgrScope(mmgr);

  if (state->m_phase != MDFNetworkState::lpTypesLoaded)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "No types were loaded, cannot finalize type registry.");
    return false;
  }

  try
  {
    state->m_feedbackInterface->onStageBegin(MDFFeedbackInterface::Validation);
    {
      validateTypes(
        state->m_networkDB->m_typesRegistry,
        state->m_networkDB->m_structMap);
    }
    state->m_feedbackInterface->onStageComplete(MDFFeedbackInterface::Validation);
  }
  catch (const ValidationException* e)
  {
    state->m_feedbackInterface->onProcessingFailure(
      MDFFeedbackInterface::Validation,
      e->m_fileResc->m_pathToDef.c_str(),
      e->m_atLine - 1,
      e->m_msg.c_str(),
      e->m_ref.c_str(),
      e->m_fileResc->m_clientUserData);

    return false;
  }

  state->m_feedbackInterface->logMessage(MDFFeedbackInterface::Info, "Loaded %u types.", 
    state->m_networkDB->m_typesRegistry.getNumberRegisteredTypes());

  return true;
};

//----------------------------------------------------------------------------------------------------------------------
bool MDFNetworkBuilder::addMDF(const char* fname, void* clientUserData)
{
  if (state == 0)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "init() must be called before working with the MDF network.");
    return false;
  }

  assert(mmgr);
  ActiveMemoryManagerAutoScoped mmgrScope(mmgr);

  if (state->m_phase == MDFNetworkState::lpInitial)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "No types loaded, cannot read MDF data.");
    return false;
  }
  if (state->m_phase == MDFNetworkState::lpFinalizedModules)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "Network already validated, cannot add new MDFs.");
    return false;
  }
  state->m_phase = MDFNetworkState::lpAddingModules;


  parserMemoryAlloc(ModuleDef, defToFill);
  state->m_networkDB->m_moduleDefs.push_back(defToFill);

  defToFill->m_pathToDef.set(fname);
  defToFill->m_clientUserData = clientUserData;

  MDFModule::Context context;
  context.m_keywords        = &state->m_networkDB->m_keywords;
  context.m_typesRegistry   = &state->m_networkDB->m_typesRegistry;
  context.m_constants       = &state->m_networkDB->m_constants;
  context.m_def             = defToFill;
  context.m_clientUserData  = clientUserData;

  MDFModule::Parser parseMDF(state->m_feedbackInterface, 0);
  ParseResult result = parseMDF.runFromFile(fname, &context);

  // store file data from context
  if (result != prEmptyFile)
  {
    defToFill->m_filetime.dwLowDateTime = context.m_lowDateTime;
    defToFill->m_filetime.dwHighDateTime = context.m_highDateTime;
  }

  if (result == prSuccess)
  {
    defToFill->m_NUID = nmtl::hfn::hashString(defToFill->m_name.c_str());

    try
    {
      // post-process & validate VDs
      state->m_moduleAnalysis->validateModuleVarDecls(*defToFill);
    }
    catch (const ValidationException* e)
    {
      state->m_feedbackInterface->onProcessingFailure(
        MDFFeedbackInterface::Validation,
        e->m_fileResc->m_pathToDef.c_str(),
        e->m_atLine - 1,
        e->m_msg.c_str(),
        e->m_ref.c_str(),
        e->m_fileResc->m_clientUserData);

      return false;
    }

    return true;
  }
  
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDFNetworkBuilder::postProcessMDFs()
{
  if (state == 0)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "init() must be called before working with the MDF network.");
    return false;
  }

  assert(mmgr);
  ActiveMemoryManagerAutoScoped mmgrScope(mmgr);

  if (state->m_phase != MDFNetworkState::lpAddingModules)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "No MDFs loaded, cannot finalize network.");
    return false;
  }
  if (state->m_networkDB->m_moduleDefs.size() == 0)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::Error, "Network is empty.");
    return false;
  }

  state->m_feedbackInterface->logMessage(MDFFeedbackInterface::Info, "Loaded %u Modules.", 
    state->m_networkDB->m_moduleDefs.size());

  // validate contents, check for any obvious errors
  // any found will be printed by validateModuleDefs() itself
  try
  {
    state->m_feedbackInterface->onStageBegin(MDFFeedbackInterface::Validation);
    {
      state->m_moduleAnalysis->validateModuleDefs(state->m_networkDB->m_moduleDefs);
    }
    state->m_feedbackInterface->onStageComplete(MDFFeedbackInterface::Validation);
  }
  catch (const ValidationException* e)
  {
    state->m_feedbackInterface->onProcessingFailure(
      MDFFeedbackInterface::Validation,
      e->m_fileResc->m_pathToDef.c_str(),
      e->m_atLine - 1,
      e->m_msg.c_str(),
      e->m_ref.c_str(),
      e->m_fileResc->m_clientUserData);

    return false;
  }

  // post-process the data and form links between dependent modules
  try
  {
    state->m_feedbackInterface->onStageBegin(MDFFeedbackInterface::Connectivity);
    {
      state->m_moduleAnalysis->calculateConnectivity(state->m_networkDB->m_moduleDefs, state->m_networkDB->m_limbInstances, &state->m_networkDB->m_instance);
    }
    state->m_feedbackInterface->onStageComplete(MDFFeedbackInterface::Connectivity);
  }
  catch (const ConnectivityException* e)
  {
    state->m_feedbackInterface->onProcessingFailure(
      MDFFeedbackInterface::Connectivity,
      e->m_def->m_pathToDef.c_str(),
      e->m_atLine,
      e->m_msg.c_str(),
      e->m_ref.c_str(),
      e->m_def->m_clientUserData);

    return false;
  }

  state->m_feedbackInterface->logMessage(MDFFeedbackInterface::Info, "Parallelism Analysis...");

  state->m_moduleAnalysis->parallelismAnalysis(
    state->m_networkDB->m_instance);


  // report on un-used junctions
  iterate_begin_const_ptr(ModuleDef, state->m_networkDB->m_moduleDefs, def)
  {
    iterate_begin_const_ptr(ModuleDef::ConnectionSet, def->m_connectionSets, set)
    {
      iterate_begin_const_ptr(ModuleDef::Junction, set->m_junctions, jnc)
      {
        if (jnc->m_unused)
        {
          // get the set that this junction is used in
          const ModuleDef::ConnectionSet* cs = def->lookupConnectionSetBySku(jnc->m_connectionSku);
          assert(cs);

          // make sure the set is actually used - otherwise we probably don't care that the junction isn't in use either
          if (cs->m_instantiated)
          {
            state->m_feedbackInterface->onProcessingFailure(
              MDFFeedbackInterface::Validation,
              def->m_pathToDef.c_str(),
              jnc->m_lineDefined - 1,
              "Unused junction in connection set",
              jnc->m_name.c_str(),
              def->m_clientUserData);

            return false;
          }
        }
      }
      iterate_end
    }
    iterate_end
  }
  iterate_end

  state->m_phase = MDFNetworkState::lpFinalizedModules;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDFNetworkBuilder::addBehaviourDef(const char* fname, void* clientUserData)
{
  if (state == 0)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "init() must be called before working with the MDF network.");
    return false;
  }

  assert(mmgr);
  ActiveMemoryManagerAutoScoped mmgrScope(mmgr);

  if (state->m_phase != MDFNetworkState::lpFinalizedModules && 
      state->m_phase != MDFNetworkState::lpAddingBehaviours)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "Module network not loaded.");
    return false;
  }
  state->m_phase = MDFNetworkState::lpAddingBehaviours;

  parserMemoryAlloc(BehaviourDef, defToFill);
  state->m_networkDB->m_behaviourDefs.push_back(defToFill);

  defToFill->m_pathToDef.set(fname);
  defToFill->m_clientUserData = clientUserData;

  MDFBehaviour::Context context;
  context.m_keywords        = &state->m_networkDB->m_keywords;
  context.m_typesRegistry   = &state->m_networkDB->m_typesRegistry;
  context.m_constants       = &state->m_networkDB->m_constants;
  context.m_def             = defToFill;
  context.m_clientUserData  = clientUserData;

  MDFBehaviour::Parser parseMDF(state->m_feedbackInterface, 0);
  ParseResult result = parseMDF.runFromFile(fname, &context);

  // store file data from context
  if (result != prEmptyFile)
  {
    defToFill->m_filetime.dwLowDateTime = context.m_lowDateTime;
    defToFill->m_filetime.dwHighDateTime = context.m_highDateTime;
  }

  if (result == prSuccess)
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDFNetworkBuilder::postProcessBehaviourDefs()
{
  if (state == 0)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "init() must be called before working with the MDF network.");
    return false;
  }

  assert(mmgr);
  ActiveMemoryManagerAutoScoped mmgrScope(mmgr);

  if (state->m_phase != MDFNetworkState::lpAddingBehaviours || 
      state->m_networkDB->m_behaviourDefs.size() == 0)
  {
    // not a failure, just no behaviours and nothing to do
    state->m_phase = MDFNetworkState::lpSealed;
    return true;
  }

  // validate, check for any obvious errors
  try
  {
    state->m_feedbackInterface->onStageBegin(MDFFeedbackInterface::Validation);
    {
      validateBehaviours(
        *state->m_networkDB,
        state->m_networkDB->m_behaviourDefs);
    }
    state->m_feedbackInterface->onStageComplete(MDFFeedbackInterface::Validation);
  }
  catch (const ValidationException* e)
  {
    state->m_feedbackInterface->onProcessingFailure(
      MDFFeedbackInterface::Connectivity,
      e->m_fileResc->m_pathToDef.c_str(),
      e->m_atLine - 1,
      e->m_msg.c_str(),
      e->m_ref.c_str(),
      e->m_fileResc->m_clientUserData);

    return false;
  }

  state->m_feedbackInterface->logMessage(MDFFeedbackInterface::Info, "Loaded %u behaviours.", 
    state->m_networkDB->m_behaviourDefs.size());

  state->m_phase = MDFNetworkState::lpSealed;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
struct PreproData
{
  PreproData(StringBuilder& sb, bool fr, bool vm, MDFFeedbackInterface* fbi) :
    pathBase(sb), 
    forceRegeneration(fr), 
    verboseMode(vm),
    feedbackInterface(fbi)
  {}

  StringBuilder&  pathBase;
  bool            forceRegeneration;
  bool            verboseMode;

  MDFFeedbackInterface  *feedbackInterface;

private:
  PreproData();
  PreproData(const PreproData& rhs);
  PreproData& operator = (const PreproData& rhs);
};


//----------------------------------------------------------------------------------------------------------------------
void preprocessFileResource(PreproData& data, FileResource &fr)
{
  fr.createArtefacts();

  iterate_begin(CodeGenArtefact, fr.m_artefacts, atf)
  {
    data.pathBase.restore();
    data.pathBase.appendPString(atf.m_filepath);

    // store full path
    atf.m_fullPathCache.set(data.pathBase.getBufferPtr());
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
void preprocessType(Type* t, void* userdata)
{
  // only talk to the base types
  if (!t->m_struct)
  {
    PreproData* data = (PreproData*)userdata;

    t->createArtefacts();

    iterate_begin(CodeGenArtefact, t->m_artefacts, atf)
    {
      data->pathBase.restore();
      data->pathBase.appendPString(atf.m_filepath);

      // store full path
      atf.m_fullPathCache.set(data->pathBase.getBufferPtr());
    }
    iterate_end
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool MDFNetworkBuilder::preprocessForCodeGeneration(const char* outputDirectory, bool forceRegeneration, bool verboseMode)
{
  if (state == 0)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "init() must be called before working with the MDF network.");
    return false;
  }

  assert(mmgr);
  ActiveMemoryManagerAutoScoped mmgrScope(mmgr);

  if (state->m_phase != MDFNetworkState::lpSealed)
  {
    state->m_feedbackInterface->logMessage(MDFFeedbackInterface::APIError, "Module network not finalized.");
    return false;
  }

  {
    StringBuilder sb(64);
    sb.appendCharBuf(outputDirectory);
    sb.save();

    //----------------------------------------------------------------------------------------------
    // first phase; go through all components, get them to create artefact records;
    // then check those Artefacts to see if we need to do code generation

    PreproData ppd(sb, forceRegeneration, verboseMode, state->m_feedbackInterface);
    state->m_networkDB->m_typesRegistry.iterate(preprocessType, &ppd);

    StructMap::value_walker structs = state->m_networkDB->m_structMap.walker();
    while (structs.next())
    {
      Struct* s = structs();
      preprocessFileResource(ppd, *s);
    }

    iterate_begin_ptr(ModuleDef, state->m_networkDB->m_moduleDefs, def)
    {
      preprocessFileResource(ppd, *def);
    }
    iterate_end

    iterate_begin_ptr(BehaviourDef, state->m_networkDB->m_behaviourDefs, bdef)
    {
      preprocessFileResource(ppd, *bdef);
    }
    iterate_end
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDFNetworkBuilder::getNDBIsValid() const
{
  if (state != 0 && state->m_phase != 0)
  {
    return state->m_phase == MDFNetworkState::lpSealed;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const NetworkDatabase& MDFNetworkBuilder::getNDB() const
{
  assert(state->m_phase == MDFNetworkState::lpSealed);
  return *state->m_networkDB;
}

//----------------------------------------------------------------------------------------------------------------------
MDFSyntaxHighlightingInterface::~MDFSyntaxHighlightingInterface()
{
}


//----------------------------------------------------------------------------------------------------------------------
struct SyntaxHighlighterState
{
  enum 
  {
    memoryHeapSize = 1024 * 1024 * 1
  };

  SyntaxHighlighterState() :
    m_memoryManager(memoryHeapSize, memoryHeapSize),
    m_networkDB(0)
  {
    reset();
  }

  ~SyntaxHighlighterState()
  {
    if (m_networkDB)
      delete m_networkDB;
  }

  void reset()
  {
    // everything 'under' the NDB is allocated via the MDF active memory manager
    // so we just clear that to zero and rebuild the NDB to start again
    if (m_networkDB)
      delete m_networkDB;

    m_memoryManager.resetForReuse();

    {
      ActiveMemoryManagerAutoScoped mmgrScope(&m_memoryManager);
      m_networkDB = new NetworkDatabase(128);
    }
  }

  /**
   * given a grammar mode, this resets LibMDF, creates a new Parser and Context pair of the 
   * respective type and returns them ready to run.
   */
  void prepare(MDFSyntaxHighlighter::Grammar gr, ParserBase** pb, ContextBase** cb, MDFSyntaxHighlightingInterface* shi)
  {
    reset();

    ActiveMemoryManagerAutoScoped mmgrScope(&m_memoryManager);

    switch (gr)
    {
      case MDFSyntaxHighlighter::Types:
        {
          MDFTypes::Context *parseContext     = new MDFTypes::Context;
          MDFTypes::Parser *parserInst        = new MDFTypes::Parser(m_feedbackInterface, shi);

          parseContext->m_structMap           = &m_networkDB->m_structMap;
          
          *pb = parserInst;
          *cb = parseContext;
        }
        break;

      case MDFSyntaxHighlighter::Modules:
        {
          MDFModule::Context *parseContext    = new MDFModule::Context;
          MDFModule::Parser *parserInst       = new MDFModule::Parser(m_feedbackInterface, shi);

          parserMemoryAlloc(ModuleDef, defToFill);
          parseContext->m_def = defToFill;

          *pb = parserInst;
          *cb = parseContext;
        }
        break;

      case MDFSyntaxHighlighter::Behaviours:
        {
          MDFBehaviour::Context *parseContext = new MDFBehaviour::Context;
          MDFBehaviour::Parser *parserInst    = new MDFBehaviour::Parser(m_feedbackInterface, shi);

          parserMemoryAlloc(BehaviourDef, defToFill);
          parseContext->m_def = defToFill;

          *pb = parserInst;
          *cb = parseContext;
        }
        break;

      default:
        assert(0);
        break;
    }

    (*cb)->m_keywords        = &m_networkDB->m_keywords;
    (*cb)->m_typesRegistry   = &m_networkDB->m_typesRegistry;
    (*cb)->m_constants       = &m_networkDB->m_constants;
  }

  ParserMemory            m_memoryManager;
  NetworkDatabase        *m_networkDB;

  MDFFeedbackInterface   *m_feedbackInterface;

private:
  SyntaxHighlighterState(const SyntaxHighlighterState&);
};


//----------------------------------------------------------------------------------------------------------------------
MDFSyntaxHighlighter::MDFSyntaxHighlighter(MDFFeedbackInterface* fbInterface)
{
  state = new SyntaxHighlighterState();
  state->m_feedbackInterface = fbInterface;
}

//----------------------------------------------------------------------------------------------------------------------
MDFSyntaxHighlighter::~MDFSyntaxHighlighter()
{
  delete state;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDFSyntaxHighlighter::run(const char* filename, Grammar gr, MDFSyntaxHighlightingInterface* shi)
{
  ParseResult result;

  ParserBase* pb;
  ContextBase* cb;

  state->prepare(gr, &pb, &cb, shi);

  {
    ActiveMemoryManagerAutoScoped mmgrScope(&state->m_memoryManager);
    result = pb->runFromFile(filename, cb);
  }

  delete cb;
  delete pb;

  return (result == prSuccess);
}

//----------------------------------------------------------------------------------------------------------------------
bool MDFSyntaxHighlighter::run(const char* stream, size_t streamLength, Grammar gr, MDFSyntaxHighlightingInterface* shi)
{
  ParseResult result;

  ParserBase* pb;
  ContextBase* cb;

  state->prepare(gr, &pb, &cb, shi);

  {
    ActiveMemoryManagerAutoScoped mmgrScope(&state->m_memoryManager);
    result = pb->runFromMemory(stream, streamLength, cb);
  }

  delete cb;
  delete pb;

  return (result == prSuccess);
}

//----------------------------------------------------------------------------------------------------------------------
bool MDFSyntaxHighlighter::run(const wchar_t* stream, size_t streamLength, Grammar gr, MDFSyntaxHighlightingInterface* shi)
{
  ParseResult result;

  ParserBase* pb;
  ContextBase* cb;

  state->prepare(gr, &pb, &cb, shi);

  {
    ActiveMemoryManagerAutoScoped mmgrScope(&state->m_memoryManager);
    result = pb->runFromMemory(stream, streamLength, cb);
  }

  delete cb;
  delete pb;

  return (result == prSuccess);
}
