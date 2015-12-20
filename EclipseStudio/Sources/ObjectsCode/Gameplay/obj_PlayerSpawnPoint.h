#pragma once

#include "GameCommon.h"
#include "BasePlayerSpawnPoint.h"

#include "APIScaleformGfx.h"

class obj_PlayerSpawnPoint : public BasePlayerSpawnPoint
{
	DECLARE_CLASS(obj_PlayerSpawnPoint, BasePlayerSpawnPoint)
private:
	int		m_SelectedSpawnPoint;

	bool	m_bEditorCheckSpawnPointAtStart;

public:
	obj_PlayerSpawnPoint();
	virtual ~obj_PlayerSpawnPoint();

	void DoDrawComposite( const r3dCamera& Cam );

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();
#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	virtual void		AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) OVERRIDE;
	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

	virtual	BOOL		OnNetReceive(DWORD EventID, const void* packetData, int packetSize);

	virtual GameObject *Clone();

	virtual void		OnPickerPick(int locationId);
};
