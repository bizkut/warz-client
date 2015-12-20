
#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "GameObjects\GameObj.h"
#include "GameObjects\ObjManag.h"

#include "rendering/Deffered/DeferredHelpers.h"

#include "obj_DebugTexture.h"

IMPLEMENT_CLASS(obj_DebugTexture, "obj_DebugTexture", "Object");
AUTOREGISTER_CLASS(obj_DebugTexture);

obj_DebugTexture::obj_DebugTexture()
{
	m_TextureToShow = 0 ;

	m_DrawX0 = -1.F;
	m_DrawX1 = +1.F;
	m_DrawY0 = -1.F;
	m_DrawY1 = +1.F;
}

BOOL obj_DebugTexture::Load(const char *fname)
//-----------------------------------------------------------------------
{
	if(!parent::Load(fname)) return FALSE;

	return TRUE;
}

BOOL obj_DebugTexture::OnCreate()
{
	parent::OnCreate();

	ObjFlags	|= OBJFLAG_DisableShadows ;
	setSkipOcclusionCheck(true);
	ObjFlags	|= OBJFLAG_SkipCastRay ;
	ObjFlags	|= OBJFLAG_AlwaysDraw ;
	ObjFlags	|= OBJFLAG_ForceSleep;

	m_isSerializable = false ;
	
	DrawOrder	= OBJ_DRAWORDER_LAST;

	r3dBoundBox bboxLocal ;

	bboxLocal.Size = r3dPoint3D(10,10,10);
	bboxLocal.Org = -bboxLocal.Size * 0.5f;

	SetBBoxLocal( bboxLocal ) ;

	UpdateTransform();

	return 1;
}

#ifndef FINAL_BUILD
float obj_DebugTexture::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	return starty-scry;
}
#endif
BOOL obj_DebugTexture::OnDestroy()
{
	return parent::OnDestroy();
}

struct DebugTextureRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		DebugTextureRenderable *This = static_cast<DebugTextureRenderable*>( RThis );

		This->Parent->DoDraw();
	}

	obj_DebugTexture* Parent;
};

void obj_DebugTexture::DoDraw()
{
	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ |R3D_BLEND_PUSH );

	r3dDrawBox2D(	( m_DrawX0 * 0.5f + 0.5f ) * r3dRenderer->ScreenW,
					( m_DrawY0 * 0.5f + 0.5f ) * r3dRenderer->ScreenH,
					( m_DrawX1 - m_DrawX0 ) * 0.5f * r3dRenderer->ScreenW,
					( m_DrawY1 - m_DrawY0 ) * 0.5f * r3dRenderer->ScreenH,
					r3dColor24( 255, 255, 255 ),
					m_TextureToShow );

	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

}

void
obj_DebugTexture::SetDebugTex( r3dTexture* tex, float x0, float y0, float x1, float y1 )
{
	m_TextureToShow = tex ;
	
	m_DrawX0 = x0 ;
	m_DrawX1 = x1 ;

	m_DrawY0 = y0 ;
	m_DrawY1 = y1 ;	
}

#define RENDERABLE_DEBUGTEXTURE_SORT_VALUE (5*RENDERABLE_USER_SORT_VALUE)

/*virtual*/
void
obj_DebugTexture::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
	// debug
	if( m_TextureToShow )
	{
		DebugTextureRenderable rend;

		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_DEBUGTEXTURE_SORT_VALUE;

		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}
}

void obj_DebugTexture::ReadSerializedData(pugi::xml_node& node)
{
	// shouldn't get here!
	r3d_assert( false ) ;
}

void obj_DebugTexture::WriteSerializedData(pugi::xml_node& node)
{
	// shouldn't get here!
	r3d_assert( false ) ;
}

