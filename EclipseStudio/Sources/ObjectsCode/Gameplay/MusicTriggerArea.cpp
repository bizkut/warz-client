#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "GameObjects\GameObj.h"
#include "GameObjects\ObjManag.h"
#include "../../Editors/ObjectManipulator3d.h"
#include "../../multiplayer/ClientGameLogic.h"
#include "../ai/AI_Player.H"

#include "rendering/Deffered/DeferredHelpers.h"

#include "MusicTriggerArea.h"

IMPLEMENT_CLASS(obj_MusicTriggerArea, "obj_MusicTriggerArea", "Object");
AUTOREGISTER_CLASS(obj_MusicTriggerArea);

extern bool g_bEditMode;
static r3dTexture *SoundIcon = NULL;

int obj_MusicTriggerArea::forceShowSoundBubble = 0;

obj_MusicTriggerArea::obj_MusicTriggerArea()
: sndEvent(0)
, sndID(-1)
, triggerRadius(50)
, masterVolume(1.0f)
, cooldownTime(20.0f)
, nextPlayAllowedAt(0)
{
	memset(SoundFilename, 0, sizeof(SoundFilename));
	ObjTypeFlags |= OBJTYPE_Sound;
	ObjFlags |= OBJFLAG_IconSelection;
	serializeFile = SF_SoundData;
}

BOOL obj_MusicTriggerArea::Load(const char *fname)
{
	if (!parent::Load(fname)) return FALSE;

	if (!SoundIcon) SoundIcon = r3dRenderer->LoadTexture("Data\\Images\\Sound.dds");

	return TRUE;
}

BOOL obj_MusicTriggerArea::OnCreate()
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

#ifndef FINAL_BUILD
void obj_MusicTriggerArea::UpdateSound( const r3dSTLString& soundPath )
{
	strcpy_s(SoundFilename, _countof(SoundFilename), soundPath.c_str());
	sndID = SoundSys.GetEventIDByPath(soundPath.c_str());
	SoundSys.Release(sndEvent);
	sndEvent = NULL;
}

float obj_MusicTriggerArea::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		starty += imgui_Static ( scrx, starty, "Music Trigger Properties" );
		starty += imgui_Checkbox(scrx, starty, "Show Sound Bubbles", &forceShowSoundBubble, 1);
		starty += imgui_Value_Slider(scrx, starty, "Trigger Radius", &triggerRadius, 0.0f, 5000.0f, "%0.2f");
		starty += imgui_Value_Slider(scrx, starty, "Cooldown(seconds)", &cooldownTime, 0.0f, 10000.0f, "%0.2f");
		starty += imgui_Value_Slider(scrx, starty, "Master Volume", &masterVolume, 0.0f, 1.0f, "%0.2f");
		starty += imgui_Static ( scrx, starty, "Music Sound Source:" );
		starty += imgui_Static ( scrx, starty, SoundFilename );

		const stringlist_t & sl = SoundSys.GetSoundsList();
		static float offset = 0;
		static int selectedSoundIndex = 0;
		int prevIdx = selectedSoundIndex;
		imgui_DrawList(scrx, starty, 360.0f, 122.0f, sl, &offset, &selectedSoundIndex);
		starty += 122.0f;
		if (prevIdx != selectedSoundIndex)
		{
			const r3dSTLString &newPath = sl[selectedSoundIndex].c_str();
			PropagateChange(newPath, &obj_MusicTriggerArea::UpdateSound, selected ) ;
		}
	}

	return starty-scry;
}
#endif

BOOL obj_MusicTriggerArea::OnDestroy()
{
	if (sndEvent)
	{
		SoundSys.Stop(sndEvent);
		SoundSys.Release(sndEvent);
		sndEvent = 0;
	}
	return parent::OnDestroy();
}

BOOL obj_MusicTriggerArea::Update()
{
	const r3dPoint3D &pos = GetPosition();

	// check if we already playing music, and if yes, check if it finished
	if(sndEvent)
	{
		int soundState = SoundSys.GetState(sndEvent);
		if(!SoundSys.isPlaying(sndEvent)) 		
		{
			SoundSys.Release(sndEvent);
			sndEvent = NULL;

			nextPlayAllowedAt = r3dGetTime() + cooldownTime;
		}
	}
	else
	{
		r3dPoint3D plPos(0,0,0);
		bool hasPos = false;
		const ClientGameLogic& CGL = gClientLogic();
		if(CGL.localPlayer_)
		{
			plPos = CGL.localPlayer_->GetPosition();
			hasPos = true;
		}
#ifndef FINAL_BUILD
		else if(g_bEditMode && d_allow_musictriggerarea->GetInt()>0)
		{
			plPos = gCam;
			hasPos = true;
		}
#endif
		if(hasPos)
		{
			// check if timer allows us to play another sound
			if(r3dGetTime() > nextPlayAllowedAt)
			{
				// check if player is inside of trigger area
				if((plPos-pos).Length() < triggerRadius)
				{
					sndEvent = SoundSys.Play(sndID, pos);
					SoundSys.SetPaused(sndEvent, true); // start paused to prevent one tick spike in sound volume
					SoundSys.SetProperty(sndEvent, FMOD_EVENTPROPERTY_VOLUME, &masterVolume);
					SoundSys.SetPaused(sndEvent, false);

				}
			}
		}
	}

	return 1;
}

struct MusicTriggerRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		MusicTriggerRenderable *This = static_cast<MusicTriggerRenderable*>( RThis );

		This->Parent->DoDraw();
	}

	obj_MusicTriggerArea* Parent;
};

void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);
struct MusicTriggerHelperRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		MusicTriggerHelperRenderable *This = static_cast<MusicTriggerHelperRenderable*>( RThis );

		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

		if((This->Parent->GetPosition() - Cam).Length() < 100)
			r3dDrawIcon3D(This->Parent->GetPosition(), SoundIcon, r3dColor(255,255,255), 32);

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
	}

	obj_MusicTriggerArea* Parent;	
};

#define RENDERABLE_SOUND_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_MusicTriggerArea::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )  /*OVERRIDE*/
{
#ifdef FINAL_BUILD
	return;
#else
	// don't draw debug info if we're not in editor mode
	if ( !g_bEditMode )
		return;

	if( r_hide_icons->GetInt() )
		return ;

	float idd = r_icons_draw_distance->GetFloat();
	idd *= idd;

	if( ( Cam - GetPosition() ).LengthSq() > idd )
		return;

	{
		MusicTriggerRenderable rend;
		rend.Init();
		rend.SortValue = RENDERABLE_SOUND_USER_SORT_VALUE;
		rend.Parent = this;
		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}

	// helper
	extern int CurHUDID;
	if(CurHUDID == 0)
	{
		MusicTriggerHelperRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_SOUND_USER_SORT_VALUE;
		render_arrays[ rsDrawComposite1 ].PushBack( rend );
	}
#endif
}

extern ObjectManipulator3d g_Manipulator3d;
void obj_MusicTriggerArea::DoDraw()
{
#ifndef FINAL_BUILD
	if(forceShowSoundBubble || g_Manipulator3d.IsSelected(this))
	{
		r3dColor clr = g_Manipulator3d.IsSelected(this)?r3dColor::red:r3dColor::green;
		clr.A = 128;
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

		r3dDrawUniformSphere ( GetPosition(), triggerRadius, gCam, clr );
		r3dRenderer->Flush();
	}
#endif
}

void obj_MusicTriggerArea::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node musicTriggerNode = node.child("musicTrigger");
	const char *fn = musicTriggerNode.attribute("Sound").value();
	strcpy_s(SoundFilename, _countof(SoundFilename), fn);
	sndID = SoundSys.GetEventIDByPath(SoundFilename);

	triggerRadius = musicTriggerNode.attribute("triggerRadius").as_float();
	cooldownTime = musicTriggerNode.attribute("cooldownTime").as_float();
	masterVolume = musicTriggerNode.attribute("volume").as_float();
}

void obj_MusicTriggerArea::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node musicTriggerNode = node.append_child();
	musicTriggerNode.set_name("musicTrigger");
	musicTriggerNode.append_attribute("Sound") = SoundFilename;
	musicTriggerNode.append_attribute("triggerRadius") = triggerRadius;
	musicTriggerNode.append_attribute("cooldownTime") = cooldownTime;
	musicTriggerNode.append_attribute("volume") = masterVolume;
}

GameObject*	obj_MusicTriggerArea::Clone()
{
	obj_MusicTriggerArea * pClone = (obj_MusicTriggerArea*)( srv_CreateGameObject ( "obj_MusicTriggerArea", "cloneMusicTriggerArea", GetPosition() ) );
	r3d_assert ( pClone );

	r3dscpy(pClone->SoundFilename, SoundFilename);
	pClone->triggerRadius = triggerRadius;
	pClone->masterVolume = masterVolume;
	pClone->cooldownTime = cooldownTime;
	pClone->sndID = SoundSys.GetEventIDByPath(SoundFilename);

	return pClone;
}
