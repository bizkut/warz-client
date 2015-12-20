//=========================================================================
//	Module: obj_MissionArea.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================
#pragma once

#ifndef FINAL_BUILD
#include "GameCommon.h"
#include "gameobjects/GameObj.h"

namespace MissionAreaType
{
	enum EMissionAreaType
	{
		 AABB	= 1
		,Sphere

		,MAX_AREA_TYPE // This Must Be Last
	};
	static const char* MissionAreaTypeNames[] = {
		 "AABB"
		,"Sphere"
	};
};

class obj_MissionArea : public GameObject
{
	DECLARE_CLASS(obj_MissionArea, GameObject)

public:
	obj_MissionArea();
	~obj_MissionArea();

	virtual void	AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam);
	virtual float	DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected);

	virtual BOOL	OnCreate();
	virtual BOOL	Update();
	virtual BOOL	OnDestroy();
	virtual	void	ReadSerializedData(pugi::xml_node& node);
	virtual void	WriteSerializedData(pugi::xml_node& node);

	void			UpdateAreaTypeChanged( const int& areaType );
	const char*		GetMissionAreaTypeName();

public:
	MissionAreaType::EMissionAreaType	m_areaType;
	r3dVector	m_extents;
};

#endif // FINAL_BUILD