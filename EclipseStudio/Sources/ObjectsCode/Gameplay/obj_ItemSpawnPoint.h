#pragma once

#include "GameCommon.h"
#include "BaseItemSpawnPoint.h"

#include "APIScaleformGfx.h"

class obj_ItemSpawnPoint : public BaseItemSpawnPoint
{
	DECLARE_CLASS(obj_ItemSpawnPoint, BaseItemSpawnPoint)
protected:
	int		m_SelectedSpawnPoint;
	bool m_bEditorCheckSpawnPointAtStart;

	r3dColor editor_spawnPointBoxColor;

#ifndef FINAL_BUILD
	int						m_LootBoxAnalysis_NextUpdateTime;
	LootBoxConfig*			m_LootBoxAnalysis_LootBoxConfig;
#endif

public:
	obj_ItemSpawnPoint();
	virtual ~obj_ItemSpawnPoint();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();
#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;

	static void			AnalyzeLootBoxData(float analyzeWindowInHours);
#endif

	void				DoDrawComposite( const r3dCamera& Cam );

	virtual void		AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) OVERRIDE;
	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

	virtual	BOOL		OnNetReceive(DWORD EventID, const void* packetData, int packetSize);
	void				SetSelectedSpawnPoint(int ssp) { m_SelectedSpawnPoint = ssp; }

	virtual void		OnPickerPick(int locationId);

	virtual GameObject *Clone();

	virtual void		WriteSerializedData(pugi::xml_node& node);
	virtual	void		ReadSerializedData(pugi::xml_node& node);
};

#ifndef FINAL_BUILD
class obj_HackerItemSpawnPoint : public obj_ItemSpawnPoint
{
	DECLARE_CLASS(obj_HackerItemSpawnPoint, obj_ItemSpawnPoint)
public:
	obj_HackerItemSpawnPoint();
	virtual ~obj_HackerItemSpawnPoint();
};
#endif