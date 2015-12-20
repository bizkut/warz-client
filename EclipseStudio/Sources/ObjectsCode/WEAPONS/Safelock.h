#pragma  once

#include "GameCommon.h"
#include "../Gameplay/SharedUsableItem.h"

class obj_Safelock : public SharedUsableItem
{
	DECLARE_CLASS(obj_Safelock, SharedUsableItem)
public:
	obj_Safelock();
	virtual ~obj_Safelock();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual BOOL		OnDestroy();

	virtual BOOL		Update();

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

	uint32_t			m_ItemID;
	float				m_RotX;
	uint32_t			m_OwnerCustomerID; // to show it to devs
};
