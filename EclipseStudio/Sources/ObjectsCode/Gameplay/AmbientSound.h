#ifndef __AMBIENTSOUND_H__
#define __AMBIENTSOUND_H__

class obj_AmbientSound : public GameObject
{
	DECLARE_CLASS(obj_AmbientSound, GameObject)
private:
	static int forceShowSoundBubble;
protected:
	int   sndID;
	void *sndEvent;

    float min3DDist;
    float max3DDist;
	float masterVolume;

	float startTime;
	float endTime;
	float fadeTime;

	float nextPlayAllowedAt;
	float triggerCooldownTime;
	int   isMusicTrigger;

public:
	void DoDraw();

public:
	obj_AmbientSound();	

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

#endif //__AMBIENTSOUND_H__