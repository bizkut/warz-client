//=========================================================================
//	Module: obj_AirDropSpawn.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "obj_AirDropSpawn.h"
#include "../../XMLHelpers.h"
#include "../../Editors/LevelEditor.h"
#include "../WEAPONS/WeaponArmory.h"
#include "../../../../GameEngine/ai/AI_Brain.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_AirDropSpawn, "obj_AirDropSpawn", "Object");
AUTOREGISTER_CLASS(obj_AirDropSpawn);

extern bool g_bEditMode;

//////////////////////////////////////////////////////////////////////////

namespace
{
//////////////////////////////////////////////////////////////////////////

	struct AirDropSpawnCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			AirDropSpawnCompositeRenderable *This = static_cast<AirDropSpawnCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);

			r3dDrawLine3D(This->Parent->GetPosition(), This->Parent->GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, r3dColor24::yellow);
			r3dDrawCircle3D(This->Parent->GetPosition(), This->Parent->spawnRadius, Cam, 0.1f, r3dColor::orange);

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}

		obj_AirDropSpawn *Parent;
	};
}

//////////////////////////////////////////////////////////////////////////

obj_AirDropSpawn::obj_AirDropSpawn()
: spawnRadius(30.0f)
{
	serializeFile = SF_ServerData;
	m_bEnablePhysics = false;
	m_DefaultItems = 1;
}

//////////////////////////////////////////////////////////////////////////

obj_AirDropSpawn::~obj_AirDropSpawn()
{

}

//////////////////////////////////////////////////////////////////////////

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_AirDropSpawn::AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam)
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

	AirDropSpawnCompositeRenderable rend;

	rend.Init();
	rend.Parent		= this;
	rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;

	render_arrays[ rsDrawDebugData ].PushBack( rend );
#endif
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_AirDropSpawn::OnCreate()
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

BOOL obj_AirDropSpawn::Update()
{
	return parent::Update();
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_AirDropSpawn::OnDestroy()
{
	return parent::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////

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
float obj_AirDropSpawn::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float y = scry;

	y += parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected);
	y += 5.0f;

	y += imgui_Static(scrx, y, "AirDrop parameters:");

	y += imgui_Value_Slider(scrx, y, "Radius", &spawnRadius, 5.0f, 400.0f, "%0.2f");
	y += imgui_Checkbox(scrx, y, "Use Default Items", &m_DefaultItems, 1);
///////////////////////////////
		if(m_DefaultItems == 0)
		{
			static stringlist_t lootBoxNames1,lootBoxNames2,lootBoxNames3,lootBoxNames4,lootBoxNames5,lootBoxNames6;
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
					lootBoxNames1.push_back(lootBoxes[i].name);
					lootBoxNames2.push_back(lootBoxes[i].name);
					lootBoxNames3.push_back(lootBoxes[i].name);
					lootBoxNames4.push_back(lootBoxes[i].name);
					lootBoxNames5.push_back(lootBoxes[i].name);
					lootBoxNames6.push_back(lootBoxes[i].name);
					lootBoxIDs[i] = lootBoxes[i].id;
				}
			}
			int sel = 0;
			int sel2 = 0;
			int sel3 = 0;
			int sel4 = 0;
			int sel5 = 0;
			int sel6 = 0;
			static float offset = 0;
			static float offset2 = 0;
			static float offset3 = 0;
			static float offset4 = 0;
			static float offset5 = 0;
			static float offset6 = 0;
			for(int i=0; i<numLootBoxes; ++i)
			{
				if(m_LootBoxID1 == lootBoxIDs[i])
					sel = i;
				if(m_LootBoxID2 == lootBoxIDs[i])
					sel2 = i;
				if(m_LootBoxID3 == lootBoxIDs[i])
					sel3 = i;
				if(m_LootBoxID4 == lootBoxIDs[i])
					sel4 = i;
				if(m_LootBoxID5 == lootBoxIDs[i])
					sel5 = i;
				if(m_LootBoxID6 == lootBoxIDs[i])
					sel6 = i;
			}
			y += imgui_Static ( scrx, y, "Loot box 1:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames1, &offset, &sel))
			{
				m_LootBoxID1 = lootBoxIDs[sel];
				PropagateChange( m_LootBoxID1, &obj_AirDropSpawn::m_LootBoxID1, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 2:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames2, &offset2, &sel2))
			{
				m_LootBoxID2 = lootBoxIDs[sel2];
				PropagateChange( m_LootBoxID2, &obj_AirDropSpawn::m_LootBoxID2, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 3:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames3, &offset3, &sel3))
			{
				m_LootBoxID3 = lootBoxIDs[sel3];
				PropagateChange( m_LootBoxID3, &obj_AirDropSpawn::m_LootBoxID3, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 4:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames4, &offset4, &sel4))
			{
				m_LootBoxID4 = lootBoxIDs[sel4];
				PropagateChange( m_LootBoxID4, &obj_AirDropSpawn::m_LootBoxID4, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 5:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames5, &offset5, &sel5))
			{
				m_LootBoxID5 = lootBoxIDs[sel5];
				PropagateChange( m_LootBoxID5, &obj_AirDropSpawn::m_LootBoxID5, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 6:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames6, &offset6, &sel6))
			{
				m_LootBoxID6 = lootBoxIDs[sel6];
				PropagateChange( m_LootBoxID6, &obj_AirDropSpawn::m_LootBoxID6, this, selected ) ;
			}
			y += 122;
		}
///////////////////////////////

	return y - scry;
}
#endif

//////////////////////////////////////////////////////////////////////////

void obj_AirDropSpawn::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
	pugi::xml_node AirDropSpawnNode = node.append_child();
	AirDropSpawnNode.set_name("LootID_parameters");
	SetXMLVal("spawn_radius", AirDropSpawnNode, &spawnRadius);
	SetXMLVal("m_DefaultItems", AirDropSpawnNode, &m_DefaultItems);
	if (m_DefaultItems == 1)
	{
		m_LootBoxID1 = 0;
		m_LootBoxID2 = 0;
		m_LootBoxID3 = 0;
		m_LootBoxID4 = 0;
		m_LootBoxID5 = 0;
		m_LootBoxID6 = 0;
	}
	SetXMLVal("m_LootBoxID1", AirDropSpawnNode, &m_LootBoxID1);
	SetXMLVal("m_LootBoxID2", AirDropSpawnNode, &m_LootBoxID2);
	SetXMLVal("m_LootBoxID3", AirDropSpawnNode, &m_LootBoxID3);
	SetXMLVal("m_LootBoxID4", AirDropSpawnNode, &m_LootBoxID4);
	SetXMLVal("m_LootBoxID5", AirDropSpawnNode, &m_LootBoxID5);
	SetXMLVal("m_LootBoxID6", AirDropSpawnNode, &m_LootBoxID6);
}

// NOTE: this function must stay in sync with server version
void obj_AirDropSpawn::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node AirDropSpawnNode = node.child("LootID_parameters");
	GetXMLVal("spawn_radius", AirDropSpawnNode, &spawnRadius);
	GetXMLVal("m_DefaultItems", AirDropSpawnNode, &m_DefaultItems);
	GetXMLVal("m_LootBoxID1", AirDropSpawnNode, &m_LootBoxID1);
	GetXMLVal("m_LootBoxID2", AirDropSpawnNode, &m_LootBoxID2);
	GetXMLVal("m_LootBoxID3", AirDropSpawnNode, &m_LootBoxID3);
	GetXMLVal("m_LootBoxID4", AirDropSpawnNode, &m_LootBoxID4);
	GetXMLVal("m_LootBoxID5", AirDropSpawnNode, &m_LootBoxID5);
	GetXMLVal("m_LootBoxID6", AirDropSpawnNode, &m_LootBoxID6);
}

//////////////////////////////////////////////////////////////////////////

GameObject * obj_AirDropSpawn::Clone()
{
	obj_AirDropSpawn * newSpawnPoint = static_cast<obj_AirDropSpawn*>(srv_CreateGameObject("obj_AirDropSpawn", FileName.c_str(), GetPosition()));
	newSpawnPoint->CloneParameters(this);
	return newSpawnPoint;
}

//////////////////////////////////////////////////////////////////////////

void obj_AirDropSpawn::CloneParameters(obj_AirDropSpawn *o)
{
	spawnRadius = o->spawnRadius;
}
