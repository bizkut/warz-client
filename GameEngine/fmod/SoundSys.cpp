#include "r3dPCH.h"
#include "r3d.h"

#include "SoundSys.h"

static	int	gNextSoundID = 5000;

#define MAX_SOUND_GROUPS 64
static	int	sndGroupVolumes[MAX_SOUND_GROUPS];

#define MAX_SOUND_CHANNELS 1024

r3dPoint3D	__SND_ListenerPos;

// libs
/*#ifdef _DEBUG
#pragma comment(lib, "fmodexL_vc.lib")
#pragma comment(lib, "fmod_eventL.lib")
#pragma comment(lib, "fmod_event_netL.lib")
#else
#pragma comment(lib, "fmodex_vc.lib")
#pragma comment(lib, "fmod_event.lib")
#pragma comment(lib, "fmod_event_net.lib")
#endif*/
// ptumik: only link with release libs, as debug sometimes are very slow to load sound library
#pragma comment(lib, "fmodex_vc.lib")
#pragma comment(lib, "fmod_event.lib")
#pragma comment(lib, "fmod_event_net.lib")


//
//
// Actual Code
//
//

CSoundSystem	SoundSys;

CSoundSystem::CSoundSystem()
: FMODEventSystem(0)
, m_SoundBank00(0)
, m_SoundBank_Streaming(0)
, m_SoundBank_Weapons(0)
, m_isSoundDisabled(false)
, m_WarZ_SoundBank(0)
{
	__SND_ListenerPos.Assign(0,0,0);

}

CSoundSystem::~CSoundSystem()
{
	Close();
}

//----------------------------------------------------------------
// define file system callbacks
FMOD_RESULT F_CALLBACK FMOD_OpenCallback(const char* name, int unicode, unsigned int* filesize, void** handle, void** userdata)
{
	r3dFile* file = r3d_open(name, "rb");
	if(!file)
		return FMOD_ERR_FILE_NOTFOUND;
	*filesize = file->size;
	*handle = (void*)file;

	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_CloseCallback(void* handle, void* userdata)
{
	r3dFile* file = (r3dFile*)handle;
	fclose(file);

	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_ReadCallback(void* handle, void* buffer, unsigned int sizebytes, unsigned int* bytesread, void* userdata)
{
	r3dFile* file = (r3dFile*)handle;
	*bytesread = fread(buffer, 1, sizebytes, file);
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_SeekCallback(void* handle, unsigned int pos, void* userdata)
{
	r3dFile* file = (r3dFile*)handle;
	fseek(file, pos, SEEK_SET);
	
	return FMOD_OK;
}

int CSoundSystem::Open()
{
	r3dOutToLog("SOUND: initializing sound system\n");
	FMOD_RESULT      result;
	unsigned int     version;
	int              numdrivers;
	FMOD_SPEAKERMODE speakermode;
	FMOD_CAPS        caps;
	char             name[256];

	m_SoundBank00 = NULL;
	m_SoundBank_Streaming = NULL;
	m_SoundBank_Weapons = NULL;
	m_WarZ_SoundBank = NULL;

	// Create a System object and initialize.
	result = FMOD::EventSystem_Create(&FMODEventSystem);
	SND_ERR_CHK(result);

	FMOD::System *FMODSystem = 0;
	result = FMODEventSystem->getSystemObject(&FMODSystem);
	SND_ERR_CHK(result);

	result = FMODSystem->getVersion(&version);
	SND_ERR_CHK(result);

	if (version < FMOD_VERSION)
	{
		r3dError("Sound Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
		return 0;
	}

	result = FMODSystem->getNumDrivers(&numdrivers);
	SND_ERR_CHK(result);

	r3dOutToLog("SOUND: '%d' sound drivers found\n", numdrivers);
	if (numdrivers == 0)
	{
		m_isSoundDisabled = true;
		result = FMODSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		SND_ERR_CHK(result);
	}
	else
	{
		result = FMODSystem->getDriverCaps(0, &caps, 0, &speakermode);
		SND_ERR_CHK(result);

		r3dOutToLog("SOUND: sound caps = 0x%X, speakermode= %d\n", caps, speakermode);

		result = FMODSystem->setSpeakerMode(speakermode);       /* Set the user selected speaker mode. */
		SND_ERR_CHK(result);

		if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
		{                                                   /* You might want to warn the user about this. */
			r3dOutToLog("Sound: Acceleration slider is off! Very bad for performance");
			result = FMODSystem->setDSPBufferSize(1024, 10);
			SND_ERR_CHK(result);
		}

		result = FMODSystem->getDriverInfo(0, name, 256, 0);
		SND_ERR_CHK(result);

		r3dOutToLog("SOUND: sound driver '%s'\n", name);

		if (strstr(name, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
		{
			result = FMODSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
			SND_ERR_CHK(result);
		}
	}
	
	FMOD_INITFLAGS fmodInitFlags = FMOD_INIT_NORMAL;
#ifndef FINAL_BUILD
	fmodInitFlags |= FMOD_INIT_ENABLE_PROFILE;
#endif

	result = FMODEventSystem->init(MAX_SOUND_CHANNELS, fmodInitFlags, 0);
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
	{
		result = FMODSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		SND_ERR_CHK(result);

		result = FMODEventSystem->init(MAX_SOUND_CHANNELS, fmodInitFlags, 0);/* ... and re-init. */
		SND_ERR_CHK(result);
	}
	else if(result == FMOD_ERR_NET_SOCKET_ERROR)
	{
		FMODEventSystem->release();
		FMODEventSystem = NULL;
		r3dOutToLog("CSoundSystem disabled because of socket error\n");
		return false;
	}
	else if(result!=FMOD_OK) // failed to init sound, drop back to no sound and notify user
	{
		m_isSoundDisabled = true;
		result = FMODSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		SND_ERR_CHK(result);

		result = FMODEventSystem->init(MAX_SOUND_CHANNELS, fmodInitFlags, 0);
	}
	SND_ERR_CHK(result);

	// override FMOD filesystem
	result = FMODSystem->setFileSystem(&FMOD_OpenCallback, &FMOD_CloseCallback, &FMOD_ReadCallback, &FMOD_SeekCallback, 0, 0, 2048);
	SND_ERR_CHK(result);

	// ptumik: no need to call this, as by default it is set to 1
	//result = FMODEventSystem->set3DNumListeners(1);
	//SND_ERR_CHK(result);
	float dopplerShift = 1.0f;
	float distanceScale = 1.0f; // meters
	float rolloffScale = 1.0f; // default 1.0f. higher value will make sounds "disappear" faster
	result = FMODSystem->set3DSettings(dopplerShift, distanceScale, rolloffScale);
	SND_ERR_CHK(result);

	FMOD_REVERB_PROPERTIES prop1 = FMOD_PRESET_OFF;
	FMODSystem->setReverbAmbientProperties(&prop1);

	//	Init FMOD Network API
#ifndef FINAL_BUILD
	FMOD::NetEventSystem_Init(FMODEventSystem);
#endif

	r3dOutToLog("SOUND: sound system ready\n");
	return true;
}

//----------------------------------------------------------------

int CSoundSystem::Close()
{
	m_isSoundDisabled = false;

	while(!m_ManagedSoundsList.empty())
	{
		ManagedSoundS handle = m_ManagedSoundsList.front();
		Release(handle.handle);
		m_ManagedSoundsList.pop_front();
	}

	if (soundsProject)
	{
		soundsProject->release();
		soundsProject = 0;
	}

	if (FMODEventSystem)
	{
		FMODEventSystem->unloadFSB("Sounds_bank00.fsb", 0);
		m_SoundBank00->release();
		FMODEventSystem->unloadFSB("WarZ_Music.fsb", 0);
		m_SoundBank_Streaming->release();
		FMODEventSystem->unloadFSB("AutomaticWeapons.fsb", 0);
		m_SoundBank_Weapons->release();

		FMODEventSystem->unloadFSB("WarZ_Sounds.fsb", 0);
		m_WarZ_SoundBank->release();

		FMOD::NetEventSystem_Shutdown();
		FMODEventSystem->release();
		FMODEventSystem = 0;
	}

	return 1;
}

int CSoundSystem::LoadSoundEffects(const char *basedir, const char *fname)
{
	if(!FMODEventSystem)
	{
		r3dOutToLog("LoadSoundEffects: SoundSystem not active\n");
		return 0; 
	}

	FMOD_RESULT rv;
	const int bufSize = 256;
	char full_name[bufSize];
	sprintf(full_name, "%s\\%s", basedir, fname);
	r3dOutToLog("FMOD: LoadSoundEffects: Loading from %s\n", full_name);

	r3dFile* f = r3d_open(full_name, "rb");
	if ( !f )
	{
		r3dOutToLog("Failed to open %s\n", full_name);
		return 0;
	}

	sprintf(full_name, "%s\\", basedir);
	rv = FMODEventSystem->setMediaPath(full_name); SND_ERR_CHK(rv);

	char* fileBuffer = game_new char[f->size + 1];
	r3d_assert(fileBuffer);
	fread(fileBuffer, f->size, 1, f);
	fileBuffer[f->size] = 0;

	FMOD_EVENT_LOADINFO loadInfo;
	loadInfo.size = sizeof(FMOD_EVENT_LOADINFO);
	loadInfo.loadfrommemory_length = f->size;
	rv = FMODEventSystem->load(fileBuffer, &loadInfo, &soundsProject); SND_ERR_CHK(rv);

	fclose(f);
	delete [] fileBuffer;

	// preload FSB files, as otherwise we will have lags during gameplay
	FMOD::System *FMODSystem = 0;
	rv = FMODEventSystem->getSystemObject(&FMODSystem); SND_ERR_CHK(rv);
	rv = FMODSystem->createSound("Data/Sounds/Sounds_bank00.fsb", FMOD_CREATECOMPRESSEDSAMPLE, 0, &m_SoundBank00); SND_ERR_CHK(rv);
	rv = FMODEventSystem->preloadFSB("Sounds_bank00.fsb", 0, m_SoundBank00); SND_ERR_CHK(rv);

	rv = FMODSystem->createSound("Data/Sounds/WarZ_Music.fsb", FMOD_CREATECOMPRESSEDSAMPLE, 0, &m_SoundBank_Streaming); SND_ERR_CHK(rv);
	rv = FMODEventSystem->preloadFSB("WarZ_Music.fsb", 0, m_SoundBank_Streaming); SND_ERR_CHK(rv);

	rv = FMODSystem->createSound("Data/Sounds/AutomaticWeapons.fsb", FMOD_CREATECOMPRESSEDSAMPLE, 0, &m_SoundBank_Weapons); SND_ERR_CHK(rv);
	rv = FMODEventSystem->preloadFSB("AutomaticWeapons.fsb", 0, m_SoundBank_Weapons); SND_ERR_CHK(rv);

	rv = FMODSystem->createSound("Data/Sounds/WarZ_Sounds.fsb", FMOD_CREATECOMPRESSEDSAMPLE, 0, &m_WarZ_SoundBank); SND_ERR_CHK(rv);
	rv = FMODEventSystem->preloadFSB("WarZ_Sounds.fsb", 0, m_WarZ_SoundBank); SND_ERR_CHK(rv);

	return 1;
}

FMOD::EventReverb* CSoundSystem::createPresetReverb(const char* presetName)
{
	if (!FMODEventSystem || !soundsProject)
		return NULL;

	FMOD_RESULT result;
	FMOD_REVERB_PROPERTIES props;
#define CONVERT_PROP(x) { FMOD_REVERB_PROPERTIES temp = x; memcpy(&props, &temp, sizeof(FMOD_REVERB_PROPERTIES)); }
	if(strcmp(presetName, "none")==0)
		CONVERT_PROP(FMOD_PRESET_OFF)
	else if(strcmp(presetName, "generic")==0)
		CONVERT_PROP(FMOD_PRESET_GENERIC)
	else if(strcmp(presetName, "padded_cell")==0)
		CONVERT_PROP(FMOD_PRESET_PADDEDCELL)
	else if(strcmp(presetName, "room")==0)
		CONVERT_PROP(FMOD_PRESET_ROOM)
	else if(strcmp(presetName, "bathroom")==0)
		CONVERT_PROP(FMOD_PRESET_BATHROOM)
	else if(strcmp(presetName, "livingroom")==0)
		CONVERT_PROP(FMOD_PRESET_LIVINGROOM)
	else if(strcmp(presetName, "stoneroom")==0)
		CONVERT_PROP(FMOD_PRESET_STONEROOM)
	else if(strcmp(presetName, "auditorium")==0)
		CONVERT_PROP(FMOD_PRESET_AUDITORIUM)
	else if(strcmp(presetName, "concerthall")==0)
		CONVERT_PROP(FMOD_PRESET_CONCERTHALL)
	else if(strcmp(presetName, "cave")==0)
		CONVERT_PROP(FMOD_PRESET_CAVE)
	else if(strcmp(presetName, "arena")==0)
		CONVERT_PROP(FMOD_PRESET_ARENA)
	else if(strcmp(presetName, "hangar")==0)
		CONVERT_PROP(FMOD_PRESET_HANGAR)
	else if(strcmp(presetName, "carpettedhallway")==0)
		CONVERT_PROP(FMOD_PRESET_CARPETTEDHALLWAY)
	else if(strcmp(presetName, "hallway")==0)
		CONVERT_PROP(FMOD_PRESET_HALLWAY)
	else if(strcmp(presetName, "stonecorridor")==0)
		CONVERT_PROP(FMOD_PRESET_STONECORRIDOR)
	else if(strcmp(presetName, "alley")==0)
		CONVERT_PROP(FMOD_PRESET_ALLEY)
	else if(strcmp(presetName, "forest")==0)
		CONVERT_PROP(FMOD_PRESET_FOREST)
	else if(strcmp(presetName, "city")==0)
		CONVERT_PROP(FMOD_PRESET_CITY)
	else if(strcmp(presetName, "mountains")==0)
		CONVERT_PROP(FMOD_PRESET_MOUNTAINS)
	else if(strcmp(presetName, "quarry")==0)
		CONVERT_PROP(FMOD_PRESET_QUARRY)
	else if(strcmp(presetName, "plain")==0)
		CONVERT_PROP(FMOD_PRESET_PLAIN)
	else if(strcmp(presetName, "parkinglot")==0)
		CONVERT_PROP(FMOD_PRESET_PARKINGLOT)
	else if(strcmp(presetName, "sewerpipe")==0)
		CONVERT_PROP(FMOD_PRESET_SEWERPIPE)
	else if(strcmp(presetName, "underwater")==0)
		CONVERT_PROP(FMOD_PRESET_UNDERWATER)
	else
	{
		result = FMODEventSystem->getReverbPreset(presetName, &props, NULL);
		SND_ERR_CHK(result);
	}
#undef CONVERT_PROP

	FMOD::EventReverb* reverb = NULL;
	result = FMODEventSystem->createReverb(&reverb);
	SND_ERR_CHK(result);

	result = reverb->setProperties(&props);
	SND_ERR_CHK(result);
	
	return reverb;
}

//----------------------------------------------------------------
float CSoundSystem::getEventMax3DDistance(int SampleID)
{
	if (!FMODEventSystem || !soundsProject)
		return -1.0f;
	if(SampleID == -1)
		return -1.0f;

	FMOD::Event *e = 0;
	FMOD_RESULT result;
	result = soundsProject->getEventByProjectID(SampleID, FMOD_EVENT_INFOONLY, &e);
	SND_ERR_CHK(result);

	if (!e) return -1.0f;

	FMOD_MODE mode;
	e->getPropertyByIndex(FMOD_EVENTPROPERTY_MODE, &mode);
	r3d_assert(mode == FMOD_3D); // should be called only for 3D events

	float dist=0.0f;
	e->getPropertyByIndex(FMOD_EVENTPROPERTY_3D_MAXDISTANCE, &dist);

	return dist;
}

void* CSoundSystem::Play(int SampleID, const r3dPoint3D& Pos, bool donotcheckIsAudible)
{
	if (!FMODEventSystem || !soundsProject)
		return NULL;
	if(SampleID == -1)
		return NULL;

	FMOD::Event *e = 0;
	FMOD_RESULT result;
	result = soundsProject->getEventByProjectID(SampleID, FMOD_EVENT_INFOONLY, &e);
	SND_ERR_CHK(result);

	if(Is3DSound(e))
		return Play3D(SampleID, Pos, donotcheckIsAudible);
	return Play2D(SampleID);
}

void CSoundSystem::PlayAndForget(int SampleID, const r3dPoint3D& Pos)
{
	void* handle = Play(SampleID, Pos);
	if(handle)
		m_ManagedSoundsList.push_back(ManagedSoundS(handle));
}

#ifdef VEHICLES_ENABLED
void CSoundSystem::PlayAndForgetWithVolume(int SampleID, const r3dPoint3D& Pos, float volume)
{
	void* handle = Play(SampleID, Pos);
	SoundSys.SetProperty(handle, FMOD_EVENTPROPERTY_VOLUME, &volume);
	if(handle)
		m_ManagedSoundsList.push_back(ManagedSoundS(handle));
}
#endif

void * CSoundSystem::Play2D(int SampleID)
{
	if (!FMODEventSystem || !soundsProject)
		return NULL;

	R3DPROFILE_FUNCTION("CSoundSystem::Play2D");
	FMOD::Event *e = 0;
	FMOD_RESULT result;
	result = soundsProject->getEventByProjectID(SampleID, FMOD_EVENT_NONBLOCKING, &e);
	if (FMOD_OK != result)
		return 0;

	if(e)
	{
		result = e->start();
		SND_ERR_CHK(result);
	}
	return e;
}

bool CSoundSystem::SetSoundPos(void *handle, const r3dPoint3D& pos)
{
	if (!FMODEventSystem || !handle)
		return false;

	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);

	bool paused = false;
	e->getPaused(&paused);
	if(!paused)
		e->set3DAttributes((const FMOD_VECTOR*)&pos, NULL);

	return !paused;
}

void * CSoundSystem::Play3D(int SampleID, const r3dPoint3D& Pos, bool donotcheckIsAudible)
{
	if (!FMODEventSystem)
		return NULL;

	R3DPROFILE_FUNCTION("CSoundSystem::Play3D");
	//	Check for audibility of this sound
	FMOD_RESULT result = FMOD_OK;
	FMOD::Event *eInfo = 0, *e = 0;
	result = soundsProject->getEventByProjectID(SampleID, FMOD_EVENT_INFOONLY, &eInfo);
	if (result != FMOD_OK)
		return 0;

	if(!donotcheckIsAudible)
		if (!IsAudible(eInfo, Pos))
			return 0;

	result = soundsProject->getEventByProjectID(SampleID, FMOD_EVENT_NONBLOCKING, &e); 
	if (FMOD_OK != result)
		return 0;

	if(e)
	{
		result = e->start(); SND_ERR_CHK(result);
		result = e->set3DAttributes(reinterpret_cast<const FMOD_VECTOR*>(&Pos), 0, 0); SND_ERR_CHK(result);
	}
	return e;
}

void CSoundSystem::SetPaused(void* handle, bool paused)
{
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	if (e) 
		e->setPaused(paused);
}

bool CSoundSystem::GetPaused(void* handle)
{
	bool res = false;
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	if (e) 
		e->getPaused(&res);

	return res;
}

void CSoundSystem::Stop(void *handle)
{
	R3DPROFILE_FUNCTION("CSoundSystem::Stop");
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	if (e) e->stop(false);
}

void CSoundSystem::KeyOff(void *handle, char* param_name)
{
	R3DPROFILE_FUNCTION("CSoundSystem::KeyOff");
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	if(e)
	{
		FMOD::EventParameter* eventParam = NULL;
		FMOD_RESULT res = e->getParameter(param_name, &eventParam);
		if(res == FMOD_OK && eventParam)
			eventParam->keyOff();
	}
}

void CSoundSystem::SetParamValue(void *handle, char* param_name, float value)
{
	R3DPROFILE_FUNCTION("CSoundSystem::SetParamValue");
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	if(e)
	{
		FMOD::EventParameter* eventParam = NULL;
		FMOD_RESULT res = e->getParameter(param_name, &eventParam);
		if(res == FMOD_OK && eventParam)
			eventParam->setValue(value);
	}
}

void CSoundSystem::Start(void *handle)
{
	R3DPROFILE_FUNCTION("CSoundSystem::Start");
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	if (e) e->start();
}

//////////////////////////////////////////////////////////////////////////

int CSoundSystem::GetState(void *handle) const
{
	R3DPROFILE_FUNCTION("CSoundSystem::GetState");
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	FMOD_EVENT_STATE st = 0;
	if (e)
		e->getState(&st);
	return st;
}

bool CSoundSystem::isPlaying(void* handle) const
{
	int state = GetState(handle);
	return ((state & FMOD_EVENT_STATE_CHANNELSACTIVE) || (state & FMOD_EVENT_STATE_PLAYING)); 
}

void CSoundSystem::Update(const r3dPoint3D &pos, const r3dPoint3D &dir, const r3dPoint3D& up)
{
	if(!FMODEventSystem)
		return;
	FMOD_RESULT result;

	// update sounds volumes
	FMOD::EventCategory* eventCat=0;
	result = FMODEventSystem->getCategory("master", &eventCat); SND_ERR_CHK(result);
	eventCat->setVolume(s_sound_volume->GetFloat());
	result = FMODEventSystem->getCategory("music", &eventCat); SND_ERR_CHK(result);
	eventCat->setVolume(s_music_volume->GetFloat());

	r3dPoint3D vDir = dir.NormalizeTo();
	r3dPoint3D left = up.Cross(vDir);
	left.Normalize();
	r3dPoint3D correctUp = vDir.Cross(left);
	correctUp.Normalize();
	result = FMODEventSystem->set3DListenerAttributes(0, (const FMOD_VECTOR*)&pos, 0, (const FMOD_VECTOR*)&vDir, (const FMOD_VECTOR*)&correctUp);
	SND_ERR_CHK(result);
	result = FMODEventSystem->update();
	SND_ERR_CHK(result);
	result = FMOD::NetEventSystem_Update();
	SND_ERR_CHK(result);

	ManagedSoundsList::iterator it;
	float ft = r3dGetFrameTime();
	for(it=m_ManagedSoundsList.begin(); it!=m_ManagedSoundsList.end();)
	{
		ManagedSoundS& s = *it;
		if(!isPlaying(s.handle))
			s.timeNotPlaying += ft;
		if(s.timeNotPlaying > 5.0f) // wait 5 seconds before release, otherwise when playing a lot of similar sounds, it will cutoff those that are still playing. fmod bug i think.
		{
			Release(s.handle);
			it = m_ManagedSoundsList.erase(it);
			continue;
		}
		++it;
	}

	/*FMOD::System *s = 0;
	FMODEventSystem->getSystemObject(&s);
	// update low pass filter to simulate loss of high frequency details with the distance
	for( int i = 0; i < MAX_SOUND_CHANNELS; ++i )
	{
		FMOD::Channel* pChannel = NULL;
		result = s->getChannel( i, &pChannel );
		if( result == FMOD_OK )
		{
			FMOD_VECTOR temp;
			result = pChannel->get3DAttributes( &temp, NULL );
			r3dVector sound_pos(temp.x, temp.y, temp.z);
			if( result == FMOD_OK )
			{
				float distance = (pos-sound_pos).Length();
				float occlusion = 0.f;

				float distantLowpassRange_max = 80.0f;
				float distantLowpassRange_min = 15.0f;
				float distantLowpassRange_delta = distantLowpassRange_max-distantLowpassRange_min;
				float maxDistantOcclusion = 0.8f;
				
				if( distance > distantLowpassRange_max )
				{
					occlusion = maxDistantOcclusion;
				}
				else if( distance > distantLowpassRange_min )
				{
					occlusion = ( ( distance - distantLowpassRange_min ) / distantLowpassRange_delta ) * maxDistantOcclusion;
				}
				result = pChannel->set3DOcclusion( occlusion, 0.f );
			}
		}
	}*/
}

//////////////////////////////////////////////////////////////////////////

void CSoundSystem::Release(void *handle)
{
	R3DPROFILE_FUNCTION("CSoundSystem::Release");
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	if (e)
	{
		FMOD_RESULT rv = FMOD_OK;
		int groupIdx = -1;
		FMOD::EventGroup *eg = 0;
		e->getParentGroup(&eg);
		if (eg)
		{
			rv = eg->freeEventData(e);
			SND_ERR_CHK(rv);
		}
		e->release();
	}
}

//////////////////////////////////////////////////////////////////////////

float CSoundSystem::GetSoundMaxDistance(void *handle) const
{
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	float maxDist = 0;
	if (e)
	{
		e->getPropertyByIndex(FMOD_EVENTPROPERTY_3D_MAXDISTANCE, &maxDist, true); // true here, as when we are calling setPropertyByIndex, we are setting it only for that particular instance
	}
	return maxDist;
}

//////////////////////////////////////////////////////////////////////////

float CSoundSystem::GetSoundMaxDistance(int id) const
{
	FMOD::Event *e = 0;
	soundsProject->getEventByProjectID(id, FMOD_EVENT_INFOONLY, &e);
	return GetSoundMaxDistance(e);
}

//////////////////////////////////////////////////////////////////////////

int CSoundSystem::GetEventIDByPath(const char *path) const
{
	if (!path || !soundsProject)
		return -1;

	if(strlen(path)<3)
		return -1;

	FMOD::Event *e = 0;
	FMOD_RESULT rv = FMODEventSystem->getEvent(path, FMOD_EVENT_INFOONLY, &e);
	//r3d_assert(rv == FMOD_OK);
	if (rv != FMOD_OK)
	{
#ifdef _DEBUG
		r3dOutToLog("SoundSystem: Cannot find event with path: %s\n", path);
#endif
		return -1;
	}

	FMOD_EVENT_INFO ei;
	rv = e->getInfo(0, 0, &ei);
	e->release();
	r3d_assert(rv == FMOD_OK);
	if (rv != FMOD_OK)
		return -1;

	return ei.projectid;
}

//////////////////////////////////////////////////////////////////////////

void CSoundSystem::Set3DAttributes(void *handle, const r3dVector *pos, const r3dVector *velocity, const r3dVector *orientataion) const
{
	R3DPROFILE_FUNCTION("CSoundSystem::Set3DAttributes");
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	float maxDist = 0;
	FMOD_RESULT result;
	if (e && Is3DSound(handle))
	{
		const FMOD_VECTOR *fmPos = reinterpret_cast<const FMOD_VECTOR*>(pos);
		const FMOD_VECTOR *fmVel = reinterpret_cast<const FMOD_VECTOR*>(velocity);
		const FMOD_VECTOR *fmOrientataion = reinterpret_cast<const FMOD_VECTOR*>(orientataion);
		result = e->set3DAttributes(fmPos, fmVel, fmOrientataion);
		SND_ERR_CHK(result);
	}
}

//////////////////////////////////////////////////////////////////////////

bool CSoundSystem::IsHandleValid(void *handle) const
{
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	FMOD_RESULT r = FMOD_ERR_INVALID_HANDLE;
	if (e)
	{
		bool dummy = false;
		r = e->getPaused(&dummy);
	}
	return r == FMOD_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CSoundSystem::Is3DSound(void *handle) const
{
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	if (!e) return false;

	FMOD_MODE mode;
	e->getPropertyByIndex(FMOD_EVENTPROPERTY_MODE, &mode);
	return mode == FMOD_3D;
}

bool CSoundSystem::IsAudible(void *handle, const r3dPoint3D &pos) const
{
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	if (!e) return false;

	FMOD_MODE mode;
	e->getPropertyByIndex(FMOD_EVENTPROPERTY_MODE, &mode);
	if(mode == FMOD_3D)
	{
		float maxDist = 0;
		e->getPropertyByIndex(FMOD_EVENTPROPERTY_3D_MAXDISTANCE, &maxDist, true);

		r3dPoint3D listenerPos;
		FMODEventSystem->get3DListenerAttributes(0, reinterpret_cast<FMOD_VECTOR*>(&listenerPos), 0, 0, 0);

		return (pos - listenerPos).Length() <= maxDist;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

void AppendParentRecursively(r3dSTLString &path, FMOD::EventGroup *eg);
void AppendParentProject(r3dSTLString &path, FMOD::EventProject *ep);

void AppendParentRecursively(r3dSTLString &path, FMOD::Event *e)
{
	char *buf = 0;
	e->getInfo(0, &buf, 0);
	if (buf)
		path = r3dSTLString("/") + buf;
	FMOD::EventGroup *eg = 0;
	e->getParentGroup(&eg);
	if (eg)
		AppendParentRecursively(path, eg);
}

//////////////////////////////////////////////////////////////////////////

void AppendParentRecursively(r3dSTLString &path, FMOD::EventGroup *eg)
{
	char *buf= 0;
	eg->getInfo(0, &buf);
	if (buf)
	{
		path = r3dSTLString("/") + buf + path;
	}
	FMOD::EventGroup *nextEG = 0;
	eg->getParentGroup(&nextEG);
	if (nextEG)
	{
		AppendParentRecursively(path, nextEG);
	}
	else
	{
		FMOD::EventProject *ep = 0;
		eg->getParentProject(&ep);
		if (ep)
			AppendParentProject(path, ep);
	}
}

//////////////////////////////////////////////////////////////////////////

void AppendParentProject(r3dSTLString &path, FMOD::EventProject *ep)
{
	FMOD_EVENT_PROJECTINFO pi;
	ep->getInfo(&pi);
	if (pi.name[0]!='\0')
	{
		path = pi.name + path;
	}
}

//////////////////////////////////////////////////////////////////////////

const stringlist_t & CSoundSystem::GetSoundsList() const
{
	if (FMODEventSystem && soundsList.empty())
	{
		int numEvents = 0;
		FMODEventSystem->getNumEvents(&numEvents);
		
		soundsList.reserve(numEvents);
		for (int i = 0; i < numEvents; ++i)
		{
			FMOD::Event *e = 0;
			FMODEventSystem->getEventBySystemID(i, FMOD_EVENT_INFOONLY, &e);
			if (e)
			{
				soundsList.resize(soundsList.size() + 1);
				AppendParentRecursively(soundsList.back(), e);
			}

		}
	}
	return soundsList;
}

//////////////////////////////////////////////////////////////////////////

void CSoundSystem::SetProperty(void *handle, FMOD_EVENT_PROPERTY propID, void *value)
{
	R3DPROFILE_FUNCTION("CSoundSystem::SetProperty");
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	if (!e) return;

	FMOD_RESULT r = e->setPropertyByIndex(propID, value, true);
	r3d_assert(r == FMOD_OK);
}

void CSoundSystem::GetProperty(void *handle, FMOD_EVENT_PROPERTY propID, void *value)
{
	R3DPROFILE_FUNCTION("CSoundSystem::GetProperty");
	FMOD::Event *e = reinterpret_cast<FMOD::Event*>(handle);
	if (!e) return;

	FMOD_RESULT r = e->getPropertyByIndex(propID, value, true);
	r3d_assert(r == FMOD_OK);
}

//////////////////////////////////////////////////////////////////////////

// Server Wrapping function
int snd_InitSoundSystem()
{
	return SoundSys.Open();
}

int snd_CloseSoundSystem()
{
	return SoundSys.Close();
}


int snd_LoadSoundEffects(char *basedir, char *fname)
{
	return SoundSys.LoadSoundEffects(basedir, fname);
}

int snd_UpdateSoundListener(const r3dPoint3D &Pos, const r3dPoint3D &dir, const r3dPoint3D& up)
{
	SoundSys.Update(Pos, dir, up);
	__SND_ListenerPos = Pos;
	return 1;
}

void snd_SetGroupVolume(int groupId, int Volume)
{
	assert(groupId >= SOUND_GROUP_START && groupId < SOUND_GROUP_START + MAX_SOUND_GROUPS);

	sndGroupVolumes[groupId - SOUND_GROUP_START] = Volume;
}

