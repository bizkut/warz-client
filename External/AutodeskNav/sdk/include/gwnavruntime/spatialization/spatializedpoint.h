/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: JUBA
#ifndef Navigation_SpatializedPoint_H
#define Navigation_SpatializedPoint_H


#include "gwnavruntime/world/databasebinding.h"
#include "gwnavruntime/navgraph/identifiers/navgraphedgerawptr.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/base/memory.h"

namespace Kaim
{

class World;
class DatabaseBinding;
class Database;
class WorldIntegerPos;
class NavTrianglePtr;

class Bot;
class CylinderObstacle;
class BoxObstacle;
class PointOfInterest;


/// Enumerates the possible object types the SpatializedPoint refers to.
enum SpatializedPointObjectType
{
	SpatializedPointObjectType_Undefined = 0,
	SpatializedPointObjectType_Bot,
	SpatializedPointObjectType_CylinderObstacle,
	SpatializedPointObjectType_BoxObstacle,
	SpatializedPointObjectType_PointOfInterest,

	SpatializedPointObjectType_Count
};


// INTERNAL CLASS: Point spatialization in one Database. The point is either:
// - in a NavMesh triangle
// - outside any NavData.
class PointDbSpatialization
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Spatialization)
public:
	PointDbSpatialization() {}

	void Invalidate();

	KyResult UpdateFromPrevious(Database* database, KyFloat32 height, const Vec3f& oldPosition, const PointDbSpatialization& oldDbSpatialization,
		const Vec3f& newPosition, WorldIntegerPos& newPositionInteger);

	void UpdateFromScratch(Database* database, KyFloat32 height, const Vec3f& newPosition, const WorldIntegerPos* newPositionInteger = KY_NULL);

public:
	NavTriangleRawPtr m_navTriangle;
};


/// Class used to initialize a SpatializedPoint.
class SpatializedPointInitConfig
{
public:
	// ---------------------------------- Main API Functions ----------------------------------

	SpatializedPointInitConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults();


	// ---------------------------------- Initialization ----------------------------------

	void InitFromBot(Ptr<DatabaseBinding> databaseBinding, Bot* bot, KyFloat32 height);
	void InitFromCylinderObstacle(Ptr<DatabaseBinding> databaseBinding, CylinderObstacle* cylinderObstacle, KyFloat32 height);
	void InitFromBoxObstacle(Ptr<DatabaseBinding> databaseBinding, BoxObstacle* boxObstacle, KyUInt32 indexInObject, KyFloat32 height);
	void InitFromPointOfInterest(Ptr<DatabaseBinding> databaseBinding, PointOfInterest* pointOfInterest, KyFloat32 height);


	// ---------------------------------- Public Data Members ----------------------------------

	/// Defines the Databases in which the SpatializedPoint will be spatialized.
	/// Mandatory: you must provide a DatabaseBinding to correctly initialize
	/// a SpatializedPoint.
	Ptr<DatabaseBinding> m_databaseBinding;

	void* m_object;
	SpatializedPointObjectType m_objectType;
	KyUInt32 m_indexInObject;

	KyFloat32 m_height;
};


/// Internal class used for point spatialization in NavData. All Objects that are
/// located in one NavTriangle or on one NavGraph edge per Database (ex: Bot) use
/// one instance of this class.
class SpatializedPoint: public RefCountBase<SpatializedPoint, MemStat_Spatialization>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Spatialization)
	KY_CLASS_WITHOUT_COPY(SpatializedPoint)

public:
	// ---------------------------------- Main API Functions ----------------------------------

	SpatializedPoint();
	~SpatializedPoint();

	void Init(const SpatializedPointInitConfig& initConfig);
	void Clear();
	void Invalidate();
	void InvalidateAllDbSpatializations();


	// ---------------------------------- Getters ----------------------------------

	const DatabaseBinding*           GetDatabaseBinding()                           const;
	      KyUInt32                   GetDbSpatializationCount()                     const;
	      PointDbSpatialization&     GetDbSpatialization(KyUInt32 boundDatabaseIdx)      ;
	const PointDbSpatialization&     GetDbSpatialization(KyUInt32 boundDatabaseIdx) const;
	const Vec3f&                     GetPosition()                                  const;
	      KyFloat32                  GetHeight()                                    const;
	      void*                      GetObject()                                    const;
	      SpatializedPointObjectType GetObjectType()                                const;
	      KyUInt32                   GetIndexInObject()                             const;

	NavTrianglePtr GetNavTrianglePtr(Database* database) const;


public: // internal
	// Just sets the position. Does not update any spatialization information.
	void SetPosition(const Vec3f& position);

	// Just sets the height. Does not update any spatialization information.
	void SetHeight(KyFloat32 height);

	// Computes new spatialization of PointDbSpatialization,
	// and updated NavFloors that contained and now contains the SpatializedPoint.
	// Returns Spatialization has changed, i.e. at least one PointDbSpatialization is spatialized differently
	bool UpdateSpatialization(const Vec3f& newPosition);

	// When a navFloor is deactivated, this function will invalidate all
	// references to it in this SpatializedPoint.
	void OnNavFloorDeActivation(NavFloor* navFloor);

	static const char* GetObjectTypeDescrition(SpatializedPointObjectType objectType);

private:
	void CreateDbSpatializationPoints();
	void DestroyDbSpatializationPoints();

	Vec3f m_position; // position last time the spatialization was updated, shared by all databases
	Ptr<DatabaseBinding> m_databaseBinding;
	PointDbSpatialization* m_dbSpatializationPoints; // array indexed like m_databaseBinding
	void* m_object;
	SpatializedPointObjectType m_objectType;
	KyUInt32 m_indexInObject;
	KyFloat32 m_height;
};


KY_INLINE void SpatializedPointInitConfig::SetDefaults()
{
	m_databaseBinding = KY_NULL;
	m_object = KY_NULL;
	m_objectType = SpatializedPointObjectType_Undefined;
	m_indexInObject = 0;
}

KY_INLINE void PointDbSpatialization::Invalidate()
{
	m_navTriangle.Invalidate();
}

KY_INLINE void SpatializedPointInitConfig::InitFromBot(Ptr<DatabaseBinding> databaseBinding, Bot* bot, KyFloat32 height)
{
	m_databaseBinding = databaseBinding;
	m_object = bot;
	m_objectType = SpatializedPointObjectType_Bot;
	m_indexInObject = 0;
	m_height = height;
}

KY_INLINE void SpatializedPointInitConfig::InitFromCylinderObstacle(Ptr<DatabaseBinding> databaseBinding, CylinderObstacle* cylinderObstacle, KyFloat32 height)
{
	m_databaseBinding = databaseBinding;
	m_object = cylinderObstacle;
	m_objectType = SpatializedPointObjectType_CylinderObstacle;
	m_indexInObject = 0;
	m_height = height;
}

KY_INLINE void SpatializedPointInitConfig::InitFromBoxObstacle(Ptr<DatabaseBinding> databaseBinding, BoxObstacle* boxObstacle, KyUInt32 indexInObject, KyFloat32 height)
{
	m_databaseBinding = databaseBinding;
	m_object = boxObstacle;
	m_objectType = SpatializedPointObjectType_BoxObstacle;
	m_indexInObject = indexInObject;
	m_height = height;
}

KY_INLINE void SpatializedPointInitConfig::InitFromPointOfInterest(Ptr<DatabaseBinding> databaseBinding, PointOfInterest* pointOfInterest, KyFloat32 height)
{
	m_databaseBinding = databaseBinding;
	m_object = pointOfInterest;
	m_objectType = SpatializedPointObjectType_PointOfInterest;
	m_indexInObject = 0;
	m_height = height;
}

KY_INLINE SpatializedPoint::SpatializedPoint()
	: m_databaseBinding(KY_NULL)
	, m_dbSpatializationPoints(KY_NULL)
	, m_object(KY_NULL)
	, m_objectType(SpatializedPointObjectType_Undefined)
	, m_indexInObject(0)
{}

KY_INLINE SpatializedPoint::~SpatializedPoint() { Clear(); }

KY_INLINE const DatabaseBinding*           SpatializedPoint::GetDatabaseBinding()                           const { return m_databaseBinding;                                   }
KY_INLINE       KyUInt32                   SpatializedPoint::GetDbSpatializationCount()                     const { return m_databaseBinding->GetBoundDatabaseCount();          }
KY_INLINE const Vec3f&                     SpatializedPoint::GetPosition()                                  const { return m_position;                                          }
KY_INLINE       KyFloat32                  SpatializedPoint::GetHeight()                                    const { return m_height;                                            }
KY_INLINE       void*                      SpatializedPoint::GetObject()                                    const { return m_object;                                            }
KY_INLINE       SpatializedPointObjectType SpatializedPoint::GetObjectType()                                const { return m_objectType;                                        }
KY_INLINE       KyUInt32                   SpatializedPoint::GetIndexInObject()                             const { return m_indexInObject;                                     }
KY_INLINE       PointDbSpatialization&     SpatializedPoint::GetDbSpatialization(KyUInt32 boundDatabaseIdx)       { return m_dbSpatializationPoints[boundDatabaseIdx]; }
KY_INLINE const PointDbSpatialization&     SpatializedPoint::GetDbSpatialization(KyUInt32 boundDatabaseIdx) const { return m_dbSpatializationPoints[boundDatabaseIdx]; }

KY_INLINE void SpatializedPoint::SetPosition(const Vec3f& position) { m_position = position; }
KY_INLINE void SpatializedPoint::SetHeight(KyFloat32 height)        { m_height = height;     }


} // namespace Kaim

#endif // Navigation_SpatializedPoint_H
