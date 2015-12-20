#pragma once

#include "GameCommon.h"

class obj_PostBox : public MeshGameObject
{
	DECLARE_CLASS(obj_PostBox, MeshGameObject)
	
public:
	float		useRadius;
		
	static r3dgameVector(obj_PostBox*) LoadedPostboxes;
public:
	obj_PostBox();
	virtual ~obj_PostBox();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();
 #ifndef FINAL_BUILD
 	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
 #endif
	virtual	void		AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam);
	virtual void		WriteSerializedData(pugi::xml_node& node);
	virtual	void		ReadSerializedData(pugi::xml_node& node);

};
