/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: BRGR - secondary contact: JUBA
#ifndef Navigation_TraverseLogicObsoleteChecker_H
#define Navigation_TraverseLogicObsoleteChecker_H

#include "gwnavruntime/base/types.h"

namespace Kaim
{

class Vec3f;
class NavTag;
class NavTriangleRawPtr;
class NavGraphEdgeRawPtr;

class LogicWithoutCostMultipler;
class LogicWithCostMultiplerPerNavTag;
class LogicWithCostMultiplerPerTriangle;


/// This class is used in order to check that an inherited TraverseLogic is not implementing an obsolete function!
/// It is systematically done when adding a NavigationProfile to the World.
/// This check is done at runtime. It is based on checking static function pointers.
/// The idea is if the pointer returned by the Derived class is the same than the one returned by the Base class,
/// in that case we know for sure that the Derived class doesn't implement and/or hide the Base class's function.
class TraverseLogicObsoleteChecker
{
	TraverseLogicObsoleteChecker();
	KY_CLASS_WITHOUT_COPY(TraverseLogicObsoleteChecker)

public:
	template<class TLogic>
	static KY_INLINE bool HasNoObsoleteFunctions() { return Check<TLogic>(typename TLogic::CostMultiplierUsage()); }

private:
	// ----- SimpleTraverseLogic interface check ----

	class SimpleTraverseLogicObsoleteFuncPtr
	{
	public:
		bool CompareAndLog(const SimpleTraverseLogicObsoleteFuncPtr& other);

	public:
		bool (*m_ptrCanTraverse)(void*, const NavTag&);
		KyFloat32 (*m_ptrGetCostMultiplier)(void*, const NavTag&);
		bool (*m_ptrCanTraverseGraphEdgeAndGetCost)(void*, const Vec3f&, const Vec3f&, const NavTag&, KyFloat32&);
	};

	// Init is not a member of SimpleTraverseLogicObsoleteFuncPtr to satisfy friend declaration in KY_TRAVERSE_LOGIC
	// Some compilers doesn't consider a nested class of a friend class as a friend class
	template<class TLogic>
	static void Init(SimpleTraverseLogicObsoleteFuncPtr& funcPtr)
	{
		funcPtr.m_ptrCanTraverse = &TLogic::CanTraverse;
		funcPtr.m_ptrGetCostMultiplier = &TLogic::GetCostMultiplier;
		funcPtr.m_ptrCanTraverseGraphEdgeAndGetCost = &TLogic::CanTraverseGraphEdgeAndGetCost;
	}

	template<class TLogic>
	static bool Check(const LogicWithoutCostMultipler&)
	{
		SimpleTraverseLogicObsoleteFuncPtr base;
		Init<typename TLogic::BaseClass>(base);

		SimpleTraverseLogicObsoleteFuncPtr derived;
		Init<TLogic>(derived);

		return base.CompareAndLog(derived);
	}

	
	// ----- TraverseLogicWithCostMultiplerPerNavTag interface check ----

	template<class TLogic>
	static bool Check(const LogicWithCostMultiplerPerNavTag&) { return true; }

	// ----- TraverseLogicWithCostPerTriangle interface check ----
	
	template<class TLogic>
	static bool Check(const LogicWithCostMultiplerPerTriangle&) { return true; }
};



}

#endif

