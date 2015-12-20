#ifndef __PWAR_OBJ_DUMMY_H
#define __PWAR_OBJ_DUMMY_H

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class DummyObject : public GameObject
{
	DECLARE_CLASS(DummyObject, GameObject)

public:
	DummyObject();
	virtual				~DummyObject();

	virtual	BOOL		Load(const char* name);

	virtual void		AppendShadowRenderables( RenderArray&  rarr, int sliceIndex, const r3dCamera& Cam ) OVERRIDE;
	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

	virtual	BOOL		Update();

	void				DoDraw();
};


#endif	// __PWAR_OBJ_DUMMY_H
