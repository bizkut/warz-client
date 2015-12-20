#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "SharedUsableItem.h"

SharedUsableItemList m_SharedUsableItemList;

SharedUsableItem::SharedUsableItem()
{
	ObjTypeFlags |= OBJTYPE_GameplayItem|OBJTYPE_SharedUsableItem;
	m_lastPickupAttempt = 0;
	m_StayStillTime = 0;
	m_NotSleepTime  = 0;
	m_ActionUI_Title = "NONE";
	m_ActionUI_Msg = "NONE";
	DisablePhysX = false;

	m_SharedUsableItemList.push_back(this);
}

SharedUsableItem::~SharedUsableItem()
{
	m_SharedUsableItemList.erase(std::find(m_SharedUsableItemList.begin(), m_SharedUsableItemList.end(), this));
}

BOOL SharedUsableItem::Update()
{

	if (m_IsAirDrop)
		return TRUE;

	// update underlying object first, so we'll get new object position from physx
	MeshGameObject::Update();

	// hack, piece of shit PhysX. CCD objects are not going into sleep.
	if(PhysicsObject && !PhysicsObject->IsSleeping() && !FirstUpdate && !DisablePhysX)
	{
		if((oldstate.Position - GetPosition()).Length() < 0.005f )
		{
			// we can't just sleep object here, as physx ticks isn't synced with game ticks
			// because of that, position might be not changed on current frame
			m_StayStillTime += r3dGetFrameTime();
			if(m_StayStillTime > 0.1f)
			{
				PhysicsObject->ForceToSleep();
			}
		}
		else
		{
			m_StayStillTime = 0;
		}

		// check if object was moving too far
		m_NotSleepTime += r3dGetFrameTime();
		if(!PhysicsObject->IsSleeping() && m_NotSleepTime > 5.0f)
		{
			// object is moving somewhere for too long.... reset to start position
			// need to somehow inform us about that object - maybe bad geometry there..
			SetPosition(m_spawnPos);
			PhysicsObject->ForceToSleep();
		}
	}

	// check if object for some reason fell through the ground or just fell too much (spawn point was placed in air?)
	{
		if((GetPosition() - m_spawnPos).Length() > 4.0f)
		{
			SetPosition(m_spawnPos);
			if(PhysicsObject)
				PhysicsObject->ForceToSleep();
		}
	}

	return TRUE;
}