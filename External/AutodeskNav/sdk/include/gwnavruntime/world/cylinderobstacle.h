/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: none
#ifndef Navigation_CylinderObstacle_H
#define Navigation_CylinderObstacle_H

#include "gwnavruntime/world/worldelement.h"
#include "gwnavruntime/spatialization/spatializedcylinder.h"
#include "gwnavruntime/dynamicnavmesh/tagvolumetrigger.h"


namespace Kaim
{

class World;
class DatabaseBinding;


/// Class used to provide CylinderObstacle initialization parameters.
class CylinderObstacleInitConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_CylinderObstacle)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	CylinderObstacleInitConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults()
	{
		m_world = KY_NULL;
		m_databaseBinding = KY_NULL;
		m_navTag.Clear();
		m_navTag.SetAsExclusive();
		m_startPosition.Set(0.0f, 0.0f, 0.0f);
		m_userData = KY_NULL;
		m_radius = 0.3f;
		m_height = 1.8f;
	}


	// ---------------------------------- Public Data Members ----------------------------------

	/// Mandatory: you must provide a World when calling CylinderObstacle::Init.
	World* m_world;

	/// Defines the Databases in which the CylinderObstacle will be spatialized. If let
	/// to KY_NULL, World's default DatabaseBinding will be used (spatializes in
	/// all Databases).
	Ptr<DatabaseBinding> m_databaseBinding;

	/// This DynamicNavTag will be used to set up the TagVolume NavTag when the
	/// CylinderObstacle will trigger it.
	DynamicNavTag m_navTag;

	Vec3f m_startPosition;
	void* m_userData;
	KyFloat32 m_radius;
	KyFloat32 m_height;
};


/// Class used to provide CylinderObstacle new position and velocity to be
/// integrated at the next update.
class CylinderObstacleUpdateConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_CylinderObstacle)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	CylinderObstacleUpdateConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults()
	{
		m_position.Set(0.0f, 0.0f, 0.0f);
		m_velocity.Set(0.0f, 0.0f, 0.0f);
	}


	// ---------------------------------- Public Data Members ----------------------------------

	Vec3f m_position;
	Vec3f m_velocity; ///< The CylinderObstacle velocity expressed in meter per seconds.
};


/// CylinderObstacles represent dynamic objects of your game engine that prevent
/// your Bots from moving freely in a vertical cylinder volume. This is especially
/// recommended for player representation. You can also use this for cylindric
/// obstacles that cannot tilt (if your obstacle can tilt, you must use a BoxObstacle).
///
/// By default CylinderObstacles are integrated in avoidance trajectory
/// computation. If your game obstacle does not move, you can set DoesTriggerTagVolume
/// to true to make Gameware Navigation spawn and manage a TagVolume around the
/// CylinderObstacle that will be integrated into the DynamicNavMesh.
class CylinderObstacle: public WorldElement
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_CylinderObstacle)
	KY_CLASS_WITHOUT_COPY(CylinderObstacle)

public:
	static  WorldElementType GetStaticType() { return TypeCylinderObstacle; }
	virtual WorldElementType GetType() const { return TypeCylinderObstacle; }

	virtual const char*      GetTypeName() const { return "CylinderObstacle"; }

	// ---------------------------------- Main API Functions ----------------------------------

	CylinderObstacle();
	CylinderObstacle(const CylinderObstacleInitConfig& initConfig);

	virtual ~CylinderObstacle();


	/// Sets m_world and allocates memory depending on m_databaseBinding but does
	/// not add the CylinderObstacle to the world.
	/// Does not modify the world.
	/// If m_world is already set, Clear() is called.
	/// Asserts if initConfig.m_world is NULL.
	void Init(const CylinderObstacleInitConfig& initConfig);

	/// Prepares this instance for destruction or reuse; it sets all members
	/// to their default values, and clears memory of PointSpatialization.
	void Clear();

	/// Adds the CylinderObstacle to the World: it becomes active in the simulation,
	/// it is updated, taken into account by Bots, etc., and it 
	/// increments the RefCount of this instance.
	/// NOTE: this method has immediate effect: ensure to call this outside of any
	/// Gameware Navigation computation (World::Update() or threaded query
	/// computation). However, the spatialization is not updated immediately, this
	/// will be done in next World::Update().
	void AddToWorld();

	/// Removes the CylinderObstacle from the World: it is removed from the
	/// simulation, it is no longer updated, it is ignored by Bots, and
	/// the RefCount of this instance is decremented by 1.
	/// NOTE: this method has immediate effect: ensure to call this outside of any
	/// Gameware Navigation computation (Kaim::World::Update or threaded query
	/// computation). The spatialization information are immediately invalidated too.
	void RemoveFromWorld();

	/// CylinderObstacle internals are not updated immediately: the provided
	/// CylinderObstacleUpdateConfig is copied for use at the next World::Update,
	/// after which the CylinderObstacle internals are updated.
	void SetNextUpdateConfig(const CylinderObstacleUpdateConfig& updateConfig);

	/// This information is copied and applied in the next World::Update.
	/// Setting to true will trigger a TagVolume for this CylinderObstacle during
	/// the next World::Update. The TagVolume will then be integrated into the
	/// DynamicNavMesh, and the obstacle will be no longer be integrated into
	/// avoidance computation.
	/// While set to true, the position is not updated. If the obstacle triggering
	/// a TagVolume is moved, set to false, wait at least one frame to ensure the
	/// CylinderObstacle is updated, then set it back to true once your obstacle is
	/// stationary again.
	void SetDoesTriggerTagVolume(bool triggerTagVolume);


	// ---------------------------------- Getters ----------------------------------

	const DynamicNavTag&         GetNavTag()                     const; ///
	const SpatializedCylinder&   GetSpatializedCylinder()        const; ///
	SpatializedCylinder&         GetSpatializedCylinder();              ///

	KyFloat32                    GetRadius()                     const; ///
	KyFloat32                    GetHeight()                     const; ///< Used only for visual debugging. Not relevant for navigation.
	const Vec3f&                 GetPosition()                   const; ///
	const Vec3f&                 GetVelocity()                   const; ///

	bool                         DoesTriggerTagVolume()          const; ///
	const TagVolume*             GetTagVolume()                  const; ///
	TagVolume::IntegrationStatus GetTagVolumeIntegrationStatus() const; ///

	const DatabaseBinding*       GetDatabaseBinding()            const; ///

	/// Returns the triangle this CylinderObstacle is lying based on its binding to the given database.
	NavTrianglePtr GetNavTrianglePtr(Database* database) const;


public: // internal
	virtual void DoSendVisualDebug(VisualDebugServer& server, VisualDebugSendChangeEvent changeEvent); // Inherited from WorldElement

	// Updates the SpatializedCylinder position and velocity with the data provided
	// in the CylinderObstacleUpdateConfig.
	// Computes then the spatialization of the SpatializedCylinder.
	void UpdateSpatialization();


	// Setup the TagVolume with its DynamicNavTag and the contour that best fits the obstacle.
	// This is called by TagVolumeTrigger when the CylinderObstacle is updated with
	// DoesTriggerTagVolume set to true.
	void SetupTagVolumeInitConfig(TagVolumeInitConfig& tagVolumeInitConfig);

	// Called when the TagVolume integration is complete. It invalidates all Db spatializations.
	void OnTagVolumeIntegration();


private:
	class InternalUpdateConfig
	{
	public:
		InternalUpdateConfig() { SetDefaults(); }

		void SetDefaults()
		{
			m_updateConfig.SetDefaults();
			m_doesTriggerTagVolume = false;
		}

		CylinderObstacleUpdateConfig m_updateConfig;
		bool m_doesTriggerTagVolume;
	};

	TagVolumeTrigger<CylinderObstacle> m_tagVolumeTrigger;
	DynamicNavTag m_navTag;
	InternalUpdateConfig m_internalUpdateConfig;
	Ptr<DatabaseBinding> m_databaseBinding;
	SpatializedCylinder m_spatializedCylinder;
};


KY_INLINE const DynamicNavTag&         CylinderObstacle::GetNavTag()                     const { return m_navTag;              }
KY_INLINE SpatializedCylinder&         CylinderObstacle::GetSpatializedCylinder()              { return m_spatializedCylinder; }
KY_INLINE const SpatializedCylinder&   CylinderObstacle::GetSpatializedCylinder()        const { return m_spatializedCylinder; }

KY_INLINE KyFloat32                    CylinderObstacle::GetRadius()                     const { return m_spatializedCylinder.GetRadius();   }
KY_INLINE KyFloat32                    CylinderObstacle::GetHeight()                     const { return m_spatializedCylinder.GetHeight();   }
KY_INLINE const Vec3f&                 CylinderObstacle::GetPosition()                   const { return m_spatializedCylinder.GetPosition(); }
KY_INLINE const Vec3f&                 CylinderObstacle::GetVelocity()                   const { return m_spatializedCylinder.GetVelocity(); }

KY_INLINE bool                         CylinderObstacle::DoesTriggerTagVolume()          const { return m_internalUpdateConfig.m_doesTriggerTagVolume; }
KY_INLINE const TagVolume*             CylinderObstacle::GetTagVolume()                  const { return m_tagVolumeTrigger.GetTagVolume();             }
KY_INLINE TagVolume::IntegrationStatus CylinderObstacle::GetTagVolumeIntegrationStatus() const { return m_tagVolumeTrigger.GetIntegrationStatus();     }

KY_INLINE const DatabaseBinding*       CylinderObstacle::GetDatabaseBinding()            const { return m_databaseBinding; }

KY_INLINE void CylinderObstacle::SetDoesTriggerTagVolume(bool doesTriggerTagVolume) { m_internalUpdateConfig.m_doesTriggerTagVolume = doesTriggerTagVolume; }

KY_INLINE void CylinderObstacle::OnTagVolumeIntegration()
{
	m_spatializedCylinder.InvalidateAllDbSpatializations();
	ForceSendAllVisualDebug();
}


KY_INLINE void CylinderObstacle::UpdateSpatialization()
{
	m_tagVolumeTrigger.Update(m_internalUpdateConfig.m_doesTriggerTagVolume);

	if (m_tagVolumeTrigger.GetIntegrationStatus() != TagVolume::IntegrationStatus_Integrated)
		m_spatializedCylinder.UpdateSpatialization(m_internalUpdateConfig.m_updateConfig.m_position);
	else
		m_spatializedCylinder.SetPosition(m_internalUpdateConfig.m_updateConfig.m_position);

	m_spatializedCylinder.SetVelocity(m_internalUpdateConfig.m_updateConfig.m_velocity);
	
	ForceSendAllVisualDebug();
}

} // namespace Kaim

#endif // Navigation_CylinderObstacle_H
