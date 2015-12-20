#include "r3dPCH.h"
#include "r3d.h"

#include "Gameplay_Params.h"
#include "GameCommon.h"
#include "../ai/AI_Player.H"

#include "obj_ItemSpawnPoint.h"
#include "gameobjects/obj_Mesh.h"

#include "multiplayer/P2PMessages.h"
#include "multiplayer/ClientGameLogic.h"
#include "../WEAPONS/WeaponArmory.h"

#include "Editors/ObjectManipulator3d.h"

#ifndef FINAL_BUILD
#include "CkHttpRequest.h"
#include "CkHttp.h"
#include "CkHttpResponse.h"
#include "CkByteData.h"
#endif

IMPLEMENT_CLASS(obj_ItemSpawnPoint, "obj_ItemSpawnPoint", "Object");
AUTOREGISTER_CLASS(obj_ItemSpawnPoint);

extern bool g_bEditMode;

#ifndef FINAL_BUILD
IMPLEMENT_CLASS(obj_HackerItemSpawnPoint, "obj_HackerItemSpawnPoint", "Object");
AUTOREGISTER_CLASS(obj_HackerItemSpawnPoint);

obj_HackerItemSpawnPoint::obj_HackerItemSpawnPoint():obj_ItemSpawnPoint()
{
	editor_spawnPointBoxColor = r3dColor::yellow;
	m_bEditorCheckSpawnPointAtStart = false;
}

obj_HackerItemSpawnPoint::~obj_HackerItemSpawnPoint()
{
}

static std::vector<obj_ItemSpawnPoint*>			m_LootBoxArray;
#endif

obj_ItemSpawnPoint::obj_ItemSpawnPoint()
{
	m_bEnablePhysics = false; // they do not need physics
	editor_spawnPointBoxColor = r3dColor::green;
	ObjTypeFlags |= OBJTYPE_ItemSpawnPoint;
	m_SelectedSpawnPoint = 0;
	m_bEditorCheckSpawnPointAtStart = true;
#ifndef FINAL_BUILD
	m_LootBoxAnalysis_NextUpdateTime = 0;
	m_LootBoxArray.push_back(this);
#endif
}

obj_ItemSpawnPoint::~obj_ItemSpawnPoint()
{
#ifndef FINAL_BUILD
	m_LootBoxArray.erase(std::find(m_LootBoxArray.begin(), m_LootBoxArray.end(), this));
#endif
}

BOOL obj_ItemSpawnPoint::Load(const char *fname)
{
#ifndef FINAL_BUILD
	if(!g_bEditMode)
#endif
	{
		return TRUE; // do not load meshes in deathmatch mode, not showing control points
	}
	const char* cpMeshName = "Data\\ObjectsDepot\\Capture_Points\\Flag_Pole_01.sco";

	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_ItemSpawnPoint::OnCreate()
{
	parent::OnCreate();

#ifndef FINAL_BUILD
	if(!g_bEditMode)
#endif
		ObjFlags |= OBJFLAG_SkipCastRay;

#ifndef FINAL_BUILD
	if(g_bEditMode) // to make it easier in editor to edit spawn points
	{
		setSkipOcclusionCheck(true);
		ObjFlags |= OBJFLAG_AlwaysDraw | OBJFLAG_ForceSleep ;
	}
#endif

	UpdateTransform();

	return 1;
}


BOOL obj_ItemSpawnPoint::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_ItemSpawnPoint::Update()
{
	static int delayCheckSpawnPointsInAir = 1000;
	if(delayCheckSpawnPointsInAir)
		--delayCheckSpawnPointsInAir;
	if(g_bEditMode && m_bEditorCheckSpawnPointAtStart && delayCheckSpawnPointsInAir ==0)
	{
		m_bEditorCheckSpawnPointAtStart = false;
		for(ITEM_SPAWN_POINT_VECTOR::iterator it=m_SpawnPointsV.begin(); it!=m_SpawnPointsV.end(); ++it)
		{
			PxVec3 from(it->pos.x, it->pos.y+1.0f, it->pos.z);
			PxRaycastHit hit;
			PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC));
			if(g_pPhysicsWorld->raycastSingle(from, PxVec3(0,-1,0), 10000, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter))
			{
				r3dPoint3D hitPos(hit.impact.x, hit.impact.y, hit.impact.z);
				if(R3D_ABS(it->pos.y - hitPos.y) > 2.0f)
				{
					r3dArtBug("Item CP Spawn at %.2f, %.2f, %.2f has spawn position(s) in the air. Please select it and use Check Locations button\n", 
						GetPosition().x, GetPosition().y, GetPosition().z);
					break;
				}
			}
		}
		r3dShowArtBugs();
	}

	return parent::Update();
}

//------------------------------------------------------------------------
struct ItemControlPointShadowGBufferRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ItemControlPointShadowGBufferRenderable *This = static_cast<ItemControlPointShadowGBufferRenderable*>( RThis );
		This->Parent->MeshGameObject::Draw( Cam, This->DrawState );
	}

	obj_ItemSpawnPoint*	Parent;
	eRenderStageID		DrawState;
};

struct ItemControlPointCompositeRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ItemControlPointCompositeRenderable *This = static_cast<ItemControlPointCompositeRenderable*>( RThis );

		This->Parent->DoDrawComposite( Cam );
	}

	obj_ItemSpawnPoint*	Parent;	
};



#define RENDERABLE_CTRLPOINT_SORT_VALUE (6*RENDERABLE_USER_SORT_VALUE)

void obj_ItemSpawnPoint::AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) /*OVERRIDE*/
{
#ifndef FINAL_BUILD
	if(!g_bEditMode)
#endif
		return;

	ItemControlPointShadowGBufferRenderable rend;

	rend.Init();
	rend.Parent		= this;
	rend.SortValue	= RENDERABLE_CTRLPOINT_SORT_VALUE;
	rend.DrawState	= rsCreateSM;

	rarr.PushBack( rend );
}

//------------------------------------------------------------------------
void obj_ItemSpawnPoint::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
#ifndef FINAL_BUILD
	if(!g_bEditMode)
#endif
		return;

	// gbuffer
	{
		ItemControlPointShadowGBufferRenderable rend;

		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_CTRLPOINT_SORT_VALUE;
		rend.DrawState	= rsFillGBuffer;

		render_arrays[ rsFillGBuffer ].PushBack( rend );
	}

#ifndef FINAL_BUILD
	if(r_hide_icons->GetInt())
		return;

	// composite
	if( g_bEditMode && r_show_item_spawns->GetBool() )
	{
		ItemControlPointCompositeRenderable rend;

		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_CTRLPOINT_SORT_VALUE;

		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}
#endif
}

//------------------------------------------------------------------------
void obj_ItemSpawnPoint::DoDrawComposite( const r3dCamera& Cam )
{
#ifndef FINAL_BUILD
	r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH | R3D_BLEND_NOALPHA);
	
	r3dDrawLine3D(GetPosition(), GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, editor_spawnPointBoxColor);

	// draw circles
	for(size_t i=0; i<m_SpawnPointsV.size(); ++i)
	{
		//r3dDrawCircle3D(m_SpawnPointsV[i].pos, 2.0f, Cam, 0.4f, ((i==m_SelectedSpawnPoint&&g_Manipulator3d.PickedObject() == this)?r3dColor24::red:r3dColor24::grey));
        //r3dDrawLine3D(m_SpawnPointsV[i].pos-r3dPoint3D(0.25f,0,0), m_SpawnPointsV[i].pos+r3dPoint3D(0.25f,0,0), Cam, 0.5f, ((i==m_SelectedSpawnPoint&&g_Manipulator3d.PickedObject() == this)?r3dColor24::red:r3dColor24::grey));
		r3dBoundBox box = m_SpawnPointsV[i].GetDebugBBox();
		float dist = (gCam - box.Center()).Length();
		if (dist < 150.0f)
			r3dDrawBoundBox(box, Cam, ((i==m_SelectedSpawnPoint&&g_Manipulator3d.PickedObject() == this)?r3dColor24::red:editor_spawnPointBoxColor), 0.03f, false );
	}
	r3dRenderer->Flush();
	r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
#endif
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD
struct tempS
{
	char* name;
	uint32_t id;
};
static bool SortLootboxesByName(const tempS d1, const tempS d2)
{
	return stricmp(d1.name, d2.name)<0;
}

float obj_ItemSpawnPoint::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{	
		starty += imgui_Static ( scrx, starty, "Loot box analysis" );
		static float					m_LootBoxAnalysisPeriodInHours=1;
		starty += imgui_Value_Slider(scrx, starty, "Analysis Window (hours)", &m_LootBoxAnalysisPeriodInHours, 1.0f, 744, "%.1f");
		if(imgui_Button(scrx, starty, 200, 25, "Analyze"))
		{
			AnalyzeLootBoxData(m_LootBoxAnalysisPeriodInHours);
		}
		starty+=25;

		starty += imgui_Static ( scrx, starty, "Item Spawn Point Parameters" );
		starty += imgui_Value_Slider(scrx, starty, "Tick Period (sec)", &m_TickPeriod, 1.0f, 50000.0f, "%.2f");
		starty += imgui_Value_Slider(scrx, starty, "Cooldown (sec)", &m_Cooldown, 1.0f, 50000.0f, "%.2f");
		starty += imgui_Value_Slider(scrx, starty, "De-spawn (sec)", &m_DestroyItemTimer, 0.0f, 50000.0f, "%.2f");
		starty += imgui_DrawColorPicker(scrx, starty, "Item Spawn Color", &editor_spawnPointBoxColor, 250, false);
		int isOneItemSpawn = m_OneItemSpawn?1:0;
		starty += imgui_Checkbox(scrx, starty, "One Item Spawn", &isOneItemSpawn, 1);
		m_OneItemSpawn = isOneItemSpawn?true:false;
		int isSpawnLootCrate = m_SpawnLootCrate?1:0;
		starty += imgui_Checkbox(scrx, starty, "Spawn Loot Crate", &isSpawnLootCrate, 1);
		m_SpawnLootCrate = isSpawnLootCrate?true:false;
		int isUniqueSpawnSystem = m_UniqueSpawnSystem?1:0;
		starty += imgui_Checkbox(scrx, starty, "Unique Spawn System", &isUniqueSpawnSystem, 1);
		m_UniqueSpawnSystem = isUniqueSpawnSystem?true:false;
		
		if(!m_OneItemSpawn)
		{
			static stringlist_t lootBoxNames;
			static int* lootBoxIDs = NULL;
			static int numLootBoxes = 0;
			if(numLootBoxes == 0)
			{
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
				// sort loot boxes by name (but keep first entry as EMPTY)
				if(lootBoxes.size()>3)
					std::sort(lootBoxes.begin()+1, lootBoxes.end(), SortLootboxesByName);

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
				if(m_LootBoxID == lootBoxIDs[i])
					sel = i;
			starty += imgui_Static ( scrx, starty, "Loot box:" );
			if(imgui_DrawList(scrx, starty, 360, 122, lootBoxNames, &offset, &sel))
			{
				m_LootBoxID = lootBoxIDs[sel];
				PropagateChange( m_LootBoxID, &obj_ItemSpawnPoint::m_LootBoxID, this, selected ) ;
			}
			starty += 122;
		}
		else
		{
			starty += imgui_Value_SliderI(scrx, starty, "ItemID", (int*)&m_LootBoxID, 0, 1000000, "%d", false);
			PropagateChange( m_LootBoxID, &obj_ItemSpawnPoint::m_LootBoxID, this, selected ) ;
		}
		
		// don't allow multi edit of this
		if( selected.Count() <= 1 )
		{
			{
				if(imgui_Button(scrx, starty, 200, 25, "Check locations"))
				{
					for(ITEM_SPAWN_POINT_VECTOR::iterator it=m_SpawnPointsV.begin(); it!=m_SpawnPointsV.end(); ++it)
					{
						PxVec3 from(it->pos.x, it->pos.y+1.0f, it->pos.z);
						PxRaycastHit hit;
						PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC));
						if(g_pPhysicsWorld->raycastSingle(from, PxVec3(0,-1,0), 10000, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter))
						{
							r3dPoint3D hitPos(hit.impact.x, hit.impact.y, hit.impact.z);
							if(R3D_ABS(it->pos.y - hitPos.y) > 2.0f)
							{
								gCam = it->pos + r3dPoint3D(0,1,0);
								extern int CurHUDID;
								Hud[CurHUDID].SetCamPos(gCam);
								break;
							}
						}
					}
				}
				starty += 30;
			}

			if(imgui_Button(scrx+110, starty, 100, 25, "Add Location"))
			{
				ItemSpawn itemSpawn;
				itemSpawn.pos = GetPosition() + r3dPoint3D(2, 0, 2);
				m_SpawnPointsV.push_back(itemSpawn);
				m_SelectedSpawnPoint = m_SpawnPointsV.size()-1;
			}

			starty += 25;

			int i=0;
			for(ITEM_SPAWN_POINT_VECTOR::iterator it=m_SpawnPointsV.begin(); it!=m_SpawnPointsV.end(); )
			{
				// selection button
				char tempStr[32];
				sprintf(tempStr, "Location %d", i+1);
				if(imgui_Button(scrx, starty, 100, 25, tempStr, i==m_SelectedSpawnPoint))
				{
					// shift click on location will set camera to it
					if(Keyboard->IsPressed(kbsLeftShift))
					{
						extern BaseHUD* HudArray[6];
						extern int CurHUDID;
						HudArray[CurHUDID]->FPS_Position = m_SpawnPointsV[i].pos;
						HudArray[CurHUDID]->FPS_Position.y += 0.1f;
					}
					
					m_SelectedSpawnPoint = i;
				}

				// delete button
				if(m_SpawnPointsV.size() > 1)
				{
					if(imgui_Button(scrx + 110, starty, 100, 25, "DEL"))
					{
						it = m_SpawnPointsV.erase(it);
						continue;
					}
					m_SelectedSpawnPoint = R3D_CLAMP(m_SelectedSpawnPoint, 0, (int)m_SpawnPointsV.size()-1);
				}

				starty += 26;

				++it;
				++i;
			}

			extern r3dPoint3D UI_TargetPos;
			extern gobjid_t UI_TargetObjID;
			if((Mouse->IsPressed(r3dMouse::mLeftButton)) && Keyboard->IsPressed(kbsLeftControl) && UI_TargetObjID != this->ID && !m_SpawnPointsV.empty())
				m_SpawnPointsV[m_SelectedSpawnPoint].pos = UI_TargetPos;

			if (Mouse->WasPressed(r3dMouse::mLeftButton))
			{
				if (Mouse->IsPressed(r3dMouse::mLeftButton) && Keyboard->IsPressed(kbsLeftControl) && Keyboard->IsPressed(kbsLeftShift))
				{
					ItemSpawn itemSpawn;
					itemSpawn.pos = UI_TargetPos;
					m_SpawnPointsV.push_back(itemSpawn);
					m_SelectedSpawnPoint = m_SpawnPointsV.size()-1;
				}
			}
			if((Mouse->IsPressed(r3dMouse::mLeftButton)) && Keyboard->IsPressed(kbsLeftControl) && UI_TargetObjID != this->ID && !m_SpawnPointsV.empty())
				m_SpawnPointsV[m_SelectedSpawnPoint].pos = UI_TargetPos;

			starty += imgui_Static(scrx, starty, "Tip: SHIFT-Location button click - navigate to location");
			starty += imgui_Static(scrx, starty, "Tip: CTRL-click - reposition current location");
		}
	}

	return starty-scry;
}

struct ItemLoot_Stat
{
	uint32_t itemID;
	uint32_t lootBoxID;
	uint32_t numSpawns;
	uint32_t numLootboxesSpawns;
};
struct Item_Stat
{
	uint32_t itemID;
	uint32_t numSpawns;
};
bool operator == (ItemLoot_Stat& s1, const ItemLoot_Stat& s2)
{
	return (s1.itemID == s2.itemID && s1.lootBoxID == s2.lootBoxID);
}
bool operator == (Item_Stat& s1, const Item_Stat& s2)
{
	return (s1.itemID == s2.itemID);
}
bool SortLootboxStats_ByLootbox(const ItemLoot_Stat& d1, const ItemLoot_Stat& d2)
{
	if(d1.lootBoxID != d2.lootBoxID)
		return d1.lootBoxID < d2.lootBoxID;
	return d1.numSpawns < d2.numSpawns;
}
bool SortLootboxStats_ByItemID(const Item_Stat& d1, const Item_Stat& d2)
{
	return d1.numSpawns < d2.numSpawns;
}


void obj_ItemSpawnPoint::AnalyzeLootBoxData(float analyzeWindowInHours)
{
	// firstly update loot box content
	{
		CkHttp http;
		int success = http.UnlockComponent("ARKTOSHttp_decCLPWFQXmU");
		if (success != 1) 
			r3dError("Internal error!!!");

		CkHttpRequest req;
		req.UsePost();
		req.put_Path("/WarZ/api/php/api_GetLootBoxConfig.php");
		req.AddParam("serverkey", "Fg5jaBgj3uy3ja");

		CkHttpResponse* resp = http.SynchronousRequest("apiwarz.kongsi.asia", 80, false, req);
		if(!resp)
		{
			r3dOutToLog("!!! timeout getting lootbox db");
			return;
		}

		// we can't use getBosyStr() because it'll fuckup characters inside UTF-8 xml
		CkByteData bodyData;
		resp->get_Body(bodyData);
		delete resp;

		// note, not _inplace function, fmlFile should have it own buffer
		pugi::xml_document out_xmlFile;
		pugi::xml_parse_result parseResult = out_xmlFile.load_buffer(
			(void*)bodyData.getBytes(), 
			bodyData.getSize(), 
			pugi::parse_default, 
			pugi::encoding_utf8);
		if(!parseResult)
		{
			r3dOutToLog("!!! Failed to parse lootbox db, error: %s", parseResult.description());
			return;
		}

		g_pWeaponArmory->updateLootBoxContent(out_xmlFile.child("LootBoxDB"));
	}

	int currentTime = 0;
	int maxTime = int(analyzeWindowInHours*60.0f*60.0f); // convert to sec
	std::vector<obj_ItemSpawnPoint*>::iterator it;

	// reset stats before run
	for(it=m_LootBoxArray.begin(); it!=m_LootBoxArray.end(); ++it)
	{
		obj_ItemSpawnPoint* spawnPoint = *it;
		spawnPoint->m_LootBoxAnalysis_NextUpdateTime = 0;
		spawnPoint->m_LootBoxAnalysis_LootBoxConfig = NULL;
		if(!spawnPoint->m_OneItemSpawn)
		{
			spawnPoint->m_LootBoxAnalysis_LootBoxConfig = const_cast<LootBoxConfig*>(g_pWeaponArmory->getLootBoxConfig(spawnPoint->m_LootBoxID));
			if(spawnPoint->m_LootBoxAnalysis_LootBoxConfig == NULL)
			{
				r3dOutToLog("LootBox: !!!! Failed to get lootbox with ID: %d.\n", spawnPoint->m_LootBoxID);
				//return;
			}
			else if(spawnPoint->m_LootBoxAnalysis_LootBoxConfig->entries.size() == 0)
			{
				r3dOutToLog("LootBox: !!!! m_LootBoxID %d does NOT have items inside\n", spawnPoint->m_LootBoxID);
				//return;
			}
		}
		for(size_t i=0; i<spawnPoint->m_SpawnPointsV.size(); ++i)
		{
			ItemSpawn& spawn = spawnPoint->m_SpawnPointsV[i];
			spawn.cooldown = 0;
		}
	}

	// item spawn stats
	std::vector<ItemLoot_Stat> Stats_By_Lootboxes; // brute force :)
	std::vector<Item_Stat> Stats_By_ItemID;

	// logic is simple. iterate all lootboxes and simulate game time, by going 1 sec at a time
	for(currentTime=0; currentTime<maxTime; ++currentTime)
	{
		for(it=m_LootBoxArray.begin(); it!=m_LootBoxArray.end(); ++it)
		{
			obj_ItemSpawnPoint* spawnPoint = *it;

			if(currentTime > spawnPoint->m_LootBoxAnalysis_NextUpdateTime)
			{
				spawnPoint->m_LootBoxAnalysis_NextUpdateTime = currentTime + (int)spawnPoint->m_TickPeriod;

				if(!spawnPoint->m_OneItemSpawn)
				{
					if(spawnPoint->m_LootBoxAnalysis_LootBoxConfig == NULL)
						continue;
					if(spawnPoint->m_LootBoxAnalysis_LootBoxConfig->entries.size() == 0)
						continue;
				}

				if(!spawnPoint->m_UniqueSpawnSystem)
				{
					for(size_t i=0; i<spawnPoint->m_SpawnPointsV.size(); ++i)
					{
						ItemSpawn& spawn = spawnPoint->m_SpawnPointsV[i];
						if(spawn.cooldown < currentTime) 
						{
							// roll item and mark it inside spawn
							wiInventoryItem wi;
							if(!spawnPoint->m_OneItemSpawn)
							{
								wi = RollItem(spawnPoint->m_LootBoxAnalysis_LootBoxConfig, 0);
							}
							else
							{
								wi.itemID   = spawnPoint->m_LootBoxID;
								wi.quantity = 1;
							}

							// check if somehow roll wasn't successful
							if(wi.itemID == 0)
							{
								spawn.cooldown = spawnPoint->m_Cooldown + currentTime; // mark that spawn point not to spawn anything until next cooldown
							}
							else
							{
								// item is spawned. check destroy timer and add random destroy timer to it to simulate destroy timer
								if(spawnPoint->m_DestroyItemTimer > 0.0f)
									spawn.cooldown = spawnPoint->m_Cooldown + currentTime + u_GetRandom(0.0f, spawnPoint->m_DestroyItemTimer);
								else // no destroy timer, simulate that player will pick up item in one hour
									spawn.cooldown = spawnPoint->m_Cooldown + currentTime + 1*60*60;

								// add stat for it
								if(!spawnPoint->m_OneItemSpawn)
								{
									ItemLoot_Stat newStat; newStat.itemID = wi.itemID; newStat.lootBoxID = spawnPoint->m_LootBoxID; newStat.numSpawns = 1; newStat.numLootboxesSpawns = spawnPoint->m_SpawnPointsV.size();
									std::vector<ItemLoot_Stat>::iterator it = std::find(Stats_By_Lootboxes.begin(), Stats_By_Lootboxes.end(), newStat);
									if(it == Stats_By_Lootboxes.end())
										Stats_By_Lootboxes.push_back(newStat);
									else
										it->numSpawns++;
								}

								Item_Stat newStat2; newStat2.itemID = wi.itemID; newStat2.numSpawns = 1;
								std::vector<Item_Stat>::iterator it2 = std::find(Stats_By_ItemID.begin(), Stats_By_ItemID.end(), newStat2);
								if(it2 == Stats_By_ItemID.end())
									Stats_By_ItemID.push_back(newStat2);
								else
									it2->numSpawns++;
							}
							break; // only one item spawn per tickPeriod
						}
					}
				}
				else
				{
					bool SpawnSystemReady = true;
					for(size_t i=0; i<spawnPoint->m_SpawnPointsV.size(); ++i)
					{
						ItemSpawn& spawn = spawnPoint->m_SpawnPointsV[i];
						if(spawn.itemID || spawn.cooldown > currentTime)
						{
							SpawnSystemReady = false;
							break;
						}
					}
					if(SpawnSystemReady && spawnPoint->m_SpawnPointsV.size()>0)
					{
						int p = (int)u_GetRandom(0.0f, float(spawnPoint->m_SpawnPointsV.size()-1));
						ItemSpawn& spawn = spawnPoint->m_SpawnPointsV[p];
						{
							// roll item and mark it inside spawn
							wiInventoryItem wi;
							if(!spawnPoint->m_OneItemSpawn)
							{
								wi = RollItem(spawnPoint->m_LootBoxAnalysis_LootBoxConfig, 0);
							}
							else
							{
								wi.itemID   = spawnPoint->m_LootBoxID;
								wi.quantity = 1;
							}

							// check if somehow roll wasn't successful
							if(wi.itemID == 0)
							{
								spawn.cooldown = spawnPoint->m_Cooldown + currentTime; // mark that spawn point not to spawn anything until next cooldown
							}
							else
							{
								// item is spawned. check destroy timer and add random destroy timer to it to simulate destroy timer
								if(spawnPoint->m_DestroyItemTimer > 0.0f)
									spawn.cooldown = spawnPoint->m_Cooldown + currentTime + u_GetRandom(0.0f, spawnPoint->m_DestroyItemTimer);
								else // no destroy timer, simulate that player will pick up item in one hour
									spawn.cooldown = spawnPoint->m_Cooldown + currentTime + 1*60*60;

								// add stat for it
								if(!spawnPoint->m_OneItemSpawn)
								{
									ItemLoot_Stat newStat; newStat.itemID = wi.itemID; newStat.lootBoxID = spawnPoint->m_LootBoxID; newStat.numSpawns = 1; newStat.numLootboxesSpawns = spawnPoint->m_SpawnPointsV.size();
									std::vector<ItemLoot_Stat>::iterator it = std::find(Stats_By_Lootboxes.begin(), Stats_By_Lootboxes.end(), newStat);
									if(it == Stats_By_Lootboxes.end())
										Stats_By_Lootboxes.push_back(newStat);
									else
										it->numSpawns++;
								}

								Item_Stat newStat2; newStat2.itemID = wi.itemID; newStat2.numSpawns = 1;
								std::vector<Item_Stat>::iterator it2 = std::find(Stats_By_ItemID.begin(), Stats_By_ItemID.end(), newStat2);
								if(it2 == Stats_By_ItemID.end())
									Stats_By_ItemID.push_back(newStat2);
								else
									it2->numSpawns++;
							}
						}
					}
				}
			}
		}
	}

	// print results
	{
		FILE* resf = fopen_for_write("loot_by_lootbox.txt", "w");
		std::sort(Stats_By_Lootboxes.begin(), Stats_By_Lootboxes.end(), SortLootboxStats_ByLootbox);
		if(!Stats_By_Lootboxes.empty())
		{
			uint32_t lastI = 0;
			uint32_t lastID = Stats_By_Lootboxes[0].lootBoxID;
			uint32_t totalSpawns = 0;
			uint32_t i=0;
			for(; i<Stats_By_Lootboxes.size(); ++i)
			{
				if(lastID != Stats_By_Lootboxes[i].lootBoxID)
				{
					for(uint32_t j=lastI; j<i; ++j)
					{
						Stats_By_Lootboxes[j].numLootboxesSpawns = totalSpawns;
					}
					lastID = Stats_By_Lootboxes[i].lootBoxID;
					lastI = i;
					totalSpawns = Stats_By_Lootboxes[i].numLootboxesSpawns;
				}
				else
				{
					totalSpawns += Stats_By_Lootboxes[i].numLootboxesSpawns;
				}
			}
			// and do it for the last batch
			for(uint32_t j=lastI; j<i; ++j)
			{
				Stats_By_Lootboxes[j].numLootboxesSpawns = totalSpawns;
			}
		}
		for(std::vector<ItemLoot_Stat>::iterator it = Stats_By_Lootboxes.begin(); it!=Stats_By_Lootboxes.end(); ++it)
		{
			char tmpStr[2048];
			sprintf(tmpStr, "Lootbox (%d): %s, Item: %s, Spawned: %d\n", it->numLootboxesSpawns, g_pWeaponArmory->getConfig(it->lootBoxID)->m_StoreName, it->itemID=='GOLD'?"GOLD":g_pWeaponArmory->getConfig(it->itemID)->m_StoreName, it->numSpawns);
			fwrite(tmpStr, 1, strlen(tmpStr), resf);
		}
		fclose(resf);
	}
	{
		FILE* resf = fopen_for_write("loot_by_itemID.txt", "w");
		std::sort(Stats_By_ItemID.begin(), Stats_By_ItemID.end(), SortLootboxStats_ByItemID);
		for(std::vector<Item_Stat>::iterator it = Stats_By_ItemID.begin(); it!=Stats_By_ItemID.end(); ++it)
		{
			char tmpStr[2048];
			sprintf(tmpStr, "Item: %s, Spawned: %d\n", it->itemID=='GOLD'?"GOLD":g_pWeaponArmory->getConfig(it->itemID)->m_StoreName, it->numSpawns);
			fwrite(tmpStr, 1, strlen(tmpStr), resf);
		}
		fclose(resf);
	}
	MessageBox(NULL, "Analyze completed!\nPlease look for files loot_by_lootbox.txt and loot_by_itemID.txt in your Bin folder", "DONE!", MB_OK);
}

#endif

BOOL obj_ItemSpawnPoint::OnNetReceive(DWORD EventID, const void* packetData, int packetSize)
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

void obj_ItemSpawnPoint::OnPickerPick(int locationId)
{
	if (locationId >= 0 && locationId < static_cast<int>(m_SpawnPointsV.size()))
	{
		m_SelectedSpawnPoint = locationId;
	}
}

void obj_ItemSpawnPoint::ReadSerializedData(pugi::xml_node& node)
{
	BaseItemSpawnPoint::ReadSerializedData(node);
	pugi::xml_node cpEditorNode = node.child("Editor_ItemSpawnPoint");
	if(!cpEditorNode.empty())
	{
		editor_spawnPointBoxColor.R = cpEditorNode.attribute("Color.r").as_int();
		editor_spawnPointBoxColor.G = cpEditorNode.attribute("Color.g").as_int();
		editor_spawnPointBoxColor.B = cpEditorNode.attribute("Color.b").as_int();
	}
}

void obj_ItemSpawnPoint::WriteSerializedData(pugi::xml_node& node)
{
	BaseItemSpawnPoint::WriteSerializedData(node);
	pugi::xml_node cpEditorNode = node.append_child();
	cpEditorNode.set_name("Editor_ItemSpawnPoint");
	cpEditorNode.append_attribute("Color.r") = editor_spawnPointBoxColor.R;
	cpEditorNode.append_attribute("Color.g") = editor_spawnPointBoxColor.G;
	cpEditorNode.append_attribute("Color.b") = editor_spawnPointBoxColor.B;
}

GameObject *obj_ItemSpawnPoint::Clone()
{
	obj_ItemSpawnPoint * pNew = (obj_ItemSpawnPoint*)srv_CreateGameObject("obj_ItemSpawnPoint", FileName.c_str(), GetPosition () );
	GameObject::PostCloneParamsCopy(pNew);
	pNew->editor_spawnPointBoxColor = editor_spawnPointBoxColor;
	pNew->m_TickPeriod = m_TickPeriod;
	pNew->m_Cooldown = m_Cooldown;
	pNew->m_LootBoxID = m_LootBoxID;
	pNew->m_OneItemSpawn = m_OneItemSpawn;
	pNew->m_DestroyItemTimer = m_DestroyItemTimer;
	pNew->m_SpawnLootCrate = m_SpawnLootCrate;
	pNew->m_UniqueSpawnSystem = m_UniqueSpawnSystem;

	return pNew;
}
