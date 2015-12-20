//=========================================================================
//	Module: r3dInterpolator.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

class r3dSlerpInterpolator
{
	r3dSlerpInterpolator( const r3dSlerpInterpolator& rhs) { }
	const r3dSlerpInterpolator& operator =( const r3dSlerpInterpolator& rhs ) { }

public:
	r3dSlerpInterpolator();
	r3dSlerpInterpolator( const r3dVector& start, const r3dVector& finish, float timeToFinish, bool startPaused = false, bool inXZPlane = true );
	~r3dSlerpInterpolator();

	void Reset( const r3dVector& start, const r3dVector& finish, float timeToFinish, bool startPaused = false, bool inXZPlane = true );

	void Update();
	void Pause();
	void Resume();

	const r3dVector GetCurrent() const;
	const float& GetFinishAngle() const;

	bool IsPaused() const;
	bool IsFinished() const;

#ifndef FINAL_BUILD
public:
	r3dVector	m_startVec;
	r3dVector	m_finishVec;
#endif
protected:
	r3dQuat		m_start;
	r3dQuat		m_finish;
	float		m_timeToFinish;
	float		m_elapsedTime;

	float		m_finishAngle;

	bool		m_isPaused;
	bool		m_isFinished;
};