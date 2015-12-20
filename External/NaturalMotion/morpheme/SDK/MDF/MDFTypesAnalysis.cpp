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
#include "MDFTypesAnalysis.h"
#include "StringBuilder.h"
#include "Utils.h"
#include "nmtl/algorithm.h"

#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
void walkStructAlignment(const Struct *s, unsigned int& structAlignmentInherit)
{
  for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
  {
    Struct::VarDecls::const_iterator it = s->m_vardecl[i].begin();
    Struct::VarDecls::const_iterator end = s->m_vardecl[i].end();
    for (; it != end; ++it)
    {
      const Struct::VarDecl& vd = (*it);
      assert(vd.m_knownType);

      if (vd.m_knownType->m_alignment > structAlignmentInherit)
        structAlignmentInherit = vd.m_knownType->m_alignment;

      if (vd.m_knownType->m_struct)
      {
        walkStructAlignment(vd.m_knownType->m_struct, structAlignmentInherit);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// find any instances of __align__ modifiers, write the chosen alignment into the Type
void findAlignmentModifiers(Type* t, void*)
{
  const ModifierOption* alignMo = hasTypeModifier(t->m_modifiers, tmAlign);
  if (alignMo)
  {
    int alignmentBoundary = atoi(alignMo->m_value.c_str());
    assert(alignmentBoundary > 0);

    t->m_alignment = alignmentBoundary;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void validateTypes(TypesRegistry& typesRegistry, StructMap& structs)
{
  char vdNameCapFront[128];
  StringBuilder sbuild(32);
  StructMap::value_walker structValues = structs.walker();

  // move any __align__ modifiers into Type alignment variables
  typesRegistry.iterate(findAlignmentModifiers, 0);


  // pass 1 - register all structs & enums as identifiable types
  while (structValues.next())
  {
    Struct* s = structValues();

    for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
    {
      Struct::EnumDecls::iterator it = s->m_enumDecl[i].begin();
      Struct::EnumDecls::iterator end = s->m_enumDecl[i].end();
      for (; it != end; ++it)
      {
        Struct::EnumDecl &enm = (*it);

        // assume 4-byte enums, register with struct name as namespace suffix
        parserMemoryAllocArgs(Type, t, enm.m_ownerStruct->getFullNamespace(sbuild), enm.m_name.c_str(), 4);

        t->m_pathToDef = s->m_pathToDef;
        t->m_filetime = s->m_filetime;

        if (i == Struct::abPrivate)
          t->m_private = true;

        t->m_namespaceStack = enm.m_ownerStruct->m_namespaceStack;

        if (!typesRegistry.registerType(t))
        {
          throw(new ValidationException(s, enm.m_lineDefined, enm.m_name.c_str(), "enum type already registered"));
        }
      }
    }

    // record the structure type
    parserMemoryAllocArgs(Type, t, s->m_namespace.c_str(), s->m_name.c_str(), 0);
    t->m_struct = s;
    s->m_type = t;

    t->m_pathToDef = s->m_pathToDef;
    t->m_filetime = s->m_filetime;

    t->m_namespaceStack = s->m_namespaceStack;

    if (!typesRegistry.registerType(t))
    {
      throw(new ValidationException(s, s->m_lineDefined, s->m_name.c_str(), "type already registered"));
    }
  }

  // pass 2 - resolve 'known type' pointers for all struct members
  structValues.reset();
  while (structValues.next())
  {
    Struct* s = structValues();

    for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
    {
      iterate_begin(Struct::VarDecl, s->m_vardecl[i], vd)
      {
        // capitalize the first character of the variable name
        const char* vdName = vd.m_varname.c_str();
        NMP_STRNCPY_S(vdNameCapFront, sizeof(vdNameCapFront), vdName);
        vdNameCapFront[0] = (char)toupper(vdName[0]);
        vd.m_varnameCapFront.set(vdNameCapFront);

        // try looking up in the registry as is
        vd.m_knownType = typesRegistry.lookupFromVarDecl(vd);

        // not found, see if this type was defined in the struct
        if (!vd.m_knownType)
        {
          vd.m_knownType = typesRegistry.lookupByName(
            PString(s->getFullNamespace(sbuild)),
            vd.m_typename);
        }

        // cannot resolve type, bail out
        if (!vd.m_knownType)
        {
          vd.toString(sbuild);
          throw(new ValidationException(s, vd.m_lineDefined, sbuild.getBufferPtr(), "unknown type used"));
        }
      }
      iterate_end
    }

    // if this struct inherits from another, look it up and store a link to the type
    if (!s->m_inheritanceName.empty())
    {
      const Type* inheritant = typesRegistry.lookupByName(
        s->m_inheritanceNamespace,
        s->m_inheritanceName);

      if (!inheritant)
      {
        throw(new ValidationException(s, s->m_lineDefined, s->m_inheritanceName.c_str(), "cannot derive from unknown type"));
      }

      if (!inheritant->m_struct && 
          !hasTypeModifier(inheritant->m_modifiers, tmExternalClass) &&
          !hasTypeModifier(inheritant->m_modifiers, tmExternalStruct))
      {
        throw(new ValidationException(s, s->m_lineDefined, s->m_inheritanceName.c_str(), "cannot derive from basic/built-in type"));
      }

      s->m_inheritancePtr = inheritant;
      s->m_dependencies.registerDependentType(inheritant, false);
    }
  }


  // pass 3 - cascade alignment values; any structs with aligned variables
  //          need to inherit their alignment, as the compiler will do this on the generated code
  structValues.reset();
  while (structValues.next())
  {
    Struct* s = structValues();

    unsigned int alignInherit = 0;
    walkStructAlignment(s, alignInherit);

    if (alignInherit > 0)
    {
      s->m_type->m_alignment = alignInherit;
    }
  }


  VarDeclTypeSizeSorter sorter;

  // pass 4 - cache all struct sizes, sort vardecls, handle type dependency
  structValues.reset();
  while (structValues.next())
  {
    Struct* s = structValues();

    // calculate struct type size
    s->m_type->m_sizeInBytes = s->getSize();

    // sort each block by effective type size
    for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
    {
      nmtl::sort( s->m_vardecl[i].begin( ), s->m_vardecl[i].end( ), sorter );

      // search to see if any var decls have any pertinent attributes that will affect the struct as a whole
      iterate_begin(Struct::VarDecl, s->m_vardecl[i], vd)
      {
        if (hasTypeModifier(vd.m_knownType->m_modifiers, tmValidator) || 
            hasStructModifier(s->m_modifiers, smValidator) || 
            hasVarDeclModifier(vd.m_modifiers, vdmValidator))
          s->m_hasValidationFunction = true;

        if (hasTypeModifier(vd.m_knownType->m_modifiers, tmPostCombine))
          s->m_hasPostCombineFunction = true;

        if (hasTypeModifier(vd.m_knownType->m_modifiers, tmPreProcessor))
          s->m_hasPreProcessorMacro = true;
      }
      iterate_end
    }

    // lookup type dependencies, store found Type* in dependency list
    ParsedTypeDependencies::const_iterator it = s->m_typeDependencies.begin();
    ParsedTypeDependencies::const_iterator end = s->m_typeDependencies.end();
    for (; it != end; ++it)
    {
      const ParsedTypeDependency& td = (*it);
      td.m_vdc.getNamespaceFromIndices(sbuild);

      const Type* vdt = typesRegistry.lookupByName((PString)sbuild.getBufferPtr(), td.m_vdc.m_typespec.getLast());
      if (!vdt)
      {
        throw(new ValidationException(s, td.m_vdc.m_lineDefined, td.m_vdc.m_typespec.getLast().c_str(), "unknown type found in struct dependency"));
      }

      s->m_dependencies.registerDependentType(vdt, td.m_ptrOrRef);
    }

    // m_dependentTypes
  }
}
