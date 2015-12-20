/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: GUAL
#ifndef Navigation_TriangleCostMap_H
#define Navigation_TriangleCostMap_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/database/activenavfloorcollection.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/navmesh/identifiers/navtrianglerawptr.h"
#include "gwnavruntime/navmesh/identifiers/navhalfedgerawptr.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/database/activedata.h"
#include "gwnavruntime/math/box2iiterator.h"
#include "gwnavruntime/database/stitchdatamanagerutils.h"
#include "gwnavruntime/querysystem/workingmemcontainers/trianglestatusingrid.h"
#include "gwnavruntime/queries/utils/breadthfirstsearchtraversal.h"

namespace Kaim
{

class ActiveNavFloorCollection;
class NavTriangleRawPtr;
class TriangleCostInCell;
class Box2f;
class Database;

class ITriangleCostMap : public RefCountBase<ITriangleCostMap, MemStat_CostMap>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_CostMap)
public:
	ITriangleCostMap();
	virtual ~ITriangleCostMap();

	// This function is called by the Database each time something changed in the cellBox
	// covered by the TriangleCostMap (NavData add/remove or dynamicNavmesh).
	// In Your implementation you have to called one of the InitCostMap function
	// before calling SetCostMultiplier on each triangle you want to set a custom costmultiplier
	// \Pre this triangleCostMap must have been bound to a Database first.
	virtual void Recompute() = 0;

protected:

	// ----- Helper Functions to be called in Recompute 

	/// This Function is an helper that browses all the triangles of all the 
	/// NavFloor whose bounding box intersects the input Box3f aabb.
	/// It allocates memory and, on each of these triangles, it calls the ComputeCostMultiplier
	/// function of the template argument object.
	/// CostMultiplierComputer is a class which must have this method :
	///   KyFloat32 ComputeCostMultiplier(const NavTriangleRawPtr& triangleRawPtr);
	template<class CostMultiplierComputer>
	void InitAndBrowseAllTrianglesInBox3f(CostMultiplierComputer& compute, const Box3f& aabb);

	/// This Function is an helper that runs a propagation from the input position to browse all
	/// the triangles that are reachable from the input position within a Volume. (by reachable we 
	/// mean connexity, not visibility).
	/// It allocates memory using the bounding Box of the volume you have to provide.
	/// The intersection tests of triangles against the volume during the propagation are performed
	/// using the template argument method 
	///   bool DoesTriangleIntersectVolume(const NavTriangleRawPtr& triangleRawPtr).
	/// On each traversed triangles, it calls the ComputeCostMultiplier function of the
	/// template argument object :
	/// KyFloat32 ComputeCostMultiplier(const NavTriangleRawPtr& triangleRawPtr)
	///
	/// CostMultiplierComputer is a class which must have these 2 methods :
	///   bool DoesTriangleIntersectVolume(const NavTriangleRawPtr& triangleRawPtr);
	///   KyFloat32 ComputeCostMultiplier(const NavTriangleRawPtr& triangleRawPtr);
	template<class CostMultiplierComputer>
	void InitAndPropagateInTrianglesFromPosInVolume(CostMultiplierComputer& compute, const Box3f& aabb, const Vec3f& inputPosition);

public:
	void BindToDatabase(Database* database);
	void UnbindFromDatabase();
	bool IsBindToADatabase() const { return m_database != KY_NULL; }

	void SetCostMultiplier(const NavTriangleRawPtr& triangle, KyFloat32 costMultiplier);
	KyFloat32 GetCostMultiplier(const NavTriangleRawPtr& triangle) const;

	const CellBox& GetCellBox() const { return m_cellBox; }

protected:
	// return true if the data structure is initialized and ready to use
	// returns false if the costMap was not bind to a database or if there is no triangle to set
	// a costMultipler to (that can happen if your box in outside the data !)
	bool InitCostMapForBox3f(const Box3f& aabb);

	// test if the navFloor intersects the Box3f
	bool DoesNavFloorIntersectBox3f(NavFloor* navFloor, const Box3f& aabb) const;

protected:
	Database* m_database;
	char* m_bufferOfCostsInCell;
	KyUInt32 m_sizeOfBuffer; /*< For internal use. Do not modify. */
	CellBox m_cellBox;
public: // Internal
	KyUInt32 ComputeBufferSizeForCellBox(KyUInt32 elementCountInBuffe) const;

	void InitFromNewCellBox();

	void InitBuffer(KyUInt32 newElementCount);

	void AllocateMemory(KyUInt32 newElementCount);
	void ReleaseMemory(); // Release the memory

	// for O(1) remove from database collections
	KyUInt32 GetIndexInCollection() const { return m_indexInCollection; }
	void SetIndexInCollection(KyUInt32 indexInCollection) { m_indexInCollection = indexInCollection; }
	KyUInt32 m_indexInCollection;         // for O(1) remove from database collections
};


}

#include "gwnavruntime/navmesh/itrianglecostmap.inl"

#endif

