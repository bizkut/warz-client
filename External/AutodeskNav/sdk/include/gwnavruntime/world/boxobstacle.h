/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_BoxObstacle_H
#define Navigation_BoxObstacle_H


#include "gwnavruntime/world/worldelement.h"
#include "gwnavruntime/math/box3f.h"
#include "gwnavruntime/math/transform.h"
#include "gwnavruntime/spatialization/spatializedcylinder.h"
#include "gwnavruntime/world/tagvolume.h"
#include "gwnavruntime/dynamicnavmesh/tagvolumetrigger.h"


namespace Kaim
{

class DatabaseBinding;


/// Enumerates the possible controls for how a BoxObstacle should be internally
/// represented.
enum BoxObstacleRotationMode
{
	/// Used for obstacles that are moving mainly horizontally (such as cars,
	/// doors, etc.). Gameware Navigation adopts a more precise representation
	/// for such obstacles.
	BoxObstacleRotation_Yaw = 0,

	/// Used for all obstacles which are freely rotating (for example, concrete
	/// blocs just after the explosion of a building). Gameware Navigation uses
	/// a raw global bounding cylinder to represent such obstacles.
	BoxObstacleRotation_Free
};


/// Enumerates the possible up-axis definition for a BoxObstacle. This determines
/// the sampling of yaw-rotating BoxObstacles.
enum BoxObstacleUpAxis
{
	BoxObstacleUp_X = 0,
	BoxObstacleUp_Minus_X,
	BoxObstacleUp_Y,
	BoxObstacleUp_Minus_Y,
	BoxObstacleUp_Z,
	BoxObstacleUp_Minus_Z
};


/// Class used to provide BoxObstacle initialization parameters.
class BoxObstacleInitConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_BoxObstacle)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	BoxObstacleInitConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults()
	{
		m_world = KY_NULL;
		m_databaseBinding = KY_NULL;
		m_navTag.Clear();
		m_navTag.SetAsExclusive();
		m_localCenter.Set(0.0f, 0.0f, 0.0f);
		m_localHalfExtents.Set(0.0f, 0.0f, 0.0f);
		m_startPosition.Set(0.0f, 0.0f, 0.0f);
		m_userData = KY_NULL;
		m_rotationMode = BoxObstacleRotation_Free;
		m_upAxis = BoxObstacleUp_Z;
	}


	// ---------------------------------- Public Data Members ----------------------------------

	/// Mandatory: you must provide a World when calling BoxObstacle::Init.
	World* m_world;

	/// Defines the Databases in which the BoxObstacle will be spatialized. If let
	/// to KY_NULL, World's default DatabaseBinding will be used (spatializes on
	/// all Databases).
	Ptr<DatabaseBinding> m_databaseBinding;

	/// This DynamicNavTag will be used to set up the TagVolume NavTag when the
	/// BoxObstacle will trigger it.
	DynamicNavTag m_navTag;

	/// Defines the local AABBox center relatively to rotation axis. This should
	/// be let to Vec3f::Zero(): your physics engine should provide the velocity
	/// of your box center.
	/// When not using physics engine and facing an obstacle rotating around a
	/// static axis (like a door), you can set it to ease the BoxObstacleUpdateConfig
	/// computation.
	Vec3f m_localCenter;

	Vec3f m_localHalfExtents;
	Vec3f m_startPosition;

	void* m_userData;

	BoxObstacleRotationMode m_rotationMode;

	/// Used only for BoxObstacles that are yaw-rotating. It defines the local
	/// axis direction around which the BoxObstacle's yaw rotation is applied.
	BoxObstacleUpAxis m_upAxis;
};


/// Class used to provide BoxObstacle new position, orientation and velocities
/// to be integrated at next update.
class BoxObstacleUpdateConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_BoxObstacle)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	BoxObstacleUpdateConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults()
	{
		m_transform.SetIdentity();
		m_linearVelocity.Set(0.0f, 0.0f, 0.0f);
		m_angularVelocity.Set(0.0f, 0.0f, 0.0f);
	}


	// ---------------------------------- Public Data Members ----------------------------------

	Transform m_transform;

	/// The linear velocity, expressed in World coordinate system, at the local
	/// transform center of the box.
	Vec3f m_linearVelocity;

	/// Stores the rotational speed as rotation vector expressed in World coordinate
	/// system. This should provides at once rotation axis and angular speed (in
	/// radians per second).
	Vec3f m_angularVelocity;
};


/// Each instance of the BoxObstacle class represents a dynamic, physical object
/// in your game engine that will prevent your Bots from moving freely through
/// its bounding box. Typical examples are crates, furniture, movable or
/// destructible walls, rubble, vehicles, doors, etc.
///
/// By default BoxObstacles are integrated in avoidance trajectory computation
/// through their bounding cylinders. If your game obstacle does not move, you
/// can set DoesTriggerTagVolume to true to make Gameware Navigation spawn and
/// manage a TagVolume around the BoxObstacle that will be integrated into the
/// DynamicNavMesh.
class BoxObstacle: public WorldElement
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_BoxObstacle)
	KY_CLASS_WITHOUT_COPY(BoxObstacle)

public:
	static  WorldElementType GetStaticType() { return TypeBoxObstacle; }
	virtual WorldElementType GetType() const { return TypeBoxObstacle; }

	virtual const char*      GetTypeName() const { return "BoxObstacle"; }


	// ---------------------------------- Main API Functions ----------------------------------

	BoxObstacle();
	BoxObstacle(const BoxObstacleInitConfig& initConfig);

	virtual ~BoxObstacle();

	/// Sets m_world and allocates memory depending on m_databaseBinding; it 
	/// does not add the obstacle to the world, and does not modify the world.
	/// If m_world is already set, the obstacle is first removed from m_world.
	/// Asserts if initConfig.m_world == NULL, or if half extents are invalid.
	void Init(const BoxObstacleInitConfig& initConfig);

	/// Prepares this instance for destruction and / or reuse: sets all members
	/// to their default values and clears all SpatializedCylinders.
	void Clear();

	/// Adds the BoxObstacle to the World. Once added, it becomes active in the 
	/// simulation, is updated, taken into account by Bots, etc.
	/// This increments the RefCount of this instance.
	/// NOTE: this method has immediate effect: ensure to call this outside of any
	/// Gameware Navigation computation (World::Update() or threaded query
	/// computation). However, the spatialization is not updated immediately, this
	/// will be done in next World::Update().
	void AddToWorld();

	/// Removes the BoxObstacle from the World. It is removed from the
	/// simulation, updates are stopped, and it is ignored by Bots.
	/// This decrements the RefCount of this instance.
	/// NOTE: this method has immediate effect: ensure to call this outside of any
	/// Gameware Navigation computation (Kaim::World::Update or threaded query
	/// computation). The spatialization information are immediately invalidated too.
	void RemoveFromWorld();

	/// The BoxObstacle internals are not updated immediately: the provided
	/// BoxObstacleUpdateConfig is copied for use at the next World::Update, 
	/// after which the BoxObstacle internals are updated.
	void SetNextUpdateConfig(const BoxObstacleUpdateConfig& updateConfig);

	/// Sets the next rotation mode used to update this BoxObstacle.
	/// For BoxObstacleUpdateConfig, the BoxObstacleRotationMode is 
	/// copied and applied in the next World::Update.
	void SetRotationMode(BoxObstacleRotationMode mode);

	/// This information is copied and applied in the next World::Update.
	/// Setting to true will trigger a TagVolume for this BoxObstacle during the
	/// next World::Update. The TagVolume will then be integrated into the
	/// DynamicNavMesh, and the obstacle will be no longer be visible as sampled
	/// cylinders nor integrated into avoidance computation.
	/// While set to true, the position is not updated. If the obstacle triggering
	/// a TagVolume is moved, set to false, wait at least one frame to ensure the
	/// BoxObstacle is updated, then set it back to true once your obstacle is
	/// stationary again.
	void SetDoesTriggerTagVolume(bool triggerTagVolume);


	// ---------------------------------- Getters ----------------------------------

	const Vec3f&                   GetLocalCenter()                       const;
	const Vec3f&                   GetLocalHalfExtents()                  const;
	const BoxObstacleUpdateConfig& GetUpdateConfig()                      const;
	BoxObstacleRotationMode        GetRotationMode()                      const;
	const TagVolume*               GetTagVolume()                         const;
	const DynamicNavTag&           GetNavTag()                            const;
	KyUInt32                       GetSpatializedCylinderCount()          const;
	const SpatializedCylinder&     GetSpatializedCylinder(KyUInt32 index) const;
	const Transform&               GetTransform()                         const;
	TagVolume::IntegrationStatus   GetTagVolumeIntegrationStatus()        const;
	const DatabaseBinding*         GetDatabaseBinding()                   const;

	/// Returns whether the TriggerTagVolume flag is raised or not; not if the
	/// BoxObstacle has actually triggered a TagVolume. To get current TagVolume
	/// status, use GetTagVolumeIntegrationStatus() method.
	bool DoesTriggerTagVolume() const;

	/// Returns a short description of the rotation mode for visual debug
	static const char* GetRotationModeDescription(BoxObstacleRotationMode rotationMode);

private:
	class InternalUpdateConfig
	{
	public:
		InternalUpdateConfig() { SetDefaults(); }

		void SetDefaults()
		{
			m_updateConfig.SetDefaults();
			m_rotationMode = BoxObstacleRotation_Free;
			m_doesTriggerTagVolume = false;
		}

		BoxObstacleUpdateConfig m_updateConfig;
		BoxObstacleRotationMode m_rotationMode;
		bool m_doesTriggerTagVolume;
	};

	// Computes the SpatializedCylinder set that best fit the obstacle extents, in
	// local coordinates.
	// This is called each time the BoxObstacle is reinitialized, and the first time
	// the obstacle is updated with BoxObstacleRotation_Yaw flag.
	void ComputeSampledPoints();

	// Computes the SpatializedCylinder positions as sampled on a given plane.
	// Automatically called by ComputeSampledPoints once the third dimension is reduced 
	// according to the BoxObstacleUpAxis provided in BoxObstacleInitConfig.
	void ComputeSampledPoints_2D(const Vec3f& baseCenter, const Vec3f& axis0, const Vec3f& axis1,
		KyFloat32 halfExt0, KyFloat32 halfExt1, KyFloat32 sampledPointsHeight);

	// Computes the SpatializedCylinder positions as sampled along a given line.
	// Automatically called by ComputeSampledPoints_2D, once the second dimension is reduced 
	// accordingly to the wider 2D extent.
	void ComputeSampledPoints_1D(const Vec3f& baseCenter, const Vec3f& axis,
		KyFloat32 length, KyFloat32 width, KyFloat32 sampledPointsHeight);

	// Computes the global position and the velocity at a given local position
	// according to transform, and linear and angular velocities.
	void ComputePositionAndVelocity(const Vec3f& localPosition, Vec3f& newPosition, Vec3f& newVelocity);


public: // internal
	virtual void DoSendVisualDebug(VisualDebugServer& visualDebugServer, VisualDebugSendChangeEvent changeEvent); // Inherited from WorldElement

	// Updates the SpatializedCylinders positions and velocities according to
	// the Transform, and linear and angular velocities of this BoxObstacle; 
	// then computes the spatialization of the set to be used accordingly to update status.
	void UpdateSpatialization();

	// Sets up the TagVolume with its DynamicNavTag and the contour that best fits the obstacle.
	// This is called by TagVolumeTrigger when the BoxObstacle is updated with a 
	// DoesTriggerTagVolume set to true.
	void SetupTagVolumeInitConfig(TagVolumeInitConfig& tagVolumeInitConfig);

	// Called when the TagVolume integration is complete. It invalidates all Db spatializations.
	void OnTagVolumeIntegration();


	// NOTE: direct access to spatialization sets should not be used.
	// Use the non-internal public sets that provide information
	// on the SpatializedCylinder set currently in use according to
	// rotation mode.
	// They are used only in unit tests to validate they are well managed
	// according to the rotation mode and the triggerTagVolume flag.
	SpatializedCylinder& GetCentralSpatializedCylinder();
	KyUInt32             GetSampledSpatializedCylinderCount();
	SpatializedCylinder& GetSampledSpatializedCylinder(KyUInt32 index);
	TagVolume*           GetTagVolume();


private:
	TagVolumeTrigger<BoxObstacle> m_tagVolumeTrigger;
	DynamicNavTag m_navTag;

	Vec3f m_localCenter;
	Vec3f m_localHalfExtents;
	InternalUpdateConfig m_internalUpdateConfig;

	Ptr<DatabaseBinding> m_databaseBinding;
	SpatializedCylinder m_centralSpatializedCylinder;
	SpatializedCylinder* m_sampledSpatializedCylinders;
	Vec3f* m_sampledSpatializedCylinderLocalPositions;
	KyUInt32 m_sampledSpatializedCylinderCount;

	BoxObstacleUpAxis m_upAxis;
};

KY_INLINE const DynamicNavTag&           BoxObstacle::GetNavTag()                     const { return m_navTag;                                      }
KY_INLINE const Vec3f&                   BoxObstacle::GetLocalCenter()                const { return m_localCenter;                                 }
KY_INLINE const Vec3f&                   BoxObstacle::GetLocalHalfExtents()           const { return m_localHalfExtents;                            }
KY_INLINE const BoxObstacleUpdateConfig& BoxObstacle::GetUpdateConfig()               const { return m_internalUpdateConfig.m_updateConfig;         }
KY_INLINE BoxObstacleRotationMode        BoxObstacle::GetRotationMode()               const { return m_internalUpdateConfig.m_rotationMode;         }

KY_INLINE bool                           BoxObstacle::DoesTriggerTagVolume()          const { return m_internalUpdateConfig.m_doesTriggerTagVolume; }
KY_INLINE const TagVolume*               BoxObstacle::GetTagVolume()                  const { return m_tagVolumeTrigger.GetTagVolume();             }
KY_INLINE TagVolume::IntegrationStatus   BoxObstacle::GetTagVolumeIntegrationStatus() const { return m_tagVolumeTrigger.GetIntegrationStatus();     }
KY_INLINE const DatabaseBinding*         BoxObstacle::GetDatabaseBinding()            const { return m_databaseBinding;                             }

KY_INLINE KyUInt32 BoxObstacle::GetSpatializedCylinderCount()          const
{
	return (GetRotationMode() == BoxObstacleRotation_Yaw) ? m_sampledSpatializedCylinderCount : 1;
}
KY_INLINE const SpatializedCylinder& BoxObstacle::GetSpatializedCylinder(KyUInt32 index) const
{
	return (GetRotationMode() == BoxObstacleRotation_Yaw) ? m_sampledSpatializedCylinders[index] : m_centralSpatializedCylinder;
}

KY_INLINE const Transform& BoxObstacle::GetTransform() const { return m_internalUpdateConfig.m_updateConfig.m_transform; }

KY_INLINE SpatializedCylinder& BoxObstacle::GetCentralSpatializedCylinder()               { return m_centralSpatializedCylinder;         }
KY_INLINE KyUInt32             BoxObstacle::GetSampledSpatializedCylinderCount()          { return m_sampledSpatializedCylinderCount;    }
KY_INLINE SpatializedCylinder& BoxObstacle::GetSampledSpatializedCylinder(KyUInt32 index) { return m_sampledSpatializedCylinders[index]; }
KY_INLINE TagVolume*           BoxObstacle::GetTagVolume()                                { return m_tagVolumeTrigger.GetTagVolume();    }

} // Kaim

#endif // Navigation_BoxObstacle_H
