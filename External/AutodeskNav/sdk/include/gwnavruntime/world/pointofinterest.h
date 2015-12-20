/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: JUBA
#ifndef Navigation_PointOfInterest_H
#define Navigation_PointOfInterest_H


#include "gwnavruntime/spatialization/spatializedpoint.h"
#include "gwnavruntime/world/worldelement.h"


namespace Kaim
{

static const KyUInt32 PointOfInterestType_Undefined = 0; ///< The default value of PointOfInterest type.
static const KyUInt32 PointOfInterestType_FirstClient = 16; ///< The first customizable PointOfInterest type value.


/// Class used to provide PointOfInterest initialization parameters.
class PointOfInterestInitConfig
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	PointOfInterestInitConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults()
	{
		m_startPosition.Set(0.0f, 0.0f, 0.0f);
		m_world = KY_NULL;
		m_databaseBinding = KY_NULL;
		m_poiData = KY_NULL;
		m_poiType = PointOfInterestType_Undefined;
		m_height = 0.0f;
	}


	// ---------------------------------- Public Data Members ----------------------------------

	/// Mandatory: you must provide a World when calling CylinderObstacle::Init.
	World* m_world;

	/// Defines the Databases in which the CylinderObstacle will be spatialized. If let
	/// to KY_NULL, World's default DatabaseBinding will be used (spatializes on
	/// all Databases).
	Ptr<DatabaseBinding> m_databaseBinding;

	void* m_poiData;
	Vec3f m_startPosition;

	/// PointOfInterest might have an height. This increase the spatialization
	/// range accordingly upwards.
	/// \units           meters
	/// \defaultvalue    0
	KyFloat32 m_height;

	/// Defines the type of the PointOfInterest. The values lower than
	/// #PointOfInterestType_FirstClient are reserved to Gameware Navigation
	/// internal PointOfInterest types.
	KyUInt32 m_poiType;
};


/// Class used to provide PointOfInterest new position to be integrated at next update.
class PointOfInterestUpdateConfig
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	PointOfInterestUpdateConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults() { m_position.Set(0.0f, 0.0f, 0.0f); }


	// ---------------------------------- Public Data Members ----------------------------------

	Vec3f m_position;
};


/// PointOfInterest is a tagged position spatialized into the NavMesh. You can
/// use this for special interest positions that can be easily retrieved nearby
/// your Bots (they can be, for example, cover points).
/// This is purely runtime and dynamic: points of interest are spawned like the
/// other WorldElements (Bot, BoxObstacle, etc.), and can be moved as well.
class PointOfInterest: public WorldElement
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Spatialization)
	KY_CLASS_WITHOUT_COPY(PointOfInterest)

public:
	static  WorldElementType GetStaticType() { return TypePointOfInterest; }
	virtual WorldElementType GetType() const { return TypePointOfInterest; }

	virtual const char*      GetTypeName() const { return "PointOfInterest"; }


	// ---------------------------------- Main API Functions ----------------------------------

	PointOfInterest();
	PointOfInterest(const PointOfInterestInitConfig& initConfig);

	~PointOfInterest();


	/// Sets m_world and allocates memory depending on m_databaseBinding. It
	/// does not add the PointOfInterest to the world, and it does not modify the world.
	/// If m_world is already set, the PointOfInterest is first removed from m_world.
	/// Asserts if initConfig.m_world == NULL
	void Init(const PointOfInterestInitConfig& initConfig);

	/// Prepares this instance to destruction and / or reuse: sets all members
	/// to their default values and clears the SpatializedPoint.
	void Clear();

	/// Adds the PointOfInterest to the World. It becomes active in the simulation,
	/// it is updated, it is made available for retrieval through queries, and 
	/// the RefCount of this instance is incremented by 1.
	/// NOTE: this method has immediate effect: ensure to call this outside of any
	/// Gameware Navigation computation (World::Update() or threaded query
	/// computation). However, the spatialization is not updated immediately, this
	/// will be done in next World::Update().
	void AddToWorld();

	/// Removes the PointOfInterest from the World. It is removed from the
	/// simulation, it is no longer updated, it cannot be retrieved, and
	/// the RefCount of this instance is decremented by 1.
	/// NOTE: this method has immediate effect: ensure to call this outside of any
	/// Gameware Navigation computation (Kaim::World::Update or threaded query
	/// computation). The spatialization information are immediately invalidated too.
	void RemoveFromWorld();

	/// The PointOfInterest internals are not updated immediately: the provided
	/// PointOfInterestUpdateConfig is copied for use at the next World::Update,
	/// after which the PointOfInterest internals are updated.
	void SetNextUpdateConfig(const PointOfInterestUpdateConfig& updateConfig);


	// ---------------------------------- Getters ----------------------------------

	const SpatializedPoint& GetSpatializedPoint()    const;
	void*                   GetPointOfInterestData() const;
	KyUInt32                GetPointOfInterestType() const;


public: // internal
	virtual void DoSendVisualDebug(VisualDebugServer& server, VisualDebugSendChangeEvent changeEvent); // Inherited from WorldElement

	// Updates the SpatializedPoint position with the data provided
	// in the PointOfInterestUpdateConfig.
	// Computes then the spatialization of the SpatializedPoint.
	void UpdateSpatialization();


private:
	PointOfInterestUpdateConfig m_nextUpdateConfig;
	Ptr<SpatializedPoint> m_spatializedPoint;
	void* m_poiData;
	KyUInt32 m_poiType;
};


KY_INLINE PointOfInterest::PointOfInterest() : m_poiData(KY_NULL) , m_poiType(PointOfInterestType_Undefined) {}
KY_INLINE PointOfInterest::PointOfInterest(const PointOfInterestInitConfig& initConfig)
	: m_poiData(KY_NULL) , m_poiType(PointOfInterestType_Undefined)
{
	Init(initConfig);
}

KY_INLINE PointOfInterest::~PointOfInterest() { Clear(); }

KY_INLINE void PointOfInterest::SetNextUpdateConfig(const PointOfInterestUpdateConfig& updateConfig) { m_nextUpdateConfig = updateConfig; }

KY_INLINE const SpatializedPoint& PointOfInterest::GetSpatializedPoint()    const { return *m_spatializedPoint; }
KY_INLINE void*                   PointOfInterest::GetPointOfInterestData() const { return m_poiData;           }
KY_INLINE KyUInt32                PointOfInterest::GetPointOfInterestType() const { return m_poiType;           }

KY_INLINE void PointOfInterest::UpdateSpatialization()                     { m_spatializedPoint->UpdateSpatialization(m_nextUpdateConfig.m_position); ForceSendAllVisualDebug(); }

} // namespace Kaim

#endif // Navigation_PointOfInterest_H
