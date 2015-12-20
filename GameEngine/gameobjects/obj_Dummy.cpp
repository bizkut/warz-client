#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_Dummy.h"

#include "rendering/Deffered/DeferredHelpers.h"

DummyObject::DummyObject()
{
	DrawOrder	= OBJ_DRAWORDER_NORMAL;
	CompoundID	= 0;
	CompoundOffset = r3dPoint3D(0,0,0);
}

DummyObject::~DummyObject()
{

}

BOOL DummyObject::Load(const char* name)
{
	return parent::Load(name);
}

struct DummyRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam  )
	{
		DummyRenderable* This = static_cast<DummyRenderable*>( RThis );

		This->Parent->DoDraw();
	}

	DummyObject* Parent;
};

/*virtual*/
void
DummyObject::AppendShadowRenderables( RenderArray &rarr, int sliceIndex, const r3dCamera& Cam ) /*OVERRIDE*/
{
	DummyRenderable rend;

	rend.Init();
	rend.Parent = this;
	rend.SortValue = RENDERABLE_USER_SORT_VALUE;

	rarr.PushBack( rend );
}

/*virtual*/
void
DummyObject::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
	DummyRenderable rend;

	rend.Init();
	rend.Parent = this;
	rend.SortValue = RENDERABLE_USER_SORT_VALUE;

	render_arrays[ rsFillGBuffer ].PushBack( rend );
}


BOOL DummyObject::Update()
{
	r3dBoundBox localBBox ;
	localBBox.Size = r3dVector ( 1,1,1 );
	localBBox.Org = -0.5f * localBBox.Size;

	SetBBoxLocal( localBBox ) ;

	UpdateTransform();
	return TRUE;
}

void DummyObject::DoDraw()
{
	r3dBox3D box;
	box.Size = GetScale ();
	box.Org = GetPosition () - 0.5f * box.Size;
	DrawDeferredBox3D ( box, r3dColor::white, false );
}