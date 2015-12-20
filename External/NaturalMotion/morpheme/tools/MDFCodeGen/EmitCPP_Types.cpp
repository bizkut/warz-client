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
#include "EmitCPP_Types.h"

#include "NMPlatform/NMPlatform.h"

// -------------------------------------------------------------------------------------------------------------------
bool TypeGen::writeType(const Type* ty)
{
  assert(ty->m_struct);
  const Struct* st = ty->m_struct;

  StringBuilder sb(64);
  ty->appendTypeFilename(sb);
  
  HeaderGuard hg("TYPE_%s", sb.c_str());

  code.writeHeaderTop(hg);

  writeDefInclude(code, projectRootDir, ty->m_pathToDef.c_str());

  StringBuilder fwdDeclsExternal(64), fwdDeclsInternal(64);
  code.startUniqueHeaders();
  calculateDependentHeadersAndFwdDecls(st->m_dependencies, true, code, &fwdDeclsExternal, &fwdDeclsInternal);

  unsigned int combGroup = code.addUniqueHeaderGroup("for combiners");
  code.addUniqueHeader(combGroup, "\"euphoria/erJunction.h\"");

  unsigned int constGroup = code.addUniqueHeaderGroup("constants");
  code.addUniqueHeader(constGroup, "\"NetworkConstants.h\"");

  unsigned int validatorGroup = code.addUniqueHeaderGroup("hook functions");
  code.addUniqueHeader(validatorGroup, "\"euphoria/erValueValidators.h\"");
  code.addUniqueHeader(validatorGroup, "\"euphoria/erValuePostCombiners.h\"");
  code.emitUniqueHeaders();

  if (fwdDeclsExternal.getLength() > 0)
  {
    code.newLine();
    code.writeRaw(fwdDeclsExternal.getBufferPtr());
  }

  code.newLine();
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

  // open namespace(s) for this struct
  if (!st->m_namespace.empty())
  {
    for (unsigned int i=0; i<st->m_namespaceStack.count(); i++)
    {
      code.write("namespace %s { ", st->m_namespaceStack.get(i).c_str());
    }    
  }

  code.newLine();

  // write comment header for the type, including the file that 
  // declared it and it's total data size
  char originFile[MAX_PATH];
  NMP_STRNCPY_S(originFile, MAX_PATH, st->m_pathToDef.c_str());
  PathStripPath(originFile);

  code.writeHorizontalSeparator();
  code.writeComment("Declaration from '%s'", originFile);
  code.writeComment("Data Payload: %i Bytes", st->getSize());
  if (ty->m_alignment > 0)
    code.writeComment("Alignment: %i", ty->m_alignment);

  // include list of modifier keywords (mainly for parser debugging)
  if (st->m_modifiers.stack.count() > 0)
  {
    sb.reset();
    keywords.dumpModifierOptions(st->m_modifiers, sb);
    code.writeComment("Modifiers: %s ", sb.getBufferPtr());
  }

  // add brief type description
  if (!(st->m_shortDesc.empty()))
  {
    code.newLine();
    code.writeRaw("%s", st->m_shortDesc);
    code.newLine();
  }

  // begin struct definition
  code.writeRaw("struct %s", st->m_name.c_str());

  // write out inheritance 
  if (!st->m_inheritanceName.empty())
  {
    code.writeRaw(" : public ");

    if (st->m_inheritanceNamespace.empty())
    {
      code.writeRaw("%s", st->m_inheritanceName.c_str());
    }
    else
    {
      code.writeRaw("%s::%s", st->m_inheritanceNamespace.c_str(), st->m_inheritanceName.c_str());
    }
  }

  // begin struct innards
  code.newLine();
  code.openBrace();
  code.resetAccessibility(CodeWriter::kPublic);

  DataFootprint fc;

  // write variables
  for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
  {
    if (st->m_enumDecl[i].size() > 0 ||
        st->m_vardecl[i].size() > 0 ||
        st->m_functionDecl[i].size() > 0)
    {
      // write public:, private:
      if (i == 0)
        code.switchAccessibility(CodeWriter::kPublic);
      else
        code.switchAccessibility(CodeWriter::kPrivate);

      code.newLine();


      Struct::EnumDecls::const_iterator enum_it = st->m_enumDecl[i].begin();
      Struct::EnumDecls::const_iterator enum_end = st->m_enumDecl[i].end();
      for (; enum_it != enum_end; ++enum_it)
      {
        const Struct::EnumDecl& enm = (*enum_it);
        writeEnumDecl(code, enm);
      }

      iterate_begin_const(Struct::VarDecl, st->m_vardecl[i], vd)
      {
        std::string tabs = code.makeTabString(1);

        vd.toString(sb, true, true);

        writeVarDecl(code, vd, fc);

        // write out a comment that contains the size of any embedded data types and
        // any attribute modifiers applied to this variable
        sb.reset();
        if (vd.m_knownType->m_struct)
        {
          sb.appendInt(vd.getSize());
          sb.appendCharBuf(" Bytes ");
        }

        if (vd.m_modifiers.stack.count() > 0)
        {
          sb.appendCharBuf(" mods: ");
          keywords.dumpModifierOptions(vd.m_modifiers, sb);
        }

        if (sb.getLength() > 0)
        {
          code.writeRaw(" // %s", sb.getBufferPtr());
        }

        code.newLine();
      }
      iterate_end

      if (st->m_functionDecl[i].size() > 0)
      {
        code.newLine();
        code.writeComment("functions");
        code.newLine();

        Struct::FnDecls::const_iterator fn_it = st->m_functionDecl[i].begin();
        Struct::FnDecls::const_iterator fn_end = st->m_functionDecl[i].end();
        for (; fn_it != fn_end; ++fn_it)
        {
          // Function comment.
          if (!((*fn_it).m_shortDesc.empty()))
          {
            // Erase < from the comment if there is any.
            // Write-raw every comment line separately,
            // inserting tabs in between, trim '\n' from the end of last line.

            const std::string endOfLine("\n");
            const std::string fwrdSlashLessThan("/<");
            const std::string lessThan("<");
            std::string::size_type breaklocation = std::string::npos;
            std::string comment((*fn_it).m_shortDesc.c_str());
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
          } // End of function comment.

          // Function.
          const PString& fnDecl = (*fn_it).m_fn;
          code.write("%s", fnDecl.c_str());
        }
      }

      code.newLine();
    }
  }

  code.switchAccessibility(CodeWriter::kPublic);
  code.newLine();

  bool wroteQuatFlipFn = false;

  // write combiner function
  if (!hasStructModifier(st->m_modifiers, smNoCombine))
  {
    // write blend operators
    if (st->allVarDeclsAreBlendable())
    {
      writeCombineOperator(ty, TypeGen::MulAcc);
      writeCombineOperator(ty, TypeGen::Mul);
      writeCombineOperator(ty, TypeGen::AddAcc);
      writeCombineOperator(ty, TypeGen::Add);
      code.newLine();
    }

    {
      for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
      {
        iterate_begin_const(Struct::VarDecl, st->m_vardecl[i], vd)
        {
          // HAX HAX HAX
          if (strcmp(vd.m_typename.c_str(), "Quat") == 0)
          {
            if (!wroteQuatFlipFn)
            {
              code.write("NM_INLINE void doQuatFlip(const %s &acc)", st->m_name.c_str()); 
              code.openBrace();
              wroteQuatFlipFn = true;
            }

            code.write("if (%s.dot(acc.%s) < 0.f)", vd.m_varname.c_str(), vd.m_varname.c_str());
            code.write("  %s.negate();", vd.m_varname.c_str());
          }
        }
        iterate_end
      }
    }
    if (wroteQuatFlipFn)
    {
      code.closeBrace();
      code.newLine();
    }
  }

  // write out a validation function, if the struct has var decls with validator attributes
  if (st->m_hasValidationFunction)
  {
    code.write("NM_INLINE void validate() const");
    code.openBrace();

    code.write("#if NM_CALL_VALIDATORS");
    for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
    {
      iterate_begin_const(Struct::VarDecl, st->m_vardecl[i], vd)
      {
        writeVarDeclValidator(code, vd);
      }
      iterate_end
    }
    code.write("#endif // NM_CALL_VALIDATORS");

    // add hook function, if specified
    if (const ModifierOption* vHook = hasStructModifier(st->m_modifiers, smValidator))
    {
      code.write("%s();", vHook->m_value.c_str(), st->m_name.c_str());
    }

    code.closeBrace();
    code.newLine();
  }

  // ditto on post-combine
  if (st->m_hasPostCombineFunction)
  {
    code.write("NM_INLINE void postCombine()");
    code.openBrace();

    for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
    {
      iterate_begin_const(Struct::VarDecl, st->m_vardecl[i], vd)
      {
        writeVarDeclPostCombine(code, vd);
      }
      iterate_end
    }

    // call the validators post-post-combine
    if (st->m_hasValidationFunction)
    {
      code.write("validate();");
    }

    code.closeBrace();
    code.newLine();
  }


  // finish struct innards
  code.closeBraceWithSemicolon(false);
  code.write(" // struct %s", st->m_name.c_str());
  code.newLine();

  // close namespaces
  if (!st->m_namespace.empty())
  {
    for (unsigned int i=0; i<st->m_namespaceStack.count(); i++)
      code.write("} // namespace %s", st->m_namespaceStack.get(i).c_str());
  }

  code.newLine();
  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLine();

  StringBuilder ns(32);
  ns.appendCharBuf("NM_BEHAVIOUR_LIB_NAMESPACE::");
  if (!st->m_namespace.empty())
  {
    ns.appendPString(st->m_namespace);
    ns.appendCharBuf("::");
  }
  ns.appendPString(st->m_name);

  // HAX
  if (wroteQuatFlipFn)
  {
    code.write("namespace ER");
    code.openBrace();
    {
      code.write("template<>");
      code.write("NM_INLINE void quatFlip(%s &val, const %s &acc)", 
        ns.c_str(), 
        ns.c_str());

      code.openBrace();
      {
        code.write("val.doQuatFlip(acc);");
      }
      code.closeBrace();
    }
    code.closeBrace();
  }

  code.writeHeaderBottom(hg);
  return true;
}


// -------------------------------------------------------------------------------------------------------------------
void TypeGen::writeCombineOperator(const Type* ty, TypeGen::CombineOperator co)
{
  const Struct* st = ty->m_struct;

  switch (co)
  {
    case TypeGen::MulAcc:
      code.write("NM_INLINE void operator *= (const float fVal)");
      break;
    case TypeGen::Mul:
      code.write("NM_INLINE %s operator * (const float fVal) const", st->m_name.c_str());
      break;
    case TypeGen::AddAcc:
      code.write("NM_INLINE void operator += (const %s& rhs)", st->m_name.c_str());
      break;
    case TypeGen::Add:
      code.write("NM_INLINE %s operator + (const %s& rhs) const", st->m_name.c_str(), st->m_name.c_str());
      break;
  }

  code.openBrace();
  {
    switch (co)
    {
      case TypeGen::Mul:
      case TypeGen::Add:
        code.write("%s result;", st->m_name.c_str() );
        break;
    }

    for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
    {
      iterate_begin_const(Struct::VarDecl, st->m_vardecl[i], vd)
      {
        const ModifierOption* moPreProcessor = hasTypeModifier(vd.m_knownType->m_modifiers, tmPreProcessor);
        if (moPreProcessor)
          code.write("#if defined(%s)", moPreProcessor->m_value.c_str());

        assert (vd.m_knownType->isBlendableType());

        if (vd.m_arraySize > 0)
        {
          for (uint32_t a=0; a<vd.m_arraySize; a++)
          {
            switch (co)
            {
              case TypeGen::MulAcc:
                code.write("%s[%i] *= fVal;", vd.m_varname.c_str(), a);
                break;
              case TypeGen::Mul:
                code.write("result.%s[%i] = %s[%i] * fVal;", vd.m_varname.c_str(), a, vd.m_varname.c_str(), a);
                break;
              case TypeGen::AddAcc:
                code.write("%s[%i] += rhs.%s[%i];", vd.m_varname.c_str(), a, vd.m_varname.c_str(), a);
                break;
              case TypeGen::Add:
                code.write("result.%s[%i] = %s[%i] + rhs.%s[%i];", vd.m_varname.c_str(), a, vd.m_varname.c_str(), a, vd.m_varname.c_str(), a);
                break;
            }
          }
        }
        else
        {
          switch (co)
          {
          case TypeGen::MulAcc:
            code.write("%s *= fVal;", vd.m_varname.c_str());
            break;
          case TypeGen::Mul:
            code.write("result.%s = %s * fVal;", vd.m_varname.c_str(), vd.m_varname.c_str());
            break;
          case TypeGen::AddAcc:
            code.write("%s += rhs.%s;", vd.m_varname.c_str(), vd.m_varname.c_str());
            break;
          case TypeGen::Add:
            code.write("result.%s = %s + rhs.%s;", vd.m_varname.c_str(), vd.m_varname.c_str(), vd.m_varname.c_str());
            break;
          }
        }
        if (moPreProcessor)
          code.write("#endif // %s", moPreProcessor->m_value.c_str());
      }
      iterate_end
    }

    switch (co)
    {
      case TypeGen::Mul:
      case TypeGen::Add:
        code.write("return result;");
        break;
    }
  }
  code.closeBrace();
  code.newLine();
}
