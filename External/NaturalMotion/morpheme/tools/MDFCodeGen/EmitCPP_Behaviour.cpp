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
#include "EmitCPP_Behaviour.h"

#include "NMPlatform/NMPlatform.h"

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Turn a behaviour variable into a compilable string; behaviours have their own small subset
 * of types that map directly to C++ types, rather than going via the MDF types system (as they
 * need to interface with MR)
 */
void writeAttributeVariable(CodeWriter& code, const BehaviourDef::BaseVariable& av)
{
  code.writeTabs();

  switch (av.getType())
  {
    case bhvtInteger:
      code.writeRaw("int32_t %s", av.m_name.c_str());
      break;

    case bhvtBool:
      code.writeRaw("bool %s", av.m_name.c_str());
      break;

    case bhvtFloat:
      code.writeRaw("float %s", av.m_name.c_str());
      break;

    case bhvtVector3:
      code.writeRaw("NMP::Vector3 %s", av.m_name.c_str());
      break;

    case bhvtPhysicsObjectID:
      code.writeRaw("void* %s", av.m_name.c_str());
      break;

    default:
      NMP_ASSERT_FAIL();
      break;
  }

  // add optional array dimensions
  if (av.isArray())
  {
    if (av.m_arraySizeAsString.empty())
      code.writeRaw("[%i]", av.getArraySize());
    else
      code.writeRaw("[%s]", av.m_arraySizeAsString.c_str());
  }
  code.writeRaw(";");

  // add comment on the same line, or jump to the next
  if (!av.m_comment.empty())
  {
    code.writeTab();

    // Write-raw every comment line separately,
    // inserting tabs in between.
    const std::string endOfLine("\n");
    std::string::size_type breaklocation = std::string::npos;
    std::string comment(av.m_comment.c_str());
    bool singleLineComment = true;

    do
    {
      breaklocation = comment.find(endOfLine);
      if (breaklocation != std::string::npos)
      {
        std::string commentLine(comment.substr(0, breaklocation + endOfLine.size()));
        code.writeRaw(commentLine.c_str());
        code.writeTabs();
        comment.erase(0, breaklocation + endOfLine.size());
        singleLineComment = false;
      }
      else if (singleLineComment)
      {
        code.writeRaw(av.m_comment.c_str());
      }
    }
    while (breaklocation != std::string::npos);
    code.newLine();
  }
  else
  {
    code.newLine();
  }
}

// ---------------------------------------------------------------------------------------------------------------------
/**
 * 
 */
bool BehaviourGen::writeBehaviourHeader(const BehaviourDef &bdef, CodeWriter& code, const LimbInstances& limbInstances, const char* projectRootDir)
{
  HeaderGuard hg("BDEF_%s", bdef.m_name.c_str());
  code.writeHeaderTop(hg);

  writeDefInclude(code, projectRootDir, bdef.m_pathToDef.c_str());

  code.write("#include \"NMPlatform/NMPlatform.h\"");
  code.write("#include \"NMPlatform/NMVector3.h\"");
  code.write("#include \"physics/mrPhysicsSerialisationBuffer.h\"");

  code.write("#include \"euphoria/erBehaviour.h\"");
  code.write("#include \"euphoria/erLimb.h\"");
  code.newLine();

  code.write("#include \"NetworkDescriptor.h\"");
  code.write("#include \"NetworkConstants.h\"");
  code.newLines(2);

  code.write("namespace ER");
  code.openBrace();
  code.write("class Character;");
  code.write("class Module;");
  code.closeBrace();
  code.newLines(2);

  code.newLines(2);
  code.write("#ifdef NM_COMPILER_MSVC");
  code.write("# pragma warning ( push )");
  code.write("# pragma warning ( disable : 4324 ) // structure was padded due to __declspec(align()) ");
  code.write("#endif // NM_COMPILER_MSVC");
  code.newLine();

  code.write("#ifndef NM_BEHAVIOUR_LIB_NAMESPACE");
  code.write("  #error behaviour library namespace undefined");
  code.write("#endif");
  code.newLine();

  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");

  // write pose requirements data structure
  code.write("struct %sPoseRequirements", bdef.m_name.c_str());
  code.openBrace();
  // Add public flags for if the animation input should be disabled
  iterate_begin_const(BehaviourDef::AnimationInput, bdef.m_animationInputs, ain)
  {
      code.write("bool enablePoseInput_%s;", ain.m_name.c_str());
    }
  iterate_end;

  code.newLine();
  code.write("%sPoseRequirements()", bdef.m_name.c_str());
  code.openBrace();
  // Add flags for if the animation input should be disabled
  iterate_begin_const(BehaviourDef::AnimationInput, bdef.m_animationInputs, ain)
  {
      code.write("enablePoseInput_%s = true;", ain.m_name.c_str());
    }
  iterate_end;
  code.closeBrace();

  code.closeBraceWithSemicolon();  // struct
  code.newLine();


  // write attribute data decoder structure
  code.write("struct %sBehaviourData", bdef.m_name.c_str());
  code.openBrace();
  code.write("friend class %sBehaviour;", bdef.m_name.c_str());
  code.newLine();

  // declare enums we had defined in the bdef
  iterate_begin_const(BehaviourDef::AttributeGroup, bdef.m_attributeGroups, ag)
  {
    iterate_begin_const(BehaviourDef::AttributeGroup::EnumDecl, ag.m_enumDecl, enm)
    {
      writeEnumDecl(code, enm);
    }
    iterate_end
  }
  iterate_end

  code.write("private:");
  code.newLine();

  // -----------------------------------------------------------------------------------------------

  if (!bdef.m_sortedControlParams.empty())
  {
    code.newLine();
    code.write("struct ControlParams");
    code.openBrace();
    iterate_begin_const(BehaviourDef::ControlParamPtr, bdef.m_sortedControlParams, cp)
    {
      writeAttributeVariable(code, *cp);
    }
    iterate_end;
    code.closeBraceWithSemicolon();

    code.write("ControlParams m_cp;");
    code.newLine();
  }

  // -----------------------------------------------------------------------------------------------

  if (!bdef.m_sortedOutputControlParams.empty())
  {
    code.newLine();
    code.write("struct OutputControlParams");
    code.openBrace();
    iterate_begin_const(BehaviourDef::OutputControlParamPtr, bdef.m_sortedOutputControlParams, cp)
    {
      writeAttributeVariable(code, *cp);
    }
    iterate_end;
    code.closeBraceWithSemicolon();

    code.write("OutputControlParams m_ocp;");
    code.newLine();
  }

  // -----------------------------------------------------------------------------------------------
  // generate storage for the emitted messages
  
  if (!bdef.m_emittedMessages.empty())
  {
    code.newLine();
    code.write("uint32_t m_messages;");
    code.newLine();
  }

  // -----------------------------------------------------------------------------------------------
  // generate storage for any BodyPose inputs

  iterate_begin_const(BehaviourDef::AnimationInput, bdef.m_animationInputs, ain)
  {
      code.write("BodyPoseData  pd_%s;", ain.m_name.c_str());
    }
  iterate_end

  // -----------------------------------------------------------------------------------------------
  // write attribute variables, grouped to reduce padding

  unsigned int numFloatsExpected = 0;
  unsigned int numIntsExpected = 0;
  unsigned int numUInt64sExpected = 0;

  iterate_begin_const(BehaviourDef::AttributeVariablePtr, bdef.m_sortedAttributes, av)
  {
    numFloatsExpected += av->getNumFloats();
    numIntsExpected += av->getNumInts();
    numUInt64sExpected += av->getNumUInt64s();

    switch (av->getType())
    {
      case bhvtVector3:
      case bhvtFloat:
      case bhvtInteger:
      case bhvtPhysicsObjectID:
        writeAttributeVariable(code, *av);
        break;
    }
    // not bools, enums first
  }
  iterate_end

  iterate_begin_const(BehaviourDef::AttributeGroup, bdef.m_attributeGroups, ag)
  {
    iterate_begin_const(BehaviourDef::AttributeGroup::EnumDecl, ag.m_enumDecl, enm)
    {
      if (hasBehaviourVariableModifier(enm.m_modifiers, bvmEnumNoInstance))
        continue;

      code.write("%s enum_%s;", enm.m_name.c_str(), enm.m_name.c_str());
      numIntsExpected ++;
    }
    iterate_end
  }
  iterate_end

  iterate_begin_const(BehaviourDef::AttributeVariablePtr, bdef.m_sortedAttributes, av)
  {
    if (av->getType() == bhvtBool)
    {
      writeAttributeVariable(code, *av);
    }
  }
  iterate_end

  code.newLines(2);
  code.write("public:");
  code.newLine();

  code.write("%sBehaviourData()", bdef.m_name.c_str());
  code.openBrace();

  iterate_begin_const(BehaviourDef::AttributeGroup, bdef.m_attributeGroups, ag)
  {
    iterate_begin_const(BehaviourDef::AttributeVariable, ag.m_vars, av)
    {
      if (av.isNotArray())
      {
        switch (av.getType())
        {
          case bhvtPhysicsObjectID:
          case bhvtInteger:
            code.write("%s = %i;", av.m_name.c_str(), av.m_default.i);
            break;

          case bhvtBool:
            code.write("%s = %s;", av.m_name.c_str(), av.m_default.b?"true":"false");
            break;

          case bhvtFloat:
            code.write("%s = float(%g);", av.m_name.c_str(), av.m_default.f);
            break;

          case bhvtVector3:
            code.write("%s.set(float(%g), float(%g), float(%g));", av.m_name.c_str(), av.m_default.v[0], av.m_default.v[1], av.m_default.v[2]);
            break;

          default:
            NMP_ASSERT_FAIL();
            break;
        }
      }
      else
      {
        for (unsigned int i=0; i<av.getArraySize(); i++)
        {
          switch (av.getType())
          {
            case bhvtPhysicsObjectID:
            case bhvtInteger:
              code.write("%s[%i] = %i;", av.m_name.c_str(), i, av.m_default.i);
              break;

            case bhvtBool:
              code.write("%s[%i] = %s;", av.m_name.c_str(), i, av.m_default.b?"true":"false");
              break;

            case bhvtFloat:
              code.write("%s[%i] = float(%g);", av.m_name.c_str(), i, av.m_default.f);
              break;

            case bhvtVector3:
              code.write("%s[%i].set(float(%g), float(%g), float(%g));", av.m_name.c_str(), i, av.m_default.v[0], av.m_default.v[1], av.m_default.v[2]);
              break;

            default:
              NMP_ASSERT_FAIL();
              break;
          }
        }
      }
    }
    iterate_end

    // support default enum entry modifier
    iterate_begin_const(BehaviourDef::AttributeGroup::EnumDecl, ag.m_enumDecl, enm)
    {
      if (hasBehaviourVariableModifier(enm.m_modifiers, bvmEnumNoInstance))
        continue;

      if (const ModifierOption* enmDefault = hasBehaviourVariableModifier(enm.m_modifiers, bvmEnumDefaultIndex))
      {
       code.write("enum_%s = %s;", enm.m_name.c_str(), enmDefault->m_value.c_str());
      }
      else
      {
       code.write("enum_%s = %s;", enm.m_name.c_str(), enm.m_entries[0].m_name.c_str());
      }
    }
    iterate_end
  }
  iterate_end

  if (!bdef.m_emittedMessages.empty())
  {
    code.write("m_messages = uint32_t(0);");
  }

  code.closeBrace();
  code.newLine();

  // -----------------------------------------------------------------------------------------------

  code.write("void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);");
  code.newLine();

  // -----------------------------------------------------------------------------------------------

  code.write("void updateAndClearEmittedMessages(uint32_t& messages);");
  code.newLine();

  // -----------------------------------------------------------------------------------------------
  // write the decoder
  {
      const bool zeroNonCpAttribs = 
        (numFloatsExpected) == 0 && 
        (numIntsExpected ) == 0 &&
        (numUInt64sExpected) == 0;
    // This fn only operates on non-cp default value attributes.
    code.write("void updateFromMessageParams(const ER::AttribDataBehaviourParameters* %s)",
      (zeroNonCpAttribs)?"NMP_USED_FOR_ASSERTS(data)":"data");
    code.openBrace();

    code.write("NMP_ASSERT(data->m_floats->m_numValues == %i);", numFloatsExpected);
    code.write("NMP_ASSERT(data->m_ints->m_numValues == %i);", numIntsExpected);
    code.write("NMP_ASSERT(data->m_uint64s->m_numValues == %i);", numUInt64sExpected);

    if (!zeroNonCpAttribs)
    {
      code.newLine();
    }

    // Initialize indexs for reading values from behaviourParams - offset so as to ignore default CP values
    unsigned int floatIndex = 0;
    unsigned int intIndex = 0;
    unsigned int uint64Index = 0;

    if (!zeroNonCpAttribs)
    {
      code.newLine();
    }

    iterate_begin_const(BehaviourDef::AttributeVariablePtr, bdef.m_messageParamAttributes, avPtr)
    {
      std::string variableName = avPtr->m_name.c_str();

      if (avPtr->isCpDefault())
      {
      }
      else if (avPtr->isNotArray())
      {
        switch (avPtr->getType())
        {
        case bhvtPhysicsObjectID:
          code.write("%s = data->m_uint64s->m_values[%i];", variableName.c_str(), uint64Index++);
          break;

        case bhvtInteger:
          code.write("%s = data->m_ints->m_values[%i];", variableName.c_str(), intIndex++);
          break;

        case bhvtBool:
          code.write("%s = (data->m_ints->m_values[%i] != 0);", variableName.c_str(), intIndex++);
          break;

        case bhvtFloat:
          code.write("%s = data->m_floats->m_values[%i];", variableName.c_str(), floatIndex++);
          break;

        case bhvtVector3:
          code.write("%s.set(data->m_floats->m_values[%i], data->m_floats->m_values[%i], data->m_floats->m_values[%i]);", variableName.c_str(), floatIndex, floatIndex+1, floatIndex+2);
          floatIndex+=3;
          break;

        default:
          NMP_ASSERT_FAIL();
          break;
        }
      }
      else
      {
        for (unsigned int i=0; i<avPtr->getArraySize(); i++)
        {
          switch (avPtr->getType())
          {
          case bhvtPhysicsObjectID:
            code.write("%s[%i] = data->m_uint64s->m_values[%i];", variableName.c_str(), i, uint64Index++);
            break;

          case bhvtInteger:
            code.write("%s[%i] = data->m_ints->m_values[%i];", variableName.c_str(), i, intIndex++);
            break;

          case bhvtBool:
            code.write("%s[%i] = (data->m_ints->m_values[%i] != 0);", variableName.c_str(), i, intIndex++);
            break;

          case bhvtFloat:
            code.write("%s[%i] = data->m_floats->m_values[%i];", variableName.c_str(), i, floatIndex++);
            break;

          case bhvtVector3:
            code.write("%s[%i].set(data->m_floats->m_values[%i], data->m_floats->m_values[%i], data->m_floats->m_values[%i]);", variableName.c_str(), i, floatIndex, floatIndex+1, floatIndex+2);
            floatIndex+=3;
            break;

          default:
            NMP_ASSERT_FAIL();
            break;
          }
        }
      }
    }
    iterate_end

    iterate_begin_const(BehaviourDef::AttributeGroup, bdef.m_attributeGroups, ag)
    {
      iterate_begin_const(BehaviourDef::AttributeGroup::EnumDecl, ag.m_enumDecl, enm)
      {
        if (hasBehaviourVariableModifier(enm.m_modifiers, bvmEnumNoInstance))
          continue;

        code.write("enum_%s = (%s)data->m_ints->m_values[%i];", enm.m_name.c_str(), enm.m_name.c_str(), intIndex++);
      }
      iterate_end
    }
    iterate_end

    iterate_begin_const(BehaviourDef::AnimationInput, bdef.m_animationInputs, ain)
    {
      code.newLine();

      int poseIndex = 0;
      iterate_begin_const_ptr(LimbInstance, limbInstances, limbInst)
      {
        code.write("pd_%s.m_poseWeights[%i] = %s_ApplyTo%s;", ain.m_name.c_str(), poseIndex, ain.m_name.c_str(), limbInst->m_name.c_str());
        poseIndex ++;
      }
      iterate_end
    }
    iterate_end

    code.closeBrace();
    code.newLine();
  }

  unsigned int numCpFloatsExpected = 0;
  unsigned int numCpIntsExpected = 0;
  unsigned int numCpUInt64sExpected = 0;
  unsigned int numCpVector3sExpected = 0; 

  // Copy values from message params directly into m_cp struct
  iterate_begin_const(BehaviourDef::ControlParamGroup, bdef.m_controlParamGroups, cg)
  {
    iterate_begin_const(BehaviourDef::ControlParam, cg.m_params, cp)
    {
      numCpFloatsExpected += cp.getNumFloats();
      numCpIntsExpected += cp.getNumInts();
      numCpUInt64sExpected += cp.getNumUInt64s();
      numCpVector3sExpected += cp.getNumVector3s();
    }
    iterate_end
  }
  iterate_end

  // -----------------------------------------------------------------------------------------------
  // UpdateFromControlParams
  if (!bdef.m_controlParamGroups.empty())
  {
    code.writeHorizontalSeparator();

    code.write("void updateFromControlParams(const ER::AttribDataBehaviourState* const data)", bdef.m_name.c_str());
    code.openBrace();

    code.write("NMP_ASSERT(data->m_floats->m_numValues == %i);", numCpFloatsExpected);
    code.write("NMP_ASSERT(data->m_ints->m_numValues == %i);", numCpIntsExpected);
    code.write("NMP_ASSERT(data->m_uint64s->m_numValues == %i);", numCpUInt64sExpected);
    code.write("// Vector 3 are stored in a float array with 4 entries per item");
    code.write("NMP_ASSERT(data->m_vector3Data->m_numValues == %i);", numCpVector3sExpected * 4);
    code.newLine();

    unsigned int floatIndex = 0, intIndex = 0, uint64Index = 0, vector3Index =0;

    // Copy values from message params directly into m_cp struct
    iterate_begin_const(BehaviourDef::ControlParamGroup, bdef.m_controlParamGroups, cg)
    {
      iterate_begin_const(BehaviourDef::ControlParam, cg.m_params, cp)
      {
        std::string variableName = cp.m_name.c_str();

        // Prefix name with "m_cp."
        variableName.insert(0, "m_cp.");

        if (cp.isNotArray())
        {
          switch (cp.getType())
          {
          case bhvtPhysicsObjectID:
            code.write("%s = (void*) (data->m_uint64s->m_values[%i]);", variableName.c_str(), uint64Index++);
            break;

          case bhvtInteger:
            code.write("%s = data->m_ints->m_values[%i];", variableName.c_str(), intIndex++);
            break;

          case bhvtBool:
            code.write("%s = (data->m_ints->m_values[%i] != 0);", variableName.c_str(), intIndex++);
            break;

          case bhvtFloat:
            code.write("%s = data->m_floats->m_values[%i];", variableName.c_str(), floatIndex++);
            break;

          case bhvtVector3:
            code.write("%s.set(data->m_vector3Data->m_values[%i], data->m_vector3Data->m_values[%i], data->m_vector3Data->m_values[%i]);", variableName.c_str(), vector3Index, vector3Index+1, vector3Index+2);
            vector3Index+=4;
            break;

          default:
            NMP_ASSERT_FAIL();
            break;
          }
        }
        else
        {
          for (unsigned int i=0; i<cp.getArraySize(); i++)
          {
            switch (cp.getType())
            {
            case bhvtPhysicsObjectID:
              code.write("%s[%i] = (void*) (data->m_uint64s->m_values[%i]);", cp.m_name.c_str(), i, uint64Index++);
              break;

            case bhvtInteger:
              code.write("%s[%i] = data->m_ints->m_values[%i];", variableName.c_str(), i, intIndex++);
              break;

            case bhvtBool:
              code.write("%s[%i] = (data->m_ints->m_values[%i] != 0);", variableName.c_str(), i, intIndex++);
              break;

            case bhvtFloat:
              code.write("%s[%i] = data->m_floats->m_values[%i];", variableName.c_str(), i, floatIndex++);
              break;

            case bhvtVector3:
              code.write("%s[%i].set(data->m_vector3Data->m_values[%i], data->m_vector3Data->m_values[%i], data->m_vector3Data->m_values[%i]);", variableName.c_str(), i, vector3Index, vector3Index+1, vector3Index+2);
              vector3Index+=4;
              break;

            default:
              NMP_ASSERT_FAIL();
              break;
            }
          }
        }
      }
      iterate_end
    }
    iterate_end

    code.closeBrace();

    code.newLine();
  }

  code.newLine();

  // -----------------------------------------------------------------------------------------------
  // write accessors

  char avNameCapFront[512];
  iterate_begin_const(BehaviourDef::AttributeGroup, bdef.m_attributeGroups, ag)
  {
    // Attribute group comment.
    if (!ag.m_comment.empty())
    {
      writeComment(code, ag.m_comment.c_str(), true);
      code.newLines(2);
    }

    iterate_begin_const(BehaviourDef::AttributeVariable, ag.m_vars, av)
    {
      if (hasBehaviourVariableModifier(av.m_modifiers, bvmPrivate))
      {
        continue;
      }

      // Attribute variable comment.
      if (!av.m_comment.empty())
      {
        writeComment(code, av.m_comment.c_str(), true);
        code.newLine();
      }

      code.writeTabs();
      switch (av.getType())
      {
        case bhvtPhysicsObjectID:
          code.writeRaw("void* ");
          break;

        case bhvtInteger:
          code.writeRaw("int32_t ");
          break;

        case bhvtBool:
          code.writeRaw("bool ");
          break;

        case bhvtFloat:
          code.writeRaw("float ");
          break;

        case bhvtVector3:
          code.writeRaw("const NMP::Vector3& ");
          break;

        default:
          NMP_ASSERT_FAIL();
          break;
      }

      NMP_ASSERT(av.m_name.length() < 512);
      NMP_STRNCPY_S(avNameCapFront, sizeof(avNameCapFront), av.m_name.c_str());
      avNameCapFront[0] = (char)toupper(avNameCapFront[0]);

      if (av.isNotArray())
      {
        code.writeRaw("get%s() const { return %s; }", avNameCapFront, av.m_name.c_str());
        code.newLine();
      }
      else
      {
        code.writeRaw("get%s(unsigned int index) const { NMP_ASSERT(index < %i); return %s[index]; }", avNameCapFront, av.getArraySize(), av.m_name.c_str());
        code.newLine();

        code.write("enum { max%s = %i };", avNameCapFront, av.getArraySize());
      }

    }
    iterate_end

    iterate_begin_const(BehaviourDef::AttributeGroup::EnumDecl, ag.m_enumDecl, enm)
    {
      if (hasBehaviourVariableModifier(enm.m_modifiers, bvmPrivate) ||
          hasBehaviourVariableModifier(enm.m_modifiers, bvmEnumNoInstance))
        continue;

      NMP_STRNCPY_S(avNameCapFront, sizeof(avNameCapFront), enm.m_name.c_str());
      avNameCapFront[0] = (char)toupper(avNameCapFront[0]);

      // Enum comment.
      if (!enm.m_comment.empty())
      {
        writeComment(code, enm.m_comment.c_str(), true);
        code.newLine();
      }

      code.write("%s get%s() const { return enum_%s; }", enm.m_name.c_str(), avNameCapFront, enm.m_name.c_str());
    }
    iterate_end

    code.newLine();
  }
  iterate_end

  // accessors for body pose data
  bool spacingRequired = false;
  iterate_begin_const(BehaviourDef::AnimationInput, bdef.m_animationInputs, ain)
  {
      spacingRequired = true;

      // Animation input comment.
      if (!ain.m_comment.empty())
      {
        writeComment(code, ain.m_comment.c_str(), true);
        code.newLine();
      }
      code.write("const BodyPoseData& get%s() const { return pd_%s; }", ain.m_name.c_str(), ain.m_name.c_str());
    }
  iterate_end
  if (spacingRequired)
    code.newLine();

  code.writeComment("control parameters");
  iterate_begin_const(BehaviourDef::ControlParamGroup, bdef.m_controlParamGroups, cg)
  {
    // Control parameter group comment.
    if (!cg.m_comment.empty())
    {
      writeComment(code, cg.m_comment.c_str(), true);
      code.newLines(2);
    }

    iterate_begin_const(BehaviourDef::ControlParam, cg.m_params, cp)
    {
      if (hasBehaviourVariableModifier(cp.m_modifiers, bvmPrivate))
        continue;

      // Control parameter comment.
      if (!cp.m_comment.empty())
      {
        writeComment(code, cp.m_comment.c_str(), true);
        code.newLine();
      }

      code.writeTabs();
      switch(cp.getType())
      {
      case bhvtFloat:
        code.writeRaw("float ");
        break;

      case bhvtInteger:
        code.writeRaw("int ");
        break;

      case bhvtBool:
        code.writeRaw("bool ");
        break;

      case bhvtVector3:
        code.writeRaw("const NMP::Vector3& ");
        break;

      case bhvtPhysicsObjectID:
        code.writeRaw("void* ");
        break;

      default:
        NMP_ASSERT_FAIL();
        break;
      }

      NMP_ASSERT(cp.m_name.length() < 512);
      NMP_STRNCPY_S(avNameCapFront, sizeof(avNameCapFront), cp.m_name.c_str());
      avNameCapFront[0] = (char)toupper(avNameCapFront[0]);

      if (cp.isNotArray())
      {
        code.writeRaw("get%sCP() const { return m_cp.%s; }", avNameCapFront, cp.m_name.c_str());
        code.newLine();
      }
      else
      {
        code.writeRaw("get%sCP(unsigned int index) const { NMP_ASSERT(index < %i); return m_cp.%s[index]; }", avNameCapFront, cp.getArraySize(), cp.m_name.c_str());
        code.newLine();

        code.write("enum { max%sCP = %i };", avNameCapFront, cp.getArraySize());
      }
    }
    iterate_end
  }
  iterate_end

  code.newLine();
  code.writeComment("output control parameters");

  // -----------------------------------------------------------------------------------------------

  if (!bdef.m_sortedOutputControlParams.empty())
  {
    code.newLine();
    code.write("void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }");
    code.newLine();
  }

  // -----------------------------------------------------------------------------------------------

  iterate_begin_const(BehaviourDef::OutputControlParam, bdef.m_outputControlParams, cp)
  {
    if (hasBehaviourVariableModifier(cp.m_modifiers, bvmPrivate))
      continue;

    // Output Control Param comment.
    if (!cp.m_comment.empty())
    {
      writeComment(code, cp.m_comment.c_str(), true);
      code.newLine();
    }

    code.writeTabs();

    char *valueType = "void";

    switch(cp.getType())
    {
    case bhvtFloat:
      valueType = "float";
      break;

    case bhvtInteger:
      valueType = "int";
      break;

    case bhvtPhysicsObjectID:
      valueType = "void*";
      break;

    case bhvtBool:
      valueType = "bool";
      break;

    case bhvtVector3:
      valueType = "const NMP::Vector3& ";
      break;

    default:
      NMP_ASSERT_FAIL();
      break;
    }

    NMP_ASSERT(cp.m_name.length() < 512);
    NMP_STRNCPY_S(avNameCapFront, sizeof(avNameCapFront), cp.m_name.c_str());
    avNameCapFront[0] = (char)toupper(avNameCapFront[0]);

    if (cp.isNotArray())
    {
      code.writeRaw("void set%sOCP(%s value) { m_ocp.%s = value; }", avNameCapFront, valueType, cp.m_name.c_str());
      code.newLine();
    }
    else
    {
      code.writeRaw("void set%sOCP(unsigned int index, %s value) { NMP_ASSERT(index < %i); m_ocp.%s[index] = value; }", 
        avNameCapFront, valueType, cp.getArraySize(), cp.m_name.c_str());
      code.newLine();

      code.write("enum { max%sOCP = %i };", avNameCapFront, cp.getArraySize());
    }
  }
  iterate_end;

  if (!bdef.m_emittedMessages.empty())
  {
    code.write("void setMessage(uint32_t value) { m_messages |= value; }");
    code.write("void resetMessages() { m_messages = 0; }");
    code.newLine();
  }

  code.closeBraceWithSemicolon();  // struct
  code.newLines(2);


  // behaviour class
  code.write("class %sBehaviour : public ER::Behaviour", bdef.m_name.c_str());
  code.openBrace();
  code.write("public:");

  code.write("virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;");
  code.write("virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;");
  code.write("virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;");
  code.write("virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;");
  code.write("virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;");
  code.write("virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;");
  code.write("virtual const char* getBehaviourName() const NM_OVERRIDE { return \"%s\"; }", bdef.m_name.c_str());
  code.write("virtual int getBehaviourID() const NM_OVERRIDE { return %i; }", bdef.m_behaviourID);
  code.write("virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);");
  code.write("virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);");
  if (!bdef.m_sortedOutputControlParams.empty())
  {
    code.write("virtual void clearOutputControlParamsData() NM_OVERRIDE;");
  }

  code.write("class %sBehaviourDef* m_definition;", bdef.m_name.c_str());
  code.newLine();

  // ID tables
  if (!bdef.m_animationInputs.empty())
  {
    code.write("enum AnimInputIDs");
    code.openBrace();
    iterate_begin_const(BehaviourDef::AnimationInput, bdef.m_animationInputs, ain)
    {
      code.write("%s_ID = %i,", ain.m_name.c_str(), ain.m_ID);
    }
    iterate_end;
    code.closeBraceWithSemicolon();
    code.newLine();
  }
  if (!bdef.m_controlParamGroups.empty())
  {
    code.write("enum ControlParamIDs");
    code.openBrace();
    iterate_begin_const(BehaviourDef::ControlParamGroup, bdef.m_controlParamGroups, cg)
    {
      iterate_begin_const(BehaviourDef::ControlParam, cg.m_params, cp)
      {
        if (cp.isNotArray())
        {
          code.write("%s_CP_ID = %i,", cp.m_name.c_str(), cp.m_ID);
        }
        else
        {
          for (unsigned int i=0; i<cp.getArraySize(); i++)
            code.write("%s_%i_CP_ID = %i,", cp.m_name.c_str(), i, cp.m_ID + i);
        }
      }
      iterate_end;
    }
    iterate_end;
    code.closeBraceWithSemicolon();
    code.newLine();
  }
  if (!bdef.m_outputControlParams.empty())
  {
    code.write("enum OutputControlParamIDs");
    code.openBrace();
    iterate_begin_const(BehaviourDef::OutputControlParam, bdef.m_outputControlParams, cp)
    {
      if (cp.isNotArray())
      {
        code.write("%s_OCP_ID = %i,", cp.m_name.c_str(), cp.m_ID);
      }
      else
      {
        for (unsigned int i=0; i<cp.getArraySize(); i++)
          code.write("%s_%i_OCP_ID = %i,", cp.m_name.c_str(), i, cp.m_ID + i);
      }
    }
    iterate_end;
    code.closeBraceWithSemicolon();
    code.newLine();
  }

  // Emitted message
  if (!bdef.m_emittedMessages.empty())
  {
    code.write("enum EmittedMessages");
    code.openBrace();
    unsigned int i = 0;
    iterate_begin_const(BehaviourDef::EmittedMessage, bdef.m_emittedMessages, er)
    {
      code.write("%s = (1 << %i),", er.m_name.c_str(), i++);
    }
    iterate_end;
    code.closeBraceWithSemicolon();
    code.newLine();
  }

  code.write("const %sBehaviourData& getParams() const { return m_params; }", bdef.m_name.c_str());
  code.write("%sBehaviourData& getParams() { return m_params; }", bdef.m_name.c_str());
  code.write("%sPoseRequirements& getPoseRequirements() { return m_poseRequirements; }", bdef.m_name.c_str());
  code.newLine();
  code.write("protected:");
  code.write("%sBehaviourData m_params;", bdef.m_name.c_str());
  code.write("%sPoseRequirements m_poseRequirements;", bdef.m_name.c_str());

  code.closeBraceWithSemicolon();
  code.newLine();

  // behaviour def
  code.write("class %sBehaviourDef : public ER::BehaviourDef", bdef.m_name.c_str());
  code.openBrace();
  code.write("public:");

  code.write("static NM_INLINE NMP::Memory::Format getMemoryRequirements()");
  code.openBrace();
  code.write("NMP::Memory::Format result(sizeof(%sBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);", bdef.m_name.c_str());
  code.write("return result;");
  code.closeBrace();
  code.newLine();

  code.write("inline %sBehaviourDef() {}", bdef.m_name.c_str());
  code.write("virtual ER::Behaviour* newInstance() const NM_OVERRIDE");
  code.openBrace();
  code.write("void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(%sBehaviour), NMP_VECTOR_ALIGNMENT);", bdef.m_name.c_str());
  code.write("return new(alignedMemory) %sBehaviour;", bdef.m_name.c_str());
  code.closeBrace();
  code.newLine(); 
  code.write("virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;");
  code.closeBraceWithSemicolon();
  code.newLine();

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


// ---------------------------------------------------------------------------------------------------------------------
/**
 * This function is used to expand a module enabler path. because we can have arrays of modules as children, we can therefore
 * specify a module enabler path like:
 *
 * root.arms[1].*;            <- enable all children of the second entry of arms[]
 * root.arms[*];              <- enable all arms in the array
 * root.arms[0].point[*].*    <- enable all the children of all the point modules in the first entry of arms[]
 *
 * it can get quite complicated. Whenever we hit an array specifier, we then need to fork and potentially iterate across
 * all the children, building the path out for each one in turn.
 *
 * root.arms[*];    with 3 arms, gets expanded into
 * 
 * root_arms_0;
 * root_arms_1;
 * root_arms_2;
 *
 * if we add more complexity, root.arms[*].point[*], we get
 *
 * root_arms_0_point_0;
 * root_arms_0_point_1;
 * root_arms_1_point_0;
 * root_arms_1_point_1;
 * root_arms_2_point_0;
 * root_arms_2_point_1;
 *
 * 
 */
void expandModuleEnablerPath(CodeWriter& code, const BehaviourDef::ModuleToEnable& mte, unsigned int index, StringBuilder& sb)
{
  unsigned int i, pathLen = mte.m_path.count();

  // if m_allChildren is true, the path is terminated with a .*, which we don't want to care about
  if (mte.m_allChildren)
    pathLen --;

  for (i=index; i<pathLen; i++)
  {
    BehaviourDef::ModuleToEnable::PathItem *pi = mte.m_path.get(i);

    // add the child name, prepending a _ if we are not the first item in the path
    if (i > 0)
      sb.appendChar('_');
    sb.appendPString(pi->m_childInst->m_name);

    // this entry deals with an array of modules
    if (pi->m_arrayAccess)
    {
      // we need to iterate over all the modules in the array
      if (pi->m_arrayIndex == -1)
      {
        unsigned int j, arraySizeToIterate = pi->m_childInst->m_arraySize;

        for (j=0; j<arraySizeToIterate; j++)
        {
          StringBuilder forkedSB(sb);
          forkedSB.appendf("_%i", j);
          expandModuleEnablerPath(code, mte, i + 1, forkedSB);
        }
      }
      // we just want a single entry in the array
      else
      {
        StringBuilder forkedSB(sb);
        forkedSB.appendf("_%i", pi->m_arrayIndex);
        expandModuleEnablerPath(code, mte, i + 1, forkedSB);
      }

      // bailing out here ensures we don't add any non-indexed entries to the list
      return;
    }
  }

  // emit the final path
  code.write("{ NetworkManifest::%s, %s },",
    sb.c_str(),
    mte.m_allChildren?"true":"false");

  return;
}


// ---------------------------------------------------------------------------------------------------------------------
/**
 * 
 */
bool BehaviourGen::writeBehaviourClass(const BehaviourDef &bdef, CodeWriter& code, const char*)
{
  code.writeNMCopyrightHeader();

  code.write("#include \"Behaviours/%sBehaviour.h\"", bdef.m_name.c_str());
  code.write("#include \"euphoria/erBody.h\"");
  code.write("#include \"euphoria/erCharacter.h\"");
  code.write("#include \"euphoria/erAttribData.h\"");
  code.newLines(2);

  // module enabler
  code.writeHorizontalSeparator();
  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");

  code.write("const ER::BehaviourDef::ModuleToEnable* %sBehaviourDef::getModulesToEnable(uint32_t& numToEnable)", bdef.m_name.c_str());
  code.openBrace();

  int numModulesToEnable = (int)bdef.m_modulesToEnable.size();

  if (numModulesToEnable > 0)
  {
    code.write("static const ER::BehaviourDef::ModuleToEnable mteDef[] =", numModulesToEnable);
    code.openBrace();
    for (BehaviourDef::ModulesToEnable::const_iterator it = bdef.m_modulesToEnable.begin(), end = bdef.m_modulesToEnable.end(); 
      it != end; 
      ++it)
    {
      const BehaviourDef::ModuleToEnable &mte = (*it);

      // single operand targeting the root module (special case for '*;' syntax)
      if (mte.m_targetIsRoot)
      {
#ifdef _DEBUG
        unsigned int pathLen = mte.m_path.count(); sizeof(pathLen);
        assert(pathLen == 1);

        BehaviourDef::ModuleToEnable::PathItem *pi = mte.m_path.get(0); sizeof(pi);
        assert(pi->m_childInst == 0);
        assert(mte.m_allChildren);
#endif
        code.write("{ NetworkManifest::rootModule, true },");
      }
      else
      {
        StringBuilder piSB(32);
        expandModuleEnablerPath(code, mte, 0, piSB);
      }
    }
    code.closeBraceWithSemicolon();
    code.write("numToEnable = sizeof(mteDef) / sizeof(mteDef[0]);");
    code.write("return mteDef;");
  }
  else
  {
    code.write("numToEnable = 0;");
    code.write("return 0;");
  }


  code.closeBrace();
  code.newLine();

  // update params from attribute data
  code.writeHorizontalSeparator();
  if (!bdef.m_controlParamGroups.empty())
  {
    code.write("void %sBehaviour::interpretControlParams(const ER::AttribDataBehaviourState* data)", bdef.m_name.c_str());
    code.openBrace();
    code.write("m_params.updateFromControlParams(data);");
    code.closeBrace();
  }
  else
  {
    code.write("void %sBehaviour::interpretControlParams(const ER::AttribDataBehaviourState* NMP_UNUSED(data))", bdef.m_name.c_str());
    code.openBrace();
    code.closeBrace();
  }
  code.newLine();


  // update params from attribute data
  code.writeHorizontalSeparator();
  code.write("void %sBehaviour::interpretMessageData(const ER::AttribDataBehaviourParameters* data)", bdef.m_name.c_str());
  code.openBrace();
  code.write("m_params.updateFromMessageParams(data);");
  code.closeBrace();
  code.newLine();

  // animation input handler
  code.writeHorizontalSeparator();

  // write out enabled handlers for animation inputs
  bool haveBodyPoseInputs = bdef.m_animationInputs.begin() != bdef.m_animationInputs.end();
  
  if (haveBodyPoseInputs)
  {
    code.write("bool %sBehaviour::isAnimationRequired(int32_t animationID) const", bdef.m_name.c_str());
    code.openBrace();
    code.write("switch (animationID)");
    code.openBrace();
    iterate_begin_const(BehaviourDef::AnimationInput, bdef.m_animationInputs, ain)
    {
        code.write("case %s_ID:  /* %i */", ain.m_name.c_str(), ain.m_ID);
        code.write("  return m_poseRequirements.enablePoseInput_%s;", ain.m_name.c_str());
        code.newLine();
      }
    iterate_end

    code.write("default:");
    code.write("NMP_DEBUG_MSG(\"Unexpected animation ID passed to %s\");", bdef.m_name.c_str());
    code.write("NMP_ASSERT_FAIL();");
    code.write("break;");

    code.closeBrace();
    code.write("return false;");
  }
  else
  {
    code.write("bool %sBehaviour::isAnimationRequired(int32_t NMP_UNUSED(animationID)) const", bdef.m_name.c_str());
    code.openBrace();
    code.write("return false;");
  }
  code.closeBrace();
  code.newLine();


  // write out handlers for animation inputs
  if (!bdef.m_animationInputs.empty())
  {
    code.write("void %sBehaviour::interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body)", bdef.m_name.c_str());
    code.openBrace();

    code.write("switch (animationData->m_animationID)");
    code.openBrace();
  
    iterate_begin_const(BehaviourDef::AnimationInput, bdef.m_animationInputs, ain)
    {
      code.write("case %s_ID:  /* %i */", ain.m_name.c_str(), ain.m_ID);

          code.openBrace();
          {
            code.write("int iNetworkLimb = 0; // Based on network indexes.");
            code.write("int iRigLimb = 0; // Based on rig indexes.");
            code.write("for (int iLimb = 0 ; iLimb < body->getNumArms() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)");
            code.openBrace();
            code.write("m_params.pd_%s.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(", ain.m_name.c_str());
            code.write("    animationData->m_TMs);");
            code.closeBrace();
            code.write("iNetworkLimb = NetworkConstants::networkMaxNumArms;");
            code.write("for (int iLimb = 0 ; iLimb < body->getNumHeads() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)");
            code.openBrace();
            code.write("m_params.pd_%s.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(", ain.m_name.c_str());
            code.write("    animationData->m_TMs);");
            code.closeBrace();
            code.write("iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads;");
            code.write("for (int iLimb = 0; iLimb < body->getNumLegs() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)");
            code.openBrace();
            code.write("m_params.pd_%s.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(", ain.m_name.c_str());
            code.write("    animationData->m_TMs);");
            code.closeBrace();
            code.write("iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads + NetworkConstants::networkMaxNumLegs;");
            code.write("for (int iLimb = 0; iLimb < body->getNumSpines() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)");
            code.openBrace();
            code.write("m_params.pd_%s.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(", ain.m_name.c_str());
            code.write("    animationData->m_TMs);");
            code.closeBrace();
            code.write("m_params.pd_%s.m_poseWeight = animationData->m_isSet ? 1.0f : 0.0f;", ain.m_name.c_str());
          }
          code.closeBrace();

      code.write("break;");
      code.newLine();
    }
    iterate_end
  
    code.write("default:");
    code.write("NMP_DEBUG_MSG(\"Unexpected animation ID passed to %s\");", bdef.m_name.c_str());
    code.write("NMP_ASSERT_FAIL();");
    code.write("break;");
    code.closeBrace();
  }
  else
  {
    code.write("void %sBehaviour::interpretAnimationMessage(const ER::BehaviourAnimationData*, ER::Body*)", bdef.m_name.c_str());
    code.openBrace();
  }

  code.closeBrace();
  code.newLine();

  // store state
  code.writeHorizontalSeparator();
  code.write("bool %sBehaviour::storeState(MR::PhysicsSerialisationBuffer& savedState)", bdef.m_name.c_str());
  code.openBrace();
  code.write("savedState.addValue(m_params);");
  code.write("savedState.addValue(m_poseRequirements);");
  code.write("return true;");
  code.closeBrace();
  code.newLine();

  // restore state
  code.writeHorizontalSeparator();
  code.write("bool %sBehaviour::restoreState(MR::PhysicsSerialisationBuffer& savedState)", bdef.m_name.c_str());
  code.openBrace();
  code.write("m_params = savedState.getValue(m_params);");
  code.write("m_poseRequirements = savedState.getValue(m_poseRequirements);");
  code.write("return true;");
  code.closeBrace();
  code.newLine();

  // handle output control params
  code.writeHorizontalSeparator();
  code.write("void %sBehaviour::handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)", bdef.m_name.c_str());
  code.openBrace();
  code.write("m_params.updateOutputControlParams(outputControlParams, numOutputControlParams);");
  code.closeBrace();
  code.newLine();

  if (!bdef.m_sortedOutputControlParams.empty())
  {
    // clear output control params data
    code.writeHorizontalSeparator();
    code.write("void %sBehaviour::clearOutputControlParamsData()", bdef.m_name.c_str());
    code.openBrace();
    code.write("m_params.clearOutputControlParamsData();");
    code.closeBrace();
    code.newLine();
  }

  // handle EmittedMessages
  code.writeHorizontalSeparator();
  code.write("void %sBehaviour::handleEmittedMessages(uint32_t& messages)", bdef.m_name.c_str());
  code.openBrace();
  code.write("m_params.updateAndClearEmittedMessages(messages);");
  code.closeBrace();
  code.newLine();

  // UpdateOutputControlParams
  code.writeHorizontalSeparator();

  if (!bdef.m_outputControlParams.empty())
  {
    code.write("void %sBehaviourData::updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)", bdef.m_name.c_str());
    code.openBrace();

    code.write("for (size_t iOCP = 0 ; iOCP < numOutputControlParams ; ++iOCP)");
    code.openBrace();

    code.write("ER::AttribDataBehaviourParameters::OutputControlParam& outputControlParam = outputControlParams[iOCP];");

    code.write("switch (iOCP)");
    code.openBrace();

    iterate_begin_const(BehaviourDef::OutputControlParam, bdef.m_outputControlParams, cp)
    {
      if (cp.isNotArray())
      {
        code.write("case %sBehaviour::%s_OCP_ID:  /* %i */", bdef.m_name.c_str(), cp.m_name.c_str(), cp.m_ID);
        switch(cp.getType())
        {
        case bhvtFloat:
          code.write("NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);");
          code.write("*((float*) outputControlParam.data) = m_ocp.%s;", cp.m_name.c_str());
          break;

        case bhvtInteger:
          code.write("NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_INT);");
          code.write("*((int32_t*) outputControlParam.data) = m_ocp.%s;", cp.m_name.c_str());
          break;

        case bhvtBool:
          code.write("NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_BOOL);");
          code.write("*((bool*) outputControlParam.data) = m_ocp.%s;", cp.m_name.c_str());
          break;

        case bhvtVector3:
          code.write("NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_VECTOR3);");
          code.write("*((NMP::Vector3*) outputControlParam.data) = m_ocp.%s;", cp.m_name.c_str());
          break;

        case bhvtPhysicsObjectID:
          code.write("NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER);");
          code.write("*((void**) outputControlParam.data) = m_ocp.%s;", cp.m_name.c_str());
          break;

        default:
          NMP_ASSERT_FAIL();
          break;
        }
        code.write("break;");
        code.newLine();
      }
      else
      {
        for (unsigned int i=0; i<cp.getArraySize(); i++)
        {
          code.write("case %sBehaviour::%s_%i_OCP_ID:  /* %i */", bdef.m_name.c_str(), cp.m_name.c_str(), i, cp.m_ID + i);
          switch(cp.getType())
          {
          case bhvtFloat:
            code.write("NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);");
            code.write("*((float*) outputControlParam.data) = m_ocp.%s[%i];", cp.m_name.c_str(), i);
            break;

          case bhvtInteger:
            code.write("NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_INT);");
            code.write("*((int32_t*) outputControlParam.data) = m_ocp.%s[%i];", cp.m_name.c_str(), i);
            break;

          case bhvtBool:
            code.write("NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_BOOL);");
            code.write("*((bool*) outputControlParam.data) = m_ocp.%s[%i];", cp.m_name.c_str(), i);
            break;

          case bhvtVector3:
            code.write("NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_VECTOR3);");
            code.write("*((NMP::Vector3*) outputControlParam.data) = m_ocp.%s[%i];", cp.m_name.c_str(), i);
            break;

          case bhvtPhysicsObjectID:
            code.write("NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER);");
            code.write("*((void**) outputControlParam.data) = m_ocp.%s[%i];", cp.m_name.c_str(), i);
            break;

          default:
            NMP_ASSERT_FAIL();
            break;
          }
          code.write("break;");
          code.newLine();
        }
      }
    }
    iterate_end;

    code.write("default:");
    code.write("NMP_DEBUG_MSG(\"Unexpected output control param ID passed to %s\");", bdef.m_name.c_str());
    code.write("NMP_ASSERT_FAIL();");
    code.write("break;");
    code.closeBrace();
    code.closeBrace();
  }
  else
  {
    code.write("void %sBehaviourData::updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam*, size_t )", bdef.m_name.c_str());
    code.openBrace();
  }
  code.closeBrace();
  code.newLine();

  // UpdateAndClearEmittedMessages
  code.writeHorizontalSeparator();
  if (!bdef.m_emittedMessages.empty())
  {
    code.write("void %sBehaviourData::updateAndClearEmittedMessages(uint32_t& messages)", bdef.m_name.c_str());
    code.openBrace();

    code.write("messages = m_messages;");
    code.write("m_messages = 0;");
    code.closeBrace();
  }
  else
  {
    code.write("void %sBehaviourData::updateAndClearEmittedMessages(uint32_t&)", bdef.m_name.c_str());
    code.openBrace();
  }
  code.closeBrace();
  code.newLine();

  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLine();

  return true;
}
