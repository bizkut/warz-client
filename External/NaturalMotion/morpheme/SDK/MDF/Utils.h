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

#include "ParserDefs.h"

/**
 * find the module with no owner (after verification, there will be only one)
 */
const ModuleDef* getRootModule(const ModuleDefs& defs);


/**
 * Get the title of an attribute variable
 */
void getAttributeVariableTitle(const BehaviourDef::AttributeVariable& av, StringBuilder& sb);


/**
 * macros used in functions that do a lot of iterator walking!
 * usage:
 *
 * iterate_begin(type, vector, loop_varname)
 * {
 *    loop_varname.<stuff>
 * }
 * iterate_end
 *
 */
#define iterate_begin_const(_type, _vector, _iname) \
    {_type##s::const_iterator _iname##it   = _vector.begin();\
    _type##s::const_iterator _iname##end  = _vector.end();\
    for (; _iname##it != _iname##end; ++_iname##it)\
    {\
      const _type& _iname = (*_iname##it);
#define iterate_begin_const_ptr(_type, _vector, _iname) \
    {_type##s::const_iterator _iname##it   = _vector.begin();\
    _type##s::const_iterator _iname##end  = _vector.end();\
    for (; _iname##it != _iname##end; ++_iname##it)\
    {\
      const _type* _iname = (*_iname##it);
#define iterate_begin_const_reverse(_type, _vector, _iname) \
    {_type##s::const_reverse_iterator _iname##it   = _vector.rbegin();\
    _type##s::const_reverse_iterator _iname##end  = _vector.rend();\
    for (; _iname##it != _iname##end; ++_iname##it)\
    {\
      const _type& _iname = (*_iname##it);
#define iterate_begin_const_ptr_reverse(_type, _vector, _iname) \
    {_type##s::const_reverse_iterator _iname##it   = _vector.rbegin();\
    _type##s::const_reverse_iterator _iname##end  = _vector.rend();\
    for (; _iname##it != _iname##end; ++_iname##it)\
    {\
      const _type* _iname = (*_iname##it);
#define iterate_begin(_type, _vector, _iname) \
    {_type##s::iterator _iname##it   = _vector.begin();\
    _type##s::iterator _iname##end  = _vector.end();\
    for (; _iname##it != _iname##end; ++_iname##it)\
    {\
      _type& _iname = (*_iname##it);
#define iterate_begin_ptr(_type, _vector, _iname) \
    {_type##s::iterator _iname##it   = _vector.begin();\
    _type##s::iterator _iname##end  = _vector.end();\
    for (; _iname##it != _iname##end; ++_iname##it)\
    {\
      _type* _iname = (*_iname##it);
#define iterate_begin_reverse(_type, _vector, _iname) \
    {_type##s::reverse_iterator _iname##it   = _vector.rbegin();\
    _type##s::reverse_iterator _iname##end  = _vector.rend();\
    for (; _iname##it != _iname##end; ++_iname##it)\
    {\
      _type& _iname = (*_iname##it);
#define iterate_begin_reverse_ptr(_type, _vector, _iname) \
    {_type##s::reverse_iterator _iname##it   = _vector.rbegin();\
    _type##s::reverse_iterator _iname##end  = _vector.rend();\
    for (; _iname##it != _iname##end; ++_iname##it)\
    {\
      _type* _iname = (*_iname##it);
#define iterate_end   }}
