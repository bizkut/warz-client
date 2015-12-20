#pragma once

#include "Tsg_stl/TString.h"

class obj_LocalColorCorrection : public GameObject
{
public:
	DECLARE_CLASS(obj_LocalColorCorrection, GameObject)

	obj_LocalColorCorrection ();
	~obj_LocalColorCorrection ();

	void DoDraw();

	virtual BOOL		OnDestroy();

	virtual GameObject*	Clone ();

#ifndef FINAL_BUILD
	virtual float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

	virtual BOOL		Update();

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);

	void				RadiusMinSet ( float fRadius );
	float				RadiusMin () const;

	void				RadiusMaxSet ( float fRadius );
	float				RadiusMax () const;

	void				CCTextureNameSet ( const char * const & sName );
	r3dTexture *		CCTexture () const;

	void				SetSelected( bool selected );

public:

	static unsigned int					LocalColorCorrectionCount ();
	static obj_LocalColorCorrection *	LocalColorCorrectionGet ( int i );
	static obj_LocalColorCorrection *	FindNearestColorCorrection ();
	static int							GetTextureCount( const FixedString& name );
	static void							DeselectAll();
	
public:

	float				m_fRadiusMin;
	float				m_fRadiusMax;
private:
	void				DestroyTexture();

	r3dTexture *		m_pCCTexture;
	FixedString			m_sCCTexFileName;

	bool				m_bSelected;

private:

	typedef r3dTL::TArray < obj_LocalColorCorrection* > Objects_t;
	static Objects_t m_dObjects;
};
