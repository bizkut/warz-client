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

#include "Precompiled.h"
#include "Utils.h"
#include "CodeWriter.h"
#include "EmitCPP_Common.h"
#include "EmitCPP_Desc.h"

#include "NMPlatform/NMPlatform.h"

//----------------------------------------------------------------------------------------------------------------------
void recursiveWriteInstanceNIIDs(CodeWriter& code, const ModuleInstance* mi)
{
  code.write("%s,", mi->m_NIID.c_str());
  iterate_begin_const_ptr(ModuleInstance, mi->m_children, subMi)
  {
    recursiveWriteInstanceNIIDs(code, subMi);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
bool DescGen::writeDescriptorHeader(CodeWriter& code, const NetworkDatabase &ndb)
{
  HeaderGuard hg("NET_DESC");
  code.writeHeaderTop(hg);

  code.write("#include \"euphoria/erNetworkInterface.h\"");
  code.write("#include \"NMPlatform/NMMatrix34.h\"");
  code.newLine();

  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");

  code.writeHorizontalSeparator();
  code.write("struct NMBehaviourLibrary : public ER::NetworkInterface");
  code.openBrace();

  code.write("virtual const ER::NetworkInterface::Description& getDesc() const NM_OVERRIDE;");
  code.newLine();

  code.write("virtual void init() NM_OVERRIDE;");
  code.write("virtual ER::RootModule* createNetwork() const NM_OVERRIDE;");
  code.write("virtual bool destroyNetwork(ER::Module** rootModuleOut) const NM_OVERRIDE;");
  code.write("virtual void term() NM_OVERRIDE;");

  code.closeBraceWithSemicolon();
  code.newLine();

  //--------------------------------------------------------------------------------------------------------------------
  // network metrics; number of limbs and so on
  code.writeHorizontalSeparator();
  code.write("struct NetworkMetrics");
  code.openBrace();
  
  code.write("enum");
  code.openBrace();
  code.write("numLimbs = %i,", ndb.m_limbInstances.size());
  code.closeBraceWithSemicolon();

  code.write("enum LimbIndex");
  code.openBrace();

  iterate_begin_const_ptr(LimbInstance, ndb.m_limbInstances, limbInst)
  {
    code.write("%s,", limbInst->m_name.c_str());
  }
  iterate_end

  code.closeBraceWithSemicolon();
  code.newLine();

  code.write("const char* getLimbName(LimbIndex index) const;");

  code.closeBraceWithSemicolon();
  code.newLine();


  // body pose structure
  code.writeHorizontalSeparator();
  code.write("struct BodyPoseData");
  code.openBrace();
  code.write("NMP::Matrix34 m_poseEndRelativeToRoots[NetworkMetrics::numLimbs];");
  code.write("float m_poseWeights[NetworkMetrics::numLimbs];");
  code.write("float m_poseWeight;");
  code.closeBraceWithSemicolon();
  code.newLine();

  // network contents, indexing modules + behaviours
  code.writeHorizontalSeparator();
  code.write("struct NetworkManifest");
  code.openBrace();

  // build a flattened list of all the modules in the system
  code.write("enum Modules");
  code.openBrace();
  recursiveWriteInstanceNIIDs(code, ndb.m_instance);
  code.writeComment("-------------");
  code.write("NumModules");
  code.closeBraceWithSemicolon();
  code.newLine();

  // ditto behaviours
  code.write("enum Behaviours");
  code.openBrace();
  iterate_begin_const_ptr(BehaviourDef, ndb.m_behaviourDefs, bv)
  {
    code.write("BehaviourID_%s = %i,", bv->m_name.c_str(), bv->m_behaviourID);
  }
  iterate_end
  code.writeComment("-------------");
  code.write("NumBehaviours");
  code.closeBraceWithSemicolon();



  code.closeBraceWithSemicolon();
  code.newLine();

  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLine();

  code.writeHeaderBottom(hg);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DescGen::writeDescriptorCode(CodeWriter& code, const NetworkDatabase &ndb)
{
  const ModuleDef* rootInst = getRootModule(ndb.m_moduleDefs);
  bool hasBehaviours = (ndb.m_behaviourDefs.size() > 0);

  code.writeNMCopyrightHeader();

  code.write("#include \"NetworkDescriptor.h\"");
  code.write("#include \"%s.h\"", rootInst->m_name.c_str());
  iterate_begin_const_ptr(BehaviourDef, ndb.m_behaviourDefs, bv)
  {
    code.write("#include \"Behaviours/%sBehaviour.h\"", bv->m_name.c_str());
  }
  iterate_end
  code.newLine();

  // -----------------------------------------------------------------------------------------------
  // temporary DLL export of network interface
  code.write("#ifdef _WINDLL");
  code.write("static NM_BEHAVIOUR_LIB_NAMESPACE::NMBehaviourLibrary gNMBehaviourLibrary_%08X;", ndb.m_moduleNUID);
  code.write("__declspec(dllexport) ER::NetworkInterface* getNetworkInterface() { return &gNMBehaviourLibrary_%08X; }", ndb.m_moduleNUID);
  code.write("#endif // _WINDLL");
  code.newLine();

  // -----------------------------------------------------------------------------------------------
  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");

  if (hasBehaviours)
  {
    code.write("typedef ER::BehaviourDefs<%i> BehaviourDefsOut;", (int)ndb.m_behaviourDefs.size());
    code.newLine();
  }

  // -----------------------------------------------------------------------------------------------
  code.writeHorizontalSeparator();
  code.write("const ER::NetworkInterface::Description& NMBehaviourLibrary::getDesc() const");
  code.openBrace();
  code.writeTabs();
  code.writeRaw("const static ER::NetworkInterface::Description ndOut = { ");

  code.writeRaw("0x%08X", ndb.m_moduleNUID);

  code.writeRaw(" , NetworkMetrics::numLimbs };");
  code.newLine();

  code.write("return ndOut;");
  code.closeBrace();
  code.newLine();


  // -----------------------------------------------------------------------------------------------
  code.writeHorizontalSeparator();
  code.write("void NMBehaviourLibrary::init()");
  code.openBrace();
  code.newLine();

  code.write("NMP_ASSERT(m_defs == 0);");

  if (hasBehaviours)
  {
    code.write("BehaviourDefsOut *bDefs = (BehaviourDefsOut*)NMPMemoryAllocateFromFormat(BehaviourDefsOut::getMemoryRequirements()).ptr;");
    code.write("new (bDefs) BehaviourDefsOut();");
    code.write("m_defs = bDefs;");
    code.newLine();
  }
  else
  {
    code.write("m_defs = 0;");
  }

  unsigned int bvIndex = 0;
  iterate_begin_const_ptr(BehaviourDef, ndb.m_behaviourDefs, bv)
  {
    code.write("bDefs->m_defs[%i] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(%sBehaviourDef::getMemoryRequirements()).ptr;",
      bvIndex, bv->m_name.c_str());
    code.write("new (bDefs->m_defs[%i]) %sBehaviourDef();", bvIndex, bv->m_name.c_str());
    code.newLine();

    bvIndex ++;
  }
  iterate_end

  code.closeBrace();
  code.newLine();


  // -----------------------------------------------------------------------------------------------
  code.writeHorizontalSeparator();
  code.write("ER::RootModule* NMBehaviourLibrary::createNetwork() const");
  code.openBrace();

  code.writeComment("ensure ::init() has been called to configure the memory system");
  code.write("NMP_ASSERT(NMP::Memory::isInitialised());");
  code.newLine();

  code.writeComment("construct the network; new'ing the root module will create all of its children in turn");
  code.write("%s *ModuleInst = (%s*)NMPMemoryAllocateFromFormat(%s::getMemoryRequirements()).ptr;",
    rootInst->m_name.c_str(),
    rootInst->m_name.c_str(),
    rootInst->m_name.c_str());
  code.write("new (ModuleInst) %s();", 
    rootInst->m_name.c_str());
  code.write("ER::RootModule* rootModuleOut = ModuleInst;");
  code.newLine();

  code.write("return rootModuleOut;");
  code.closeBrace();
  code.newLine();

  // -----------------------------------------------------------------------------------------------
  code.writeHorizontalSeparator();
  code.write("bool NMBehaviourLibrary::destroyNetwork(ER::Module** rootModuleOut) const");
  code.openBrace();

  code.writeComment("ensure ::init() has been called to configure the memory system");
  code.write("NMP_ASSERT(NMP::Memory::isInitialised());");
  code.newLine();

  code.writeComment("destroy the network; free'ing the root module will free all of its children in turn");
  code.write("MyNetwork* rootNetwork = (MyNetwork*)*rootModuleOut;");
  code.write("rootNetwork->~MyNetwork();");
  code.write("NMP::Memory::memFree(rootNetwork);");
  code.write("(*rootModuleOut) = 0;");
  code.newLine();

  code.write("return true;");
  code.closeBrace();
  code.newLine();

  // -----------------------------------------------------------------------------------------------
  code.writeHorizontalSeparator();
  code.write("void NMBehaviourLibrary::term()");
  code.openBrace();

  if (hasBehaviours)
  {
    code.write("NMP_ASSERT(m_defs != 0);");
    code.write("BehaviourDefsOut *bDefs = (BehaviourDefsOut*)m_defs;");
    code.newLine();

    bvIndex = (int)ndb.m_behaviourDefs.size() - 1;
    iterate_begin_const_ptr_reverse(BehaviourDef, ndb.m_behaviourDefs, bv)
    {
      code.write("((%sBehaviourDef*)bDefs->m_defs[%i])->~%sBehaviourDef();", bv->m_name.c_str(), bvIndex, bv->m_name.c_str());
      code.write("NMP::Memory::memFree(bDefs->m_defs[%i]);", bvIndex);
      code.newLine();
      bvIndex --;
    }
    iterate_end

      code.write("bDefs->~BehaviourDefsOut();");
    code.write("NMP::Memory::memFree(bDefs);");
  }
  code.write("m_defs = 0;");

  code.newLine();
  code.write("ER::NetworkInterface::term();");
  code.closeBrace();
  code.newLine();


  // -----------------------------------------------------------------------------------------------
  code.writeHorizontalSeparator();
  code.write("const char* NetworkMetrics::getLimbName(LimbIndex index) const");
  code.openBrace();

  code.write("switch (index)");
  code.openBrace();

  iterate_begin_const_ptr(LimbInstance, ndb.m_limbInstances, limbInst)
  {
    code.write("case %s: return \"%s\"; ", limbInst->m_name.c_str(), limbInst->m_name.c_str());
  }
  iterate_end

  code.newLine();
  code.write("default:");
  code.write("NMP_ASSERT_FAIL();");
  code.write("break;");
  code.closeBrace();

  code.write("return 0;");
  code.closeBrace();

  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLine();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DescGen::writeNetworkConstants(CodeWriter& code, const NetworkDatabase &ndb)
{
  HeaderGuard hg("NET_CONSTANTS");
  code.writeHeaderTop(hg);

  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");

  //--------------------------------------------------------------------------------------------------------------------
  // write out any registered constants for use in the C++
  code.writeHorizontalSeparator();
  code.write("struct NetworkConstants");
  code.openBrace();

  code.write("enum");
  code.openBrace();

  NetworkConstants::const_value_walker consts = ndb.m_constants.constWalker();
  while (consts.next())
  {
    int constantValue = consts();
    code.write("%s = %i,", consts.key(), constantValue);
  }

  code.closeBraceWithSemicolon();

  code.closeBraceWithSemicolon();
  code.newLine();


  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLine();

  code.writeHeaderBottom(hg);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void writeModuleChildDataEnum(CodeWriter& code, ModuleSection ms, const char* moduleSectionString, const ModuleInstance *mi)
{
  if (mi->m_module->varDeclBlockIsNotEmpty(ms))
    code.write("%s%s inst_%s;", mi->m_module->m_name.c_str(), moduleSectionString, mi->m_NIID.c_str());

  iterate_begin_const_ptr(ModuleInstance, mi->m_children, subMi)
  {
    writeModuleChildDataEnum(code, ms, moduleSectionString, subMi);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
bool DescGen::writeDataMemoryHeader(CodeWriter& code, const NetworkDatabase &ndb)
{
  HeaderGuard hg("DATA_MEM");
  code.writeHeaderTop(hg);

  code.write("#include \"NMPlatform/NMPlatform.h\"");

  iterate_begin_const_ptr(ModuleDef, ndb.m_moduleDefs, def)
  {
    code.write("#include \"%sData.h\"", def->m_name.c_str());
  }
  iterate_end

  code.newLines(2);
  code.write("#ifdef NM_COMPILER_MSVC");
  code.write("# pragma warning ( push )");
  code.write("# pragma warning ( disable : 4324 ) // structure was padded due to __declspec(align()) ");
  code.write("#endif // NM_COMPILER_MSVC");
  code.newLine();

  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");

  for (int ms = msData; ms <= msOutputs; ms++)
  {
    code.writeHorizontalSeparator();
    code.write("NMP_ALIGN_PREFIX(32) struct DataMemory%s", ndb.m_keywords.getModuleSectionLabel((ModuleSection)ms).c_str());
    code.openBrace();

    writeModuleChildDataEnum(code, (ModuleSection)ms, ndb.m_keywords.getModuleSectionLabel((ModuleSection)ms).c_str(), ndb.m_instance);

    code.closeBrace(false);
    code.writeRaw(" NMP_ALIGN_SUFFIX(32);");
    code.newLines(2);
  }

  code.newLine();
  code.write("#ifdef NM_COMPILER_MSVC");
  code.write("# pragma warning ( pop )");
  code.write("#endif // NM_COMPILER_MSVC");
  code.newLine();

  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLine();

  code.writeHeaderBottom(hg);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DescGen::writeForceInclude(CodeWriter& code, const NetworkDatabase &)
{
  HeaderGuard hg("FORCE_INCLUDE");
  code.writeHeaderTop(hg);

  code.write("# define NM_BEHAVIOUR_LIB_NAMESPACE NMBipedBehaviours");
  code.newLine();

  code.writeHeaderBottom(hg);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void writePerModuleProfilingDeclarations(CodeWriter& code, const ModuleInstance *mi)
{
  code.write("TIMETYPE timer_update_%s[3] = {0,0,0};", mi->m_NIID.c_str());
  code.write("TIMETYPE timer_feedback_%s[3] = {0,0,0};", mi->m_NIID.c_str());

  iterate_begin_const_ptr(ModuleInstance, mi->m_children, subMi)
  {
    writePerModuleProfilingDeclarations(code, subMi);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
void writePerModuleUpdateProfilingPrintfs(CodeWriter& code, const ModuleInstance *mi)
{
  code.write("#ifdef EUPHORIA_PROFILING_DETAILED");
  code.write(
    "NMP_MSG(\"update %s, %%f, %%f, %%f, %%f\", "
    "timer_update_%s[0], timer_update_%s[1], timer_update_%s[2], "
    "timer_update_%s[0] + timer_update_%s[1] + timer_update_%s[2]);",
    mi->m_NIID.c_str(), mi->m_NIID.c_str(), mi->m_NIID.c_str(), mi->m_NIID.c_str(),
    mi->m_NIID.c_str(), mi->m_NIID.c_str(), mi->m_NIID.c_str(), mi->m_NIID.c_str());
  code.write("#endif // EUPHORIA_PROFILING_DETAILED");

  code.write("timer_update_total[0] += timer_update_%s[0];", mi->m_NIID.c_str());
  code.write("timer_update_total[1] += timer_update_%s[1];", mi->m_NIID.c_str());
  code.write("timer_update_total[2] += timer_update_%s[2];", mi->m_NIID.c_str());
  iterate_begin_const_ptr(ModuleInstance, mi->m_children, subMi)
  {
    writePerModuleUpdateProfilingPrintfs(code, subMi);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
void writeUpdateProfilingPrintfs(CodeWriter& code, const ModuleInstance *mi)
{
  code.write("timer_update_total[0] = timer_update_total[1] = timer_update_total[2] = 0.0f;");
  writePerModuleUpdateProfilingPrintfs(code, mi);
  code.write("#ifndef EUPHORIA_PROFILING_DETAILED");
  code.write(
    "NMP_MSG(\"update total, %%f, %%f, %%f, %%f\", "
    "timer_update_total[0], timer_update_total[1], timer_update_total[2], "
    "timer_update_total[0] + timer_update_total[1] + timer_update_total[2]);");
  code.write("#endif // EUPHORIA_PROFILING_DETAILED");
}

//----------------------------------------------------------------------------------------------------------------------
void writePerModuleFeedbackProfilingPrintfs(CodeWriter& code, const ModuleInstance *mi)
{
  code.write("#ifdef EUPHORIA_PROFILING_DETAILED");
  code.write(
    "NMP_MSG(\"feedback %s, %%f, %%f, %%f, %%f\", "
    "timer_feedback_%s[0], timer_feedback_%s[1], timer_feedback_%s[2], "
    "timer_feedback_%s[0] + timer_feedback_%s[1] + timer_feedback_%s[2]);",
    mi->m_NIID.c_str(), mi->m_NIID.c_str(), mi->m_NIID.c_str(), mi->m_NIID.c_str(),
    mi->m_NIID.c_str(), mi->m_NIID.c_str(), mi->m_NIID.c_str(), mi->m_NIID.c_str());
  code.write("#endif // EUPHORIA_PROFILING_DETAILED");

  code.write("timer_feedback_total[0] += timer_feedback_%s[0];", mi->m_NIID.c_str());
  code.write("timer_feedback_total[1] += timer_feedback_%s[1];", mi->m_NIID.c_str());
  code.write("timer_feedback_total[2] += timer_feedback_%s[2];", mi->m_NIID.c_str());
  iterate_begin_const_ptr(ModuleInstance, mi->m_children, subMi)
  {
    writePerModuleFeedbackProfilingPrintfs(code, subMi);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
void writeFeedbackProfilingPrintfs(CodeWriter& code, const ModuleInstance *mi)
{
  code.write("timer_feedback_total[0] = timer_feedback_total[1] = timer_feedback_total[2] = 0.0f;");
  writePerModuleFeedbackProfilingPrintfs(code, mi);
  code.write("#ifndef EUPHORIA_PROFILING_DETAILED");
  code.write(
    "NMP_MSG(\"feedback total, %%f, %%f, %%f, %%f\", "
    "timer_feedback_total[0], timer_feedback_total[1], timer_feedback_total[2], "
    "timer_feedback_total[0] + timer_feedback_total[1] + timer_feedback_total[2]);");

  code.write("timer_total[0] += g_profilingTotalTimerFraction * (timer_feedback_total[0] + timer_update_total[0] - timer_total[0]);");
  code.write("timer_total[1] += g_profilingTotalTimerFraction * (timer_feedback_total[1] + timer_update_total[1] - timer_total[1]);");
  code.write("timer_total[2] += g_profilingTotalTimerFraction * (timer_feedback_total[2] + timer_update_total[2] - timer_total[2]);");
  code.write(
    "NMP_MSG(\"total, %%f, %%f, %%f, %%f\\n\", "
    "timer_total[0], timer_total[1], timer_total[2], "
    "timer_total[0] + timer_total[1] + timer_total[2]);");
  code.write("#endif // EUPHORIA_PROFILING_DETAILED");
}

//----------------------------------------------------------------------------------------------------------------------
void writeUpdatePhaseRecursive(CodeWriter& code, ModuleInstance *mi)
{
  // Update
  // don't write update() for:
  // - root module (as it calls it from within its hand-written update fn)
  // - any module with mmNoUpdate modifier (specified by the author of the module)
  //
  if (mi->m_parent && !hasModuleModifier(mi->m_module->m_modifiers, mmNoUpdate))
  {
    // timing code for the graphing/report stuff
    code.write("#ifdef EUPHORIA_PROFILING");
    code.write("QueryPerformanceCounter( &StartTimer );");
    code.write("#endif // EUPHORIA_PROFILING");

    // Call the module update for this ModuleInstance
    code.write("module_%s->update(timeStep);", mi->m_NIID.c_str());

    code.write("#ifdef EUPHORIA_PROFILING");
    code.write("QueryPerformanceCounter( &EndTimer );");
    code.write("UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;");
    code.write(
      "timer_update_%s[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_%s[1]));", 
      mi->m_NIID.c_str(), mi->m_NIID.c_str());
    code.write("#endif // EUPHORIA_PROFILING");
  }

  // Recurse children
  if (!mi->m_updateBuckets.empty())
  {
    code.openBrace();

    bool syncPoint = false;
    iterate_begin_ptr(ExecutionBucket, mi->m_updateBuckets, ebUp)
    {
      if (!syncPoint)
        syncPoint = true;
      else
        code.write("// SYNC");

      iterate_begin_ptr(ModuleInstance, ebUp->m_instances, ebMi)
      {
        // Declare pointer to child module (only) if required
        //
        // Required if we'll be calling, combineInputs, combineOutputs or update.
        // Otherwise omitted to avoid unreferenced variable compiler warning/error.
        //
        bool needModulePtrAccess = 
          (ebMi->m_module->varDeclBlockHasJunctionAssignments(msOutputs))
          || (ebMi->m_module->varDeclBlockHasJunctionAssignments(msInputs))
          || !hasModuleModifier(ebMi->m_module->m_modifiers, mmNoUpdate);
        if (needModulePtrAccess)
        {
          code.write("%s* module_%s = (%s*)getModule(NetworkManifest::%s);",
            ebMi->m_module->m_name.c_str(),
            ebMi->m_NIID.c_str(),
            ebMi->m_module->m_name.c_str(),
            ebMi->m_NIID.c_str());
        }

        // This block need exist only to contain a call to one of the above mentioned
        // functions or if the module has children to recurse.
        //
        if (needModulePtrAccess == true || !ebMi->m_updateBuckets.empty())
        {
          code.write("if (isEnabled(NetworkManifest::%s))", ebMi->m_NIID.c_str());
          code.openBrace();

          // Combine inputs
          if (ebMi->m_module->varDeclBlockHasJunctionAssignments(msInputs))
          {
            // timing code for the graphing/report stuff
            code.write("#ifdef EUPHORIA_PROFILING");
            code.write("QueryPerformanceCounter( &StartTimer );");
            code.write("#endif // EUPHORIA_PROFILING");

            code.write("module_%s->combineInputs();", ebMi->m_NIID.c_str());

            code.write("#ifdef EUPHORIA_PROFILING");
            code.write("QueryPerformanceCounter( &EndTimer );");
            code.write("UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;");
            code.write(
              "timer_update_%s[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_%s[0]));", 
              ebMi->m_NIID.c_str(), ebMi->m_NIID.c_str());
            code.write("#endif // EUPHORIA_PROFILING");
          }
          writeUpdatePhaseRecursive(code, ebMi);

          code.closeBrace();
          //
        }
        iterate_end
      }
      iterate_end

        code.closeBrace();
    }
  }

  // Combine outputs
  if (mi->m_module->varDeclBlockHasJunctionAssignments(msOutputs))
  {
    // timing code for the graphing/report stuff
    code.write("#ifdef EUPHORIA_PROFILING");
    code.write("QueryPerformanceCounter( &StartTimer );");
    code.write("#endif // EUPHORIA_PROFILING");

    code.write("module_%s->combineOutputs();", mi->m_NIID.c_str());

    code.write("#ifdef EUPHORIA_PROFILING");
    code.write("QueryPerformanceCounter( &EndTimer );");
    code.write("UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;");
    code.write(
      "timer_update_%s[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_%s[2]));", 
      mi->m_NIID.c_str(), mi->m_NIID.c_str());
    code.write("#endif // EUPHORIA_PROFILING");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void writeFeedbackPhaseRecursive(CodeWriter& code, ModuleInstance *mi)
{
  // Feedback
  // don't write [feedback] for 
  // root module, (as per update above)
  // any module with mmNoFeedback modifier (also as per update above)
  if (mi->m_parent && !hasModuleModifier(mi->m_module->m_modifiers, mmNoFeedback))
  {
    // timing code for the graphing/report stuff
    code.write("#ifdef EUPHORIA_PROFILING");
    code.write("QueryPerformanceCounter( &StartTimer );");
    code.write("#endif // EUPHORIA_PROFILING");

    code.write("module_%s->feedback(timeStep);", mi->m_NIID.c_str());

    code.write("#ifdef EUPHORIA_PROFILING");
    code.write("QueryPerformanceCounter( &EndTimer );");
    code.write("UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;");
    code.write(
      "timer_feedback_%s[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_%s[1]));", 
      mi->m_NIID.c_str(), mi->m_NIID.c_str());
    code.write("#endif // EUPHORIA_PROFILING");

  }

  // Recurse children
  if (!mi->m_feedbackBuckets.empty())
  {
    code.openBrace();

    bool syncPoint = false;
    iterate_begin_ptr(ExecutionBucket, mi->m_feedbackBuckets, ebUp)
    {
      if (!syncPoint)
        syncPoint = true;
      else
        code.write("// SYNC");

      iterate_begin_ptr(ModuleInstance, ebUp->m_instances, ebMi)
      {
        // Declare pointer to child module (only) if required.
        //
        // Required only to call  combineFeedInputs, combineFeedOutputs or feedback.
        // Otherwise omitted to avoid unreferenced variable compiler warning/error.
        //
        bool needModulePtrAccess = 
          (ebMi->m_module->varDeclBlockHasJunctionAssignments(msFeedOutputs))
          || (ebMi->m_module->varDeclBlockHasJunctionAssignments(msFeedInputs))
          || !hasModuleModifier(ebMi->m_module->m_modifiers, mmNoFeedback);
        if (needModulePtrAccess)
        {
          code.write("%s* module_%s = (%s*)getModule(NetworkManifest::%s);",
            ebMi->m_module->m_name.c_str(),
            ebMi->m_NIID.c_str(),
            ebMi->m_module->m_name.c_str(),
            ebMi->m_NIID.c_str());
        }

        // The block generated below need only exist if there is a call to one
        // of the above mentioned functions or there are children to recurse.
        //
        if (needModulePtrAccess || !ebMi->m_feedbackBuckets.empty())
        {
          code.write("if (isEnabled(NetworkManifest::%s))", ebMi->m_NIID.c_str());
          code.openBrace();

          if (ebMi->m_module->varDeclBlockHasJunctionAssignments(msFeedInputs))
          {
            // timing code for the graphing/report stuff
            code.write("#ifdef EUPHORIA_PROFILING");
            code.write("QueryPerformanceCounter( &StartTimer );");
            code.write("#endif // EUPHORIA_PROFILING");

            code.write("module_%s->combineFeedbackInputs();", ebMi->m_NIID.c_str());

            code.write("#ifdef EUPHORIA_PROFILING");
            code.write("QueryPerformanceCounter( &EndTimer );");
            code.write("UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;");
            code.write(
              "timer_feedback_%s[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_%s[0]));", 
              ebMi->m_NIID.c_str(), ebMi->m_NIID.c_str());
            code.write("#endif // EUPHORIA_PROFILING");
          }

          writeFeedbackPhaseRecursive(code, ebMi);

          code.closeBrace();
        }
      }
      iterate_end
    }
    iterate_end

    code.closeBrace();
  }

  if (mi->m_module->varDeclBlockHasJunctionAssignments(msFeedOutputs))
  {
    // timing code for the graphing/report stuff
    code.write("#ifdef EUPHORIA_PROFILING");
    code.write("QueryPerformanceCounter( &StartTimer );");
    code.write("#endif // EUPHORIA_PROFILING");

    code.write("module_%s->combineFeedbackOutputs();", mi->m_NIID.c_str());

    code.write("#ifdef EUPHORIA_PROFILING");
    code.write("QueryPerformanceCounter( &EndTimer );");
    code.write("UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;");
    code.write(
      "timer_feedback_%s[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_%s[2]));", 
      mi->m_NIID.c_str(), mi->m_NIID.c_str());
    code.write("#endif // EUPHORIA_PROFILING");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void getTotalNumberOfInstances(const ModuleInstance *inst, unsigned int &totalInst)
{
  totalInst ++;

  iterate_begin_const_ptr(ModuleInstance, inst->m_children, chInst)
  {
    getTotalNumberOfInstances(chInst, totalInst);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
bool DescGen::writeTaskExec(CodeWriter& code, const NetworkDatabase &ndb)
{
  // count the number of instantiated modules in the network
  unsigned int numInst = 0;
  getTotalNumberOfInstances(ndb.m_instance, numInst);

  const ModuleDef* rootInst = getRootModule(ndb.m_moduleDefs);


  code.writeNMCopyrightHeader();

  code.write("#include \"NetworkDescriptor.h\"");
  code.write("#include \"euphoria/erModule.h\"");
  code.write("#include \"NMPlatform/NMTimer.h\"");

  code.newLine();
  iterate_begin_const_ptr(ModuleDef, ndb.m_moduleDefs, def)
  {
    code.write("#include \"%s.h\"", def->m_name.c_str());
  }
  iterate_end

  code.writeHorizontalSeparator();

  code.write("#define TIMETYPE float");
  code.write("TIMETYPE g_profilingTimerFraction = TIMETYPE(1.0); // amount to update the timings by each update - for a rolling average");
  code.write("TIMETYPE g_profilingTotalTimerFraction = TIMETYPE(1.0); // amount to update the total timings by each update - for a rolling average");

  // -----------------------------------------------------------------------------------------------
  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");

  code.write("#ifdef EUPHORIA_PROFILING");
  writePerModuleProfilingDeclarations(code, ndb.m_instance);
  code.write("TIMETYPE timer_update_total[3] = {0,0,0};");
  code.write("TIMETYPE timer_feedback_total[3] = {0,0,0};");
  code.write("TIMETYPE timer_total[3] = {0,0,0};");
  code.write("#endif // EUPHORIA_PROFILING\n");

  code.write("void %s::executeNetworkUpdate(float timeStep)", rootInst->m_name.c_str());
  code.openBrace();
  {
    {
      // Create some floats to store our timings data in.
      code.write("#ifdef EUPHORIA_PROFILING");

      code.write("LARGE_INTEGER TicksPerSecond;");
      code.write("QueryPerformanceFrequency( &TicksPerSecond );");
      code.write("TIMETYPE dMicrosecondsPerTick = (TIMETYPE) (double(1.0) / ((double)TicksPerSecond.QuadPart * 0.000001));");
      code.write("LARGE_INTEGER StartTimer, EndTimer;");
      code.write("__int64 UpdateTicks;");
      code.write("#endif // EUPHORIA_PROFILING");
    }

    writeUpdatePhaseRecursive(code, ndb.m_instance);

    {
      // Print out the floats to give us some timing data.
      code.write("#ifdef EUPHORIA_PROFILING");
      code.write("#ifdef EUPHORIA_PROFILING_DETAILED");
      code.write("NMP_MSG(\"\\n\\nUPDATE TIMINGS:\");");
      code.write("#endif // EUPHORIA_PROFILING_DETAILED");
      writeUpdateProfilingPrintfs(code, ndb.m_instance);
      code.write("#endif // EUPHORIA_PROFILING");
    }

  }
  code.closeBrace();

  code.writeHorizontalSeparator();

  code.write("void %s::executeNetworkFeedback(float timeStep)", rootInst->m_name.c_str());
  code.openBrace();
  {
    {
      // Create some floats to store our timings data in.
      code.write("#ifdef EUPHORIA_PROFILING");

      code.write("LARGE_INTEGER TicksPerSecond;");
      code.write("QueryPerformanceFrequency( &TicksPerSecond );");
      code.write("TIMETYPE dMicrosecondsPerTick = (TIMETYPE) (double(1.0) / ((double)TicksPerSecond.QuadPart * 0.000001));");
      code.write("LARGE_INTEGER StartTimer, EndTimer;");
      code.write("__int64 UpdateTicks;");
      code.write("#endif // EUPHORIA_PROFILING");
    }

    writeFeedbackPhaseRecursive(code, ndb.m_instance);

    {
      // Print out the floats to give us some timing data.
      code.write("#ifdef EUPHORIA_PROFILING");
      code.write("#ifdef EUPHORIA_PROFILING_DETAILED");
      code.write("NMP_MSG(\"\\n\\nFEEDBACK TIMINGS:\");");
      code.write("#endif // EUPHORIA_PROFILING_DETAILED");
      writeFeedbackProfilingPrintfs(code, ndb.m_instance);
      code.write("#endif // EUPHORIA_PROFILING");
    }
  }
  code.closeBrace();
  code.newLine();


  // -----------------------------------------------------------------------------------------------
  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLine();

  return true;
}
