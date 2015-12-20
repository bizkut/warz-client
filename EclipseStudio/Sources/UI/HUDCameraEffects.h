#pragma once

class HUDCameraEffects
{
public:
	struct RotationState
	{
		r3dPoint3D Offset ;

		float Radius ;

		float AngleY ;
		float AngleX ;
		float AngleSpeed ;

		int RotationOn ;
		float Strength ;

		RotationState() ;
	};

public:
	HUDCameraEffects() ;
	~HUDCameraEffects() ;

public:
	void Update( r3dCamera* oCam, const r3dPoint3D& camPos ) ;

	void SyncParams( float dt ) ;
	
	void StartRotation() ;
	void StopRotation() ;

	void UpdateRotation( r3dCamera* oCam, const r3dPoint3D& camPos, float dt ) ;

private:
	RotationState	m_RotationState ;
	float			m_LastTime ;
	int				m_LastFrame ;

} extern * g_pHUDCameraEffects ;