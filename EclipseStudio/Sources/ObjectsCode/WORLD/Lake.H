
#ifndef __PWAR_OBJECT_LAKE_H
#define __PWAR_OBJECT_LAKE_H

#ifndef WO_SERVER
#include "XPSObject.h"
#include "building.h"
#endif // WO_SERVER
#include "water.h"


class obj_Lake : public GameObject, public WaterBase
{
	DECLARE_CLASS(obj_Lake, GameObject)
public:
#ifndef WO_SERVER
	r3dXPSObject<R3D_WATER_VERTEX>*	OceanPlane;
#endif // WO_SERVER
	float PlaneScale;
	float CurrentScale;

	obj_Lake();

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();
	virtual	BOOL		Update();
	virtual	void		ReadSerializedData(pugi::xml_node& node);
#ifndef WO_SERVER
	virtual void		WriteSerializedData(pugi::xml_node& node);
#endif // WO_SERVER

	void UpdateGrid();

#ifndef WO_SERVER
	void DrawComposite( const r3dCamera& Cam );
	void DrawDepth( const r3dCamera& Cam );

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif
#endif // WO_SERVER
};

extern obj_Lake		*objOceanPlane;

#endif	// __PWAR_OBJECT_Reflect_H
