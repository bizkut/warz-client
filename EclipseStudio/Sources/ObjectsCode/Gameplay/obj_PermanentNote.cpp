#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_PermanentNote.h"

IMPLEMENT_CLASS(obj_PermanentNote, "obj_PermanentNote", "Object");
AUTOREGISTER_CLASS(obj_PermanentNote);

obj_PermanentNote::obj_PermanentNote()
{
	m_msgIDString[0] = 0;
	DisablePhysX = true;
}

obj_PermanentNote::~obj_PermanentNote()
{
}

BOOL obj_PermanentNote::OnCreate()
{
	if(!PhysicsConfig.ready)
		ReadPhysicsConfig();
	PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
	PhysicsConfig.isDynamic = false;
	PhysicsConfig.isFastMoving = false;

	if(parent::OnCreate())
	{
		m_GotData = true;
		m_TextFrom = "";
		m_Text = (gLangMngr.getString(m_msgIDString));
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD
float obj_PermanentNote::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{		
		starty += imgui_Static ( scrx, starty, "Permanent Note Params" );
		imgui2_StringValueEx(scrx, starty, 360, "StringID:", m_msgIDString);
		starty += 24; 
	}

	return starty-scry;
}
#endif

void obj_PermanentNote::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node noteNode = node.child("PermanentNote");
	r3dscpy(m_msgIDString, noteNode.attribute("stringID").value());
}

void obj_PermanentNote::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
	pugi::xml_node noteNode = node.append_child();
	noteNode.set_name("PermanentNote");
	noteNode.append_attribute("stringID") = m_msgIDString;
}

