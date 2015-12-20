#pragma once

class obj_ReverbZone : public GameObject
{
	DECLARE_CLASS(obj_ReverbZone, GameObject)
private:
	char  ReverbName[64];
	float	m_minDist;
	float	m_maxDist;
	FMOD::EventReverb* m_reverb;

	void				LoadReverb(const r3dSTLString& preset);
public:
	void DoDraw();

public:
	obj_ReverbZone();	

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
};

