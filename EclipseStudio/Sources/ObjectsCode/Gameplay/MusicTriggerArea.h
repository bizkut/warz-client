#pragma once 

class obj_MusicTriggerArea : public GameObject
{
	DECLARE_CLASS(obj_MusicTriggerArea, GameObject)
private:
	static int forceShowSoundBubble;
protected:
	char  SoundFilename[256];
	int   sndID;
	void *sndEvent;

	float masterVolume;
	float triggerRadius;
	float cooldownTime;
	float nextPlayAllowedAt;

public:
	void DoDraw();

public:
	obj_MusicTriggerArea();	

#ifndef FINAL_BUILD
	void				UpdateSound( const r3dSTLString& soundPath ) ;
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	virtual GameObject*	Clone ();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )  OVERRIDE;

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);
};
