#pragma once
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

/**
 * this file contains the definitions for all the 'intermediate' data that
 * will be loaded during the MDF parse
 */

#include <float.h>      // for FLT_MAX, etc

#include "nmtl/hashmap_dense_dynamic.h"
#include "nmtl/unique_vector.h"
#include "FixedStack.h"
#include "PString.h"
#include "StringBuilder.h"
#include "ParserMemory.h"

#include "MDFKeywords.h"


//----------------------------------------------------------------------------------------------------------------------
/**
 * Defines a code generation output; each major component will generate a list of these that can be used during
 * generation dependency scanning and code generation
 */
struct CodeGenArtefact
{
  CodeGenArtefact(int type, const char* path) :
    m_type(type),
    m_filepath(path)
  {}

  PString                 m_filepath;         // eg. "Grab.h", or "Types/PoseData.h"
  PString                 m_fullPathCache;    // set during preprocessForCodeGeneration()
  int                     m_type;             // custom per component
};
typedef nmtl::vector<CodeGenArtefact> CodeGenArtefacts;

//----------------------------------------------------------------------------------------------------------------------
/**
 * Base class for MDF components that are parsed from files - eg. the module defs, behaviour defs, etc. 
 */
struct FileResource
{
  FileResource() : 
    m_artefacts(ActiveMemoryManager::getForNMTL()),
    m_clientUserData(0)
  {}

  PString                   m_pathToDef;        // full path to the original resource definition
  FILETIME                  m_filetime;         // last-write time, used during regeneration dependency checks
  CodeGenArtefacts          m_artefacts;
  void                     *m_clientUserData;

  virtual void createArtefacts() = 0;
};

//----------------------------------------------------------------------------------------------------------------------

struct ModuleDef;
struct Type;
struct Struct;
struct BehaviourDef;
struct LimbInstance;
struct ModuleInstance;
struct ModuleBinding;
struct BoundModuleDependency;

typedef nmtl::unique_vector<const char*>                  UniqueNames;

// iterator/container types for Types
typedef nmtl::pod_vector<const Type*>                     Types;
typedef nmtl::unique_vector<const Type*>                  UniqueTypes;

// containers for module defs
typedef nmtl::unique_vector<ModuleDef*>                   UniqueModuleDefPtrs;
typedef nmtl::unique_vector<BoundModuleDependency*>       ModuleDependencies;
typedef nmtl::pod_vector<ModuleDef*>                      ModuleDefs;
typedef nmtl::pod_vector<const ModuleDef*>                ConstModuleDefs;

// and behaviour defs
typedef nmtl::pod_vector<BehaviourDef*>                   BehaviourDefs;

// limbs
typedef nmtl::pod_vector<LimbInstance*>                   LimbInstances;

// module instances
typedef nmtl::pod_vector<ModuleInstance*>                 ModuleInstances;
typedef nmtl::unique_vector<ModuleInstance*>              UniqueModuleInstances;

// for storing types in nmtl hashmaps
typedef nmtl::hkey64 TypeKey64;
typedef nmtl::hashmap_dense_dynamic<TypeKey64, Struct*>   StructMap;

typedef nmtl::hashmap_dense_dynamic<const char*, int>     NetworkConstants;

enum NetworkExcutionPhase
{
  nepUpdate,
  nepFeedback
};

//----------------------------------------------------------------------------------------------------------------------
struct ModuleBinding
{
  ModuleBinding() : 
    m_ops(ActiveMemoryManager::getForNMTL())
  {
  }

  enum Op
  {
    GetOwner,
    LookupChildByName,
  };

  struct BindOp
  {
    BindOp() : m_arrayIndex(0) {}

    Op            m_op;
    PString       m_text;
    unsigned int  m_arrayIndex;
  };

  typedef nmtl::pod_vector<BindOp*> BindOps;

  ModuleBinding* deepCopy();

  // given a starting ModuleInstance, 'run' the binding ops to 
  // find the instance that it refers to
  ModuleInstance* findCorrespondingInstance(ModuleInstance* startInst) const;

  BindOps     m_ops;
};

//----------------------------------------------------------------------------------------------------------------------
struct BoundModuleDependency
{
  BoundModuleDependency() : m_def(0), m_binding(0) {}

  ModuleDef        *m_def;
  ModuleBinding    *m_binding;

  unsigned int genHash() const;
};

//----------------------------------------------------------------------------------------------------------------------
/**
 * temporary storage used when parsing and extracting variable declarations
 */
struct VarDeclConstruction
{
  unsigned int        m_lineDefined;
  PStringStack16      m_typespec;         // stack of declaration components
  int                 m_namespaceStart,   // first index of namespace component, or -1 for none
                      m_namespaceEnd;     // last
  bool                m_typesealed;       // toggled when the parser has hit the variable type component and
                                          // therefore expects no more type specifiers
  ModifierOptions     m_modifiers;


  inline void clear()
  {
    m_lineDefined = 0;
    m_typespec.reset();
    m_namespaceStart = 
      m_namespaceEnd = -1;
    m_typesealed = false;
    m_modifiers.stack.reset();
  }

  inline bool isEmpty()
  {
    return (m_typespec.count() == 0);
  }

  /**
   * write out a namespace string, given the start/end component indices above
   */
  void getNamespaceFromIndices(StringBuilder& out) const;

  /**
   * fill a stack with namespace components
   */
  void createNamespaceStack(PStringStack16& out) const;
};


//----------------------------------------------------------------------------------------------------------------------
struct ParsedTypeDependency
{
  ParsedTypeDependency(const VarDeclConstruction& vdc, bool ptrOrRef) : m_vdc(vdc), m_ptrOrRef(ptrOrRef) {}

  VarDeclConstruction m_vdc;
  bool                m_ptrOrRef;
};
typedef nmtl::vector<ParsedTypeDependency>  ParsedTypeDependencies;


//----------------------------------------------------------------------------------------------------------------------
struct TypeDependencies
{
  TypeDependencies() : 
    m_dependentTypesByPtrRef(8, ActiveMemoryManager::getForNMTL()),
    m_dependentTypes(8, ActiveMemoryManager::getForNMTL()),
    m_dependentBaseTypes(8, ActiveMemoryManager::getForNMTL())
  {}

  UniqueTypes             m_dependentTypesByPtrRef;
  UniqueTypes             m_dependentTypes;
  UniqueTypes             m_dependentBaseTypes;

  void registerDependentType(const Type* t, bool ptrOrRef);
};


//----------------------------------------------------------------------------------------------------------------------
/**
 * base class for enumeration declarations; stores stack of enum entries, manages value index assignment
 */
struct EnumDeclBase
{
  EnumDeclBase() : 
    m_entries(ActiveMemoryManager::getForNMTL()),
    m_currentValue(0)
  {} 

  void add(const PString& name, unsigned int lineDefined);
  void add(const PString& name, int value, unsigned int lineDefined, const char* expr = 0);

  /**
   * an entry is a single item in an enumeration
   */
  struct Entry
  {
    Entry() : 
      m_value(-1), m_customValueUsed(false) {}

    Entry(const PString& name, int val, bool custom, unsigned int lineDefined, const char* expr = 0) :
      m_name(name), m_expression(expr), m_value(val), m_lineDefined(lineDefined), m_customValueUsed(custom)
    {}

    PString   m_name;                 // entry name, eg. "eNumLimb"
    PString   m_expression;
    PString   m_comment;              // optional comment documentation
    int       m_value;                // value index
    bool      m_customValueUsed;      // true if value index was set manually, eg. "eNumLimb = 6"
    unsigned int  m_lineDefined;      // which line in the file defined this variable
  };
  typedef nmtl::vector<Entry>  Entries;

  unsigned int        m_lineDefined;
  PString             m_name,
                      m_comment;
  Entries             m_entries;


  inline const EnumDeclBase::Entry* findNamedEnumEntry(const PString& name) const
  {
    for (EnumDeclBase::Entries::const_iterator ee_it = m_entries.begin(), ee_end = m_entries.end(); 
      ee_it != ee_end; 
      ++ee_it)
    {
      const EnumDeclBase::Entry& enumEntry = (*ee_it);

      if (enumEntry.m_name == name)
        return &enumEntry;
    }

    return 0;
  }

protected:

  int                 m_currentValue;   // index used when adding entries using normal append method (eg. without custom value)
};

//----------------------------------------------------------------------------------------------------------------------
// search a vector of enums for an entry with the given name
template <typename ENT>
inline const EnumDeclBase::Entry*  searchAllEnumEntries(const PString& name, const nmtl::vector<ENT>& evec)
{
  for (nmtl::vector<ENT>::const_iterator it = evec.begin(), end = evec.end(); 
        it != end; 
        ++it)
  {
    const EnumDeclBase& ed = (*it);
    const EnumDeclBase::Entry* res = ed.findNamedEnumEntry(name);

    if (res)
      return res;
  }

  return 0;
}


//----------------------------------------------------------------------------------------------------------------------
/**
 * 
 */
struct VarDeclBase
{
  inline VarDeclBase() { clear(); }
  virtual ~VarDeclBase() {}

  enum 
  { 
    vdMaxArraySize      = 64            // this can be anything, it's just to catch outrageous typos in the MDF during analysis
  };

  // - Parser --------------

  // fundamentals - {namespace} <typename> <variable name>, eg. "NMP::Vector3 myVector"
  PString               m_namespace,          // 'NMP'
                        m_typename,           // 'Vector3'
                        m_varname;            // 'myVector'
  
  char                  m_storageSpecifier;   // storage specifier, eg. '*' or '&'

  unsigned int          m_arraySize;          // if >0, vardecl refers to array type, eg. int myInts[3] :: m_arraySize == 3
  PString               m_arraySizeAsString;  // if not empty, contains identifier that produced the m_arraysize value, eg. an enum
                                              // name or network constant, eg. "NetworkConstants::rigMaxHeads"

  ModifierOptions       m_modifiers;          // list of attributes applied to this variable

  unsigned int          m_lineDefined;        // which line in the file defined this variable
  PString               m_comment;            // optional comment documentation

  // - Analysis ------------

  PString               m_varnameCapFront;    // varname but with the first letter capitalized; cached, as it's used a lot during codegen
  const Type           *m_knownType;          // pointer to the Type for this decl; will be non-null after analysis


  inline void clear()
  {
    m_namespace.clear();
    m_typename.clear();
    m_varname.clear();

    m_storageSpecifier = 0;

    m_arraySize = 0;
    m_arraySizeAsString.clear();
    
    m_modifiers.stack.reset();
    m_lineDefined = 0;

    m_comment.clear();

    m_varnameCapFront.clear();
    m_knownType = 0;
  }

  bool isCompoundType() const;
  bool hasCombineFunction() const;

  // fill 'out' with a typical variable declaration string, sans terminating semicolon
  // eg: "NMP::Vector3 varName[15]"
  // if justType is true, output halts before name/arrayspec, eg: "NMP::Vector3"
  // if bypassAlias is true, and the type used by this vardecl is aliased, then the type pointed to
  //    by the alias will be used for namespace/typename; this should be true for code-gen, when we 
  //    want alias names to boil down into their underlying types. for everything else it can be false.
  void toString(StringBuilder& out, bool justType = false, bool bypassAlias = false) const;

  // return size in bytes of this vardecl
  unsigned int getSize() const; 
};

//----------------------------------------------------------------------------------------------------------------------
/**
 * use with std::sort to rearrange a vector of VarDecls by type size to help reduce
 * padding in a given def
 */
struct VarDeclTypeSizeSorter
{
  bool operator() (VarDeclBase& elem1, VarDeclBase& elem2);
};

//----------------------------------------------------------------------------------------------------------------------
struct FnDeclBase
{
  inline FnDeclBase() { clear(); }
  virtual ~FnDeclBase() {}

  PString  m_fn;  // function code

  inline void clear()
  {
    m_fn.clear();
  }
};


// ---------------------------------------------------------------------------------------------------------------------
// structure containing the intermediate data for a module def
// ready to be sent to the code-generator, etc
struct ModuleDef : public FileResource
{
  struct Junction;
  typedef nmtl::pod_vector<Junction*>     Junctions;
  typedef nmtl::unique_vector<Junction*>  UniqueJunctions;

  ModuleDef();

  // -------------------------------------------------------------------------------------------------------------------
  // build artefacts generated by an instance of a ModuleDef
  enum Artefacts
  {
    ModuleHeader,
    ModuleCode,
    ModuleData,
    ModulePackaging,
    ModulePackagingCode
  };


  // - Parser -----------------------------------------------

  enum
  {
    InvalidNUID = 0xFFFF    // default (and invalid) value for module network UID
  };

  PString                   m_name;
  PString                   m_shortDesc;        // optional comment documentation
  PString                   m_moduleOwner,
                            m_moduleOwnerSku;
  unsigned int              m_NUID,
                            m_lineDefined,
                            m_limbIndex;        // tracking index for limb instances that use this module
  ModifierOptions           m_modifiers;

  ParsedTypeDependencies    m_parsedTypeDependencies;


  // - Connectivity -----------------------------------------

  ModuleDef                *m_moduleOwnerPtr;
  PString                   m_moduleOwnerClassName;
  TypeDependencies          m_typeDependencies;


  // full list of all modules that have to have been updated before this module gets updated
  ModuleDependencies        m_updateExecutionDependencies,
                            m_feedbackExecutionDependencies;

  // the above lists, but with dependencies that will be satisfied by parent/child 
  // hierarchy removed. eg. if a module depends on 'BodyFrame' but it's parent does too,
  // the child need not 'depend' on it as the fact the parent will be updated first will
  // produce the same result
  ModuleDependencies        m_reducedUpdateDependencies,
                            m_reducedFeedbackDependencies;


  // -------------------------------------------------------------------------------------------------------------------
  // get the size of the data for this module; does not
  // include any extra requirements from the module class, just the 
  // packaging payload
  unsigned int getSize() const; 

  // -------------------------------------------------------------------------------------------------------------------
  bool isRootModule() const { return (m_moduleOwnerPtr == 0); }


  // -------------------------------------------------------------------------------------------------------------------
  struct Child
  {
    Child() : 
      m_lineDefined(0), 
      m_arraySize(0), 
      m_modulePtr(0),
      m_moduleInstances(ActiveMemoryManager::getForNMTL())
    {}

    // - Parser --------------

    unsigned int            m_lineDefined;      // which line in the file defined this child
    unsigned int            m_arraySize;        // if >0, this refers to array of children of this type
    PString                 m_module;           // the module type in question
    PString                 m_sku;              // optional sku of module
    PString                 m_name;             // name of this child

    // - Connectivity --------

    ModuleDef              *m_modulePtr;        // pointer back to the module definition used
    ModuleInstances         m_moduleInstances;  // just one, or m_arraySize if this is an array of modules
  };

  // note: yeah, English grammar failure here, the '<type>s' format is used
  //        by some iterator macros elsewhere, so...
  typedef nmtl::vector<Child>                         Childs;
  typedef nmtl::pod_vector<const ModuleDef::Child*>   ConstChildPtrs;

  Childs                    m_children;


  // -------------------------------------------------------------------------------------------------------------------
  // look up a child with linear name-match search
  const Child* getChildByName(const PString &name) const;


  // -------------------------------------------------------------------------------------------------------------------
  struct VarDecl : public VarDeclBase
  {
    VarDecl() : VarDeclBase(), 
      m_modulePtr(0), 
      m_ownerBlock(msInvalid),
      m_connectedJunctions(4, ActiveMemoryManager::getForNMTL())
    { 
    }

    // - Parser --------------

    ModuleDef          *m_modulePtr;          // has ptr to module that houses the target vardecl/junction
    ModuleSection       m_ownerBlock;

    // - Connectivity --------

    UniqueJunctions     m_connectedJunctions;

    // -----------------------------------------------------------------------------------------------------------------
    // structure for referring to a VarDecl, complete with optional array access
    struct Target
    {
      Target() : 
        m_varDeclPtr(0), 
        m_arrayElementAccess(0)
      {}

      VarDecl    *m_varDeclPtr;           // variable to connect
      void       *m_arrayElementAccess;   // optional; if not-null, this refers to array element of m_varDeclPtr
                                          // with the index determined by the type of Component referenced 
                                          // (eg. will be a ArrayAccess or ArrayAccessByLink component instance)
    };

    // -----------------------------------------------------------------------------------------------------------------
    // test to see if lhs is type-compatible with rhs (eg. so connection could be made)
    // if fails, returns string literal describing failing test
    static const char* varDeclTargetsConnectable(const Target& lhsTarget, const Target& rhsTarget);
  };

  // -------------------------------------------------------------------------------------------------------------------
  struct EnumDecl : public EnumDeclBase
  {
    EnumDecl() : EnumDeclBase(),
      m_modulePtr(0) {} 

    // - Parser --------------

    ModuleDef        *m_modulePtr;
  };

  //--------------------------------------------------------------------------------------------------------------------
  struct FnDecl : public FnDeclBase
  {
    FnDecl() : FnDeclBase()
    {
      m_lineDefined = 0;
      m_shortDesc.clear();
    }

    unsigned int          m_lineDefined;        // which line in the file defined this function
    PString               m_shortDesc;          // function short description
  };

  // --------------------------------------------------------
  typedef nmtl::vector<VarDecl>      VarDecls;
  typedef nmtl::vector<EnumDecl>     EnumDecls;
  typedef nmtl::vector<FnDecl>       FnDecls;

  VarDecls            m_vardecl[msNumVarDeclBlocks];
  VarDecls            m_private;                    // special group for ptr/ref vars on PPU-side modules

  EnumDecls           m_enumDecl;

  FnDecls             m_privateAPI,
                      m_publicAPI;

  // - Analysis ------------
  bool                m_varDeclBlockHasJunctionAssignments[msNumVarDeclBlocks];


  // quick helpers used when deciding whether to generate data packets for vardecl blocks
  inline bool varDeclBlockIsEmpty(ModuleSection ms) const { return (m_vardecl[ms].size() == 0); }
  inline bool varDeclBlockIsNotEmpty(ModuleSection ms) const { return (m_vardecl[ms].size() > 0); }

  // returns true if the given module section has any junction assignments (and therefore should
  // have a combine() function generated in the module code)
  inline bool varDeclBlockHasJunctionAssignments(ModuleSection ms) const { return m_varDeclBlockHasJunctionAssignments[ms]; }


  // -----------------------------------------------------------------------------------------------------------------
  struct ConnectionPathTarget
  {
    BoundModuleDependency         m_moduleTarget; // contains the pointer to the targeted module, plus dependency binding info
    
    ModuleDef::VarDecl::Target    m_varDecl;
    ModuleDef::Junction          *m_junction;    // if null, this is a var decl target.. and vice versa
    int                           m_expandToMultiple;

    void clear()
    {
      m_moduleTarget.m_def = 0;
      m_moduleTarget.m_binding = 0;
      m_varDecl.m_arrayElementAccess = 0;
      m_varDecl.m_varDeclPtr = 0;
      m_junction = 0;
      m_expandToMultiple = -1;
    }
  };

  // -------------------------------------------------------------------------------------------------------------------
  struct Connection
  {
    // -----------------------------------------------------------------------------------------------------------------
    // A ConnectionPath is a collection of tokens that define a target of some kind:
    // 'owner.out.spineTwist'
    // each part of the path is made up Components (eg. 'spineTwist'); these are analysed post-parse
    // to enforce syntactic validity of ConnectionPaths - eg. a ccModuleName Component can only follow
    // a ccOwner Component. There are specialized forms of Components for accessing data in more exotic ways, 
    // such as items in an array, by the special child-index value, etc.
    struct Path
    {
      // ---------------------------------------------------------------------------------------------------------------
      struct Component
      {
        enum Type
        {
          cBasic,
          cArrayAccess,             // [1], [numLegs]
          cArrayAccessByLink,       // [owner.owner.$ChildIndex]
          cInstanceVariable,        // $ChildIndex
          cExpandArray,             // [*]
          cIterateArray,            // []
        };

        Component(const char* text, Type t = cBasic);
        virtual ~Component() {}

        inline bool isType(Type t) const { return m_type == t; }

        // add appropriate array access formatting for this component;
        // returns false and does not modify StringBuilder if this is not
        // a component that specifies array access semantics.
        bool formatArrayAccess(
          StringBuilder& strOut,
          bool addArrayBraces = false,
          unsigned int arrayIndex = 0) const;

        // clone this component
        virtual Component* deepCopy() const;

        PString           m_text;
        Type              m_type;
        ComponentContext  m_context;
        const void       *m_contextData;

      private:
        Component& operator = (const Component& rhs);
      };
      typedef nmtl::pod_vector<Component*> Components;

      // ---------------------------------------------------------------------------------------------------------------
      // ArrayAccess - accessing a variable/module array by specific index, eg: out.spineTwist[4], out.spineTwist[numLegs]
      struct ComponentArrayAccess : public Component
      {
        ComponentArrayAccess(const char* text);
        virtual Component* deepCopy() const;

        unsigned int  m_arrayIndex;
      };

      // ---------------------------------------------------------------------------------------------------------------
      // ArrayAccessByLink - accessing an array with a targeted value; such as "owner.$ChildIndex"
      struct ComponentArrayAccessByLink : public Component
      {
        ComponentArrayAccessByLink(const char* text);
        virtual Component* deepCopy() const;

        // emit the array link path as a connected string, ready for dropping into codegen
        void formatLinkPath(StringBuilder& sb) const;

        ModuleDef::Connection::Path  *m_linkPath;
      };

      // -----------------------------------------------------------------------------------------------------------------
      // InstanceVariable - special component found only in ArrayAccessByLink chains, eg. owner.owner.in.spineTwist[ owner.$ChildIndex ]
      struct ComponentInstanceVariable : public Component
      {
        ComponentInstanceVariable(const char* text);
      };

      // -----------------------------------------------------------------------------------------------------------------
      // Expand - invoked via [*] syntax, explodes array members into junction sources
      struct ComponentExpandArray : public Component
      {
        ComponentExpandArray(const char* text);
      };

      // -----------------------------------------------------------------------------------------------------------------
      // Iterate - manual array unrolling via [] syntax
      struct ComponentIterateArray : public Component
      {
        ComponentIterateArray(const char* text);
      };


      // ---------------------------------------------------------------------------------------------------------------
      Path();

      /**
       * function that clones the path and all components, returning the pointer to the new instance
       */
      Connection::Path* deepCopy() const;

      // ---------------------------------------------------------------------------------------------------------------
      // calculate the value of m_componentsHashKey from the stored path components
      void generateHashKey();

      // - Parser --------------

      Components              m_components;           // sequence of components that make up this path
      nmtl::hkey64            m_componentsHashKey;    // created at parse-time, a UID representing a combination of all path components
                                                    // used when checking for duplicate assignments, etc

      // - Connectivity --------

      ConnectionPathTarget    m_target;
      bool                    m_localDataOnly;


      // dump the path contents to a string, optionally wrapping a given index with 
      // some kind of highlight (for sending errors to the console)
      void toString(StringBuilder& out, unsigned int highlightIndex = -1) const;
    };
    typedef nmtl::pod_vector<Connection::Path*> Paths;


    // -----------------------------------------------------------------------------------------------------------------
    Connection(unsigned int line, ConnectionTask task);

    // - Parser --------------

    unsigned int        m_lineDefined;            // which line in the file defined this connection
    ConnectionTask      m_task;                   // what is this connection doing? creating a new junction?
    Connection::Path   *m_target;                 // assignment target for this connection, eg. "junctions.data"
    ModifierOptions     m_modifiers;

    // if task == 'New Junction'
    JunctionMode        m_newJunctionMode;        // optional, validity based on m_task value

    // if task != 'New Junction', this should be a single path (a path pointing to an existing Junction)
    // otherwise it defines all of a new junction's inputs
    Paths               m_paths;

    // - Connectivity --------

    bool                m_wasInheritedViaOverride;  // a tag to ensure that any __override__ defs are actually used

    // -----------------------------------------------------------------------------------------------------------------
    // for sorting the connection def list, ensures 'create junction' tasks come before 'assign junction' ones
    static bool SortByTask(const Connection* lhs, const Connection* rhs);
  };
  typedef nmtl::pod_vector<Connection*> Connections;

  struct ConnectionSet;

  // -------------------------------------------------------------------------------------------------------------------
  struct Junction
  {
    // -----------------------------------------------------------------------------------------------------------------
    struct ConnectionSource
    {
      ConnectionSource(BoundModuleDependency& bmd);

      ModuleDef::Connection::Path  *m_sourcePath;     // path that defines the source target
      BoundModuleDependency         m_moduleTarget;   // which module the decl/junction below belongs to
      
      Junction                     *m_junction;       // if null, this is a var decl target.. and vice versa
      VarDecl::Target               m_varDecl;

      inline bool connectsToVarDecl() const { return (m_junction == 0); }
    };
    typedef nmtl::vector<ConnectionSource> ConnectionSources;

    // -----------------------------------------------------------------------------------------------------------------
    Junction(unsigned int lineDefined, JunctionMode mode, const PString& name, const PString& sku);

    // - Parser --------------
    
    ModuleDef                  *m_ownerModule;        // module that owns this junction
    ModuleDef::ConnectionSet   *m_ownerSet;           // which connection set the junction belongs to

    unsigned int                m_lineDefined;
    JunctionMode                m_mode;
    PString                     m_name;
    PString                     m_connectionSku;      // which sku created this junction (default sku if empty)

    // - Connectivity --------

    ConnectionSources           m_connectionSources;
    ModuleDependencies          m_moduleReferences;

    bool                        m_unused;             // toggled when encountered in an Assign Junction task
                                                      // .. then checked to ensure we don't have junctions created and not used

    // - Validation ----------

    ModuleDef::VarDecl::Target  m_representativeVarDecl;    // a vardecl target for use when checking for connectable types
    int                         m_expandToMultiple;

    // check if the given junction mode will accept unblendable types -
    // that is, types/structs not marked with __blendable__ attr
    static bool supportsUnblendableTypes(JunctionMode jm);

  private:
    Junction();
  };


  // -------------------------------------------------------------------------------------------------------------------
  struct ConnectionSet
  {
    ConnectionSet();

    // - Parser --------------

    PString               m_name;                 // sku name, or empty for default

    Connections           m_connectionDefs;
    ModifierOptions       m_modifiers;

    // - Validation ----------

    struct NewJunctionTask
    {
      inline NewJunctionTask(Junction* j) : 
        m_junction(j) 
      {}

      Junction           *m_junction;
    };

    struct AssignJunctionTask
    {
      inline AssignJunctionTask(Junction* j) : 
        m_junction(j) 
      {
        m_junction->m_unused = false; // mark the junction as in-use
      }

      Junction           *m_junction;             // junction that will be assigned
      VarDecl::Target     m_targetVarDecl;        // where it's being plugged into
      bool                m_localDataOnly;        // true if the assignment is to a vardecl in the current module (eg. no "owner" usage)
    };

    typedef nmtl::vector<NewJunctionTask>      NewJunctionTasks;
    typedef nmtl::vector<AssignJunctionTask>   AssignJunctionTasks;

    Junctions             m_junctions;
    ModuleDependencies    m_moduleReferences;     // from walking connection paths

    // the various things that this connection set will do
    NewJunctionTasks      m_newJunctionTasks;
    AssignJunctionTasks   m_assignJunctionTask;

    bool                  m_instantiated;         // true if this set is used in the network

    // look up a junction with linear name-match search
    Junction* lookupJunctionByName(const PString &name);
  };

  typedef nmtl::vector<ConnectionSet*>   ConnectionSets;
  ConnectionSets    m_connectionSets;
  ConnectionSet    *m_defaultSet;       // may be null

  // look up a set with linear name-match search
  const ConnectionSet* lookupConnectionSetBySku(const PString &sku) const;

  virtual void createArtefacts();

private:
  ModuleDef(const ModuleDef& rhs);
  ModuleDef& operator=(const ModuleDef& rhs);
};


//----------------------------------------------------------------------------------------------------------------------
struct Type : public FileResource
{
  // create a type with the given namespace, name and size hint
  // eg. "NMP", "Vector3", 16
  Type(const char* nspace, const char* name, unsigned int sizeInBytes);

  // create a copy of the given type
  Type(const Type& rhs);


  PString           m_namespace,      // namespace, eg. "Foo::Bar"; will include name of struct if embedded type (eg. enum)
                    m_name;           // type name, eg. "Vector3"
  PStringStack16    m_namespaceStack; // stack storage of namespace components, easier access than parsing m_namespace

  unsigned int      m_sizeInBytes,
                    m_alignment;
  ModifierOptions   m_modifiers;
  bool              m_private;        // if false, it's a public type; private is only for locally defined stuff (enums) in structs


  const Struct     *m_struct;         // will be pointer to struct instance if this type is a complex struct type
  const Type       *m_aliasType;      // if this is a type alias, this will point back to the original Type*


  inline bool isAliasType() const { return (m_aliasType != 0); }

  bool isExternalType() const;      // external_class/struct
  bool isCompoundType() const;      // struct or external_class/struct
  bool hasCombineFunction() const;
  bool isBlendableType() const;     // check if type has __blendable__; if it's a struct, check all enclosed types

  // append an appropriately formatted filename string (minus prefix or extension)
  // eg. <namespace1>_<namespace2>_<typename>
  void appendTypeFilename(StringBuilder& sb) const;

  // concatenate the struct name to the struct namespace, giving a namespace path
  // for embedded types (eg. enums in structs)
  const char* getFullNamespace(StringBuilder& sb) const;

  // return a 'flattened' type name for referring to type in function names, macros, etc
  // eg: NMP::Vector3 => NMP_Vector3
  const char* getFlattenedTypename(StringBuilder& sb) const;

  virtual void createArtefacts();

private:
  Type();
  Type& operator=(const Type& rhs);
};


//----------------------------------------------------------------------------------------------------------------------
struct Struct : public FileResource
{
  Struct();

  enum AccessibilityBlock
  {
    abPublic,
    abPrivate,
    abNumAccessabilityBlocks
  };

  enum Artefacts
  {
    StructHeader,
  };

  //--------------------------------------------------------------------------------------------------------------------
  struct EnumDecl : public EnumDeclBase
  {
    EnumDecl() : EnumDeclBase(),
      m_ownerStruct(0), m_accessibility(abPublic) {}

    // - Parser --------------

    Struct               *m_ownerStruct;
    AccessibilityBlock    m_accessibility;
  };

  //--------------------------------------------------------------------------------------------------------------------
  struct VarDecl : public VarDeclBase
  {
    VarDecl() : VarDeclBase(), 
      m_ownerStruct(0), m_accessability(abPublic) {}

    // - Parser --------------

    Struct               *m_ownerStruct;
    AccessibilityBlock    m_accessability;
  };

  //--------------------------------------------------------------------------------------------------------------------
  struct FnDecl : public FnDeclBase
  {
    FnDecl() : FnDeclBase()
    {
      m_lineDefined = 0;
      m_shortDesc.clear();
    }

    unsigned int          m_lineDefined;        // which line in the file defined this function
    PString               m_shortDesc;          // function short description
  };

  typedef nmtl::vector<VarDecl>     VarDecls;
  typedef nmtl::vector<FnDecl>      FnDecls;
  typedef nmtl::vector<EnumDecl>    EnumDecls;


  // - Parser --------------
  unsigned int            m_lineDefined;

  PString                 m_namespace,
                          m_name,
                          m_shortDesc;        // optional comment documentation
  PStringStack16          m_namespaceStack;

  PString                 m_inheritanceName,
                          m_inheritanceNamespace;

  ModifierOptions         m_modifiers;
  ParsedTypeDependencies  m_typeDependencies;

  VarDecls                m_vardecl[abNumAccessabilityBlocks];
  FnDecls                 m_functionDecl[abNumAccessabilityBlocks];
  EnumDecls               m_enumDecl[abNumAccessabilityBlocks];

  // - Validation ----------
  Type                   *m_type;
  TypeDependencies        m_dependencies;

  bool                    m_hasValidationFunction,
                          m_hasPostCombineFunction,
                          m_hasPreProcessorMacro;

  // - Connectivity --------
  const Type             *m_inheritancePtr;


  // search the enum decls for an entry with the given name
  const EnumDeclBase::Entry*  findEnumEntry(const PString& name) const;

  // concatenate the struct name to the struct namespace, giving a namespace path
  // for embedded types (eg. enums in structs)
  const char* getFullNamespace(StringBuilder& sb) const;

  // get the size of the struct contents, in bytes
  unsigned int getSize() const; 

  // check the types of all var decls to ensure that they can all be blended (type has __blendable__)
  bool allVarDeclsAreBlendable() const;

  virtual void createArtefacts();

private:
  Struct(const Struct& rhs);
  Struct& operator=(const Struct& rhs);
};


//----------------------------------------------------------------------------------------------------------------------
struct BehaviourDef : public FileResource
{
  BehaviourDef();

  enum Artefacts
  {
    BehaviourHeader,
    BehaviourCode
  };

  //--------------------------------------------------------------------------------------------------------------------
  struct BaseVariable
  {
    BaseVariable(BhDefVariableType vt, const PString& name, unsigned int arraySize);

    enum
    {
      kNumFlags
    };

    // - Parser --------------
    PString           m_name,
                      m_comment,
                      m_title;              // optional 'DisplayName' name for UI widgets, etc; leave blank to use m_name
    ModifierOptions   m_modifiers;
    unsigned int      m_lineDefined;
    unsigned int      m_ID;

    PString           m_arraySizeAsString;  // if not empty, contains identifier that produced the m_arraysize value, eg. an enum
                                            // name or network constant, eg. "NetworkConstants::rigMaxHeads"

    inline BhDefVariableType getType() const { return m_type; }
    inline bool isArray() const { return (m_arraySize != 0); }
    inline bool isNotArray() const { return (m_arraySize == 0); }
    inline bool isCpDefault() const { return strstr(m_name.c_str(), "_Default") != NULL; } // MORPH-21323: Optimize by doing test once
    inline unsigned int getArraySize() const { return m_arraySize; }

    inline unsigned int getNumFloats() const { return m_numFloats; }
    inline unsigned int getNumInts() const { return m_numInts; }
    inline unsigned int getNumUInt64s() const { return m_numUInt64s; }
    inline unsigned int getNumVector3s() const { return m_numVector3s; }

  private:
    // private as the numFloats/Ints is calculated during ctor, based on arraySize & type
    BhDefVariableType m_type;
    unsigned int      m_arraySize;          // if >0, baseVar refers to array type, eg. int myInts[3] :: m_arraySize == 3
    unsigned int      m_numFloats,          // number of 'floats' or 'ints' used to represent this variable;
                      m_numInts,            // this is used during codegen / lua manifest stuff
                      m_numUInt64s,
                      m_numVector3s;
  };

  //--------------------------------------------------------------------------------------------------------------------
  struct AttributeData
  {
    union
    {
      float   f;
      int     i;
      bool    b;
      float   v[3];
      void*   ptr;
    };
  };

  //--------------------------------------------------------------------------------------------------------------------
  struct AttributeVariable : public BaseVariable
  {
    AttributeVariable(BhDefVariableType vt, const PString& name, unsigned int arraySize);

    AttributeData   m_default,
                    m_min,
                    m_max;
    bool            m_hasDefault,
                    m_hasMin, 
                    m_hasMax;

    virtual bool isPerAnimSet() const { return hasBehaviourVariableModifier(m_modifiers, bvmPerAnimSet) != 0; }
  };
  typedef nmtl::vector<AttributeVariable>      AttributeVariables;

  //--------------------------------------------------------------------------------------------------------------------
  struct ControlParam : public AttributeVariable
  {
    ControlParam(BhDefVariableType vt, const PString& name, unsigned int arraySize);
  };
  typedef nmtl::vector<ControlParam>      ControlParams;

  //--------------------------------------------------------------------------------------------------------------------
  struct OutputControlParam : public BaseVariable
  {
    OutputControlParam(BhDefVariableType vt, const PString& name, unsigned int arraySize);
  };



  //--------------------------------------------------------------------------------------------------------------------
  struct EmittedMessage
  {
    EmittedMessage(const char* name) :
      m_name(name),
      m_lineDefined(0)
    {}

    PString           m_name;           // eg. "HasFallen"
    unsigned int      m_lineDefined;
    PString           m_comment;
  };

  //--------------------------------------------------------------------------------------------------------------------
  struct ModuleToEnable
  {
    ModuleToEnable() : 
      m_targetIsRoot(false),
      m_allChildren(false) 
    {}

    // each path item represents a component of the enabler path, eg;
    // "root.arm.point.*"
    struct PathItem
    {
      PathItem(const char* name) :
        m_name(name),
        m_arrayAccess(false),
        m_arrayIndex(-1),
        m_childInst(0),
        m_modulePrev(0)
      {}
      
      // - Parser --------------
      PString           m_name;           // eg. "arm"
      bool              m_arrayAccess;    // access an array of modules, or a specific element
      int               m_arrayIndex;     // -1 for 'enable all in array', otherwise an element index

      // - Analysis ------------
      const ModuleDef::Child   *m_childInst;
      const ModuleDef          *m_modulePrev;
    };
    typedef FixedStack<PathItem*, 16> PathItem16;

    // - Parser --------------
    PathItem16          m_path;

    // - Analysis ------------
    bool                m_targetIsRoot;
    bool                m_allChildren;
  };

  //--------------------------------------------------------------------------------------------------------------------
  struct AttributeGroup
  {
    AttributeGroup(const PString& name) : 
      m_name(name),
      m_enumDecl(ActiveMemoryManager::getForNMTL()),
      m_vars(ActiveMemoryManager::getForNMTL()),
      m_lineDefined(0)
    {}

    struct EnumDecl : public EnumDeclBase
    {
      EnumDecl() : EnumDeclBase(),
        m_ownerBehaviourDef(0) {}

      // - Parser --------------

      ModifierOptions     m_modifiers;

      BehaviourDef       *m_ownerBehaviourDef;
    };

    PString               m_name,
                          m_comment;
    unsigned int          m_lineDefined;
    ModifierOptions       m_modifiers;

    typedef nmtl::vector<EnumDecl>           EnumDecls;
    EnumDecls             m_enumDecl;

    AttributeVariables    m_vars;
  };

  AttributeGroup* findAttributeGroup(const PString& name) const;
  AttributeGroup* findOrCreateAttributeGroup(const PString& name);

  //--------------------------------------------------------------------------------------------------------------------
  struct ControlParamGroup
  {
    ControlParamGroup(const PString& name) : 
    m_name(name),
    m_params(ActiveMemoryManager::getForNMTL()),
    m_lineDefined(0)
  {}

  PString               m_name,
                        m_comment;
  unsigned int          m_lineDefined;

  ControlParams         m_params;
  };

  ControlParamGroup* findControlParamGroup(const PString& name) const;
  ControlParamGroup* findOrCreateControlParamGroup(const PString& name);

  //--------------------------------------------------------------------------------------------------------------------
  struct AnimationInput
  {

    AnimationInput(const PString& name) : 
      m_name(name), m_lineDefined(0), m_ID(0) {
        m_defaults[0] = m_defaults[1] = m_defaults[2] = m_defaults[3] = 1.0f;
      }

    PString           m_name,
                      m_comment;
    ModifierOptions   m_modifiers;
    unsigned int      m_lineDefined;
    unsigned int      m_ID;

    // Default values for the animation input (Arm, Head, Leg, Spine)
    float             m_defaults[4];
  };

  //--------------------------------------------------------------------------------------------------------------------
  typedef const ControlParam*                 ControlParamPtr;
  typedef const OutputControlParam*           OutputControlParamPtr;
  typedef const AttributeVariable*            AttributeVariablePtr;
  typedef nmtl::vector<AttributeGroup>        AttributeGroups;
  typedef nmtl::vector<ControlParamGroup>     ControlParamGroups;
  typedef nmtl::vector<ModuleToEnable>        ModulesToEnable;
  typedef nmtl::vector<OutputControlParam>    OutputControlParams;
  typedef nmtl::vector<EmittedMessage>        EmittedMessages;
  typedef nmtl::vector<AnimationInput>        AnimationInputs;
  typedef nmtl::vector<ControlParamPtr>       ControlParamPtrs;
  typedef nmtl::vector<OutputControlParamPtr> OutputControlParamPtrs;
  typedef nmtl::vector<AttributeVariablePtr>  AttributeVariablePtrs;


  // - Parser --------------
  unsigned int            m_lineDefined;
  PString                 m_name;
  PString                 m_shortDesc; // the short definition (a comment)

  ModifierOptions         m_modifiers;

  unsigned int            m_behaviourID;

  AttributeGroups         m_attributeGroups;
  ControlParamGroups      m_controlParamGroups;
  ModulesToEnable         m_modulesToEnable;
  OutputControlParams     m_outputControlParams;
  EmittedMessages         m_emittedMessages;
  AnimationInputs         m_animationInputs;

  PString                 m_version;
  PString                 m_displayName;
  PString                 m_group;
  PString                 m_include;
  PString                 m_pingroups;
  PString                 m_upgrade;
  PString                 m_defaultsDisplayFunc;
  PString                 m_displayFunc;
  bool                    m_development;

  // - Analysis ------------

  // lists of params and attributes, sorted by size to try and avoid padding in generated data wrapper
  ControlParamPtrs        m_sortedControlParams;
  OutputControlParamPtrs  m_sortedOutputControlParams;
  AttributeVariablePtrs   m_sortedAttributes;

  AttributeVariablePtrs   m_messageParamAttributes; // attributes ordered for message params structure.

  virtual void createArtefacts();

private:
  BehaviourDef(const BehaviourDef& rhs);
  BehaviourDef& operator=(const BehaviourDef& rhs);
};


//----------------------------------------------------------------------------------------------------------------------
// manager class that holds a registry of known types, handles registration, look-up and aliasing
//
struct TypesRegistryData;
class TypesRegistry
{
public:

  TypesRegistry();
  ~TypesRegistry();

  // function signature used when iterating over type registry
  typedef void (*ConstTypeIterator)(const Type* t, void* userdata);
  typedef void (*TypeIterator)(Type* t, void* userdata);

  // type Alias that provides a way to duplicate types with new names and overridden modifiers
  struct Alias
  {
    const Type*       m_type;
    ModifierOptions   m_modifiers;
  };

  // add items to the registry
  bool registerType(Type* t);  // returns false if type is already registered
  bool addTypeAlias(const PString& alias, const TypesRegistry::Alias* target);

  // access to registered types, either by passing in an iterator or by getting a copy
  // of the DB as an array
  unsigned int getNumberRegisteredTypes() const;
  void const_iterate(ConstTypeIterator ti, void* userdata = 0) const;
  void iterate(TypeIterator ti, void* userdata = 0);
  void getAllTypes(Types& ty) const;

  const Type* lookupFromVarDecl(const VarDeclBase& vd) const;
  const Type* lookupByName(const PString& sNamespace, const PString& sName) const;

private:
  TypesRegistryData  *m_data;
};

//----------------------------------------------------------------------------------------------------------------------
// if a module is marked as a 'Limb' then for every instance of it we create a LimbInstance; this is used
// to expose limb-assignment stuff to morpheme, etc.
//
struct LimbInstance
{
  LimbInstance();
  enum LimbType
  {
     kArm,
     kHead,
     kLeg,
     kSpine,

     kUnknown
  };

  const ModuleInstance     *m_inst;       // the module child instance that spawned this limb
  unsigned int              m_index;      // index local to the module type that created it
  PString                   m_name;       // system name for the limb, eg. Arm_1
  LimbType                  m_limbtype;   // They type of the limb
};

//----------------------------------------------------------------------------------------------------------------------
// simple list of modules, defines a group of 1-or-more children whose dependencies are such that
// they can be updated in parallel.
//
struct ExecutionBucket
{
  ExecutionBucket();

  ModuleInstances   m_instances;
};
typedef nmtl::pod_vector<ExecutionBucket*>  ExecutionBuckets;


//----------------------------------------------------------------------------------------------------------------------
struct ConnectionInstance
{
  ConnectionInstance() :
    m_instanceTarget(0),
    m_varDeclTarget(0),
    m_junction(0),
    m_junctionGroup(-1),
    m_varDeclSource(0),
    m_instanceSource(0) 
  {
  }

  const ModuleInstance               *m_instanceTarget;
  const ModuleDef::VarDecl::Target   *m_varDeclTarget;  // which variable this binds to in the owner instance

  const ModuleDef::Junction          *m_junction;
  int                                 m_junctionGroup;

  const ModuleDef::VarDecl::Target   *m_varDeclSource;
  const ModuleInstance               *m_instanceSource;
};
typedef nmtl::pod_vector<ConnectionInstance*>  ConnectionInstances;


//----------------------------------------------------------------------------------------------------------------------
// represents an instance of a ModuleDef; strung together to form a virtual instance of the
// network, allowing for accurate traversal of the live network topology
//
struct ModuleInstance
{
  ModuleInstance();

  ModuleDef              *m_module;               // pointer to module definition for this instance
  PString                 m_name;                 // name of child instance from ModuleDef - eg. Arm -> myLeftArm <-
  PString                 m_sku;                  // which sku was used to instance the module
  PString                 m_NIID;                 // Network Instance ID; unique identifier that represents this 
                                                  // instance in the global network table ("arm_0", "arm_1", etc. see NetworkDescriptor.h)

  unsigned int            m_arrayIndex;
  bool                    m_isArrayInstance;

  ModuleInstance         *m_parent;               // parent instance, or null for root module instance
  ModuleInstances         m_children;             // list of child instances

  ConnectionInstances     m_connections;

  // - Dependency & Concurrency Analysis -------

  UniqueModuleInstances   m_updateDependencies;   // list of module instances, formed from the ModuleDef's reducedUpdateDependencies
  UniqueModuleInstances   m_feedbackDependencies;

  ExecutionBuckets        m_updateBuckets,        // N number of buckets that contain a list of child modules of this instance
                          m_feedbackBuckets;      // can be executed in parallel. This also therefore defines the dependency
                                                  // sorted update/feedback order of this instance's children
};

//----------------------------------------------------------------------------------------------------------------------
// The conglomeration of all MDF components, representation of a complete euphoria3 MDF database
//
struct NetworkDatabase
{
  explicit NetworkDatabase(unsigned int defaultContainerSize);

  unsigned int        m_moduleNUID;

  KeywordsManager     m_keywords;               // manager for checking/validating keywords & modifiers
  TypesRegistry       m_typesRegistry;          // registry for all types in the system
  NetworkConstants    m_constants;              // constants bank configured by build system

  // lists/tables of MDF constructs
  ModuleDefs          m_moduleDefs;
  StructMap           m_structMap;
  StructMap           m_commentMap;
  BehaviourDefs       m_behaviourDefs;
  LimbInstances       m_limbInstances;

  // instantiated network topology + concurrency data
  ModuleInstance     *m_instance;

private:
  // disallow accidental copy/assignment
  NetworkDatabase();
  NetworkDatabase(const NetworkDatabase& rhs);
  NetworkDatabase& operator=(const NetworkDatabase& rhs);
};


// ---------------------------------------------------------------------------------------------------------------------
// hashing functions for various mapped types
namespace nmtl
{
  namespace hfn
  {
    NMTL_INLINE unsigned int hashFunction(ModuleDef* key) { return hashFunction(reinterpret_cast<void*>(key)); }

    NMTL_INLINE unsigned int hashFunction(ModuleInstance* key) { return hashFunction(reinterpret_cast<void*>(key)); }

    NMTL_INLINE unsigned int hashFunction(const Type* key) { return hashFunction(reinterpret_cast<const void*>(key)); }

    NMTL_INLINE unsigned int hashFunction(ModuleDef::Junction* key) { return hashFunction(reinterpret_cast<void*>(key)); }

    NMTL_INLINE unsigned int hashFunction(BoundModuleDependency* key) { return key->genHash(); }

    template <typename KeyType>
    NMTL_INLINE bool hashKeysEqual(BoundModuleDependency* lhs, BoundModuleDependency* rhs) 
    { 
      return lhs->genHash() == rhs->genHash(); 
    }
  }
}

const char* getLimbNameFromArray(const char* limbArrayName);
const char* stripWhiteSpace(const char* scData);
