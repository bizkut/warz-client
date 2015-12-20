//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
#pragma once

#include <WTypes.h>
#include "maxheap.h"
#include "Animatable.h"

// Callback for EnumAnimTree:
//
// Scope values:

#define SCOPE_DOCLOSED 1   		// do "closed" animatables.
#define SCOPE_SUBANIM  2		// do the sub anims 
#define SCOPE_CHILDREN 4 		// do the node children
#define SCOPE_OPEN	(SCOPE_SUBANIM|SCOPE_CHILDREN) // do all open animatables
#define SCOPE_ALL	(SCOPE_OPEN|SCOPE_DOCLOSED)     // do all animatables

// Return values for AnimEnum procs
#define ANIM_ENUM_PROCEED 1
#define ANIM_ENUM_STOP    2
#define ANIM_ENUM_ABORT   3

// R5 and later only
#define ANIM_ENUM_SKIP    4   // Do not include this anim in the hierarchy.
#define ANIM_ENUM_SKIP_NODE 5 // Do not include this node and its subAnims, but include its children

/*! \sa  Class Animatable.\n\n
\par Description:
This class is the callback object for <b>Animatable::EnumAnimTree()</b>. This
keeps track of the depth of the enumeration. */
class AnimEnum: public MaxHeapOperators {
	protected:
		int depth;
		int scope;  
		DWORD tv;
	public:
		/*! \remarks Constructor. Sets default scope and depth if specified.
		\return  A new AnimEnum object. */
	 	AnimEnum(int s = SCOPE_OPEN, int deep = 0, DWORD tv=0xffffffff) 
			{scope = s; depth = deep; this->tv = tv;}
		/*! \remarks Destructor. */
		virtual ~AnimEnum() {;}
		/*! \remarks Implemented by the System.\n\n
		Sets the scope. See below for possible values.
		\param s Specifies the scope to set. See below. */
		void SetScope(int s) { scope = s; }
		/*! \remarks Implemented by the System.\n\n
		Returns the scope.
		\return  One or more of the following scope values:\n\n
		<b>SCOPE_DOCLOSED</b>\n\n
		Do closed animatables.\n\n
		<b>SCOPE_SUBANIM</b>\n\n
		Do the sub anims\n\n
		<b>SCOPE_CHILDREN</b>\n\n
		Do the node children\n\n
		<b>SCOPE_OPEN</b>\n\n
		Do all open animatables\n\n
		Equal to <b>(SCOPE_SUBANIM|SCOPE_CHILDREN)</b>\n\n
		<b>SCOPE_ALL</b>\n\n
		Do all animatables.\n\n
		Equal to <b>(SCOPE_OPEN|SCOPE_DOCLOSED)</b> */
		int Scope() { return scope; }
		/*! \remarks Implemented by the System.\n\n
		Increments the depth count. */
		void IncDepth() { depth++; }
		/*! \remarks Implemented by the System.\n\n
		Decrements the depth count. */
		void DecDepth() { depth--; }
		/*! \remarks Implemented by the System.\n\n
		Returns the depth count. */
		int Depth() { return depth; }
		DWORD TVBits() {return tv;}
		/*! \remarks This is the method called by <b>EnumAnimTree()</b>.
		\param anim The sub anim.
		\param client The client anim. This is the parent with a sub-anim of <b>anim</b>.
		\param subNum The index of the sub-anim that <b>anim</b> is to <b>client</b>. For
		example, if you were to call <b>client-\>SubAnim(subNum)</b> it would
		return <b>anim</b>.
		\return  One of the following values:\n\n
		<b>ANIM_ENUM_PROCEED</b>\n\n
		Continue the enumeration process.\n\n
		<b>ANIM_ENUM_STOP</b>\n\n
		Stop the enumeration process at this level.\n\n
		<b>ANIM_ENUM_ABORT</b>\n\n
		Abort the enumeration processing. */
		virtual int proc(Animatable *anim, Animatable *client, int subNum)=0;
	};

// A useful enumerator
class ClearAnimFlagEnumProc : public AnimEnum
{
	DWORD flag;
public:
	//! Constructor
	ClearAnimFlagEnumProc(DWORD f)
		: flag(f)
	{
		// empty
	}
	int proc(Animatable* anim, Animatable* client, int subNum)
	{
		UNUSED_PARAM(client);
		UNUSED_PARAM(subNum);
		anim->ClearAFlag(flag);
		return ANIM_ENUM_PROCEED;
	}
};