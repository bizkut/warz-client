#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_DroppedItem.h"

#include "ObjectsCode/weapons/WeaponArmory.h"
#include "../EFFECTS/obj_ParticleSystem.h"
#include "../../multiplayer/ClientGameLogic.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/MaterialTypes.h"
#include "../ai/AI_Player.H"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_DroppedItem, "obj_DroppedItem", "Object");
AUTOREGISTER_CLASS(obj_DroppedItem);

obj_DroppedItem::obj_DroppedItem()
{
	m_AllowAsyncLoading = 1;
	NeedInitPhysics = 0;
	m_AirHigthPos = 200.0f;
	m_IsOnTerrain = false;
	m_IsAirDrop = false;
	NetworLocal = false;
	AirDropPos = r3dPoint3D(0,0,0);
	AirCraftDistance = -600.0f;
	m_FirstTime = 0;
	AirCraft=NULL;
	Light1=NULL;
	Light2=NULL;
}

obj_DroppedItem::~obj_DroppedItem()
{
}

void obj_DroppedItem::SetHighlight( bool highlight )
{
	m_FillGBufferTarget = highlight ? rsFillGBufferAfterEffects : rsFillGBuffer;
}

bool obj_DroppedItem::GetHighlight() const
{
	return m_FillGBufferTarget == rsFillGBufferAfterEffects;
}

BOOL obj_DroppedItem::Load(const char *fname)
{
	return TRUE;
}

BOOL obj_DroppedItem::OnCreate()
{
	R3DPROFILE_FUNCTION( "obj_DroppedItem::OnCreate" );

#ifndef FINAL_BUILD
	if( g_bEditMode )
	{
		m_Item.itemID = 'GOLD';
		SetHighlight( true );
	}
#endif

	r3d_assert(m_Item.itemID);
	
	const char* cpMeshName = "";
	if(m_Item.itemID == 'GOLD')
	{
		cpMeshName = "Data\\ObjectsDepot\\Weapons\\Item_Money_Stack_01.sco";
	}
	else if(m_Item.itemID == 'ARDR')
	{
		cpMeshName = "Data\\ObjectsDepot\\WZ_CityProps\\container_airdrop.sco";
		if (m_FirstTime == 1)
		{
			SetObjFlags(OBJFLAG_SkipDraw | OBJFLAG_DisableShadows);
			//AirCraft create
			AirCraft = (obj_Building*)srv_CreateGameObject("obj_Building", "Data\\ObjectsDepot\\WZ_C130\\C130_Air.sco", GetPosition());
			AirCraft->DrawDistanceSq = FLT_MAX;
			r3dscpy(AirCraft->m_sAnimName,"C130_Air.anm");
			AirCraft->m_bGlobalAnimFolder = 0;
			AirCraft->m_bAnimated = 1;

			sndAircraft = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/Vehicles/C130/HerculesEnguine"),AirCraft->GetPosition());
			SoundSys.Stop(sndAircraft);
		}
		
	}
	else if(m_Item.itemID == 'FLPS')
	{
		SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("Barricade_Smash_Wood"), GetPosition(), r3dPoint3D(0,1,0));
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "airdrop3", GetPosition());
		cpMeshName = "Data\\ObjectsDepot\\Weapons\\item_flare_emergency01.sco";
	}
	else if(m_Item.itemID == 'LOOT')
	{
		cpMeshName = "Data\\ObjectsDepot\\Weapons\\Item_LootCrate_01.sco";
	}
	else if(m_Item.itemID == 'PRBX')
	{
		cpMeshName = "Data\\ObjectsDepot\\Weapons\\mil_box_wood_m_02.sco";
		m_bEnablePhysics=true;
	}
	else
	{
		const ModelItemConfig* cfg = (const ModelItemConfig*)g_pWeaponArmory->getConfig(m_Item.itemID);
		switch(cfg->category)
		{
			case storecat_Account:
			case storecat_Boost:
			case storecat_LootBox:
			case storecat_HeroPackage:
				r3dError("spawned item is not model");
				break;
		}
		cpMeshName = cfg->m_ModelPath;
	}
	
	if(!parent::Load(cpMeshName)) 
		return FALSE;

	if(m_Item.itemID == 'GOLD')
	{
		m_ActionUI_Title = gLangMngr.getString("Money");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpMoney");
	}
	else if(m_Item.itemID == 'ARDR')
	{
		m_ActionUI_Title = gLangMngr.getString("Construction");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpMoney");
	}
	else if(m_Item.itemID == 'PRBX')
	{
		m_ActionUI_Title = gLangMngr.getString("Construction");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpMoney");
	}
	else if(m_Item.itemID == 'FLPS')
	{
		m_ActionUI_Title = gLangMngr.getString("Item");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else if(m_Item.itemID == 'LOOT')
	{
		m_ActionUI_Title = gLangMngr.getString("Item");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else
	{
		const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(m_Item.itemID);
		m_ActionUI_Title = cfg->m_StoreName;
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}

	
	if(m_Item.itemID != 'PRBX' && m_Item.itemID != 'ARDR')
	{
		ReadPhysicsConfig();
		PhysicsConfig.isDynamic = false; // to prevent items from falling through the ground
		PhysicsConfig.isKinematic = true; // to prevent them from being completely static, as moving static object is causing physx performance issues, and we cannot place item at this point, as it is not loaded fully yet
		PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = false;
		PhysicsConfig.isFastMoving = false; // fucked up PhysX CCD is crashing all the time
	
		SetPosition(GetPosition()+r3dPoint3D(0,0.25f,0));
	}

	if(m_Item.itemID == 'ARDR')
	{
		//AirCraft Position Start and Sound
		if (AirCraft!=NULL) 
			AirCraft->SetPosition(m_spawnPos+r3dPoint3D(0,0,AirCraftDistance));

		m_spawnPos = AirDropPos;
		SetPosition(m_spawnPos);
		m_IsAirDrop = true;
		m_bEnablePhysics = true;
	}
	else {
		m_spawnPos = GetPosition();
	}

	// will have to create it later - when we are loaded.
	if( m_bEnablePhysics && m_IsAirDrop == false )
	{
		NeedInitPhysics = 1;
		m_bEnablePhysics = 0;
	}

	parent::OnCreate();

	return 1;
}

void obj_DroppedItem::StartLights()
{
	if (Light1==NULL)
	{
		Light1 = (obj_LightHelper*)srv_CreateGameObject("obj_LightHelper", "Omni", GetPosition() + r3dPoint3D(0,3,0));
		Light1->Color = r3dColor::white;
		Light1->LT.Intensity = 2.0f;
		Light1->bOn = true;
		Light1->innerRadius = 0.0f;
		Light1->outerRadius = 8.37f;
		Light1->bKilled = false;
		Light1->DrawDistanceSq = FLT_MAX;
		Light1->SetPosition(GetPosition()+r3dPoint3D(0,0,3.11f));
	}

	if (Light2==NULL)
	{
		Light2 = (obj_LightHelper*)srv_CreateGameObject("obj_LightHelper", "Omni", GetPosition() + r3dPoint3D(0,3,0));
		Light2->Color = r3dColor::white;
		Light2->LT.Intensity = 2.0f;
		Light2->bOn = true;
		Light2->innerRadius = 0.0f;
		Light2->outerRadius = 8.37f;
		Light2->bKilled = false;
		Light2->DrawDistanceSq = FLT_MAX;
		Light2->SetPosition(GetPosition()+r3dPoint3D(0,0,-3.11f));
	}
}

void obj_DroppedItem::UpdateObjectPositionAfterCreation()
{
	if(!PhysicsObject)
		return;

	PxActor* actor = PhysicsObject->getPhysicsActor();
	if(!actor)
		return;

	PxBounds3 pxBbox = actor->getWorldBounds();
	PxVec3 pxCenter = pxBbox.getCenter();

	// place object on the ground, to prevent excessive bouncing
	{
		PxRaycastHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
		if(g_pPhysicsWorld->raycastSingle(PxVec3(pxCenter.x, pxCenter.y, pxCenter.z), PxVec3(0, -1, 0), 50.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
		{
			float diff = GetPosition().y - pxBbox.minimum.y;
			SetPosition(r3dPoint3D(hit.impact.x, hit.impact.y+diff, hit.impact.z));
		}
	}
}

BOOL obj_DroppedItem::OnDestroy()
{

	if (SoundSys.IsHandleValid(sndAircraft))
	{
		SoundSys.Release(sndAircraft);
		sndAircraft = NULL;
	}
	if (AirCraft!=NULL)
		AirCraft = NULL;

	if (Light1!=NULL)
		Light1=NULL;
	if (Light2!=NULL)
		Light2=NULL;

	return parent::OnDestroy();
}

BOOL obj_DroppedItem::Update()
{
	if( NeedInitPhysics && MeshLOD[ 0 ] && MeshLOD[ 0 ]->IsDrawable() && m_Item.itemID != 'ARDR')
	{
		m_bEnablePhysics = 1;
		CreatePhysicsData();
		NeedInitPhysics = 0;
		UpdateObjectPositionAfterCreation();
	}

	if (AirCraft!=NULL && NetworLocal)
	{
		float dist = (AirCraft->GetPosition() - m_spawnPos).Length();

		if(dist<1.8f)
		{
			ResetObjFlags(OBJFLAG_SkipDraw | OBJFLAG_DisableShadows);
			StartLights();
		}

		AirCraft->SetPosition(m_spawnPos+r3dPoint3D(0,0,AirCraftDistance));
		SoundSys.SetSoundPos(sndAircraft,AirCraft->GetPosition());
		AirCraftDistance+=2.0f;

		if (!SoundSys.isPlaying(sndAircraft))
			SoundSys.Start(sndAircraft);

		if (AirCraftDistance>600.0f)
		{
			SoundSys.Stop(sndAircraft);
			AirCraft->setActiveFlag(0);
			AirCraft = NULL;
		}
	}

	parent::Update();
	
	r3dPoint3D pos = GetBBoxWorld().Center();

#ifndef FINAL_BUILD
	if( !g_bEditMode )
#endif
	{
		const ClientGameLogic& CGL = gClientLogic();
		if(CGL.localPlayer_ && (CGL.localPlayer_->GetPosition() - pos).Length() < 10.0f)
			SetHighlight(true);
		else
			SetHighlight(false);
	}

	return TRUE;
}

void obj_DroppedItem::ServerPost(float posY)
{
	if (!NetworLocal)
		return;

		m_spawnPos.y=posY;
		SetPosition(m_spawnPos);

		if (Light1!=NULL)
		{
			Light1->SetPosition(r3dPoint3D(Light1->GetPosition().x,posY+1,Light1->GetPosition().z));
		}
		if (Light2!=NULL)
		{
			Light2->SetPosition(r3dPoint3D(Light2->GetPosition().x,posY+1,Light2->GetPosition().z));
		}
}

void obj_DroppedItem::AppendRenderables( RenderArray ( & render_arrays )[ rsCount ], const r3dCamera& Cam )
{
	MeshGameObject::AppendRenderables( render_arrays, Cam );

	if( GetHighlight() )
	{
		MeshObjDeferredHighlightRenderable rend;

		rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::blue.GetPacked() );
		rend.SortValue = 0;
		rend.DoExtrude = 0;
		
		render_arrays[ rsDrawHighlightPass0 ].PushBack( rend );

		rend.DoExtrude = 1;
		render_arrays[ rsDrawHighlightPass1 ].PushBack( rend );
	}
}


