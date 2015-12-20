#pragma once

class obj_DebugTexture : public GameObject
{
	DECLARE_CLASS(obj_DebugTexture, GameObject)

public:
	obj_DebugTexture();	

#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);

	virtual void		DoDraw();

	virtual void		SetDebugTex( r3dTexture* tex, float x0, float y0, float x1, float y1 );

private:
	r3dTexture*			m_TextureToShow ;

	float				m_DrawX0 ;
	float				m_DrawX1 ;
	float				m_DrawY0 ;
	float				m_DrawY1 ;
};

