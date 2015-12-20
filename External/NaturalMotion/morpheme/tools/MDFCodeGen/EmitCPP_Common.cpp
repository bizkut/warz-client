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
#include "MappedIO.h"
#include "EmitCPP_Common.h"
#include "CodeWriter.h"

#include "NMPlatform/NMPlatform.h"

//----------------------------------------------------------------------------------------------------------------------
DataFootprint::DataFootprint() : 
  m_currentUsage(0), m_paddingIndex(0) 
{

}

//----------------------------------------------------------------------------------------------------------------------
void DataFootprint::alignTo(CodeWriter& code, unsigned int av)
{
  if (m_currentUsage % av != 0)
  {
    unsigned int delta = av - (m_currentUsage % av);

    if (delta == 1)
    {
      code.write("unsigned char pad_%i; // automatic padding => %i (av %i)", 
        m_paddingIndex, m_currentUsage + delta, av);
    }
    else
    {
      code.write("unsigned char pad_%i[%i]; // automatic padding => %i (av %i)", 
        m_paddingIndex, delta, m_currentUsage + delta, av);
    }

    append(delta);

    m_paddingIndex ++;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// add headers to bring in dependent types; forward-declare if possible
//
void calculateDependentHeadersAndFwdDecls(
  const TypeDependencies& dep, 
  bool forHeader, 
  CodeWriter& code,
  StringBuilder* fwdDeclsExternal,
  StringBuilder* fwdDeclsInternal)
{
  unsigned int extTypeGroup = code.addUniqueHeaderGroup("external types");
  unsigned int knTypeGroup = code.addUniqueHeaderGroup("known types");

  UniqueTypes::const_iterator it, end, p_it, p_end;

  if (forHeader)
  {
    it = dep.m_dependentTypes.const_begin();
    end = dep.m_dependentTypes.const_end();
  }
  else
  {
    it = dep.m_dependentTypesByPtrRef.const_begin();
    end = dep.m_dependentTypesByPtrRef.const_end();
  }

  StringBuilder sb(64);
  for (; it != end; ++it)
  {
    const Type* t = (*it);
    const ModifierOption* moClass = hasTypeModifier(t->m_modifiers, tmExternalClass);
    const ModifierOption* moStruct = hasTypeModifier(t->m_modifiers, tmExternalStruct);
    const ModifierOption* moImportBuiltin = hasTypeModifier(t->m_modifiers, tmImportAsBuiltIn);

    if (moClass)
      code.addUniqueHeader(extTypeGroup, moClass->m_value.c_str());
    else if (moStruct)
      code.addUniqueHeader(extTypeGroup, moStruct->m_value.c_str());
    else if (t->m_struct)
    {
      sb.reset();
      sb.appendCharBuf("\"Types/");
      t->appendTypeFilename(sb);
      sb.appendCharBuf(".h\"");
      code.addUniqueHeader(knTypeGroup, sb.getBufferPtr());
    }

    if (moImportBuiltin)
      code.addUniqueHeader(extTypeGroup, moImportBuiltin->m_value.c_str());
  }

  p_it = dep.m_dependentTypesByPtrRef.const_begin();
  p_end = dep.m_dependentTypesByPtrRef.const_end();
  for (; p_it != p_end; ++p_it)
  {
    const Type* t = (*p_it);
    const ModifierOption* moImportBuiltin = hasTypeModifier(t->m_modifiers, tmImportAsBuiltIn);

    if (moImportBuiltin)
      code.addUniqueHeader(extTypeGroup, moImportBuiltin->m_value.c_str());
  }


  if (!forHeader)
    return;

  assert(fwdDeclsInternal && fwdDeclsExternal);
  fwdDeclsInternal->reset();
  fwdDeclsExternal->reset();

  p_it = dep.m_dependentTypesByPtrRef.const_begin();
  p_end = dep.m_dependentTypesByPtrRef.const_end();
  for (; p_it != p_end; ++p_it)
  {
    const Type* t = (*p_it);
    const ModifierOption* moClass = hasTypeModifier(t->m_modifiers, tmExternalClass);
    const ModifierOption* moStruct = hasTypeModifier(t->m_modifiers, tmExternalStruct);
    const ModifierOption* moImportBuiltin = hasTypeModifier(t->m_modifiers, tmImportAsBuiltIn);
    bool alreadyIncluded = false;

    if (moImportBuiltin)
      continue;

    // check to see if this type was already included
    it = dep.m_dependentTypes.const_begin();
    for (; it != end; ++it)
    {
      if ((*it) == t)
      {
        alreadyIncluded = true;
        break;
      }
    }

    if (!alreadyIncluded)
    {
      // choose with list to emit into; externals will live outside the global library namespace declaration
      StringBuilder* fwdDecls = (moClass || moStruct) ? fwdDeclsExternal : fwdDeclsInternal;

      fwdDecls->appendCharBuf(code.makeTabString().c_str());

      if (!t->m_namespace.empty())
      {
        for (unsigned int i=0; i<t->m_namespaceStack.count(); i++)
        {
          fwdDecls->appendf("namespace %s { ", t->m_namespaceStack.get(i).c_str());
        }
      }

      if (moClass)
        fwdDecls->appendf("class %s; ", t->m_name.c_str());
      else
        fwdDecls->appendf("struct %s; ", t->m_name.c_str());

      if (!t->m_namespace.empty())
      {
        for (unsigned int i=0; i<t->m_namespaceStack.count(); i++)
          fwdDecls->appendCharBuf("} ", 2);
      }

      fwdDecls->appendChar('\n');
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// find any validator attributes applied to the given VarDecl, check decl first, as that overrides any set on the type
//
const ModifierOption* getValidatorForVarDecl(const VarDeclBase& vd)
{
  // we check var decl first, which overrides any type-validator set
  const ModifierOption* validator = hasVarDeclModifier(vd.m_modifiers, vdmValidator);
  if (validator == 0)
    validator = hasTypeModifier(vd.m_knownType->m_modifiers, tmValidator);

  return validator;
}

//----------------------------------------------------------------------------------------------------------------------
// returns true if the vardecl has some kind of validator attached; either directly, via it's type or
// if the structure contains vardecls that have validators
//
bool varDeclHasValidatorHook(const VarDeclBase& vd)
{
  return (getValidatorForVarDecl(vd) != 0) ||
    (vd.m_knownType->m_struct && vd.m_knownType->m_struct->m_hasValidationFunction);
}

//----------------------------------------------------------------------------------------------------------------------
void writeVarDeclValidator(CodeWriter& code, const VarDeclBase& vd, bool useArrayAPI)
{
  const char* vdName = vd.m_varname.c_str();
  const char* vdNameCapFront = vd.m_varnameCapFront.c_str();

  const ModifierOption* moPreProcessor = hasTypeModifier(vd.m_knownType->m_modifiers, tmPreProcessor);
  if (moPreProcessor)
    code.write("#if defined(%s)", moPreProcessor->m_value.c_str());

  // if the vardecl is a struct type, check to see if it has a validate() function
  if (vd.m_knownType->m_struct && vd.m_knownType->m_struct->m_hasValidationFunction)
  {
    if (vd.m_arraySize > 0)
    {
      if (useArrayAPI)
      {
        code.openBrace();
        code.write("uint32_t num%s = calculateNum%s();", vdNameCapFront, vdNameCapFront);
        code.write("for (uint32_t i=0; i<num%s; i++)", vdNameCapFront);
        {
          code.openBrace();
          {
            code.write("if (get%sImportance(i) > 0.0f)", vdNameCapFront);
            code.write("  %s[i].validate();", vdName);
          }
          code.closeBrace();
        }
        code.closeBrace();
      }
      else
      {
        for (unsigned int i=0; i<vd.m_arraySize; i++)
        {
          code.write("%s[%i].validate();", vdName, i);
        }
      }
    }
    else
    {
      if (useArrayAPI)
      {
        code.write("if (get%sImportance() > 0.0f)", vdNameCapFront);
        code.openBrace();
      }

      code.write("%s.validate();", vdName);

      if (useArrayAPI)
      {
        code.closeBrace();
      }
    }
  }
  else
  {
    // otherwise check for presence of a validator attribute, write in given function
    const ModifierOption* validator = getValidatorForVarDecl(vd);
    if (validator)
    {
      if (vd.m_arraySize > 0)
      {
        if (useArrayAPI)
        {
          code.openBrace();
          code.write("uint32_t num%s = calculateNum%s();", vdNameCapFront, vdNameCapFront);
          code.write("for (uint32_t i=0; i<num%s; i++)", vdNameCapFront);
          {
            code.openBrace();
            {
              code.write("if (get%sImportance(i) > 0.0f)", vdNameCapFront);
              code.openBrace();
              code.write("NM_VALIDATOR(%s(%s[i]), \"%s\");", validator->m_value.c_str(), vdName, vdName);
              code.closeBrace();
            }
            code.closeBrace();
          }
          code.closeBrace();
        }
        else
        {
          for (unsigned int i=0; i<vd.m_arraySize; i++)
          {
            code.write("NM_VALIDATOR(%s(%s[%i]), \"%s[%i]\");", validator->m_value.c_str(), vdName, i, vdName, i);
          }
        }
      }
      else
      {
        if (useArrayAPI)
        {
          code.write("if (get%sImportance() > 0.0f)", vdNameCapFront);
          code.openBrace();
        }

        code.write("NM_VALIDATOR(%s(%s), \"%s\");", validator->m_value.c_str(), vdName, vdName);

        if (useArrayAPI)
        {
          code.closeBrace();
        }
      }
    }
  }
  if (moPreProcessor)
    code.write("#endif // %s", moPreProcessor->m_value.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void writeVarDeclPostCombine(CodeWriter& code, const VarDeclBase& vd, const char* blockPrefix)
{
  const char* vdName = vd.m_varname.c_str();
  const char* vdBlock = (blockPrefix == 0) ? "" : blockPrefix;

  const ModifierOption* moPreProcessor = hasTypeModifier(vd.m_knownType->m_modifiers, tmPreProcessor);
  if (moPreProcessor)
    code.write("#if defined(%s)", moPreProcessor->m_value.c_str());

  // if the vardecl is a struct type, check to see if it has a postCombine() function
  if (vd.m_knownType->m_struct && vd.m_knownType->m_struct->m_hasPostCombineFunction)
  {
    if (vd.m_arraySize > 0)
    {
      for (unsigned int i=0; i<vd.m_arraySize; i++)
        code.write("%s%s[%i].postCombine();", vdBlock, vdName, i);
    }
    else
    {
      code.write("%s%s.postCombine();", vdBlock, vdName);
    }
  }
  else
  {
    // otherwise check for presence of a post-combine attribute, write in given function
    const ModifierOption* validator = hasTypeModifier(vd.m_knownType->m_modifiers, tmPostCombine);
    if (validator)
    {
      if (vd.m_arraySize > 0)
      {
        for (unsigned int i=0; i<vd.m_arraySize; i++)
          code.write("PostCombiners::%s(%s%s[%i]);", validator->m_value.c_str(), vdBlock, vdName, i);
      }
      else
      {
        code.write("PostCombiners::%s(%s%s);", validator->m_value.c_str(), vdBlock, vdName);
      }
    }
  }
  if (moPreProcessor)
    code.write("#endif // %s", moPreProcessor->m_value.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
// write out a relative include path to the definition file supplied
//
void writeDefInclude(CodeWriter& code, const char* projectRootDir, const char* pathToDefinitionFile)
{
  code.writeComment("include definition file to create project dependency");
  code.writeRaw("#include \"");

  char relativeTypePath[MAX_PATH];

  // PathRelativePathTo can't cope with with a projectRootDir of .\ so handle that separately
  //
  if (strncmp(projectRootDir, ".\\", MAX_PATH - 1) == 0)
  {
    NMP_SPRINTF(relativeTypePath, MAX_PATH - 1, ".\\%s", pathToDefinitionFile);
  }
  else
  {
    if (!PathRelativePathTo(relativeTypePath, projectRootDir, FILE_ATTRIBUTE_DIRECTORY, pathToDefinitionFile, 0))
    {
      printf("\nERROR: could not compute relative path from project root:\n  root: '%s'\n  file: '%s'\n",
        projectRootDir,
        pathToDefinitionFile);
    }
  }

  // a speculative fix for relative paths generated thus: "/ModuleDefinitions/stuff.types"
  // we need it so the compiler treats it as properly relative, so it needs no forward slash or a anchoring '.'
  if (relativeTypePath[0] == '\\' || relativeTypePath[0] == '/')
    code.writeRaw(".");

  // Xcode only likes / in the include paths, bails on \ 
  size_t n = strlen(relativeTypePath);
  for (size_t i=0; i<n; i++)
  {
    if (relativeTypePath[i] == '\\')
      relativeTypePath[i] = '/';
  }

  code.writeRaw("%s\"", relativeTypePath);

  code.newLines(2);
}

//----------------------------------------------------------------------------------------------------------------------
void writeVarDecl(CodeWriter& code, const VarDeclBase& vd, DataFootprint& fc)
{
  if (vd.m_knownType->m_alignment > 0)
    fc.alignTo(code, vd.m_knownType->m_alignment);

  const ModifierOption* moPreProcessor = hasTypeModifier(vd.m_knownType->m_modifiers, tmPreProcessor);
  if (moPreProcessor)
    code.write("#if defined(%s)", moPreProcessor->m_value.c_str());

  StringBuilder varDecl(32);
  vd.toString(varDecl, false, true);

  code.writeTabs();
  code.writeRaw("%s;", varDecl.getBufferPtr());

  // Write any comment text attached to this decl
  if (!vd.m_comment.empty())
  {
    writeComment(code, vd.m_comment.c_str(), false);
  }

  // write out the alias name alongside, as reference
  if (vd.m_knownType->isAliasType())
  {
    code.writeTab();

    // try to align them to a column edge if we can, otherwise it gets a bit messy
    if (vd.m_comment.empty())
    {
      int spacing = 40 - varDecl.getLength();
      if (spacing < 0)
        spacing = 0;

      code.writeSpacing(spacing);

      vd.toString(varDecl, true, false);
      code.writeRaw("///< (%s)", varDecl.getBufferPtr());
    }
    else
    {
      vd.toString(varDecl, true, false);
      code.writeRaw("(%s)", varDecl.getBufferPtr());
    }
  }

  code.newLine();

  if (moPreProcessor)
    code.write("#endif // %s", moPreProcessor->m_value.c_str());

  fc.append(vd.getSize());
}

//----------------------------------------------------------------------------------------------------------------------
void writeVarDecls(const ModuleDef &module, CodeWriter& code, ModuleSection blk, DataFootprint& fc)
{
  iterate_begin_const(ModuleDef::VarDecl, module.m_vardecl[blk], vd)
  {
    writeVarDecl(code, vd, fc);
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
// Write-raw every comment line separately,
// inserting tabs in between, trim '\n' from the end of last line.
// If the comment is a trailing (single line) comment, optionally erase '<' trailing comment tag.
void writeComment(CodeWriter& code, const std::string& comment, bool eraseTrailingTag)
{
  const std::string endOfLine("\n");
  std::string::size_type breaklocation = std::string::npos;
  std::string newComment(comment);
  bool singleLineComment = true;
  const std::string::size_type commentSize = newComment.size();
  std::string::size_type currentSize = 0;

  do
  {
    breaklocation = newComment.find(endOfLine);
    if (breaklocation != std::string::npos)
    {
      std::string commentLine(newComment.substr(0, breaklocation + endOfLine.size()));
      currentSize += commentLine.size();

      if (currentSize == commentSize) // Last comment line?
      {
        // Trim '\n' from the end of last comment line.
        std::string::size_type breaklocation = commentLine.rfind(endOfLine);
        if (breaklocation == (commentLine.size() - endOfLine.size()))
        {
          commentLine.replace(breaklocation, endOfLine.size(), "");
        }
      }

      code.writeTabs();
      code.writeRaw(commentLine.c_str());
      newComment.erase(0, breaklocation + endOfLine.size());
      singleLineComment = false;
    }
    else if (singleLineComment)
    {
      if (eraseTrailingTag)
      {
        // Erase '<' from the comment if applicable.
        const std::string fwrdSlashLessThan("/<");
        const std::string lessThan("<");
        std::string newComment(comment);
        std::string::size_type breaklocation = std::string::npos;

        breaklocation = newComment.find(fwrdSlashLessThan);
        if (breaklocation != std::string::npos)
        {
          newComment.replace(breaklocation + 1, (lessThan).size(), "");
        }

        code.writeTabs();
        code.writeRaw(newComment.c_str());
      }
      else // This is a single line trailing comment containing '<' trailing comment tag.
      {
        code.writeTabs();
        code.writeRaw(comment.c_str());
      }
    }
  }
  while (breaklocation != std::string::npos);
}

//----------------------------------------------------------------------------------------------------------------------
// write out an enum; each entry looks like this:
//
//     /* <entry value> */  eEntryName < = OptionalLiteral>,
//
// eg.
//     /*  6 */ eTokenState = NetworkConstants::defaultState,
//
void writeEnumDecl(CodeWriter& code, const EnumDeclBase& enm)
{
  // Enum comment.
  if (!enm.m_comment.empty())
  {
    writeComment(code, enm.m_comment.c_str(), true);
    code.newLine();
  }

  // Enum.
  code.write("enum %s", enm.m_name.c_str());
  code.openBrace();

  EnumDeclBase::Entries::const_iterator eIt = enm.m_entries.begin(), eEnd = enm.m_entries.end();
  for (; eIt != eEnd; ++eIt)
  {
    const EnumDeclBase::Entry& enumEntry = (*eIt);

    if (enumEntry.m_expression.empty())
    {
      if (enumEntry.m_customValueUsed)
      {
        code.write(
          "/* %2i */ %s = %i,  %s",
          enumEntry.m_value,
          enumEntry.m_name.c_str(),
          enumEntry.m_value,
          enumEntry.m_comment.c_str());
      }
      else
      {
        code.write("/* %2i */ %s,  %s",enumEntry.m_value, enumEntry.m_name.c_str(), enumEntry.m_comment.c_str());
      }
    }
    else
    {
      code.write(
        "/* %2i */ %s = (%s),  %s",
        enumEntry.m_value,
        enumEntry.m_name.c_str(),
        enumEntry.m_expression.c_str(),
        enumEntry.m_comment.c_str());
    }
  }

  code.closeBraceWithSemicolon();
  code.newLine();
}

//----------------------------------------------------------------------------------------------------------------------
void populateDescendentList(const ModuleDef* def, ModuleDef::ConstChildPtrs &descendentList)
{
  iterate_begin_const(ModuleDef::Child, def->m_children, child)
  {
    descendentList.push_back(&child);

    populateDescendentList(child.m_modulePtr, descendentList);
  }
  iterate_end
}
