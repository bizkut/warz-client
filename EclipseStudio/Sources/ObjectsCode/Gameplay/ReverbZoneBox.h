#pragma once

class obj_ReverbZoneBox : public GameObject
{
	DECLARE_CLASS(obj_ReverbZoneBox, GameObject)
private:
	char  ReverbName[64];
	r3dPoint3D m_bboxSize;
	float	m_distance;
	FMOD::EventReverb* m_reverb;

	void				LoadReverb(const r3dSTLString& preset);
public:
	void DoDraw();

public:
	obj_ReverbZoneBox();	

#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )  OVERRIDE;

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);

	// this function doesn't test against the top or bottom of the oriented bound box. 
	static float FindPointDistanceToEdgeOfOrientedBoundBox(  const r3dBoundBox &Box, const r3dPoint3D &rotation, const r3dPoint3D &location );
};

