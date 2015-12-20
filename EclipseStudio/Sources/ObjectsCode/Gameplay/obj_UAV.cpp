#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "Gameplay_Params.h"

#include "multiplayer/P2PMessages.h"
#include "multiplayer/ClientGameLogic.h"
#include "ObjectsCode\AI\AI_Player.H"

#include "APIScaleformGfx.h"
#include "..\..\ui\HUDDisplay.h"

extern bool g_bEditMode;

extern HUDDisplay*	hudMain;

#include "obj_UAV.h"

IMPLEMENT_CLASS(obj_UAV, "obj_UAV", "Object");
AUTOREGISTER_CLASS(obj_UAV);

	// minX, maxX, minZ, maxZ, maxY
	float		uav_LevelBounds[5] = {-9999, 9999, -9999, 9999, 9999};

obj_UAV::obj_UAV() : 
	netMover(this, 0.1f, (float)PKT_C2C_MoveSetCell_s::UAV_CELL_RADIUS)
{
	ViewAngle   = r3dPoint3D(0, 0, 0);
	vVision     = r3dPoint3D(1, 0, 0);
	isDamaged   = 0;
	netVelocity = r3dPoint3D(0, 0, 0);

	tDrift[0]   = 2;
	tDrift[1]   = 2;
	vDrift[1]   = r3dPoint3D(0, 0, 0);
	ObjTypeFlags |= OBJTYPE_UAV;
	UaVisDestroy = false;
	state_ = UAV_Active;
}

obj_UAV::~obj_UAV()
{
}

BOOL obj_UAV::Load(const char *fname)
{
	const char* mesh = "Data\\ObjectsDepot\\Vehicles\\Drone.sco";
	const char* skel = "Data\\ObjectsDepot\\Vehicles\\Drone.skl";
	if(!parent::Load(mesh)) 
		r3dError("failed to get uav mesh %s", mesh);
		
	r3d_assert(m_BindSkeleton);

	return TRUE;
}

BOOL obj_UAV::OnCreate()
{
	ReadPhysicsConfig();

	// override some
	PhysicsConfig.group = PHYSCOLL_NETWORKPLAYER; // so that you can shoot your own UAV and also to make sure that it will not collide with player only geometry
	PhysicsConfig.isDynamic = TRUE;
	PhysicsConfig.isKinematic = NetworkLocal ? FALSE : TRUE;

	/*PhysicsConfig.type = PHYSICS_TYPE_MESH;
	PhysicsConfig.group = PHYSCOLL_COLLISION_GEOMETRY;
	PhysicsConfig.isDynamic = true;
	PhysicsConfig.isKinematic = false;*/
	//PhysicsConfig.isFastMoving = true;
	//PhysicsConfig.isVehicle = true;
	//PhysicsConfig.ready = true;

	parent::OnCreate();

	// load anim and use it
	int aid = m_AnimPool.Add("defaultanim", "Data\\ObjectsDepot\\Vehicles\\Animations\\Drone.anm");
	m_Animation.StartAnimation(aid, ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
	m_bAnimated = 1;
	
	// disable gravity
	if(NetworkLocal)
	{
		r3d_assert(PhysicsObject);
		PhysicsObject->getPhysicsActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	}
	
	ViewAngle.x = -GetRotationVector().x;
	netMover.Teleport(GetPosition());

	if (!SoundSys.isPlaying(UavSnd))
	{
		UavSnd = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/Vehicles/Drone"),GetPosition());
		SoundSys.Start(UavSnd);
	}
	
	return TRUE;
}


BOOL obj_UAV::OnDestroy()
{
	GameObject* fx = GameWorld().GetObject(damageFx);
	if(fx) fx->setActiveFlag(0);
	
	if(hudMain)
	{
		if(!m_TagIcon.IsUndefined())
			hudMain->removeUserIcon(m_TagIcon);
	}

	if (SoundSys.isPlaying(UavSnd))
	{
		SoundSys.Stop(UavSnd);
	}

	return parent::OnDestroy();
}

void obj_UAV::SetDamagedState()
{
	r3d_assert(!isDamaged);
	isDamaged = true;

	GameObject* fx = srv_CreateGameObject("obj_ParticleSystem", "fire_cameradrone", GetPosition());
	damageFx = fx->GetSafeID();
}

void obj_UAV::UpdatePositionBounds()
{
	r3d_assert(NetworkLocal);
	
	// adjust uav position bounds
	r3dPoint3D pos = GetPosition();
	
	if(pos.x < uav_LevelBounds[0])
		pos.x = uav_LevelBounds[0] + 0.01f;
	if(pos.x > uav_LevelBounds[1])
		pos.x = uav_LevelBounds[1] - 0.01f;
	if(pos.z < uav_LevelBounds[2])
		pos.z = uav_LevelBounds[2] + 0.01f;
	if(pos.z > uav_LevelBounds[3])
		pos.z = uav_LevelBounds[3] - 0.01f;
	if(pos.y > uav_LevelBounds[4])
		pos.y = uav_LevelBounds[4] - 0.01f;
		
	if(GetPosition() != pos) {
		SetPosition(pos);
		SetVelocity(r3dPoint3D(0, 0, 0));
	}
		
	return;
}

extern gobjid_t m_LocalPlayer_CurrentAimAt;
BOOL obj_UAV::Update()
{
	const float curTime = r3dGetTime();
	const float fTimePassed = r3dGetFrameTime();
	
	if (UaVisDestroy)
		return parent::Update();

	if (SoundSys.isPlaying(UavSnd))
	{
		SoundSys.SetSoundPos(UavSnd,GetPosition());
	}

	if(NetworkLocal)
	{
		UpdatePositionBounds();

		// decay horizontal velocity in 0.5 sec
		float decayXZ = R3D_MAX(0.0f, 1.0f - (fTimePassed / 0.5f));
		// decay vertical velocity in 0.1 sec
		float decayY  = R3D_MAX(0.0f, 1.0f - (fTimePassed / 0.1f));

		r3dPoint3D vel = GetVelocity();
		vel.x *= decayXZ;
		vel.y *= decayY;
		vel.z *= decayXZ;
		if(vel.Length() < 0.001f) vel = r3dPoint3D(0, 0, 0);
		SetVelocity(vel);
	}
	
	if(NetworkLocal)
	{
		PKT_C2C_MoveSetCell_s n1;
		PKT_C2C_MoveRel_s     n2;

		CNetCellMover::moveData_s md;
		md.pos       = GetPosition();
		md.turnAngle = GetRotationVector().x;
		md.bendAngle = 0;
		md.state     = 0;
		{
			DWORD pktFlags = netMover.SendPosUpdate(md, &n1, &n2);

			if(pktFlags & 0x1)
				p2pSendToHost(this, &n1, sizeof(n1), true);
			if(pktFlags & 0x2)
				p2pSendToHost(this, &n2, sizeof(n2), false);
		}
	}
	else
	{
		UpdatePositionFromRemote();
	}

	GameObject* fx = GameWorld().GetObject(damageFx);
	if(fx) fx->SetPosition(GetPosition());

	// angle drift code
	if(1)
	{
		tDrift[0] += fTimePassed / tDrift[1];
		if(tDrift[0] > 1) {
			tDrift[1]    = u_GetRandom(0.3f, 0.6f);
			tDrift[0]    = 0.0f;
			
			vDrift[0]    = vDrift[1];
			vDrift[1].y  = u_GetRandom(-3, 3);
			vDrift[1].z  = u_GetRandom(-3, 3);
		}
		r3dPoint3D rot;
		rot.x = -ViewAngle.x;
		rot.y = R3D_LERP(vDrift[0].y, vDrift[1].y, tDrift[0]);
		rot.z = R3D_LERP(vDrift[0].z, vDrift[1].z, tDrift[0]);
		SetRotationVector(rot);
	}

	if (g_bEditMode)
		return parent::Update();

	const ClientGameLogic& CGL = gClientLogic();
	obj_Player* owner = (obj_Player*)GameWorld().GetObject(ownerID);
	if(owner)
	{
		if (CGL.localPlayer_)
		{
			if (CGL.localPlayer_->uavViewActive_ == false)
			{
				if (!m_TagIcon.IsUndefined())
					hudMain->removeUserIcon(m_TagIcon);
				return parent::Update();
			}
		}

		if(m_TagIcon.IsUndefined() && CGL.localPlayer_)
		{
			// todo: need a better way to find player's reputation
			int rep = 0;
			{
				for(int i=0; i<256; ++i)
				{
					if(strcmp(CGL.playerNames[i].Gamertag, CGL.localPlayer_->CurLoadout.Gamertag)==0)
					{
						rep = CGL.playerNames[i].reputation;
						break;
					}
				}
			}

			hudMain->addCharTag(CGL.localPlayer_->CurLoadout.Gamertag, false, rep, m_TagIcon);
		}

		if(!m_TagIcon.IsUndefined())
		{
			R3DPROFILE_FUNCTION("moveScreenIcon");
			r3dPoint3D scrCoord;
			r3dProjectToScreen(GetPosition()+r3dPoint3D(0,2.0f,0), &scrCoord);

			bool showTag = false;
			bool showName = false;
			bool alwaysShow = false;
			
			// show tag only for friendly UAV
			if(CGL.localPlayer_)
			{
				float dist = (CGL.localPlayer_->GetPosition()-GetPosition()).Length();
				if(CGL.localPlayer_->bDead) // if local player is dead, use camera instead of player position
					dist = (gCam - GetPosition()).Length();

				showTag = true;

				if( (dist < 20.0f) ||
					(this->GetSafeID() == m_LocalPlayer_CurrentAimAt)
					)
				{
					showName = true;
				}
			}

#ifndef FINAL_BUILD
			if(d_observer_mode->GetBool())
			{
				showTag = true;
				showName = true;
			}
#endif

			//hudMain->showPlayerNameTag(m_TagIcon, showName);
			//hudMain->moveUserIcon(m_TagIcon, GetPosition()+r3dPoint3D(0,2.0f,0), alwaysShow, !showTag);

			hudMain->setCharTagTextVisible(m_TagIcon, showName, true, false);
			r3dPoint3D offset(0,0.3f,0);

			// get head position
			hudMain->moveUserIcon(m_TagIcon, CGL.localPlayer_->GetHeadPosition()+offset, true);
		}
	}
	
	return parent::Update();
}

void obj_UAV::UpdatePositionFromRemote()
{
	r3d_assert(!NetworkLocal);
	
	const float fTimePassed = r3dGetFrameTime();

	float rotX      = GetRotationVector().x;
	float turnAngle = netMover.NetData().turnAngle;

	if(fabs(rotX - turnAngle) > 0.01f) 
	{
		extern float getMinimumAngleDistance(float from, float to);
		float f1 = getMinimumAngleDistance(rotX, turnAngle);
		rotX += ((f1 < 0) ? -1 : 1) * fTimePassed * 360;
		float f2 = getMinimumAngleDistance(rotX, turnAngle);
		if((f1 > 0 && f2 <= 0) || (f1 < 0 && f2 >= 0))
			rotX = turnAngle;
	}
	ViewAngle.x = -rotX;
	
	if(netVelocity.LengthSq() > 0.0001f)
	{
		SetPosition(GetPosition() + netVelocity * fTimePassed);

		// check if we overmoved to target position
		r3dPoint3D v = netMover.GetNetPos() - GetPosition();
		float d = netVelocity.Dot(v);
		if(d < 0) {
			SetPosition(netMover.GetNetPos());
			netVelocity = r3dPoint3D(0, 0, 0);
		}
	}

}

void obj_UAV::OnNetPacket(const PKT_C2C_MoveSetCell_s& n)
{
	netMover.SetCell(n);
}

void obj_UAV::OnNetPacket(const PKT_C2C_MoveRel_s& n)
{
	const CNetCellMover::moveData_s& md = netMover.DecodeMove(n);
	
	// calc velocity to reach position on time for next update
	r3dPoint3D vel = netMover.GetVelocityToNetTarget(
		GetPosition(),
		GPP->UAV_FLY_SPEED_V * 1.5f,
		1.0f);

	netVelocity = vel;
}

void obj_UAV::OnNetPacket(const PKT_S2C_UAVSetState_s& n)
{
	switch(n.state)
	{
		case 0:
			break;

		case 1: // TODO: damaged particle
			if(!isDamaged)
			{
				SetDamagedState();
				state_ = UAV_Damaged;
			}
			break;

		case 2: // dead
			GameObject* fx = GameWorld().GetObject(damageFx);
			if(fx) fx->setActiveFlag(0);
			state_ = UAV_Killed;
			if (gClientLogic().localPlayer_->uavId_ == GetSafeID())
			{

			if (SoundSys.isPlaying(UavSnd))
			{
				SoundSys.Stop(UavSnd);
			}
				if(hudMain)
					hudMain->showMessage(gLangMngr.getString("$HUD_Msg_UAVKilled"));

				SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/Effects/Explosions/RPG01"),GetPosition());

				if (GetNetworkID() == gClientLogic().localPlayer_->UavNid_)
				{
					
					gClientLogic().localPlayer_->uavId_ = invalidGameObjectID;
					gClientLogic().localPlayer_->uavViewActive_ = false;
					gClientLogic().localPlayer_->uavRequested_  = 0;
					gClientLogic().localPlayer_->ToggleUAVView(true);

					for(int i=0; i<gClientLogic().localPlayer_->CurLoadout.BackpackSize; i++)
					{
						wiInventoryItem& wi = gClientLogic().localPlayer_->CurLoadout.Items[i];
						if (wi.itemID == WeaponConfig::ITEMID_Cypher2)
						{
							wi.Reset();
							gClientLogic().localPlayer_->OnBackpackChanged(i);
							break;
						}

					}
				}
				UaVisDestroy = true;

			}

			srv_CreateGameObject("obj_ParticleSystem", "explosion_camera_drone", GetPosition());
			if(  gClientLogic().localPlayer_ != NULL && gClientLogic().localPlayer_->uavId_ == GetSafeID() ) {
				gClientLogic().localPlayer_->ToggleUAVView( true );
			}
			GameWorld().DeleteObject( this );
			//setActiveFlag(0);	
			break;
	}
}

BOOL obj_UAV::OnNetReceive(DWORD EventID, const void* packetData, int packetSize)
{
	r3d_assert(!(ObjFlags & OBJFLAG_JustCreated)); // make sure that object was actually created before processing net commands

	switch(EventID)
	{
	default: return FALSE;

	case PKT_C2C_MoveSetCell:
		{
			const PKT_C2C_MoveSetCell_s& n = *(PKT_C2C_MoveSetCell_s*)packetData;
			r3d_assert(packetSize == sizeof(n));
			
			OnNetPacket(n);
			break;
		}
	case PKT_C2C_MoveRel:
		{
			const PKT_C2C_MoveRel_s& n = *(PKT_C2C_MoveRel_s*)packetData;
			r3d_assert(packetSize == sizeof(n));
			
			OnNetPacket(n);
			break;
		}

	case PKT_S2C_UAVSetState:
		{
			const PKT_S2C_UAVSetState_s& n = *(PKT_S2C_UAVSetState_s*)packetData;
			r3d_assert(packetSize == sizeof(n));
			
			OnNetPacket(n);
			break;
		}
	
	}

	return TRUE;
}

void obj_UAV::SetVelocity( const r3dPoint3D& vel )
{
	r3dPoint3D newVelocity = vel;
	// grab the target position after 1 tenth of a second. 
	r3dPoint3D targetPos = GetPosition() + vel *0.1f;

	// if the new position will be inside the bounds, we can use the velocity.  Otherwise kill the motion in that direction. 
	if(targetPos.x < uav_LevelBounds[0] ||
		targetPos.x > uav_LevelBounds[1]) {
		newVelocity.x = 0.0f; 
	} 
	if ( targetPos.z < uav_LevelBounds[2] ||
		targetPos.z > uav_LevelBounds[3] ) 
	{
		newVelocity.z = 0.0f; 
	}

	if ( targetPos.y > uav_LevelBounds[4] )
	{
		newVelocity.y = 0.0f;
	}

	obj_Building::SetVelocity( newVelocity );
}
