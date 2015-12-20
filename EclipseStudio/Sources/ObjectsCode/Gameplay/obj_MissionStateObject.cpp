//=========================================================================
//	Module: obj_MissionStateObject.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================
#include "r3dPCH.h"
#include "r3d.h"

#include "obj_MissionStateObject.h"
#include "../../multiplayer/ClientGameLogic.h"
#include "../ai/AI_Player.H"
#ifndef FINAL_BUILD
#include "../../Editors/LevelEditor.h"
#include "GameLevel.h"
#endif

//-------------------------------------------------------------------------

#ifdef MISSIONS

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_MissionStateObject, "obj_MissionStateObject", "Object");
AUTOREGISTER_CLASS(obj_MissionStateObject);

obj_MissionStateObject::obj_MissionStateObject()
	: m_ActionUI_Text( NULL )
{
	sprintf( m_ActionUI_TitleID, "Note" );
	sprintf( m_ActionUI_MsgID, "HoldEToReadNote" );
	sprintf( m_ActionUI_TextID, "\0" );
}

obj_MissionStateObject::~obj_MissionStateObject()
{
}

BOOL obj_MissionStateObject::Load(const char *name)
{
	if( name[0] != 0 && stricmp(name, "MissionStateObject") != 0 && parent::Load(name))
		return TRUE;

	// Default to a Item_MsgBottle.
	FileName = "Data\\ObjectsDepot\\Weapons\\Item_MsgBottle.sco";
	if(parent::Load(FileName.c_str())) 
		return TRUE;

	return FALSE;
}

BOOL obj_MissionStateObject::OnCreate()
{
	m_ActionUI_Title = gLangMngr.getString(m_ActionUI_TitleID);
	m_ActionUI_Msg = gLangMngr.getString(m_ActionUI_MsgID);
	if( stricmp( m_ActionUI_TextID, "" ) )
	{
		m_ActionUI_Text = gLangMngr.getString(m_ActionUI_TextID);
	}
	m_spawnPos = GetPosition();

#ifndef FINAL_BUILD
	if( g_bEditMode )
	{
		InitObjLists();
		SetHighlight( true );
		m_bEnablePhysics = false;
	}
#endif

	return parent::OnCreate();
}

BOOL obj_MissionStateObject::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_MissionStateObject::Update()
{
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

	return parent::Update();
}

#ifndef FINAL_BUILD
struct MissionEntry
{
	uint32_t		ID;
	r3dSTLString	Name;
};
typedef r3dgameVector(MissionEntry) missionlist;
missionlist			MissionData;

typedef r3dgameVector(CategoryStruct)  catlist;
extern catlist		ObjectCategories;
extern stringlist_t	CatNames;
stringlist_t		MissionStateObjCatNames;
int					MissionStateObjInit		= 0;

void obj_MissionStateObject::InitObjLists()
{
	if( MissionStateObjInit > 0 )
		return;

	MissionStateObjInit = 1;

	InitMissionData();

	MissionStateObjCatNames.push_back("WZ_InteriorProps");
	MissionStateObjCatNames.push_back("WZ_TownProps");
}

bool obj_MissionStateObject::InitMissionData()
{
	char fname[MAX_PATH];
	sprintf(fname, "%s\\Missions.xml", r3dGameLevel::GetHomeDir());
	
	r3dFile* f = r3d_open(fname, "rb");
	if ( !f )
	{
		r3dOutToLog("!!! ERROR: Failed to open %s\n", fname);
		return false;
	}

	char* fileBuffer = game_new char[f->size + 1];
	r3d_assert(fileBuffer);
	fread(fileBuffer, f->size, 1, f);
	fileBuffer[f->size] = 0;
	pugi::xml_document xmlFile;
	pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
	fclose(f);
	if(!parseResult)
	{
		r3dOutToLog("!!! Failed to parse XML, error: %s\n", parseResult.description());
		return false;
	}
	pugi::xml_node xmlDB = xmlFile.child("MissionDB");
	{
		pugi::xml_node xmlMission = xmlDB.child("Mission");
		while(!xmlMission.empty())
		{
			bool active = xmlMission.attribute("active");
			if( active )
			{
				MissionEntry entry;
				entry.ID	= xmlMission.attribute("missionID").as_uint();
				entry.Name	= gLangMngr.getString(xmlMission.attribute("name").value());
				
				MissionData.push_back( entry );
			}

			xmlMission = xmlMission.next_sibling();
		}
	}
	return true;
}

float obj_MissionStateObject::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{		
		starty += imgui_Static( scrx, starty, "Mission State Object Parameters:" );
		imgui2_StringValueEx( scrx, starty, scrw, "Action Title ID", m_ActionUI_TitleID );
		starty += 26;
		imgui2_StringValueEx( scrx, starty, scrw, "Action Msg ID", m_ActionUI_MsgID );
		starty += 26;
		imgui2_StringValueEx( scrx, starty, scrw, "Action Text ID", m_ActionUI_TextID );
		starty += 26;

		starty += imgui_Static( scrx, starty, "Used by Missions:" );
		for( int i = 0; i < (int)MissionData.size(); ++i )
		{
			int isSelected = 0;
			int wasIsSelected = 0;
			isSelected = wasIsSelected = (( m_missionIDs.find( MissionData[i].ID ) )!=m_missionIDs.end())?1:0;
			starty += imgui_Checkbox(scrx, starty, MissionData[i].Name.c_str(), &isSelected, 1);
			if(isSelected != wasIsSelected)
			{
				if(wasIsSelected && !isSelected)
					m_missionIDs.erase(m_missionIDs.find( MissionData[i].ID ));
				else if(!wasIsSelected && isSelected)
					m_missionIDs.insert( MissionData[i].ID );
			}
		}

		starty += imgui_Static( scrx, starty, "Visualization:" );
		static char catName[64]		= "";
		static char className[64]	= "";
		static char fileName[64]	= "";
		static char relFolder[256]	= "";
		static char str[256]		= "";
		static int idx				= -1;
		static float catOffset		= 0.0f;
		static float objOffset		= 0.0f;
		imgui_DrawList( scrx, starty, 360, 100, MissionStateObjCatNames, &catOffset, &idx );
		
		int idx0 = -1;
		if( idx >= 0 && idx < (int)MissionStateObjCatNames.size() )
		{
			sprintf(catName, "%s", MissionStateObjCatNames.at(idx).c_str());
			sprintf(className, "");
			sprintf(fileName, "");

			r3dSTLString relPath( "\\" );
			relPath += catName;


			stringlist_t::iterator elem = std::find( CatNames.begin(), CatNames.end(), catName );
			if( elem != CatNames.end() )
			{
				idx0 = std::distance( CatNames.begin(), elem );

				if( idx0 < (int)ObjectCategories.size() )
				{
					const CategoryStruct& cat = ObjectCategories.at(idx0);

					sprintf( className, "%s", cat.ObjectsClasses.at(0).c_str() );
					sprintf( fileName, "%s", cat.ObjectsNames.at(0).c_str() );
					sprintf( relFolder, "%s", cat.ObjectsSubfolders.at(0).c_str() );
				}
			}
		}
		starty += 64;
		starty += imgui_Static( scrx, starty, fileName );
		static int idx1 = 0;
		if( idx0 >= 0 )
		{
			const CategoryStruct& cat = ObjectCategories.at(idx0);

			imgui_DrawList( scrx, starty, 360, 200, cat.ObjectsNames, &objOffset, &idx1 );

			if( idx1 >= 0 && idx1 < (int)cat.ObjectsNames.size() )
			{
				sprintf( className, "%s", cat.ObjectsClasses.at(idx1).c_str() );
				sprintf( fileName, "%s", cat.ObjectsNames.at(idx1).c_str() );
				sprintf( relFolder, "%s", cat.ObjectsSubfolders.at(idx1).c_str() );
			}

			if( className[0] > 0 && fileName[0] > 0 )
			{
				if( cat.Type )
					sprintf( str, "%s", fileName );
				else
					sprintf( str, "Data\\ObjectsDepot\\%s%s\\%s", catName, relFolder, fileName );
			}

			starty += 210;

			FileName = str;

			// Swap out the displayed mesh.
			r3dMesh* pMesh = r3dGOBAddMesh(FileName.c_str());
			if( pMesh )
				MeshLOD[ 0 ] = pMesh;
		}
	}

	return starty;
}

void obj_MissionStateObject::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);

	pugi::xml_node objNode = node.append_child();
	objNode.set_name("StateParams");
	objNode.append_attribute("UITitleID") = m_ActionUI_TitleID;
	objNode.append_attribute("UIMsgID") = m_ActionUI_MsgID;
	objNode.append_attribute("UITextID") = m_ActionUI_TextID;
	objNode.append_attribute("numMissionsSelected") = m_missionIDs.size();
	std::tr1::unordered_set<uint32_t>::const_iterator iter = m_missionIDs.begin();
	for(size_t i=0; iter != m_missionIDs.end(); ++i, ++iter)
	{
		pugi::xml_node mNode = objNode.append_child();
		char tempStr[32];
		sprintf(tempStr, "m%d", i);
		mNode.set_name(tempStr);
		mNode.append_attribute("id") = *iter;
	}
}
#endif

void obj_MissionStateObject::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);

	pugi::xml_node objNode = node.child("StateParams");
	sprintf( m_ActionUI_TitleID, objNode.attribute("UITitleID").value() );
	sprintf( m_ActionUI_MsgID, objNode.attribute("UIMsgID").value() );
	sprintf( m_ActionUI_TextID, objNode.attribute("UITextID").value() );
	uint32_t numMissions = objNode.attribute("numMissionsSelected").as_uint();
	for(uint32_t i=0; i<numMissions; ++i)
	{
		char tempStr[32];
		sprintf(tempStr, "m%d", i);
		pugi::xml_node mNode = objNode.child(tempStr);
		r3d_assert(!mNode.empty());
		m_missionIDs.insert(mNode.attribute("id").as_uint());
	}
}

bool obj_MissionStateObject::PlayerHasValidMission( obj_Player* player )
{
	if( !player )
		return false;

	for( std::vector<uint32_t>::const_iterator iter = player->acceptedMissions.begin();
		 iter != player->acceptedMissions.end(); ++iter )
	{
		// Does the player have a valid MissionID?
		if( m_missionIDs.find( *iter ) != m_missionIDs.end() )
		{
			uint32_t missionID = *iter;

			// Does the player have a valid Action to go with that MissionID?
			for( std::map<uint32_t, obj_Player::MissionActionData>::const_iterator mapIter = player->missionsActionData.begin();
				 mapIter != player->missionsActionData.end(); ++mapIter )
			{
				const obj_Player::MissionActionData& actionData = mapIter->second;
				if( actionData.missionID == missionID && 
					actionData.actionType == 4 /* ACT_State */ &&
					actionData.itemID == GetHashID() &&
					!actionData.completed )
				{
					return true;
				}
			}
		}
	}
	return false;
}

void obj_MissionStateObject::SetHighlight( bool highlight )
{
	m_FillGBufferTarget = highlight ? rsFillGBufferAfterEffects : rsFillGBuffer;
}

bool obj_MissionStateObject::GetHighlight() const
{
	return m_FillGBufferTarget == rsFillGBufferAfterEffects;
}

bool obj_MissionStateObject::IsNote()
{
	// If there is text to display, it is assumed to be a note.
	return m_ActionUI_Text != NULL;
}

const char* obj_MissionStateObject::GetText()
{
	return m_ActionUI_Text;
}

void obj_MissionStateObject::AppendRenderables( RenderArray ( & render_arrays )[ rsCount ], const r3dCamera& Cam )
{
	MeshGameObject::AppendRenderables( render_arrays, Cam );

	if( GetHighlight() && PlayerHasValidMission( gClientLogic().localPlayer_ ) )
	{
		MeshObjDeferredHighlightRenderable rend;

		rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::green.GetPacked() );
		rend.SortValue = 0;
		rend.DoExtrude = 0;
		
		render_arrays[ rsDrawHighlightPass0 ].PushBack( rend );

		rend.DoExtrude = 1;
		render_arrays[ rsDrawHighlightPass1 ].PushBack( rend );
	}
}

#endif