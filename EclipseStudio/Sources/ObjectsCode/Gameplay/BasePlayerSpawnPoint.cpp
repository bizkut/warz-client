#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "BasePlayerSpawnPoint.h"

#include "multiplayer/P2PMessages.h"

IMPLEMENT_CLASS(BasePlayerSpawnPoint, "BasePlayerSpawnPoint", "Object");
//AUTOREGISTER_CLASS(BasePlayerSpawnPoint);

BasePlayerSpawnPoint::BasePlayerSpawnPoint()
{
	serializeFile = SF_ServerData;

	spawnType_ = SPAWN_NEUTRAL;
	m_NumSpawnPoints = 0;
}

BasePlayerSpawnPoint::~BasePlayerSpawnPoint()
{
}

void BasePlayerSpawnPoint::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node cpNode = node.child("baseControlPoint");
	spawnType_ = cpNode.attribute("spawnType").as_int();

	m_NumSpawnPoints = cpNode.attribute("numSpawnPoints").as_int();
	r3d_assert(m_NumSpawnPoints >=0 && m_NumSpawnPoints <= MAX_SPAWN_POINTS);
	if(m_NumSpawnPoints == 0) // add default
	{
		m_NumSpawnPoints = 1;
		m_SpawnPoints[0].pos = GetPosition()+r3dPoint3D(2,0,2);
		m_SpawnPoints[0].dir = 0;
	}
	else
	{
		char tempStr[32];
		for(int i=0; i<m_NumSpawnPoints; ++i)
		{
			sprintf(tempStr, "point%d", i);
			pugi::xml_node spawnNode = cpNode.child(tempStr);
			r3d_assert(!spawnNode.empty());
			m_SpawnPoints[i].pos.x = spawnNode.attribute("x").as_float();
			m_SpawnPoints[i].pos.y = spawnNode.attribute("y").as_float();
			m_SpawnPoints[i].pos.z = spawnNode.attribute("z").as_float();
			m_SpawnPoints[i].dir   = spawnNode.attribute("dir").as_float();
		}
	}
}

void BasePlayerSpawnPoint::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node cpNode = node.append_child();
	cpNode.set_name("baseControlPoint");
	cpNode.append_attribute("spawnType") = spawnType_;
	cpNode.append_attribute("numSpawnPoints") = m_NumSpawnPoints;
	for(int i=0; i<m_NumSpawnPoints; ++i)
	{
		pugi::xml_node spawnNode = cpNode.append_child();
		char tempStr[32];
		sprintf(tempStr, "point%d", i);
		spawnNode.set_name(tempStr);
		spawnNode.append_attribute("x") = m_SpawnPoints[i].pos.x;
		spawnNode.append_attribute("y") = m_SpawnPoints[i].pos.y;
		spawnNode.append_attribute("z") = m_SpawnPoints[i].pos.z;
		spawnNode.append_attribute("dir") = m_SpawnPoints[i].dir;
	}
}

//////////////////////////////////////////////////////////////////////////

r3dBoundBox BasePlayerSpawnPoint::node_s::GetDebugBBox() const
{
	r3dBoundBox bb;
	r3dVector size(4.2f, 0.4f, 4.2f);
	bb.Org = pos - size * 0.5f;
	bb.Size = size;
	return bb;
}