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

#include "MDF.h"

struct ModuleAnalysisData;

/**
 * wrapper class around a few heavyweight functions that are responsible for validating the 
 * parsed module network data
 */
class ModuleAnalysis
{
public:

  ModuleAnalysis(const KeywordsManager* keywords, const TypesRegistry* typesRegistry);
  ~ModuleAnalysis();

  /**
   * called after parsing each module, provides post-processing on 
   * vardecls and initial tests to ensure they are properly defined
   */
  void validateModuleVarDecls(ModuleDef& def);


  /**
   * walk all definitions and test for any semantic errors not caught
   * during initial parse phase
   */
  void validateModuleDefs(ModuleDefs& defs);


  /**
   * create connectivity information, link modules together and ensure
   * dependencies are non-cyclic. During processing, limb instances are discovered
   * and stored in the given array.
   */
  void calculateConnectivity(ModuleDefs& defs, LimbInstances& limbInsts, ModuleInstance **topoInstance);
  /**
   * sorts modules into concurrent execution buckets
   */
  void parallelismAnalysis(ModuleInstance* rootInstance);

private:

  struct ConnectionPathTargetResult
  {
    ModuleDef                    *m_modulePtr;
    ModuleDef::VarDecl::Target    m_varDecl;
    ModuleDef::Junction          *m_junction;    // if null, this is a var decl target.. and vice versa
    int                           m_expandToMultiple;

    void clear()
    {
      m_modulePtr = 0;
      m_varDecl.m_arrayElementAccess = 0;
      m_varDecl.m_varDeclPtr = 0;
      m_junction = 0;
      m_expandToMultiple = -1;
    }
  };

  void calculateConnectionPath(
    ModuleDef::Connection::Path& cp,              // path to execute
    unsigned int cpLineDefined,                   // line where the path was defined, should we need to throw exceptions
    ModuleDef& def,                               // owning module definition
    ModuleDef::ConnectionSet& set,                // current connection set (for looking up other junctions)
    ModuleDependencies* moduleDependencyTable);

  void validateModuleVarDecl(
    ModuleDef& def, 
    ModuleDef::VarDecl& vd);

  void validateConnectionPath(
    ModuleDef& def, 
    ModuleDef::Connection& con, 
    ModuleDef::Connection::Path& cp);

  bool createArrayExpansions(
    const ModuleDef::Connection::Path& inputPath, 
    ModuleDef::Connection::Paths& expandedPaths);

  void calculateAndExpandSourcePaths(
    ModuleDef::Connection& con,
    ModuleDef& def,
    ModuleDef::ConnectionSet& set,
    ModuleDependencies* moduleDependencyTable);

  void recurseAndCreateLimbInstances(
    const ModuleInstance* instance,
    LimbInstances& limbInsts);

  void instantiateNetworkTopology(
    ModuleDef *def, 
    ModuleInstance** instance);

  void createConnectionInstances(
    ModuleInstance* instance);

  void processNetworkTopologyForDependencies(
    ModuleInstance* instance);

  bool doParentModulesHaveThisDependency(
    const ModuleDef* parent, 
    const ModuleDef* dep, 
    NetworkExcutionPhase nep);


  ModuleAnalysis(const ModuleAnalysis& rhs);
  ModuleAnalysis& operator = (const ModuleAnalysis& rhs);

  ModuleAnalysisData   *data; // pimpl
};
