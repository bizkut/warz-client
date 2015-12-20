#include "r3dPCH.h"

#include "r3d.h"

#include "GameObjects/ObjManag.h"

#include "HUDCameraEffects.h"

HUDCameraEffects::RotationState::RotationState()
: Radius( 0.f )
, AngleY( 0.f )
, AngleX( 0.f )
, AngleSpeed( 0.f )
, RotationOn( 0 )
, Strength( 0.f )
{
	Offset.x = 0.f ;
	Offset.y = 0.f ;
	Offset.z = 0.f ;
}

//------------------------------------------------------------------------

HUDCameraEffects::HUDCameraEffects()
: m_LastTime( 0.f )
, m_LastFrame( -1 )
{

}

//------------------------------------------------------------------------

HUDCameraEffects::~HUDCameraEffects()
{

}

//------------------------------------------------------------------------

void
HUDCameraEffects::SyncParams( float dt )
{
	m_RotationState.Offset.x = R3D_LERP( m_RotationState.Offset.x, g_char_spect_x->GetFloat(), dt ) ;
	m_RotationState.Offset.y = R3D_LERP( m_RotationState.Offset.y, g_char_spect_y->GetFloat(), dt ) ;
	m_RotationState.Offset.z = R3D_LERP( m_RotationState.Offset.z, g_char_spect_z->GetFloat(), dt ) ;

	m_RotationState.Radius	= R3D_LERP( m_RotationState.Radius, g_char_spect_radius->GetFloat(), dt ) ;

	m_RotationState.AngleX		= R3D_LERP( m_RotationState.AngleX, g_char_spect_angle_x->GetFloat(), dt ) ;
	m_RotationState.AngleSpeed	= R3D_LERP( m_RotationState.AngleSpeed, g_char_spect_angle_vel->GetFloat(), dt ) ;
}

//------------------------------------------------------------------------

void
HUDCameraEffects::StartRotation()
{
	m_RotationState.Offset.x = g_char_spect_x->GetFloat() ;
	m_RotationState.Offset.y = g_char_spect_y->GetFloat() ;
	m_RotationState.Offset.z = g_char_spect_z->GetFloat() ;

	m_RotationState.Radius = g_char_spect_radius->GetFloat() ;

	m_RotationState.AngleY		= g_char_spect_angle_y->GetFloat() ;
	m_RotationState.AngleX		= g_char_spect_angle_x->GetFloat() ;
	m_RotationState.AngleSpeed	= g_char_spect_angle_vel->GetFloat() ;

	m_RotationState.RotationOn	= 1 ;
	m_RotationState.Strength = 0.f ;
}

//------------------------------------------------------------------------

void
HUDCameraEffects::StopRotation()
{
	m_RotationState.RotationOn = 0 ;
}

//------------------------------------------------------------------------

void
HUDCameraEffects::UpdateRotation( r3dCamera* oCam, const r3dPoint3D& camPos, float dt )
{
	m_RotationState.Strength += dt ;
	m_RotationState.Strength = R3D_MIN( m_RotationState.Strength, 1.0f ) ;

	m_RotationState.AngleY += m_RotationState.AngleSpeed * dt ;

	r3dPoint3D target = camPos + m_RotationState.Offset ;

	r3dPoint3D cam ;

	cam = target ;

	r3dPoint3D ray ;

	float ax = m_RotationState.AngleX * float( M_PI ) / 180.f ;
	float ay = m_RotationState.AngleY * float( M_PI ) / 180.f ;

	ray.x = m_RotationState.Radius * sinf( ay ) * cosf( ax ) ;
	ray.z = m_RotationState.Radius * cosf( ay ) * cosf( ax ) ;
	ray.y = m_RotationState.Radius * sinf( ax ) ;

	cam += ray ;

	*static_cast<r3dPoint3D*>( oCam ) = R3D_LERP( *static_cast<r3dPoint3D*>( oCam ), cam, m_RotationState.Strength ) ;

	r3dPoint3D pointTo = R3D_LERP( oCam->vPointTo, -ray, m_RotationState.Strength ) ;

	oCam->PointTo( target + pointTo ) ;
}

//------------------------------------------------------------------------

void
HUDCameraEffects::Update( r3dCamera* oCam, const r3dPoint3D& camPos )
{
	int curFrameId = GameWorld().GetFrameId() ;

	if( m_LastFrame != curFrameId - 1 
			&&
		m_LastFrame != curFrameId 
			)
	{
		m_LastTime = r3dGetTime() ;
	}

	m_LastFrame = curFrameId ;

	float newTime = r3dGetTime() ;

	float dt = newTime - m_LastTime ;
	m_LastTime = newTime ;

	if( g_char_spect->GetInt() && !m_RotationState.RotationOn )
	{
		StartRotation() ;
	}

	if( !g_char_spect->GetInt() && m_RotationState.RotationOn )
	{
		StopRotation() ;
	}

	SyncParams( dt ) ;

	if( m_RotationState.RotationOn )
	{
		UpdateRotation( oCam, camPos, dt ) ;
	}


}

//------------------------------------------------------------------------

HUDCameraEffects * /*extern*/ g_pHUDCameraEffects ;