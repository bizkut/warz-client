#ifndef __MapBorder_H
#define __MapBorder_H

class obj_MapBorder : public GameObject
{
	DECLARE_CLASS(obj_MapBorder, GameObject)

private:
	float m_Width; // x
	float m_Length; // z
	float m_Timeout;
	int m_RenderVisualization;

public:

	void DoDrawComposite();
	void DoDrawGBuffer();

public:
	obj_MapBorder();	

#ifndef FINAL_BUILD
	void				CenterOnMap();
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);

};

#endif	// __MapBorder_H
