//=========================================================================
//	Module: r3dInterpolator.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "r3dInterpolator.h"

r3dSlerpInterpolator::r3dSlerpInterpolator()
	: m_start( 0, 0, 0, 0 )
	, m_finish( 0, 0, 0, 0 )
	, m_finishAngle( -90.0f )
	, m_timeToFinish( 1.0f )
	, m_elapsedTime( 1.0f )
	, m_isPaused( false )
	, m_isFinished( true )
#ifndef FINAL_BUILD
	, m_startVec( 0, 0, -1 )
	, m_finishVec( 0, 0, -1 )
#endif
{
}

r3dSlerpInterpolator::r3dSlerpInterpolator( const r3dVector& start, const r3dVector& finish, float timeToFinish, bool startPaused /*= false */, bool inXZPlane /*= true */ )
	: m_timeToFinish( timeToFinish )
	, m_elapsedTime( 0.0f )
	, m_isPaused( startPaused )
	, m_isFinished( false )
{
	Reset( start, finish, timeToFinish, startPaused, inXZPlane );
}

r3dSlerpInterpolator::~r3dSlerpInterpolator()
{
}

void r3dSlerpInterpolator::Reset( const r3dVector& start, const r3dVector& finish, float timeToFinish, bool startPaused /*= false */, bool inXZPlane /*= true */ )
{
	m_timeToFinish	= timeToFinish;
	m_elapsedTime	= 0.0f;
	m_isPaused		= startPaused;
	m_isFinished	= false;

	r3dVector v1( ( inXZPlane ) ? r3dVector( start.x, 0.0f, start.z ).Normalize() : start.NormalizeTo() );
	r3dVector v2( ( inXZPlane ) ? r3dVector( finish.x, 0.0f, finish.z ).Normalize() : finish.NormalizeTo() );
#ifndef FINAL_BUILD
	m_startVec = v1;
	m_finishVec = v2;
#endif

	// Convert to quaternions.
	r3dVector vForward( 0, 0, 1 );
	float dot = R3D_CLAMP( vForward.Dot( v1 ), -1.0f, 1.0f );
	r3dVector a( ( 1.0 - fabs( dot ) < 0.0001f ) ? r3dVector( 0.0f, dot, 0.0f ) : vForward.Cross( v1 ).Normalize() );
	float halfAngle = r3dAcos( dot ) * 0.5f;
	float sinHalfAngle = r3dSin( halfAngle );
	m_start.x = a.x * sinHalfAngle;
	m_start.y = a.y * sinHalfAngle;
	m_start.z = a.z * sinHalfAngle;
	m_start.w = r3dCos( halfAngle );
	D3DXQuaternionNormalize( &m_start, &m_start );

#if _DEBUG
	r3d_assert( !_isnan( m_start.x ) && !_isnan( m_start.y ) && !_isnan( m_start.z ) );
#else
	if( _isnan( m_start.x ) || _isnan( m_start.y ) || _isnan( m_start.z ) )
	{
#ifndef FINAL_BUILD
		r3dOutToLog("!!!Error: %s (line %d) CrossProduct a<%0.4f, %0.4f, %0.4f> halfAngle(%0.4f)  dot(%0.4f)\n", __FILE__, __LINE__, a.x, a.y, a.z, halfAngle, dot);
#endif
		m_start.x = 0.0f;
		m_start.y = 0.0f;
		m_start.z = 0.0f;
		m_start.w = 1.0f;
	}
#endif

	dot = R3D_CLAMP( vForward.Dot( v2 ), -1.0f, 1.0f );
	halfAngle = r3dAcos( dot ) * 0.5f;
	sinHalfAngle = r3dSin( halfAngle );
	r3dVector b( ( 1.0 - fabs( dot ) < 0.0001f ) ? r3dVector( 0.0f, dot, 0.0f ) : vForward.Cross( v2 ).Normalize() );
	m_finish.x = b.x * sinHalfAngle;
	m_finish.y = b.y * sinHalfAngle;
	m_finish.z = b.z * sinHalfAngle;
	m_finish.w = r3dCos( halfAngle );
	D3DXQuaternionNormalize( &m_finish, &m_finish );

#ifdef _DEBUG
	r3d_assert( !_isnan( m_finish.x ) && !_isnan( m_finish.y ) && !_isnan( m_finish.z ) );
#else
	if( _isnan( m_finish.x ) || _isnan( m_finish.y ) || _isnan( m_finish.z ) )
	{
#ifndef FINAL_BUILD
		r3dOutToLog("!!!Error: %s (line %d) CrossProduct b<%0.4f, %0.4f, %0.4f> halfAngle(%0.4f) dot(%0.4f)\n", __FILE__, __LINE__, b.x, b.y, b.z, halfAngle, dot);
#endif
		m_finish.x = 0.0f;
		m_finish.y = 0.0f;
		m_finish.z = 0.0f;
		m_finish.w = 1.0f;
	}
#endif

	// Save the finish angle for network transmission.
	m_finishAngle = R3D_RAD2DEG( atan2f(-v2.z, v2.x) ) - 90.0f;
}

void r3dSlerpInterpolator::Update()
{
	if( m_isPaused || m_isFinished )
		return;

	m_elapsedTime += r3dGetFrameTime();
	if( m_elapsedTime >= m_timeToFinish )
	{
		// Time's up.
		m_elapsedTime = m_timeToFinish;
		m_isFinished = true;
		return;
	}
}

void r3dSlerpInterpolator::Pause()
{
	m_isPaused = true;
}

void r3dSlerpInterpolator::Resume()
{
	m_isPaused = false;
}

const r3dVector r3dSlerpInterpolator::GetCurrent() const
{
	// Interpolate
	r3dQuat out;
	D3DXQuaternionSlerp( &out, &m_start, &m_finish, m_elapsedTime / m_timeToFinish );

	// Rotate the Forward Vector to the Out Rotation.
	D3DXVECTOR3 outVec;
	D3DXVECTOR3 fwd( 0, 0, 1 );
	D3DXMATRIX rotMatrix;
	D3DXMatrixIdentity( &rotMatrix );
	D3DXMatrixRotationQuaternion( &rotMatrix, &out );
	D3DXVec3TransformNormal( &outVec, &fwd, &rotMatrix );
	return r3dVector( outVec.x, outVec.y, outVec.z );
}

const float& r3dSlerpInterpolator::GetFinishAngle() const
{
	return m_finishAngle;
}

bool r3dSlerpInterpolator::IsPaused() const
{
	return m_isPaused;
}

bool r3dSlerpInterpolator::IsFinished() const
{
	return m_isFinished;
}
