#pragma once

#include "GameCommon.h"

class BasePlayerSpawnPoint : public MeshGameObject
{
	DECLARE_CLASS(BasePlayerSpawnPoint, MeshGameObject)

  public:
	enum type_e {
	  SPAWN_BASE, // when player spawns first time, he spawns at those points
	  SPAWN_NEUTRAL, // when player dies and respawns, he spawns to the closes of those points
	};
	int		spawnType_;
	
	struct node_s
	{
		r3dPoint3D	pos;
		float		dir;
		int			danger; // how much "danger" this point has in it
		float		lastTimeDangerUpdated;

		node_s():pos(0,0,0),dir(0.0f),danger(0),lastTimeDangerUpdated(0.0f) {}

		//	This function used only for debug visualization and selection purposes
		r3dBoundBox GetDebugBBox() const;
	};
	
	static const int MAX_SPAWN_POINTS = 16;
	node_s m_SpawnPoints[MAX_SPAWN_POINTS];
	int			m_NumSpawnPoints;

	void getSpawnPoint(r3dPoint3D& pos, float& dir) const
	{
		r3d_assert(m_NumSpawnPoints > 0 && m_NumSpawnPoints <= MAX_SPAWN_POINTS);
		int i = random(m_NumSpawnPoints-1);
		r3d_assert(i>=0 && i < MAX_SPAWN_POINTS);
		pos = m_SpawnPoints[i].pos;
		dir = m_SpawnPoints[i].dir;
	}

	void getSpawnPointByIdx(int index, r3dPoint3D& pos, float& dir) const
	{
		r3d_assert(m_NumSpawnPoints > 0 && m_NumSpawnPoints <= MAX_SPAWN_POINTS);
		r3d_assert(index>=0 && index < MAX_SPAWN_POINTS);
		pos = m_SpawnPoints[index].pos;
		dir = m_SpawnPoints[index].dir;
	}

  public:
	BasePlayerSpawnPoint();
	virtual ~BasePlayerSpawnPoint();
	
	virtual void		WriteSerializedData(pugi::xml_node& node);
	virtual	void		ReadSerializedData(pugi::xml_node& node);
};
