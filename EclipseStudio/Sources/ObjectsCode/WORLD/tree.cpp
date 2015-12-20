#include "r3dpch.h"
#include "r3d.h"

#include "UI/UIimEdit.h"

#include "Editors/LevelEditor_Collections.h"

#include "GameObjects\gameobj.h"
#include "GameObjects\objmanag.h"
#include "tree.h"
#include "..\..\bin\Data\Shaders\DX9_P1\system\LibSM\shadow_config.h" // shader config file
#include "..\..\..\..\GameEngine\TrueNature2\Terrain3.h"
#include "..\..\..\..\GameEngine\TrueNature2\Terrain2.h"
#include "../../Editors/CollectionsManager.h"

//
//
// 	class for Building Object, as you may guess..
//
//

IMPLEMENT_CLASS(obj_Tree, "obj_Tree", "Object");
AUTOREGISTER_CLASS(obj_Tree);

extern GameObject*	TreeObject;

obj_Tree::obj_Tree()
{
	r3d_assert(TreeObject == 0);
	ObjTypeFlags |= OBJTYPE_Trees;
	TreeObject = this; // because it's effing huge object, I'm making a hack, as otherwise I have to make my scene tree way too huge
}

obj_Tree::~obj_Tree()
{
	TreeObject = 0;
}

BOOL obj_Tree::OnCreate()
{

	parent::OnCreate();
	DrawOrder	= OBJ_DRAWORDER_LAST-10;
	ObjFlags      |= OBJFLAG_SkipCastRay;

	if(m_SceneBox)
		m_SceneBox->Remove(this);
	m_SceneBox = 0;

	return 1;
}

BOOL obj_Tree::Load(const char *fname)
//-----------------------------------------------------------------------
{


	if(!parent::Load(fname)) return FALSE;


	// Object won't be saved when level saved
	bPersistent 	= 0;

	// ObjTypeFlags   |= OBJTYPE_obj_A10Plane;

	Name     = "Tree";
	return TRUE;
}


BOOL obj_Tree::OnEvent(int event, void *data)
{
	return parent::OnEvent(event, data);

}



BOOL obj_Tree::CheckCastRay(const r3dPoint3D& vStart, const r3dPoint3D& vRay, float fRaytLen, float *fDist, int *facen)
{
	return false;
}



extern GameObject *gPlayer;
float Get_Terrain_Height(r3dPoint3D *pos);
extern int IsBillboard;

/*

	CELL_GCMUTIL_ATTR_TEXCOORD3 - pos

*/


void Update_Instance_Map ();


BOOL obj_Tree::Update()
{
	if ( ! g_trees->GetBool() )
		return true;

	R3DPROFILE_FUNCTION( "obj_Tree::Update" );

	gCollectionsManager.Update();

	r3dBoundBox bboxLocal ;

	bboxLocal.Org = r3dPoint3D(0,0,0);
	if(Terrain)
	{
		const r3dTerrainDesc& desc = Terrain->GetDesc() ;
		bboxLocal.Size = r3dPoint3D( desc.XSize, 5000, desc.ZSize );
	}
	else
		bboxLocal.Size = r3dPoint3D( 128, 128, 128 ) ;

	SetBBoxLocalAndWorld( bboxLocal, bboxLocal ) ;

	return TRUE;
}

void obj_Tree::AppendMaterials(r3dgfxVector(r3dMaterial*)& materials)
{
	gCollectionsManager.GetUsedMaterials(materials);
}

//------------------------------------------------------------------------
/*virtual*/

struct TreeShadowRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		R3DPROFILE_FUNCTION("TreeShadowRenderable");
		TreeShadowRenderable *This = static_cast<TreeShadowRenderable*>( RThis );

		This->Parent->DoDrawShadows();
	}

	obj_Tree* Parent;
};

#define RENDERABLE_TREE_SORT_VALUE (18*RENDERABLE_USER_SORT_VALUE)

void
obj_Tree::AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) /*OVERRIDE*/
{
	if ( ! g_trees->GetBool() )
		return;

	TreeShadowRenderable rend;

	rend.Init();

	rend.Parent			= this;
	rend.SortValue		= RENDERABLE_TREE_SORT_VALUE;

	rarr.PushBack( rend );
}

//------------------------------------------------------------------------
/*virtual*/

struct TreeRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		R3DPROFILE_FUNCTION("TreeRenderable");
		TreeRenderable *This = static_cast<TreeRenderable*>( RThis );

		This->Parent->DoDraw();
	}

	obj_Tree* Parent;
};

struct TreeDepthRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		R3DPROFILE_FUNCTION("TreeDepthRenderable");
		TreeRenderable *This = static_cast<TreeRenderable*>( RThis );

		This->Parent->DoDepthDraw();
	}

	obj_Tree* Parent;
};


void
obj_Tree::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
	if ( ! g_trees->GetBool() )
		return;

	TreeRenderable rend;

	rend.Init();
	rend.Parent		= this;
	rend.SortValue	= RENDERABLE_TREE_SORT_VALUE;

	render_arrays[ rsFillGBuffer ].PushBack( rend );

	if( r_depth_mode->GetInt() )
	{
		TreeDepthRenderable drend;

		drend.Init();
		drend.Parent		= this;
		drend.SortValue	= RENDERABLE_TREE_SORT_VALUE;

		render_arrays[ rsDrawDepth ].PushBack( drend );
	}
}

//------------------------------------------------------------------------


void
obj_Tree::DoDraw()
{
	r3dRenderer->SetDefaultCullMode( D3DCULL_NONE );
	gCollectionsManager.Render(R3D_IDME_NORMAL);
	r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );
}

//------------------------------------------------------------------------

void
obj_Tree::DoDepthDraw()
{
	r3dRenderer->SetDefaultCullMode( D3DCULL_NONE );
	gCollectionsManager.Render(R3D_IDME_DEPTH);
	r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );
}

//------------------------------------------------------------------------
void
obj_Tree::DoDrawShadows()
{
	r3dRenderer->SetDefaultCullMode( D3DCULL_NONE );
	gCollectionsManager.Render(R3D_IDME_SHADOW);
	r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );
}


