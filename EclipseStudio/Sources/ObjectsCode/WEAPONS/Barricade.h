#pragma  once

#include "GameCommon.h"

class obj_Barricade : public GameObject
{
	DECLARE_CLASS(obj_Barricade, GameObject)
	friend struct BarricadeDeferredRenderable;

	bool isRenderable;
	bool defferredPhysicsDisable;

public:
	void DrawDebugInfo();
	bool CanStopVehicle();
	void Destroy(bool playParticles);

public:
	obj_Barricade();
	virtual ~obj_Barricade();

	virtual	BOOL		OnCreate();
	virtual BOOL		OnDestroy();

	virtual void		OnContactModify(PhysicsCallbackObject *obj, PxContactSet& contacts);

	virtual BOOL		Update();

	void LightTurnOn();
	void LightTurnOff();

	virtual void		AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) OVERRIDE;
	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )  OVERRIDE;

	virtual r3dMesh*	GetObjectMesh();
	virtual r3dMesh*	GetObjectLodMesh() OVERRIDE;

	uint32_t			m_ItemID;
	float				m_RotX;
	bool				Turn;
protected:
	r3dMesh* m_PrivateModel;
	
	r3dLight		m_Light; // used only for lights that are placed by player
	void*			m_GeneratorSound; // for generator only

	int				m_BBoxSet;
};
