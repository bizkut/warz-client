//=========================================================================
//	Module: ParticleEditorUndoSubsystem.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"

#ifndef FINAL_BUILD

#include "r3d.h"
#include "ParticleEditorUndoSubsystem.h"
#include "EditedValueTracker.h"
#include "../ObjectsCode/EFFECTS/obj_ParticleSystem.h"

//-------------------------------------------------------------------------
//	Value tracker list
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//	Particle system values
//-------------------------------------------------------------------------

ValueTracker<float> OrgGlobalScaleTracker;
ValueTracker<float> WarmUpTimeTracker;
ValueTracker<float> DepthBlendValueTracker;
ValueTracker<int> ZSortEnabledTracker;
ValueTracker<int> DeferredMeshParticlesTracker;
ValueTracker<int> DeferredMeshParticlesShadowsTracker;
ValueTracker<int> EmitterTypeTracker;
ValueTracker<float> EmitRadiusTracker;
ValueTracker<r3dVector> EmitVectorTracker;
ValueTracker<float> EmitTimeTracker;
ValueTracker<r3dVector> OrgDirTracker;
ValueTracker<r3dString> TextureFileNameTracker;
ValueTracker<int> BDistortTracker;
ValueTracker<r3dString> DistortTextureFileNameTracker;
ValueTracker<int> BNormalMapTracker;
ValueTracker<r3dString> NormalTextureFileNameTracker;
ValueTracker<float> BumpPowerTracker;
ValueTracker<float> GlowIntensityTracker;
ValueTracker<float> GlowFromDistortB_or_FromDiffuseATracker;
ValueTracker<int> HasLightTracker;
ValueTracker<r3dTimeGradient2> LightColorTracker;
ValueTracker<r3dTimeGradient2> LightRadiusTracker;
ValueTracker<r3dTimeGradient2> LightPowerTracker;

//-------------------------------------------------------------------------
//	Emitter parameters
//-------------------------------------------------------------------------

ValueTracker<int> bEmitterTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<int> bEmitterDistanceSpawnTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<int> bEmmiterTypeIDTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<int> bSingleParticleTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<int> StartFrameTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> FrameRateTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> StartTimeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> EndTimeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> ParticleBirthRateTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dVector> EmmiterOffsetTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> ParticleSizeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> ParticleLifeTimeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> ParticleVelocityTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> ParticleGravityTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> EmitDistanceTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> UVSpeedTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dVector> BornPosRandTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> ParticleSizeVarTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> ParticleLifeTimeVarTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> ParticleVelocityVarTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> ParticleGravityVarTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> ParticleSpinVarTracker(TT_PARTICLE_EMITTER_VARS);

ValueTracker<r3dVector> MotionRandTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> MotionRandDeltaTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> MotionRandSmoothTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<int> bRandomDirectionTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dVector> RandomVectorTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dVector> DirectionRandTracker(TT_PARTICLE_EMITTER_VARS);

ValueTracker<float> RayWidthTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> TrailStepDistTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> TrailSizeCoefMinTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> TrailSizeCoefMaxTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> TrailOpacityCoefMinTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> TrailOpacityCoefMaxTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> TrailTaleFadeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> TrailTaleFadePowTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<float> TrailDriftTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<int> bDirectionOrientedTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> AngleXOverLifeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> AngleYOverLifeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> AngleZOverLifeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dVector> vDirOrientedAdditRotationTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dString> MeshFNameTracker(TT_PARTICLE_EMITTER_VARS); // mesh
ValueTracker<r3dString> MeshTexFNameTracker(TT_PARTICLE_EMITTER_VARS); // texture
ValueTracker<int> bMeshDisableCullingTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<int> bUseDistortionTextureTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dString> MeshDistortTexFNameTracker(TT_PARTICLE_EMITTER_VARS); // Distortion tex
ValueTracker<int> ParticleTypeTracker(TT_PARTICLE_EMITTER_VARS);

ValueTracker<r3dTimeGradient2> ColorOverLifeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> OpacityOverLifeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> BlendModeOverLifeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> SizeOverLifeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> SpinOverLifeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> WeightOverLifeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> BindWeightOverLifeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> VelocityOverLifeTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> FramesOverLifeTracker(TT_PARTICLE_EMITTER_VARS);

ValueTracker<float> BirthChartsTimeLapseTracker(TT_PARTICLE_EMITTER_VARS);
ValueTracker<r3dTimeGradient2> BirthSizeOverLifeTracker(TT_PARTICLE_EMITTER_VARS);

//////////////////////////////////////////////////////////////////////////

void NotifyReloadPartTexture(const char *str);
extern const char * PARTICLE_DATA_PATH_PATTERN;
void ParticleReloadTex(r3dTexture** tex, const char* name);
void ParticleReloadMesh(r3dParticleEmitter &PE, const char *Str2, bool defaultMaterial );
extern r3dParticleData *EditTorch;
extern int curEmitterSlot;
extern obj_ParticleSystem *ParticleObj;

//////////////////////////////////////////////////////////////////////////

void ReloadPartTex(r3dString &path, ValueTracker<r3dString> &)
{
	if (path.Length() == 0)
	{
		EditTorch->Texture = 0;
		return;
	}
	char buf[256] = {0}	;
	sprintf(buf, PARTICLE_DATA_PATH_PATTERN, path.c_str());
	NotifyReloadPartTexture(buf);
}

void ReloadNormalTex(r3dString &path, ValueTracker<r3dString> &)
{
	if (path.Length() == 0)
	{
		EditTorch->NormalTexture = 0;
		return;
	}
	char buf[256] = {0}	;
	sprintf(buf, PARTICLE_DATA_PATH_PATTERN, path.c_str());
	ParticleReloadTex(&EditTorch->NormalTexture, buf);
}

void ReloadDistortTex(r3dString &path, ValueTracker<r3dString> &)
{
	if (path.Length() == 0)
	{
		EditTorch->DistortTexture = 0;
		return;
	}

	char buf[256] = {0}	;
	sprintf(buf, PARTICLE_DATA_PATH_PATTERN, path.c_str());
	ParticleReloadTex(&EditTorch->DistortTexture, buf);
}

void ReloadMeshForEmitter(r3dString &path, ValueTracker<r3dString> &)
{
	char buf[256] = {0}	;
	sprintf(buf, PARTICLE_DATA_PATH_PATTERN, path.c_str());
	if (curEmitterSlot < r3dParticleData::MAX_EMITTER_SLOTS && curEmitterSlot >= 0)
	ParticleReloadMesh(*EditTorch->PType[curEmitterSlot], buf, false);
}

//////////////////////////////////////////////////////////////////////////

void InitParticleValueTrackers(r3dParticleData *pd)
{
	g_pUndoHistory->ClearUndoHistory();

	if (!pd) return;

	OrgGlobalScaleTracker.Init(pd->OrgGlobalScale);
	WarmUpTimeTracker.Init(pd->WarmUpTime);
	DepthBlendValueTracker.Init(pd->DepthBlendValue);
	ZSortEnabledTracker.Init(pd->ZSortEnabled);
	DeferredMeshParticlesTracker.Init(pd->bDeferredMeshParticles);
	DeferredMeshParticlesShadowsTracker.Init(pd->bDeferredMeshParticlesShadows);
	EmitterTypeTracker.Init(pd->EmitterType);
	EmitRadiusTracker.Init(pd->EmitRadius);
	EmitVectorTracker.Init(pd->EmitVector);
	EmitTimeTracker.Init(pd->EmitTime);
	OrgDirTracker.Init(pd->OrgDirection);
	TextureFileNameTracker.Init(pd->TextureFileName);
	TextureFileNameTracker.SetOnUndoCallback(&ReloadPartTex);
	BDistortTracker.Init(pd->bDistort);
	DistortTextureFileNameTracker.Init(pd->DistortTextureFileName);
	DistortTextureFileNameTracker.SetOnUndoCallback(&ReloadDistortTex);
	BNormalMapTracker.Init(pd->bNormalMap);
	NormalTextureFileNameTracker.Init(pd->NormalTextureFileName);
	NormalTextureFileNameTracker.SetOnUndoCallback(&ReloadNormalTex);
	BumpPowerTracker.Init(pd->fBumpPower);
	GlowIntensityTracker.Init(pd->GlowIntensity);
	GlowFromDistortB_or_FromDiffuseATracker.Init(pd->GlowFromDistortB_or_FromDiffuseA);
//	HasLightTracker.Init(pd->HasLight);
//	LightColorTracker.Init(pd->LightColor);
//	LightRadiusTracker.Init(pd->LightRadius);
//	LightPowerTracker.Init(pd->LightPower);
}

//////////////////////////////////////////////////////////////////////////

void InitParticleEmitterValueTrackers(r3dParticleEmitter &pe)
{
	bEmitterTracker.Init(pe.bEmmiter);
	bEmitterDistanceSpawnTracker.Init(pe.bEmitterDistanceSpawn);
	bEmmiterTypeIDTracker.Init(pe.bEmmiterTypeID);
	bSingleParticleTracker.Init(pe.bSingleParticle);
	StartFrameTracker.Init(pe.StartFrame);
	FrameRateTracker.Init(pe.FrameRate);
	StartTimeTracker.Init(pe.StartTime);
	EndTimeTracker.Init(pe.EndTime);
	ParticleBirthRateTracker.Init(pe.ParticleBirthRate);
	EmmiterOffsetTracker.Init(pe.EmmiterOffset);
	ParticleSizeTracker.Init(pe.ParticleSize);
	ParticleLifeTimeTracker.Init(pe.ParticleLifeTime);
	ParticleVelocityTracker.Init(pe.ParticleVelocity);
	ParticleGravityTracker.Init(pe.ParticleGravity);
	EmitDistanceTracker.Init(pe.EmitDistance);
	UVSpeedTracker.Init(pe.UVSpeed);
	BornPosRandTracker.Init(pe.BornPosRand);
	ParticleSizeVarTracker.Init(pe.ParticleSizeVar);
	ParticleLifeTimeVarTracker.Init(pe.ParticleLifeTimeVar);
	ParticleVelocityVarTracker.Init(pe.ParticleVelocityVar);
	ParticleGravityVarTracker.Init(pe.ParticleGravityVar);
	ParticleSpinVarTracker.Init(pe.ParticleSpinVar);

	MotionRandTracker.Init(pe.MotionRand);
	MotionRandDeltaTracker.Init(pe.MotionRandDelta);
	MotionRandSmoothTracker.Init(pe.MotionRandSmooth);
	bRandomDirectionTracker.Init(pe.bRandomDirection);
	RandomVectorTracker.Init(pe.RandomVector);
	DirectionRandTracker.Init(pe.DirectionRand);

	RayWidthTracker.Init(pe.RayWidth);
	TrailStepDistTracker.Init(pe.TrailStepDist);
	TrailSizeCoefMinTracker.Init(pe.TrailSizeCoefMin);
	TrailSizeCoefMaxTracker.Init(pe.TrailSizeCoefMax);
	TrailOpacityCoefMinTracker.Init(pe.TrailOpacityCoefMin);
	TrailOpacityCoefMaxTracker.Init(pe.TrailOpacityCoefMax);
	TrailTaleFadeTracker.Init(pe.TrailTaleFade);
	TrailTaleFadePowTracker.Init(pe.TrailTaleFadePow);
	TrailDriftTracker.Init(pe.TrailDrift);
	bDirectionOrientedTracker.Init(pe.bDirectionOriented);
	AngleXOverLifeTracker.Init(pe.AngleXOverLife);
	AngleYOverLifeTracker.Init(pe.AngleYOverLife);
	AngleZOverLifeTracker.Init(pe.AngleZOverLife);
	vDirOrientedAdditRotationTracker.Init(pe.vDirOrientedAdditRotation);
	MeshFNameTracker.Init(pe.MeshFName); // mesh
	MeshFNameTracker.SetOnUndoCallback(&ReloadMeshForEmitter);
	bMeshDisableCullingTracker.Init(pe.bMeshDisableCulling);
	bUseDistortionTextureTracker.Init(pe.bMeshUseDistortTexture);
	ParticleTypeTracker.Init(pe.ParticleType);

	ColorOverLifeTracker.Init(pe.ColorOverLife);
	OpacityOverLifeTracker.Init(pe.OpacityOverLife);
	BlendModeOverLifeTracker.Init(pe.BlendModeOverLife);
	SizeOverLifeTracker.Init(pe.SizeOverLife);
	SpinOverLifeTracker.Init(pe.SpinOverLife);
	WeightOverLifeTracker.Init(pe.GravityOverLife);
	BindWeightOverLifeTracker.Init(pe.BindGravityOverLife);
	VelocityOverLifeTracker.Init(pe.VelocityOverLife);
	FramesOverLifeTracker.Init(pe.FramesOverLife);

	BirthChartsTimeLapseTracker.Init(pe.BirthChartsTimeLapse);
	BirthSizeOverLifeTracker.Init(pe.BirthSizeOverLife);
}

//-------------------------------------------------------------------------
//	ParticleEmitterDelUndo impl
//-------------------------------------------------------------------------

ParticleEmitterAddDelUndo::~ParticleEmitterAddDelUndo()
{
	delete internalCopy;
}

void ParticleEmitterAddDelUndo::CreateAction()
{
	*pel = internalCopy;
	if (internalCopy)
	{
		int backup = curEmitterSlot;
		curEmitterSlot = slotIndex;
		ValueTracker<r3dString> dummy;
		ReloadMeshForEmitter(internalCopy->MeshFName, dummy);
		curEmitterSlot = backup;

		if (slotIndex == curEmitterSlot)
		{
			ValueTrackersManager::Instance().SetUpdateMask(TT_PARTICLE_EMITTER_VARS | ValueTrackersManager::Instance().GetUpdateMask());
			InitParticleEmitterValueTrackers(**pel);
		}
	}
	internalCopy = 0;
}

//////////////////////////////////////////////////////////////////////////

void ParticleEmitterAddDelUndo::DeleteAction()
{
	r3d_assert(!internalCopy);
	delete internalCopy;

	internalCopy = *pel;
	*pel = 0;
	EditTorch->FixSubEmitters();
	ParticleObj->Torch->ClearParticlesOfType(slotIndex);
	if (curEmitterSlot == slotIndex)
	{
		ValueTrackersManager::Instance().SetUpdateMask(~TT_PARTICLE_EMITTER_VARS & ValueTrackersManager::Instance().GetUpdateMask());
	}
}

//////////////////////////////////////////////////////////////////////////

void ParticleEmitterAddDelUndo::SetAsCreateUndo()
{
	undoFunc = &ParticleEmitterAddDelUndo::DeleteAction;
	redoFunc = &ParticleEmitterAddDelUndo::CreateAction;

	m_sTitle = "Emitter create";
}

//////////////////////////////////////////////////////////////////////////

void ParticleEmitterAddDelUndo::SetAsDeleteUndo()
{
	if (!(*pel)) return;

	undoFunc = &ParticleEmitterAddDelUndo::CreateAction;
	redoFunc = &ParticleEmitterAddDelUndo::DeleteAction;

	internalCopy = gfx_new r3dParticleEmitter();
	internalCopy->Copy(*pel, (*pel)->Name);

	m_sTitle = "Emitter delete";
}

//////////////////////////////////////////////////////////////////////////

void ParticleEmitterAddDelUndo::Undo()
{
	if (undoFunc) (this->*undoFunc)();
}

//////////////////////////////////////////////////////////////////////////

void ParticleEmitterAddDelUndo::Redo()
{
	if (redoFunc) (this->*redoFunc)();
}

#endif