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
#include "EmitCPP_Module.h"

#include "NMPlatform/NMPlatform.h"

// -------------------------------------------------------------------------------------------------------------------
void addChildrenToUpdatePayloadRecursive(CodeWriter& code, const ModuleDef* module, const char* modulePath)
{
  iterate_begin_const(ModuleDef::Child, module->m_children, child)
  {
    if(child.m_arraySize)
    {
      for(uint32_t i = 0; i < child.m_arraySize; i++)
      {
        code.write("m_updatePayload->%s_%i = %s%s[%i]->m_updatePackage;", child.m_name.c_str(), i, modulePath, child.m_name.c_str(), i);

        // If the module has a connections structure, add a pointer to it here.
        if(!child.m_sku.empty())
        {
          // The child has a connections structure which is non-default
          code.write("m_updatePayload->%s%iCon = (%s_%sCon*)%s%s[%i]->getConnections();", child.m_name.c_str(), i, child.m_module.c_str(), child.m_sku.c_str(), modulePath, child.m_name.c_str(), i);
        }
        else if(child.m_modulePtr->m_connectionSets.size() != 0)
        {
          // The child is using the default sku
          code.write("m_updatePayload->%s%iCon = (%s_Con*)%s%s[%i]->getConnections();", child.m_name.c_str(), i, child.m_module.c_str(), modulePath, child.m_name.c_str(), i);
        }
      }
    }
    else
    {
      code.write("m_updatePayload->%s = %s%s->m_updatePackage;", child.m_name.c_str(), modulePath, child.m_name.c_str());
      
      // If the module has a connections structure, add a pointer to it here.
      if(!child.m_sku.empty())
      {
        // The child has a connections structure which is non-default
        code.write("m_updatePayload->%sCon = (%s_%sCon*)%s%s->getConnections();", child.m_name.c_str(), child.m_module.c_str(), child.m_sku.c_str(), modulePath, child.m_name.c_str());
      }
      else if(child.m_modulePtr->m_connectionSets.size() != 0)
      {
        // The child is using the default sku
        code.write("m_updatePayload->%sCon = (%s_Con*)%s%s->getConnections();", child.m_name.c_str(), child.m_module.c_str(), modulePath, child.m_name.c_str());
      }
    }

    StringBuilder newModulePath(32);
    newModulePath.reset();
    newModulePath.appendf("%s", modulePath);
    newModulePath.appendf("%s->", child.m_name.c_str());

    addChildrenToUpdatePayloadRecursive(code, child.m_modulePtr, newModulePath.getBufferPtr());
  }
  iterate_end

  return;
}

// -------------------------------------------------------------------------------------------------------------------
void addChildEnabledFlagsToUpdatePayloadRecursive(CodeWriter& code, const char* pathToRootModule, const ModuleDef* module, const char* modulePath)
{

  iterate_begin_const(ModuleDef::Child, module->m_children, child)
  {
    if(child.m_arraySize)
    {
      for(uint32_t i = 0; i < child.m_arraySize; i++)
      {        
        code.write("m_updatePayload->%s_%i_enabled = %sisEnabled(%s%s[%i]->getManifestIndex());", 
          child.m_name.c_str(), i,
          pathToRootModule,
          modulePath, child.m_name.c_str(), i);
      }
    }
    else
    {
      code.write("m_updatePayload->%s_enabled = %sisEnabled(%s%s->getManifestIndex());", 
        child.m_name.c_str(), 
        pathToRootModule,
        modulePath, 
        child.m_name.c_str());
    }

    StringBuilder newModulePath(32);
    newModulePath.reset();
    newModulePath.appendf("%s", modulePath);
    newModulePath.appendf("%s->", child.m_name.c_str());

    addChildEnabledFlagsToUpdatePayloadRecursive(code, pathToRootModule, child.m_modulePtr, newModulePath.getBufferPtr());
  }
  iterate_end

  return;
}

// -------------------------------------------------------------------------------------------------------------------
void ModuleGen::writeDataPacket(const ModuleDef &module, ModuleSection blk, bool emitImportanceAPI)
{
  StringBuilder strBuild(64), postAssignFn(16), doxComment(64);

  if (module.varDeclBlockIsEmpty(blk))
    return;

  // work out basic payload size
  unsigned int blockSizeBytes = 0;
  iterate_begin_const(ModuleDef::VarDecl, module.m_vardecl[blk], vd)
  {
    blockSizeBytes += vd.getSize();

    if (emitImportanceAPI)
      blockSizeBytes += sizeof(float) * ((vd.m_arraySize > 0)?vd.m_arraySize:1);
  }
  iterate_end

  unsigned int blockSizeAligned = blockSizeBytes + (32 - (blockSizeBytes % 32));

  // form name, <module><blockname>
  strBuild.appendf("%s%s", module.m_name.c_str(), keywords.getModuleSectionLabel(blk));

  // Module doxygen comment.
  doxComment.appendCharBuf(keywords.getModuleSectionLabel(blk).c_str());

  if (!module.m_shortDesc.empty())
  {
    // Process verbatim comment by performing the following operations:
    // 1. Trim // or /// from the beginning of the comment.
    // 2. Swap every other // or /// to *.
    // This is necessary because this text will be pasted inside a doxygen comment block.

    std::string text(module.m_shortDesc.c_str());
    std::string::size_type position =std::string::npos;

    // Replace /// with nothing if at the beginning of comment, otherwise replace it with *.
    const std::string fwdSlashTriplet("///");
    do
    {
      position = text.find(fwdSlashTriplet);
      if (position != std::string::npos)
      {
        const std::string replaceSymbol = (position == 0) ? ("") : (" * ");
        text.replace(position, fwdSlashTriplet.size(), replaceSymbol);
      }
    }
    while (position != std::string::npos);

    // Replace // with nothing if at the beginning of comment , otherwise replace it with *.
    const std::string fwdSlashDublet("//");
    do
    {
      position = text.find(fwdSlashDublet);
      if (position != std::string::npos)
      {
        const std::string replaceSymbol = (position == 0) ? ("") : (" * ");
        text.replace(position, fwdSlashDublet.size(), replaceSymbol);
      }
    }
    while (position != std::string::npos);

    doxComment.appendf("%s", text.c_str());
  } // End of comment processing.

  // open struct
  code.writeDoxygenClassComment(strBuild.getBufferPtr(), module.m_name.c_str(), doxComment.getBufferPtr(), "Data packet definition (%i bytes, %i aligned)", blockSizeBytes, blockSizeAligned);
  code.write("NMP_ALIGN_PREFIX(32) struct %s", strBuild.getBufferPtr());
  code.openBrace();

  // write clear() function
  code.write("NM_INLINE void clear()");
  code.openBrace();
  if (blockSizeBytes > 0)
    code.write("memset(this, 0, sizeof(%s));", strBuild.getBufferPtr());
  code.closeBrace();
  code.newLine();

  // any enumerations defined in the module get thrown into the data section
  if (blk == msData)
  {
    ModuleDef::EnumDecls::const_iterator enum_it = module.m_enumDecl.begin(),
                                         enum_end = module.m_enumDecl.end();
    for (; enum_it != enum_end; ++enum_it)
    {
      const ModuleDef::EnumDecl& enm = (*enum_it);
      writeEnumDecl(code, enm);
    }
  }

  bool varDeclsHaveValidators = false;
  if (emitImportanceAPI)
  {
    // write accessors
    iterate_begin_const(ModuleDef::VarDecl, module.m_vardecl[blk], vd)
    {
      bool disableWriteAccess = false;
      if ((blk == msFeedOutputs || blk == msOutputs) && vd.m_connectedJunctions.size())
      {
        disableWriteAccess = true;
      }


      // capitalize the first character of the variable name
      const char* vdName = vd.m_varname.c_str();
      const char* vdNameCapFront = vd.m_varnameCapFront.c_str();

      // get decl to describe its type
      vd.toString(strBuild, true, true);

      // we write a post-assign function if the variable has a validator hook, or if it is a struct
      // that has a validation function callable
      bool writePostAssignFn = varDeclHasValidatorHook(vd);

      postAssignFn.reset();
      varDeclsHaveValidators |= writePostAssignFn;

      if (vd.m_arraySize == 0)
      {
        if (writePostAssignFn)
          postAssignFn.appendf("postAssign%s(%s);", vdNameCapFront, vdName);

        if (!disableWriteAccess)
        {
          code.write("NM_INLINE void set%s(const %s& _%s, float %s_importance = 1.f)",
            vdNameCapFront,
            strBuild.getBufferPtr(),
            vdName,
            vdName);
          code.openBrace();
          {
            code.write("%s = _%s;", vdName, vdName);
            if (writePostAssignFn)
              code.write(postAssignFn.c_str());

            code.write("NMP_ASSERT(%s_importance >= 0.0f && %s_importance <= 1.0f);", vdName, vdName);
            code.write("m_%sImportance = %s_importance;", vdName, vdName);
          }
          code.closeBrace();


          // if the variable is a struct of reasonable size, add modify API to allow local modification
          // without copying via setBlah()
          if (vd.m_knownType->isCompoundType() &&
              vd.m_knownType->m_sizeInBytes >= 64)
          {
            code.write("NM_INLINE %s& start%sModification()",
              strBuild.getBufferPtr(),
              vdNameCapFront);
            code.openBrace();
            code.write("m_%sImportance = -1.0f; // set invalid until stop%sModification()", vdName, vdNameCapFront);
            code.write("return %s;", vdName);
            code.closeBrace();

            code.write("NM_INLINE void stop%sModification(float %s_importance = 1.f)",
              vdNameCapFront,
              vdName);
            code.openBrace();
            {
              if (writePostAssignFn)
                code.write(postAssignFn.c_str());

              code.write("NMP_ASSERT(%s_importance >= 0.0f && %s_importance <= 1.0f);", vdName, vdName);
              code.write("m_%sImportance = %s_importance;", vdName, vdName);
            }
            code.closeBrace();
          }

        }

        code.write("NM_INLINE float get%sImportance() const { return m_%sImportance; }", 
          vdNameCapFront,
          vdName);

        code.write("NM_INLINE const float& get%sImportanceRef() const { return m_%sImportance; }", 
          vdNameCapFront,
          vdName);

        code.write("NM_INLINE const %s& get%s() const { return %s; }", 
          strBuild.getBufferPtr(),
          vdNameCapFront,
          vdName);
      }
      else
      {
        if (writePostAssignFn)
          postAssignFn.appendf("postAssign%s(%s[i]);", vdNameCapFront, vdName);

        code.write("enum { max%s = %i };", 
          vdNameCapFront,
          vd.m_arraySize);

        code.write("NM_INLINE unsigned int getMax%s() const { return max%s; }", 
          vdNameCapFront,
          vdNameCapFront);

        if (!disableWriteAccess)
        {
          code.write("NM_INLINE void set%s(unsigned int number, const %s* _%s, float %s_importance = 1.f)",
            vdNameCapFront,
            strBuild.getBufferPtr(),
            vdName, 
            vdName);
          code.openBrace();
          {
            code.write("NMP_ASSERT(number <= max%s);", vdNameCapFront);
            code.write("NMP_ASSERT(%s_importance >= 0.0f && %s_importance <= 1.0f);", vdName, vdName);
            code.write("for (unsigned int i = 0; i<number; i++)");
            code.openBrace();
            code.write("%s[i] = _%s[i];", vdName, vdName);
            if (writePostAssignFn)
              code.write(postAssignFn.c_str());
            code.write("m_%sImportance[i] = %s_importance;", vdName, vdName);
            code.closeBrace();
          }
          code.closeBrace();

          if (writePostAssignFn)
          {
            postAssignFn.reset();
            postAssignFn.appendf("postAssign%s(%s[index]);", vdNameCapFront, vdName);
          }

          code.write("NM_INLINE void set%sAt(unsigned int index, const %s& _%s, float %s_importance = 1.f)",
            vdNameCapFront,
            strBuild.getBufferPtr(),
            vdName, 
            vdName);
          code.openBrace();
          {
            code.write("NMP_ASSERT(index < max%s);", vdNameCapFront);
            code.write("%s[index] = _%s;", vdName, vdName);
            code.write("NMP_ASSERT(%s_importance >= 0.0f && %s_importance <= 1.0f);", vdName, vdName);
            if (writePostAssignFn)
              code.write(postAssignFn.c_str());
            code.write("m_%sImportance[index] = %s_importance;", vdName, vdName);
          }
          code.closeBrace();
        }

        // if the variable is a struct of reasonable size, add modify API to allow local modification
        // without copying via setBlahAt()
        if (vd.m_knownType->isCompoundType() &&
          vd.m_knownType->m_sizeInBytes >= 64)
        {
          code.write("NM_INLINE %s& start%sModificationAt(unsigned int index)",
            strBuild.getBufferPtr(),
            vdNameCapFront);
          code.openBrace();
          code.write("NMP_ASSERT(index <= max%s);", vdNameCapFront);
          code.write("m_%sImportance[index] = -1.0f; // set invalid until stop%sModificationAt()", vdName, vdNameCapFront);
          code.write("return %s[index];", vdName);
          code.closeBrace();

          code.write("NM_INLINE void stop%sModificationAt(unsigned int index, float %s_importance = 1.f)",
            vdNameCapFront,
            vdName);
          code.openBrace();
          {
            if (writePostAssignFn)
              code.write(postAssignFn.c_str());

            code.write("NMP_ASSERT(%s_importance >= 0.0f && %s_importance <= 1.0f);", vdName, vdName);
            code.write("m_%sImportance[index] = %s_importance;", vdName, vdName);
          }
          code.closeBrace();
        }

        code.write("NM_INLINE float get%sImportance(int index) const { return m_%sImportance[index]; }", 
          vdNameCapFront,
          vdName);

        code.write("NM_INLINE const float& get%sImportanceRef(int index) const { return m_%sImportance[index]; }", 
          vdNameCapFront,
          vdName);

        code.write("NM_INLINE unsigned int calculateNum%s() const", vdNameCapFront);
        code.openBrace();
        code.write("for (int i=%i; i>=0; --i)", vd.m_arraySize -1);
        code.openBrace();
        code.write("if (m_%sImportance[i])", vdName);
        code.write("  return ((unsigned int)(i+1));");
        code.closeBrace();
        code.write("return 0;");
        code.closeBrace();

        code.write("NM_INLINE const %s& get%s(unsigned int index) const { NMP_ASSERT(index <= max%s); return %s[index]; }", 
          strBuild.getBufferPtr(),
          vdNameCapFront,
          vdNameCapFront,
          vdName);
      }

      code.newLine();
    }
    iterate_end

    code.write("protected:");
    code.newLine();
  }

  if (blockSizeBytes == 0)
    code.write("float _pad;");

  DataFootprint fc;
  writeVarDecls(module, code, blk, fc);

  if (emitImportanceAPI && !module.m_vardecl[blk].empty())
  {
    code.newLine();

    if (varDeclsHaveValidators)
    {
      code.writeComment("post-assignment stubs");
      iterate_begin_const(ModuleDef::VarDecl, module.m_vardecl[blk], vd)
      {
        if (varDeclHasValidatorHook(vd))
        {
          // capitalize the first character of the variable name
          const char* vdName = vd.m_varname.c_str();
          const char* vdNameCapFront = vd.m_varnameCapFront.c_str();

          vd.toString(strBuild, true, true);
          code.write("NM_INLINE void postAssign%s(const %s& NMP_VALIDATOR_ARG(v)) const", vdNameCapFront, strBuild.c_str());
          code.openBrace();

          code.write("#if NM_CALL_VALIDATORS");
          if (vd.m_knownType->m_struct)
          {
            code.write("v.validate();");
          }
          else
          {
            const ModifierOption* validator = getValidatorForVarDecl(vd);
            code.write("NM_VALIDATOR(%s(v), \"%s\");", validator->m_value.c_str(), vdName);
          }
          code.write("#endif // NM_CALL_VALIDATORS");

          code.closeBrace();
          code.newLine();
        }
      }
      iterate_end
    }

    code.writeComment("importance values");
    std::string tabGap = code.makeTabString();


    code.startCommaSeparatedList();
    iterate_begin_const(ModuleDef::VarDecl, module.m_vardecl[blk], vd)
    {
      if (vd.m_arraySize == 0)
        code.addToCommaSeparatedList("%s m_%sImportance", tabGap.c_str(), vd.m_varname.c_str());
      else
      {
        if (vd.m_arraySizeAsString.empty())
          code.addToCommaSeparatedList("%s m_%sImportance[%i]", tabGap.c_str(), vd.m_varname.c_str(), vd.m_arraySize);
        else
          code.addToCommaSeparatedList("%s m_%sImportance[%s]", tabGap.c_str(), vd.m_varname.c_str(), vd.m_arraySizeAsString.c_str());
      }
    }
    iterate_end

    code.write("float");
    code.emitCommaSeparatedList();
    code.writeRaw(";");

    code.newLines(2);

    iterate_begin_const_ptr(ModuleDef::ConnectionSet, module.m_connectionSets, set)
    {
      code.write("friend class %s_%sCon;", module.m_name.c_str(), set->m_name.c_str());
    }
    iterate_end
  }

  code.newLine();
  code.write("public:");
  code.newLine();

  // write out a validate() function for those data blocks without get/set API
  {
    code.writeComment("manually call any assigned validators");
    code.write("NM_INLINE void validate() const");
    code.openBrace();

    code.write("#if NM_CALL_VALIDATORS");
    iterate_begin_const(ModuleDef::VarDecl, module.m_vardecl[blk], vd)
    {
      writeVarDeclValidator(code, vd, blk != msData);
    }
    iterate_end
    code.write("#endif // NM_CALL_VALIDATORS");

    code.closeBrace();
    code.newLine();
  }

  code.closeBrace(false);
  code.writeRaw(" NMP_ALIGN_SUFFIX(32);");
  code.newLines(2);
  code.writeHorizontalSeparator();
}

// -------------------------------------------------------------------------------------------------------------------
bool ModuleGen::writePackagingHeader(const ModuleDef &module)
{
  HeaderGuard hg("%s_PKG", module.m_name.c_str());
  code.writeHeaderTop(hg);

  // write include to original definition file, creating dependency binding
  writeDefInclude(code, projectRootDir, module.m_pathToDef.c_str());

  code.write("#include \"%sData.h\"", module.m_name.c_str());
  iterate_begin_const(ModuleDef::Child, module.m_children, child)
  {
    code.write("#include \"%sData.h\"", child.m_module.c_str());
  }
  iterate_end

  code.newLine();
  if(module.m_moduleOwnerPtr)
    code.write("#include \"%sPackaging.h\"", module.m_moduleOwnerPtr->m_name.c_str());

  code.newLine();
  code.newLines(2);

  // fwd decls
  code.write("namespace MR");
  code.openBrace();
  code.write("class InstanceDebugInterface;");
  code.closeBrace();
  code.newLine();

  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");


  ModuleDef::ConstChildPtrs descendents;
  populateDescendentList(&module, descendents);

  ModuleDef::ConstChildPtrs::iterator descendent_it   = descendents.begin();
  for (; descendent_it != descendents.end(); ++descendent_it)
  {
    const ModuleDef::Child* descendent = (*descendent_it);

    code.write("struct %sUpdatePackage;", descendent->m_module.c_str());
    code.write("struct %sFeedbackPackage;", descendent->m_module.c_str());

    if(!descendent->m_sku.empty())
    {
      // The descendent has a connections structure which is non-default
      code.write("class %s_%sCon;", descendent->m_module.c_str(), descendent->m_sku.c_str());
    }
    else if (descendent->m_modulePtr->m_connectionSets.size() != 0)
    {
      // The descendent is using the default SKU
      code.write("class %s_Con;", descendent->m_module.c_str());
    }

  }

  code.newLine();
  code.writeHorizontalSeparator();

  StringBuilder apiBaseCtor(32);
  code.writeComment("API Packaging");
  {
    code.write("struct %sAPIBase", module.m_name.c_str());
    code.openBrace();

    // build ctor args list
    sb.reset();
    code.startCommaSeparatedList();
    if (module.varDeclBlockIsNotEmpty(msData))
    {
      code.addToCommaSeparatedList("const %s%s* const _data",    module.m_name.c_str(), keywords.getModuleSectionLabel(msData).c_str());
      sb.appendf(",data(_data) ");
      apiBaseCtor.appendf(",_data ");
    }
    if (module.varDeclBlockIsNotEmpty(msInputs))
    {
      code.addToCommaSeparatedList("const %s%s* const _in",      module.m_name.c_str(), keywords.getModuleSectionLabel(msInputs).c_str());
      sb.appendf(",in(_in) ");
      apiBaseCtor.appendf(",_in ");
    }
    if (module.varDeclBlockIsNotEmpty(msUserInputs))
    {
      code.addToCommaSeparatedList("const %s%s* const _userIn",      module.m_name.c_str(), keywords.getModuleSectionLabel(msUserInputs).c_str());
      sb.appendf(",userIn(_userIn) ");
      apiBaseCtor.appendf(",_userIn ");
    }
    if (module.varDeclBlockIsNotEmpty(msFeedInputs))
    {
      code.addToCommaSeparatedList("const %s%s* const _feedIn", module.m_name.c_str(), keywords.getModuleSectionLabel(msFeedInputs).c_str());
      sb.appendf(",feedIn(_feedIn) ");
      apiBaseCtor.appendf(",_feedIn ");
    }
    if (module.m_moduleOwnerPtr)
    {
      code.addToCommaSeparatedList("const %sAPIBase* const _owner", module.m_moduleOwnerPtr->m_name.c_str());
      sb.appendf(",owner(_owner) ");
      apiBaseCtor.appendf(",_owner ");
    }

    // cut off the initial comma
    sb.trimCharsFromFront(',');
    apiBaseCtor.trimCharsFromFront(',');


    // ctor initializer list
    code.write("%sAPIBase(", module.m_name.c_str());
    code.emitCommaSeparatedList(1);
    code.write(") :%s {}", sb.c_str());
    code.newLine();

    if (module.varDeclBlockIsNotEmpty(msData))
      code.write("const %s%s* const data;",    module.m_name.c_str(), keywords.getModuleSectionLabel(msData).c_str());
    if (module.varDeclBlockIsNotEmpty(msInputs))
      code.write("const %s%s* const in;",      module.m_name.c_str(), keywords.getModuleSectionLabel(msInputs).c_str());
    if (module.varDeclBlockIsNotEmpty(msUserInputs))
      code.write("const %s%s* const userIn;",      module.m_name.c_str(), keywords.getModuleSectionLabel(msUserInputs).c_str());
    if (module.varDeclBlockIsNotEmpty(msFeedInputs))
      code.write("const %s%s* const feedIn;", module.m_name.c_str(), keywords.getModuleSectionLabel(msFeedInputs).c_str());

    // owner pointer (if it exists)
    if (module.m_moduleOwnerPtr)
    {
      code.newLine();
      code.write("const %sAPIBase* const owner;", module.m_moduleOwnerPtr->m_name.c_str());
    }

    code.newLine();
    code.write("%sAPIBase(const %sAPIBase& rhs);", module.m_name.c_str(), module.m_name.c_str());
    code.write("%sAPIBase& operator = (const %sAPIBase& rhs);", module.m_name.c_str(), module.m_name.c_str());

    if (module.m_publicAPI.size() > 0)
    {
      code.newLine();
      code.writeComment("public API functions declared in module definition");
      code.newLine();
      ModuleDef::FnDecls::const_iterator fIt = module.m_publicAPI.begin();
      ModuleDef::FnDecls::const_iterator fEnd = module.m_publicAPI.end();
      for (; fIt != fEnd; ++fIt)
      {
        // Function comment.
        if (!((*fIt).m_shortDesc.empty()))
        {
          // Erase < from the comment if there is any.
          // Write-raw every comment line separately,
          // inserting tabs in between, trim '\n' from the end of last line.

          const std::string endOfLine("\n");
          const std::string fwrdSlashLessThan("/<");
          const std::string lessThan("<");
          std::string::size_type breaklocation = std::string::npos;
          std::string comment((*fIt).m_shortDesc.c_str());
          bool singleLineComment = true;
          const std::string::size_type commentSize = comment.size();
          std::string::size_type currentSize = 0;

          // Erase < from the comment.
          breaklocation = comment.find(fwrdSlashLessThan);
          if (breaklocation != std::string::npos)
          {
            comment.replace(breaklocation + 1, (lessThan).size(), "");
          }

          do
          {
            breaklocation = comment.find(endOfLine);
            if (breaklocation != std::string::npos)
            {
              std::string commentLine(comment.substr(0, breaklocation + endOfLine.size()));
              currentSize += commentLine.size();
              if (currentSize == commentSize) // Last comment line?
              {
                // Trim \n from the end of last comment line.
                std::string::size_type breaklocation = commentLine.rfind(endOfLine);
                if (breaklocation == (commentLine.size() - endOfLine.size()))
                {
                  commentLine.replace(breaklocation, endOfLine.size(), "");
                }
              }
              code.writeTabs();
              code.writeRaw(commentLine.c_str());
              comment.erase(0, breaklocation + endOfLine.size());
              singleLineComment = false;
            }
            else if (singleLineComment)
            {
              code.writeTabs();
              code.writeRaw(comment.c_str());
            }
          }
          while (breaklocation != std::string::npos);

          code.newLine();
        } // End of function comment processing.

        // Function code.
        code.write("%s", (*fIt).m_fn.c_str());
      }
      code.newLine();
    }

    code.closeBraceWithSemicolon();
  }

  code.newLine();
  code.writeHorizontalSeparator();

  if (!hasModuleModifier(module.m_modifiers, mmGlobalAccess))
  {
    code.writeComment("Update Packaging");
    code.write("struct %sUpdatePackage : public %sAPIBase", module.m_name.c_str(), module.m_name.c_str());
    code.openBrace();

    // build ctor args list
    sb.reset();
    code.startCommaSeparatedList();
    if (module.varDeclBlockIsNotEmpty(msData))
    {
      code.addToCommaSeparatedList("%s%s* const _data",          module.m_name.c_str(), keywords.getModuleSectionLabel(msData).c_str());
      sb.appendf(", data(_data)");
    }
    if (module.varDeclBlockIsNotEmpty(msInputs))
    {
      code.addToCommaSeparatedList("const %s%s* const _in",      module.m_name.c_str(), keywords.getModuleSectionLabel(msInputs).c_str());
    }
    if (module.varDeclBlockIsNotEmpty(msUserInputs))
    {
      code.addToCommaSeparatedList("const %s%s* const _userIn",  module.m_name.c_str(), keywords.getModuleSectionLabel(msUserInputs).c_str());
    }
    if (module.varDeclBlockIsNotEmpty(msFeedInputs))
    {
      code.addToCommaSeparatedList("const %s%s* const _feedIn",  module.m_name.c_str(), keywords.getModuleSectionLabel(msFeedInputs).c_str());
    }
    if (module.varDeclBlockIsNotEmpty(msOutputs))
    {
      code.addToCommaSeparatedList("%s%s* const _out",           module.m_name.c_str(), keywords.getModuleSectionLabel(msOutputs).c_str());
      sb.appendf(", out(_out)");
    }
    if (module.m_moduleOwnerPtr)
    {
      code.addToCommaSeparatedList("const %sAPIBase* const _owner", module.m_moduleOwnerPtr->m_name.c_str());
    }

    // ctor initializer list
    code.write("%sUpdatePackage(", module.m_name.c_str());
    code.emitCommaSeparatedList(1);
    code.write(") : %sAPIBase(%s)%s ", module.m_name.c_str(), apiBaseCtor.c_str(), sb.c_str());

    code.openBrace();

    code.closeBrace();
    code.newLine();

    if (module.varDeclBlockIsNotEmpty(msData))
      code.write("%s%s* const data;",    module.m_name.c_str(), keywords.getModuleSectionLabel(msData).c_str());
    code.newLine();
    if (module.varDeclBlockIsNotEmpty(msOutputs))
      code.write("%s%s* const out;",    module.m_name.c_str(), keywords.getModuleSectionLabel(msOutputs).c_str());

    code.newLines(2);
    
    // Update api if required (as specified in the module def)
    if ( hasModuleModifier(module.m_modifiers, mmNoUpdate) )
    {
      code.writeComment("No update api required for this module!");
    }
    else
    {
      code.writeComment("module update entrypoint");
      code.write("void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);");
    }

    code.newLine();
    code.newLine();
    code.write("%sUpdatePackage(const %sUpdatePackage& rhs);", module.m_name.c_str(), module.m_name.c_str());
    code.write("%sUpdatePackage& operator = (const %sUpdatePackage& rhs);", module.m_name.c_str(), module.m_name.c_str());
    
    code.closeBraceWithSemicolon();
  }

  code.writeHorizontalSeparator();

  if (!hasModuleModifier(module.m_modifiers, mmGlobalAccess))
  {
    code.writeComment("Feedback Packaging");
    code.write("struct %sFeedbackPackage : public %sAPIBase", module.m_name.c_str(), module.m_name.c_str());
    code.openBrace();

    // build ctor args list
    sb.reset();
    code.startCommaSeparatedList();
    if (module.varDeclBlockIsNotEmpty(msData))
    {
      code.addToCommaSeparatedList("%s%s* const _data",          module.m_name.c_str(), keywords.getModuleSectionLabel(msData).c_str());
      sb.appendf(", data(_data)");
    }
    if (module.varDeclBlockIsNotEmpty(msFeedInputs))
    {
      code.addToCommaSeparatedList("const %s%s* const _feedIn", module.m_name.c_str(), keywords.getModuleSectionLabel(msFeedInputs).c_str());
    }
    if (module.varDeclBlockIsNotEmpty(msInputs))
    {
      code.addToCommaSeparatedList("const %s%s* const _in",      module.m_name.c_str(), keywords.getModuleSectionLabel(msInputs).c_str());
    }
    if (module.varDeclBlockIsNotEmpty(msUserInputs))
    {
      code.addToCommaSeparatedList("const %s%s* const _Userin",  module.m_name.c_str(), keywords.getModuleSectionLabel(msUserInputs).c_str());
    }
    if (module.varDeclBlockIsNotEmpty(msFeedOutputs))
    {
      code.addToCommaSeparatedList("%s%s* const _feedOut",      module.m_name.c_str(), keywords.getModuleSectionLabel(msFeedOutputs).c_str());
      sb.appendf(", feedOut(_feedOut)");
    }
    if (module.m_moduleOwnerPtr)
    {
      code.addToCommaSeparatedList("const %sAPIBase* const _owner", module.m_moduleOwnerPtr->m_name.c_str());
    }

    // ctor initializer list
    code.write("%sFeedbackPackage(", module.m_name.c_str());
    code.emitCommaSeparatedList(1);
    code.write(") : %sAPIBase(%s)%s ", module.m_name.c_str(), apiBaseCtor.c_str(), sb.c_str());

    code.openBrace();
    code.closeBrace();
    code.newLine();

    if (module.varDeclBlockIsNotEmpty(msData))
      code.write("%s%s* const data;",    module.m_name.c_str(), keywords.getModuleSectionLabel(msData).c_str());
    code.newLine();
    if (module.varDeclBlockIsNotEmpty(msFeedOutputs))
      code.write("%s%s* const feedOut;",    module.m_name.c_str(), keywords.getModuleSectionLabel(msFeedOutputs).c_str());

    code.newLines(2);

    // Feedback api if required (as specified in the module def)
    if ( hasModuleModifier(module.m_modifiers, mmNoFeedback) )
    {
      code.writeComment("No update api required for this module!");
    }
    else
    {
      code.writeComment("module feedback entrypoint");
      code.write("void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);");
    }

    code.newLine();
    code.newLine();
    code.write("%sFeedbackPackage(const %sFeedbackPackage& rhs);", module.m_name.c_str(), module.m_name.c_str());
    code.write("%sFeedbackPackage& operator = (const %sFeedbackPackage& rhs);", module.m_name.c_str(), module.m_name.c_str());

    code.closeBraceWithSemicolon();
  }
  code.newLine();
  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLine();

  code.writeHeaderBottom(hg);
  return true;
}

// -------------------------------------------------------------------------------------------------------------------
bool ModuleGen::writeDataHeader(const ModuleDef &module)
{
  HeaderGuard hgLabel("%s_DATA", module.m_name.c_str());

  code.writeHeaderTop(hgLabel);

  // write include to original definition file, creating dependency binding
  writeDefInclude(code, projectRootDir, module.m_pathToDef.c_str());

  // emit dependent headers
  StringBuilder fwdDeclsExternal(64), fwdDeclsInternal(64);
  code.startUniqueHeaders();
  calculateDependentHeadersAndFwdDecls(module.m_typeDependencies, true, code, &fwdDeclsExternal, &fwdDeclsInternal);

  // add validator headers to the pile
  unsigned int validatorGroup = code.addUniqueHeaderGroup("hook functions");
  code.addUniqueHeader(validatorGroup, "\"euphoria/erValueValidators.h\"");
  code.addUniqueHeader(validatorGroup, "\"euphoria/erValuePostCombiners.h\"");

  unsigned int constGroup = code.addUniqueHeaderGroup("constants");
  code.addUniqueHeader(constGroup, "\"NetworkConstants.h\"");

  // write out includes
  code.emitUniqueHeaders();
  code.newLine();

  // manually add string.h for memset
  code.write("#include <string.h>");
  code.newLine();

  // write forward declarations calculated above
  if (fwdDeclsExternal.getLength() > 0)
  {
    code.newLine();
    code.writeRaw(fwdDeclsExternal.getBufferPtr());
  }

  // prepare to write out data for each block type
  code.writeHorizontalSeparator();

  code.write("#ifndef NM_BEHAVIOUR_LIB_NAMESPACE");
  code.write("  #error behaviour library namespace undefined");
  code.write("#endif");
  code.newLine();
  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");

  if (fwdDeclsInternal.getLength() > 0)
  {
    code.writeRaw(fwdDeclsInternal.getBufferPtr());
    code.newLine();
  }

  code.write("#ifdef _MSC_VER");
  code.write("  #pragma warning(push)");
  code.write("  #pragma warning(disable: 4324) // structure was padded due to __declspec(align()) ");
  code.write("#endif // _MSC_VER");
  code.newLine();

  writeDataPacket(module, msData, false);
  writeDataPacket(module, msInputs, true);
  writeDataPacket(module, msUserInputs, true);
  writeDataPacket(module, msOutputs, true);
  writeDataPacket(module, msFeedInputs, true);
  writeDataPacket(module, msFeedOutputs, true);

  code.newLine();
  code.write("#ifdef _MSC_VER");
  code.write("  #pragma warning(pop)");
  code.write("#endif // _MSC_VER");

  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLine();

  code.writeHeaderBottom(hgLabel);
  return true;
}

// -------------------------------------------------------------------------------------------------------------------
bool ModuleGen::writeHeader(const ModuleDef &module)
{
  HeaderGuard hg(module.m_name.c_str());

  code.writeHeaderTop(hg);

  // write include to original definition file, creating dependency binding
  writeDefInclude(code, projectRootDir, module.m_pathToDef.c_str());

  StringBuilder fwdDeclsExternal(64), fwdDeclsInternal(64);
  code.startUniqueHeaders();
  calculateDependentHeadersAndFwdDecls(module.m_typeDependencies, true, code, &fwdDeclsExternal, &fwdDeclsInternal);

  unsigned int mdfBaseGroup = code.addUniqueHeaderGroup("base dependencies");
  code.addUniqueHeader(mdfBaseGroup, "\"euphoria/erJunction.h\"");
  code.addUniqueHeader(mdfBaseGroup, "\"euphoria/erModule.h\"");
  code.addUniqueHeader(mdfBaseGroup, "\"euphoria/erCharacter.h\"");

  unsigned int mdfDataGroup = code.addUniqueHeaderGroup("module data");
  code.addUniqueHeader(mdfDataGroup, "\"%sData.h\"", module.m_name.c_str());

  code.emitUniqueHeaders();

  if (fwdDeclsExternal.getLength() > 0)
  {
    code.newLine();
    code.writeRaw(fwdDeclsExternal.getBufferPtr());
  }

  code.newLine();
  code.write("namespace MR { struct PhysicsSerialisationBuffer; }");

  code.newLine();
  code.writeHorizontalSeparator();

  code.write("#ifndef NM_BEHAVIOUR_LIB_NAMESPACE");
  code.write("  #error behaviour library namespace undefined");
  code.write("#endif");
  code.newLine();

  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");

  if (fwdDeclsInternal.getLength() > 0)
  {
    code.writeRaw(fwdDeclsInternal.getBufferPtr());
    code.newLine();
  }

  // fwd-declare child modules
  // children block
  iterate_begin_const(ModuleDef::Child, module.m_children, child)
  {
    code.write("class %s;", child.m_module.c_str(), child.m_name.c_str());
  }
  iterate_end


  if (module.isRootModule())
  {
    code.write("class %sDataAllocator : public ER::ModuleDataAllocator", module.m_name.c_str());
    code.openBrace();
    code.write("public:");
    code.newLine();

    code.write("virtual void init() NM_OVERRIDE;");
    code.write("virtual void* alloc(ER::ModuleDataAllocator::Section sct, size_t size, size_t alignment) NM_OVERRIDE;");
    code.write("virtual void clear(ER::ModuleDataAllocator::Section sct) NM_OVERRIDE;");
    code.write("virtual void term() NM_OVERRIDE;");

    code.write("private:");
    code.write("unsigned char*   m_mem[ER::ModuleDataAllocator::NumSections];");
    code.write("size_t           m_offsets[ER::ModuleDataAllocator::NumSections],");
    code.write("                 m_limits[ER::ModuleDataAllocator::NumSections];");

    code.closeBraceWithSemicolon();

    code.newLine();
    code.writeHorizontalSeparator();
  }

  if (!module.m_moduleOwnerClassName.empty())
    code.write("class %s;", module.m_moduleOwnerClassName.c_str());

  code.write("struct %sAPIBase;", module.m_name.c_str());
  if (!hasModuleModifier(module.m_modifiers, mmGlobalAccess))
  {
    code.write("struct %sUpdatePackage;", module.m_name.c_str());
    code.write("struct %sFeedbackPackage;", module.m_name.c_str());
  }

  code.newLine();

  if (module.isRootModule())
    code.write("class %s : public ER::RootModule", module.m_name.c_str());
  else
    code.write("class %s : public ER::Module", module.m_name.c_str());

  code.openBrace();

  code.write("public:");
  code.newLine();

  // ctor/dtor pair
  if (module.isRootModule())
    code.write("%s();", module.m_name.c_str());
  else
    code.write("%s(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);", module.m_name.c_str());
  code.write("virtual ~%s();", module.m_name.c_str());

  if (module.m_privateAPI.size() > 0)
  {
    code.newLine();
    code.writeComment("private API functions declared in module definition");
    code.newLine();
    ModuleDef::FnDecls::const_iterator fIt = module.m_privateAPI.begin();
    ModuleDef::FnDecls::const_iterator fEnd = module.m_privateAPI.end();
    for (; fIt != fEnd; ++fIt)
    {
      // Function comment.
      if (!((*fIt).m_shortDesc.empty()))
      {
        // TOOD This is exactly same code as for public interface function comment processing.
        // Make a function.

        // Erase < from the comment if there is any.
        // Write-raw every comment line separately,
        // inserting tabs in between, trim '\n' from the end of last line.

        const std::string endOfLine("\n");
        const std::string fwrdSlashLessThan("/<");
        const std::string lessThan("<");
        std::string::size_type breaklocation = std::string::npos;
        std::string comment((*fIt).m_shortDesc.c_str());
        bool singleLineComment = true;
        const std::string::size_type commentSize = comment.size();
        std::string::size_type currentSize = 0;

        // Erase < from the comment.
        breaklocation = comment.find(fwrdSlashLessThan);
        if (breaklocation != std::string::npos)
        {
          comment.replace(breaklocation + 1, (lessThan).size(), "");
        }

        do
        {
          breaklocation = comment.find(endOfLine);
          if (breaklocation != std::string::npos)
          {
            std::string commentLine(comment.substr(0, breaklocation + endOfLine.size()));
            currentSize += commentLine.size();
            if (currentSize == commentSize) // Last comment line?
            {
              // Trim \n from the end of last comment line.
              std::string::size_type breaklocation = commentLine.rfind(endOfLine);
              if (breaklocation == (commentLine.size() - endOfLine.size()))
              {
                commentLine.replace(breaklocation, endOfLine.size(), "");
              }
            }
            code.writeTabs();
            code.writeRaw(commentLine.c_str());
            comment.erase(0, breaklocation + endOfLine.size());
            singleLineComment = false;
          }
          else if (singleLineComment)
          {
            code.writeTabs();
            code.writeRaw(comment.c_str());
          }
        }
        while (breaklocation != std::string::npos);

        code.newLine();
      }

      // Function code.
      code.write("%s", (*fIt).m_fn.c_str());
    }
    code.newLine();
  }

  code.newLine();

  if (module.isRootModule())
  {
    code.write("virtual void update(float timeStep) NM_OVERRIDE;");
    code.write("virtual void feedback(float timeStep) NM_OVERRIDE;");

    code.write("virtual void executeNetworkUpdate(float timeStep) NM_OVERRIDE;");
    code.write("virtual void executeNetworkFeedback(float timeStep) NM_OVERRIDE;");
  }
  else
  {
    // Create update and feedback decl's if not specified otherwise
    if ( hasModuleModifier(module.m_modifiers, mmNoUpdate))
    {
      code.writeComment("No update() api is required for this module.");
    }
    else
    {
      code.write("void update(float timeStep);");
    }
    if ( hasModuleModifier(module.m_modifiers, mmNoFeedback))
    {
      code.writeComment("No feedback() api is required for this module.");
    }
    else
    {
      code.write("void feedback(float timeStep);");
    }
  }
  code.newLine();
  code.write("virtual void clearAllData() NM_OVERRIDE;");
  code.write("virtual void entry() NM_OVERRIDE;");
  code.write("virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;");

  // Define only the required combiner interfaces
  if (module.varDeclBlockHasJunctionAssignments(msInputs))
  {
    code.write("void combineInputs() { m_moduleCon->combineInputs(this); }");
  }
  if (module.varDeclBlockHasJunctionAssignments(msOutputs))
  {
    code.write("void combineOutputs() { m_moduleCon->combineOutputs(this); }");
  }
  if (module.varDeclBlockHasJunctionAssignments(msFeedInputs))
  {
    code.write("void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }");
  }
  if (module.varDeclBlockHasJunctionAssignments(msFeedOutputs))
  {
    code.write("void combineFeedbackOutputs() { m_moduleCon->combineFeedbackOutputs(this); }");
  }
  
  code.write("virtual const char* getClassName() const  NM_OVERRIDE { return \"%s\"; }", module.m_name.c_str());

  code.newLine();
  code.write("virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;");
  code.write("virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;");

  if (module.isRootModule())
  {
    code.newLine();
    code.writeComment("from ER::RootModule");
    code.write("virtual void initLimbIndices() NM_OVERRIDE;");
    code.write("virtual ER::ModuleDataAllocator* getDataAllocator() NM_OVERRIDE { return &m_mdAllocator; }");
    code.newLine();
  }

  code.newLine();

  // data blocks
  code.writeComment("module data blocks");
  for (int i=msData; i<msNumVarDeclBlocks; i++)
  {
    if (module.varDeclBlockIsNotEmpty((ModuleSection)i))
    {
      code.write("%s%s* %s;", 
        module.m_name.c_str(), 
        keywords.getModuleSectionLabel((ModuleSection)i).c_str(),
        keywords.getModuleSectionName((ModuleSection)i).c_str());
    }
  }

  if (!module.m_private.empty())
  {
    DataFootprint fc;
    code.newLine();
    iterate_begin_const(ModuleDef::VarDecl, module.m_private, vd)
    {
      writeVarDecl(code, vd, fc);
      code.newLine();
    }
    iterate_end
    code.newLine();
  }

  code.newLine();
  code.writeComment("owner access");
  if (!module.m_moduleOwnerClassName.empty())
  {
    code.write("const %s* owner;", module.m_moduleOwnerClassName.c_str());
    code.write("virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }");
  }
  else
  {
    code.write("virtual const ER::Module* getOwner() const  NM_OVERRIDE { return 0; }");
  }
  code.newLine();

  // children block
  unsigned int totalModuleChildren = 0;
  code.writeComment("module children");
  iterate_begin_const(ModuleDef::Child, module.m_children, child)
  {
    // count up total children, including all those in arrays
    if (child.m_arraySize > 0)
      totalModuleChildren += child.m_arraySize;
    else
      totalModuleChildren ++;

    code.writeTabs();

    code.writeRaw("%s* %s", child.m_module.c_str(), child.m_name.c_str());

    if (child.m_arraySize > 0)
      code.writeRaw("[%i];\n", child.m_arraySize);
    else
      code.writeRaw(";\n", child.m_arraySize);
  }
  iterate_end

  code.newLine();
  code.writeComment("child module access");
  if (totalModuleChildren > 0)
  {
    code.write("ER::Module* m_childModules[%i];", totalModuleChildren);
    code.write("virtual ER::Module* getChild(int32_t index) const  NM_OVERRIDE { NMP_ASSERT(index >= 0 && index < %i && m_childModules[index] != 0); return m_childModules[index]; }", totalModuleChildren);
  }
  else
  {
    code.write("virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }");
  }
  code.write("virtual const char* getChildName(int32_t index) const NM_OVERRIDE;");
  code.write("virtual int32_t getNumChildren() const  NM_OVERRIDE { return %i; }", totalModuleChildren);

  code.newLine();
  code.writeComment("Module packaging");
  code.write("%sAPIBase* m_apiBase;", module.m_name.c_str());
  if (!hasModuleModifier(module.m_modifiers, mmGlobalAccess))
  {
    code.write("%sUpdatePackage* m_updatePackage;", module.m_name.c_str());
    code.write("%sFeedbackPackage* m_feedbackPackage;", module.m_name.c_str());
  }

  code.newLine();
  code.write("static NM_INLINE NMP::Memory::Format getMemoryRequirements()");
  code.openBrace();
  code.write("NMP::Memory::Format result(sizeof(%s), NMP_VECTOR_ALIGNMENT);", module.m_name.c_str());
  code.write("return result;");
  code.closeBrace();
  
  if (module.isRootModule())
  {
    code.newLine();
    code.write("%sDataAllocator   m_mdAllocator;", module.m_name.c_str());
  }

  code.closeBraceWithSemicolon();
  code.newLine();


  // class definition
  code.writeHorizontalSeparator();

  // create a ..Con class for each connection set
  iterate_begin_const_ptr(ModuleDef::ConnectionSet, module.m_connectionSets, set)
  {
    code.write("class %s_%sCon : public ER::ModuleCon", module.m_name.c_str(), set->m_name.c_str());
    code.openBrace();

    code.write("public:");
    code.newLine();

    code.write("virtual ~%s_%sCon();", module.m_name.c_str(), set->m_name.c_str());

    code.write("static NMP::Memory::Format getMemoryRequirements();");

    code.newLine();
    code.write("virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;");
    code.write("virtual void buildConnections(%s* module);", module.m_name.c_str());
    code.write("void relocate();");
    // Declare Module_Con combiners as required for the particular connection set
    for(int i = 1; i <= msOutputs; ++i )
    {
      ModuleSection blk = (ModuleSection)i;
      if (i == msUserInputs)
      {
        continue;
      }
      iterate_begin_const(ModuleDef::ConnectionSet::AssignJunctionTask, set->m_assignJunctionTask, ajt)
      {
        // this junction assignment is valid for the current block (inputs, outputs, etc)?
        if (ajt.m_targetVarDecl.m_varDeclPtr->m_ownerBlock == blk)
        {
          switch(blk)
          {
          case msFeedInputs:
            code.write("virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;");
            break;
          case msInputs:
            code.write("virtual void combineInputs(ER::Module* module) NM_OVERRIDE;");
            break;
          case msUserInputs:
            break;
          case msFeedOutputs:
            code.write("virtual void combineFeedbackOutputs(ER::Module* module) NM_OVERRIDE;");
            break;
          case msOutputs:
            code.write("virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;");
            break;
          default:
            break;
          }
          // Sufficient to have found just one junction assignment for this block
          break;
        }
      }
      iterate_end;
    }

    code.newLine();
    code.write("private:");
    code.newLine();

    code.startCommaSeparatedList();
    iterate_begin_const(ModuleDef::ConnectionSet::NewJunctionTask, set->m_newJunctionTasks, njt)
    {
      // if the junction is connecting arrays together, create an entry for
      // each element in the array
      if (njt.m_junction->m_expandToMultiple > 0)
      {
        for (int a=0; a<njt.m_junction->m_expandToMultiple; a++)
        {
          code.addToCommaSeparatedList("*junc_%s_%i", 
            njt.m_junction->m_name.c_str(), 
            a);
        }
      }
      else
      {
        code.addToCommaSeparatedList("*junc_%s", 
          njt.m_junction->m_name.c_str());
      }
    }
    iterate_end

    if (code.hasCommaSeparatedListEntries())
    {
      code.write("ER::Junction");
      code.emitCommaSeparatedList(2);
      code.writeRaw(";");
      code.newLines(2);
    }

    code.closeBraceWithSemicolon();
    code.newLine();
  }
  iterate_end


  code.writeHorizontalSeparator();

  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLine();

  code.writeHeaderBottom(hg);
  return true;
}

// -------------------------------------------------------------------------------------------------------------------
bool ModuleGen::writeInterface(const ModuleDef &module, CodeWriter &code)
{
  if (!hasModuleModifier(module.m_modifiers, mmGenerateInterface))
    return true;

  if (!hasModuleModifier(module.m_modifiers, mmGlobalAccess))
  {
    printf(" Modules cannot be marked as generating an interface unless also marked as global access\n");
    return false;
  }

  StringBuilder className(128);
  className.appendCharBuf(module.m_name.c_str());
  className.appendCharBuf("Wrapper");

  StringBuilder moduleType(128);
  moduleType.appendCharBuf(module.m_name.c_str());
  char firstLetter = moduleType.getChar(0);
  moduleType.setChar(0, (char) tolower(firstLetter));

  code.writeHorizontalSeparator();

  code.write("class %s : public BI<%s>", className.c_str(), module.m_name.c_str());
  code.openBrace();
  code.write("public:");
  code.write(
    "%s(const ER::Character* character) : BI<%s>(character, NetworkManifest::%s) {}",
    className.c_str(), module.m_name.c_str(), moduleType.c_str());
  code.newLine();

  // write input interface
  if (module.m_vardecl[msUserInputs].size() != 0)
  {
    code.writeComment("==================== User Inputs ====================");
    const ModuleDef::VarDecls &varDecls = module.m_vardecl[msUserInputs];

    StringBuilder typeString(64);

    ModuleDef::VarDecls::const_iterator it   = varDecls.begin();
    ModuleDef::VarDecls::const_iterator end  = varDecls.end();
    for (; it != end; ++it)
    {
      typeString.reset();
      const ModuleDef::VarDecl &vd = (*it);

      StringBuilder typeString(64);
      if (vd.m_knownType->m_aliasType)
      {
        if (!vd.m_knownType->m_aliasType->m_namespace.empty())
        {
          typeString.appendPString(vd.m_knownType->m_aliasType->m_namespace);
          typeString.appendCharBuf("::");
        }
        typeString.appendPString(vd.m_knownType->m_aliasType->m_name);
      }
      else
      {
        typeString.appendPString(vd.m_typename);
      }

      if (!vd.m_comment.empty())
      {
        // Erase < from the comment if any.
        // Write-raw every comment line separately,
        // inserting tabs in between, trim '\n' from the end of last line.

        std::string comment(vd.m_comment.c_str());
        std::string::size_type breaklocation = std::string::npos;

        // Erase < from the comment.
        const std::string fwrdSlashLessThan("/<");
        const std::string lessThan("<");
        breaklocation = comment.find(fwrdSlashLessThan);
        if (breaklocation != std::string::npos)
        {
          comment.replace(breaklocation + 1, (lessThan).size(), "");
        }

        const std::string endOfLine("\n");
        bool singleLineComment = true;
        const std::string::size_type commentSize = comment.size();
        std::string::size_type currentSize = 0;

        do
        {
          breaklocation = comment.find(endOfLine);
          if (breaklocation != std::string::npos)
          {
            std::string commentLine(comment.substr(0, breaklocation + endOfLine.size()));
            currentSize += commentLine.size();
            if (currentSize == commentSize) // Last comment line?
            {
              // Trim \n from the end of last comment line.
              std::string::size_type breaklocation = commentLine.rfind(endOfLine);
              if (breaklocation == (commentLine.size() - endOfLine.size()))
              {
                commentLine.replace(breaklocation, endOfLine.size(), "");
              }
            }
            code.writeTabs();
            code.writeRaw(commentLine.c_str());
            comment.erase(0, breaklocation + endOfLine.size());
            singleLineComment = false;
          }
          else if (singleLineComment)
          {
            code.writeTabs();
            code.writeRaw(comment.c_str());
          }
        }
        while (breaklocation != std::string::npos);
        code.newLine();
      }

      if (vd.m_arraySize > 0)
      {
        code.write("void set%sAt(unsigned int index, const %s& value, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->set%sAt(index, value, importance);}", 
          vd.m_varnameCapFront.c_str(), typeString.c_str(), vd.m_varnameCapFront.c_str());
        code.write("void set%s(unsigned int number, const %s* values, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->set%s(number, values, importance);}", 
          vd.m_varnameCapFront.c_str(), typeString.c_str(), vd.m_varnameCapFront.c_str());
      }
      else
      {
        code.write("void set%s(const %s& value, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->set%s(value, importance);}", 
          vd.m_varnameCapFront.c_str(), typeString.c_str(), vd.m_varnameCapFront.c_str());
      }
      code.newLine();
    }
  }

  // write feed output interface
  if (module.m_vardecl[msFeedOutputs].size() != 0)
  {
    code.writeComment("==================== Feed outputs ====================");
    const ModuleDef::VarDecls &varDecls = module.m_vardecl[msFeedOutputs];

    StringBuilder typeString(64);

    ModuleDef::VarDecls::const_iterator it   = varDecls.begin();
    ModuleDef::VarDecls::const_iterator end  = varDecls.end();
    for (; it != end; ++it)
    {
      typeString.reset();
      const ModuleDef::VarDecl &vd = (*it);

      StringBuilder typeString(64);
      if (vd.m_knownType->m_aliasType)
      {
        if (!vd.m_knownType->m_aliasType->m_namespace.empty())
        {
          typeString.appendPString(vd.m_knownType->m_aliasType->m_namespace);
          typeString.appendCharBuf("::");
        }
        typeString.appendPString(vd.m_knownType->m_aliasType->m_name);
      }
      else
      {
        typeString.appendPString(vd.m_typename);
      }

      if (!vd.m_comment.empty())
      {
        // Erase < from the comment if any.
        // Write-raw every comment line separately,
        // inserting tabs in between, trim '\n' from the end of last line.

        std::string comment(vd.m_comment.c_str());
        std::string::size_type breaklocation = std::string::npos;

        // Erase < from the comment.
        const std::string fwrdSlashLessThan("/<");
        const std::string lessThan("<");
        breaklocation = comment.find(fwrdSlashLessThan);
        if (breaklocation != std::string::npos)
        {
          comment.replace(breaklocation + 1, (lessThan).size(), "");
        }

        const std::string endOfLine("\n");
        bool singleLineComment = true;
        const std::string::size_type commentSize = comment.size();
        std::string::size_type currentSize = 0;

        do
        {
          breaklocation = comment.find(endOfLine);
          if (breaklocation != std::string::npos)
          {
            std::string commentLine(comment.substr(0, breaklocation + endOfLine.size()));
            currentSize += commentLine.size();
            if (currentSize == commentSize) // Last comment line?
            {
              // Trim \n from the end of last comment line.
              std::string::size_type breaklocation = commentLine.rfind(endOfLine);
              if (breaklocation == (commentLine.size() - endOfLine.size()))
              {
                commentLine.replace(breaklocation, endOfLine.size(), "");
              }
            }
            code.writeTabs();
            code.writeRaw(commentLine.c_str());
            comment.erase(0, breaklocation + endOfLine.size());
            singleLineComment = false;
          }
          else if (singleLineComment)
          {
            code.writeTabs();
            code.writeRaw(comment.c_str());
          }
        }
        while (breaklocation != std::string::npos);
        code.newLine();
      }

      if (vd.m_arraySize > 0)
      {
        code.write("const %s& get%s(unsigned int index) const {NMP_ASSERT(isAvailable()); return m_module->feedOut->get%s(index);}", 
          typeString.c_str(), vd.m_varnameCapFront.c_str(), vd.m_varnameCapFront.c_str());

        code.write("float get%sImportance(int index) const {NMP_ASSERT(isAvailable()); return m_module->feedOut->get%sImportance(index);}", 
          vd.m_varnameCapFront.c_str(), vd.m_varnameCapFront.c_str());
      }
      else
      {
        code.write("const %s& get%s() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->get%s();}", 
          typeString.c_str(), vd.m_varnameCapFront.c_str(), vd.m_varnameCapFront.c_str());

        code.write("float get%sImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->get%sImportance();}", 
          vd.m_varnameCapFront.c_str(), vd.m_varnameCapFront.c_str());
      }

      code.newLine();
    }
  }

  code.closeBraceWithSemicolon();
  code.newLine();

  return true;
}

// -------------------------------------------------------------------------------------------------------------------
void formatConnectionPath(
  StringBuilder& strOut, 
  ModuleDef::Connection::Path *path,
  unsigned int arrayIndex)
{
  // iterate the path, turning them into an actual pointer-access chain
  ModuleDef::Connection::Path::Components::const_iterator compIt = path->m_components.begin(), 
                                                          compEnd  = path->m_components.end(); 
  --compEnd; // skip last entry, requires special handling

  for (; compIt != compEnd; ++compIt)
  {
    const ModuleDef::Connection::Path::Component* comp = (*compIt);

    strOut.appendPString(comp->m_text);

    // add array access based on component type, if applicable
    comp->formatArrayAccess(strOut, true, arrayIndex);

    strOut.appendCharBuf("->", 2);
  }
}

/**
 *
 */
void formatConnectionSource(
  StringBuilder& strOut, 
  const ModuleDef::Junction::ConnectionSource &cs,
  bool arrayAccess, 
  unsigned int arrayIndex)
{
  StringBuilder pathToSource(64);

  strOut.reset();

  // This assert will check that we are not passing one junction into another.  It's a temporary way to disallow
  //  junctions from feeding into other junctions.
  NMP_ASSERT(!cs.m_junction);

  // source is a junction, just supply name of junc instance (as we can only refer to junctions local to the module)
  if (cs.m_junction)
  {
    strOut.appendf("junc_%s", cs.m_junction->m_name.c_str());
    if (arrayAccess)
      strOut.appendf("_%i", arrayIndex);
  }
  else
  {
    formatConnectionPath(pathToSource, cs.m_sourcePath, arrayIndex);

    // sort out potential array element access .. otherwise this buffer is left empty
    StringBuilder arrayElement(32);
    {
      // the target variable has a supplied array access component
      if (cs.m_varDecl.m_arrayElementAccess)
      {
        bool addedArraySpec = false;
        ModuleDef::Connection::Path::Component* comp = (ModuleDef::Connection::Path::Component*)cs.m_varDecl.m_arrayElementAccess;
        addedArraySpec = comp->formatArrayAccess(arrayElement, false, arrayIndex);
        assert(addedArraySpec);
      }
      // or if we are just acessing arrays, and the variable IS an array, just pop the current iteration index in
      else if (arrayAccess && cs.m_varDecl.m_varDeclPtr->m_arraySize > 0)
      {
        arrayElement.appendf("%i", arrayIndex);
      }
    }

    // emit the variable & importance addresses
    strOut.appendf("(char*)&(module->%sget%s(%s)), (float*)&(module->%sget%sImportanceRef(%s))", 
          pathToSource.c_str(), 
          cs.m_varDecl.m_varDeclPtr->m_varnameCapFront.c_str(),
          arrayElement.c_str(),
          pathToSource.c_str(),
          cs.m_varDecl.m_varDeclPtr->m_varnameCapFront.c_str(),
          arrayElement.c_str());
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// cache class to promote re-use of string buffers while writing all the junction assignments
struct JunctionAssignmentWriter
{
  JunctionAssignmentWriter(CodeWriter& cw, const KeywordsManager& kw) :
    code(cw),
    keywords(kw),
    varDesc(32),
    assignmentImp(32)
    {}

  void write(
    const ModuleDef::VarDecl& vd,
    const ModuleDef::Junction& junc,
    const char* arrayIndex,
    const char* juncArray);

private:
  CodeWriter               &code;
  const KeywordsManager    &keywords;
  StringBuilder             varDesc,
                            assignmentImp;

  JunctionAssignmentWriter();
  JunctionAssignmentWriter(const JunctionAssignmentWriter& rhs);
  JunctionAssignmentWriter operator = (const JunctionAssignmentWriter& rhs);
};

// ---------------------------------------------------------------------------------------------------------------------
void __stdcall upperCaseFirstLetter(char *d)
{
  if (d != 0)
    d[0] = (char)toupper(d[0]);
}

// ---------------------------------------------------------------------------------------------------------------------
void JunctionAssignmentWriter::write(
  const ModuleDef::VarDecl& vd,
  const ModuleDef::Junction& junc,
  const char* arrayIndex,
  const char* juncArray)
{
  varDesc.reset();

  assignmentImp.reset();
  assignmentImp.appendf("%s.m_%sImportance%s",
    keywords.getModuleSectionName(vd.m_ownerBlock).c_str(),
    vd.m_varname.c_str(),
    arrayIndex);

  varDesc.appendPString(keywords.getJunctionModeLabel(junc.m_mode));
  varDesc.invoke(upperCaseFirstLetter);

  if (vd.m_ownerBlock == msOutputs || vd.m_ownerBlock == msFeedOutputs)
  {
    code.write("NMP_ASSERT(%s == 0.0f);", assignmentImp.c_str());
  }

  code.write("%s = junc_%s%s->combine%s(&%s.%s%s);", 
    assignmentImp.c_str(),

    junc.m_name.c_str(),
    juncArray,

    varDesc.getBufferPtr(),

    keywords.getModuleSectionName(vd.m_ownerBlock).c_str(),
    vd.m_varname.c_str(),
    arrayIndex);

  // write out a postCombine() function for those data blocks that get combined
  if (junc.m_mode != jmDirectInput &&
      junc.m_mode != jmWinnerTakesAll)
  {
    varDesc.reset();
    varDesc.appendPString(keywords.getModuleSectionName(vd.m_ownerBlock));
    varDesc.appendChar('.');

    writeVarDeclPostCombine(code, vd, varDesc.c_str());
  }
}

// ---------------------------------------------------------------------------------------------------------------------
void writeFlattenedHierarchyEntry(CodeWriter& code, const ModuleDef *def, const char* argPath, const char* argAccess)
{
  StringBuilder sbPath(16), sbAccess(16);
  if (!def->m_children.empty())
  {
    unsigned chIndex = 0;
    iterate_begin_const(ModuleDef::Child, def->m_children, chDef)
    {
      if (chDef.m_arraySize > 0)
      {
        for (unsigned int i=0; i<chDef.m_arraySize; i++)
        {
          sbPath.reset();
          sbPath.appendf("%s%s_%i", argPath, chDef.m_name.c_str(), i);

          sbAccess.reset();
          if (argAccess[0] == '\0')
            sbAccess.appendf("%s[%i]", chDef.m_name.c_str(), i);
          else
            sbAccess.appendf(argAccess, chIndex++);

          code.write("m_allModules[NetworkManifest::%s] = %s;", sbPath.c_str(), sbAccess.c_str());
          code.write("m_allModules[NetworkManifest::%s]->assignManifestIndex(NetworkManifest::%s);", sbPath.c_str(), sbPath.c_str());

          sbPath.appendChar('_');
          sbAccess.appendCharBuf("->getChild(%i)");

          writeFlattenedHierarchyEntry(code, chDef.m_modulePtr, sbPath.c_str(), sbAccess.c_str());
        }
      }
      else
      {
        sbPath.reset();
        sbPath.appendf("%s%s", argPath, chDef.m_name.c_str());

        sbAccess.reset();
        if (argAccess[0] == '\0')
          sbAccess.appendPString(chDef.m_name);
        else
          sbAccess.appendf(argAccess, chIndex++);

        code.write("m_allModules[NetworkManifest::%s] = %s;", sbPath.c_str(), sbAccess.c_str());
        code.write("m_allModules[NetworkManifest::%s]->assignManifestIndex(NetworkManifest::%s);", sbPath.c_str(), sbPath.c_str());

        sbPath.appendChar('_');
        sbAccess.appendCharBuf("->getChild(%i)");

        writeFlattenedHierarchyEntry(code, chDef.m_modulePtr, sbPath.c_str(), sbAccess.c_str());
      }

    }
    iterate_end
  }
}

// -------------------------------------------------------------------------------------------------------------------
bool ModuleGen::writeCode(const ModuleDef &module)
{
  StringBuilder sb(32);
  code.writeNMCopyrightHeader();


  code.startUniqueHeaders();
  unsigned int mdfDepGroup = code.addUniqueHeaderGroup("module def dependencies");
  unsigned int mdfChildGroup = code.addUniqueHeaderGroup("module children dependencies");
  
  // add includes for module, module owner and packaging
  if (!module.m_moduleOwner.empty())
    code.addUniqueHeader(mdfDepGroup, "\"%s.h\"", module.m_moduleOwner.c_str());
  code.addUniqueHeader(mdfDepGroup, "\"%s.h\"", module.m_name.c_str());
  code.addUniqueHeader(mdfDepGroup, "\"%sPackaging.h\"", module.m_name.c_str());

  // look through all connections, adding modules that are references (as owners, etc)
  iterate_begin_const_ptr(ModuleDef::ConnectionSet, module.m_connectionSets, set)
  {
    for (ModuleDependencies::const_iterator mIt = set->m_moduleReferences.const_begin(), 
                                            mEnd = set->m_moduleReferences.const_end(); 
                                            mIt != mEnd; 
                                            ++mIt)
    {
      code.addUniqueHeader(mdfDepGroup, "\"%s.h\"", (*mIt)->m_def->m_name.c_str());
    }
  }
  iterate_end

  // add parent hierarchy
  ModuleDef *parent = module.m_moduleOwnerPtr;
  while (parent)
  {
    code.addUniqueHeader(mdfDepGroup, "\"%s.h\"", parent->m_name.c_str());
    code.addUniqueHeader(mdfDepGroup, "\"%sPackaging.h\"", parent->m_name.c_str());
    parent = parent->m_moduleOwnerPtr;
  }

  // add includes to the code file for each of the modules used as descendents by the module
  ModuleDef::ConstChildPtrs descendents;
  populateDescendentList(&module, descendents);
  
  if (!descendents.empty())
  {
    ModuleDef::ConstChildPtrs::iterator descendent_it   = descendents.begin();
    for (; descendent_it != descendents.end(); ++descendent_it)
    {
      const ModuleDef::Child* descendent = (*descendent_it);
      code.addUniqueHeader(mdfChildGroup, "\"%s.h\"", descendent->m_module.c_str());
    }
  }

  calculateDependentHeadersAndFwdDecls(module.m_typeDependencies, false, code, 0, 0);

  unsigned int mdfMiscGroup = code.addUniqueHeaderGroup("misc");
  code.addUniqueHeader(mdfMiscGroup, "\"euphoria/erEuphoriaLogger.h\"");
  code.addUniqueHeader(mdfMiscGroup, "\"euphoria/erDebugDraw.h\"");
  code.addUniqueHeader(mdfMiscGroup, "\"physics/mrPhysicsSerialisationBuffer.h\"");

  // root module
  if (module.isRootModule())
  {
    code.addUniqueHeader(mdfMiscGroup, "\"NetworkDescriptor.h\"");
    code.addUniqueHeader(mdfMiscGroup, "\"NetworkDataMemory.h\"");
  }

  // write out header list
  code.emitUniqueHeaders();

  code.newLine();
  code.newLines(2);

  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");


  // -----------------------------------------------------------------------------------------------------------------
  // the root module controls the memory management for the rest of the
  // network - write out the data allocator functionality if this is the root
  //
  if (module.isRootModule())
  {
    writeDataAllocatorFns(module);
  }

  // -----------------------------------------------------------------------------------------------------------------
  // bulk of the module class code
  //
  writeConstructor(module);
  writeDestructor(module);
  writeCreateFn(module);
  writeClearAllDataFn(module);
  writeLoadStoreStateFns(module);
  writeGetChildNameFn(module);
  
  // stubs that call behaviour user-code
  if (!hasModuleModifier(module.m_modifiers, mmGlobalAccess))
  {
    writeUpdateAndFeedbackFns(module);
  }

  code.newLine();

  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLines(2);

  return true;
}


// -------------------------------------------------------------------------------------------------------------------
bool ModuleGen::writePackagingCode(const ModuleDef &module)
{
  StringBuilder sb(32);
  code.writeNMCopyrightHeader();

  code.startUniqueHeaders();
  unsigned int mdfDepGroup = code.addUniqueHeaderGroup("module def dependencies");

  // add includes for module, module owner and packaging
  if (!module.m_moduleOwner.empty())
    code.addUniqueHeader(mdfDepGroup, "\"%s.h\"", module.m_moduleOwner.c_str());
  code.addUniqueHeader(mdfDepGroup, "\"%s.h\"", module.m_name.c_str());
  code.addUniqueHeader(mdfDepGroup, "\"%sPackaging.h\"", module.m_name.c_str());

  // look through all connections, adding modules that are references (as owners, etc)
  iterate_begin_const_ptr(ModuleDef::ConnectionSet, module.m_connectionSets, set)
  {
    for (ModuleDependencies::const_iterator mIt = set->m_moduleReferences.const_begin(), 
                                            mEnd = set->m_moduleReferences.const_end(); 
                                            mIt != mEnd; 
                                            ++mIt)
    {
      code.addUniqueHeader(mdfDepGroup, "\"%s.h\"", (*mIt)->m_def->m_name.c_str());
    }
  }
  iterate_end

  // add parent hierarchy
  ModuleDef *parent = module.m_moduleOwnerPtr;
  
  while (parent)
  {
    code.addUniqueHeader(mdfDepGroup, "\"%s.h\"", parent->m_name.c_str());
    code.addUniqueHeader(mdfDepGroup, "\"%sPackaging.h\"", parent->m_name.c_str());
    parent = parent->m_moduleOwnerPtr;
  }

  // write out header list
  code.emitUniqueHeaders();

  code.newLine();
  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");

  // -----------------------------------------------------------------------------------------------------------------
  StringBuilder connectionClassName(16);
  iterate_begin_const_ptr(ModuleDef::ConnectionSet, module.m_connectionSets, set)
  {
    sb.reset();

    // ---------------------------------------------------------------------------------------------
    // module connection class constructor (basically just call buildConnections)
    // ---------------------------------------------------------------------------------------------

    connectionClassName.reset();
    connectionClassName.appendf("%s_%sCon", module.m_name.c_str(), set->m_name.c_str());

    code.writeHorizontalSeparator();
    code.write("void %s::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))", connectionClassName.c_str());
    code.openBrace();
    code.write("buildConnections((%s*)module);", module.m_name.c_str());
    code.closeBrace();
    code.newLine();


    // ---------------------------------------------------------------------------------------------
    // module connection class destructor (delete all junction instances)
    // ---------------------------------------------------------------------------------------------

    code.writeHorizontalSeparator();
    code.write("%s::~%s()", connectionClassName.c_str(), connectionClassName.c_str());
    code.openBrace();

    // walk backwards through the 'new junction' tasks, destroy each one
    iterate_begin_const_reverse(ModuleDef::ConnectionSet::NewJunctionTask, set->m_newJunctionTasks, njt)
    {
      if (njt.m_junction->m_expandToMultiple > 0)
      {
        for (unsigned int a=njt.m_junction->m_expandToMultiple; a>0; a--)
        {
          code.write("junc_%s_%i->destroy();", njt.m_junction->m_name.c_str(), a - 1);
        }
      }
      else
      {
        code.write("junc_%s->destroy();", njt.m_junction->m_name.c_str());
      }
    }
    iterate_end

      code.closeBrace();
    code.newLine();

    // ---------------------------------------------------------------------------------------------
    // module connection class getMemoryRequirements
    // ---------------------------------------------------------------------------------------------
    code.writeHorizontalSeparator();
    code.write("NMP::Memory::Format %s::getMemoryRequirements()", 
      connectionClassName.getBufferPtr());
    code.openBrace();
    code.write("NMP::Memory::Format result(sizeof(%s), 16);", connectionClassName.getBufferPtr());
    code.newLine();

    // Allocate space for the junctions themselves.
    iterate_begin_const(ModuleDef::ConnectionSet::NewJunctionTask, set->m_newJunctionTasks, njt)
    {
      // if the junction is connecting arrays together, allocate space for
      // each element in the array
      if (njt.m_junction->m_expandToMultiple > 0)
      {
        for (int a = 0; a < njt.m_junction->m_expandToMultiple; a++)
        {
          code.write("result += ER::Junction::getMemoryRequirements(%d);", 
            (unsigned int)njt.m_junction->m_connectionSources.size());
        }
      }
      else
      {
        code.write("result += ER::Junction::getMemoryRequirements(%d);", 
          (unsigned int)njt.m_junction->m_connectionSources.size());
      }
    }
    iterate_end

    code.write("return result;");
    code.closeBrace();


    // ---------------------------------------------------------------------------------------------
    // buildConnections - create junctions and wire up Edges
    // ---------------------------------------------------------------------------------------------

    code.writeHorizontalSeparator();
    code.write("void %s::buildConnections(%s* module)", 
      connectionClassName.getBufferPtr(),
      module.m_name.c_str());
    code.openBrace();

    code.writeComment("Reconstruct the previously-allocated resource to fill in the junctions");
    code.write("NMP::Memory::Format format = getMemoryRequirements();");
    code.write("NMP::Memory::Resource resource = {this, format};");
    code.write("NMP::Memory::Format classFormat(sizeof(%s), 16);", connectionClassName.getBufferPtr());
    code.write("resource.increment(classFormat);");
    code.newLine();
    code.write("ER::Junction* activeJnc = 0;");
    code.newLine();

    code.writeComment("build and connect up all junctions");

    // create code for each 'new junction' task, creating a ER::Junction and wiring up the Edge data inside
    unsigned int edgeCost = 0, juncCost = 0, edgeIndex = 0;
    StringBuilder juncEdgeString(64);
    iterate_begin_const(ModuleDef::ConnectionSet::NewJunctionTask, set->m_newJunctionTasks, njt)
    {
      // if the junction is connecting arrays together, create an entry for
      // each element in the array
      if (njt.m_junction->m_expandToMultiple > 0)
      {
        for (int a=0; a<njt.m_junction->m_expandToMultiple; a++)
        {
          code.write("junc_%s_%i = activeJnc = ER::Junction::init(resource, %d, ER::Junction::%s);", 
            njt.m_junction->m_name.c_str(), 
            a,
            (unsigned int)njt.m_junction->m_connectionSources.size(),
            keywords.getJunctionModeLabel(njt.m_junction->m_mode).c_str());

          edgeIndex = 0;
          iterate_begin_const(ModuleDef::Junction::ConnectionSource, njt.m_junction->m_connectionSources, jcs)
          {
            formatConnectionSource(juncEdgeString, jcs, true, a);
            code.write("activeJnc->getEdges()[%d].reinit( %s );", edgeIndex++, juncEdgeString.c_str());
          }
          iterate_end

            juncCost ++;
        }
      }
      else
      {
        code.write("junc_%s = activeJnc = ER::Junction::init(resource, %d, ER::Junction::%s);", 
          njt.m_junction->m_name.c_str(),
          (unsigned int)njt.m_junction->m_connectionSources.size(),
          keywords.getJunctionModeLabel(njt.m_junction->m_mode).c_str());

        edgeIndex = 0;
        iterate_begin_const(ModuleDef::Junction::ConnectionSource, njt.m_junction->m_connectionSources, jcs)
        {
          formatConnectionSource(juncEdgeString, jcs, false, 0);
          code.write("activeJnc->getEdges()[%d].reinit( %s );", edgeIndex++, juncEdgeString.c_str());
        }
        iterate_end

          juncCost ++;
      }

      edgeCost += (unsigned int)njt.m_junction->m_connectionSources.size();
      code.newLine();
    }
    iterate_end

    code.closeBrace();
    code.newLine();



    // ---------------------------------------------------------------------------------------------
    // relocate - restore pointers to junctions and wire up Edges
    // ---------------------------------------------------------------------------------------------

    code.writeHorizontalSeparator();
    code.write("void %s::relocate()", connectionClassName.getBufferPtr());
    code.openBrace();

    code.writeComment("Reconstruct the previously-allocated resource to fill in the junctions");
    code.write("NMP::Memory::Format format = getMemoryRequirements();");
    code.write("NMP::Memory::Resource resource = {this, format};");
    code.write("NMP::Memory::Format classFormat(sizeof(%s), 16);", connectionClassName.getBufferPtr());
    code.write("resource.increment(classFormat);");
    code.newLine();

    code.writeComment("Reconnect all junctions");

    // create code for each 'new junction' task, creating a ER::Junction and wiring up the Edge data inside
    edgeIndex = 0;
    iterate_begin_const(ModuleDef::ConnectionSet::NewJunctionTask, set->m_newJunctionTasks, njt)
    {
      // if the junction is connecting arrays together, create an entry for
      // each element in the array
      if (njt.m_junction->m_expandToMultiple > 0)
      {
        for (int a=0; a<njt.m_junction->m_expandToMultiple; a++)
        {
          code.write("junc_%s_%i = ER::Junction::relocate(resource);", 
            njt.m_junction->m_name.c_str(), a);

          edgeIndex = 0;
          iterate_begin_const(ModuleDef::Junction::ConnectionSource, njt.m_junction->m_connectionSources, jcs)
          {
            formatConnectionSource(juncEdgeString, jcs, true, a);
            code.write("// junc_%s_%i->getEdges()[%d].reinit( %s );", 
              njt.m_junction->m_name.c_str(), a, edgeIndex++, juncEdgeString.c_str());
          }
          iterate_end
        }
      }
      else
      {
        code.write("junc_%s = ER::Junction::relocate(resource);", 
          njt.m_junction->m_name.c_str());

        edgeIndex = 0;
        iterate_begin_const(ModuleDef::Junction::ConnectionSource, njt.m_junction->m_connectionSources, jcs)
        {
          formatConnectionSource(juncEdgeString, jcs, false, 0);
          code.write("// junc_%s->getEdges()[%d].reinit( %s );",
            njt.m_junction->m_name.c_str(), edgeIndex++, juncEdgeString.c_str());
        }
        iterate_end
      }

      code.newLine();
    }
    iterate_end

    code.closeBrace();
    code.newLine();


    // ---------------------------------------------------------------------------------------------
    // combine functions
    // ---------------------------------------------------------------------------------------------

    StringBuilder arrayAccess(16), juncArray(16);
    JunctionAssignmentWriter jnAssignmentWriter(code, keywords);

    for (int k=msFeedInputs; k<=msOutputs; k++)
    {
      if (k == msUserInputs)
        continue;
      ModuleSection blk = (ModuleSection)k;
      
      // empty block - combine function not required
      if (module.varDeclBlockIsEmpty(blk) || 
        set->m_assignJunctionTask.empty())
      {
        continue;
      }

      bool emptyCombineFunction = true;
      iterate_begin_const(ModuleDef::ConnectionSet::AssignJunctionTask, set->m_assignJunctionTask, ajt)
      {
        // this junction assignment is valid for the current block (inputs, outputs, etc)?
        if (ajt.m_targetVarDecl.m_varDeclPtr->m_ownerBlock == blk)
        {
          // write the opening gambit for the combine function
          if (emptyCombineFunction)
          {
            code.writeHorizontalSeparator();
            code.writeComment("Combiner api.");
            code.write("void %s::combine%s(ER::Module* modulePtr)", 
              connectionClassName.getBufferPtr(), 
              keywords.getModuleSectionLabel(blk).c_str());
            code.openBrace();

            code.write("%s* module = (%s*)modulePtr;", 
              module.m_name.c_str(), 
              module.m_name.c_str());

            code.write("%s%s& %s = *module->%s;", 
              module.m_name.c_str(), 
              keywords.getModuleSectionLabel(blk).c_str(),
              keywords.getModuleSectionName(blk).c_str(),
              keywords.getModuleSectionName(blk).c_str());

            code.newLine();
            code.writeComment("Junction assignments.");

            emptyCombineFunction = false;
          }

          // Write junction assignments 3 types
          //

          const ModuleDef::VarDecl& vd = *ajt.m_targetVarDecl.m_varDeclPtr;
          
          // 1 Expand "array to array"
          //
          if (ajt.m_junction->m_expandToMultiple > 0)
          {
            // generate the junction assignments for array-to-array assignments;
            // this involves making a junction for each item in the array
            for (int a=0; a<ajt.m_junction->m_expandToMultiple; a++)
            {
              arrayAccess.reset();
              arrayAccess.appendf("[%i]", a);

              juncArray.reset();
              juncArray.appendf("_%i", a);

              jnAssignmentWriter.write(vd, *ajt.m_junction, arrayAccess.c_str(), juncArray.c_str());
            }
          }
          else
          {
            // 2 "to array element"
            //
            // are we doing junction assignment into a specific element of an array?
            if (ajt.m_targetVarDecl.m_arrayElementAccess)
            {
              ModuleDef::Connection::Path::Component* ajtAEA = static_cast<ModuleDef::Connection::Path::Component*>(ajt.m_targetVarDecl.m_arrayElementAccess);

              // by-link access means array index is unknown at code-gen time, so add an assert to catch out-of-bounds access
              if (ajtAEA->isType(ModuleDef::Connection::Path::Component::cArrayAccessByLink))
              {
                arrayAccess.reset();
                ajtAEA->formatArrayAccess(arrayAccess, false);
                code.write("NMP_ASSERT(%s < %i);", arrayAccess.c_str(), ajt.m_targetVarDecl.m_varDeclPtr->m_arraySize);
              }

              arrayAccess.reset();
              ajtAEA->formatArrayAccess(arrayAccess, true);
              jnAssignmentWriter.write(vd, *ajt.m_junction, arrayAccess.c_str(), "");
            }
            else
            {
              // 3 "simple junction to junction"
              //
              // plain ol' junctions
              jnAssignmentWriter.write(vd, *ajt.m_junction, "", "");
            }
          }
        }
      }
      iterate_end

      // Combiner function def was generated, close it.
      if (!emptyCombineFunction)
      {
        code.newLine();
        code.write("#if NM_CALL_VALIDATORS");
        code.write("%s.validate();", keywords.getModuleSectionName(blk).c_str());
        code.write("#endif // NM_CALL_VALIDATORS");
        code.closeBrace();
        code.newLine();
      }
    }
  }
  iterate_end
  code.newLine();

  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLines(2);


  return true;
}

// -------------------------------------------------------------------------------------------------------------------
void ModuleGen::writeConstructor(const ModuleDef& module)
{
  code.writeHorizontalSeparator();

  if (module.isRootModule())
    code.write("%s::%s() : ER::RootModule()", module.m_name.c_str(), module.m_name.c_str());
  else
    code.write("%s::%s(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)", module.m_name.c_str(), module.m_name.c_str());

  code.openBrace();

  if (module.isRootModule())
  {
    code.write("m_mdAllocator.init();");
    code.write("ER::ModuleDataAllocator* mdAllocator = &m_mdAllocator;");
    code.newLine();

    if (module.m_connectionSets.size() != 0)
    {
      code.write("m_moduleCon = (%s_Con*)NMPMemoryAllocateFromFormat(%s_Con::getMemoryRequirements()).ptr;", module.m_name.c_str(), module.m_name.c_str());
      code.write("new(m_moduleCon) %s_Con(); // set up vtable", module.m_name.c_str());
      code.newLine();
    }
  }

  for (int i=msData; i<msNumVarDeclBlocks; i++)
  {
    ModuleSection msi = (ModuleSection)i;

    if (module.varDeclBlockIsNotEmpty(msi))
    {
      code.write("%s = (%s%s*)mdAllocator->alloc(ER::ModuleDataAllocator::%s, sizeof(%s%s), __alignof(%s%s));", 
        keywords.getModuleSectionName(msi).c_str(),
        module.m_name.c_str(), 
        keywords.getModuleSectionLabel(msi).c_str(),
        keywords.getModuleSectionLabel(msi).c_str(),
        module.m_name.c_str(), 
        keywords.getModuleSectionLabel(msi).c_str(),
        module.m_name.c_str(), 
        keywords.getModuleSectionLabel(msi).c_str());
    }
  }
  code.newLine();

  if (module.m_children.size() != 0)
  {
    int index = 0;
    iterate_begin_const(ModuleDef::Child, module.m_children, child)
    {
      bool hasConnectionSet = false;
      sb.reset();
      if (child.m_modulePtr->m_connectionSets.size() == 0 && child.m_sku.empty())
      {
        sb.appendCharBuf(child.m_module.c_str());
      }
      else
      {
        sb.appendf("%s_%s", child.m_module.c_str(), child.m_sku.c_str());
        hasConnectionSet = true;
      }

      if (child.m_arraySize > 0)
      {
        for (unsigned int i=0; i<child.m_arraySize; i++)
        {
          if(hasConnectionSet)
          {
            code.write("%sCon* con%i = (%sCon*)NMPMemoryAllocateFromFormat(%sCon::getMemoryRequirements()).ptr;", sb.c_str(), index, sb.c_str(), sb.c_str());
            code.write("new(con%i) %sCon(); // set up vtable", index, sb.c_str());
          }

          code.write("m_childModules[%i] = %s[%i] = (%s*)NMPMemoryAllocateFromFormat(%s::getMemoryRequirements()).ptr;", 
            index++,
            child.m_name.c_str(), 
            i,
            child.m_module.c_str(),
            child.m_module.c_str());

          if(hasConnectionSet)
            code.write("new (%s[%i]) %s(mdAllocator, con%i);", child.m_name.c_str(), i, child.m_module.c_str(), index-1);
          else
            code.write("new (%s[%i]) %s(mdAllocator, NULL);", child.m_name.c_str(), i, child.m_module.c_str());

        }
      }
      else
      {
        if(hasConnectionSet)
        {
          code.write("%sCon* con%i = (%sCon*)NMPMemoryAllocateFromFormat(%sCon::getMemoryRequirements()).ptr;", sb.c_str(), index, sb.c_str(), sb.c_str());
          code.write("new(con%i) %sCon(); // set up vtable", index, sb.c_str());
        }

        code.write("m_childModules[%i] = %s = (%s*)NMPMemoryAllocateFromFormat(%s::getMemoryRequirements()).ptr;", 
          index++,
          child.m_name.c_str(), 
          child.m_module.c_str(),
          child.m_module.c_str());
        if(hasConnectionSet)
          code.write("new (%s) %s(mdAllocator, con%i);", child.m_name.c_str(), child.m_module.c_str(), index-1);
        else
          code.write("new (%s) %s(mdAllocator, NULL);", child.m_name.c_str(), child.m_module.c_str());
      }

       code.newLine();
    }
    iterate_end
  }

  // flattened access to entire network
  if (module.isRootModule())
  {
    code.newLine();
    code.write("m_numModules = NetworkManifest::NumModules;");
    code.write("m_allModules = (ER::Module**)NMPMemoryAlloc(sizeof(ER::Module*) * m_numModules);");
    code.write("m_allModules[NetworkManifest::rootModule] = this; ");
    code.write("m_allModules[NetworkManifest::rootModule]->assignManifestIndex(NetworkManifest::rootModule);");
    writeFlattenedHierarchyEntry(code, &module, "", "");

    // write the enable-state array initialization loop
    code.newLine();
    code.write("m_moduleEnableStates = (ER::RootModule::ModuleEnableState*)NMPMemoryAlloc(sizeof(ER::RootModule::ModuleEnableState) * m_numModules);");
    code.write("for (unsigned int cm = 0; cm < m_numModules; cm ++)");
    code.openBrace();
    code.write("m_moduleEnableStates[cm].m_enabled = 0;");
    code.write("m_moduleEnableStates[cm].m_enabledByOwner = 1;");
    code.closeBrace();
    code.newLine();
  }

  code.write("m_apiBase = (%sAPIBase*)NMPMemoryAllocAligned(sizeof(%sAPIBase), 16);", module.m_name.c_str(), module.m_name.c_str());

  if(!hasModuleModifier(module.m_modifiers, mmGlobalAccess))
  {
    code.write("m_updatePackage = (%sUpdatePackage*)NMPMemoryAllocAligned(sizeof(%sUpdatePackage), 16);", module.m_name.c_str(), module.m_name.c_str());
    code.write("m_feedbackPackage = (%sFeedbackPackage*)NMPMemoryAllocAligned(sizeof(%sFeedbackPackage), 16);", module.m_name.c_str(), module.m_name.c_str());
  }

  code.closeBrace();
  code.newLine();
}

// -------------------------------------------------------------------------------------------------------------------
void ModuleGen::writeDestructor(const ModuleDef& module)
{
  code.writeHorizontalSeparator();

  code.write("%s::~%s()", module.m_name.c_str(), module.m_name.c_str());
  code.openBrace();
  iterate_begin_const_reverse(ModuleDef::Child, module.m_children, child)
  {
    if (child.m_arraySize > 0)
    {
      for (unsigned int i=0; i<child.m_arraySize; i++)
      {
        // Free the module connections
        code.write("ER::ModuleCon* %s%iConnections = %s[%i]->getConnections();", child.m_name.c_str(), i, child.m_name.c_str(), i);
        code.write("%s%iConnections->~ModuleCon();", child.m_name.c_str(), i);
        code.write("NMP::Memory::memFree(%s%iConnections);", child.m_name.c_str(), i);

        // Free the module itself
        code.write("%s[%i]->~%s();", child.m_name.c_str(), i, child.m_module.c_str());
        code.write("NMP::Memory::memFree(%s[%i]);", child.m_name.c_str(), i);
        code.newLine();
      }
    }
    else
    {
      // Free the module connections
      code.write("ER::ModuleCon* %sConnections = %s->getConnections();", child.m_name.c_str(), child.m_name.c_str());
      code.write("if (%sConnections)", child.m_name.c_str());
      code.openBrace();
      code.write("%sConnections->~ModuleCon();", child.m_name.c_str());
      code.write("NMP::Memory::memFree(%sConnections);", child.m_name.c_str());
      code.closeBrace();

      // Free the module itself
      code.write("%s->~%s();", child.m_name.c_str(), child.m_module.c_str());
      code.write("NMP::Memory::memFree(%s);", child.m_name.c_str());
    }

    code.newLine();
  }
  iterate_end

  if (!module.m_moduleOwnerClassName.empty())
    code.write("owner = 0;");
  if (module.m_children.size() != 0)
    code.write("memset(m_childModules, 0, sizeof(ER::Module*) * getNumChildren());");

  code.newLine();
  code.write("NMP::Memory::memFree(m_apiBase);");

  if(!hasModuleModifier(module.m_modifiers, mmGlobalAccess))
  {
    code.write("NMP::Memory::memFree(m_updatePackage);");
    code.write("NMP::Memory::memFree(m_feedbackPackage);");
  }

  if (module.isRootModule())
  {
    code.write("NMP::Memory::memFree(m_allModules);");
    code.write("m_allModules = 0;");
    code.write("m_numModules = 0;");

    code.write("NMP::Memory::memFree(m_moduleEnableStates);");
    code.write("m_moduleEnableStates = 0;");

    code.write("m_mdAllocator.term();");
  }

  code.closeBrace();
  code.newLine();
}

// -------------------------------------------------------------------------------------------------------------------
void ModuleGen::writeCreateFn(const ModuleDef& module)
{
  code.writeHorizontalSeparator();
  code.write("void %s::create(ER::Module* parent, int childIndex)", module.m_name.c_str());
  code.openBrace();
  code.write("ER::Module::create(parent, childIndex);");

  if (!module.m_moduleOwnerClassName.empty())
  {
    code.write("owner = (%s*)parent;", module.m_moduleOwnerClassName.c_str());
    code.write("if (getConnections())");
    code.write("  getConnections()->create(this, (ER::Module*)owner);");
  }
  else
  {
    code.write("if (getConnections())");
    code.write("  getConnections()->create(this, 0);");
  }

  code.newLine();

  sb.reset();
  code.startCommaSeparatedList();
  if (module.varDeclBlockIsNotEmpty(msData))
  {
    code.addToCommaSeparatedList("data");
    sb.appendf(", data");
  }
  if (module.varDeclBlockIsNotEmpty(msInputs))
  {
    code.addToCommaSeparatedList("in");
    sb.appendf(", in");
  }
  if (module.varDeclBlockIsNotEmpty(msUserInputs))
  {
    code.addToCommaSeparatedList("userIn");
    sb.appendf(", userIn");
  }
  if (module.varDeclBlockIsNotEmpty(msFeedInputs))
  {
    code.addToCommaSeparatedList("feedIn");
    sb.appendf(", feedIn");
  }
  if (module.varDeclBlockIsNotEmpty(msOutputs))
  {
    code.addToCommaSeparatedList("out");
  }

  if (module.m_moduleOwnerPtr)
  {
    code.addToCommaSeparatedList("owner->m_apiBase");
    sb.appendf(", owner->m_apiBase");
  }

  sb.trimCharsFromFront(',');

  iterate_begin_const(ModuleDef::Child, module.m_children, child)
  {
    if (child.m_arraySize > 0)
    {
      code.write("for (int i=0; i<%i; i++)", child.m_arraySize);
      code.write("  %s[i]->create(this, i);", child.m_name.c_str());
    }
    else
    {
      code.write("%s->create(this);", child.m_name.c_str());
    }
  }
  iterate_end

  code.newLine();
  code.write("new(m_apiBase) %sAPIBase(%s);", module.m_name.c_str(), sb.c_str());

  if(!hasModuleModifier(module.m_modifiers, mmGlobalAccess))
  {
    code.write("new(m_updatePackage) %sUpdatePackage(", module.m_name.c_str());
    code.emitCommaSeparatedList(1, 3);
    code.write(");");

    // This is a list of the children's "feedIn" pointers for the feedbackPackage to generate it's constructor from
    code.startCommaSeparatedList();
    if (module.varDeclBlockIsNotEmpty(msData))
      code.addToCommaSeparatedList("data");
    if (module.varDeclBlockIsNotEmpty(msFeedInputs))
      code.addToCommaSeparatedList("feedIn");
    if (module.varDeclBlockIsNotEmpty(msInputs))
      code.addToCommaSeparatedList("in");
    if (module.varDeclBlockIsNotEmpty(msUserInputs))
      code.addToCommaSeparatedList("userIn");
    if (module.varDeclBlockIsNotEmpty(msFeedOutputs))
      code.addToCommaSeparatedList("feedOut");

    if (module.m_moduleOwnerPtr)
      code.addToCommaSeparatedList("owner->m_apiBase");

    code.write("new(m_feedbackPackage) %sFeedbackPackage(", module.m_name.c_str());
    code.emitCommaSeparatedList(1, 3);
    code.write(");");
  }

  code.closeBrace();
  code.newLine();
}

// -------------------------------------------------------------------------------------------------------------------
void ModuleGen::writeLoadStoreStateFns(const ModuleDef &module)
{
  code.writeHorizontalSeparator();

  // write serialisation store function
  code.write("bool %s::storeState(MR::PhysicsSerialisationBuffer& savedState)", module.m_name.c_str());
  code.openBrace();

  for (int i=msData; i<msNumVarDeclBlocks; i++)
  {
    if (module.varDeclBlockIsNotEmpty((ModuleSection)i))
    {
      code.write("savedState.addValue(*%s);", 
        keywords.getModuleSectionName((ModuleSection)i).c_str());
    }
  }

  code.write("storeStateChildren(savedState);");
  code.write("return true;");

  code.closeBrace();
  code.newLine();

  // write serialisation restore function
  code.writeHorizontalSeparator();
  code.write("bool %s::restoreState(MR::PhysicsSerialisationBuffer& savedState)", module.m_name.c_str());
  code.openBrace();

  for (int i=msData; i<msNumVarDeclBlocks; i++)
  {
    if (module.varDeclBlockIsNotEmpty((ModuleSection)i))
    {
      code.write("*%s = savedState.getValue<%s%s>();", 
        keywords.getModuleSectionName((ModuleSection)i).c_str(),
        module.m_name.c_str(),
        keywords.getModuleSectionLabel((ModuleSection)i).c_str());
    }
  }

  code.write("restoreStateChildren(savedState);");
  code.write("return true;");

  code.closeBrace();
  code.newLine();
}

// -------------------------------------------------------------------------------------------------------------------
void ModuleGen::writeClearAllDataFn(const ModuleDef& module)
{
  code.writeHorizontalSeparator();

  code.write("void %s::clearAllData()", module.m_name.c_str());
  code.openBrace();
  {
    if (module.varDeclBlockIsNotEmpty(msData))
      code.write("data->clear();");
    if (module.varDeclBlockIsNotEmpty(msInputs))
      code.write("in->clear();");
    if (module.varDeclBlockIsNotEmpty(msUserInputs))
      code.write("userIn->clear();");
    if (module.varDeclBlockIsNotEmpty(msOutputs))
      code.write("out->clear();");
    if (module.varDeclBlockIsNotEmpty(msFeedInputs))
      code.write("feedIn->clear();");
    if (module.varDeclBlockIsNotEmpty(msFeedOutputs))
      code.write("feedOut->clear();");
  }
  code.closeBrace();
  code.newLine();
}

// -------------------------------------------------------------------------------------------------------------------
void ModuleGen::writeUpdateAndFeedbackFns(const ModuleDef& module)
{
  if(hasModuleModifier(module.m_modifiers, mmNoUpdate))
  {
    // write comment to show where update function would have gone if we had need of one
    code.writeHorizontalSeparator();
    code.writeComment("No update() api required", module.m_name.c_str());
  }
  else
  {
    // -----------------------------------------------------------------------------------
    // write update function
    code.writeHorizontalSeparator();

    code.write("void %s::update(float timeStep)", module.m_name.c_str());
    code.openBrace();
    {
      code.write("const ER::RootModule* rootModule = getRootModule();");
      code.write("MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());");

      code.write("m_updatePackage->update(timeStep, rootModule->getDebugInterface());");

      code.write("#if NM_CALL_VALIDATORS");
      if (module.varDeclBlockIsNotEmpty(msData))
        code.write("data->validate();");
      code.write("#endif // NM_CALL_VALIDATORS");
    }
    code.closeBrace();
    code.newLine();
  }


  if(hasModuleModifier(module.m_modifiers, mmNoFeedback))
  {
    // write comment to show where feedback function would have gone if we had need of one
    code.writeHorizontalSeparator();
    code.writeComment("No feedback() required", module.m_name.c_str());
  }
  else
  {
    // -----------------------------------------------------------------------------------
    // write feedback function
    code.writeHorizontalSeparator();

    code.write("void %s::feedback(float timeStep)", module.m_name.c_str());
    code.openBrace();
    {
      code.write("const ER::RootModule* rootModule = getRootModule();");
      code.write("MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());");

      code.write("m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());");

      code.write("#if NM_CALL_VALIDATORS");
      if (module.varDeclBlockIsNotEmpty(msData))
        code.write("data->validate();");
      code.write("#endif // NM_CALL_VALIDATORS");
    }
    code.closeBrace();
    code.newLine();
  }
}

// -------------------------------------------------------------------------------------------------------------------
void ModuleGen::writeGetChildNameFn(const ModuleDef& module)
{
  code.writeHorizontalSeparator();

  // child name fn
  int chIndex = 0;
  bool hasChildren = !module.m_children.empty();

  code.write("const char* %s::getChildName(int32_t%s) const", module.m_name.c_str(), hasChildren?" index":"");
  code.openBrace();
  if (hasChildren)
  {
    code.write("switch (index)");
    code.openBrace();
    iterate_begin_const(ModuleDef::Child, module.m_children, child)
    {
      if (child.m_arraySize > 0)
      {
        for (unsigned int i=0; i<child.m_arraySize; i++)
        {
          code.write("case %i: return \"%s[%i]\";", chIndex++, child.m_name.c_str(), i);
        }
      }
      else
      {
        code.write("case %i: return \"%s\";", chIndex++, child.m_name.c_str());
      }
    }
    iterate_end
    code.write("default: NMP_ASSERT_FAIL();");
    code.closeBrace();
  }
  else
  {
    code.write("NMP_ASSERT_FAIL();");
  }
  code.write("return 0;");

  code.closeBrace();
  code.newLine();
}

// -------------------------------------------------------------------------------------------------------------------
void ModuleGen::writeDataAllocatorFns(const ModuleDef& module)
{
  code.writeHorizontalSeparator();

  code.write("void %sDataAllocator::init()", module.m_name.c_str());
  code.openBrace();

  for (int ms = msData; ms <= msOutputs; ms++)
  {
    code.write("m_mem[ER::ModuleDataAllocator::%s] = (unsigned char*)NMPMemoryAllocAligned(NMP::Memory::align(sizeof(DataMemory%s), 128), 128);", 
      keywords.getModuleSectionLabel((ModuleSection)ms).c_str(), 
      keywords.getModuleSectionLabel((ModuleSection)ms).c_str());

    code.write("m_offsets[ER::ModuleDataAllocator::%s] = 0;", 
      keywords.getModuleSectionLabel((ModuleSection)ms).c_str());

    code.write("m_limits[ER::ModuleDataAllocator::%s] =  NMP::Memory::align(sizeof(DataMemory%s), 128);", 
      keywords.getModuleSectionLabel((ModuleSection)ms).c_str(),
      keywords.getModuleSectionLabel((ModuleSection)ms).c_str());
  }

  code.closeBrace();


  code.writeHorizontalSeparator();
  code.write("void* %sDataAllocator::alloc(ER::ModuleDataAllocator::Section sct, size_t size, size_t alignment)", module.m_name.c_str());
  code.openBrace();
  code.write("m_offsets[sct] = (m_offsets[sct] + (alignment - 1)) & ~(alignment - 1);");
  code.write("unsigned char* mL = &((m_mem[sct])[m_offsets[sct]]);");
  code.write("m_offsets[sct] += size;");
  code.write("NMP_ASSERT(m_offsets[sct] <= m_limits[sct]);");
  code.write("return (void*)mL;");
  code.closeBrace();


  code.writeHorizontalSeparator();
  code.write("void %sDataAllocator::clear(ER::ModuleDataAllocator::Section sct)", module.m_name.c_str());
  code.openBrace();
  code.write("memset(m_mem[sct], 0, m_limits[sct]);");
  code.closeBrace();

  
  code.writeHorizontalSeparator();
  code.write("void %sDataAllocator::term()", module.m_name.c_str());
  code.openBrace();

  for (int ms = msData; ms <= msOutputs; ms++)
  {
    code.write("NMP::Memory::memFree( m_mem[ER::ModuleDataAllocator::%s] );", 
      keywords.getModuleSectionLabel((ModuleSection)ms).c_str());

    code.write("m_offsets[ER::ModuleDataAllocator::%s] = 0;", 
      keywords.getModuleSectionLabel((ModuleSection)ms).c_str());
  }

  code.closeBrace();
  code.newLine();
}
