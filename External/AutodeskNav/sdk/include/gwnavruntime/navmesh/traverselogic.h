/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: GUAL
#ifndef Navigation_TraverseLogic_H
#define Navigation_TraverseLogic_H


#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/basesystem/logger.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/database/navtag.h"

namespace Kaim
{

class NavTriangleRawPtr;
class NavGraphEdgeRawPtr;

class LogicDoNotUseCanEnterNavTag       { public: LogicDoNotUseCanEnterNavTag() {} };
class LogicDoUseCanEnterNavTag          { public:    LogicDoUseCanEnterNavTag() {} };

class LogicWithoutCostMultipler         { public:         LogicWithoutCostMultipler() {} };
class LogicWithCostMultiplerPerNavTag   { public:   LogicWithCostMultiplerPerNavTag() {} };
class LogicWithCostMultiplerPerTriangle { public: LogicWithCostMultiplerPerTriangle() {} };

/// A TraverseLogic is a full static class that implements some functions and typedef used to customize
/// the Navigation and that is passed as template arguments to many queries.
/// Most relevant example are: AStarQuery<TraverseLogic> and RayCast<TraverseLogic>.
/// a TraverseLogic is typically set thanks to NavigationProfile<TraverseLogic>
///
/// You can create your own TraverseLogic by deriving it from one the three base TraverseLogic class
/// below, depending on how you want to customize the PathFinding/PathFollowing. Each of this three base
/// TraverseLogic defines typename that are used to perform optimal code generation (using function overloading).
/// The template interface is different depending on which logic you derive from.
/// In your derivation, you can overwrite all or only some of the functions present in the base TraverseLogic.
///
/// Each of the three base TraverseLogic must be templated by LogicDoNotUseCanEnterNavTag or LogicDoUseCanEnterNavTag
/// This template parameter tells whether a CanEnterNavTag function needs to be called to forbid passing from one
/// navTag to another at some positions (for instance, at an entrance of a 1-way corridor), depending on whether
/// you want the function CanEnterNavTag(...) to be called and tested by the Queries on the Database.
/// Note that is you use LogicDoUseCanEnterNavTag, you have also to implement the function
/// static bool CanEnterNavTag(void* /*traverseLogicUserData*/, const NavTag& /*exitNavTag*/, const NavTag& /*enterNavTag*/, const Vec3f& /*pos*/)
/// This method is called each time the path finding query or path following system considers a transition to another 
/// NavTag, after the CanTraverseNavTag() method indicates that the NavTag can be traversed. It is expected to
/// determine whether or not the transition from the exitNavTag to the enterNavTag is possible. The input position is
/// the exact position of the transition between the two NavTags.
/// If this method returns true, the transition will be allowed; if it returns false, the transition will not be allowed.
/// It is called on the NavMesh only.

/// SimpleTraverseLogic defines all the functions that are potentially called in the PathFinding and PathFollowing system when
/// there is not not customization of the cost at all. A navTag is allowed or forbidden.
template <class TCanEnterNavTagMode = LogicDoNotUseCanEnterNavTag>
class SimpleTraverseLogic
{
	KY_TRAVERSE_LOGIC(SimpleTraverseLogic, LogicWithoutCostMultipler, TCanEnterNavTagMode)
public:
	/// These method are called each time the path finding query, path following system or any other query considers crossing into a
	/// different navTag. It is expected to determine whether or not the path is allowed to cross it by returning true if it is traversable  and 
	/// false if it is not traversable.
	static KY_INLINE bool CanTraverseNavTag(void* /*traverseLogicUserData*/, const NavTag& /*navTag*/) { return true; }

	/// This method is called for each candidate node in the Path calculation. It is expected to determine the approximate cost of moving
	/// the specified distance toward the final destination. This method is only used by the AStarQuery. You do not have to provide an
	/// implementation for other kinds of queries that propagate only through the NavMesh, such as the RayCastQuery. This method is not
	/// called during path following. 
	static KY_INLINE KyFloat32 GetHeuristicFromDistanceToDest(void* /*traverseLogicUserData*/, KyFloat32 distance3DToDestination) { return distance3DToDestination; }

	/// This method is only required when you set up your customizer to validate NavTag transitions. If you use the default
	/// DoNotUseCanEnterNavTag template parameter, you do not need to provide an implementation of CanEnterNavTag().
	static KY_INLINE bool CanEnterNavTag(void* /*traverseLogicUserData*/, const NavTag& /*exitNavTag*/, const NavTag& /*enterNavTag*/,
		const Vec3f& /*pos*/) { return true; }

private:
	// ----- Obsolete Functions -----
	// These functions are now obsolete, they were part of the SimpleTraverseLogic interface.
	// You should remove them from your inheriting TraverseLogic class, 
	// and implement __only__ the public functions above.
	// Implementations of such functions in inheriting classes can be caught at runtime when adding a NavigationProfile templated by your TraverseLogic class to the World.
	// Or you can call TraverseLogicObsoleteChecker::HasNoObsoleteFunctions<YourTraverseLogic>()

	static bool CanTraverse(void*, const Kaim::NavTag&) 
	{ KY_LOG_ERROR(("Obsolete Function should never be called")); KY_DEBUG_BREAK; return false; }

	static KyFloat32 GetCostMultiplier(void*, const Kaim::NavTag&) 
	{ KY_LOG_ERROR(("Obsolete Function should never be called")); KY_DEBUG_BREAK; return 0.f; }

	static bool CanTraverseGraphEdgeAndGetCost(void*, const Kaim::Vec3f&, const Kaim::Vec3f&, const Kaim::NavTag&, KyFloat32&)
	{ KY_LOG_ERROR(("Obsolete Function should never be called")); KY_DEBUG_BREAK; return false; }
};


class DefaultTraverseLogic : public SimpleTraverseLogic<LogicDoNotUseCanEnterNavTag>
{
};




/// TraverseLogicWithCostMultiplerPerNavTag defines all the functions that are potentially called in
/// the PathFinding and PathFollowing system when there the cost need to be customized according to the navTag.
template <class TCanEnterNavTagMode>
class TraverseLogicWithCostMultiplerPerNavTag
{
	KY_TRAVERSE_LOGIC(TraverseLogicWithCostMultiplerPerNavTag, LogicWithCostMultiplerPerNavTag, TCanEnterNavTagMode)
public:

	/// These method are called each time the path finding query, path following system or any other query considers crossing
	/// into a navTag. It is expected to determine whether or not the path is allowed to cross it by returning true if it is
	/// traversable  and false if it is not traversable.
	/// These functions can also be called in a context that request a cost multiplier associated to the navTag. In this case,
	/// a non-null pointer is passed as costMultiplier parameter in order to update it according to the navTag if needed.
	/// It is a value initialized at 1.0f that will be multiplied by the default 3D-distance.
	/// By setting costMultiplier values above 1.0, you can make your Bot less likely to plan and follow paths that cross that
	/// navTag. Similarly, by returning values less than 1.0, you can make your Bot prefer traveling through this navTag
	/// over traveling through other navTag with lower cost multiplier.
	/// Warning : Null or negative cost multipliers are not supported!
	/// Warning : if a costMultiplier if not requested, costMultiplier parameter is null !
	static KY_INLINE bool CanTraverseNavTag(void* /*traverseLogicUserData*/, const NavTag& /*navTag*/, KyFloat32* /*costMultiplier*/) { return true; }

	/// This method is called for each candidate node in the A* calculation. It is expected to determine the approximate cost of moving
	/// the specified distance toward the final destination. You can simply return the distance3DToDestination parameter. 
	/// This method is only used by the AStarQuery. You do not have to provide an implementation for other kinds of queries that propagate
	/// only through the NavMesh, such as the RayCastQuery. This method is not called during path following. 
	static KY_INLINE KyFloat32 GetHeuristicFromDistanceToDest(void* /*traverseLogicUserData*/, KyFloat32 distance3DToDestination) { return distance3DToDestination; }

	/// This method is only required when you set up your customizer to validate NavTag transitions. If you use the default
	/// DoNotUseCanEnterNavTag template parameter, you do not need to provide an implementation of CanEnterNavTag().
	static KY_INLINE bool CanEnterNavTag(void* /*traverseLogicUserData*/, const NavTag& /*exitNavTag*/, const NavTag& /*enterNavTag*/,
		const Vec3f& /*pos*/) { return true; }

private:
	// ----- Obsolete Functions -----
	// These functions are now obsolete, they were part of the SimpleTraverseLogic interface.
	// You should remove them from your inheriting TraverseLogic class, 
	// and implement __only__ the public functions above.
	// Implementations of such functions in inheriting classes can be caught at runtime when adding a NavigationProfile templated by your TraverseLogic class to the World.
	// Or you can call TraverseLogicObsoleteChecker::HasNoObsoleteFunctions<YourTraverseLogic>()
};



/// TraverseLogicWithCostMultiplerPerNavTag defines all the functions that are potentially called in
/// the PathFinding and PathFollowing system when there the cost need to be customized according to the navTag.
///
/// WARNING : This is for advanced usage only : The tesselation of you navMesh should be dense enough to get a correct behavior.
///
template <class TCanEnterNavTagMode>
class TraverseLogicWithCostPerTriangle
{
	KY_TRAVERSE_LOGIC(TraverseLogicWithCostPerTriangle, LogicWithCostMultiplerPerTriangle, TCanEnterNavTagMode)
public:
	/// These method are called each time the path finding query, path following system or any other query considers crossing into a
	/// different triangle of NavMesh or edge of the NavGraph. It is expected to determine whether or not the path is allowed to cross
	/// it by returning true if it is traversable and false if it is not traversable.
	/// These functions can also be called in a context that request a cost multiplier associated to the triangle/edge. In this case,
	/// a non-null pointer is passed as costMultiplier parameter in order to update it according to the triangle or edge if needed.
	/// It is a value initialized at 1.0f that will be multiplied by the default 3D-distance.
	/// By setting costMultiplier values above 1.0, you can make your Bot less likely to plan and follow paths that cross that
	/// navTag. Similarly, by returning values less than 1.0, you can make your Bot prefer traveling through this navTag
	/// over traveling through other triangle with lower cost multiplier.
	/// Warning : Null or negative cost multipliers are not supported!
	/// Warning : if a costMultiplier if not requested, costMultiplier parameter is null !
	static KY_INLINE bool CanTraverseNavTriangle (void* /*traverseLogicUserData*/,  const NavTriangleRawPtr&  /*triangle*/, KyFloat32* /*costmultiplier*/) { return true; }
	static KY_INLINE bool CanTraverseNavGraphEdge(void* /*traverseLogicUserData*/, const NavGraphEdgeRawPtr& /*graphEdge*/, KyFloat32* /*costMultiplier*/) { return true; }

	/// This method is called for each candidate node in the A* calculation. It is expected to determine the approximate cost of moving
	/// the specified distance toward the final destination. You can simply return the distance3DToDestination parameter. 
	/// This method is only used by the AStarQuery. You do not have to provide an implementation for other kinds of queries that propagate
	/// only through the NavMesh, such as the RayCastQuery. This method is not called during path following. 
	static KY_INLINE KyFloat32 GetHeuristicFromDistanceToDest(void* /*traverseLogicUserData*/, KyFloat32 distance3DToDestination) { return distance3DToDestination; }

	/// This method is only required when you set up your customizer to validate NavTag transitions. If you use the default
	/// DoNotUseCanEnterNavTag template parameter, you do not need to provide an implementation of CanEnterNavTag().
	static KY_INLINE bool CanEnterNavTag(void* /*traverseLogicUserData*/, const NavTag& /*exitNavTag*/, const NavTag& /*enterNavTag*/,
		const Vec3f& /*pos*/) { return true; }

private:
	// ----- Obsolete Functions -----
	// These functions are now obsolete, they were part of the SimpleTraverseLogic interface.
	// You should remove them from your inheriting TraverseLogic class, 
	// and implement __only__ the public functions above.
	// Implementations of such functions in inheriting classes can be caught at runtime when adding a NavigationProfile templated by your TraverseLogic class to the World.
	// Or you can call TraverseLogicObsoleteChecker::HasNoObsoleteFunctions<YourTraverseLogic>()
};

// LocalCostAwareTraverseLogic -
// This traverse logic is build on top of the customer traverse logic. It used by the avoidance and target on path computer processes
// To determine which areas the bot can shortcut or avoid into.
struct LocalCostAwareTraverseLogicData
{
public:
	void*                  m_userTraverseLogicData;
	KyArray<const NavTag*> m_allowedNavTagForTraversal;
};

template <class TraverseLogic>
class LocalCostAwareTraverseLogic : public TraverseLogic
{
public:
	static KY_INLINE bool CanTraverseNavTag(void* traverseLogicUserData, const NavTag& navTag) 
	{ 
		LocalCostAwareTraverseLogicData* customTraverseLogicData = (LocalCostAwareTraverseLogicData*)traverseLogicUserData;
		if( customTraverseLogicData->m_allowedNavTagForTraversal.GetCount() == 0)
			return true;

		for(KyUInt32 i=0; i < customTraverseLogicData->m_allowedNavTagForTraversal.GetCount(); i++)
		{
			const NavTag* validNavTag = customTraverseLogicData->m_allowedNavTagForTraversal[i];
			if( navTag == *validNavTag )
				return true;
		}
		return false; 
	}

	static KY_INLINE bool CanTraverseNavTag(void* traverseLogicUserData, const NavTag& navTag, KyFloat32* costMultiplier) 
	{ 
		if (costMultiplier == KY_NULL) 
			return CanTraverseNavTag(traverseLogicUserData, navTag); 

		LocalCostAwareTraverseLogicData* customTraverseLogicData = (LocalCostAwareTraverseLogicData*)traverseLogicUserData;
		bool defaultResult = TraverseLogic::CanTraverseNavTag(customTraverseLogicData->m_userTraverseLogicData, navTag, costMultiplier);
		if(defaultResult == false)
			return false;

		if( customTraverseLogicData->m_allowedNavTagForTraversal.GetCount() == 0)
			return true;

		// We compare this navtag cost with the ones collected on the path
		// If it is below the cost, then we can go
		KyFloat32 currentNavTagcost = (*costMultiplier);
		KyFloat32 allowedNavTagcost = currentNavTagcost;
		for(KyUInt32 i=0; i < customTraverseLogicData->m_allowedNavTagForTraversal.GetCount(); i++)
		{
			const NavTag* validNavTag = customTraverseLogicData->m_allowedNavTagForTraversal[i];
			TraverseLogic::CanTraverseNavTag(customTraverseLogicData->m_userTraverseLogicData, *validNavTag, &allowedNavTagcost);
			if(currentNavTagcost<=allowedNavTagcost)
				return true;
		}

		return false;
	}

	static KY_INLINE bool CanTraverseNavTriangle (void* traverseLogicUserData,  const NavTriangleRawPtr&  triangle, KyFloat32* costmultiplier) 
	{ 
		LocalCostAwareTraverseLogicData* customTraverseLogicData = (LocalCostAwareTraverseLogicData*)traverseLogicUserData;
		return TraverseLogic::CanTraverseNavTriangle(customTraverseLogicData->m_userTraverseLogicData, triangle, costmultiplier);
	}

	static KY_INLINE bool CanTraverseNavGraphEdge(void* traverseLogicUserData, const NavGraphEdgeRawPtr& graphEdge, KyFloat32* costMultiplier) 
	{ 
		LocalCostAwareTraverseLogicData* customTraverseLogicData = (LocalCostAwareTraverseLogicData*)traverseLogicUserData;
		return TraverseLogic::CanTraverseNavGraphEdge(customTraverseLogicData->m_userTraverseLogicData, graphEdge, costMultiplier);
	}

	static KY_INLINE KyFloat32 GetHeuristicFromDistanceToDest(void* traverseLogicUserData, KyFloat32 distance3DToDestination) 
	{ 
		LocalCostAwareTraverseLogicData* customTraverseLogicData = (LocalCostAwareTraverseLogicData*)traverseLogicUserData;
		return TraverseLogic::GetHeuristicFromDistanceToDest(customTraverseLogicData->m_userTraverseLogicData, distance3DToDestination); 
	}

	static KY_INLINE bool CanEnterNavTag(void* traverseLogicUserData, const NavTag& exitNavTag, const NavTag& enterNavTag, const Vec3f& pos) 
	{ 
		LocalCostAwareTraverseLogicData* customTraverseLogicData = (LocalCostAwareTraverseLogicData*)traverseLogicUserData;
		return TraverseLogic::CanEnterNavTag(customTraverseLogicData->m_userTraverseLogicData, exitNavTag, enterNavTag, pos); 
	}
};



}

#endif

