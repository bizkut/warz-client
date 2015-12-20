#pragma once

class obj_EnvmapProbe;

class EnvmapProbes
{
	// types
public:
	typedef r3dTL::TArray< obj_EnvmapProbe* > ProbeArr;
	static const int WRONG_IDX = -1;

	// construction/ destruction
public:
	EnvmapProbes();
	~EnvmapProbes();

	// manipulation/ access
public:
	r3dTexture*	GetClosestTexture( const r3dPoint3D& pos ) const;
	void		Register( obj_EnvmapProbe* probe );
	void		Unregister( obj_EnvmapProbe* probe );

	void		SetEnvmapUpdateMode( bool bUpdateEnvmap );

	int					GetCount() const;
	obj_EnvmapProbe*	GetProbe( int idx ) const;

	void		DeselectAll();
	void		GenerateAll();
	void		Generate( int id ) ;

	void		Init();
	void		Close();

	void		MarkDirty();
	void		Update( const r3dPoint3D& camPos );

	void		SetForceGlobal( bool forceGlobal );

	// helpers
private:
	int			GetIdxOf( obj_EnvmapProbe* probe );
	void		ResetGenStep();

	// data
private:
	 ProbeArr		mProbes;
	 r3dTexture*	mGlobalEnvmap;

	 int			mEnvmapDirty;
	 int			mEnvmapStep;
	 int			mEnvmapSubStep;
	 int			mEnvmapObjPos;

	 bool			mForceGlobal;

	 r3dPoint3D		mLastCamPos;
	 float			mLastTimeOfDay;

	 bool			mEnvmapUpdate;
} extern g_EnvmapProbes;
