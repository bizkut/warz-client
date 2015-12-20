#ifndef	__R3D_CAMERA_H
#define	__R3D_CAMERA_H

// FocalDistance value - default is 256 ( 64 degrees FOV )
//

//-----------------------------------------------------------------------
// Class camera
//-----------------------------------------------------------------------
class r3dCamera : public r3dPoint3D
{
public:

	enum ProjectionTypeE
	{
		PROJTYPE_PRESPECTIVE,
		PROJTYPE_ORTHO,
		PROJTYPE_CUSTOM
	};

	float			NearClip;
	float			FarClip;

	r3dVector		vPointTo;
	r3dVector		vUP;

	// perspective projection matrix 
	float			FOV;

	// orthographic projection matrix
	ProjectionTypeE	ProjectionType;
	float			Width;
	float			Height;
	float			Aspect;

  private:
	void		_Init(const r3dPoint3D& pos);

  public:	
				r3dCamera(const r3dPoint3D& pos);
				r3dCamera();

	void		SetOrtho	( float fWidth, float fHeight );
	inline void	SetPosition	(const r3dPoint3D& Pos);
	void		PointTo		(const r3dPoint3D& p);
	void		SetPlanes	(float nc, float fc) { NearClip = nc; FarClip = fc;}
};

/////////////////////////////////////////////////////////////////////////
// Inlines
/////////////////////////////////////////////////////////////////////////

inline	void r3dCamera::SetPosition(const r3dPoint3D& Pos)
{
  Assign(Pos);
}

//-------------------------------------------------------------------------
//	Camera accelerometer class
//-------------------------------------------------------------------------
class r3dCameraAccelerometer
{
	D3DXMATRIXA16 prevViewMat;
	D3DXMATRIXA16 invPrevViewMat;
	float prevUpdateTime;
	float prevLSpeed;
	float prevASpeed;

	#define ACCELERATION_HISTORY_LEN 8
	float linearAccelerationHistory[ACCELERATION_HISTORY_LEN];
	float angularAccelerationHistory[ACCELERATION_HISTORY_LEN];
	float maxLAcc;
	float maxAAcc;

public:
	r3dCameraAccelerometer();
	void Update(const D3DXMATRIXA16 &newViewMat, const D3DXMATRIXA16 &invNewViewMat);
	float GetMaxLinearAcceleration() const { return maxLAcc; }
	float GetMaxAngularAcceleration() const { return maxAAcc; }
};

#endif //__R3D_CAMERA_H
