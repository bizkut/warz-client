//=========================================================================
//	Module: obj_ZombieSpawn.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "obj_ZombieSpawn.h"
#include "../../XMLHelpers.h"
#include "../../Editors/LevelEditor.h"
#include "../WEAPONS/WeaponArmory.h"
#include "../../../../GameEngine/ai/AI_Brain.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_ZombieSpawn, "obj_ZombieSpawn", "Object");
AUTOREGISTER_CLASS(obj_ZombieSpawn);

extern bool g_bEditMode;

//////////////////////////////////////////////////////////////////////////

namespace
{
//////////////////////////////////////////////////////////////////////////

	struct ZombieSpawnCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			ZombieSpawnCompositeRenderable *This = static_cast<ZombieSpawnCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);

			r3dDrawLine3D(This->Parent->GetPosition(), This->Parent->GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, r3dColor24::grey);
			r3dDrawCircle3D(This->Parent->GetPosition(), This->Parent->spawnRadius, Cam, 0.1f, r3dColor(0, 255, 0));

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);

			if( This->Parent->m_CFHMaxZombies > 0 )
			{
				r3dRenderer->SetRenderingMode(R3D_BLEND_ZC | R3D_BLEND_PUSH);

				const r3dPoint3D&	pos		= This->Parent->GetPosition();
				const r3dVector		extents	= 2 * This->Parent->m_CFHExtents;
				r3dDrawBox3D( pos.x, pos.y, pos.z, extents.x, extents.y, extents.z, r3dColor24( 225.0f, 25.0f, 25.0f, 40.0f ) );

				r3dBoundBox box;
				box.Org		= pos - This->Parent->m_CFHExtents;
				box.Size	= extents;
				r3dDrawBoundBox( box, Cam, r3dColor24( 250.0f, 50.0f, 50.0f, 255.0f ), 0.01f );

				r3dRenderer->Flush();
				r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
			}


		}

		obj_ZombieSpawn *Parent;	
	};
}

//////////////////////////////////////////////////////////////////////////

obj_ZombieSpawn::obj_ZombieSpawn()
: spawnRadius(30.0f)
, zombieSpawnDelay(15.0f)
, maxZombieCount(50)
, minDetectionRadius(1)
, maxDetectionRadius(3.0f)
, sleepersRate(10.0f)
, lootBoxID(0)
, fastZombieChance(50.0f)
, speedVariation(5.0f)
, m_SprintersSpawnPerc(0.0f)
, m_SprintPerc(0.0f)
, m_SprintRadius(0.0f)
, m_SprintMaxSpeed(0.0f)
, m_SprintMaxTime(0.0f)
, m_SprintSlope(0.0f)
, m_SprintCooldownTime(0.0f)
, m_SprintFromFarPerc(0.0f)
, m_CFHZombiesSpawned(0)
, m_CFHMaxZombies(0)
, m_CFHMinAdjZombies(10)
, m_CFHPerc(0.0f)
, m_CFHSpawnPerc(0.0f)
, m_CFHExtents( 5.0f, 2.0f, 5.0f )
{
	serializeFile = SF_ServerData;
	m_bEnablePhysics = false;
}

//////////////////////////////////////////////////////////////////////////

obj_ZombieSpawn::~obj_ZombieSpawn()
{

}

//////////////////////////////////////////////////////////////////////////

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_ZombieSpawn::AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam)
{
#ifdef FINAL_BUILD
	return;
#else
	if ( !g_bEditMode )
		return;

	if(r_hide_icons->GetInt())
		return;

	float idd = r_icons_draw_distance->GetFloat();
	idd *= idd;

	if( ( Cam - GetPosition() ).LengthSq() > idd )
		return;

	ZombieSpawnCompositeRenderable rend;

	rend.Init();
	rend.Parent		= this;
	rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;

	render_arrays[ rsDrawDebugData ].PushBack( rend );
#endif
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_ZombieSpawn::OnCreate()
{
	parent::OnCreate();

	DrawOrder = OBJ_DRAWORDER_LAST;

	ObjFlags |= OBJFLAG_DisableShadows;

	r3dBoundBox bboxLocal ;
	bboxLocal.Size = r3dPoint3D(2, 2, 2);
	bboxLocal.Org = -bboxLocal.Size * 0.5f;
	SetBBoxLocal(bboxLocal) ;
	UpdateTransform();

	return 1;
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_ZombieSpawn::Update()
{
	return parent::Update();
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_ZombieSpawn::OnDestroy()
{
	return parent::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////

#ifndef FINAL_BUILD
float obj_ZombieSpawn::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float y = scry;

	y += parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected);
	y += 5.0f;

	y += imgui_Static(scrx, y, "Spawn parameters:");
	int zc = maxZombieCount;
	y += imgui_Value_SliderI(scrx, y, "Max zombies", &zc, 0, 100.0f, "%d");
	maxZombieCount = zc;
	y += imgui_Value_Slider(scrx, y, "%% Sleepers", &sleepersRate, 0, 100.0f, "%.0f");
	y += imgui_Value_Slider(scrx, y, "Spawn delay (sec)", &zombieSpawnDelay, 1.0f, 10000.0f, "%-02.3f");
	y += imgui_Value_Slider(scrx, y, "Spawn radius", &spawnRadius, 10.0f, 300.0f, "%-02.3f");

	y += imgui_Value_Slider(scrx, y, "%% FastZombie", &fastZombieChance, 0.0f, 100.0f, "%-02.3f");
	y += imgui_Value_Slider(scrx, y, "%% Speed Variation", &speedVariation, 0.0f, 50.0f, "%-02.3f");

	y += 5.0f;
	y += imgui_Static(scrx, y, "Zombie senses parameters:");
	
	y += imgui_Value_Slider(scrx, y, "Min detect radius", &minDetectionRadius, 1.0f, 100.0f, "%-02.3f");
	maxDetectionRadius = std::max(minDetectionRadius, maxDetectionRadius);
	y += imgui_Value_Slider(scrx, y, "Max detect radius", &maxDetectionRadius, 1.0f, 100.0f, "%-02.3f");
	minDetectionRadius = std::min(minDetectionRadius, maxDetectionRadius);

	y += 5.0f;
	y += imgui_Static(scrx, y, "Zombie sprint parameters:");
	
	y += imgui_Value_Slider(scrx, y, "Spawn sprinters %%", &m_SprintersSpawnPerc, 0.0f, 100.0f, "%0.2f");
	y += imgui_Value_Slider(scrx, y, "Radius", &m_SprintRadius, 5.0f, 100.0f, "%0.2f");
	y += imgui_Value_Slider(scrx, y, "%% inside radius", &m_SprintPerc, 0, 100.0f, "%0.2f");
	y += imgui_Value_Slider(scrx, y, "%% outside radius", &m_SprintFromFarPerc, 0, 100.0f, "%0.2f");
	y += imgui_Value_Slider(scrx, y, "Max speed", &m_SprintMaxSpeed, 3.0f, 10.0f, "%0.2f");
	y += imgui_Value_Slider(scrx, y, "Max time", &m_SprintMaxTime, 5.0f, 15.0f, "%0.2f");
	y += imgui_Value_Slider(scrx, y, "Speed up slope", &m_SprintSlope, 1.0f, 5.0f, "%0.2f");
	y += imgui_Value_Slider(scrx, y, "Cooldown (sec)", &m_SprintCooldownTime, 1.0f, 10.0f, "%0.2f");

	y += 5.0f;
	y += imgui_Static(scrx, y, "Zombie call for help parameters:");
	int CFHmz			= m_CFHMaxZombies;
	int CFHmaz			= m_CFHMinAdjZombies;
	float CFHPerc		= m_CFHPerc * 100.0f;
	float CFHSpawnPerc	= m_CFHSpawnPerc * 100.0f;
	y += imgui_Value_SliderI(scrx, y, "Max allowed", &CFHmz, 0, 100, "%d" );
	y += imgui_Value_SliderI(scrx, y, "Min nearby required", &CFHmaz, 0, 100, "%d");
	y += imgui_Value_Slider(scrx, y, "Call %% chance", &CFHPerc, 0.01f, 100.0f, "%0.2f");
	y += imgui_Value_Slider(scrx, y, "Spawn %% chance", &CFHSpawnPerc, 0.01f, 100.0f, "%0.2f");
	y += imgui_Value_Slider(scrx, y, "Call max x dist.", &m_CFHExtents.x, 1.0f, 20.0f, "%0.2f");
	y += imgui_Value_Slider(scrx, y, "Call max y dist.", &m_CFHExtents.y, 1.0f, 10.0f, "%0.2f");
	y += imgui_Value_Slider(scrx, y, "Call max z dist.", &m_CFHExtents.z, 1.0f, 20.0f, "%0.2f");
	m_CFHMaxZombies		= CFHmz;
	m_CFHMinAdjZombies	= CFHmaz;
	m_CFHPerc			= CFHPerc / 100.0f;
	m_CFHSpawnPerc		= CFHSpawnPerc / 100.0f;

	y += 5.0f;
	y += imgui_Static(scrx, y, "Brain configurations to use:");
	// brain selection
	{
		static int numBrains = 0;
		struct tempS
		{
			const TCHAR* name;
			uint32_t id;
		};
		static r3dgameVector(tempS) brainBoxes;
		if(numBrains == 0)
		{
			const r3dgameVector(uint32_t)& brainConfigIDs = g_pAIBrainProfiles->GetBrainConfigIDs();
			for(r3dgameVector(uint32_t)::const_iterator iter = brainConfigIDs.begin();
				iter != brainConfigIDs.end(); ++iter)
			{
				const AI_BrainConfig* brainConfig = g_pAIBrainProfiles->GetBrainConfig( *iter );
				if( brainConfig )
				{
					tempS holder;
					holder.name = brainConfig->Name;
					holder.id = brainConfig->ID;
					brainBoxes.push_back( holder );
				}
			}
			numBrains = brainBoxes.size();
		}

		for(int i=0; i<numBrains; ++i)
		{
			int isselected = 0;
			int wasIsSelected = 0;
			isselected = wasIsSelected = (std::find(ZombieBrainSelection.begin(), ZombieBrainSelection.end(), brainBoxes[i].id)!=ZombieBrainSelection.end())?1:0;
			y += imgui_Checkbox(scrx, y, brainBoxes[i].name, &isselected, 1);
			if(isselected != wasIsSelected)
			{
				if(wasIsSelected && !isselected)
					ZombieBrainSelection.erase(std::find(ZombieBrainSelection.begin(), ZombieBrainSelection.end(), brainBoxes[i].id));
				else if(!wasIsSelected && isselected)
					ZombieBrainSelection.push_back(brainBoxes[i].id);
			}
		}
	}

	y += 5.0f;
	y += imgui_Static(scrx, y, "Zombies to spawn:");
	// zombie selection
	{
		static int numZombies = 0;
		struct tempS
		{
			char* name;
			uint32_t id;
		};
		static r3dgameVector(tempS) zombieBoxes;
		if(numZombies == 0)
		{
			g_pWeaponArmory->startItemSearch();
			while(g_pWeaponArmory->searchNextItem())
			{
				uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
				const HeroConfig* cfg = g_pWeaponArmory->getHeroConfig(itemID);
				if( cfg && cfg->m_Weight == -1.0f) //< 0.0f ) // weight -1 is for zombies only
				{
					tempS holder;
					holder.name = cfg->m_StoreName;
					holder.id = cfg->m_itemID;
					zombieBoxes.push_back(holder);						 
				}
			}
			numZombies = (int)zombieBoxes.size();
		}

		for(int i=0; i<numZombies; ++i)
		{
			int isselected = 0;
			int wasIsSelected = 0;
			isselected = wasIsSelected = (std::find(ZombieSpawnSelection.begin(), ZombieSpawnSelection.end(), zombieBoxes[i].id)!=ZombieSpawnSelection.end())?1:0;
			y += imgui_Checkbox(scrx, y, zombieBoxes[i].name, &isselected, 1);
			if(isselected != wasIsSelected)
			{
				if(wasIsSelected && !isselected)
					ZombieSpawnSelection.erase(std::find(ZombieSpawnSelection.begin(), ZombieSpawnSelection.end(), zombieBoxes[i].id));
				else if(!wasIsSelected && isselected)
					ZombieSpawnSelection.push_back(zombieBoxes[i].id);
			}
		}
	}

	// loot box selection
	{
		static stringlist_t lootBoxNames;
		static int* lootBoxIDs = NULL;
		static int numLootBoxes = 0;
		if(numLootBoxes == 0)
		{
			struct tempS
			{
				char* name;
				uint32_t id;
			};
			r3dgameVector(tempS) lootBoxes;
			{
				tempS holder;
				holder.name = "EMPTY";
				holder.id = 0;
				lootBoxes.push_back(holder);		
			}

			g_pWeaponArmory->startItemSearch();
			while(g_pWeaponArmory->searchNextItem())
			{
				uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
				const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(itemID);
				if( cfg->category == storecat_LootBox )
				{
					tempS holder;
					holder.name = cfg->m_StoreName;
					holder.id = cfg->m_itemID;
					lootBoxes.push_back(holder);						 
				}
			}
			numLootBoxes = (int)lootBoxes.size();
			lootBoxIDs = game_new int[numLootBoxes];
			for(int i=0; i<numLootBoxes; ++i)
			{
				lootBoxNames.push_back(lootBoxes[i].name);
				lootBoxIDs[i] = lootBoxes[i].id;
			}
		}

		int sel = 0;
		static float offset = 0;
		for(int i=0; i<numLootBoxes; ++i)
			if(lootBoxID == lootBoxIDs[i])
				sel = i;
		y += imgui_Static ( scrx, y, "Loot box:" );
		if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames, &offset, &sel))
		{
			lootBoxID = lootBoxIDs[sel];
		}
		y += 122;
	}

	return y - scry;
}
#endif

//////////////////////////////////////////////////////////////////////////

void obj_ZombieSpawn::WriteSerializedData(pugi::xml_node& node)
{
	// Display is 0 - 100.0f, stored/used as 0 - 1.0f
	float sprintersSpawnPerc = m_SprintersSpawnPerc / 100.0f;
	float sprintPerc = m_SprintPerc / 100.0f;
	float sprintFromFarPerc = m_SprintFromFarPerc / 100.0f;

	parent::WriteSerializedData(node);
	pugi::xml_node zombieSpawnNode = node.append_child();
	zombieSpawnNode.set_name("spawn_parameters");
	SetXMLVal("spawn_radius", zombieSpawnNode, &spawnRadius);
	int mzc = maxZombieCount;
	SetXMLVal("max_zombies", zombieSpawnNode, &mzc);
	SetXMLVal("sleepers_rate", zombieSpawnNode, &sleepersRate);
	SetXMLVal("zombies_spawn_delay", zombieSpawnNode, &zombieSpawnDelay);
	SetXMLVal("min_detection_radius", zombieSpawnNode, &minDetectionRadius);
	SetXMLVal("max_detection_radius", zombieSpawnNode, &maxDetectionRadius);
	SetXMLVal("lootBoxID", zombieSpawnNode, &lootBoxID);
	SetXMLVal("fastZombieChance", zombieSpawnNode, &fastZombieChance);
	SetXMLVal("speedVariation", zombieSpawnNode, &speedVariation);
	SetXMLVal("SprintersSpawnPerc", zombieSpawnNode, &sprintersSpawnPerc);
	SetXMLVal("SprintPerc", zombieSpawnNode, &sprintPerc);
	SetXMLVal("SprintRadius", zombieSpawnNode, &m_SprintRadius);
	SetXMLVal("SprintMaxSpeed", zombieSpawnNode, &m_SprintMaxSpeed);
	SetXMLVal("SprintMaxTime", zombieSpawnNode, &m_SprintMaxTime);
	SetXMLVal("SprintSlope", zombieSpawnNode, &m_SprintSlope);
	SetXMLVal("SprintCooldownTime", zombieSpawnNode, &m_SprintCooldownTime);
	SetXMLVal("SprintFromFarPerc", zombieSpawnNode, &sprintFromFarPerc);
	SetXMLVal("CallForHelpMaxZombies", zombieSpawnNode, &m_CFHMaxZombies);
	SetXMLVal("CallForHelpMinAdjZombies", zombieSpawnNode, &m_CFHMinAdjZombies);
	SetXMLVal("CallForHelpPerc", zombieSpawnNode, &m_CFHPerc);
	SetXMLVal("CallForHelpSpawnPerc", zombieSpawnNode, &m_CFHSpawnPerc);
	SetXMLVal("CallForHelpExtents", zombieSpawnNode, &m_CFHExtents);
	zombieSpawnNode.append_attribute("numZombieSelected") = ZombieSpawnSelection.size();
	for(size_t i=0; i<ZombieSpawnSelection.size(); ++i)
	{
		pugi::xml_node zNode = zombieSpawnNode.append_child();
		char tempStr[32];
		sprintf(tempStr, "z%d", i);
		zNode.set_name(tempStr);
		zNode.append_attribute("id") = ZombieSpawnSelection[i];
	}
	zombieSpawnNode.append_attribute("numBrainsSelected") = ZombieBrainSelection.size();
	for(size_t i=0; i<ZombieBrainSelection.size(); ++i)
	{
		pugi::xml_node zNode = zombieSpawnNode.append_child();
		char tempStr[32];
		sprintf(tempStr, "b%d", i);
		zNode.set_name(tempStr);
		zNode.append_attribute("id") = ZombieBrainSelection[i];
	}
}

// NOTE: this function must stay in sync with server version
void obj_ZombieSpawn::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node zombieSpawnNode = node.child("spawn_parameters");
	GetXMLVal("spawn_radius", zombieSpawnNode, &spawnRadius);
	int mzc = maxZombieCount;
	GetXMLVal("max_zombies", zombieSpawnNode, &mzc);
	maxZombieCount = mzc;
	GetXMLVal("sleepers_rate", zombieSpawnNode, &sleepersRate);
	GetXMLVal("zombies_spawn_delay", zombieSpawnNode, &zombieSpawnDelay);
	GetXMLVal("min_detection_radius", zombieSpawnNode, &minDetectionRadius);
	GetXMLVal("max_detection_radius", zombieSpawnNode, &maxDetectionRadius);
	GetXMLVal("lootBoxID", zombieSpawnNode, &lootBoxID);
	GetXMLVal("fastZombieChance", zombieSpawnNode, &fastZombieChance);
	GetXMLVal("speedVariation", zombieSpawnNode, &speedVariation);
	GetXMLVal("SprintersSpawnPerc", zombieSpawnNode, &m_SprintersSpawnPerc);
	GetXMLVal("SprintPerc", zombieSpawnNode, &m_SprintPerc);
	GetXMLVal("SprintRadius", zombieSpawnNode, &m_SprintRadius);
	GetXMLVal("SprintMaxSpeed", zombieSpawnNode, &m_SprintMaxSpeed);
	GetXMLVal("SprintMaxTime", zombieSpawnNode, &m_SprintMaxTime);
	GetXMLVal("SprintSlope", zombieSpawnNode, &m_SprintSlope);
	GetXMLVal("SprintCooldownTime", zombieSpawnNode, &m_SprintCooldownTime);
	GetXMLVal("SprintFromFarPerc", zombieSpawnNode, &m_SprintFromFarPerc);
	GetXMLVal("CallForHelpMaxZombies", zombieSpawnNode, &m_CFHMaxZombies);
	GetXMLVal("CallForHelpMinAdjZombies", zombieSpawnNode, &m_CFHMinAdjZombies);
	GetXMLVal("CallForHelpPerc", zombieSpawnNode, &m_CFHPerc);
	GetXMLVal("CallForHelpSpawnPerc", zombieSpawnNode, &m_CFHSpawnPerc);
	GetXMLVal("CallForHelpExtents", zombieSpawnNode, &m_CFHExtents);
	uint32_t numZombies = zombieSpawnNode.attribute("numZombieSelected").as_uint();
	for(uint32_t i=0; i<numZombies; ++i)
	{
		char tempStr[32];
		sprintf(tempStr, "z%d", i);
		pugi::xml_node zNode = zombieSpawnNode.child(tempStr);
		r3d_assert(!zNode.empty());
		ZombieSpawnSelection.push_back(zNode.attribute("id").as_uint());
	}
	uint32_t numBrains = zombieSpawnNode.attribute("numBrainsSelected").as_uint();
	for(uint32_t i=0; i<numBrains; ++i)
	{
		char tempStr[32];
		sprintf(tempStr, "b%d", i);
		pugi::xml_node zNode = zombieSpawnNode.child(tempStr);
		r3d_assert(!zNode.empty());
		ZombieBrainSelection.push_back(zNode.attribute("id").as_uint());
	}

	// Display is 0 - 100.0f, stored/used as 0 - 1.0f
	m_SprintersSpawnPerc *= 100.0f;
	m_SprintPerc *= 100.0f;
	m_SprintFromFarPerc *= 100.0f;
}

//////////////////////////////////////////////////////////////////////////

GameObject * obj_ZombieSpawn::Clone()
{
	obj_ZombieSpawn * newSpawnPoint = static_cast<obj_ZombieSpawn*>(srv_CreateGameObject("obj_ZombieSpawn", FileName.c_str(), GetPosition()));
	newSpawnPoint->CloneParameters(this);
	return newSpawnPoint;
}

//////////////////////////////////////////////////////////////////////////

void obj_ZombieSpawn::CloneParameters(obj_ZombieSpawn *o)
{
	spawnRadius = o->spawnRadius;
	zombieSpawnDelay = o->zombieSpawnDelay;
	maxZombieCount = o->maxZombieCount;
	minDetectionRadius= o->minDetectionRadius;
	maxDetectionRadius = o->maxDetectionRadius;
	sleepersRate = o->sleepersRate;
	lootBoxID = o->lootBoxID;
	fastZombieChance = o->fastZombieChance;
	speedVariation = o->speedVariation;
	m_SprintersSpawnPerc = o->m_SprintersSpawnPerc;
	m_SprintPerc = o->m_SprintPerc;
	m_SprintRadius = o->m_SprintRadius;
	m_SprintMaxSpeed = o->m_SprintMaxSpeed;
	m_SprintMaxTime = o->m_SprintMaxTime;
	m_SprintSlope = o->m_SprintSlope;
	m_SprintCooldownTime = o->m_SprintCooldownTime;
	m_SprintFromFarPerc = o->m_SprintFromFarPerc;
	m_CFHZombiesSpawned = o->m_CFHZombiesSpawned;
	m_CFHMaxZombies = o->m_CFHMaxZombies;
	m_CFHMinAdjZombies = o->m_CFHMinAdjZombies;
	m_CFHPerc = o->m_CFHPerc;
	m_CFHSpawnPerc = o->m_CFHSpawnPerc;
	m_CFHExtents = o->m_CFHExtents;
	serializeFile = o->serializeFile;
	m_bEnablePhysics = o->m_bEnablePhysics;
	ZombieSpawnSelection = o->ZombieSpawnSelection;
	ZombieBrainSelection = o->ZombieBrainSelection;
}
