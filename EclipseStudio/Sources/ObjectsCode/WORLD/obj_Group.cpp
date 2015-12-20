#include "r3dPCH.h"
#ifndef FINAL_BUILD

#include "r3d.h"

#include "GameCommon.h"
#include "obj_Group.h"

#include "..\..\Editors\ObjectManipulator3d.h"

//////////////////////////////////////////////////////////////////////////

static char * g_sGroupsFileBuffer = NULL;

//////////////////////////////////////////////////////////////////////////

r3dTL::TArray < obj_Group * > obj_Group::m_dGroups;

obj_Group::obj_Group ()
{
	m_iPreviewObjectsInGroup = 0;
	m_iIndexInFile = -1;
	m_bPreviewMode = false;
	m_bGroupJustCreated = false;
	r3dscpy ( m_szName, "NO_NAME" );
	m_dGroups.PushBack(this);
}

obj_Group::~obj_Group ()
{
	while ( m_dObjects.Count() > 0 )
		RemoveObject ( m_dObjects[m_dObjects.Count() - 1] );

	for ( uint32_t i = 0; i < m_dGroups.Count(); i++ )
	{
		if ( m_dGroups[i] == this )
		{
			m_dGroups.Erase ( i );
			break;
		}
	}
}

void obj_Group::SetPreviewMode ( bool bPreview )
{
	if ( bPreview != m_bPreviewMode )
	{
		m_bPreviewMode = bPreview;
		if ( !m_bGroupJustCreated )
		{
			if ( m_bPreviewMode )
			{
				LoadGroupObjects ();
				m_dPreviewObjects.Clear ();
				for ( uint32_t i = 0; i < m_dObjects.Count(); i++ )
				{
					GameObject * pNewObj = m_dObjects[i]->Clone ();
					if ( pNewObj )
					{
						pNewObj->bPersistent = 0;
						pNewObj->ObjFlags |= OBJFLAG_SkipCastRay;
						pNewObj->ObjFlags &= ~OBJFLAG_Removed;
						m_dPreviewObjects.PushBack ( pNewObj );
					}
				}
			}
			else
			{
				for ( uint32_t i = 0; i < m_dPreviewObjects.Count (); i++ )
				{
					GameWorld().DeleteObject(m_dPreviewObjects[i]);
				}
				m_dPreviewObjects.Clear ();

				UnloadGroupObjects();
			}
		}
	}
}

void obj_Group::SetPreviewObjectsPos ( r3dVector vPos, bool dropOnGround )
{
	if ( !m_bGroupJustCreated )
	{
		r3dBoundBox tBoxUnion;
		if ( m_dPreviewObjects.Count () > 0 )
			tBoxUnion = m_dPreviewObjects[0]->GetBBoxWorld();

		for ( uint32_t i = 0; i < m_dPreviewObjects.Count (); i++ )
			tBoxUnion.ExpandTo(m_dPreviewObjects[i]->GetBBoxWorld());

		for ( uint32_t i = 0; i < m_dPreviewObjects.Count (); i++ )
		{
			GameObject * pObj = m_dPreviewObjects[i];
			if ( pObj )
			{
				r3dVector vOffset = pObj->GetPosition () - ( tBoxUnion.Org + tBoxUnion.Size * 0.5f );
				pObj->SetPosition ( vPos + vOffset );

				if( dropOnGround && Terrain )
				{
					r3dPoint3D pos = pObj->GetPosition();

					pos.y = Terrain->GetHeight( pos );

					pObj->SetPosition( pos );
				}
			}
		}
	}
}

void obj_Group::ResetJustCreatedFlag ( bool bResetPicked/* = true */)
{
	if ( m_bGroupJustCreated )
	{
		r3dTL::TArray < GameObject * > dObj = m_dObjects;
		m_dObjects.Clear ();
		for ( uint32_t i = 0; i < dObj.Count(); i++ )
		{
			GameObject * pNewObj = dObj[i]->Clone ();
			if ( pNewObj )
			{
				GameWorld().UnlinkObject ( pNewObj );
				m_dObjects.PushBack ( pNewObj );
			}
		}
		
		if ( m_dObjects.Count () > 0 )
		{
			m_bGroupJustCreated = false;
			if ( bResetPicked )
				g_Manipulator3d.PickerResetPicked();
		}
	}
}

bool obj_Group::IsObjectInGroup ( GameObject * pObj ) const
{
	for ( uint32_t i = 0; i < m_dObjects.Count(); i++ )
	{
		if ( m_dObjects[i] == pObj )
		{
			return true;
		}
	}

	return false;
}

void obj_Group::AddObject ( GameObject * pObj )
{
	r3d_assert ( !IsObjectInGroup ( pObj ) );
	m_dObjects.PushBack(pObj);
}

void obj_Group::RemoveObject ( GameObject * pObj )
{
	for ( uint32_t i = 0; i < m_dObjects.Count(); i++ )
	{
		if ( m_dObjects[i] == pObj )
		{
			m_dObjects.Erase(i);
			if ( !m_bGroupJustCreated )
				GameWorld().DeleteObject(pObj);
			break;
		}
	}
}

// static
void obj_Group::LoadFromFile ( const char * sFile )
{
	r3dFile* f = r3d_open(sFile, "rb");
	if ( !f )
		return;

	r3d_assert ( g_sGroupsFileBuffer == NULL );
	if ( g_sGroupsFileBuffer )
	{
		delete [] g_sGroupsFileBuffer;
		g_sGroupsFileBuffer = NULL;
	}

	g_sGroupsFileBuffer = game_new char[f->size + 1];
	fread(g_sGroupsFileBuffer, f->size, 1, f);
	g_sGroupsFileBuffer[f->size] = 0;
	char * fileBuffer = game_new char [f->size + 1];
	r3dscpy ( fileBuffer, g_sGroupsFileBuffer );

	pugi::xml_document xmlGroupFile;
	pugi::xml_parse_result parseResult = xmlGroupFile.load_buffer_inplace(fileBuffer, f->size);
	fclose(f);
	if(!parseResult)
		r3dError("Failed to parse XML %s, error: %s", sFile, parseResult.description());
	pugi::xml_node xmlGroup = xmlGroupFile.child("groups");
	pugi::xml_node xmlObject = xmlGroup.child("group");
	int iIndex = 0;
	while(!xmlObject.empty())
	{
		// automatic add to group list
		obj_Group * pNewGroup = obj_Group::CreateNewGroup();
		r3d_assert ( pNewGroup );
		pNewGroup->ReadSerializedData(xmlObject);
		pNewGroup->m_iIndexInFile = iIndex;
		xmlObject = xmlObject.next_sibling();

		iIndex++;
	}

	// delete only after we are done parsing xml!
	delete [] fileBuffer;
}

// static
void obj_Group::SaveToFile ( const char * sFile )
{
	pugi::xml_document xmlGroupFile;
	xmlGroupFile.append_child(pugi::node_comment).set_value("Group data");
	pugi::xml_node xmlGroup = xmlGroupFile.append_child();
	xmlGroup.set_name("groups");

	for ( uint32_t i = 0; i < m_dGroups.Count (); i++ )
	{
		m_dGroups[i]->SetPreviewMode ( false );
		m_dGroups[i]->ResetJustCreatedFlag ();
	}

	for ( uint32_t i = 0; i < m_dGroups.Count (); i++ )
	{
		pugi::xml_node xmlObject = xmlGroup.append_child();
		xmlObject.set_name("group");
		m_dGroups[i]->WriteSerializedData(xmlObject);
	}

	xmlGroupFile.save_file(sFile);
}

void obj_Group::Save()
{
	SaveToFile( "Data\\ObjectsDepot\\LevelGroups.xml" );
}

// static
obj_Group * obj_Group::CreateNewGroup ()
{
	return game_new obj_Group ();
}

// static
void obj_Group::DeleteGroup ( obj_Group & group )
{
	delete &group;
}

void obj_Group::Unsubscribe ( GameObject * pObj )
{
	for ( uint32_t i = 0; i < m_dGroups.Count(); i++ )
	{
		obj_Group * pGr = m_dGroups[i];
		for ( uint32_t j = 0; j < pGr->m_dObjects.Count (); j++ )
		{
			if ( pGr->m_dObjects[j] == pObj )
			{
				pGr->m_dObjects.Erase(j--);
			}
		}
	}
}

void obj_Group::LoadGroupObjects ()
{
	if ( m_iIndexInFile < 0 )
		return;

	UnloadGroupObjects ();

	r3d_assert ( g_sGroupsFileBuffer != NULL );

	char * fileBuffer = game_new char [strlen ( g_sGroupsFileBuffer ) + 1];
	r3dscpy ( fileBuffer, g_sGroupsFileBuffer );

	pugi::xml_document xmlGroupFile;
	pugi::xml_parse_result parseResult = xmlGroupFile.load_buffer_inplace(fileBuffer, strlen(g_sGroupsFileBuffer));
	if(!parseResult)
		r3dError("Failed to parse XML for obj_Group::LoadObjectsFromNode, error: %s", parseResult.description());
	pugi::xml_node xmlGroup = xmlGroupFile.child("groups");
	pugi::xml_node xmlObject = xmlGroup.child("group");
	int iIndex = 0;
	while(!xmlObject.empty())
	{
		if ( iIndex == m_iIndexInFile )
		{
			ReadSerializedData(xmlObject, true);
			break;
		}
		xmlObject = xmlObject.next_sibling();
		iIndex++;
	}

	delete [] fileBuffer;
}

void obj_Group::UnloadGroupObjects ()
{
	if ( m_iIndexInFile < 0 )
		return;

	for ( ; m_dObjects.Count(); )
		GameWorld().DeleteObject( m_dObjects[ m_dObjects.Count() - 1 ] );

	m_dObjects.Clear ();
}


void obj_Group::ReadSerializedData(pugi::xml_node& node, bool bObjectsOnlyLoad/* = false*/)
{
	pugi::xml_node attrNode = node.child("attr_main");

	if ( !bObjectsOnlyLoad )
	{
		r3dscpy( m_szName, attrNode.attribute("Name").value() );
		m_iPreviewObjectsInGroup = attrNode.attribute("PreviewObjectsInGroup").as_int();
		if ( m_iPreviewObjectsInGroup == 0 )
			m_bGroupJustCreated = true;
	}

	pugi::xml_node cpNode = node.child("GameObjects");

	if ( bObjectsOnlyLoad )
	{
		void LoadLevelObjectsGroups ( pugi::xml_node & curNode, r3dTL::TArray < GameObject * > & dObjects );

		LoadLevelObjectsGroups ( cpNode, m_dObjects );
		for ( uint32_t i = 0; i < m_dObjects.Count (); i++ )
			GameWorld().UnlinkObject(m_dObjects[i]);

		if ( m_dObjects.Count () == 0 )
			m_bGroupJustCreated = true;
	}
}

void obj_Group::WriteSerializedData(pugi::xml_node& node)
{
	LoadGroupObjects ();

	pugi::xml_node attrNode = node.append_child();
	attrNode.set_name("attr_main");
	attrNode.append_attribute ( "Name" ) = m_szName;
	attrNode.append_attribute ( "PreviewObjectsInGroup" ) = (m_dObjects.Count () > 0) ? m_dObjects.Count () : m_iPreviewObjectsInGroup;

	pugi::xml_node cpNode = node.append_child();
	cpNode.set_name("GameObjects");

	void SaveLevelObjects ( pugi::xml_node & curNode, r3dTL::TArray < GameObject * > & dObjects );
	SaveLevelObjects ( cpNode, m_dObjects );

	UnloadGroupObjects ();
}


#endif // FINAL_BUILD