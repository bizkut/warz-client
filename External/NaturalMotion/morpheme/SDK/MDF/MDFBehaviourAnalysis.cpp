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
#include "MDFBehaviourAnalysis.h"
#include "StringBuilder.h"
#include "Utils.h"
#include "nmtl/algorithm.h"

#include <algorithm>


//----------------------------------------------------------------------------------------------------------------------
// values used to sort variables by type, to reduce padding
static unsigned int VariableTypeSortTable[bhvtNumVariableTypes] = 
{
  4,  // Float
  4,  // Integer
  1,  // Bool
  16  // Vector3
};

//----------------------------------------------------------------------------------------------------------------------
bool sortBaseVariableByTypeSize(const BehaviourDef::BaseVariable* elem1, const BehaviourDef::BaseVariable* elem2)
{
  return VariableTypeSortTable[(int)elem1->getType()] > VariableTypeSortTable[(int)elem2->getType()];
}

//----------------------------------------------------------------------------------------------------------------------
bool sortAttributeVariablePtrsByTypeSize(const BehaviourDef::AttributeVariable* elem1, const BehaviourDef::AttributeVariable* elem2)
{
  return VariableTypeSortTable[(int)elem1->getType()] > VariableTypeSortTable[(int)elem2->getType()];
}

//----------------------------------------------------------------------------------------------------------------------
bool sortAttributeVariablePtrMessageParams(const BehaviourDef::AttributeVariable* elem1, const BehaviourDef::AttributeVariable* elem2)
{
  return elem1->isCpDefault() && !elem2->isCpDefault();
}

//----------------------------------------------------------------------------------------------------------------------
void validateBehaviours(const NetworkDatabase& ndb, BehaviourDefs& behaviours)
{
  const ModuleDef* rootModule = getRootModule(ndb.m_moduleDefs);

  // check that the behaviours have valid names
  iterate_begin_ptr(BehaviourDef, behaviours, bv)
  {
    // look the name up in the type registry to see if it clashes
    const Type* behaviourNameIsAType = ndb.m_typesRegistry.lookupByName("", bv->m_name);
    if (behaviourNameIsAType)
    {
      throw(new ValidationException(bv, bv->m_lineDefined, bv->m_name.c_str(), 
        "Behaviour name clashes with a registered type"));
    }
  }
  iterate_end

  StringBuilder ainTemp(32);
  unsigned int behaviourID = 1, controlParamID = 0, outputControlParamID = 0, bodyPoseID = 0;

  iterate_begin_ptr(BehaviourDef, behaviours, bv)
  {
    bv->m_behaviourID = behaviourID++;
    controlParamID = 0;
    outputControlParamID = 0;
    bodyPoseID = 0;

    BehaviourDef::ModulesToEnable::iterator it = bv->m_modulesToEnable.begin();
    BehaviourDef::ModulesToEnable::iterator end = bv->m_modulesToEnable.end();
    for (; it != end; ++it)
    {
      BehaviourDef::ModuleToEnable &mte = (*it);
      mte.m_allChildren = false;

      // start with the root module, lookup child names as we descend the hierarchy
      const ModuleDef* mteInst = rootModule;

      unsigned int i, pathLen = mte.m_path.count();
      for (i=0; i<pathLen; i++)
      {
        BehaviourDef::ModuleToEnable::PathItem *pi = mte.m_path.get(i);
        pi->m_modulePrev = mteInst;

        // * indicates user wants to do 'enable all children' on the specified module path
        if (pi->m_name.c_str()[0] == '*')
        {
          mte.m_allChildren = true;
        }
        else
        {
          // make sure we don't allow "module.module.*.module" - the * must be the last item
          // (this actually may never happen, given the grammar constraints should prevent it)
          if (mte.m_allChildren)
          {
            throw(new ValidationException(bv, bv->m_lineDefined, pi->m_name.c_str(), 
              "Cannot specify more modules after '*' wildcard"));
          }

          // lookup specified module
          const ModuleDef::Child* ch = mteInst->getChildByName(pi->m_name);
          if (!ch)
          {
            throw(new ValidationException(bv, bv->m_lineDefined, pi->m_name.c_str(), 
              "Module to enable not found in network definition"));
          }

          // make sure any array access makes sense
          if (pi->m_arrayAccess)
          {
            if (!ch)
            {
              throw(new ValidationException(bv, bv->m_lineDefined, pi->m_name.c_str(), 
                "could not access child instance data to validate module array access"));
            }
            if (ch->m_arraySize == 0)
            {
              throw(new ValidationException(bv, bv->m_lineDefined, pi->m_name.c_str(), 
                "this module is not stored in an array, cannot enable with array accessor"));
            }
            if (pi->m_arrayIndex >= (int)ch->m_arraySize)
            {
              throw(new ValidationException(bv, bv->m_lineDefined, pi->m_name.c_str(), 
                "index out of range - attempting to enable a module in an array with index larger than array"));
            }
          }
          else
          {
            if (ch->m_arraySize > 0)
            {
              throw(new ValidationException(bv, bv->m_lineDefined, pi->m_name.c_str(), 
                "must specify array accessor for module stored as array"));
            }
          }

          // mark our position in the path and continue
          pi->m_childInst = ch;
          mteInst = ch->m_modulePtr;
        }
      }

      // tag special case where user is targeting the root module, eg. '*;'
      mte.m_targetIsRoot = (mteInst == rootModule);
      if (mte.m_targetIsRoot && !mte.m_allChildren)
      {
        // don't actually think this is feasible
        throw(new ValidationException(bv, bv->m_lineDefined, mteInst->m_name.c_str(), 
          "cannot only enable network root module"));
      }
    }
    
    // Control Parameters - make sure that all control paramater groups appear as an attribute group
    {
      iterate_begin(BehaviourDef::ControlParamGroup, bv->m_controlParamGroups, cg)
      {
        BehaviourDef::AttributeGroup* ag = bv->findOrCreateAttributeGroup(cg.m_name);

        // Copy the help from the ControlParameterGroup if there is no help
        if (!cg.m_comment.empty() && ag->m_comment.empty())
        {
          ag->m_lineDefined = cg.m_lineDefined;
          ag->m_comment = cg.m_comment;
        }

        iterate_begin(BehaviourDef::ControlParam, cg.m_params, cp)
        {
          cp.m_ID = controlParamID;
          controlParamID += ( (cp.getArraySize() == 0) ? 1 : cp.getArraySize() );
        }
        iterate_end;
     }
      iterate_end;
    }

    // OutputControlParam
    iterate_begin(BehaviourDef::OutputControlParam, bv->m_outputControlParams, cp)
    {
      cp.m_ID = outputControlParamID;
      outputControlParamID += ( (cp.getArraySize() == 0) ? 1 : cp.getArraySize() );
    }
    iterate_end;

    // AnimationInput
    iterate_begin(BehaviourDef::AnimationInput, bv->m_animationInputs, ain)
    {
        ain.m_ID = bodyPoseID ++;

        // build 'apply' values per-limb for BodyPose inputs
        BehaviourDef::AttributeGroup* bpAg = bv->findOrCreateAttributeGroup(ain.m_name.c_str());
        if (bpAg->m_comment.empty())
        {
          bpAg->m_comment    = ain.m_comment;
          bpAg->m_lineDefined= ain.m_lineDefined;
        }

        bpAg->m_modifiers = ain.m_modifiers;

        iterate_begin_const_ptr(LimbInstance, ndb.m_limbInstances, limbInst)
        {
          ainTemp.reset();
          ainTemp.appendf("%s_ApplyTo%s", ain.m_name.c_str(), limbInst->m_name.c_str());
          BehaviourDef::AttributeVariable apTemp(bhvtFloat, ainTemp.c_str(), 0);
          apTemp.m_default.f  = (limbInst->m_limbtype < LimbInstance::kUnknown) ? ain.m_defaults[limbInst->m_limbtype] : 1.0f;
          apTemp.m_min.f      = 0.0f;
          apTemp.m_max.f      = 1.0f;
          apTemp.m_hasMin     = true;
          apTemp.m_hasMax     = true;
          ModifierOption& modopt = apTemp.m_modifiers.stack.push();
          apTemp.m_modifiers.keywords = &ndb.m_keywords;
          modopt.m_keyword = "__private__"; //BehaviourVariableModifierKeywords[bvmPrivate].m_text;

          ainTemp.reset();
          const char* moduleName = limbInst->m_inst->m_module->m_name.c_str();
          unsigned int limbIndex = limbInst->m_index - 1;
          ainTemp.appendf("%s {%s%i}", moduleName, moduleName, limbIndex);
          apTemp.m_title.set(ainTemp.c_str());

          ainTemp.reset();
          ainTemp.appendf("// Set how much of the pose to use on this %s.", moduleName);
          apTemp.m_comment.set(ainTemp.c_str());

          bpAg->m_vars.push_back(apTemp);
        }
        iterate_end
      }
    iterate_end;

    // sort the various variable piles by size, largest first
    bv->m_sortedControlParams.reserve(bv->m_controlParamGroups.size() * 8);
    iterate_begin(BehaviourDef::ControlParamGroup, bv->m_controlParamGroups, cg)
    {
      iterate_begin_const(BehaviourDef::ControlParam, cg.m_params, cp)
      {
        bv->m_sortedControlParams.push_back(&cp);
      }
      iterate_end;
    }
    iterate_end;
    nmtl::sort( bv->m_sortedControlParams.begin( ), bv->m_sortedControlParams.end( ), sortBaseVariableByTypeSize );

    bv->m_sortedOutputControlParams.reserve(bv->m_outputControlParams.size());
    iterate_begin_const(BehaviourDef::OutputControlParam, bv->m_outputControlParams, cp)
    {
      bv->m_sortedOutputControlParams.push_back(&cp);
    }
    iterate_end;
    nmtl::sort( bv->m_sortedOutputControlParams.begin( ), bv->m_sortedOutputControlParams.end( ), sortBaseVariableByTypeSize );

    bv->m_sortedAttributes.reserve(16);
    iterate_begin_const(BehaviourDef::AttributeGroup, bv->m_attributeGroups, ag)
    {
      iterate_begin_const(BehaviourDef::AttributeVariable, ag.m_vars, av)
      {
        if (hasBehaviourVariableModifier(av.m_modifiers, bvmEnumDefaultIndex) ||
            hasBehaviourVariableModifier(av.m_modifiers, bvmEnumNoInstance))
        {
          throw(new ValidationException(bv, av.m_lineDefined, av.m_name.c_str(), 
            "using a enum-only modifier on non-enum value"));
        }

        bv->m_sortedAttributes.push_back(&av);
      }
      iterate_end

      // check that any defaults specified actually exist in the enumeration they are applied to
      iterate_begin_const(BehaviourDef::AttributeGroup::EnumDecl, ag.m_enumDecl, enm)
      {
         if (const ModifierOption* enmDefault = hasBehaviourVariableModifier(enm.m_modifiers, bvmEnumDefaultIndex))
         {
           const EnumDeclBase::Entry* ent = enm.findNamedEnumEntry(enmDefault->m_value);
           if (!ent)
           {
             throw(new ValidationException(bv, enm.m_lineDefined, enmDefault->m_value.c_str(), 
               "enum default value couldn't be found in enumeration"));
           }
         }
      }
      iterate_end
    }
    iterate_end

    nmtl::sort( bv->m_sortedAttributes.begin( ), bv->m_sortedAttributes.end( ), sortAttributeVariablePtrsByTypeSize );

    // Populate list of attributes in the correct order for the message params structure
    // MORPH-21195
    bv->m_messageParamAttributes.reserve(16);
    iterate_begin_const(BehaviourDef::AttributeGroup, bv->m_attributeGroups, ag)
    {
      iterate_begin_const(BehaviourDef::AttributeVariable, ag.m_vars, av)
      {
        bv->m_messageParamAttributes.push_back(&av);
      }
      iterate_end
    }
    iterate_end

    nmtl::stable_sort( bv->m_messageParamAttributes.begin(), bv->m_messageParamAttributes.end(), sortAttributeVariablePtrMessageParams );
  }
  iterate_end
}
