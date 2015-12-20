#pragma once

#include "GameCommon.h"
#include "GameCode/UserProfile.h"
#include "SharedUsableItem.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/Lamp.h"

class obj_DroppedItem : public SharedUsableItem
{
	DECLARE_CLASS(obj_DroppedItem, SharedUsableItem)
public:
	wiInventoryItem	m_Item;

	int NeedInitPhysics;

	float	m_AirHigthPos;
	bool	m_IsOnTerrain;
	r3dPoint3D AirDropPos;
	float	AirCraftDistance;
	bool	NetworLocal;
	bool	startdown;

	class obj_LightHelper* Light1;
	class obj_LightHelper* Light2;
	
public:
	obj_DroppedItem();
	virtual ~obj_DroppedItem();

	void				SetHighlight( bool highlight );
	bool				GetHighlight() const;

	void				StartLights();
	void				UpdateObjectPositionAfterCreation();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	void* sndAircraft;
	BYTE m_FirstTime;

	class obj_ParticleSystem* Flare;

	virtual	BOOL		Update();
	void		ServerPost(float posY);
	obj_Building*	AirCraft;

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;
};
