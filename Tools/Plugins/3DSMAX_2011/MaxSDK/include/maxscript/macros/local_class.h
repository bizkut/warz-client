/*		local_class.h - macros for defining local classes and generics in a MAXScript extension .dlx
 *
 *
 *		Copyright (c) Autodesk, Inc.  1988.  John Wainwright.
 *
 */

#pragma once

#include "ClassCfg.h"
class Value;

#define local_visible_class(_cls)										\
	class _cls##Class : public ValueMetaClass							\
	{															\
	public:														\
		_cls##Class(MCHAR* name) : ValueMetaClass (name) { }				\
		void	collect() { delete this; }								\
	};															\
	extern _cls##Class _cls##_class;

#define local_applyable_class(_cls)									\
	class _cls##Class : public ValueMetaClass							\
	{															\
	public:														\
		_cls##Class(MCHAR* name) : ValueMetaClass (name) { }				\
		void	collect() { delete this; }								\
		Value* apply(Value** arglist, int count, CallContext* cc = NULL);	\
	};															\
	extern _cls##Class _cls##_class;

#define local_visible_class_instance(_cls, _name)							\
	_cls##Class _cls##_class (_M(_name));

class MS_LOCAL_ROOT_CLASS;
typedef Value* (MS_LOCAL_ROOT_CLASS::*local_value_vf)(Value**, int);
// confusing macros. Do not use them.
#define cat0(_a) _a
#define cat1(_a) cat0(_a)
#define cat2(_a, _b) cat0(_a)##cat0(_b)
#define str0(_c) #_c
#define str1(_c) str0(_c)

/*! \group Concatenation Macros
	For details on the token pasting operator and joining two strings together
	in the preprocessor see:
	\li ISO/IEC 14882:2003(E) 16.3.3 The ## operator
	\li ISO/IEC 9899:1999 (E) 6.10.3.3 The ## operator */
//@{
//! Helper macro used in concatenating two strings.
#define CONCATENATE_HELPER(x,y) x##y
//! Joins two strings together in the preprocessor before the code is compiled.
#define CONCATENATE(x,y) CONCATENATE_HELPER(x,y)
//@}

#define MS_LOCAL_ROOT_CLASS_TAG     &CONCATENATE(MS_LOCAL_ROOT_CLASS, _class)
#define MS_LOCAL_GENERIC_CLASS_TAG  &CONCATENATE(MS_LOCAL_GENERIC_CLASS, _class)
#define MS_LOCAL_GENERIC_CLASS_CLASS CONCATENATE(MS_LOCAL_GENERIC_CLASS, Class)
#define MS_LOCAL_GENERIC_CLASS_class CONCATENATE(MS_LOCAL_GENERIC_CLASS, _class)

//! \brief declares two classes used in exposing a new maxscript class.
/*! Use the DECLARE_LOCAL_GENERIC_CLASS macro below instead */
#define declare_local_generic_class                                     \
class MS_LOCAL_GENERIC_CLASS_CLASS : public ValueMetaClass              \
{                                                                       \
public:                                                                 \
     MS_LOCAL_GENERIC_CLASS_CLASS(MCHAR* name)                          \
     	: ValueMetaClass (name) { }                                     \
     void collect() { delete this; }                                    \
};                                                                      \
extern MS_LOCAL_GENERIC_CLASS_CLASS MS_LOCAL_GENERIC_CLASS_class;       \
class MS_LOCAL_GENERIC_CLASS : public Generic                           \
{                                                                       \
public:                                                                 \
               local_value_vf fn_ptr;                                   \
               MS_LOCAL_GENERIC_CLASS() { }                             \
               MS_LOCAL_GENERIC_CLASS(MCHAR* name, local_value_vf fn);  \
               classof_methods (MS_LOCAL_GENERIC_CLASS, Generic);       \
     void collect() { delete this; }                                    \
     Value* apply(Value** arglist, int count, CallContext* cc=NULL);    \
};
// Deprecated in Max 2011. Use DECLARE_LOCAL_GENERIC_CLASS(_cls) instead.
// Deprecated in Max 2011. Use DEFINE_LOCAL_GENERIC_CLASS_DEBUG_OK( _rootClass, _cls ) instead.
#pragma deprecated("declare_local_generic_class")