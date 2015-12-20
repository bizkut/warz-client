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
#include "NetworkStats.h"

//----------------------------------------------------------------------------------------------------------------------
void repositionConsoleCursor(HANDLE cHandle, unsigned short XOffset)
{
  CONSOLE_SCREEN_BUFFER_INFO sbuff = {0};
  if (cHandle != 0)
  { 
    GetConsoleScreenBufferInfo((HANDLE)cHandle, &sbuff);

    COORD columnAlign = sbuff.dwCursorPosition;
    columnAlign.X = XOffset;
    SetConsoleCursorPosition((HANDLE)cHandle, columnAlign);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void countCombines(const Type* t, unsigned int &calls)
{
  if (t->m_struct)
  {
    for (int i=Struct::abPublic; i<Struct::abNumAccessabilityBlocks; i++)
    {
      iterate_begin_const(Struct::VarDecl, t->m_struct->m_vardecl[i], vd)
      {
        countCombines(vd.m_knownType, calls);
      }
      iterate_end
    }
  }
  else
  {
    calls ++;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void sumModulePayloadCostAndChildTotal(const ModuleInstance *inst, unsigned int& size, unsigned int &childTotal)
{
  size += inst->m_module->getSize();
  childTotal ++;

  iterate_begin_const_ptr(ModuleInstance, inst->m_children, chInst)
  {
    sumModulePayloadCostAndChildTotal(chInst, size, childTotal);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
void walkInst(ModuleInstance *mi)
{
#if 0
  if (!mi->m_updateBuckets.empty())
  {
    printf("%s\n", mi->m_NIID.c_str());
    uint32_t insts = 0;

    iterate_begin_ptr(ExecutionBucket, mi->m_updateBuckets, ebUp)
    {
      printf("  + ---------- \n");
      iterate_begin_ptr(ModuleInstance, ebUp->m_instances, ebMi)
      {
        printf("  + %s\n", ebMi->m_NIID.c_str());
        insts ++;
      }
      iterate_end
    }
    iterate_end

    if (insts != mi->m_children.size())
    {
      // we have not assigned execution ordering for all the children?!
      printf(" >> dependency problem!\n");
    }
    printf("\n");
  }
#endif // 0

  StringBuilder arrayAccess(16);

  printf("---- %s ----\n", mi->m_NIID.c_str());
  iterate_begin_ptr(ConnectionInstance, mi->m_connections, coni)
  {
    printf("|%i|  %s", 
      coni->m_junctionGroup,
      coni->m_varDeclTarget->m_varDeclPtr->m_varname.c_str());

    if (coni->m_varDeclTarget->m_arrayElementAccess)
    {
      arrayAccess.reset();
      ModuleDef::Connection::Path::Component* comp = (ModuleDef::Connection::Path::Component*)coni->m_varDeclTarget->m_arrayElementAccess;
      comp->formatArrayAccess(arrayAccess, true);
      printf("%s", arrayAccess.c_str());
    }

    printf(" <- %s", coni->m_varDeclSource->m_varDeclPtr->m_varname.c_str());

    if (coni->m_varDeclSource->m_arrayElementAccess)
    {
      arrayAccess.reset();
      ModuleDef::Connection::Path::Component* comp = (ModuleDef::Connection::Path::Component*)coni->m_varDeclSource->m_arrayElementAccess;
      comp->formatArrayAccess(arrayAccess, true);
      printf("%s", arrayAccess.c_str());
    }

    printf(" @ %s\n", coni->m_instanceSource->m_NIID.c_str());
  }
  iterate_end

  iterate_begin_ptr(ModuleInstance, mi->m_children, child)
  {
    walkInst(child);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
void generateNetworkStatistics(const NetworkDatabase& ndb)
{
  HANDLE cHandle = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, NULL);

  if (cHandle) 
    SetConsoleTextAttribute(cHandle, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED);
  
  printf("\nNetwork Statistics\n\n");

  if (cHandle) 
    SetConsoleTextAttribute(cHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

  // count up how many connections each junction has, building a histogram of prominent usage
  uint32_t connectionLengthHistogram[32], connectionLengthMax = 0;
  memset(&connectionLengthHistogram, 0, sizeof(uint32_t) * 32);

  printf("---------------------------------------------------------\n");
  printf("Junction Memory Combine Cost              Est Calls ::combine()\n\n");

  // various stats
  unsigned int  modDefSize = 0, 
                numJunctions = 0, 
                numDIJunctions = 0,
                numAssignments = 0, 
                numGlobalAccessMods = 0,
                totalJunctionMemoryCost = 0, 
                totalCallsToCombine = 0, 
                childTotal = 0;

  sumModulePayloadCostAndChildTotal(ndb.m_instance, modDefSize, childTotal);

  iterate_begin_const_ptr(ModuleDef, ndb.m_moduleDefs, def)
  {
    if (hasModuleModifier(def->m_modifiers, mmGlobalAccess))
      numGlobalAccessMods ++;

    iterate_begin_const_ptr(ModuleDef::ConnectionSet, def->m_connectionSets, set)
    {
      unsigned int junctionMemoryCost = 0, callsToCombine = 0;
      iterate_begin_const_ptr(ModuleDef::Junction, set->m_junctions, jnc)
      {
        iterate_begin_const(ModuleDef::Junction::ConnectionSource, jnc->m_connectionSources, csrc)
        {
          if (!csrc.m_junction)
          {
            junctionMemoryCost += csrc.m_varDecl.m_varDeclPtr->getSize();

            const Type* jT = csrc.m_varDecl.m_varDeclPtr->m_knownType;
            countCombines(jT, callsToCombine);
          }
        }
        iterate_end
      }
      iterate_end

      totalJunctionMemoryCost += junctionMemoryCost;
      totalCallsToCombine += callsToCombine;

      printf(" %s::%s", def->m_name.c_str(), set->m_name.c_str());
      repositionConsoleCursor(cHandle, 35);
      printf(" => %d ", junctionMemoryCost);
      repositionConsoleCursor(cHandle, 45);
      printf("  |  %d\n", callsToCombine);


      iterate_begin_const_ptr(ModuleDef::Connection, set->m_connectionDefs, con)
      {
        if (con->m_task == ctCreateJunction)
        {
          numJunctions ++;

          if (con->m_newJunctionMode == jmDirectInput)
          {
            numDIJunctions ++;
          }

          uint32_t conPathsCount = (uint32_t)con->m_paths.size();
          assert(conPathsCount < 32);

          connectionLengthHistogram[conPathsCount] ++;
          connectionLengthMax = connectionLengthMax > conPathsCount ? connectionLengthMax : conPathsCount;
        }
        else if (con->m_task == ctAssignJunction)
        {
          numAssignments ++;
        }
      }
      iterate_end
    }
    iterate_end
  }
  iterate_end

  printf("\nTotal                               : %d KB    |  %d\n", totalJunctionMemoryCost / 1024, totalCallsToCombine);
  printf("---------------------------------------------------------\n");
  printf("Number Of Modules Defined           : %d \n", (int)ndb.m_moduleDefs.size());
  printf("Number Of Global Access Modules Defined   : %d (%d %%)\n", numGlobalAccessMods, (int)(((100.0f / (float)ndb.m_moduleDefs.size()) * (float)numGlobalAccessMods)) );
  printf("Number Of Modules Instantiated      : %d \n", (int)childTotal);
  printf("Approx Module Data Footprint        : %d KB\n", modDefSize / 1024);
  printf("---------------------------------------------------------\n");
  printf("Number Of Behaviours                : %d \n", (int)ndb.m_behaviourDefs.size());
  printf("Number Of Struct Types              : %d \n", (int)ndb.m_structMap.getNumUsedSlots());
  printf("---------------------------------------------------------\n");
  printf("Number Of Junctions                 : %d \n", numJunctions);
  printf("Number Of Junction Assignments      : %d \n", numAssignments);
  printf("---------------------------------------------------------\n");
  printf("Junction Connection Length Histogram\n\n");
  printf(" Length |  Instances\n");
  for (uint32_t i=1; i<=connectionLengthMax; i++)
  {
    printf(" %02i     |  ", i);
    printf("%i", connectionLengthHistogram[i]);

    if (i==1)
      printf("       [%i DirectInput]", numDIJunctions);

    printf("\n");
  }
  printf("\n");

#if 0

  walkInst(ndb.m_instance);

  printf("\n");

  iterate_begin_const_ptr(ModuleDef, ndb.m_moduleDefs, def)
  {
    printf("%s\n", def->m_name.c_str());

    for (ModuleDependencies::const_iterator mIt = def->m_reducedUpdateDependencies.const_begin(), 
                                            chEnd = def->m_reducedUpdateDependencies.const_end(); 
                                            mIt != chEnd; 
                                            ++mIt)
    {
      BoundModuleDependency *bmd = (*mIt);

      printf("  <- %s ", (*mIt)->m_def->m_name.c_str());

      for (ModuleBinding::BindOps::const_iterator it = bmd->m_binding->m_ops.begin(),
                                                  end = bmd->m_binding->m_ops.end();
                                                  it != end;
                                                  ++it)
      {
        ModuleBinding::BindOp *bo = (*it);

        switch (bo->m_op)
        {
          case ModuleBinding::GetOwner:
            printf(" [Owner] ");
            break;
          case ModuleBinding::LookupChildByName:
            printf(" [Child:%s] ", bo->m_text.c_str());
            break;
        }
      }

      printf(" \n");
    }

    printf("\n");
  }
  iterate_end

#endif 

#if 0
  iterate_begin_const_ptr(ModuleDef, ndb.m_moduleDefs, def)
  {
    iterate_begin_const_ptr(ModuleDef::ConnectionSet, def->m_connectionSets, set)
    {
      iterate_begin_const_ptr(ModuleDef::Connection, set->m_connectionDefs, con)
      {
        if (con->m_task == ctCreateJunction)
        {
          uint32_t conPathsCount = (uint32_t)con->m_paths.size();
          if (conPathsCount > 1)
          {
            const PString& jmode = ndb.m_keywords.getJunctionModeLabel(con->m_newJunctionMode);
            
            printf("%02i, %s,  %s, %s , %s\n", 
              conPathsCount, 
              def->m_name.c_str(),
              con->m_target->m_target.m_junction->m_name.c_str(),
              jmode.c_str(),
              con->m_target->m_target.m_junction->m_representativeVarDecl.m_varDeclPtr->m_knownType->m_name.c_str());
          }
        }
      }
      iterate_end
    }
    iterate_end
  }
  iterate_end

#endif 

#if 0
  typedef nmtl::hashmap_dense_dynamic<nmtl::hkey64, const ModuleDef*>   DupeTable;
  StringBuilder sb(128);

  iterate_begin_const_ptr(ModuleDef, ndb.m_moduleDefs, def)
  {
    printf("\nMODULE: %s\n", def->m_name.c_str());

    DupeTable st(8);

    iterate_begin_const(ModuleDef::Child, def->m_children, child)
    {
      ModuleDef* chDef = child.m_modulePtr;
      printf("\n  CHILD: %s\n", chDef->m_name.c_str());

      iterate_begin_const_ptr(ModuleDef::ConnectionSet, chDef->m_connectionSets, set)
      {
        iterate_begin_const_ptr(ModuleDef::Connection, set->m_connectionDefs, con)
        {
          if (con->m_task == ctCreateJunction)
          {
            iterate_begin_const_ptr(ModuleDef::Connection::Path, con->m_paths, path)
            {
              const ModuleDef *dupe;
              if (st.find(path->m_componentsHashKey, &dupe))
              {
                if (dupe != chDef)
                {
                  sb.reset();
                  path->toString(sb);
                  printf("    %s (shared with %s)\n", sb.c_str(), dupe->m_name.c_str());
                }
              }
              else
              {
                st.insert(path->m_componentsHashKey, chDef);
              }
            }
            iterate_end
          }
        }
        iterate_end
      }
      iterate_end
    }
    iterate_end
  }
  iterate_end
#endif // 0
}


//----------------------------------------------------------------------------------------------------------------------
void validateDependencyData(ModuleInstance *mi, bool &networkOK)
{
  if (!mi->m_updateBuckets.empty())
  {
    uint32_t insts = 0;

    iterate_begin_ptr(ExecutionBucket, mi->m_updateBuckets, ebUp)
    {
      insts += ebUp->m_instances.size();
    }
    iterate_end

    if (insts != mi->m_children.size())
    {
      networkOK = false;

      // we have not assigned execution ordering for all the children?!
      printf(" >> dependency problem (Update) << \n");

    }
  }

  if (!mi->m_feedbackBuckets.empty())
  {
    uint32_t insts = 0;

    iterate_begin_ptr(ExecutionBucket, mi->m_feedbackBuckets, ebUp)
    {
      insts += ebUp->m_instances.size();
    }
    iterate_end

    if (insts != mi->m_children.size())
    {
      networkOK = false;

      // we have not assigned execution ordering for all the children?!
      printf(" >> dependency problem (Feedback) << \n");

    }
  }

  iterate_begin_ptr(ModuleInstance, mi->m_children, child)
  {
    validateDependencyData(child, networkOK);
  }
  iterate_end
}


//----------------------------------------------------------------------------------------------------------------------
bool validateDependencies(const NetworkDatabase& ndb)
{
  bool networkOK = true;

  printf("\nChecking Dependencies...\n");
  validateDependencyData(ndb.m_instance, networkOK);

  return networkOK;
}
