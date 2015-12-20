#include "r3dPCH.h"
#include "r3d.h"

#include "Gameplay_Params.h"
#include "GameCommon.h"

#include "obj_VehicleSpawnPoint.h"
#include "../../XMLHelpers.h"
#include "gameobjects/obj_Mesh.h"

#include "multiplayer/P2PMessages.h"
#include "multiplayer/ClientGameLogic.h"
#include "../WEAPONS/WeaponArmory.h"

#include "Editors/ObjectManipulator3d.h"

IMPLEMENT_CLASS(obj_VehicleSpawnPoint, "obj_VehicleSpawnPoint", "Object");
AUTOREGISTER_CLASS(obj_VehicleSpawnPoint);

extern bool g_bEditMode;

obj_VehicleSpawnPoint::obj_VehicleSpawnPoint()
	: maxSpawns(0)
{
	editorSpawnPointBoxColor = r3dColor::yellow;

	ObjTypeFlags |= OBJTYPE_VehicleSpawnPoint;
	selectedSpawnPoint = 0;
	isEditorCheckSpawnPointAtStart = true;
}

obj_VehicleSpawnPoint::~obj_VehicleSpawnPoint()
{
}

BOOL obj_VehicleSpawnPoint::Load(const char* fname)
{
#ifndef FINAL_BUILD
	if (!g_bEditMode)
#endif
	{
		return TRUE;
	}

	const char* meshName = "Data\\ObjectsDepot\\Capture_Points\\Flag_Pole_01.sco";

	if (!parent::Load(meshName))
		return FALSE;

	return TRUE;
}

BOOL obj_VehicleSpawnPoint::OnCreate()
{
	parent::OnCreate();

#ifndef FINAL_BUILD
	if (!g_bEditMode)
#endif
		ObjFlags |= OBJFLAG_SkipCastRay;

#ifndef FINAL_BUILD
	if (g_bEditMode)
	{
		setSkipOcclusionCheck(true);
		ObjFlags |= OBJFLAG_AlwaysDraw | OBJFLAG_ForceSleep;
	}
#endif

	UpdateTransform();

	return TRUE;
}

BOOL obj_VehicleSpawnPoint::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_VehicleSpawnPoint::Update()
{
	static int delayCheckSpawnPointsInAir = 1000;
	if (delayCheckSpawnPointsInAir)
		--delayCheckSpawnPointsInAir;

	if (g_bEditMode && isEditorCheckSpawnPointAtStart && delayCheckSpawnPointsInAir == 0)
	{
		isEditorCheckSpawnPointAtStart = false;

		for (ITEM_SPAWN_POINT_VECTOR::iterator it = m_SpawnPointsV.begin(); it != m_SpawnPointsV.end(); ++it)
		{
			PxVec3 from(it->pos.x, it->pos.y + 1.0f, it->pos.z);
			PxRaycastHit hit;
			PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC));
			if (g_pPhysicsWorld->raycastSingle(from, PxVec3(0, -1, 0), 10000, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter))
			{
				r3dPoint3D hitPosition(hit.impact.x, hit.impact.y, hit.impact.z);
				if (R3D_ABS(it->pos.y - hitPosition.y) > 2.0f)
				{
					r3dArtBug("Vehicle Spawn at %.2f, %.2f, %.2f has spawn position in the air. Please select it and use Check Locations button\n",
						GetPosition().x, GetPosition().y, GetPosition().z);
					break;
				}
			}
		}

		r3dShowArtBugs();
	}

	return parent::Update();
}

struct VehicleControlPointShadowGBufferRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw(Renderable* renderable, const r3dCamera& camera)
	{
		VehicleControlPointShadowGBufferRenderable *This = static_cast<VehicleControlPointShadowGBufferRenderable*>(renderable);
		This->spawnPoint->MeshGameObject::Draw(camera, This->DrawState);
	}

	obj_VehicleSpawnPoint* spawnPoint;
	eRenderStageID DrawState;
};

struct VehicleControlPointCompositeRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw(Renderable* renderable, const r3dCamera& camera)
	{
		VehicleControlPointCompositeRenderable *This = static_cast<VehicleControlPointCompositeRenderable*>(renderable);
		This->spawnPoint->DoDrawComposite(camera);
	}

	obj_VehicleSpawnPoint* spawnPoint;
};

#define RENDERABLE_CTRLPOINT_SORT_VALUE (6*RENDERABLE_USER_SORT_VALUE)

void obj_VehicleSpawnPoint::AppendShadowRenderables(RenderArray& renderArray, int sliceIndex, const r3dCamera& camera)
{
#ifndef FINAL_BUILD
	if (!g_bEditMode)
#endif
		return;

	VehicleControlPointShadowGBufferRenderable renderable;

	renderable.Init();
	renderable.spawnPoint = this;
	renderable.SortValue = RENDERABLE_CTRLPOINT_SORT_VALUE;
	renderable.DrawState = rsCreateSM;

	renderArray.PushBack(renderable);
}

void obj_VehicleSpawnPoint::AppendRenderables(RenderArray(&renderArrays)[rsCount], const r3dCamera& camera)
{
#ifndef FINAL_BUILD
	if(!g_bEditMode)
#endif
		return;

	{
		VehicleControlPointShadowGBufferRenderable renderable;
		renderable.Init();
		renderable.spawnPoint = this;
		renderable.SortValue = RENDERABLE_CTRLPOINT_SORT_VALUE;
		renderable.DrawState = rsFillGBuffer;

		renderArrays[rsFillGBuffer].PushBack(renderable);
	}

#ifndef FINAL_BUILD
	if (r_hide_icons->GetInt())
		return;

	if (g_bEditMode && r_show_item_spawns->GetBool())
	{
		VehicleControlPointCompositeRenderable renderable;
		renderable.Init();
		renderable.spawnPoint = this;
		renderable.SortValue = RENDERABLE_CTRLPOINT_SORT_VALUE;

		renderArrays[rsDrawDebugData].PushBack(renderable);
	}
#endif
}

void obj_VehicleSpawnPoint::DoDrawComposite(const r3dCamera& camera)
{
#ifndef FINAL_BUILD
	r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH | R3D_BLEND_NOALPHA);
	r3dDrawLine3D(GetPosition(), GetPosition() + r3dPoint3D(0, 20.f, 0), camera, 0.4f, editorSpawnPointBoxColor);

	for (size_t i = 0; i < m_SpawnPointsV.size(); ++i)
	{
		r3dBoundBox boundBox = m_SpawnPointsV[i].GetDebugBBox();
		r3dDrawBoundBox(boundBox, camera, ((i == selectedSpawnPoint && g_Manipulator3d.PickedObject() == this)) ? r3dColor24::red : editorSpawnPointBoxColor, 0.01f, false);
	}

	r3dRenderer->Flush();
	r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
#endif
}

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
float obj_VehicleSpawnPoint::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float startY = scry;

	startY += parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected);

	if (IsParentOrEqual(&ClassData, startClass))
	{
		startY += imgui_Static(scrx, startY, "Vehicle Spawn Point Parameters");
		startY += imgui_Value_Slider(scrx, startY, "Tick Period (sec)", &m_TickPeriod, 1.0f, 50000.0f, "%.2f");
		startY += imgui_Value_Slider(scrx, startY, "Cooldown (sec)", &m_Cooldown, 1.0f, 50000.0f, "%.2f");
		startY += imgui_Value_SliderI(scrx, startY,"Max Spawns", &maxSpawns, 1, 50, "%d");
		startY += imgui_DrawColorPicker(scrx, startY, "Item Spawn Color", &editorSpawnPointBoxColor, 250, false);

		static stringlist_t lootBoxNames;
		static int* lootBoxIDs = NULL;
		static int numLootBoxes = 0;

		if (numLootBoxes == 0)
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
				uint32_t itemId = g_pWeaponArmory->getCurrentSearchItemID();
				const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(itemId);
				if (cfg->category == storecat_LootBox)
				{
					tempS holder;
					holder.name = cfg->m_StoreName;
					holder.id = cfg->m_itemID;
					lootBoxes.push_back(holder);
				}
			}

			if (lootBoxes.size() > 3)
				std::sort(lootBoxes.begin() + 1, lootBoxes.end(), SortLootboxesByName);

			numLootBoxes = (int)lootBoxes.size();
			lootBoxIDs = game_new int[numLootBoxes];

			for (int i = 0; i < numLootBoxes; ++i)
			{
				lootBoxNames.push_back(lootBoxes[i].name);
				lootBoxIDs[i] = lootBoxes[i].id;
			}
		}

		int sel = 0;
		static float offset = 0;
		for (int i = 0; i < numLootBoxes; ++i)
		{
			if (m_LootBoxID == lootBoxIDs[i])
				sel = i;
		}

		startY += imgui_Static(scrx, startY, "Loot Box:");
		if (imgui_DrawList(scrx, startY, 360, 122, lootBoxNames, &offset, &sel))
		{
			m_LootBoxID = lootBoxIDs[sel];
			PropagateChange(m_LootBoxID, &obj_VehicleSpawnPoint::m_LootBoxID, this, selected);
		}

		startY += 122;
	}

	if (selected.Count() <= 1)
	{
		if (imgui_Button(scrx, startY, 200, 25, "Check Locations"))
		{
			for (ITEM_SPAWN_POINT_VECTOR::iterator it = m_SpawnPointsV.begin(); it != m_SpawnPointsV.end(); ++it)
			{
				PxVec3 from(it->pos.x, it->pos.y + 1.0f, it->pos.z);
				PxRaycastHit hit;
				PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC));
				if (g_pPhysicsWorld->raycastSingle(from, PxVec3(0, -1, 0), 10000, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter))
				{
					r3dPoint3D hitPosition(hit.impact.x, hit.impact.y, hit.impact.z);
					if (R3D_ABS(it->pos.y - hitPosition.y) > 2.0f)
					{
						gCam = it->pos + r3dPoint3D(0, 1, 0);
						extern int CurHUDID;
						Hud[CurHUDID].SetCamPos(gCam);
						break;
					}
				}
			}
		}

		startY += 30; 

		if (imgui_Button(scrx + 110, startY, 100, 25, "Add Location"))
		{
			ItemSpawn vehicleSpawn;
			vehicleSpawn.pos = GetPosition() + r3dPoint3D(2, 0, 2);
			m_SpawnPointsV.push_back(vehicleSpawn);

			selectedSpawnPoint = m_SpawnPointsV.size() - 1;
		}

		startY += 25;

		int i = 0;

		for (ITEM_SPAWN_POINT_VECTOR::iterator it = m_SpawnPointsV.begin(); it != m_SpawnPointsV.end();)
		{
			char tempStr[32];
			sprintf(tempStr, "Location %d", i + 1);

			if (imgui_Button(scrx, startY, 100, 25, tempStr, i == selectedSpawnPoint))
			{
				if (Keyboard->IsPressed(kbsLeftShift))
				{
					extern BaseHUD* HudArray[6];
					extern int CurHUDID;
					HudArray[CurHUDID]->FPS_Position = m_SpawnPointsV[i].pos;
					HudArray[CurHUDID]->FPS_Position.y += 0.1f;
				}

				selectedSpawnPoint = i;
			}

			if (m_SpawnPointsV.size() > 1)
			{
				if (imgui_Button(scrx + 110, startY, 100, 25, "DEL"))
				{
					it = m_SpawnPointsV.erase(it);
					continue;
				}

				selectedSpawnPoint = R3D_CLAMP(selectedSpawnPoint, 0, (int)m_SpawnPointsV.size() - 1);
			}

			startY += 26;

			++it;
			++i;
		}

		extern r3dPoint3D UI_TargetPos;
		extern gobjid_t UI_TargetObjID;

		if ((Mouse->IsPressed(r3dMouse::mLeftButton)) &&
			Keyboard->IsPressed(kbsLeftControl) &&
			UI_TargetObjID != this->ID &&
			!m_SpawnPointsV.empty())
		{
			m_SpawnPointsV[selectedSpawnPoint].pos = UI_TargetPos;
		}

		startY += imgui_Static(scrx, startY, "Tip: SHIFT-Location button click - navigate to location");
		startY += imgui_Static(scrx, startY, "Tip: CTRL-click - reposition current location");
	}

	return startY - scry;
}
#endif

BOOL obj_VehicleSpawnPoint::OnNetReceive(DWORD eventID, const void* packetData, int packetSize)
{
	return TRUE;
}

void obj_VehicleSpawnPoint::OnPickerPick(int locationId)
{
	if (locationId >= 0 && locationId < static_cast<int>(m_SpawnPointsV.size()))
	{
		selectedSpawnPoint = locationId;
	}
}

void obj_VehicleSpawnPoint::ReadSerializedData(pugi::xml_node& node)
{
	BaseItemSpawnPoint::ReadSerializedData(node);
	pugi::xml_node editorNode = node.child("Editor_VehicleSpawnNode");
	if (!editorNode.empty())
	{
		editorSpawnPointBoxColor.R = editorNode.attribute("Color.r").as_int();
		editorSpawnPointBoxColor.G = editorNode.attribute("Color.g").as_int();
		editorSpawnPointBoxColor.B = editorNode.attribute("Color.b").as_int();
	}

	pugi::xml_node spawnerNode = node.child("spawn_parameters");
	GetXMLVal("MaxSpawns", spawnerNode, &maxSpawns);

	uint32_t numVehicles = spawnerNode.attribute("numVehiclesSelected").as_uint();
	for (uint32_t i = 0; i < numVehicles; ++i)
	{
		char tempStr[32];
		sprintf(tempStr, "v%d", i);
		pugi::xml_node vNode = spawnerNode.child(tempStr);
		r3d_assert(!vNode.empty());
		VehicleSpawnSelection.push_back(vNode.attribute("id").as_uint());
	}
}

void obj_VehicleSpawnPoint::WriteSerializedData(pugi::xml_node& node)
{
	BaseItemSpawnPoint::WriteSerializedData(node);
	pugi::xml_node editorNode = node.append_child();
	editorNode.set_name("Editor_VehicleSpawnNode");
	editorNode.append_attribute("Color.r") = editorSpawnPointBoxColor.R;
	editorNode.append_attribute("Color.g") = editorSpawnPointBoxColor.G;
	editorNode.append_attribute("Color.b") = editorSpawnPointBoxColor.B;

	pugi::xml_node spawnerNode = node.append_child();
	spawnerNode.set_name("spawn_parameters");
	SetXMLVal("MaxSpawns", spawnerNode, &maxSpawns);

	spawnerNode.append_attribute("numVehiclesSelected") = VehicleSpawnSelection.size();
	for (size_t i = 0; i < VehicleSpawnSelection.size(); ++i)
	{
		pugi::xml_node vNode = spawnerNode.append_child();
		char tempStr[32];
		sprintf(tempStr, "v%d", i);
		vNode.set_name(tempStr);
		vNode.append_attribute("id") = VehicleSpawnSelection[i];
	}
}

GameObject *obj_VehicleSpawnPoint::Clone()
{
	obj_VehicleSpawnPoint* pNew = (obj_VehicleSpawnPoint*)srv_CreateGameObject("obj_VehicleSpawnPoint", FileName.c_str(), GetPosition());
	GameObject::PostCloneParamsCopy(pNew);
	pNew->editorSpawnPointBoxColor = editorSpawnPointBoxColor;
	pNew->m_TickPeriod = m_TickPeriod;
	pNew->m_Cooldown = m_Cooldown;
	pNew->m_LootBoxID = m_LootBoxID;

	return pNew;
}
