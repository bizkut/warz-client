/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavCell_H
#define Navigation_NavCell_H

#include "gwnavruntime/navmesh/navmeshelement.h"
#include "gwnavruntime/containers/collection.h"
#include "gwnavruntime/navmesh/navfloor1to1stitchdata.h"

namespace Kaim
{

class ActiveNavFloorCollection;
class NavFloorPtr;
class NavFloor;
class Database;
class DynamicNavMeshQuery;
class StaticToDynamicHalfEdge;

/// This class is a runtime wrapper of a NavCellBlob.
/// NavCells are managed by the NavCellGrid. This is the NavMesh unit it term of activation - Deactivation in term of sector
///  and overlaps (using the parent NavMeshElement GuidCompound).
/// It maintains, for each NavFloorBlob, an instance of NavFloor. It also maintains dynamic versions of NavFloor if present.
class NavCell
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
	KY_CLASS_WITHOUT_COPY(NavCell)
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	const CellPos&      GetCellPos()                      const; ///< Retrieves the position of this NavCell within the grid of NavData cells. 
	KyUInt32            GetNavFloorCount()                const; ///< Retrieves the number of NavFloors in this NavCell. 
	NavFloor*           GetNavFloor(NavFloorIdx floorIdx) const; ///< Retrieves the NavFloor with the specified index. 
	const GuidCompound& GetGuidCompound()                 const; ///< Retrieves the GuidCompound that identifies the NavMeshElement that contains this NavCell. 
	NavMeshElement*     GetParentNavMeshElement()         const; ///< Retrieves a pointer to the NavMeshElement that contains this NavCell. 

	bool IsNavCellBlobEmpty() const; ///< Returns true if the NavCellBlob contains no NavFloors and triangles. 

	/// Indicates whether or not this NavCell has been considered active by the NavCellGrid in term of overlap and GuidCompound. If so,
	/// all the NavFloor (dynamic if present, static otherwise) of this NavCell are accessible from ActiveData and are used by the bots, queries, etc...
	bool IsNavCellActive() const;

public: // Internal

	enum ActivenessStatus
	{
		NOT_PROCESSED ,
		ACTIVE        ,
		INACTIVE      ,
		REMOVED
	};

	NavCell(const NavCellBlob& navCellBlob);
	~NavCell();

	void OnRemove();
	void OnDeActivate();

	static KyUInt32 ComputeSizeForNavCell(const NavCellBlob& navCellBlob);
	static NavCell* PlaceNavCellInBuffer(const BlobRef<NavCellBlob>& navCellRef, Database* database, NavMeshElement* navMeshElement, char*& currentMemoryPlace);

	KyUInt32 GetIndexInCollection() const;
	void SetIndexInCollection(KyUInt32 indexInCollection);

	NavFloor* GetStaticNavFloorVersion(NavFloorIdx floorIdx) const;
	void RestoreStaticNavFloorAsActive(NavFloorIdx floorIdx);
	void SetNewDynamicVersionOfNavFloor(NavFloorIdx floorIdx, BlobHandler<NavFloorBlob>* navFloorBlobHandler);


	ActivenessStatus GetActivenessStatus() const;
	ActivenessStatus GetNextActivenessStatus() const;
	void SetActivenessStatus(ActivenessStatus status);
	void SetNextActivenessStatus(ActivenessStatus status);

	class MetaNavFloor
	{
	public:
		MetaNavFloor();
		~MetaNavFloor();
		Ptr<NavFloor> m_staticNavFloor;                      ///< version of NavFloor coming from generation.
		Ptr<NavFloor> m_activeNavFloor;                      ///< NavFloor that is currently active (can be static version or a dynamic version). Do not modify.
		Ptr<BaseBlobHandler> m_blobHandlerOfDynamicNavFloor; ///< Handle the dynamic NavFloorBlob Memory. Do not modify.
		NavFloor1To1StitchData m_floor1To1StitchData;
	};

	const NavFloorBlob* GetFutureActiveNavFloorBlob(KyUInt32 floorIdx);
public: // Internal
	NavMeshElement* m_parentNavMeshElement;               ///< The NavMeshElement that contains this NavCell. Do not modify.
	const NavCellBlob* m_navCellBlob;                     ///< The NavCellBlob that contains the static data for this NavCell. Do not modify.

	MetaNavFloor* m_metaNavFloorBuffer;

	Ptr<DynamicNavMeshQuery> m_dynamicNavMeshQuery; // The dynamicNavMesh query that is running or that has finished on this Cell. Keep it to retrieve its result when computing run-time stitch

	KyUInt32 m_navCellIdxInNavMesh; ///< The index of the NavCell within the buffer of NavCell in its NavMeshElement. Equal to the index of NavCellBlob within the NavMeshElementBlob. Do not modify.
	KyUInt16 m_idxOfNavCellVersionCollection; ///< the idx of this NavCell in the SmallPtrTrackedCollection located at this CellPos in the NavCellGrid. For internal use. Do not modify.
	KyUInt16 m_numberOfFloors; ///< The total number of NavFloors in #m_activeNavFloors. never change. Do not modify.
	KyUInt16 m_numberOfActiveDynamicFloors; ///< The number of dynamicNavFloors in the #m_activeNavFloors buffer. Do not modify.
	KyUInt8 m_currentActivenessStatus; ///< Current ActivenessStatus. Do not modify.
	KyUInt8 m_nextActivenessStatus; ///< this is a temporary ActivenessStatus value used by the NavCellGrid when updating the database. For Internal Use. Do not modify.
	bool m_navFloorsAreStitched; ///< Indicates whether or not the NavFloors in this NavCell have been stitched into a Database. For Internal Use. Do not modify.
	bool m_mustLaunchRuntimStitchQuery;
};


KY_INLINE const GuidCompound& NavCell::GetGuidCompound()                              const { return m_parentNavMeshElement->GetGuidCompound();       }
KY_INLINE NavMeshElement*     NavCell::GetParentNavMeshElement()                      const { return m_parentNavMeshElement;                          }
KY_INLINE const CellPos&      NavCell::GetCellPos()                                   const { return m_navCellBlob->GetCellPos();                     }
KY_INLINE KyUInt32            NavCell::GetNavFloorCount()                             const { return m_numberOfFloors;                                }
KY_INLINE NavFloor*           NavCell::GetNavFloor(NavFloorIdx floorIdx)              const { return m_metaNavFloorBuffer[floorIdx].m_activeNavFloor; }
KY_INLINE NavFloor*           NavCell::GetStaticNavFloorVersion(NavFloorIdx floorIdx) const { return m_metaNavFloorBuffer[floorIdx].m_staticNavFloor; }

KY_INLINE bool NavCell::IsNavCellBlobEmpty() const { return GetNavFloorCount() == 0;            }
KY_INLINE bool NavCell::IsNavCellActive()    const { return GetActivenessStatus() == ACTIVE; }

KY_INLINE KyUInt32 NavCell::GetIndexInCollection() const { return m_idxOfNavCellVersionCollection; }
KY_INLINE void NavCell::SetIndexInCollection(KyUInt32 indexInCollection) { m_idxOfNavCellVersionCollection = (KyUInt16)indexInCollection; }

KY_INLINE NavCell::ActivenessStatus NavCell::GetActivenessStatus()          const { return (ActivenessStatus)m_currentActivenessStatus; }
KY_INLINE NavCell::ActivenessStatus NavCell::GetNextActivenessStatus()      const { return (ActivenessStatus)m_nextActivenessStatus; }
KY_INLINE void NavCell::SetActivenessStatus(ActivenessStatus status)     { m_currentActivenessStatus = (KyUInt8)status; }
KY_INLINE void NavCell::SetNextActivenessStatus(ActivenessStatus status) { m_nextActivenessStatus = (KyUInt8)status; }

}

#endif //Navigation_NavCell_H

