#pragma  once

#include "GameCommon.h"
#include "../Gameplay/SharedUsableItem.h"

class obj_FarmBlock : public SharedUsableItem
{
	DECLARE_CLASS(obj_FarmBlock, SharedUsableItem)
public:
	obj_FarmBlock();
	virtual ~obj_FarmBlock();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual BOOL		OnDestroy();

	void	Deactivate();
	void	Activate(bool isme);

	virtual BOOL		Update();

	uint32_t			m_ItemID;
	float				m_RotX;
	float				m_TimeUntilRipe;
	bool	m_Activated;
};
