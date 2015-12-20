#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "GameObjects\GameObj.h"
#include "GameObjects\ObjManag.h"
#include "../../Editors/ObjectManipulator3d.h"
#include "../../multiplayer/ClientGameLogic.h"
#include "../ai/AI_Player.H"

#include "GameLevel.h"

#include "rendering/Deffered/DeferredHelpers.h"

#include "AmbientSound.h"

IMPLEMENT_CLASS(obj_AmbientSound, "obj_AmbientSound", "Object");
AUTOREGISTER_CLASS(obj_AmbientSound);

static r3dTexture *SoundIcon = NULL;

int obj_AmbientSound::forceShowSoundBubble = 0;

obj_AmbientSound::obj_AmbientSound()
: sndEvent(0)
, sndID(-1)
, min3DDist(0)
, max3DDist(0)
, masterVolume(1.0f)
, startTime(0.0f)
, endTime(24.0f)
, fadeTime(0.0f)
, nextPlayAllowedAt(0.0f)
, isMusicTrigger(0)
, triggerCooldownTime(0.0f)
{
	ObjTypeFlags |= OBJTYPE_Sound;
	ObjFlags |= OBJFLAG_IconSelection;
	serializeFile = SF_SoundData;
}

BOOL obj_AmbientSound::Load(const char *fname)
{
	if (!parent::Load(fname)) return FALSE;

	if (!SoundIcon) SoundIcon = r3dRenderer->LoadTexture("Data\\Images\\Sound.dds");

	return TRUE;
}

BOOL obj_AmbientSound::OnCreate()
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
void obj_AmbientSound::UpdateSound( const r3dSTLString& soundPath )
{
	FileName = soundPath.c_str();
	sndID = SoundSys.GetEventIDByPath(soundPath.c_str());
	SoundSys.Release(sndEvent);
	sndEvent = NULL;
	max3DDist = 0.0f; // reset after selecting new sound, to get proper values from fmod
}

float obj_AmbientSound::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{

	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		starty += imgui_Static ( scrx, starty, "Sound Properties" );
		starty += imgui_Checkbox(scrx, starty, "Show Sound Bubbles", &forceShowSoundBubble, 1);
		starty += imgui_Value_Slider(scrx, starty, "Min 3D Dist", &min3DDist, 0.0f, max3DDist, "%0.2f");
		starty += imgui_Value_Slider(scrx, starty, "Max 3D Dist", &max3DDist, min3DDist, 10000.0f, "%0.2f");
		starty += imgui_Value_Slider(scrx, starty, "Master Volume", &masterVolume, 0.0f, 1.0f, "%0.2f");
		starty += imgui_Value_Slider(scrx, starty, "Start Time", &startTime, 0.0f, 24.0f, "%0.2f");
		starty += imgui_Value_Slider(scrx, starty, "End Time", &endTime, 0.0f, 24.0f, "%0.2f");
		starty += imgui_Value_Slider(scrx, starty, "Fade Time", &fadeTime, 0.0f, 24.0f, "%0.2f");
		starty += imgui_Checkbox(scrx, starty, "Is Trigger?", &isMusicTrigger, 1);
		starty += imgui_Value_Slider(scrx, starty, "Trigger Cooldown (sec)", &triggerCooldownTime, 0.0f, 10000.0f, "%0.2f");

		static int applyToAllSelected = 0;
		starty += imgui_Checkbox(scrx, starty, "Apply To All", &applyToAllSelected, 1);
		if(applyToAllSelected)
		{
			PropagateChange( min3DDist, &obj_AmbientSound::min3DDist, selected ) ;
			PropagateChange( max3DDist, &obj_AmbientSound::max3DDist, selected ) ;
			PropagateChange( masterVolume, &obj_AmbientSound::masterVolume, selected ) ;
			PropagateChange( startTime, &obj_AmbientSound::startTime, selected ) ;
			PropagateChange( endTime, &obj_AmbientSound::endTime, selected ) ;
			PropagateChange( fadeTime, &obj_AmbientSound::fadeTime, selected ) ;
			PropagateChange( isMusicTrigger, &obj_AmbientSound::isMusicTrigger, selected ) ;
			PropagateChange( triggerCooldownTime, &obj_AmbientSound::triggerCooldownTime, selected ) ;
			applyToAllSelected = 0;
		}

		starty += imgui_Static ( scrx, starty, "Sound Source:" );
		starty += imgui_Static ( scrx, starty, FileName.c_str() );

		const stringlist_t & sl = SoundSys.GetSoundsList();
		static float offset = 0;
		static int selectedSoundIndex = 0;
		int prevIdx = selectedSoundIndex;
		imgui_DrawList(scrx, starty, 360.0f, 122.0f, sl, &offset, &selectedSoundIndex);
		starty += 122.0f;
		if (prevIdx != selectedSoundIndex)
		{
			const r3dSTLString &newPath = sl[selectedSoundIndex].c_str();
			PropagateChange(newPath, &obj_AmbientSound::UpdateSound, selected ) ;
		}
	}

	return starty-scry;
}
#endif

BOOL obj_AmbientSound::OnDestroy()
{
	if (sndEvent)
	{
		SoundSys.Stop(sndEvent);
		SoundSys.Release(sndEvent);
		sndEvent = 0;
	}
	return parent::OnDestroy();
}

BOOL obj_AmbientSound::Update()
{
	const r3dPoint3D &pos = GetPosition();

	extern bool g_bEditMode;
	if(g_bEditMode)
	{
		m_isSerializable = FileName.Length() != 0;
	}

	// one time init
	if(max3DDist == 0.0f && sndEvent == NULL && sndID>0)
	{
		sndEvent = SoundSys.Play(sndID, pos);
		if(sndEvent)
		{
			if(SoundSys.Is3DSound(sndEvent))
			{
				SoundSys.GetProperty(sndEvent, FMOD_EVENTPROPERTY_3D_MINDISTANCE, &min3DDist);
				SoundSys.GetProperty(sndEvent, FMOD_EVENTPROPERTY_3D_MAXDISTANCE, &max3DDist);
			}
			else
				max3DDist = 100.0f;

			SoundSys.Release(sndEvent);
			sndEvent = NULL;
		}
	}

	float dist = (gCam-pos).Length();
	bool isAudible = dist < max3DDist;

	float curGTime = r3dGameLevel::Environment->__CurTime;
	float volumeMod = 1.0f;
	if(startTime < endTime) // day sounds
	{
		if(curGTime < (startTime-fadeTime))
		{
			isAudible = false;
			volumeMod = 0.0f;
		}
		else if(curGTime >= (startTime-fadeTime) && curGTime < (startTime)) // fade in
		{
			volumeMod = R3D_CLAMP((curGTime-(startTime-fadeTime))/fadeTime, 0.0f, 1.0f);
		}
		else if(curGTime >= (endTime-fadeTime) && curGTime < (endTime)) // fade out
		{
			volumeMod = 1.0f-R3D_CLAMP((curGTime-(endTime-fadeTime))/fadeTime, 0.0f, 1.0f);
		}
		else if(curGTime > endTime)
		{
			isAudible = false;
			volumeMod = 0.0f;
		}
	}
	else // night sounds
	{
		if(curGTime >= endTime && curGTime <= (startTime-fadeTime))
		{
			isAudible = false;
			volumeMod = 0.0f;
		}
		else if(curGTime>=(endTime-fadeTime) && curGTime < endTime) // fade out
		{
			volumeMod = 1.0f-R3D_CLAMP((curGTime-(endTime-fadeTime))/fadeTime, 0.0f, 1.0f);
		}
		else if(curGTime>(startTime-fadeTime) && curGTime < startTime) // fade in
		{
			volumeMod = R3D_CLAMP((curGTime-(startTime-fadeTime))/fadeTime, 0.0f, 1.0f);
		}
	}

	if(isMusicTrigger)
	{
		// check if we already playing music, and if yes, check if it finished
		if(sndEvent)
		{
			int soundState = SoundSys.GetState(sndEvent);
			if(!SoundSys.isPlaying(sndEvent)) 		
			{
				SoundSys.Release(sndEvent);
				sndEvent = NULL;

				nextPlayAllowedAt = r3dGetTime() + triggerCooldownTime;
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
				if(r3dGetTime() > nextPlayAllowedAt && isAudible)
				{
					// check if player is inside of trigger area
					if((plPos-pos).Length() < max3DDist)
					{
						sndEvent = SoundSys.Play(sndID, pos);
						SoundSys.SetPaused(sndEvent, true); // start paused to prevent one tick spike in sound volume
						SoundSys.SetProperty(sndEvent, FMOD_EVENTPROPERTY_VOLUME, &masterVolume);
						SoundSys.SetPaused(sndEvent, false);

					}
				}
			}
		}
	}
	else 
	{
		if (!SoundSys.IsHandleValid(sndEvent))
		{
			if(sndEvent)
			{
				SoundSys.Release(sndEvent);
				sndEvent = NULL;
			}
			if(isAudible)
			{
				sndEvent = SoundSys.Play(sndID, pos);
				SoundSys.SetPaused(sndEvent, true); // start paused to prevent one tick spike in sound volume before we set min\max distances
			}
		}
		else
		{
			if (!isAudible)
			{
				if(sndEvent)
				{
					SoundSys.Release(sndEvent);
					sndEvent = NULL;
				}
			}
			else
			{
				SoundSys.SetSoundPos(sndEvent, pos);

				if(SoundSys.Is3DSound(sndEvent))
				{
					SoundSys.SetProperty(sndEvent, FMOD_EVENTPROPERTY_3D_MINDISTANCE, &min3DDist);
					SoundSys.SetProperty(sndEvent, FMOD_EVENTPROPERTY_3D_MAXDISTANCE, &max3DDist);
					float volume = masterVolume*volumeMod;
					SoundSys.SetProperty(sndEvent, FMOD_EVENTPROPERTY_VOLUME, &volume);
				}
				else // if sound is 2D, simulate linear roll off
				{
					float volume = (1.0f - R3D_CLAMP((dist-min3DDist)/(max3DDist-min3DDist), 0.0f, 1.0f))*masterVolume*volumeMod;
					SoundSys.SetProperty(sndEvent, FMOD_EVENTPROPERTY_VOLUME, &volume);

				}

				if(SoundSys.GetPaused(sndEvent))
					SoundSys.SetPaused(sndEvent, false);
			}
		}
	}
	return 1;
}

struct AmbientSoundRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		AmbientSoundRenderable *This = static_cast<AmbientSoundRenderable*>( RThis );

		This->Parent->DoDraw();
	}

	obj_AmbientSound* Parent;
};

void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);
struct SoundHelperRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		SoundHelperRenderable *This = static_cast<SoundHelperRenderable*>( RThis );

		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

		if((This->Parent->GetPosition() - Cam).Length() < 100)
			r3dDrawIcon3D(This->Parent->GetPosition(), SoundIcon, r3dColor(255,255,255), 32);

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
	}

	obj_AmbientSound* Parent;	
};

#define RENDERABLE_SOUND_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_AmbientSound::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )  /*OVERRIDE*/
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
		AmbientSoundRenderable rend;
		rend.Init();
		rend.SortValue = RENDERABLE_SOUND_USER_SORT_VALUE;
		rend.Parent = this;
		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}

	// helper
	extern int CurHUDID;
	if(CurHUDID == 0)
	{
		SoundHelperRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_SOUND_USER_SORT_VALUE;
		render_arrays[ rsDrawComposite1 ].PushBack( rend );
	}
#endif
}

extern ObjectManipulator3d g_Manipulator3d;
void obj_AmbientSound::DoDraw()
{
#ifndef FINAL_BUILD
	if(forceShowSoundBubble || g_Manipulator3d.IsSelected(this))
	{
		r3dColor clr = g_Manipulator3d.IsSelected(this)?r3dColor::white:r3dColor::yellow;
		clr.A = 128;
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

		r3dDrawUniformSphere ( GetPosition(), max3DDist, gCam, clr );
		r3dRenderer->Flush();
	}
#endif
}

void obj_AmbientSound::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node ambientSoundNode = node.child("ambientSound");
	const char *fn = ambientSoundNode.attribute("Sound").value();
	FileName = fn;
	sndID = SoundSys.GetEventIDByPath(FileName.c_str());

	if(!ambientSoundNode.attribute("min3DDist").empty())
	{
		min3DDist = ambientSoundNode.attribute("min3DDist").as_float();
		max3DDist = ambientSoundNode.attribute("max3DDist").as_float();
	}
	if(!ambientSoundNode.attribute("volume").empty())
		masterVolume = ambientSoundNode.attribute("volume").as_float();

	if(!ambientSoundNode.attribute("startTime").empty())
	{
		startTime = ambientSoundNode.attribute("startTime").as_float();
		endTime = ambientSoundNode.attribute("endTime").as_float();
		fadeTime = ambientSoundNode.attribute("fadeTime").as_float();
	}
	
	if(!ambientSoundNode.attribute("isMusicTrigger").empty())
	{
		isMusicTrigger = ambientSoundNode.attribute("isMusicTrigger").as_int();
		triggerCooldownTime = ambientSoundNode.attribute("triggerCooldownTime").as_float();
	}
}

void obj_AmbientSound::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node ambientSoundNode = node.append_child();
	ambientSoundNode.set_name("ambientSound");
	ambientSoundNode.append_attribute("Sound") = FileName.c_str();
	ambientSoundNode.append_attribute("min3DDist") = min3DDist;
	ambientSoundNode.append_attribute("max3DDist") = max3DDist;
	ambientSoundNode.append_attribute("volume") = masterVolume;
	ambientSoundNode.append_attribute("startTime") = startTime;
	ambientSoundNode.append_attribute("endTime") = endTime;
	ambientSoundNode.append_attribute("fadeTime") = fadeTime;
	ambientSoundNode.append_attribute("isMusicTrigger") = isMusicTrigger;
	ambientSoundNode.append_attribute("triggerCooldownTime") = triggerCooldownTime;
}

GameObject*	obj_AmbientSound::Clone()
{
	obj_AmbientSound * pClone = (obj_AmbientSound*)( srv_CreateGameObject ( "obj_AmbientSound", "cloneAmbientSound", GetPosition() ) );
	r3d_assert ( pClone );

	pClone->FileName = FileName;
	pClone->min3DDist = min3DDist;
	pClone->max3DDist = max3DDist;
	pClone->masterVolume = masterVolume;
	pClone->startTime = startTime;
	pClone->endTime= endTime;
	pClone->fadeTime= fadeTime;
	pClone->triggerCooldownTime= triggerCooldownTime;
	pClone->isMusicTrigger= isMusicTrigger;
	pClone->sndID = SoundSys.GetEventIDByPath(FileName.c_str());

	return pClone;
}
