#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "GameObjects\GameObj.h"
#include "GameObjects\ObjManag.h"
#include "../../Editors/ObjectManipulator3d.h"

#include "rendering/Deffered/DeferredHelpers.h"

#include "ReverbZone.h"

IMPLEMENT_CLASS(obj_ReverbZone, "obj_ReverbZone", "Object");
AUTOREGISTER_CLASS(obj_ReverbZone);

static r3dTexture *SoundIcon = NULL;




void PopulateReverbStringList( stringlist_t &sl )
{
	if(sl.empty())
	{
		sl.push_back("none");
		// designer presets
		sl.push_back("indoor_default");
		// FMOD default presets
		sl.push_back("generic");
		sl.push_back("padded_cell");
		sl.push_back("room");
		sl.push_back("bathroom");
		sl.push_back("livingroom");
		sl.push_back("stoneroom");
		sl.push_back("auditorium");
		sl.push_back("concerthall");
		sl.push_back("cave");
		sl.push_back("arena");
		sl.push_back("hangar");
		sl.push_back("carpettedhallway");
		sl.push_back("hallway");
		sl.push_back("stonecorridor");
		sl.push_back("alley");
		sl.push_back("forest");
		sl.push_back("city");
		sl.push_back("mountains");
		sl.push_back("quarry");
		sl.push_back("parkinglot");
		sl.push_back("sewerpipe");
		sl.push_back("underwater");
	}
}



obj_ReverbZone::obj_ReverbZone() : 
 m_reverb(NULL)
,m_minDist(1.0f)
,m_maxDist(10.0f)
{
	r3dscpy(ReverbName, "none");
	ObjTypeFlags |= OBJTYPE_Sound;
	ObjFlags |= OBJFLAG_IconSelection;
}

BOOL obj_ReverbZone::Load(const char *fname)
{
	if (!parent::Load(fname)) return FALSE;

	if (!SoundIcon) SoundIcon = r3dRenderer->LoadTexture("Data\\Images\\Sound.dds");

	return TRUE;
}

BOOL obj_ReverbZone::OnCreate()
{
	parent::OnCreate();

	DrawOrder	= OBJ_DRAWORDER_LAST;
	setSkipOcclusionCheck(true);
	ObjFlags	|=	OBJFLAG_DisableShadows | OBJFLAG_ForceSleep;

	r3dBoundBox bboxLocal ;

	bboxLocal.Size = r3dPoint3D(2,2,2);
	bboxLocal.Org = -bboxLocal.Size * 0.5f;

	SetBBoxLocal( bboxLocal ) ;

	UpdateTransform();

	return 1;
}

void obj_ReverbZone::LoadReverb(const r3dSTLString& preset)
{
	strcpy_s(ReverbName, _countof(ReverbName), preset.c_str());

	if (m_reverb)
	{
		m_reverb->release();
		m_reverb = NULL;
	}

	m_reverb = SoundSys.createPresetReverb(ReverbName);
	if(m_reverb)
	{
		FMOD_VECTOR pos = {GetPosition().x, GetPosition().y, GetPosition().z};
		m_reverb->set3DAttributes(&pos, m_minDist, m_maxDist);
	}
}
#ifndef FINAL_BUILD
float	obj_ReverbZone::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		starty += imgui_Static ( scrx, starty, "Reverb Properties" );

		static float minDist = 0 ;
		minDist = m_minDist;
		starty += imgui_Value_Slider(scrx, starty, "Min Dist", &minDist,	0.0f,300,	"%.2f",1);
		PropagateChange( minDist, &obj_ReverbZone::m_minDist, this, selected ) ;

		static float maxDist = 0 ;
		maxDist = m_maxDist;
		starty += imgui_Value_Slider(scrx, starty, "Max Dist", &maxDist,	1.0f,300,	"%.2f",1);
		PropagateChange( maxDist, &obj_ReverbZone::m_maxDist, this, selected ) ;

		starty += imgui_Static ( scrx, starty, "Reverb preset:" );
		starty += imgui_Static ( scrx, starty, ReverbName );
		static stringlist_t sl;
		PopulateReverbStringList(sl);


		static float offset = 0;
		static int selectedReverbIndex = 0;
		int prevIdx = selectedReverbIndex;
		imgui_DrawList(scrx, starty, 360.0f, 122.0f, sl, &offset, &selectedReverbIndex);
		starty += 122;
		if (prevIdx != selectedReverbIndex)
		{
			const r3dSTLString &newPath = sl[selectedReverbIndex].c_str();
			PropagateChange(newPath, &obj_ReverbZone::LoadReverb, selected ) ;
		}
	}
	
	return starty-scry;
}
#endif

BOOL obj_ReverbZone::OnDestroy()
{
	if (m_reverb)
	{
		m_reverb->release();
		m_reverb= 0;
	}
	return parent::OnDestroy();
}

BOOL obj_ReverbZone::Update()
{
	const r3dPoint3D &pos = GetPosition();
	if(m_reverb)
	{
		FMOD_VECTOR fmod_pos = {pos.x, pos.y, pos.z};
		m_reverb->set3DAttributes(&fmod_pos, m_minDist, m_maxDist);
	}
	return 1;
}

struct ReverbZoneRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ReverbZoneRenderable *This = static_cast<ReverbZoneRenderable*>( RThis );

		This->Parent->DoDraw();
	}

	obj_ReverbZone* Parent;
};

void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);
struct ReverbHelperRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ReverbHelperRenderable *This = static_cast<ReverbHelperRenderable*>( RThis );

		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

		if((This->Parent->GetPosition() - Cam).Length() < 100)
			r3dDrawIcon3D(This->Parent->GetPosition(), SoundIcon, r3dColor(255,255,255), 32);

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
	}

	obj_ReverbZone* Parent;	
};

#define RENDERABLE_SOUND_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_ReverbZone::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )  /*OVERRIDE*/
{
#ifdef FINAL_BUILD
	return;
#else
	// don't draw debug info if we're not in editor mode
	extern bool g_bEditMode;
	if ( !g_bEditMode )
		return;

	if( r_hide_icons->GetInt() )
		return ;

	float idd = r_icons_draw_distance->GetFloat();
	idd *= idd;

	if( ( Cam - GetPosition() ).LengthSq() > idd )
		return;

	{
		ReverbZoneRenderable rend;
		rend.Init();
		rend.SortValue = RENDERABLE_SOUND_USER_SORT_VALUE;
		rend.Parent = this;
		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}

	// helper
	extern int CurHUDID;
	if(CurHUDID == 0)
	{
		ReverbHelperRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_SOUND_USER_SORT_VALUE;
		render_arrays[ rsDrawComposite1 ].PushBack( rend );
	}
#endif
}

extern ObjectManipulator3d g_Manipulator3d;
void obj_ReverbZone::DoDraw()
{
#ifndef FINAL_BUILD
    if(g_Manipulator3d.IsSelected(this))
	{
		r3dColor clr = r3dColor::green;
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

		clr.A = 196;
		r3dDrawUniformSphere ( GetPosition(), m_minDist, gCam, clr );
		clr.A = 128;
		r3dDrawUniformSphere ( GetPosition(), m_maxDist, gCam, clr );
		r3dRenderer->Flush();
	}
#endif
}

void obj_ReverbZone::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node reverbNode = node.child("reverbZone");
	const char *fn = reverbNode.attribute("preset").value();
	m_minDist = reverbNode.attribute("min").as_float();
	m_maxDist = reverbNode.attribute("max").as_float();
	
	r3dSTLString temp(fn);
	LoadReverb(temp);
}

void obj_ReverbZone::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node reverbNode = node.append_child();
	reverbNode.set_name("reverbZone");
	reverbNode.append_attribute("preset") = ReverbName;
	reverbNode.append_attribute("min") = m_minDist;
	reverbNode.append_attribute("max") = m_maxDist;
}