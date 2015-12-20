/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: GUAL
#ifndef Navigation_SpatializedCylinder_H
#define Navigation_SpatializedCylinder_H

#include "gwnavruntime/spatialization/spatializedpoint.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"


namespace Kaim
{

/// Class used to initialize a SpatializedCylinder.
/// DatabaseBinding is the set of Databases in which the SpatializedCylinder
/// will be projected.
class SpatializedCylinderInitConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Spatialization)
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	SpatializedCylinderInitConfig();

	/// Sets all members to their default value.
	void SetDefaults();

	void InitFromBot(Ptr<DatabaseBinding> databaseBinding, Bot* bot, KyFloat32 radius, KyFloat32 height);
	void InitFromCylinderObstacle(Ptr<DatabaseBinding> databaseBinding, CylinderObstacle* cylinderObstacle, KyFloat32 radius, KyFloat32 height);
	void InitFromBoxObstacle(Ptr<DatabaseBinding> databaseBinding, BoxObstacle* boxObstacle, KyUInt32 cylinderIndex, KyFloat32 radius, KyFloat32 height);


	// ---------------------------------- Public Data Members ----------------------------------

	SpatializedPointInitConfig m_spatializedPointInitConfig;
	KyFloat32 m_radius;
};


/// Internal representation of world elements, using a vertical cylinder shape. It
/// is used to localize moving cylindric elements, such as Bot or CylinderObstacle.
class SpatializedCylinder
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Spatialization)
	KY_CLASS_WITHOUT_COPY(SpatializedCylinder)

public:
	// ---------------------------------- Main API Functions ----------------------------------

	SpatializedCylinder();
	~SpatializedCylinder();

	void Init(const SpatializedCylinderInitConfig& initConfig);
	void Clear();


	// ---------------------------------- Getters ----------------------------------

	KyFloat32               GetRadius()           const;
	KyFloat32               GetHeight()           const; ///< Note: height is only used for visual debugging, it is not relevant for navigation.
	const Vec3f&            GetVelocity()         const;
	const Vec3f&            GetPosition()         const;
	const SpatializedPoint& GetSpatializedPoint() const;

	NavTrianglePtr GetNavTrianglePtr(Database* database) const;
	SpatializedPoint& GetSpatializedPoint();


	// ---------------------------------- Setters ----------------------------------

	void SetRadius(KyFloat32 radius);
	void SetHeight(KyFloat32 height);
	void SetVelocity(const Vec3f& velocity);
	void SetPosition(const Vec3f& position);


	// ---------------------------------- Spatialization Update Functions ----------------------------------

	bool UpdateSpatialization(const Vec3f& currentPosition);
	void InvalidateAllDbSpatializations();


private:
	Ptr<SpatializedPoint> m_spatializedPoint;
	Vec3f m_velocity;
	KyFloat32 m_radius;
};

KY_INLINE SpatializedCylinder::SpatializedCylinder() : m_velocity(Vec3f::Zero()) , m_radius(0.0f) {}
KY_INLINE SpatializedCylinder::~SpatializedCylinder() { Clear(); }

KY_INLINE void SpatializedCylinder::Clear()
{
	if (m_spatializedPoint != KY_NULL)
		m_spatializedPoint->Clear(); // Clear SpatializedPoint (sets all to default values and frees PointDbSpatializations memory)

	m_spatializedPoint = KY_NULL; // Decrement RefCount
}

// Getters
KY_INLINE KyFloat32               SpatializedCylinder::GetRadius()                           const { return m_radius;                                        }
KY_INLINE const Vec3f&            SpatializedCylinder::GetVelocity()                         const { return m_velocity;                                      }
KY_INLINE const Vec3f&            SpatializedCylinder::GetPosition()                         const { return m_spatializedPoint->GetPosition();               }
KY_INLINE const SpatializedPoint& SpatializedCylinder::GetSpatializedPoint()                 const { return *m_spatializedPoint;                             }
inline    NavTrianglePtr          SpatializedCylinder::GetNavTrianglePtr(Database* database) const { return m_spatializedPoint->GetNavTrianglePtr(database); }
KY_INLINE KyFloat32               SpatializedCylinder::GetHeight()                           const { return m_spatializedPoint->GetHeight();                 }

KY_INLINE SpatializedPoint& SpatializedCylinder::GetSpatializedPoint() { return *m_spatializedPoint; }

KY_INLINE void SpatializedCylinder::SetRadius(KyFloat32 radius)                        { m_radius = radius;                                         }
KY_INLINE void SpatializedCylinder::SetVelocity(const Vec3f& velocity)                 { m_velocity = velocity;                                     }
KY_INLINE void SpatializedCylinder::SetPosition(const Vec3f& position)                 { m_spatializedPoint->SetPosition(position);                 }
KY_INLINE void SpatializedCylinder::SetHeight(KyFloat32 height)                        { m_spatializedPoint->SetHeight(height);                     }
KY_INLINE bool SpatializedCylinder::UpdateSpatialization(const Vec3f& currentPosition) { return m_spatializedPoint->UpdateSpatialization(currentPosition); }
KY_INLINE void SpatializedCylinder::InvalidateAllDbSpatializations()                   { m_spatializedPoint->InvalidateAllDbSpatializations();      }



KY_INLINE SpatializedCylinderInitConfig::SpatializedCylinderInitConfig() { SetDefaults(); }
KY_INLINE void SpatializedCylinderInitConfig::SetDefaults()
{
	m_spatializedPointInitConfig.SetDefaults();
	m_radius = 0.0f;
}

KY_INLINE void SpatializedCylinderInitConfig::InitFromBot(Ptr<DatabaseBinding> databaseBinding, Bot* bot, KyFloat32 radius, KyFloat32 height)
{
	m_spatializedPointInitConfig.InitFromBot(databaseBinding, bot, height);
	m_radius = radius;
}

KY_INLINE void SpatializedCylinderInitConfig::InitFromCylinderObstacle(Ptr<DatabaseBinding> databaseBinding, CylinderObstacle* cylinderObstacle, KyFloat32 radius, KyFloat32 height)
{
	m_spatializedPointInitConfig.InitFromCylinderObstacle(databaseBinding, cylinderObstacle, height);
	m_radius = radius;
}

KY_INLINE void SpatializedCylinderInitConfig::InitFromBoxObstacle(Ptr<DatabaseBinding> databaseBinding, BoxObstacle* boxObstacle, KyUInt32 cylinderIndex, KyFloat32 radius, KyFloat32 height)
{
	m_spatializedPointInitConfig.InitFromBoxObstacle(databaseBinding, boxObstacle, cylinderIndex, height);
	m_radius = radius;
}

} // namespace Kaim

#endif // Navigation_SpatializedCylinder_H
