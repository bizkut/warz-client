#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "GameObjects\GameObj.h"
#include "GameObjects\ObjManag.h"
#include "../../Editors/ObjectManipulator3d.h"
#include "../../multiplayer/clientgamelogic.h"
#include "../AI/AI_Player.H"

#include "rendering/Deffered/DeferredHelpers.h"

#include "ReverbZoneBox.h"

extern r3dCamera gCam;

IMPLEMENT_CLASS(obj_ReverbZoneBox, "obj_ReverbZoneBox", "Object");
AUTOREGISTER_CLASS(obj_ReverbZoneBox);

static r3dTexture *SoundIcon = NULL;

extern void PopulateReverbStringList( stringlist_t &sl );

obj_ReverbZoneBox::obj_ReverbZoneBox() : 
 m_reverb(NULL)
,m_distance(10.0f)
,m_bboxSize(0.0f, 0.0f, 0.0f)
{
	r3dscpy(ReverbName, "none");
	ObjTypeFlags |= OBJTYPE_Sound;
	ObjFlags |= OBJFLAG_IconSelection;
}

BOOL obj_ReverbZoneBox::Load(const char *fname)
{
	if (!parent::Load(fname)) return FALSE;

	if (!SoundIcon) SoundIcon = r3dRenderer->LoadTexture("Data\\Images\\Sound.dds");

	return TRUE;
}

BOOL obj_ReverbZoneBox::OnCreate()
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

void obj_ReverbZoneBox::LoadReverb(const r3dSTLString& preset)
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
		m_reverb->setActive( false);
	}
}
#ifndef FINAL_BUILD
float	obj_ReverbZoneBox::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		starty += imgui_Static ( scrx, starty, "Box Size" );

		starty += imgui_Value_Slider(scrx, starty, "X Size", &m_bboxSize.x,	0.0f,300,	"%.2f",1);

		starty += imgui_Value_Slider(scrx, starty, "Y Size", &m_bboxSize.y,	0.0f,300,	"%.2f",1);

		starty += imgui_Value_Slider(scrx, starty, "Z Size", &m_bboxSize.z,	0.0f,300,	"%.2f",1);

		
		starty += imgui_Value_Slider(scrx, starty, "Distance", &m_distance,	0.0f, 100,	"%.2f",1);
		starty += imgui_Static ( scrx, starty, "Reverb Properties" );

		starty += imgui_Static ( scrx, starty, "Reverb preset:" );
		starty += imgui_Static ( scrx, starty, ReverbName );
		static stringlist_t sl;
		PopulateReverbStringList( sl );
		static float offset = 0;
		static int selectedReverbIndex = 0;
		int prevIdx = selectedReverbIndex;
		imgui_DrawList(scrx, starty, 360.0f, 122.0f, sl, &offset, &selectedReverbIndex);
		starty += 122;
		if (prevIdx != selectedReverbIndex)
		{
			const r3dSTLString &newPath = sl[selectedReverbIndex].c_str();
			PropagateChange(newPath, &obj_ReverbZoneBox::LoadReverb, selected ) ;
		}
	}
	
	return starty-scry;
}
#endif

BOOL obj_ReverbZoneBox::OnDestroy()
{
	if (m_reverb)
	{
		m_reverb->release();
		m_reverb= 0;
	}
	return parent::OnDestroy();
}

BOOL obj_ReverbZoneBox::Update()
{
	if( m_reverb ) 
	{
		r3dPoint3D realPlayerPosition = gCam;

		// generate the correct box. 
		r3dBoundBox bbox;
		bbox.Org = GetPosition(); 
		bbox.Org.y += .5f * m_bboxSize.y;
		bbox.Size = m_bboxSize;

		// rotate the player's position around box space. 
		r3dPoint3D boxSpacePlayerPosition =  realPlayerPosition - bbox.Org;  // box space.
		r3dPoint3D rotation = GetRotationVector();
		// fixing it for the odd rotation 
		float temp;
		temp = rotation.y;
		rotation.y = rotation.x;
		rotation.x = temp;
		r3dMatrix Matrix;
		r3dBuildRotationMatrix(Matrix, -rotation );

		boxSpacePlayerPosition *= Matrix;
		boxSpacePlayerPosition  += bbox.Org; // normal space. 

		// contains point currently uses an offset origin :(
		r3dBoundBox offsetBox = bbox;
		offsetBox.Org -= .5 * m_bboxSize;
		if ( offsetBox.ContainsPoint( boxSpacePlayerPosition  ) )
		{
			// since he's in the box. now we test distance to edge of the box. 
			float distance = FindPointDistanceToEdgeOfOrientedBoundBox( bbox, rotation, realPlayerPosition );
			// now manipulate it to what the fmod wants. 
			m_reverb->setActive( true);
			if( m_distance < distance ) {
				FMOD_VECTOR position = { realPlayerPosition.x, realPlayerPosition.y, realPlayerPosition.z };
				m_reverb->set3DAttributes( &position, 10, 10);
			} else {
				float percentageOfReverb = distance/ m_distance; // this is the percentage of strength we need. 
				float factor = 1/(1-percentageOfReverb); // this is a factor we'll apply to the distance to get that strength/
				float trueDistance = (realPlayerPosition - GetPosition()).Length(); // actual distance from center.
				float fmodReverbDistance =  trueDistance * factor; // the max distance from center to get the correct reverb. 
				FMOD_VECTOR position = { GetPosition().x, GetPosition().y, GetPosition().z };
				m_reverb->set3DAttributes( &position, 0, fmodReverbDistance );
			}
		} else {
			m_reverb->setActive( false );
		}
	}

	return 1;
}

struct ReverbZoneBoxRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ReverbZoneBoxRenderable *This = static_cast<ReverbZoneBoxRenderable*>( RThis );

		This->Parent->DoDraw();
	}

	obj_ReverbZoneBox* Parent;
};

void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);
struct ReverbBoxHelperRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ReverbBoxHelperRenderable *This = static_cast<ReverbBoxHelperRenderable*>( RThis );

		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

		if((This->Parent->GetPosition() - Cam).Length() < 100)
			r3dDrawIcon3D(This->Parent->GetPosition(), SoundIcon, r3dColor(255,255,255), 32);

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
	}

	obj_ReverbZoneBox* Parent;	
};

#define RENDERABLE_SOUND_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_ReverbZoneBox::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )  /*OVERRIDE*/
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
		ReverbZoneBoxRenderable rend;
		rend.Init();
		rend.SortValue = RENDERABLE_SOUND_USER_SORT_VALUE;
		rend.Parent = this;
		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}

	// helper
	extern int CurHUDID;
	if(CurHUDID == 0)
	{
		ReverbBoxHelperRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_SOUND_USER_SORT_VALUE;
		render_arrays[ rsDrawComposite1 ].PushBack( rend );
	}
#endif
}

extern ObjectManipulator3d g_Manipulator3d;
void obj_ReverbZoneBox::DoDraw()
{
#ifndef FINAL_BUILD
    if(g_Manipulator3d.IsSelected(this))
	{
		r3dColor clr = r3dColor::green;
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

		clr.A = 196;
		r3dBoundBox bbox;
		bbox.Org = GetPosition();// - 0.5f * m_bboxSize;
		bbox.Org.y += .5f * m_bboxSize.y ; 
		bbox.Size = m_bboxSize;

		r3dVector rotation = GetRotationVector();
		// there's a bug with rotation so I've to swap the axis. 
		float temp;
		temp = rotation.y;
		rotation.y = rotation.x;
		rotation.x = temp;
		
		r3dDrawOrientedBoundBox( bbox, rotation, gCam, clr );
		clr.A = 128;

		// r3dDrawUniformSphere ( GetPosition(), m_maxDist, gCam, clr );
		r3dRenderer->Flush();
	}
#endif
}

void obj_ReverbZoneBox::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node reverbNode = node.child("reverbZone");
	const char *fn = reverbNode.attribute("preset").value();
	m_bboxSize.x = reverbNode.attribute("xSize").as_float();
	m_bboxSize.y = reverbNode.attribute("ySize").as_float();
	m_bboxSize.z = reverbNode.attribute("zSize").as_float();
	m_distance = reverbNode.attribute("distance").as_float();

	r3dSTLString temp(fn);
	LoadReverb(temp);
}

void obj_ReverbZoneBox::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node reverbNode = node.append_child();
	reverbNode.set_name("reverbZone");
	reverbNode.append_attribute("preset") = ReverbName;
	reverbNode.append_attribute("xSize") = m_bboxSize.x;
	reverbNode.append_attribute("ySize") = m_bboxSize.y;
	reverbNode.append_attribute("zSize") = m_bboxSize.z;
	reverbNode.append_attribute("distance") = m_distance;
}

// done here because r3d doesn't have access to r3dPlanes for some reason. 
float obj_ReverbZoneBox::FindPointDistanceToEdgeOfOrientedBoundBox( const r3dBoundBox &Box, const r3dPoint3D &rotation, const r3dPoint3D &location )
{
	r3dPoint3D points[8];

	r3dGenerateOrientedBoundBoxPoints( points, 8,  Box, rotation ); 

	// we need to make four planes and test against the center

	r3dPlane plane( points[0], points[1], points[4] );
	float minDistance = abs(plane.SignedDistance( location)) ;
	plane.Set( points[1], points[2], points[5] );
	float distance = abs(plane.SignedDistance( location));
	if ( distance < minDistance ) 
	{
		minDistance = distance;
	}
	plane.Set( points[2], points[3], points[6] );
	distance = abs(plane.SignedDistance( location));
	if ( distance < minDistance ) 
	{
		minDistance = distance;
	}
	plane.Set( points[3], points[0], points[7] );
	distance = abs(plane.SignedDistance( location));
	if ( distance < minDistance ) 
	{
		minDistance = distance;
	}

	return minDistance;
}

