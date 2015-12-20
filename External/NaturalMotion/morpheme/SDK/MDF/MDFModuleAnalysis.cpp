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
#include "MDFKeywords.h"
#include "ParserMemory.h"
#include "StringBuilder.h"
#include "Utils.h"

#include <shlwapi.h>
#include "nmtl/algorithm.h"
#include "nmtl/hashmap_dense_dynamic.h"

using namespace MDFModule;

// static string used when throwing exceptions
StringBuilder      gExceptionSB(64);

//----------------------------------------------------------------------------------------------------------------------
struct ModuleAnalysisData
{
  enum
  {
    defaultLookupSize = 128
  };

  ModuleAnalysisData() : 
    moduleDefNameMap(defaultLookupSize, ActiveMemoryManager::getForNMTL()),
    uniqueNameLookup(defaultLookupSize, ActiveMemoryManager::getForNMTL()),
    sb(defaultLookupSize)
  {
  }

  const KeywordsManager  *keywords;
  const TypesRegistry    *typesRegistry;

  // name-to-def lookup, formed during validation phase, used in connectivity phase
  typedef nmtl::hashmap_dense_dynamic<unsigned int, ModuleDef*>    ModuleDefNameMap;
  ModuleDefNameMap        moduleDefNameMap;

  // lookup for checking unique names across collections (eg. module children)
  UniqueNames             uniqueNameLookup;

  StringBuilder           sb;
};

//----------------------------------------------------------------------------------------------------------------------
ModuleAnalysis::ModuleAnalysis(const KeywordsManager* keywords, const TypesRegistry* typesRegistry) : 
  data(0)
{
  parserMemoryAllocDirect(ModuleAnalysisData, data);
  data->keywords = keywords;
  data->typesRegistry = typesRegistry;
}

//----------------------------------------------------------------------------------------------------------------------
ModuleAnalysis::~ModuleAnalysis()
{
  data->~ModuleAnalysisData();  // for the stringbuilder
  data = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// we sort limbs alphabetically
struct LimbSorter
{
  bool operator() (LimbInstance* elem1, LimbInstance* elem2)
  {
    return (strcmp(elem1->m_name.c_str(), elem2->m_name.c_str()) < 0);
  }
};


//----------------------------------------------------------------------------------------------------------------------
// does the work of actually validating a vardecl; called by validateModuleVarDecls() on every VD in every module
//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::validateModuleVarDecl(ModuleDef& def, ModuleDef::VarDecl& vd)
{
  const unsigned int vdMaxNameLength = 260;
  static char vdNameCapFront[vdMaxNameLength];

  // capitalize the first character of the variable name
  const char* vdName = vd.m_varname.c_str();
  strcpy_s(vdNameCapFront, vdMaxNameLength, vdName);
  vdNameCapFront[0] = (char)toupper(vdName[0]);
  vd.m_varnameCapFront.set(vdNameCapFront);

  // lookup known-type data for this decl (otherwise returns null .. which is a failure)
  vd.m_knownType = data->typesRegistry->lookupFromVarDecl(vd);
  if (!vd.m_knownType)
  {
    vd.toString(gExceptionSB);
    throw(new ValidationException(&def, vd.m_lineDefined, gExceptionSB.c_str(), "Unknown type used"));
  }

  // we don't support allowing __validator__() attributes on vardecls that are compounds
  if (vd.m_knownType->isCompoundType())
  {
    if (hasVarDeclModifier(vd.m_modifiers, vdmValidator))
    {
      vd.toString(gExceptionSB);
      throw(new ValidationException(&def, vd.m_lineDefined, gExceptionSB.c_str(), "Cannot apply validators to compound variable type"));
    }
  }

  // check storage specifier
  if (vd.m_storageSpecifier != 0)
  {
    vd.toString(gExceptionSB);

    // reference variables not allowed anywhere in VarDecl blocks
    if (vd.m_storageSpecifier == '&')
    {
      throw(new ValidationException(&def, vd.m_lineDefined, gExceptionSB.c_str(), 
        "Reference variables not allowed in module blocks"));
    }

    if (vd.m_storageSpecifier != '*')
    {
      throw(new ValidationException(&def, vd.m_lineDefined, gExceptionSB.c_str(), 
        "Unknown variable access specifier"));
    }

    // pointers are only allowed in global access modules
    if (!hasModuleModifier(def.m_modifiers, mmGlobalAccess))
    {
      throw(new ValidationException(&def, vd.m_lineDefined, gExceptionSB.c_str(), 
        "Can only use pointer-type variables in global access modules"));
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
// loop through all variable decls, ensuring they are fit for purpose;
// also sort them by size at this point, for appropriate packing during code generation.
// note: this is called as each MDF is loaded, rather than during the post-process
//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::validateModuleVarDecls(ModuleDef& def)
{
  VarDeclTypeSizeSorter sorter;

  for (unsigned int n=0; n<msNumVarDeclBlocks; n++)
  {
    iterate_begin(ModuleDef::VarDecl, def.m_vardecl[n], vd)
    {
      validateModuleVarDecl(def, vd);
    }
    iterate_end

    // sort each block by effective type size
    nmtl::sort( def.m_vardecl[n].begin( ), def.m_vardecl[n].end( ), sorter);
  }

  iterate_begin(ModuleDef::VarDecl, def.m_private, vd)
  {
    validateModuleVarDecl(def, vd);
  }
  iterate_end
}


//----------------------------------------------------------------------------------------------------------------------
// validateConnectionPath() ensures the syntactical structure of the connection paths is valid.
// It also assigns more contextual identity to the path components that have little - eg. is an 'identifier' component
//    a variable name, junction name, ..? 
//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::validateConnectionPath(ModuleDef& def, ModuleDef::Connection& con, ModuleDef::Connection::Path& cp)
{
  // path mode forms a simple state machine
  enum PathMode
  {
    pmModule,             // can transition to pmOwner or pmBlock
    pmOwner,              // can transition to pmModule or pmBlock
    pmBlock,              // can transition to pmVariable
    pmVariable,           // terminal
    pmJunctionSection,    // can transition to pmJunctionName
    pmJunctionName        // terminal
  } 
  pathMode = pmModule;  // initially assume the context is pointing at the current module ('this' pointer equiv)

  unsigned int itemIndex = 0;
  cp.m_localDataOnly = true;

  iterate_begin_ptr(ModuleDef::Connection::Path::Component, cp.m_components, comp)
  {
    assert(comp->m_context != ccInvalid);

    // if this is a 'access by link' component, we need to drop in and validate it's embedded path
    if (comp->isType(ModuleDef::Connection::Path::Component::cArrayAccessByLink))
    {
      ModuleDef::Connection::Path::ComponentArrayAccessByLink* caal = 
        (ModuleDef::Connection::Path::ComponentArrayAccessByLink*)comp;

      validateConnectionPath(def, con, *caal->m_linkPath);
    }

    // handle instance variable types
    if (comp->isType(ModuleDef::Connection::Path::Component::cInstanceVariable))
    {
      // can only access instance variables when we're at module context
      if (pathMode != pmModule && 
          pathMode != pmOwner)
      {
        cp.toString(gExceptionSB, itemIndex);
        throw(new ValidationException(&def, con.m_lineDefined, gExceptionSB.c_str(), 
          "can only access instance variables from a module"));
      }

      // in the future we can support more than just 'childindex'
      if (comp->m_text == "ChildIndex")
        comp->m_context = ccIVChildIndex;
      else
      {
        cp.toString(gExceptionSB, itemIndex);
        throw(new ValidationException(&def, con.m_lineDefined, gExceptionSB.c_str(), 
          "unknown instance variable (can only be ChildIndex)"));
      }

      // terminate path
      pathMode = pmVariable;
    }

    if (comp->m_context == ccJunctions) 
    {
      // can only refer to junctions from the current sku
      if (pathMode != pmModule)
      {
        cp.toString(gExceptionSB, itemIndex);
        throw(new ValidationException(&def, con.m_lineDefined, gExceptionSB.c_str(), 
          "Connection path invalid - can only access junctions from current module"));
      }

      pathMode = pmJunctionSection;
    }
    else if (comp->m_context == ccUnknown)  // this, we must assume, means a module or junction name 
    {
      // this means we have something like "out.variable.variable"
      if (pathMode == pmVariable)
      {
        cp.toString(gExceptionSB, itemIndex);
        throw(new ValidationException(&def, con.m_lineDefined, gExceptionSB.c_str(), 
          "Connection path invalid - multiple variable definitions following block reference"));
      }

      // this means we have something like "junctions.name.name"
      if (pathMode == pmJunctionName)
      {
        cp.toString(gExceptionSB, itemIndex);
        throw(new ValidationException(&def, con.m_lineDefined, gExceptionSB.c_str(), 
          "Connection path invalid - accessing multiple junction names in sequence"));
      }

      // reassign m_context with our new understanding
      if (pathMode == pmJunctionSection)
      {
        pathMode = pmJunctionName;
        comp->m_context = ccJunctionName;
      }
      else if (pathMode == pmBlock)
      {
        pathMode = pmVariable;
        comp->m_context = ccVariableName;
      }
      else
      {
        // otherwise, we are referring to a module by name
        pathMode = pmModule;
        comp->m_context = ccModuleName;

        cp.m_localDataOnly = false;
      }
    }
    // navigation tokens
    else if (comp->m_context == ccInputs || comp->m_context == ccUserInputs || comp->m_context == ccOutputs || 
             comp->m_context == ccFeedInputs || comp->m_context == ccFeedOutputs ||
             comp->m_context == ccOwner)
    {
      if (comp->isType(ModuleDef::Connection::Path::Component::cArrayAccess) ||
          comp->isType(ModuleDef::Connection::Path::Component::cArrayAccessByLink) || 
          comp->isType(ModuleDef::Connection::Path::Component::cIterateArray) ||
          comp->isType(ModuleDef::Connection::Path::Component::cExpandArray))
      {
        cp.toString(gExceptionSB, itemIndex);
        throw(new ValidationException(&def, con.m_lineDefined, gExceptionSB.c_str(), 
          "Array access on owner/block elements is invalid."));
      }

      // check for "out.out.variable", no cascading block refs allowed
      if (pathMode == pmBlock)
      {
        cp.toString(gExceptionSB, itemIndex);
        throw(new ValidationException(&def, con.m_lineDefined, gExceptionSB.c_str(), 
          "Connection path invalid - one block definition cannot immediately follow another"));
      }
      // check for "junctions.out" or "junctions.data.out"
      else if (pathMode == pmJunctionSection || 
               pathMode == pmJunctionName)
      {
        cp.toString(gExceptionSB, itemIndex);
        throw(new ValidationException(&def, con.m_lineDefined, gExceptionSB.c_str(), 
          "Connection path invalid - block reference cannot follow junction(s)"));
      }

      if (comp->m_context == ccOwner)
      {
        // ensure that 'owner' tokens are first in the path, avoiding module.owner.module format
        // or 'owner' can follow another owner
        if ((pathMode == pmModule && itemIndex != 0) &&
            pathMode != pmOwner)
        {
          cp.toString(gExceptionSB, itemIndex);
          throw(new ValidationException(&def, con.m_lineDefined, gExceptionSB.c_str(), 
            "Connection path invalid - references to 'owner' can only start a path, not appear after other items"));
        }
        pathMode = pmOwner;

        cp.m_localDataOnly = false;
      }
      else
      {
        pathMode = pmBlock;
      }
    }
    
    itemIndex ++;
  }
  iterate_end
}


//----------------------------------------------------------------------------------------------------------------------
// called after loading all module definitions; the first pass through ensures that the described modules are 
// correctly formed, with no glaring errors, broken vardecls, etc
//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::validateModuleDefs(ModuleDefs& defs)
{
  bool encounteredRootModule = false;

  iterate_begin_ptr(ModuleDef, defs, def)
  {
    assert(def->m_NUID != ModuleDef::InvalidNUID);

    // -----------------------------------------------------------------------------------------------------------------
    // look the name up in the type registry to see if it clashes
    const Type* moduleNameIsAType = data->typesRegistry->lookupByName("", def->m_name);
    if (moduleNameIsAType)
    {
      throw(new ValidationException(def, def->m_lineDefined, def->m_name.c_str(), 
        "Module name clashes with a registered type"));
    }

    // -----------------------------------------------------------------------------------------------------------------
    // ensure this module's name is unique across the whole network
    unsigned int nameHash = def->m_name.hashValueA();
    ModuleDef *duplicateClash = 0;
    if (data->moduleDefNameMap.find(nameHash, &duplicateClash))
    {
      // on a name clash, form an error report with the two filenames of the clashing modules in
      StringBuilder pathStrip(64);
      pathStrip.appendPString(def->m_pathToDef);
      pathStrip.invoke(PathStripPath);

      gExceptionSB.reset();
      gExceptionSB.appendf("'%s' and ", pathStrip.c_str());

      pathStrip.reset();
      pathStrip.appendPString(duplicateClash->m_pathToDef);
      pathStrip.invoke(PathStripPath);

      gExceptionSB.appendf("'%s'", pathStrip.c_str());

      throw(new ValidationException(def, 1, gExceptionSB.c_str(), 
        "Duplicate module name detected"));
    }

    // it is unqiue, add a reference to it to our local name lookup
    data->moduleDefNameMap.insert(nameHash, def);

    // keep track of owner-less 'root' modules - we only want one!
    if (def->m_moduleOwner.empty())
    {
      if (encounteredRootModule)
      {
        throw(new ValidationException(def, def->m_lineDefined, def->m_name.c_str(), 
          "Second root node encountered - only one allowed per network"));
      }
      encounteredRootModule = true;
    }

    // -----------------------------------------------------------------------------------------------------------------
    // check for duplicate children
    data->uniqueNameLookup.clear();
    iterate_begin(ModuleDef::Child, def->m_children, child)
    {
      if (!data->uniqueNameLookup.add(child.m_name.c_str()))
      {
        throw(new ValidationException(def, child.m_lineDefined, child.m_name.c_str(), 
          "Child names must be unique within a module"));
      }
    }
    iterate_end

    // -----------------------------------------------------------------------------------------------------------------
    // check for duplicate variables in each block
    for (int blk = msData; blk <= msOutputs; blk ++)
    {
      data->uniqueNameLookup.clear();
      iterate_begin_const(ModuleDef::VarDecl, def->m_vardecl[blk], vd)
      {
        if (!data->uniqueNameLookup.add(vd.m_varname.c_str()))
        {
          throw(new ValidationException(def, vd.m_lineDefined, vd.m_varname.c_str(), 
            "Variable names must be unique within a module block"));
        }
      }
      iterate_end
    }

    // -----------------------------------------------------------------------------------------------------------------
    // validate connection targets
    iterate_begin_ptr(ModuleDef::ConnectionSet, def->m_connectionSets, set)
    {
      // while we're iterating, see if we have a universal (empty) set
      if (set->m_name.empty())
        def->m_defaultSet = set;

      data->uniqueNameLookup.clear();
      iterate_begin_ptr(ModuleDef::Connection, set->m_connectionDefs, con)
      {
        // make sure no invalid connections have slipped through the net
        if (con->m_task == ctInvalid)
        {
          throw(new ValidationException(def, con->m_lineDefined, "Invalid Task", 
            "Could not determine purpose of connection"));
        }

        // ensure a connection tasked with creating a new junction has the parameters necessary to do so
        if (con->m_task == ctCreateJunction)
        {
          // check the new junction has a unique name within this connection set
          const ModuleDef::Connection::Path::Component* jName = con->m_target->m_components.back();
          if (!data->uniqueNameLookup.add(jName->m_text.c_str()))
          {
            throw(new ValidationException(def, con->m_lineDefined, jName->m_text.c_str(), 
              "Duplicate junction name found in connection set"));
          }

          if (con->m_newJunctionMode == jmInvalid)
          {
            throw(new ValidationException(def, con->m_lineDefined, "Junction Construction", 
              "Connection specifies new junction but has not stored name/type of new junction"));
          }
        }
        else if (con->m_task == ctAssignJunction)
        {
          if (con->m_paths.size() != 1)
          {
            throw(new ValidationException(def, con->m_lineDefined, "Junction Assignment", 
              "Too many / too few connection paths found for a junction assignment"));
          }
        }

        // validate the path to the target
        validateConnectionPath(*def, *con, *con->m_target);

        // check for gather ops
        if (con->m_task == ctCreateJunction)
        {
          const ModuleDef::Connection::Path::Component* targetComp = con->m_target->m_components.back();
          if (targetComp->isType( ModuleDef::Connection::Path::Component::cExpandArray ))
          {
            if (con->m_newJunctionMode != jmDirectInput)
            {
              gExceptionSB.reset();
              gExceptionSB.appendPString( data->keywords->getJunctionModeLabel(con->m_newJunctionMode) );

              throw(new ValidationException(def, con->m_lineDefined, gExceptionSB.c_str(), 
                "Can only use directInput junctions with gather operations"));
            }

            if (targetComp->m_context != ccVariableName)
            {
              throw(new ConnectivityException(def, con->m_lineDefined, targetComp->m_text.c_str(), 
                "Can only assign gather operations to module member variables"));
            }

            con->m_task = ctGatherOp;
          }
        }

        // check that for a 'new junction' task, we open with a reference to the junction section
        // eg.  "junctions.data = average(...)"
        // not  "out.value = average(...)"
        if (con->m_target->m_components.size() > 0 && con->m_task == ctCreateJunction)
        {
          if (con->m_target->m_components[0]->m_context != ccJunctions) 
          {
            con->m_target->toString(gExceptionSB, 0);
            throw(new ValidationException(def, con->m_lineDefined, gExceptionSB.c_str(), 
              "Connection path invalid - new junctions must be assigned to the local junctions section"));
          }
        }
      }
      iterate_end
    }
    iterate_end

    // -----------------------------------------------------------------------------------------------------------------
    // if we do have a default set, see if we have other sets that need to have the defaults copied into them - 
    // therefore non-default sets automatically inherit junctions/assignments made in the default
    if (def->m_defaultSet)
    {
      nmtl::hashmap_dense_dynamic<const char*, ModuleDef::Connection*> junctionCreateLookup(32);
      nmtl::hashmap_dense_dynamic<nmtl::hkey64, ModuleDef::Connection*> junctionAssignLookup(32);

      iterate_begin_ptr(ModuleDef::ConnectionSet, def->m_connectionSets, set)
      {
        if (!set->m_name.empty() && !hasConnectionSetModifier(set->m_modifiers, csmNoSetInheritance))
        {
          // we need a lookup of all the junctions in the non-default set
          junctionCreateLookup.clear();
          junctionAssignLookup.clear();
          iterate_begin_ptr(ModuleDef::Connection, set->m_connectionDefs, con)
          {
            if (con->m_task == ctCreateJunction)
            {
              const ModuleDef::Connection::Path::Component* jName = con->m_target->m_components.back();
              if (junctionCreateLookup.find(jName->m_text.c_str(), 0))
              {
                throw(new ValidationException(def, con->m_lineDefined, jName->m_text.c_str(), 
                  "Duplicate junction name found in connection set."));
              }

              junctionCreateLookup.insert(jName->m_text.c_str(), con);
            }
            else if (con->m_task == ctAssignJunction)
            {
              if (junctionAssignLookup.find(con->m_target->m_componentsHashKey, 0))
              {
                con->m_target->toString(gExceptionSB);
                throw(new ValidationException(def, con->m_lineDefined, gExceptionSB.c_str(), 
                  "Duplicate junction assignment - cannot connect to the same variable more than once per set"));
              }

              junctionAssignLookup.insert(con->m_target->m_componentsHashKey, con);
            }
            else
            {
              assert(0);
            }
          }
          iterate_end

          // walk backwards over the set to copy and push to the front, so we're copying the order correctly too
          iterate_begin_reverse_ptr(ModuleDef::Connection, def->m_defaultSet->m_connectionDefs, con)
          {
            bool copyThisDef = true;
            ModuleDef::Connection* nonDefaultConSet = 0;

            if (con->m_task == ctCreateJunction)
            {
              // see if this junction is already present in the non-default set
              const ModuleDef::Connection::Path::Component* jName = con->m_target->m_components.back();
              if (junctionCreateLookup.find(jName->m_text.c_str(), &nonDefaultConSet))
              {
                // junction clash - see if we are allowed to override (eg. not copy from the default set)
                if (!hasConnectionTaskModifier(nonDefaultConSet->m_modifiers, ctmOverride))
                {
                  throw(new ValidationException(def, nonDefaultConSet->m_lineDefined, jName->m_text.c_str(), 
                    "Duplicate junction being inherited from default connection set."));
                }

                // the junction is marked to override, so skip copying it from the default set
                copyThisDef = false;
                nonDefaultConSet->m_wasInheritedViaOverride = true;
              }
            }
            else if (con->m_task == ctAssignJunction)
            {
              if (junctionAssignLookup.find(con->m_target->m_componentsHashKey, &nonDefaultConSet))
              {
                // junction assignment clash - same as above, check the modifier or bail out
                if (!hasConnectionTaskModifier(nonDefaultConSet->m_modifiers, ctmOverride))
                {
                  con->m_target->toString(gExceptionSB);
                  throw(new ValidationException(def, nonDefaultConSet->m_lineDefined, gExceptionSB.c_str(), 
                    "Duplicate junction assignment being inherited from default connection set."));
                }

                copyThisDef = false;
                nonDefaultConSet->m_wasInheritedViaOverride = true;
              }
            }
            else
            {
              assert(0);
            }

            if (copyThisDef)
            {
              // clone the def
              parserMemoryAllocArgs(ModuleDef::Connection, copiedDef, *con);
              vectorPushFront(set->m_connectionDefs, copiedDef);
            }
          }
          iterate_end
        }
      }
      iterate_end
    }

    // -----------------------------------------------------------------------------------------------------------------
    // validate connection paths
    iterate_begin_ptr(ModuleDef::ConnectionSet, def->m_connectionSets, set)
    {
      // sort the connection defs by task - "Create" first, "Assign" second
      nmtl::sort(set->m_connectionDefs.begin(), set->m_connectionDefs.end(), ModuleDef::Connection::SortByTask);

      iterate_begin_ptr(ModuleDef::Connection, set->m_connectionDefs, con)
      {
        // check that there are no rogue __override__ tags
        if (hasConnectionTaskModifier(con->m_modifiers, ctmOverride) && 
            !con->m_wasInheritedViaOverride)
        {
          con->m_target->toString(gExceptionSB);
          throw(new ValidationException(def, con->m_lineDefined, gExceptionSB.c_str(), 
            "Connection marked as 'override' but does not override anything."));
        }

        // go validate the source paths
        iterate_begin_ptr(ModuleDef::Connection::Path, con->m_paths, path)
        {
          validateConnectionPath(*def, *con, *path);
        }
        iterate_end
      }
      iterate_end
    }
    iterate_end


  }
  iterate_end
}


//----------------------------------------------------------------------------------------------------------------------
// walks a given path to find the specified target, be it junction or vardecl; returns the pointer to the instance 
// of the specified target in a ConnectionPathTargetResult structure
//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::calculateConnectionPath(
  ModuleDef::Connection::Path& cp,              // path to execute
  unsigned int cpLineDefined,                   // line where the path was defined, should we need to throw exceptions
  ModuleDef& def,                               // owning module definition
  ModuleDef::ConnectionSet& set,                // current connection set (for looking up other junctions)
  ModuleDependencies* moduleDependencyTable)    // [optional] hashtable of module dependencies that will be added to as modules are encountered
{
  unsigned int itemIndex = 0;
  cp.m_target.clear();

  // assume we start pointing at ourselves; can be modified by an 'owner' reference in the path
  ModuleDef* currentModule = &def;

  // which block of variables we would rifle through when searching for vardecls
  ModuleDef::VarDecls* blockRef = 0;

  ModuleBinding binding;

  // this loop 'executes' the connection path
  iterate_begin_ptr(ModuleDef::Connection::Path::Component, cp.m_components, comp)
  {
    switch (comp->m_context)
    {
      // -------------------------------------------------------------------------------------------
      case ccJunctionName:
        {
          // linear search for matching junction name
          for (ModuleDef::Junctions::iterator vIt  = set.m_junctions.begin(), vEnd = set.m_junctions.end(); 
            vIt != vEnd; 
            ++vIt)
          {
            ModuleDef::Junction* jnc = (*vIt);
            if (jnc->m_name == comp->m_text)
            {
              cp.m_target.m_junction = jnc;
            }
          }

          // found the specified junction?
          if (cp.m_target.m_junction == 0)
          {
            cp.toString(gExceptionSB, itemIndex);
            throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
              "Could not find named junction"));
          }

          // cascade any module references from this junction
          for (ModuleDependencies::iterator mIt = cp.m_target.m_junction->m_moduleReferences.begin(), 
                chEnd = cp.m_target.m_junction->m_moduleReferences.end(); 
                mIt != chEnd; 
                ++mIt)
          {
            BoundModuleDependency* md = *mIt;

            set.m_moduleReferences.add(md);
            if (moduleDependencyTable)
              moduleDependencyTable->add(md);
          }

          // store pointer back to junction as contextData
          comp->m_contextData = cp.m_target.m_junction;

          // we're done, we gone found a thing
          cp.m_target.m_varDecl.m_varDeclPtr = 0;
          cp.m_target.m_moduleTarget.m_def = currentModule;
          cp.m_target.m_moduleTarget.m_binding = binding.deepCopy();
          return;
        }
        break;

      // -------------------------------------------------------------------------------------------
      case ccVariableName:
        {
          // make sure we have a target block reference
          if (!blockRef || blockRef->size() == 0)
          {
            cp.toString(gExceptionSB, itemIndex);
            throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
              "Pointing to empty or invalid block"));
          }

          // we have a block to look into, search inside that
          {
            // walk and compare names to find an appropriate target
            for (ModuleDef::VarDecls::iterator vIt = blockRef->begin(), vEnd = blockRef->end(); 
                  vIt != vEnd; 
                  ++vIt)
            {
              ModuleDef::VarDecl& vd = (*vIt);
              if (vd.m_varname == comp->m_text)
              {
                // success, we have found an end-point
                cp.m_target.m_moduleTarget.m_def = currentModule;
                cp.m_target.m_moduleTarget.m_binding = binding.deepCopy();
                cp.m_target.m_varDecl.m_varDeclPtr = &vd;

                // store pointer back to VarDecl as contextData
                comp->m_contextData = &vd;

                if (comp->isType(ModuleDef::Connection::Path::Component::cArrayAccess))
                {
                  ModuleDef::Connection::Path::ComponentArrayAccess* caa = (ModuleDef::Connection::Path::ComponentArrayAccess*)comp;
                  cp.m_target.m_varDecl.m_arrayElementAccess = caa;

                  // make sure target variable is actually an array
                  if (cp.m_target.m_varDecl.m_varDeclPtr->m_arraySize == 0)
                  {
                    cp.toString(gExceptionSB, itemIndex);
                    throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
                      "Specifying array element access on non-array variable"));
                  }

                  // .. and that the requested element is in range
                  if (cp.m_target.m_varDecl.m_varDeclPtr->m_arraySize <= caa->m_arrayIndex )
                  {
                    cp.toString(gExceptionSB, itemIndex);
                    throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
                      "Array index out of range"));
                  }
                }
                else if (comp->isType(ModuleDef::Connection::Path::Component::cArrayAccessByLink))
                {
                  ModuleDef::Connection::Path::ComponentArrayAccessByLink* caal = (ModuleDef::Connection::Path::ComponentArrayAccessByLink*)comp;
                  cp.m_target.m_varDecl.m_arrayElementAccess = caal;

                  calculateConnectionPath(*caal->m_linkPath, cpLineDefined, def, set, moduleDependencyTable);

                  // make sure target variable is actually an array
                  if (cp.m_target.m_varDecl.m_varDeclPtr->m_arraySize == 0)
                  {
                    cp.toString(gExceptionSB, itemIndex);
                    throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
                      "Specifying array element access on non-array variable"));
                  }
                }
                else if (comp->isType(ModuleDef::Connection::Path::Component::cExpandArray) ||
                         comp->isType(ModuleDef::Connection::Path::Component::cIterateArray))
                {
                  cp.m_target.m_varDecl.m_arrayElementAccess = comp;

                  if (comp->isType(ModuleDef::Connection::Path::Component::cIterateArray))
                    cp.m_target.m_expandToMultiple = cp.m_target.m_varDecl.m_varDeclPtr->m_arraySize;

                  // make sure target variable is actually an array
                  if (cp.m_target.m_varDecl.m_varDeclPtr->m_arraySize == 0)
                  {
                    cp.toString(gExceptionSB, itemIndex);
                    throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
                      "Specifying array element access on non-array variable"));
                  }
                }
                break;
              }
            }

            // check that we found something
            if (cp.m_target.m_moduleTarget.m_def == 0 || cp.m_target.m_varDecl.m_varDeclPtr == 0)
            {
              cp.toString(gExceptionSB, itemIndex);
              throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
                "Could not find variable with this name in the given module & block"));
            }

            // we are referring to an array type without an array specifier - so mark this
            // as a target that needs to expand out to the size of the target array (eg. make N junctions)
            if (cp.m_target.m_varDecl.m_varDeclPtr->m_arraySize > 0 &&
                cp.m_target.m_varDecl.m_arrayElementAccess == 0)
                cp.m_target.m_expandToMultiple = cp.m_target.m_varDecl.m_varDeclPtr->m_arraySize;

            // we're done, we gone found a thing
            cp.m_target.m_junction = 0;
            return;
          }
        }
        break;

      // -------------------------------------------------------------------------------------------
      case ccModuleName:
        {
          // go lookup the specified child, make sure it exists
          const ModuleDef::Child* child = currentModule->getChildByName(comp->m_text);
          if (!child)
          {
            cp.toString(gExceptionSB, itemIndex);
            throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
              "Could not find child module with this name"));
          }

          // store pointer back to module as contextData
          comp->m_contextData = child;

          parserMemoryAlloc(ModuleBinding::BindOp, newOp);
          newOp->m_op = ModuleBinding::LookupChildByName;
          newOp->m_text = comp->m_text;

          // array access specified directly
          if (comp->isType(ModuleDef::Connection::Path::Component::cArrayAccess))
          {
            ModuleDef::Connection::Path::ComponentArrayAccess* caa = (ModuleDef::Connection::Path::ComponentArrayAccess*)comp;

            // not in an array
            if (child->m_arraySize == 0)
            {
              cp.toString(gExceptionSB, itemIndex);
              throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
                "Attempting to access a child node by array index, child is not in an array"));
            }
            // asking for a child index outside of the array range
            else if (caa->m_arrayIndex >= child->m_arraySize)
            {
              cp.toString(gExceptionSB, itemIndex);
              throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
                "Array access on child module out of bounds"));
            }

            newOp->m_arrayIndex = caa->m_arrayIndex;
          }
          // array access by link (sub path)
          else if (comp->isType(ModuleDef::Connection::Path::Component::cArrayAccessByLink))
          {
            ModuleDef::Connection::Path::ComponentArrayAccessByLink* caal = 
              (ModuleDef::Connection::Path::ComponentArrayAccessByLink*)comp;

            // 'validate' the path; required, as it also updates the junction dependencies
            calculateConnectionPath(*caal->m_linkPath, cpLineDefined, def, set, moduleDependencyTable);

            // not in an array
            if (child->m_arraySize == 0)
            {
              cp.toString(gExceptionSB, itemIndex);
              throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
                "Attempting to access a child node by array index, child is not in an array"));
            }
          }
          // array expansion/iterators
          else if (comp->isType(ModuleDef::Connection::Path::Component::cIterateArray) ||
                   comp->isType(ModuleDef::Connection::Path::Component::cExpandArray))
          {
            if (comp->isType(ModuleDef::Connection::Path::Component::cIterateArray))
              cp.m_target.m_expandToMultiple = child->m_arraySize;

            // not in an array
            if (child->m_arraySize == 0)
            {
              cp.toString(gExceptionSB, itemIndex);
              throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
                "Attempting to access a child node by array index, child is not in an array"));
            }
          }
          // make sure that if the child is infact an array, we are not missing some kind of addressing
          else
          {
            if (child->m_arraySize > 0)
            {
              cp.toString(gExceptionSB, itemIndex);
              throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
                "Attempting to access a child node stored in an array without an array index"));
            }
          }

          // shift context to the child ptr
          currentModule = child->m_modulePtr;

          binding.m_ops.push_back(newOp);

          parserMemoryAlloc(BoundModuleDependency, bmd);
          bmd->m_def = currentModule;
          bmd->m_binding = binding.deepCopy();

          // log appearance of this new module in the references caches
          set.m_moduleReferences.add(bmd);
          if (moduleDependencyTable)
            moduleDependencyTable->add(bmd);
        }
        break;

      // -------------------------------------------------------------------------------------------
      // owner ref specified, attempt to jump to the current module's owner
      case ccOwner:
        {
          // uh oh, no parent!
          if (currentModule->m_moduleOwnerPtr == 0)
          {
            cp.toString(gExceptionSB, itemIndex);
            throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
              "Could not get owner, module has no parent"));
          }

          // store pointer back to module as contextData
          comp->m_contextData = currentModule->m_moduleOwnerPtr;

          // jump up to the owner of the current
          currentModule = currentModule->m_moduleOwnerPtr;

          {
            parserMemoryAlloc(ModuleBinding::BindOp, newOp);
            newOp->m_op = ModuleBinding::GetOwner;
            binding.m_ops.push_back(newOp);
          }

          parserMemoryAlloc(BoundModuleDependency, bmd);
          bmd->m_def = currentModule;
          bmd->m_binding = binding.deepCopy();

          // log appearance of this new module in the references caches
          set.m_moduleReferences.add(bmd);
          if (moduleDependencyTable)
            moduleDependencyTable->add(bmd);
        }
        break;

      // -------------------------------------------------------------------------------------------
      case ccJunctions:
        {
          // nothing to do here, already figured out during path pre-process
        }
        break;

      case ccIVChildIndex:
        {
          // nothing to do here
        }
        break;

      // -------------------------------------------------------------------------------------------
      case ccFeedInputs:
        blockRef = &currentModule->m_vardecl[msFeedInputs];
        comp->m_contextData = blockRef;
        break;
      case ccInputs:
        blockRef = &currentModule->m_vardecl[msInputs];
        comp->m_contextData = blockRef;
        break;
      case ccUserInputs:
        blockRef = &currentModule->m_vardecl[msUserInputs];
        comp->m_contextData = blockRef;
        break;
      case ccFeedOutputs:
        blockRef = &currentModule->m_vardecl[msFeedOutputs];
        comp->m_contextData = blockRef;
        break;
      case ccOutputs:
        blockRef = &currentModule->m_vardecl[msOutputs];
        comp->m_contextData = blockRef;
        break;

      // -------------------------------------------------------------------------------------------
      default:
      case ccInvalid:
      case ccUnknown:
        {
          // we hit something we didn't understand!
          cp.toString(gExceptionSB, itemIndex);
          throw(new ConnectivityException(&def, cpLineDefined, gExceptionSB.c_str(), 
            "Encountered invalid path item type"));
        }
        break;
    }

    itemIndex ++;
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
// takes an input path containing an array expansion token [*] and creates the expanded paths from it;
// this will be called recursively, for paths with >1 [*] token that need expanding
//----------------------------------------------------------------------------------------------------------------------
bool ModuleAnalysis::createArrayExpansions(const ModuleDef::Connection::Path& inputPath, ModuleDef::Connection::Paths& expandedPaths)
{
  ModuleDef::Connection::Paths expandedLocal;

  unsigned int compIndexModify = 0;
  iterate_begin_const_ptr(ModuleDef::Connection::Path::Component, inputPath.m_components, comp)
  {
    // this component has a [*] accessor
    if (comp->isType(ModuleDef::Connection::Path::Component::cExpandArray))
    {
      // work out how big an array the component is referring to
      int expansionSize = -1;
      switch (comp->m_context)
      {
        // owner.arms[*]
        case ccModuleName:
        {
          const ModuleDef::Child* ch = (const ModuleDef::Child*)comp->m_contextData;
          assert(ch);
          expansionSize = ch->m_arraySize;
        }
        break;
        
        // out.modules[*]
        case ccVariableName:
        {
          const ModuleDef::VarDecl* vd = (const ModuleDef::VarDecl*)comp->m_contextData;
          assert(vd);
          expansionSize = vd->m_arraySize;
        }
        break;

        default:
          // no other component type / context mix permissable
          assert(0);
          break;
      }
      assert(expansionSize > 0);

      // for each expansion required:
      // * copy the whole path
      // * modify the var[*] component to be a var[0], var[1], etc for each iteration
      for (int i=0; i<expansionSize; i++)
      {
        // copy this path
        ModuleDef::Connection::Path* copyPath = inputPath.deepCopy();

        // get the component of the copied path equivalent to the one we're currently iterator across
        ModuleDef::Connection::Path::Component* compMod = copyPath->m_components[compIndexModify];

        // swap in a new array accessor, replacing the expansion one
        parserMemoryAllocArgs(ModuleDef::Connection::Path::ComponentArrayAccess, replComp, compMod->m_text.c_str());
        replComp->m_arrayIndex = i;
        replComp->m_context = compMod->m_context;
        replComp->m_contextData = compMod->m_contextData;
        copyPath->m_components[compIndexModify] = replComp;

        // nix this, as it is now detatched from the DB
        compMod->m_type = (ModuleDef::Connection::Path::Component::Type)-1;

        // store this path
        expandedLocal.push_back(copyPath);
      }

      // only process a single [*] in a given run of this function
      // the code below then recursively calls this again and again until all [*] tokens are nailed
      break;
    }

    compIndexModify ++;
  }
  iterate_end

  if (!expandedLocal.empty())
  {   
    // replace expanded paths with further expansions?
    iterate_begin_ptr(ModuleDef::Connection::Path, expandedLocal, expPath)
    {
      // if this fn returns false, it means there were no expansions, so we can 
      // copy the final results into the expandedPaths output variable
      if (!createArrayExpansions(*expPath, expandedPaths))
        expandedPaths.push_back(expPath);
    }
    iterate_end

    // we created expanded paths
    return true;
  }

  // no expansions found
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// process all the source paths in a connection, perform target calculation and array expansion, etc.
//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::calculateAndExpandSourcePaths(
  ModuleDef::Connection& con, 
  ModuleDef& def,
  ModuleDef::ConnectionSet& set,
  ModuleDependencies* moduleDependencyTable)
{
  // -----------------------------------------------------------------------------------------------------------
  // Phase 1 - we 'execute' the source paths so that they are properly configured with accurate contexts, targets
  // and pointers back to data in the rest of the module database. we need all this information before handling
  // array expansion, as crucially we need to know the sizes of arrays referred to by [*] accessors. 
  iterate_begin_ptr(ModuleDef::Connection::Path, con.m_paths, path)
  {
    // 'execute' the path to find what it is pointing at
    calculateConnectionPath(*path, con.m_lineDefined, def, set, moduleDependencyTable);
  }
  iterate_end

  // -----------------------------------------------------------------------------------------------------------
  // Phase 2 - walk the paths again, this time we are hunting for [*] array accessors. When found they are expanded
  // into multiple copies of the same path, modified with a literal index and then stored back in the path list. eg.
  //
  // owner.arms[*].out.targetPos    ->  owner.arms[0].out.targetPos
  //                                    owner.arms[1].out.targetPos
  //                                    owner.arms[2].out.targetPos

  // first we cache the current list of paths, as we will be rebuilding it with expanded additions 
  ModuleDef::Connection::Paths pathCache, pathExpansions;
  pathCache.assign(con.m_paths.begin(), con.m_paths.end());

  // clear the current path list
  con.m_paths.clear();

  iterate_begin_ptr(ModuleDef::Connection::Path, pathCache, path)
  {
    pathExpansions.clear();

    // try expanding this path - if this returns true, it means expansions were created and placed in 'pathExpansions'
    if (!createArrayExpansions(*path, pathExpansions))
    {
      // .. if false, it means this path has no [*] tokens and can be added back to the path list
      con.m_paths.push_back(path);
    }
    else
    {
      // copy the expanded paths into the list, calling 'calculateConnectionPath' on each new path to ensure
      // that it's internal context/target data is valid
      iterate_begin_ptr(ModuleDef::Connection::Path, pathExpansions, expPath)
      {
        con.m_paths.push_back(expPath);
        calculateConnectionPath(*expPath, con.m_lineDefined, def, set, moduleDependencyTable);
      }
      iterate_end
    }
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
// secondary processing for module defs; binds modules together (looking up ModuleDef instances from names, etc), 
// creates limb instances, constructs the virtual network topology, expand gather-operation connections into 
// the appropriate pile of resultant junctions, generate 'tasks' for all the connections in every module connection set..
// .. and probably other things too. It does a lot.
//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::calculateConnectivity(ModuleDefs& defs, LimbInstances& limbInsts, ModuleInstance **topoInstance)
{
  // ------------------------------------------------------------------------------
  // phase one - link up the module owners/children and do first-pass resolution of
  //              connections, creating junction objects as we go. these then get
  //              wired together in the second pass.

  ModuleDef *rootModule = 0;
  iterate_begin_ptr(ModuleDef, defs, def)
  {
    // link up module owners
    if (!def->m_moduleOwner.empty())
    {
      unsigned int nameHash = def->m_moduleOwner.hashValueA();

      // lookup owner
      data->moduleDefNameMap.find(nameHash, &def->m_moduleOwnerPtr);
      if (def->m_moduleOwnerPtr == 0)
      {
        throw(new ConnectivityException(def, 1, def->m_moduleOwner.c_str(), 
          "Could not find named module owner in network"));
      }

      // calculate the owner's class name for caching purposes
      data->sb.reset();
      data->sb.appendPString(def->m_moduleOwner);
      if (def->m_moduleOwnerPtr->m_connectionSets.size() != 0 &&
          !def->m_moduleOwnerSku.empty())
      {
        data->sb.appendChar('_');
        data->sb.appendPString(def->m_moduleOwnerSku);
      }
      def->m_moduleOwnerClassName = data->sb.c_str();
    }
    else
    {
      def->m_moduleOwnerPtr = 0;
      assert(rootModule == 0);
      rootModule = def;
    }

    // fill in child pointers similarly
    iterate_begin(ModuleDef::Child, def->m_children, child)
    {
      unsigned int nameHash = child.m_module.hashValueA();

      // lookup child
      data->moduleDefNameMap.find(nameHash, &child.m_modulePtr);
      if (child.m_modulePtr == 0)
      {
        throw(new ConnectivityException(def, child.m_lineDefined, child.m_module.c_str(), 
          "Could not find named child in module"));
      }

      bool validChildConnectionSkuFound = false;
      
      // allow linking to child with no connections if desired sku is "default"
      if (child.m_modulePtr->m_connectionSets.size() == 0 && child.m_sku.empty())
        validChildConnectionSkuFound = true;

      iterate_begin_ptr(ModuleDef::ConnectionSet, child.m_modulePtr->m_connectionSets, set)
      {
        if (set->m_name == child.m_sku)
        {
          validChildConnectionSkuFound = true;
          set->m_instantiated = true;
        }
      }
      iterate_end

      if (!validChildConnectionSkuFound)
      {
        throw(new ConnectivityException(def, child.m_lineDefined, child.m_sku.c_str(), 
          "Child does not have compatible connection sku"));
      }
    }
    iterate_end 
  }
  iterate_end

  instantiateNetworkTopology(rootModule, topoInstance);


  // -------------------------------------------------------------------------------------------------------------------
  // phase two - handle gather operations, special cases that expand into multiple create/assign tasks
  //              these are done first so they can be generically processed by the task generator below

  iterate_begin_ptr(ModuleDef, defs, def)
  {
    iterate_begin_ptr(ModuleDef::ConnectionSet, def->m_connectionSets, set)
    {
      ModuleDef::Connections newGatherConnections;
      
      iterate_begin_ptr(ModuleDef::Connection, set->m_connectionDefs, con)
      {
        if (con->m_task == ctGatherOp)
        {         
          // process the source paths, preparation for cutting this connection up into 
          // multiple junction/assignments
          calculateAndExpandSourcePaths(*con, *def, *set, 0);

          // process the target
          calculateConnectionPath(*con->m_target, con->m_lineDefined, *def, *set, 0);

          ModuleDef::ConnectionPathTarget &targetResult = con->m_target->m_target;

          // test for parsing impossibilities (or developer infallibility)
          assert(targetResult.m_junction == 0);
          assert(targetResult.m_expandToMultiple == -1);

#ifdef _DEBUG
          ModuleDef::Connection::Path::Component* targetArray = static_cast<ModuleDef::Connection::Path::Component*>(targetResult.m_varDecl.m_arrayElementAccess); sizeof(targetArray);
          assert(targetArray->isType(ModuleDef::Connection::Path::Component::cExpandArray));
#endif

          // ensure the target is [a] an array and [b] of the right size
          if (targetResult.m_varDecl.m_varDeclPtr->m_arraySize == 0)
          {
            throw(new ConnectivityException(def, con->m_lineDefined, targetResult.m_varDecl.m_varDeclPtr->m_varname.c_str(), 
              "Gather operation must target an array variable"));
          }
          if (targetResult.m_varDecl.m_varDeclPtr->m_arraySize < con->m_paths.size())
          {
            gExceptionSB.reset();
            gExceptionSB.appendf("Gather operation target array (%i) must be at least as large as the number of input sources (%i)", 
              targetResult.m_varDecl.m_varDeclPtr->m_arraySize,
              con->m_paths.size());

            throw(new ConnectivityException(def, con->m_lineDefined, targetResult.m_varDecl.m_varDeclPtr->m_varname.c_str(), 
              gExceptionSB.c_str()));
          }

          // formulate a junction name prefix for the expanded items
          data->sb.reset();
          data->sb.appendf("junc_%s_%s_", 
            data->keywords->getModuleSectionLabel(targetResult.m_varDecl.m_varDeclPtr->m_ownerBlock).c_str(), 
            targetResult.m_varDecl.m_varDeclPtr->m_varname.c_str());
          data->sb.save();

          // create a directInput junction + assignment for every entry in the source path list
          int arrayAssignmentIndex = 0;
          iterate_begin_ptr(ModuleDef::Connection::Path, con->m_paths, path)
          {
            // update junction name with the current array index
            data->sb.restore();
            data->sb.appendInt(arrayAssignmentIndex);

            // create a junction path of type 'junctions.<junc_name>'
            ModuleDef::Connection::Path *autoJuncPath;
            {
              parserMemoryAllocDirect(ModuleDef::Connection::Path, autoJuncPath);
              {
                parserMemoryAllocArgs(ModuleDef::Connection::Path::Component, comp, "junctions");
                comp->m_context = data->keywords->toConnectionContext(comp->m_text);
                autoJuncPath->m_components.push_back(comp);
              }
              {
                parserMemoryAllocArgs(ModuleDef::Connection::Path::Component, comp, data->sb.c_str());
                comp->m_context = data->keywords->toConnectionContext(comp->m_text);
                autoJuncPath->m_components.push_back(comp);
              }
              autoJuncPath->generateHashKey();
            }

            // get the variable target, ensure it is as we expect
            ModuleDef::Connection::Path *clonedTarget = con->m_target->deepCopy();
            const ModuleDef::Connection::Path::Component* targetComp = clonedTarget->m_components.back();
            {
              assert(targetComp->isType( ModuleDef::Connection::Path::Component::cExpandArray ));

              // create an array access component thing[n]
              parserMemoryAllocArgs(ModuleDef::Connection::Path::ComponentArrayAccess, replacementArrayComp, targetComp->m_text.c_str());
              replacementArrayComp->m_context = targetComp->m_context;
              replacementArrayComp->m_arrayIndex = arrayAssignmentIndex ++;

              // swap in the array target component
              clonedTarget->m_components.pop_back();
              clonedTarget->m_components.push_back(replacementArrayComp);   
            }

            // create a new connection, to create a new junction for this array element
            {
              parserMemoryAllocArgs(ModuleDef::Connection, cd, con->m_lineDefined, ctCreateJunction);
              cd->m_newJunctionMode = jmDirectInput;

              ModuleDef::Connection::Path *clonedSource = path->deepCopy();

              // junctions.junc_block_var_n = directInput(source.path[n])
              cd->m_paths.push_back(clonedSource);
              cd->m_target = autoJuncPath;

              validateConnectionPath(*def, *con, *clonedSource);
              validateConnectionPath(*def, *con, *cd->m_target);

              newGatherConnections.push_back(cd);
            }
            // ... and then bind to the array element
            {
              parserMemoryAllocArgs(ModuleDef::Connection, cd, con->m_lineDefined, ctAssignJunction);

              // source.path[n] = junctions.junc_block_var_n;
              cd->m_paths.push_back(autoJuncPath);
              cd->m_target = clonedTarget;

              validateConnectionPath(*def, *con, *autoJuncPath);
              validateConnectionPath(*def, *con, *cd->m_target);

              newGatherConnections.push_back(cd);
            }
          }
          iterate_end
        }
      }
      iterate_end

      // if we created new connections as part of the gather-op processing, push them to the end of the defs list
      // and then re-sort so that create/assign are in the right order (and the gather op results are sorted into
      // the right place, by line number)
      if (!newGatherConnections.empty())
      {
        set->m_connectionDefs.insert(set->m_connectionDefs.end(), newGatherConnections.begin(), newGatherConnections.end());
        nmtl::sort(set->m_connectionDefs.begin(), set->m_connectionDefs.end(), ModuleDef::Connection::SortByTask);
      }

    }
    iterate_end
  }
  iterate_end


  // -------------------------------------------------------------------------------------------------------------------
  // phase three - turn connection defs into tasks

  iterate_begin_ptr(ModuleDef, defs, def)
  {
    iterate_begin_ptr(ModuleDef::ConnectionSet, def->m_connectionSets, set)
    {
      iterate_begin_ptr(ModuleDef::Connection, set->m_connectionDefs, con)
      {
        // -----------------------------------------------------------------------------------------
        // Create Junction
        // eg. junctions.avg01 = average(betaInst.out.angleValue, gammaInst.out.angleValue);
        // 
        // we register a new junction &  discover what needs to be plugged into it
        //
        if (con->m_task == ctCreateJunction)
        {
          // get the name of our new junction
          const ModuleDef::Connection::Path::Component* jName = con->m_target->m_components.back();
          if (jName->m_context != ccJunctionName)
          {
            throw(new ConnectivityException(def, con->m_lineDefined, jName->m_text.c_str(), 
              "Expected junction name as last component of connection target"));
          }

          // add a new junction to our pile
          parserMemoryAllocArgs(ModuleDef::Junction, newJunc, con->m_lineDefined, con->m_newJunctionMode, jName->m_text, set->m_name);

          newJunc->m_ownerModule = def;
          newJunc->m_ownerSet = set;

          set->m_junctions.push_back(newJunc);

          // create a 'new junction' task to execute
          set->m_newJunctionTasks.push_back(ModuleDef::ConnectionSet::NewJunctionTask(newJunc));

          // -----------------------------------------------------------------------------------------------------------
          calculateAndExpandSourcePaths(*con, *def, *set, &newJunc->m_moduleReferences);

          // -----------------------------------------------------------------------------------------------------------
          // do final post-processing work on the paths, now that we have all the expanded ones in place
          ModuleDef::VarDecl::Target representativeVarDecl;
          iterate_begin_ptr(ModuleDef::Connection::Path, con->m_paths, path)
          {
            // mark junction as in-use if it is used as a connection source
            if (path->m_target.m_junction)
              path->m_target.m_junction->m_unused = false;

            // if we have already processed a connection source, we will know what type/access parameters
            // that all other sources need to adhere to - so check that they can connect
            if (representativeVarDecl.m_varDeclPtr)
            {
              ModuleDef::VarDecl::Target *typeToTest = 0;
              if (path->m_target.m_junction)
              {
                typeToTest = &path->m_target.m_junction->m_representativeVarDecl;
              }
              else
              {
                typeToTest = &path->m_target.m_varDecl;
              }
              assert(typeToTest);

              // ensure variable types are compatible
              const char* incompatReason = ModuleDef::VarDecl::varDeclTargetsConnectable(*typeToTest, representativeVarDecl);
              if (incompatReason)
              {
                if (path->m_target.m_junction)
                {
                  gExceptionSB.reset();
                  gExceptionSB.appendPString(path->m_target.m_junction->m_name);
                }
                else
                {
                  path->m_target.m_varDecl.m_varDeclPtr->toString(gExceptionSB);
                  gExceptionSB.prependf("{%s} ", path->m_target.m_moduleTarget.m_def->m_name.c_str());
                }

                gExceptionSB.appendCharBuf(" (");
                gExceptionSB.appendCharBuf(incompatReason);
                gExceptionSB.appendCharBuf(")");

                throw(new ConnectivityException(def, con->m_lineDefined, gExceptionSB.c_str(), 
                  "connection sources are incompatible"));
              }
            }

            // pointing to a junction outside this module? calculateConnectionPath() should disallow this, i'm really
            // just asserting this to help document the process
            if (path->m_target.m_junction != 0 &&
                path->m_target.m_moduleTarget.m_def != def)
            {
              throw(new ConnectivityException(def, con->m_lineDefined, path->m_target.m_junction->m_name.c_str(), 
                "Impossible connection to junction in other module"));
            }

            // copy the target data that was returned from calculateConnectionPath()
            newJunc->m_connectionSources.push_back(ModuleDef::Junction::ConnectionSource(path->m_target.m_moduleTarget));
            ModuleDef::Junction::ConnectionSource &ct = newJunc->m_connectionSources.back();
            ct.m_junction = path->m_target.m_junction;
            if (!ct.m_junction)
              ct.m_varDecl = path->m_target.m_varDecl;
            
            // if a connection source element required expansion, store the fact with the junction in question (as it
            // will be the thing that gets expanded into multiple things) 
            if (path->m_target.m_expandToMultiple != -1)
            {
              // junction not expanding yet, can set this new value
              if (newJunc->m_expandToMultiple == -1)
              {
                newJunc->m_expandToMultiple = path->m_target.m_expandToMultiple;
              }
              // junction already marked to expand; check we have the same value, otherwise that is invalid...
              else if (newJunc->m_expandToMultiple > path->m_target.m_expandToMultiple)
              {
                throw(new ConnectivityException(def, con->m_lineDefined, newJunc->m_name.c_str(), 
                  "Cannot have source array larger than target array length"));
              }
            }

            ct.m_sourcePath = path;

            if (representativeVarDecl.m_varDeclPtr == 0)
            {
              if (ct.m_junction)
              {
                representativeVarDecl = ct.m_junction->m_representativeVarDecl;
              }
              else
              {
                representativeVarDecl = ct.m_varDecl;
              }
              assert(representativeVarDecl.m_varDeclPtr);
            }
          }
          iterate_end

          assert(representativeVarDecl.m_varDeclPtr);
          newJunc->m_representativeVarDecl = representativeVarDecl;

          if (newJunc->m_connectionSources.size() == 1)
            newJunc->m_mode = jmDirectInput;

          con->m_target->m_target.m_junction = newJunc;
        }

        // -----------------------------------------------------------------------------------------
        // Assign Junction
        // eg. feedIn.angleValue = junctions.pri01;
        // 
        // assign the output of a junction to a module member variable
        //
        else if (con->m_task == ctAssignJunction)
        {
          const ModuleDef::Connection::Path::Component* targetComp = con->m_target->m_components.back();

          // this should have already been ensured during path pre-processing, but no harm in asserting the fact
          if (targetComp->m_context != ccVariableName)
          {
            throw(new ConnectivityException(def, con->m_lineDefined, targetComp->m_text.c_str(), 
              "Can only assign junctions to module member variables"));
          }

          // check the target hasn't got an expansion operator [*]
          if (targetComp->isType(ModuleDef::Connection::Path::Component::cExpandArray))
          {
            throw(new ConnectivityException(def, con->m_lineDefined, targetComp->m_text.c_str(), 
              "Cannot specify array expansion in target variable"));
          }
          else if (targetComp->isType(ModuleDef::Connection::Path::Component::cExpandArray))
          {
            throw(new ConnectivityException(def, con->m_lineDefined, targetComp->m_text.c_str(), 
              "Cannot specify array expansion in target variable"));
          }

          if (con->m_paths.size() != 1)
          {
            throw(new ConnectivityException(def, con->m_lineDefined, targetComp->m_text.c_str(), 
              "Too many source paths when assigning junction"));
          }

          // get the path to the junction (should be 2 components long)
          const ModuleDef::Connection::Path *srcPath = con->m_paths.front();

          // get the name of our junction to assign
          const ModuleDef::Connection::Path::Component* jName = srcPath->m_components.back();
          if (jName->m_context != ccJunctionName)
          {
            throw(new ConnectivityException(def, con->m_lineDefined, jName->m_text.c_str(), 
              "Expected junction name as last component of source path"));
          }

          // create a new task
          ModuleDef::Junction* asnJnc = set->lookupJunctionByName(jName->m_text);
          if (asnJnc == 0)
          {
            throw(new ConnectivityException(def, con->m_lineDefined, jName->m_text.c_str(), 
              "could not find named junction"));
          }

          ModuleDef::ConnectionSet::AssignJunctionTask ajt(asnJnc);

          // work out where what plugging this thing into
          calculateConnectionPath(*con->m_target, con->m_lineDefined, *def, *set, &asnJnc->m_moduleReferences);
          ModuleDef::ConnectionPathTarget &targetResult = con->m_target->m_target;

          assert(targetResult.m_junction == 0);

          ajt.m_targetVarDecl = targetResult.m_varDecl;
          ajt.m_localDataOnly = (targetResult.m_moduleTarget.m_def == def);

          // tag the module section that the target variable lives in as having junction assignments
          ajt.m_targetVarDecl.m_varDeclPtr->m_modulePtr->m_varDeclBlockHasJunctionAssignments[ajt.m_targetVarDecl.m_varDeclPtr->m_ownerBlock] = true;
          

          // if a connection target requires expansion, store the fact with the junction in question (as it
          // will be the thing that gets expanded into multiple things) 
          if (targetResult.m_expandToMultiple != -1)
          {
            // junction not expanding yet, can set this new value
            if (asnJnc->m_expandToMultiple == -1)
            {
              asnJnc->m_expandToMultiple = targetResult.m_expandToMultiple;
            }
            // junction already marked to expand; check we have the same value, otherwise that is invalid...
            else if (asnJnc->m_expandToMultiple > targetResult.m_expandToMultiple)
            {
              throw(new ConnectivityException(def, con->m_lineDefined, asnJnc->m_name.c_str(), 
                "Cannot have source array larger than target array length"));
            }
          }

          // make sure combine target is compatible with 'expandToMultiple' value
          // eg. ensure that if we are iterating arrays in junction sources, we must have an array as a target
          // NOTE: we CAN have an iterator as a target WITHOUT arrays in the sources; just blends the same thing. unlikely but valid.
          if (asnJnc->m_expandToMultiple != -1)
          {
            if ((int)ajt.m_targetVarDecl.m_varDeclPtr->m_arraySize < asnJnc->m_expandToMultiple) // MORPH-21326: Check whether to use != or <
            {
              ajt.m_targetVarDecl.m_varDeclPtr->toString(gExceptionSB);

              throw(new ConnectivityException(def, con->m_lineDefined, gExceptionSB.c_str(), 
                "Junction assignment is iterating across array variables, but target is not an array"));
            }
          }

          // ensure the type returned by the junction can connect to the target variable
          const char* incompatReason = ModuleDef::VarDecl::varDeclTargetsConnectable(ajt.m_junction->m_representativeVarDecl, ajt.m_targetVarDecl);
          if (incompatReason)
          {
            ajt.m_targetVarDecl.m_varDeclPtr->toString(gExceptionSB);

            gExceptionSB.appendCharBuf(" (");
            gExceptionSB.appendCharBuf(incompatReason);
            gExceptionSB.appendCharBuf(")");

            throw(new ConnectivityException(def, con->m_lineDefined, gExceptionSB.c_str(), 
              "Cannot connect junction to this variable"));
          }

          // check two junctions aren't vying for control of a single variable
          if (!ajt.m_targetVarDecl.m_varDeclPtr->m_connectedJunctions.add(ajt.m_junction))
          {
            ajt.m_targetVarDecl.m_varDeclPtr->toString(gExceptionSB);
            throw(new ConnectivityException(def, con->m_lineDefined, gExceptionSB.c_str(), 
              "Variable already has this junction connected"));
          }

          set->m_assignJunctionTask.push_back(ajt);
        }
      }
      iterate_end
    }
    iterate_end

  }
  iterate_end 


  // -------------------------------------------------------------------------------------------------------------------
  // phase three - find limbs, sort them on module name alphabetically

  recurseAndCreateLimbInstances(*topoInstance, limbInsts);

  LimbSorter lsort;
  nmtl::sort(limbInsts.begin( ), limbInsts.end( ), lsort);


  // -------------------------------------------------------------------------------------------------------------------
  // phase four - calculate dependencies

  iterate_begin_ptr(ModuleDef, defs, def)
  {
    ParsedTypeDependencies::const_iterator it = def->m_parsedTypeDependencies.begin();
    ParsedTypeDependencies::const_iterator end = def->m_parsedTypeDependencies.end();
    for (; it != end; ++it)
    {
      const ParsedTypeDependency& td = (*it);
      td.m_vdc.getNamespaceFromIndices(gExceptionSB);

      const Type* vdt = data->typesRegistry->lookupByName(gExceptionSB.c_str(), td.m_vdc.m_typespec.getLast());
      if (!vdt)
      {
        throw(new ConnectivityException(def, td.m_vdc.m_lineDefined, td.m_vdc.m_typespec.getLast().c_str(), 
          "Unknown type found in module dependency"));
      }

      def->m_typeDependencies.registerDependentType(vdt, td.m_ptrOrRef);
    }

    // add the owner to the dependency lists; we cannot execute a child
    // until the parent has been updated
    ModuleDef* parentModule = def->m_moduleOwnerPtr;
    if (parentModule)
    {
      parserMemoryAlloc(ModuleBinding, binding);
      parserMemoryAlloc(ModuleBinding::BindOp, newOp);
      newOp->m_op = ModuleBinding::GetOwner;
      binding->m_ops.push_back(newOp);

      {
        parserMemoryAlloc(BoundModuleDependency, bmd);
        bmd->m_def = parentModule;
        bmd->m_binding = binding;

        def->m_updateExecutionDependencies.add(bmd);
      }

      {
        parserMemoryAlloc(BoundModuleDependency, bmd);
        bmd->m_def = parentModule;
        bmd->m_binding = binding->deepCopy();

        def->m_feedbackExecutionDependencies.add(bmd);
      }
    }

    nmtl::unique_vector<nmtl::hkey64> uniqueJunctionAssignments(16);

    // create list of modules that this module depends upon, from the connection paths
    iterate_begin_const_ptr(ModuleDef::ConnectionSet, def->m_connectionSets, set)
    {
      uniqueJunctionAssignments.clear();
      iterate_begin_const(ModuleDef::ConnectionSet::AssignJunctionTask, set->m_assignJunctionTask, ajt)
      {
        // Junction Assignments - check for duplicate assignments to the same variable
        // -------------------------------------------------------------------------------------------------------------
        // this code uses a unique_vector and generated string representations of junction variable assignments
        // to ensure that we are not writing into the same variable multiple times, overwriting values in a single step
        {
          data->sb.reset();

          // this code is similar to that found in the code generator; it handles the three types of target variable
          // addressing - expansion to an array of values (eg. targets[0], targets[1], targets[2]), explicit
          // addressing (eg. targets[5]) or simply by name, no array access required. 
          ModuleDef::VarDecl* vd = ajt.m_targetVarDecl.m_varDeclPtr;
          if (ajt.m_junction->m_expandToMultiple > 0)
          {
            for (int a=0; a<ajt.m_junction->m_expandToMultiple; a++)
            {
              gExceptionSB.reset();
              gExceptionSB.appendf("%s.%s[%i]", data->keywords->getModuleSectionName(vd->m_ownerBlock).c_str(), vd->m_varname.c_str(), a);

              // push the hashed value into the unique_vector; it will return false on a collision, meaning we have duplicate assignments
              if (!uniqueJunctionAssignments.add(gExceptionSB.asHash64()))
              {
                throw(new ConnectivityException(def, ajt.m_junction->m_lineDefined, gExceptionSB.c_str(), 
                  "Junctions are writing to this variable multiple times, overwriting each other"));
              }
            }
          }
          else
          {
            gExceptionSB.reset();
            if (ajt.m_targetVarDecl.m_arrayElementAccess)
            {
              ModuleDef::Connection::Path::Component* ajtAEA = static_cast<ModuleDef::Connection::Path::Component*>(ajt.m_targetVarDecl.m_arrayElementAccess);
              ajtAEA->formatArrayAccess(data->sb, true);
              gExceptionSB.appendf("%s.%s%s", data->keywords->getModuleSectionName(vd->m_ownerBlock).c_str(), vd->m_varname.c_str(), data->sb.c_str());
            }
            else
            {
              gExceptionSB.appendf("%s.%s", data->keywords->getModuleSectionName(vd->m_ownerBlock).c_str(), vd->m_varname.c_str());
            }

            // push the hashed value into the unique_vector; it will return false on a collision, meaning we have duplicate assignments
            if (!uniqueJunctionAssignments.add(gExceptionSB.asHash64()))
            {
              throw(new ConnectivityException(def, ajt.m_junction->m_lineDefined, gExceptionSB.c_str(), 
                "Junctions are writing to this variable multiple times, overwriting each other"));
            }
          }
        }

        // setup execution dependencies:
        // copy junction dependencies, depending where the result is going
        ModuleSection targetVarMS = ajt.m_targetVarDecl.m_varDeclPtr->m_ownerBlock;
        switch (targetVarMS)
        {
          case msInputs:
          case msUserInputs:
            {
              iterate_begin_const(ModuleDef::Junction::ConnectionSource, ajt.m_junction->m_connectionSources, jcs)
              {
                if (jcs.m_varDecl.m_varDeclPtr)
                {
                  ModuleSection obMs = jcs.m_varDecl.m_varDeclPtr->m_ownerBlock;
                  if (obMs == msOutputs || obMs == msInputs || obMs == msUserInputs)
                  {
                    if (jcs.m_moduleTarget.m_def == def ||
                      jcs.m_moduleTarget.m_binding->m_ops[0]->m_op == ModuleBinding::LookupChildByName)
                      continue;
                    
                    parserMemoryAlloc(BoundModuleDependency, bmd);
                    bmd->m_def = jcs.m_moduleTarget.m_def;
                    bmd->m_binding = jcs.m_moduleTarget.m_binding->deepCopy();

                    def->m_updateExecutionDependencies.add(bmd);
                  }
                }
              }
              iterate_end
            }
            break;

          case msOutputs:
            {
              iterate_begin_const(ModuleDef::Junction::ConnectionSource, ajt.m_junction->m_connectionSources, jcs)
              {
                if (jcs.m_varDecl.m_varDeclPtr)
                {
                  ModuleSection obMs = jcs.m_varDecl.m_varDeclPtr->m_ownerBlock;
                  if (obMs == msOutputs)
                  {
                    if (jcs.m_moduleTarget.m_def == def ||
                        jcs.m_moduleTarget.m_binding->m_ops[0]->m_op == ModuleBinding::LookupChildByName)
                      continue;

                    parserMemoryAlloc(BoundModuleDependency, bmd);
                    bmd->m_def = jcs.m_moduleTarget.m_def;
                    bmd->m_binding = jcs.m_moduleTarget.m_binding->deepCopy();

                    def->m_updateExecutionDependencies.add(bmd);
                  }
                }
              }
              iterate_end
            }
            break;

          case msFeedInputs:
            {
              iterate_begin_const(ModuleDef::Junction::ConnectionSource, ajt.m_junction->m_connectionSources, jcs)
              {
                if (jcs.m_varDecl.m_varDeclPtr)
                {
                  ModuleSection obMs = jcs.m_varDecl.m_varDeclPtr->m_ownerBlock;
                  if (obMs == msFeedOutputs || obMs == msFeedInputs)
                  {
                    if (jcs.m_moduleTarget.m_def == def ||
                      jcs.m_moduleTarget.m_binding->m_ops[0]->m_op == ModuleBinding::LookupChildByName)
                      continue;
                    
                    parserMemoryAlloc(BoundModuleDependency, bmd);
                    bmd->m_def = jcs.m_moduleTarget.m_def;
                    bmd->m_binding = jcs.m_moduleTarget.m_binding->deepCopy();

                    def->m_feedbackExecutionDependencies.add(bmd);
                  }
                }
              }
              iterate_end
            }
            break;

          case msFeedOutputs:
            {
              iterate_begin_const(ModuleDef::Junction::ConnectionSource, ajt.m_junction->m_connectionSources, jcs)
              {
                if (jcs.m_varDecl.m_varDeclPtr)
                {
                  ModuleSection obMs = jcs.m_varDecl.m_varDeclPtr->m_ownerBlock;
                  if (obMs == msFeedOutputs)
                  {
                    if (jcs.m_moduleTarget.m_def == def ||
                        jcs.m_moduleTarget.m_binding->m_ops[0]->m_op == ModuleBinding::LookupChildByName)
                      continue;

                    parserMemoryAlloc(BoundModuleDependency, bmd);
                    bmd->m_def = jcs.m_moduleTarget.m_def;
                    bmd->m_binding = jcs.m_moduleTarget.m_binding->deepCopy();

                    def->m_feedbackExecutionDependencies.add(bmd);
                  }
                }
              }
              iterate_end
            }
            break;
        }
      }
      iterate_end
    }
    iterate_end
  }
  iterate_end


  // -------------------------------------------------------------------------------------------------------------------
  // go check for unblendable types being used in incompatible junctions

  iterate_begin_ptr(ModuleDef, defs, def)
  {
    iterate_begin_const_ptr(ModuleDef::ConnectionSet, def->m_connectionSets, set)
    {
      iterate_begin_const(ModuleDef::ConnectionSet::AssignJunctionTask, set->m_assignJunctionTask, ajt)
      {
        // this type cannot be blended during combine()
        if (!ajt.m_targetVarDecl.m_varDeclPtr->m_knownType->isBlendableType())
        {
          // can the junction configuration support types that can't be blended? if not, bail out
          if (!ModuleDef::Junction::supportsUnblendableTypes(ajt.m_junction->m_mode))
          {
            throw(new ConnectivityException(def, ajt.m_junction->m_lineDefined, ajt.m_targetVarDecl.m_varDeclPtr->m_varname.c_str(), 
              "Junction mode does not support un-blendable type"));
          }
        }
      }
      iterate_end
    }
    iterate_end
  }
  iterate_end


  // create filtered dependencies - strip out those that are already satisfied by 
  // the module parent's own dependency list.
  //
  // eg Module X depends on Module Y
  //    Module A is a child of Module X, and also depends on Module Y
  //    But because X will have had to update by virtue of being Module A's parent..
  //    Module A need not concern itself with depending on Y, as A will have already ensured
  //        that Y ran before it updated itself.
  //
  //    .. clear as mud?
  //
  iterate_begin_ptr(ModuleDef, defs, def)
  {
    for (ModuleDependencies::iterator mIt = def->m_updateExecutionDependencies.begin(), 
          chEnd = def->m_updateExecutionDependencies.end(); 
          mIt != chEnd; 
          ++mIt)
    {
      bool shouldFilter = false;
      if (def->m_moduleOwnerPtr)
        shouldFilter = doParentModulesHaveThisDependency(def->m_moduleOwnerPtr, (*mIt)->m_def, nepUpdate);

      if (!shouldFilter)
      {
        def->m_reducedUpdateDependencies.add(*mIt);
      }
    }

    for (ModuleDependencies::iterator mIt = def->m_feedbackExecutionDependencies.begin(), 
          chEnd = def->m_feedbackExecutionDependencies.end(); 
          mIt != chEnd; 
          ++mIt)
    {
      bool shouldFilter = false;
      if (def->m_moduleOwnerPtr)
        shouldFilter = doParentModulesHaveThisDependency(def->m_moduleOwnerPtr, (*mIt)->m_def, nepFeedback);

      if (!shouldFilter)
      {
        def->m_reducedFeedbackDependencies.add(*mIt);
      }
    }
  }
  iterate_end

  createConnectionInstances(*topoInstance);
  processNetworkTopologyForDependencies(*topoInstance);
}

//----------------------------------------------------------------------------------------------------------------------
// walk the module instances looking for those that are marked with the __limb__ modifier
//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::recurseAndCreateLimbInstances(const ModuleInstance* instance, LimbInstances& limbInsts)
{
  static const char* kLimbNames[] = { 
    "Arm",
    "Head",
    "Leg",
    "Spine"
  };

  const ModifierOption* moLimb = hasModuleModifier(instance->m_module->m_modifiers, mmLimb);
  if (moLimb)
  {
    parserMemoryAlloc(LimbInstance, li);

    instance->m_module->m_limbIndex ++;
    li->m_inst = instance;
    li->m_index = instance->m_module->m_limbIndex;

    data->sb.reset();
    data->sb.appendf("%s_%i", instance->m_module->m_name.c_str(), li->m_index);
    li->m_name = data->sb.c_str();

    // Find the limb type from the name - if it can't be found it is "Unknown"
    int i = 0;
    for (; i < LimbInstance::kUnknown ; ++i)
    {
      if (strcmp(kLimbNames[i], instance->m_module->m_name.c_str())==0)
        break;
    }
    li->m_limbtype = LimbInstance::LimbType(i);

    limbInsts.push_back(li);
  }

  iterate_begin_const_ptr(ModuleInstance, instance->m_children, subMi)
  {
    recurseAndCreateLimbInstances(subMi, limbInsts);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
void buildConnectionDataForModuleInstance(ModuleInstance* mi)
{
  ModuleDef* def = mi->m_module;

  if (def->m_connectionSets.empty())
    return;

  // get the connection set that this instance would use at runtime
  const ModuleDef::ConnectionSet *set = 0;
  if (mi->m_sku.empty())
  {
    set = def->m_connectionSets[0];
  }
  else
  {
    set = def->lookupConnectionSetBySku(mi->m_sku);
  }
  assert(set);

  int jnGroup = 0;

  mi->m_connections.reserve(6);
  iterate_begin_const(ModuleDef::ConnectionSet::AssignJunctionTask, set->m_assignJunctionTask, ajt)
  {
    iterate_begin_const(ModuleDef::Junction::ConnectionSource, ajt.m_junction->m_connectionSources, jcs)
    {
      parserMemoryAlloc(ConnectionInstance, ci);

      ci->m_instanceTarget = mi;
      ci->m_varDeclTarget = &ajt.m_targetVarDecl;

      ci->m_junctionGroup = jnGroup;
      ci->m_junction = ajt.m_junction;

      ci->m_instanceSource = jcs.m_moduleTarget.m_binding->findCorrespondingInstance(mi);
      ci->m_varDeclSource = &jcs.m_varDecl;

      mi->m_connections.push_back(ci);
    }
    iterate_end

    jnGroup ++;
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
// instantiate the network, walking module children and generating a ModuleInstance for each. this provides an iteratable
// topology of modules that is used during dependency analysis / codegen
//----------------------------------------------------------------------------------------------------------------------
void recurseAndBuildModuleInstances(ModuleDef *def, ModuleInstance* curMI, const char* path)
{
  StringBuilder sb(16);
  if (!def->m_children.empty())
  {
    iterate_begin(ModuleDef::Child, def->m_children, chDef)
    {
      sb.reset();
      sb.appendf("%s%s", path, chDef.m_name.c_str());

      if (chDef.m_arraySize > 0)
      {
        sb.save();
        for (unsigned int i=0; i<chDef.m_arraySize; i++)
        {
          sb.restore();
          sb.appendf("_%i", i);

          parserMemoryAlloc(ModuleInstance, mi);
          mi->m_parent = curMI;
          mi->m_sku = chDef.m_sku;
          mi->m_name = chDef.m_name;
          mi->m_module = chDef.m_modulePtr;
          mi->m_NIID = sb.c_str();
          mi->m_isArrayInstance = true;
          mi->m_arrayIndex = i;
          curMI->m_children.push_back(mi);
          chDef.m_moduleInstances.push_back(mi);

          sb.appendf("_");

          recurseAndBuildModuleInstances(chDef.m_modulePtr, mi, sb.c_str());
        }
      }
      else
      {
        parserMemoryAlloc(ModuleInstance, mi);
        mi->m_parent = curMI;
        mi->m_sku = chDef.m_sku;
        mi->m_name = chDef.m_name;
        mi->m_module = chDef.m_modulePtr;
        mi->m_NIID = sb.c_str();
        mi->m_isArrayInstance = false;
        mi->m_arrayIndex = 0;
        curMI->m_children.push_back(mi);
        chDef.m_moduleInstances.push_back(mi);

        sb.appendChar('_');

        recurseAndBuildModuleInstances(chDef.m_modulePtr, mi, sb.c_str());
      }
    }
    iterate_end
  }
}

//----------------------------------------------------------------------------------------------------------------------
// perform a second sweep through the module instances, this time forming the instance-space dependencies
//----------------------------------------------------------------------------------------------------------------------
void recurseAndBuildModuleInstanceDependencies(ModuleInstance* mi, NetworkExcutionPhase nep)
{
  ModuleDependencies::const_iterator it, end;

  if (nep == nepUpdate)
  {
    it = mi->m_module->m_reducedUpdateDependencies.const_begin();
    end = mi->m_module->m_reducedUpdateDependencies.const_end(); 
  }
  else if (nep == nepFeedback)
  {
    it = mi->m_module->m_reducedFeedbackDependencies.const_begin();
    end = mi->m_module->m_reducedFeedbackDependencies.const_end(); 
  }
  else
  {
    assert(0);
  }

  for (; it != end; ++it)
  {
    BoundModuleDependency *bmd = (*it);

    // execute the module bindings in 'instance space' to find the instances that we depend upon
    ModuleInstance *curMi = mi, *miCache = 0;
    for (ModuleBinding::BindOps::const_iterator it = bmd->m_binding->m_ops.begin(),
                                                end = bmd->m_binding->m_ops.end();
                                                it != end;
                                                ++it)
    {
      ModuleBinding::BindOp *bo = (*it);

      switch (bo->m_op)
      {
        case ModuleBinding::GetOwner:
          miCache = curMi;
          curMi = curMi->m_parent;
          break;

        case ModuleBinding::LookupChildByName:
          {
            ModuleInstance* found = 0;
            iterate_begin_ptr(ModuleInstance, curMi->m_children, chMi)
            {
              if (chMi->m_name == bo->m_text)
              {
                if (chMi->m_isArrayInstance)
                {
                  if (chMi->m_arrayIndex == bo->m_arrayIndex)
                  {
                    found = chMi;
                    break;
                  }
                }
                else
                {
                  found = chMi;
                  break;
                }
              }
            }
            iterate_end

            assert(found);
            
            // miCache holds the instance encountered one-previous to the
            // last move - eg. 
            //
            //                  Hazard      Network
            // inputInstance -> owner(1) -> owner(2) -> bodyFrame -> twistMod
            // 
            // at the point of looking for bodyFrame, miCache will point to Hazard
            // at the point of looking for twistMod, miCache will still point to Hazard
            //
            // this captures cross-network references so that the top-level child
            // will know not to execute until those cross-network modules have done their business.
            //
            //
            if (miCache)
            {
              if (nep == nepUpdate)
                miCache->m_updateDependencies.add(found);
              else
                miCache->m_feedbackDependencies.add(found);
            }

            curMi = found;
          }
          break;
      }
    }

    if (nep == nepUpdate)
      mi->m_updateDependencies.add(curMi);
    else
      mi->m_feedbackDependencies.add(curMi);
  }

  // walk the children to do the same down the other instances
  iterate_begin_ptr(ModuleInstance, mi->m_children, chMi)
  {
    recurseAndBuildModuleInstanceDependencies(chMi, nep);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::instantiateNetworkTopology(ModuleDef *def, ModuleInstance** instance)
{
  parserMemoryAlloc(ModuleInstance, mi);
  mi->m_module = def;
  mi->m_NIID = "rootModule";
  recurseAndBuildModuleInstances(def, mi, "");

  *instance = mi;
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::createConnectionInstances(ModuleInstance* instance)
{
  buildConnectionDataForModuleInstance(instance);

  // walk the children to do the same down the other instances
  iterate_begin_ptr(ModuleInstance, instance->m_children, chMi)
  {
    createConnectionInstances(chMi);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::processNetworkTopologyForDependencies(ModuleInstance* instance)
{
  recurseAndBuildModuleInstanceDependencies(instance, nepUpdate);
  recurseAndBuildModuleInstanceDependencies(instance, nepFeedback);
}

//----------------------------------------------------------------------------------------------------------------------
// check to see if the module 'dep' is in the parent's dependencies, and so on up the chain until we hit the root module
//----------------------------------------------------------------------------------------------------------------------
bool ModuleAnalysis::doParentModulesHaveThisDependency(const ModuleDef* parent, const ModuleDef* dep, NetworkExcutionPhase nep)
{
  ModuleDependencies::const_iterator mIt, chEnd;
  
  if (nep == nepUpdate)
  {
    mIt = parent->m_updateExecutionDependencies.const_begin();
    chEnd = parent->m_updateExecutionDependencies.const_end();
  }
  else if (nep == nepFeedback)
  {
    mIt = parent->m_feedbackExecutionDependencies.const_begin();
    chEnd = parent->m_feedbackExecutionDependencies.const_end();
  }
  else
  {
    assert(0);
  }

  for (;mIt != chEnd; ++mIt)
  {
    const ModuleDef* localDep = (*mIt)->m_def;
    if (localDep == dep)
    {
      return true;
    }
  }

  if (parent->m_moduleOwnerPtr)
    return doParentModulesHaveThisDependency(parent->m_moduleOwnerPtr, dep, nep);
  else
    return false;
}

//----------------------------------------------------------------------------------------------------------------------
struct ExecutionInstance
{
  ExecutionInstance(ModuleInstance* inst, NetworkExcutionPhase nep) :
    m_inst(inst),
    m_dependencies(8, ActiveMemoryManager::getForNMTL())
  {
    if (nep == nepUpdate)
    {
      for (UniqueModuleInstances::const_iterator mIt = inst->m_updateDependencies.const_begin(), 
                                                 chEnd = inst->m_updateDependencies.const_end(); 
                                                 mIt != chEnd; 
                                                 ++mIt)
      {
        m_dependencies.add( (*mIt) );
      }
    }
    else if (nep == nepFeedback)
    {
      for (UniqueModuleInstances::const_iterator mIt = inst->m_feedbackDependencies.const_begin(), 
                                                 chEnd = inst->m_feedbackDependencies.const_end(); 
                                                 mIt != chEnd; 
                                                 ++mIt)
      {
        m_dependencies.add( (*mIt) );
      }
    }
    else
    {
      assert(0);
    }
  }

  ModuleInstance         *m_inst;
  UniqueModuleInstances   m_dependencies;
};
typedef nmtl::pod_vector<ExecutionInstance*> ExecutionInstances;

//----------------------------------------------------------------------------------------------------------------------
void recursiveAddExecutedChildren(UniqueModuleInstances* executedModules, ModuleInstances& insts)
{
  // add all the module instances in 'insts' to the executedModules table, and then recurse to their children too
  iterate_begin_ptr(ModuleInstance, insts, inst)
  {
    executedModules->add(inst);
    recursiveAddExecutedChildren(executedModules, inst->m_children);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
void recursiveBuildExecutionBuckets(ModuleInstance* mi, const UniqueModuleInstances* executedModules, NetworkExcutionPhase nep)
{
  // build a temporary list of child instances, used to 
  // hammer at the dependencies lists while sorting for execution order
  ExecutionInstances exInsts(ActiveMemoryManager::getForNMTL());
  iterate_begin_ptr(ModuleInstance, mi->m_children, chMi)
  {
    parserMemoryAllocArgs(ExecutionInstance, exI, chMi, nep);
    exInsts.push_back(exI);
  }
  iterate_end

  // we take a copy of the current list of executed modules, as it will be modified and
  // passed down the chain as we execute children
  UniqueModuleInstances* executedModulesForThisBranch = 0;
  if (executedModules)
  {
    parserMemoryAllocDirectArgs(UniqueModuleInstances, executedModulesForThisBranch, *executedModules);
  }
  else
  {
    parserMemoryAllocDirectArgs(UniqueModuleInstances, executedModulesForThisBranch, 64, ActiveMemoryManager::getForNMTL());
  }

  // add the current instance to the list of ones that have been executed. all
  // module children rely on their parent having been updated before they will be
  executedModulesForThisBranch->add(mi);

  for (;;)
  {
    parserMemoryAlloc(ExecutionBucket, execBucket);

    iterate_begin_ptr(ExecutionInstance, exInsts, exI)
    {
      bool defHasDeps = exI->m_dependencies.size() > 0;
      if (defHasDeps)
      {
        for (UniqueModuleInstances::iterator mIt = executedModulesForThisBranch->begin(),
                                             chEnd = executedModulesForThisBranch->end();
                                             mIt != chEnd; 
                                             ++mIt)
        {
          exI->m_dependencies.remove((*mIt));
        }

        if (exI->m_dependencies.size() == 0)
        {
          execBucket->m_instances.push_back(exI->m_inst);
        }
      }
    }
    iterate_end

    if (execBucket->m_instances.size() > 0)
    {
      iterate_begin_ptr(ModuleInstance, execBucket->m_instances, chMi)
      {
        recursiveBuildExecutionBuckets(chMi, executedModulesForThisBranch, nep);

        executedModulesForThisBranch->add(chMi);
        recursiveAddExecutedChildren(executedModulesForThisBranch, chMi->m_children);
      }
      iterate_end

      if (nep == nepUpdate)
        mi->m_updateBuckets.push_back(execBucket);
      else
        mi->m_feedbackBuckets.push_back(execBucket);
    }
    else
    {
      iterate_begin_ptr(ExecutionInstance, exInsts, exI)
      {
        // we have unresolved dependencies, this is Not Good
        if(exI->m_dependencies.size() > 0)
        {
          printf("ERROR: dependency ordering problem found (%s)\n", nep == nepUpdate ? "Update" : "Feedback");
          printf("       in module instance '%s'\n\n", exI->m_inst->m_NIID.c_str());
          
          for (UniqueModuleInstances::const_iterator mIt = exI->m_dependencies.const_begin(), 
                                                     chEnd = exI->m_dependencies.const_end(); 
                                                     mIt != chEnd; 
                                                     ++mIt)
          {
            printf("       + unresolved : '%s'\n", (*mIt)->m_NIID.c_str());

            UniqueModuleInstances::const_iterator dIt, dEnd;
            
            if (nep == nepUpdate)
            {
              dIt = (*mIt)->m_updateDependencies.const_begin(); 
              dEnd = (*mIt)->m_updateDependencies.const_end(); 
            }
            else
            {
              dIt = (*mIt)->m_feedbackDependencies.const_begin(); 
              dEnd = (*mIt)->m_feedbackDependencies.const_end(); 
            }

            // go check and see if the two are depending on each other
            for (;dIt != dEnd; ++dIt)
            {
              printf("         \\_ '%s'", (*dIt)->m_NIID.c_str());

              if ((*dIt)->m_NIID == exI->m_inst->m_NIID)
              {
                printf("              [CIRCULAR DEPENDENCY]");
              }
              printf("\n");
            }
          }
          printf("\n");
        }
      }
      iterate_end

      break;
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
// build the execution buckets for each ModuleInstance, defining the execution order of child instances for the
// update & feedback phase based on data dependency analysis.
//----------------------------------------------------------------------------------------------------------------------
void ModuleAnalysis::parallelismAnalysis(ModuleInstance* rootInstance)
{
  recursiveBuildExecutionBuckets(rootInstance, 0, nepUpdate);
  recursiveBuildExecutionBuckets(rootInstance, 0, nepFeedback);
}