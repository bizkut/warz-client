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
#include "EmitManifest.h"
#include "FileUtils.h"

#include "CodeWriter.h"
#include <fstream>

static const char *kTypeStrings[] = { "float", "integer", "bool", "vector3", "physicsObjectID" };
static const char *kBoolStrings[] = { "false", "true" };

static const char *kEmittedMessagesTitle = "Emitted Messages";

static const char* kEmittedRequestHelp = "The message that should be sent or send a Clear All to remove any "
  "unhandled messages from the target state machine";
static const char* kActionHelp = "The action that should be performed on the destination statemachine. Set "
  "marks this message as sent, Clear removes this message from the destination "
  "if it hasn't been processed.";
static const char* kTargetHelp = "The state machine to send this message to. It is only possible to send "
  "messages to state machines below this node in the network.";

// ---------------------------------------------------------------------------------------------------------------------
static const char* sBoilerplate = 
{
   "\n\
    local animSets = listAnimSets()\n"
};

static const char *sVector3ComponentName[3] = { "X", "Y", "Z" };


// ---------------------------------------------------------------------------------------------------------------------
class ManifestGen
{
public:
  ManifestGen(const KeywordsManager& keymgr, const NetworkConstants &netConsts, CodeWriter& luawriter, StringBuilder* stringb, StringBuilder* quoteb) :
    keywords(keymgr), 
    constants(netConsts), 
    lua(luawriter),
    sb(stringb),
    qb(quoteb)
  {}

  void writeManifest(const BehaviourDef& bv) const;

  void writeFunctionPin(const char* name, const PString* helptext, bool input, bool required) const;
  void writeCommonDataPinMembers(const BehaviourDef::BaseVariable& base) const;
  void writeOutputDataPin(const BehaviourDef::OutputControlParam& cp, int arrayIndex = -1) const;
  void writeInputDataPin(const BehaviourDef::ControlParam& cp, int arrayIndex = -1, const char* name = NULL) const;
  void writeMinMaxDefaultValue(const BehaviourDef::AttributeVariable& av) const;
  void writeAttributeDeclaration(const BehaviourDef::AttributeVariable& av, int arrayIndex = -1) const;
  void writeAttributeNames(const BehaviourDef::AttributeVariable& av, int arrayIndex = -1) const;

  void writeUI(const BehaviourDef::AttributeVariable& av, int arrayIndex = -1, const char* arrayName = NULL) const;
  const StringBuilder& escapeQuotes(const PString& helptext) const;
  const StringBuilder& convertCommentToHelptext(const PString& comment) const;
  void getDisplayName(StringBuilder& sb, int arrayIndex, const char* limbName, const BehaviourDef::AttributeVariable& base) const;

  const KeywordsManager  &keywords;
  const NetworkConstants &constants;
  CodeWriter             &lua;
  StringBuilder          *sb;
  mutable StringBuilder  *qb;

private:
  ManifestGen();
  ManifestGen(const ManifestGen& rhs);
  ManifestGen& operator = (ManifestGen& rhs);
};

// ---------------------------------------------------------------------------------------------------------------------
const StringBuilder& ManifestGen::escapeQuotes(const PString& helptext) const
{
  qb->reset();
  if (!helptext.empty())
  {
    const char* str = helptext.c_str();
    for (const char* ch = str ; (*ch) ; ++ch)
    {
      if ((*ch) == '"')
      {
        qb->appendChar('\\');
      }
      qb->appendChar(*ch);
    }
  }

  return *qb;
}

// ---------------------------------------------------------------------------------------------------------------------
const StringBuilder& ManifestGen::convertCommentToHelptext(const PString& comment) const
{
  qb->reset();
  if (!comment.empty())
  {
    std::string text(comment.c_str());
    const std::string whitespace(" ");
    const std::string lessThan("<");
    const std::string fwdSlashDublet("//");
    const std::string fwdSlashTriplet("///");
    std::string::size_type begintext = std::string::npos;

    // Erase any ///.
    do
    {
      begintext = text.find(fwdSlashTriplet);
      if (begintext != std::string::npos)
      {
        text.replace(begintext, fwdSlashTriplet.size(), "");
      }
    }
    while (begintext != std::string::npos);

    // Erase any //.
    do
    {
      begintext = text.find(fwdSlashDublet);
      if (begintext != std::string::npos)
      {
        text.replace(begintext, fwdSlashDublet.size(), "");
      }
    }
    while (begintext != std::string::npos);

    // Erase < from the comment.
    begintext = text.find(lessThan);
    if (begintext == 0)
    {
      text.replace(begintext, lessThan.size(), "");
    }

    // Trim all whitespace from the front.
    do
    {
      begintext = text.find(whitespace);
      if (begintext == 0)
      {
        text.replace(begintext, whitespace.size(), "");
      }
    }
    while (begintext != std::string::npos && begintext == 0);

    // Erase all \n from the comment.
    text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());

    // Escape quotes.
    const char* str = text.c_str();
    for (const char* ch = str ; (*ch) ; ++ch)
    {
      if ((*ch) == '"')
      {
        qb->appendChar('\\');
      }
      qb->appendChar(*ch);
    }
  }

  return *qb;
}

// ---------------------------------------------------------------------------------------------------------------------
void ManifestGen::writeFunctionPin(const char* name, const PString* helptext, bool input, bool required) const
{
  lua.write("[\"%s\"] = ", name);
  lua.openBrace();
  lua.write("input = %s,", input?"true":"false");

  if (helptext && !helptext->empty())
  {
    lua.write("helptext = \"%s\",", convertCommentToHelptext(*helptext).c_str());
  }
  if(required)
  {
    lua.write("mode = \"Required\",");
  }
  lua.write("interfaces = ");
  
  lua.openBrace();
  lua.write("\"Transforms\",");
  lua.write("\"Time\",");
  lua.closeBraceWithComma();

  lua.closeBraceWithComma();
  lua.newLine();
}

// ---------------------------------------------------------------------------------------------------------------------
void ManifestGen::writeCommonDataPinMembers(const BehaviourDef::BaseVariable& base) const
{
  const PString* helptext = &base.m_comment;
  const BhDefVariableType type = base.getType();
  const ModifierOptions& modifiers = base.m_modifiers;

  if (helptext && !helptext->empty())
  {
    lua.write("helptext = \"%s\",", convertCommentToHelptext(*helptext).c_str());
  }

  lua.write("type = \"%s\",", keywords.stringFromBehaviourDefVariableType(type).c_str());

  if(hasBehaviourVariableModifier(modifiers, bvmLimbIndex))
  {
    lua.write("subtype = \"LimbIndex\",");
  }
  else if(hasBehaviourVariableModifier(modifiers, bvmLimbPartIndex))
  {
    lua.write("subtype = \"LimbPartIndex\",");
  }
  else if(hasBehaviourVariableModifier(modifiers, bvmPhysicsPartIndex))
  {
    lua.write("subtype = \"PhysicsPartIndex\",");
  }
}

//---------------------------------------------------------------------------------------------------------------------
void ManifestGen::writeInputDataPin(const BehaviourDef::ControlParam& cp, int arrayIndex, const char* limbName) const
{
  const char* name = cp.m_name.c_str();

  sb->reset();
  if (arrayIndex >= 0)
    sb->appendf("_%i", arrayIndex);

  lua.write("[\"%s%s\"] = ", name, sb->c_str());
  lua.openBrace();
  lua.write("input = true,");

  writeCommonDataPinMembers(cp);
  if(hasBehaviourVariableModifier(cp.m_modifiers, bvmCompulsoryInput))
  {
    lua.write("mode = \"Required\",");
  }
  else if(!hasBehaviourVariableModifier(cp.m_modifiers, bvmExposedAsPin))
  {
    lua.write("mode = \"HiddenInNetworkEditor\",");
  }

  // write the display string for the cp
  {
    sb->reset();
    getDisplayName(*sb, arrayIndex, limbName, cp);
    lua.write("displayName = \"%s\",", sb->c_str());
  }

  if(cp.m_hasDefault)
  {
    writeMinMaxDefaultValue(cp);
  }

  if (hasBehaviourVariableModifier(cp.m_modifiers, bvmPerAnimSet))
  {
    lua.writeRaw("perAnimSet = true, ");
  }

  lua.closeBraceWithComma();
  lua.newLine();
}

// ---------------------------------------------------------------------------------------------------------------------
void ManifestGen::getDisplayName(StringBuilder& sb, int arrayIndex, const char* limbName, const BehaviourDef::AttributeVariable& base) const
{
  getAttributeVariableTitle(base, sb);
  /*
  if (limbName)
  {
    sb->appendChar(' ');
    sb->appendf(limbName, arrayIndex);
  }
  else if (arrayIndex >= 0)
  {
    sb->appendf("%i", arrayIndex);
  }

  StringBuilder indx(8);
  if (arrayIndex >= 0)
    indx.appendf("_%i", arrayIndex);



  const ModifierOption* moTitle = hasBehaviourVariableModifier(base.m_modifiers, bvmTitle);
  const PString* str = &base.m_name;
  if(moTitle)
  {
    str = &moTitle->m_value;
  }*/

  /*if (!base.isArray() || !limbName || base.getArraySize() <= 1)
  {
    sb->appendf(str->c_str());
  }
  else*/

  if (base.isArray())
  {
    if (limbName)
    {
      sb.appendChar(' ');
      sb.appendf(limbName, arrayIndex);
    }
    /*else if (arrayIndex >= 0)
    {
      sb.appendf("%i", arrayIndex);
    }*/
  }
}

// ---------------------------------------------------------------------------------------------------------------------
void ManifestGen::writeOutputDataPin(const BehaviourDef::OutputControlParam& cp, int arrayIndex) const
{
  const char* name = cp.m_name.c_str();

  sb->reset();
  if (arrayIndex >= 0)
    sb->appendf("_%i", arrayIndex);

  lua.write("[\"%s%s\"] = ", name, sb->c_str());
  lua.openBrace();
  lua.write("input = false,");
  
  writeCommonDataPinMembers(cp);
  
  lua.closeBraceWithComma();
  lua.newLine();
}


// ---------------------------------------------------------------------------------------------------------------------
void ManifestGen::writeMinMaxDefaultValue(const BehaviourDef::AttributeVariable& av) const
{
  switch (av.getType())
  {
  case bhvtFloat:
    {
      lua.write("value = %f,", av.m_default.f);
      if (av.m_hasMin)
        lua.write("min = %f,", av.m_min.f);
      if (av.m_hasMax)
        lua.write("max = %f,", av.m_max.f);
    }
    break;

  case bhvtInteger:
    {
      lua.write("value = %i,", av.m_default.i);
      if (av.m_hasMin)
        lua.write("min = %i,", av.m_min.i);
      if (av.m_hasMax)
        lua.write("max = %i,", av.m_max.i);
    }
    break;

  case bhvtVector3:
    {
      lua.write("value = {x=%f, y=%f,z=%f},", av.m_default.v[0], av.m_default.v[1] ,av.m_default.v[2]);
      if (av.m_hasMin)
        lua.write("min = %i,", av.m_min.i);
      if (av.m_hasMax)
        lua.write("max = %i,", av.m_max.i);
    }
    break;

  case bhvtBool:
    {
      lua.write("value = %s,", av.m_default.b?"true":"false");
    }
    break;

  case bhvtPhysicsObjectID:
    {
      lua.write("value = 0, min = 0, max = 0,");
    }
    break;

  case bhvtMessage:
    {
      lua.writeRaw("");
    }
    break;

  default:
    assert(0);
    break;
  }
}

// ---------------------------------------------------------------------------------------------------------------------
void ManifestGen::writeAttributeDeclaration(const BehaviourDef::AttributeVariable& av, int arrayIndex) const
{
  sb->reset();
  if (arrayIndex >= 0)
    sb->appendf("_%i", arrayIndex);
    lua.openBrace();

    lua.write("name = \"%s%s\",",
      av.m_name.c_str(), 
      sb->c_str());

    lua.write("type = \"%s\",",
      av.getType() == bhvtPhysicsObjectID ? "int" : keywords.stringFromBehaviourDefVariableType(av.getType()).c_str());

    writeMinMaxDefaultValue(av);

    // For simplicity's sake, we'll write out all attributes as per anim set. Subsequently we can
    // change this so that we'll follow the modifier here, but the serialise function will still
    // make all the custom floats/ints etc per anim set. See MORPH-11243
    if (hasBehaviourVariableModifier(av.m_modifiers, bvmPerAnimSet))
    {
      lua.write("perAnimSet = true,");
    }

    if (!av.m_comment.empty())
    {
      lua.write("helptext = \"%s\",", convertCommentToHelptext(av.m_comment).c_str());
    }

    lua.closeBraceWithComma();
}

// ---------------------------------------------------------------------------------------------------------------------
void ManifestGen::writeAttributeNames(const BehaviourDef::AttributeVariable& av, int arrayIndex) const
{
  sb->reset();
  if (arrayIndex >= 0)
    sb->appendf("_%i", arrayIndex);

  if (av.getType() == bhvtVector3)
  {
    for (int i=0; i<3; i++)
      lua.write("\"%s%s%s\",", av.m_name.c_str(), sVector3ComponentName[i], sb->c_str());
  }
  else
  {
    lua.write("\"%s%s\",", av.m_name.c_str(), sb->c_str());
  }
}

// ---------------------------------------------------------------------------------------------------------------------
void ManifestGen::writeUI(const BehaviourDef::AttributeVariable& av, int arrayIndex, const char* arrayName) const
{
  bool perAnimSet = av.isPerAnimSet();

  sb->reset();
  getDisplayName(*sb, arrayIndex, arrayName, av);

  StringBuilder indx(8);
  if (arrayIndex >= 0)
    indx.appendf("_%i", arrayIndex);

  const char *noEditor = hasBehaviourVariableModifier(av.m_modifiers, bvmCompulsoryInput)
    ? ", noEditor=true" : "";

  switch (av.getType())
  {
  case bhvtPhysicsObjectID:
    {
      lua.write("{title = \"%s\", perAnimSet = %s, type = \"int\", attributes = \"%s%s\"%s},", 
        sb->c_str(),
        kBoolStrings[perAnimSet],
        av.m_name.c_str(), indx.c_str(),
        noEditor);
    }
    break;
  default:
    {
      lua.write("{title = \"%s\", perAnimSet = %s, type = \"%s\", attributes = \"%s%s\"%s},", 
        sb->c_str(),
        kBoolStrings[perAnimSet],
        kTypeStrings[av.getType()],
        av.m_name.c_str(), indx.c_str(),
        noEditor);
    }
    break;
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// map from our variable enum to the semantic type
const char* cpEnumForVariableType(BhDefVariableType vt)
{
  switch (vt)
  {
  case bhvtFloat:   return "ATTRIB_SEMANTIC_CP_FLOAT"; break;
  case bhvtInteger: return "ATTRIB_SEMANTIC_CP_INT"; break;
  case bhvtBool:    return "ATTRIB_SEMANTIC_CP_BOOL"; break;
  case bhvtVector3: return "ATTRIB_SEMANTIC_CP_VECTOR3"; break;
  case bhvtPhysicsObjectID: return "ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER"; break;
  }

  return "Unknown";
}

// ---------------------------------------------------------------------------------------------------------------------
// write integer-type attributes
void writeIntTypeAttributes(CodeWriter& lua, const BehaviourDef::AttributeVariable& av, unsigned int& intIndex)
{
  switch (av.getType())
  {
  case bhvtInteger:
    {
      lua.write("for asIdx, asVal in ipairs(animSets) do");
      if (av.isNotArray())
      {
        if (hasBehaviourVariableModifier(av.m_modifiers, bvmPerAnimSet))
          lua.write("  stream:writeInt(getValue(node, \"%s\", asVal), string.format(\"Int_%i_%%d\", asIdx-1))", av.m_name.c_str(), intIndex++);
        else
          lua.write("  stream:writeInt(getValue(node, \"%s\"), string.format(\"Int_%i_%%d\", asIdx-1))", av.m_name.c_str(), intIndex++);
      }
      else
      {
        for (unsigned int i=0; i<av.getArraySize(); i++)
        {
          if (hasBehaviourVariableModifier(av.m_modifiers, bvmPerAnimSet))
            lua.write("  stream:writeInt(getValue(node, \"%s_%i\", asVal), string.format(\"Int_%i_%%d\", asIdx-1))", av.m_name.c_str(), i, intIndex++);
          else
            lua.write("  stream:writeInt(getValue(node, \"%s_%i\"), string.format(\"Int_%i_%%d\", asIdx-1))", av.m_name.c_str(), i, intIndex++);
        }
      }
      lua.write("end");
    }
    break;

  case bhvtBool:
    {
      lua.write("for asIdx, asVal in ipairs(animSets) do");
      if (av.isNotArray())
      {
        if (hasBehaviourVariableModifier(av.m_modifiers, bvmPerAnimSet))
          lua.write("  stream:writeInt( ( ( getValue(node, \"%s\", asVal) and 1 ) or 0 ) , string.format(\"Int_%i_%%d\", asIdx-1))", av.m_name.c_str(), intIndex++);
        else
          lua.write("  stream:writeInt( ( ( getValue(node, \"%s\") and 1 ) or 0 ) , string.format(\"Int_%i_%%d\", asIdx-1))", av.m_name.c_str(), intIndex++);
      }
      else
      {
        for (unsigned int i=0; i<av.getArraySize(); i++)
        {
          if (hasBehaviourVariableModifier(av.m_modifiers, bvmPerAnimSet))
            lua.write("  stream:writeInt( ( ( getValue(node, \"%s_%i\", asVal) and 1 ) or 0 ) , string.format(\"Int_%i_%%d\", asIdx-1))", av.m_name.c_str(), i, intIndex++);
          else
            lua.write("  stream:writeInt( ( ( getValue(node, \"%s_%i\") and 1 ) or 0 ) , string.format(\"Int_%i_%%d\", asIdx-1))", av.m_name.c_str(), i, intIndex++);
        }
      }
      lua.write("end");
    }
    break;
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// write uint64-type attributes
void writeUInt64TypeAttributes(CodeWriter& lua, const BehaviourDef::AttributeVariable& av, unsigned int& uint64Index)
{
  switch (av.getType())
  {
  case bhvtPhysicsObjectID:
    {
      lua.write("for asIdx, asVal in ipairs(animSets) do");
      if (av.isNotArray())
      {
        if (hasBehaviourVariableModifier(av.m_modifiers, bvmPerAnimSet))
          lua.write("  stream:writeInt(getAttribute(node, \"%s\", asVal), string.format(\"UInt64_%i_%%d\", asIdx-1))", av.m_name.c_str(), uint64Index++);
        else
          lua.write("  stream:writeInt(getAttribute(node, \"%s\"), string.format(\"UInt64_%i_%%d\", asIdx-1))", av.m_name.c_str(), uint64Index++);
      }
      else
      {
        for (unsigned int i=0; i<av.getArraySize(); i++)
        {
          if (hasBehaviourVariableModifier(av.m_modifiers, bvmPerAnimSet))
            lua.write("  stream:writeInt(getAttribute(node, \"%s_%i\", asVal), string.format(\"UInt64_%i_%%d\", asIdx-1))", av.m_name.c_str(), i, uint64Index++);
          else
            lua.write("  stream:writeInt(getAttribute(node, \"%s_%i\"), string.format(\"UInt64_%i_%%d\", asIdx-1))", av.m_name.c_str(), i, uint64Index++);
        }
      }
      lua.write("end");
    }
    break;
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// write uint64-type attributes
void writeVector3TypeAttributes(CodeWriter& lua, const BehaviourDef::AttributeVariable& av, unsigned int& vector3Index)
{
  switch (av.getType())
  {
  case bhvtVector3:
    {
      lua.write("for asIdx, asVal in ipairs(animSets) do");
      if (av.isNotArray())
      {
        if (hasBehaviourVariableModifier(av.m_modifiers, bvmPerAnimSet))
          lua.write("  stream:writeVector3(getValue(node, \"%s\", asVal), string.format(\"Vector3_%i_%%d\", asIdx-1))", av.m_name.c_str(), vector3Index++);
        else
          lua.write("  stream:writeFloat(getValue(node, \"%s\"), string.format(\"Vector3_%i_%%d\", asIdx-1))", av.m_name.c_str(), vector3Index++);
      }
      else
      {
        for (unsigned int i=0; i<av.getArraySize(); i++)
        {
          if (hasBehaviourVariableModifier(av.m_modifiers, bvmPerAnimSet))
            lua.write("  stream:writeFloat(getValue(node, \"%s_%i\", asVal), string.format(\"Vector3_%i_%%d\", asIdx-1))", av.m_name.c_str(),  i, vector3Index++);
          else
            lua.write("  stream:writeFloat(getValue(node, \"%s_%i\"), string.format(\"Vector3_%i_%%d\", asIdx-1))", av.m_name.c_str(),  i, vector3Index++);
        }
      }
      lua.write("end");
    }
    break;
  }
}


// ---------------------------------------------------------------------------------------------------------------------
// write float-type attributes
void writeFloatTypeAttributes(CodeWriter& lua, const BehaviourDef::AttributeVariable& av, unsigned int& floatIndex)
{
  switch (av.getType())
  {
  case bhvtFloat:
    {
      lua.write("for asIdx, asVal in ipairs(animSets) do");
      if (av.isNotArray())
      {
        if (hasBehaviourVariableModifier(av.m_modifiers, bvmPerAnimSet))
          lua.write("  stream:writeFloat(getValue(node, \"%s\", asVal), string.format(\"Float_%i_%%d\", asIdx-1))", av.m_name.c_str(), floatIndex++);
        else
          lua.write("  stream:writeFloat(getValue(node, \"%s\"), string.format(\"Float_%i_%%d\", asIdx-1))", av.m_name.c_str(), floatIndex++);
      }
      else
      {
        for (unsigned int i=0; i<av.getArraySize(); i++)
        {
          if (hasBehaviourVariableModifier(av.m_modifiers, bvmPerAnimSet))
            lua.write("  stream:writeFloat(getValue(node, \"%s_%i\", asVal), string.format(\"Float_%i_%%d\", asIdx-1))", av.m_name.c_str(), i, floatIndex++);
          else
            lua.write("  stream:writeFloat(getValue(node, \"%s_%i\"), string.format(\"Float_%i_%%d\", asIdx-1))", av.m_name.c_str(), i, floatIndex++);
        }
      }
      lua.write("end");
    }
    break;
  }
}

// ---------------------------------------------------------------------------------------------------------------------
void ManifestGen::writeManifest(const BehaviourDef& bv) const
{
  lua.write("--------------------------------------------------------------------------------------------------");
  lua.write("--                                 This file is auto-generated                                  --");
  lua.write("--------------------------------------------------------------------------------------------------");

  lua.newLine();
  lua.write("require \"ui/AttributeEditor/AttributeEditor.lua\"");

  unsigned int numEr = bv.m_emittedMessages.size();
  if (numEr>0)
  {
    lua.write("require \"luaAPI/ManifestUtils.lua\"");
  }
  lua.newLines(2);

  if (bv.m_development)
  {
    lua.write("if app.isDevBuild() then");
    lua.indent(1);
  }

  lua.write("local behaviourNode = ");
  lua.openBrace();

  // descriptor
  if (!bv.m_displayName.empty())
  {
    lua.write("displayName = \"%s\",", bv.m_displayName.c_str());
  }

  // descriptor
  if (!bv.m_version.empty())
  {
    lua.write("version = %s,", bv.m_version.c_str());
  }

  // topology
  const char* networkNames[] = {
    "networkMaxNumArms",
    "networkMaxNumLegs",
    "networkMaxNumHeads",
    0
  };
  lua.write("topology =");
  lua.openBrace();
  for (int i = 0 ; networkNames[i] ; ++i)
  {
    int val;
    if (constants.find(networkNames[i], &val))
    {
      lua.write("%s = %d,", networkNames[i], val);
    }
  }
  lua.closeBraceWithComma();

  // Help text.
  if (!bv.m_shortDesc.empty())
  {
    // This conversion is necessary because test description will be pasted
    // to lua helptext comment block.
    lua.write("helptext = \"%s\",", convertCommentToHelptext(bv.m_shortDesc).c_str());
  }

  lua.write("group = \"%s\",", bv.m_group.c_str());
  lua.write("image = \"%sBehaviour.png\",", bv.m_name.c_str());
  lua.write("id = generateNamespacedId(idNamespaces.NaturalMotion, 128),");
  lua.newLine();

  // -----------------------------------------------------------------------------------------------
  // function pins
  lua.write("functionPins = ");
  lua.openBrace();

  iterate_begin_const(BehaviourDef::AnimationInput, bv.m_animationInputs, ain)
  {
    bool requiresConnection = hasBehaviourVariableModifier(ain.m_modifiers, bvmCompulsoryInput) != 0;
    writeFunctionPin(ain.m_name.c_str(), &ain.m_comment, true, requiresConnection);
  }
  iterate_end
  writeFunctionPin("Result", 0, false, false);

  lua.closeBraceWithComma();
  lua.newLine();

  // -----------------------------------------------------------------------------------------------
  // data pins
  if (!bv.m_controlParamGroups.empty() || !bv.m_outputControlParams.empty())
  {
    lua.write("dataPins = ");
    lua.openBrace();
 
    iterate_begin_const(BehaviourDef::ControlParamGroup, bv.m_controlParamGroups, cg)
    {
      iterate_begin_const(BehaviourDef::ControlParam, cg.m_params, cp)
      {
        if (cp.isNotArray())
        {
          writeInputDataPin(cp);
        }
        else
        {
          const char* nameToWrite = getLimbNameFromArray(cp.m_arraySizeAsString.c_str());
          for (unsigned int i=0; i<cp.getArraySize(); i++)
          {
            writeInputDataPin(cp, (int)i, nameToWrite);
          }
        }
      }
      iterate_end;
    }
    iterate_end;

    iterate_begin_const(BehaviourDef::OutputControlParam, bv.m_outputControlParams, cp)
    {
      if (cp.isNotArray())
      {
        writeOutputDataPin(cp);
      }
      else
      {
        for (unsigned int i=0; i<cp.getArraySize(); i++)
        {
          writeOutputDataPin(cp, (int) i);
        }
      }
    }
    iterate_end;
    lua.closeBraceWithComma();
    lua.newLine();
  }

  // -----------------------------------------------------------------------------------------------
  // pin grouping
  lua.write("pinOrder = ");
  lua.openBrace();
  iterate_begin_const(BehaviourDef::AnimationInput, bv.m_animationInputs, ain)
  {
    lua.write("\"%s\",", ain.m_name.c_str());
  }
  iterate_end;

  iterate_begin_const(BehaviourDef::ControlParamGroup, bv.m_controlParamGroups, cg)
  {
    iterate_begin_const(BehaviourDef::ControlParam, cg.m_params, cp)
    {
      if (cp.isNotArray())
      {
        lua.write("\"%s\",", cp.m_name.c_str());
      }
      else
      {
        for (unsigned int i=0; i<cp.getArraySize(); i++)
        {
          lua.write("\"%s_%i\",", cp.m_name.c_str(), i);
        }
      }
    }
    iterate_end;
  }
  iterate_end;
  lua.write("\"Result\",");
  iterate_begin_const(BehaviourDef::OutputControlParam, bv.m_outputControlParams, cp)
  {
    if (cp.isNotArray())
    {
      lua.write("\"%s\",", cp.m_name.c_str());
    }
    else
    {
      for (unsigned int i=0; i<cp.getArraySize(); i++)
      {
        lua.write("\"%s_%i\",", cp.m_name.c_str(), i);
      }
    }
  }
  iterate_end;
  lua.closeBraceWithComma();
  lua.newLine();


  // -----------------------------------------------------------------------------------------------
  // attributes list
  unsigned int numIntsInAttributes = 0;
  unsigned int numFloatsInAttributes = 0;
  unsigned int numUInt64sInAttributes = 0;
  unsigned int numVector3sInAttributes = 0;


  lua.write("attributes = ");
  lua.openBrace();
  iterate_begin_const(BehaviourDef::AttributeGroup, bv.m_attributeGroups, ag)
  {
    iterate_begin_const(BehaviourDef::AttributeVariable, ag.m_vars, av)
    {
      if (av.isNotArray())
      {
        writeAttributeDeclaration(av);
      }
      else
      {
        for (unsigned int i=0; i<av.getArraySize(); i++)
        {
          writeAttributeDeclaration(av, (int)i);
        }
      }

      // Sum the number of ints and floats required to represent all of the attributes.
      numFloatsInAttributes += av.getNumFloats();
      numIntsInAttributes += av.getNumInts();
      numUInt64sInAttributes += av.getNumUInt64s();
      numVector3sInAttributes += av.getNumVector3s();
    }
    iterate_end

    iterate_begin_const(BehaviourDef::AttributeGroup::EnumDecl, ag.m_enumDecl, enm)
    {
      if (hasBehaviourVariableModifier(enm.m_modifiers, bvmEnumNoInstance))
        continue;

      lua.writeTabs();
      lua.writeRaw("{ name = \"%s\", type = \"int\", value = %i ", enm.m_name.c_str(), enm.m_entries[0].m_value);

      // For simplicity's sake, we'll write out all attributes as per anim set. Subsequently we can
      // change this so that we'll follow the modifier here, but the serialize function will still
      // make all the custom floats/ints etc per anim set. See MORPH-11243
      if (hasBehaviourVariableModifier(enm.m_modifiers, bvmPerAnimSet))
      {
        lua.writeRaw(", perAnimSet = true ");
      }

      if (!enm.m_comment.empty())
      {
        lua.writeRaw(", helptext=\"%s\" ", convertCommentToHelptext(enm.m_comment).c_str());
      }

      lua.writeRaw(" },");
      lua.newLine();
    }
    iterate_end
  }
  iterate_end

  // Emitted message attribute
  unsigned int erIndex = 0;
  iterate_begin_const(BehaviourDef::EmittedMessage, bv.m_emittedMessages, er)
  {
    lua.write("-- Request %d, (%s)", erIndex, er.m_name.c_str());
    lua.write("{ name = \"EmittedRequest%d\", type = \"request\", helptext = \"%s\"},", erIndex, kEmittedRequestHelp);
    lua.write("{ name = \"Action%d\", type = \"string\", value = \"Set\", helptext = \"%s\"},", erIndex, kActionHelp);
    lua.write("{ name = \"Target%d\", type = \"ref\", kind = \"allStateMachines\", helptext = \"%s\"},", erIndex, kTargetHelp);
    lua.newLine();
    erIndex++;
  }
  iterate_end;

  lua.closeBraceWithComma();
  lua.newLine();

  lua.write("-------------------------------------------------------------------");
  lua.newLine();

  // -----------------------------------------------------------------------------------------------
  // serializer
  {
    lua.write("serialize = function(node, stream) ");
    lua.indent(1);

    lua.write("stream:writeInt(%i, \"BehaviourID\")", bv.m_behaviourID);
    lua.newLine();

    // ---------- common serializer prefix
    lua.writeRaw(sBoilerplate);
    lua.newLine();

    lua.write("-------------------------------------------------------------------");
    lua.newLine();

    // ---------- write integer-type attributes
    unsigned int intIndex = 0;
    lua.write("stream:writeUInt(%i, \"numBehaviourInts\")", numIntsInAttributes);

    iterate_begin_const(BehaviourDef::AttributeVariablePtr, bv.m_messageParamAttributes, avPtr)
    {
      writeIntTypeAttributes(lua, *avPtr, intIndex);
    }
    iterate_end

    iterate_begin_const(BehaviourDef::AttributeGroup, bv.m_attributeGroups, ag)
    {
      iterate_begin_const(BehaviourDef::AttributeGroup::EnumDecl, ag.m_enumDecl, enm)
      {
        if (hasBehaviourVariableModifier(enm.m_modifiers, bvmEnumNoInstance))
          continue;

        lua.write("for asIdx, asVal in ipairs(animSets) do");
        lua.write("  stream:writeInt(getValue(node, \"%s\"), string.format(\"Int_%i_%%d\", asIdx-1))", enm.m_name.c_str(), intIndex++);
        lua.write("end");
      }
      iterate_end
    }
    iterate_end
    lua.newLine();

    // ---------- write float-type attributes
    unsigned int floatIndex = 0;
    lua.write("stream:writeUInt(%i, \"numBehaviourFloats\")", numFloatsInAttributes);

    iterate_begin_const(BehaviourDef::AttributeVariablePtr, bv.m_messageParamAttributes, avPtr)
    {
      writeFloatTypeAttributes(lua, *avPtr, floatIndex);
    }
    iterate_end

    lua.newLine();

     unsigned int vector3Index = 0;
    lua.write("stream:writeUInt(%i, \"numBehaviourVector3s\")", numVector3sInAttributes);

    iterate_begin_const(BehaviourDef::AttributeVariablePtr, bv.m_messageParamAttributes, avPtr)
    {
      writeVector3TypeAttributes(lua, *avPtr, vector3Index);
    }
    iterate_end

    lua.newLine();

    // ---------- write uint64-type attributes
    unsigned int uint64Index = 0;
    lua.write("stream:writeUInt(%i, \"numBehaviourUInt64s\")", numUInt64sInAttributes);

    iterate_begin_const(BehaviourDef::AttributeVariablePtr, bv.m_messageParamAttributes, avPtr)
    {
      writeUInt64TypeAttributes(lua, *avPtr, uint64Index);
    }
    iterate_end

    lua.newLine();

    lua.write("-------------------------------------------------------------------");
    lua.newLine();

    // ---------- write custom inputs
    iterate_begin_const(BehaviourDef::AnimationInput, bv.m_animationInputs, ain)
    {
      lua.write("local inputNodeID_%s = -1", ain.m_name.c_str());
      lua.write("if isConnected{SourcePin = node .. \".%s\", ResolveReferences = true} then", ain.m_name.c_str());
      lua.write("  inputNodeID_%s = getConnectedNodeID(node, \"%s\")", ain.m_name.c_str(), ain.m_name.c_str());
      lua.write("end");
      lua.newLine();
    }
    iterate_end

    lua.write("stream:writeUInt(%i, \"numBehaviourNodeAnimationInputs\")", (int)bv.m_animationInputs.size());
    lua.newLine();

    unsigned int cpaIndex = 0;
    iterate_begin_const(BehaviourDef::AnimationInput, bv.m_animationInputs, ain)
    {
      lua.write("stream:writeNetworkNodeId(inputNodeID_%s, \"BehaviourNodeAnimationInput_%i\")", ain.m_name.c_str(), cpaIndex);
      lua.newLine();

      cpaIndex++;
    }
    iterate_end

    lua.write("-------------------------------------------------------------------");
    lua.newLine();

    // ---------- write control params
    unsigned int totalCPCount = 0;

    // control parameters (data pin inputs) can store default values.   In this case we need to create a data block of
    // the correct size to store these values

    unsigned int numIntsInControlParams = 0;
    unsigned int numFloatsInControlParams = 0;
    unsigned int numUInt64sInControlParams = 0;
    unsigned int numVector3sInControlParams = 0;

    iterate_begin_const(BehaviourDef::ControlParamGroup, bv.m_controlParamGroups, cg)
    {
      iterate_begin_const(BehaviourDef::ControlParam, cg.m_params, cp)
      {
        if (cp.isNotArray())
        {
          totalCPCount ++;
        }
        else
        {
          totalCPCount += cp.getArraySize();
        }
        numIntsInControlParams += cp.getNumInts();
        numFloatsInControlParams += cp.getNumFloats(); 
        numUInt64sInControlParams += cp.getNumUInt64s();
        numVector3sInControlParams += cp.getNumVector3s();
      }
      iterate_end
    }
    iterate_end

    lua.write("stream:writeUInt(%i, \"numInputCPInts\")", numIntsInControlParams);
    lua.write("stream:writeUInt(%i, \"numInputCPFloats\")", numFloatsInControlParams);
    lua.write("stream:writeUInt(%i, \"numInputCPUInt64s\")", numUInt64sInControlParams);
    lua.write("stream:writeUInt(%i, \"numInputCPVector3s\")", numVector3sInControlParams);

    lua.write("stream:writeUInt(%u, \"numBehaviourControlParameterInputs\")", totalCPCount);
    lua.newLine();

    unsigned int cpIndex = 0;
    iterate_begin_const(BehaviourDef::ControlParamGroup, bv.m_controlParamGroups, cg)
    {
      iterate_begin_const(BehaviourDef::ControlParam, cg.m_params, cp)
      {
        const char* writerName = "writeDataPinAltName";
        if(cp.isPerAnimSet())
        {
          writerName = "writePerAnimSetDataPinAltName";
        }

        if (cp.isNotArray())
        {
          lua.write("%s(stream, node, \"%s\", \"BehaviourControlParameterInputID_%i\")", writerName, cp.m_name.c_str(), cpIndex );
          lua.write("stream:writeString(\"%s\", \"BehaviourControlParameterInputType_%i\")", cpEnumForVariableType(cp.getType()), cpIndex);
          
          cpIndex++;
        }
        else
        {
          for (unsigned int i=0; i<cp.getArraySize(); i++)
          {
            lua.write("%s(stream, node, \"%s_%d\", \"BehaviourControlParameterInputID_%i\" )", writerName, cp.m_name.c_str(), i, cpIndex );
            lua.write("stream:writeString(\"%s\", \"BehaviourControlParameterInputType_%i\")", cpEnumForVariableType(cp.getType()), cpIndex);
            cpIndex++;
          }
        }
        lua.newLine();
      }
      iterate_end;
    }
    iterate_end;

    // ---------- write output control param types
    unsigned int ocpIndex = 0;
    iterate_begin_const(BehaviourDef::OutputControlParam, bv.m_outputControlParams, cp)
    {
      if (cp.isNotArray())
      {
        lua.write("stream:writeString(\"%s\", \"BehaviourControlParameterOutputType_%i\")", cpEnumForVariableType(cp.getType()), ocpIndex);
        ocpIndex++;
      }
      else
      {
        for (unsigned int i=0; i<cp.getArraySize(); i++)
        {
          lua.write("stream:writeString(\"%s\", \"BehaviourControlParameterOutputType_%i\")", cpEnumForVariableType(cp.getType()), ocpIndex);
          ocpIndex++;
        }
      }
    }
    iterate_end;

    lua.write("stream:writeUInt(%u, \"numBehaviourControlParameterOutputs\")", ocpIndex);
    lua.newLine();

    // ---------- write emitted message types 
    lua.write("-------------------------------------------------------------------");
    lua.newLine();
    unsigned int erNumMessages = bv.m_emittedMessages.size();
    lua.write("local numMessageSlots = %u", erNumMessages);
    lua.write("local numOutputMessages = 0");
    if (erNumMessages>0)
    {
      lua.write("-- An array of emitted messages length numMessageSlots or 0, (use all slots or none).");
      lua.write("for i = 0, (numMessageSlots-1) do");  
      lua.write("  if (serializeRequest(node, stream, tostring(i), tostring(i), true)) then");       
      lua.write("    numOutputMessages = numOutputMessages + 1");
      lua.write("  end");
      lua.write("end");
    }

    lua.write("if (numOutputMessages > 0) then");
    lua.write("  stream:writeUInt(numMessageSlots, \"NumMessageSlots\")");
    lua.write("  stream:writeBool(true, \"NodeEmitsMessages\")");
    lua.write("else");
    lua.write("  stream:writeUInt(0, \"NumMessageSlots\")");
    lua.write("  stream:writeBool(false, \"NodeEmitsMessages\")");
    lua.write("end");


    
    lua.newLine();

    // end of serialize
    lua.indent(-1);
    lua.write("end, -- Serialize function");
    lua.newLine();
  }

  lua.write("-------------------------------------------------------------------");
  lua.newLine();

  // -----------------------------------------------------------------------------------------------
  // validate
  {
    lua.write("validate = function(node) ");
    lua.indent(1);
    lua.write("return defaultPinValidation(node)");
    lua.indent(-1);
    lua.write("end, -- Validate function");
    lua.newLine();
  }

  lua.write("-------------------------------------------------------------------");
  lua.newLine();

  // -----------------------------------------------------------------------------------------------
  // getTransformChannels
  {
    lua.write("getTransformChannels = function(node, set) ");
    lua.indent(1);

    lua.write("local transformChannels = { }");
    lua.write("local rigSize = anim.getRigSize(set)");
    lua.write("for i=1, rigSize do");
    lua.write("  transformChannels[i - 1] = true");
    lua.write("end");
    lua.write("return transformChannels");

    // end of validate
    lua.indent(-1);
    lua.write("end,");
    lua.newLine();
  }

  lua.closeBrace();

  if (bv.m_development)
  {
    lua.indent(-1);
    lua.write("end");
  }
  lua.newLines(2);

  lua.write("-------------------------------------------------------------------");
  lua.newLine();

  // -----------------------------------------------------------------------------------------------
  // custom UI for vector3, etc
  lua.write("local attributeGroups =");
  lua.openBrace();
  if (!bv.m_attributeGroups.empty())
  {
    bool isExpanded = true;

    BehaviourDef::AttributeVariablePtrs attrVars;
    iterate_begin_const(BehaviourDef::AttributeGroup, bv.m_attributeGroups, ag)
    {
      attrVars.clear();
      iterate_begin_const(BehaviourDef::AttributeVariable, ag.m_vars, av)
        attrVars.push_back(&av);
      iterate_end

      BehaviourDef::ControlParamGroup* cg = bv.findControlParamGroup(ag.m_name);
      if (cg)
      {
        iterate_begin_const(BehaviourDef::ControlParam, cg->m_params, cp)
          attrVars.push_back(&cp);
        iterate_end
      }

      if (   !attrVars.empty() 
          && (bv.m_emittedMessages.empty() || strcmp(kEmittedMessagesTitle, ag.m_name.c_str()) != 0))
      {
        const ModifierOption* moTitle = hasBehaviourVariableModifier(ag.m_modifiers, bvmTitle);

        lua.openBrace();
        lua.writeTabs();
        if (moTitle)
        {
          lua.writeRaw("title = %s,", moTitle->m_value.c_str());
        }
        else
        {
          lua.writeRaw("title = \"%s\",", ag.m_name.c_str());
        }

        // help
        if (!ag.m_comment.empty())
        {
          lua.newLine();
          lua.writeTabs();
          lua.writeRaw("helptext = \"%s\",", convertCommentToHelptext(ag.m_comment).c_str());
        }

        // isExpanded
        lua.newLine();
        lua.writeTabs();
        lua.writeRaw("isExpanded = %s,", kBoolStrings[isExpanded]);
        isExpanded = false;

        // If get the display function
        //
        if(!bv.m_defaultsDisplayFunc.empty())
        {
          lua.newLine();
          lua.writeTabs();
          lua.writeRaw("displayFunc = %s,", bv.m_defaultsDisplayFunc.c_str());
        }

        // details (of all the attributes in this section)
        lua.newLine();
        lua.writeTabs();
        lua.writeRaw("details = ");
        lua.newLine();
        lua.openBrace();

        iterate_begin_const(BehaviourDef::AttributeVariablePtr, attrVars, av)
        {
          if (av->isNotArray())
            writeUI(*av);
          else
          {
            const char* nameToWrite = getLimbNameFromArray(av->m_arraySizeAsString.c_str());
            if (nameToWrite || av->getArraySize() > 1)
            {
              // Write titles with named items or arrays with multiple items.
              for (unsigned int i=0; i < av->getArraySize(); i++)
              {
                writeUI(*av, (int)i, nameToWrite);
              }
            }
            else
            {
              // If there's no name to append and only one array item, append an
              // empty string.
              writeUI(*av, 0, "");
            }
          }
        }
        iterate_end

        lua.closeBraceWithComma();
        lua.closeBraceWithComma();
      }
    }
    iterate_end

    // Emitted requests attribute
    unsigned int numRequests = bv.m_emittedMessages.size();
    if (numRequests>0)
    {
      lua.openBrace();
      lua.write("title = \"Emitted Messages\",");
      lua.write("requests = ");
      lua.openBrace();
      unsigned int requestIndex = 0;
      iterate_begin_const(BehaviourDef::EmittedMessage, bv.m_emittedMessages, er)
      {
        lua.openBrace();
        lua.write("name = \"%s\", ", er.m_name);

        // help
        if (!er.m_comment.empty())
        {
          lua.write("helptext = \"%s\",", convertCommentToHelptext(er.m_comment).c_str());
        }
        lua.closeBraceWithComma();
        requestIndex++;
      }
      iterate_end
      lua.closeBraceWithComma();

      iterate_begin_const(BehaviourDef::AttributeGroup, bv.m_attributeGroups, ag)
      {
        if (!ag.m_vars.empty() && strcmp(kEmittedMessagesTitle, ag.m_name.c_str()) == 0)
        {
          lua.writeTabs();
          lua.writeRaw("details = ");
          lua.newLine();
          lua.openBrace();

          iterate_begin_const(BehaviourDef::AttributeVariable, ag.m_vars, av)
          {
            if (av.isNotArray())
              writeUI(av);
            else
            {
              const char* nameToWrite = getLimbNameFromArray(av.m_arraySizeAsString.c_str());
              for (unsigned int i=0; i<av.getArraySize(); i++, nameToWrite)
              {
                writeUI(av, (int)i, nameToWrite);
              }
            }
          }
          iterate_end
          lua.closeBraceWithComma();
          break;
        }
      }
      iterate_end

      lua.closeBraceWithComma();
    }
  }

  lua.closeBrace();
  lua.newLine();

  lua.writeRaw("return ");
  lua.openBrace();
  lua.write("data = behaviourNode,");
  lua.write("attributeGroups = attributeGroups,");
  lua.closeBrace();
}

// ---------------------------------------------------------------------------------------------------------------------
bool emitManifest(const char* outputDirectory, const NetworkDatabase& ndb, bool forceGeneration)
{
  char outputFile[MAX_PATH];

  // nuke existing manifests if we are in 'force' mode
  if (forceGeneration)
  {
    deleteFiles(outputDirectory, "*.lua");
  }

  StringBuilder sb(64);
  StringBuilder qb(1024);
  CodeWriter luaCodeWriter;
  ManifestGen manifestGen(ndb.m_keywords, ndb.m_constants, luaCodeWriter, &sb, &qb);

  printf("\n---------------------------------------------------------\nManifest Generation:\n");
  iterate_begin_const_ptr(BehaviourDef, ndb.m_behaviourDefs, bv)
  {
     // create path to header
    NMP_STRNCPY_S(outputFile, sizeof(outputFile), outputDirectory);
    NMP_STRNCAT_S(outputFile, sizeof(outputFile), bv->m_name.c_str());
    NMP_STRNCAT_S(outputFile, sizeof(outputFile), ".lua");

    {
      luaCodeWriter.begin();
      manifestGen.writeManifest(*bv);

      // unless we are forced to regenerate, check to see if the on-disk version is different
      bool filesMatched = false;
      if (!forceGeneration)
        filesMatched = fileContentsCompare(outputFile, luaCodeWriter.getContents());

      if (!filesMatched)
      {
        printf("    %s\n", bv->m_name.c_str());

        FILE *myFile = NULL;
        fopen_s(&myFile, outputFile, "wt");

        if (!myFile)
        {
          printf("Could not open '%s' for writing (Error %i)\n", outputFile, GetLastError());
          return false;
        }

        luaCodeWriter.writeContentsAndClose(myFile);
        fclose(myFile);
      }
    }
  }
  iterate_end

  printf("\n");

  return true;
}
