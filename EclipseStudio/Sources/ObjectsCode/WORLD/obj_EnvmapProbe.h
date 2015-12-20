#pragma once

#include "r3dString.h"

#include "GameCommon.h"

class obj_EnvmapProbe : public GameObject
{
public:
	static const int WRONG_ID = -1;

public:
	DECLARE_CLASS(obj_EnvmapProbe, GameObject)

	obj_EnvmapProbe ();
	~obj_EnvmapProbe ();

	void DoDraw();

	virtual BOOL		OnCreate();
	virtual BOOL		OnDestroy();

	virtual void		OnDelete() OVERRIDE;

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;	
	virtual BOOL		Update();

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);

#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif
			   
	float				GetRadius () const;

	float				GetRadiusSq() const;

	r3dTexture*			GetTexture() const;

	void				SetSelected( bool selected );
	void				Generate();

	int					IsInfinite() const;

	r3dString			GetTextureName( bool forSaving ) const;
	int					GetID() const;

	static void			CreateFolder( bool forSaving );

	// helpers
private:
	void				SetID( int id );
	void				UpdateBBox();

	// data
private:
	r3dVector			mSize;
	r3dVector			mCurrentBBoxSize;

	r3dTexture*			mTexture;

	bool				mSelected;

	int					mInfiniteRadius;

	int					mEdgeSize;

	int					mID;

	static int			mIDGen;
};
