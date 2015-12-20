//=========================================================================
//	Module: obj_MissionStateObject.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#ifdef MISSIONS

#include "SharedUsableItem.h"

class obj_Player;

class obj_MissionStateObject : public SharedUsableItem
{
	DECLARE_CLASS(obj_MissionStateObject, SharedUsableItem)
protected:
	char	m_ActionUI_TitleID[64];
	char	m_ActionUI_MsgID[64];
	char	m_ActionUI_TextID[64];
	const char*	m_ActionUI_Text;

public:
	std::tr1::unordered_set<uint32_t>	m_missionIDs;

public:
	obj_MissionStateObject();
	~obj_MissionStateObject();

	virtual	BOOL		Load( const char *name );
	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();
	virtual	BOOL		Update();

 #ifndef FINAL_BUILD
	void				InitObjLists();
	bool				InitMissionData();
 	virtual	float		DrawPropertyEditor( float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected ) OVERRIDE;
	virtual void		WriteSerializedData( pugi::xml_node& node );
 #endif
	virtual	void		ReadSerializedData( pugi::xml_node& node );

	bool				PlayerHasValidMission( obj_Player* player );

	void				SetHighlight( bool highlight );
	bool				GetHighlight() const;

	bool				IsNote();
	const char*			GetText();

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;
};

#endif