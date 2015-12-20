#include	"r3dPCH.h"
#include	"r3d.h"

#define DEFAULT_FOV	103

//-----------------------------------------------------------------------
r3dCamera::r3dCamera(const r3dPoint3D& pos)
//-----------------------------------------------------------------------
{
  _Init(pos);
  vUP = r3dVector( 0, 1, 0 );
}

//-----------------------------------------------------------------------
r3dCamera::r3dCamera()
//-----------------------------------------------------------------------
{
  _Init(r3dPoint3D(0, 0, 0));
  vUP = r3dVector( 0, 1, 0 );
}

//-----------------------------------------------------------------------
void r3dCamera::_Init(const r3dPoint3D& pos)
//-----------------------------------------------------------------------
{
	Assign(pos);

	FOV = DEFAULT_FOV;

	vPointTo.Assign(0,0,1);

	NearClip 	= 0.1f;
	FarClip 	= 14000;

	ProjectionType = PROJTYPE_PRESPECTIVE;
	Width = 0;
	Height = 0;

	extern float __r3dGlobalAspect;
	Aspect = __r3dGlobalAspect;
}


//-----------------------------------------------------------------------
void r3dCamera::PointTo(const r3dPoint3D& p)
//-----------------------------------------------------------------------
{
  vPointTo = p - *this;
  vPointTo.Normalize();
}

//--------------------------------------------------------------------------------------------------------
void r3dCamera::SetOrtho( float fWidth, float fHeight )
{
	ProjectionType = PROJTYPE_ORTHO;
	Width = fWidth;
	Height = fHeight;
}

//-------------------------------------------------------------------------
//	Camera accelerometer class
//-------------------------------------------------------------------------

r3dCameraAccelerometer::r3dCameraAccelerometer()
: prevUpdateTime(0)
, prevLSpeed(0)
, prevASpeed(0)
, maxLAcc(0)
, maxAAcc(0)
{
	D3DXMatrixIdentity(&prevViewMat);
	D3DXMatrixIdentity(&invPrevViewMat);
	ZeroMemory(linearAccelerationHistory, sizeof(linearAccelerationHistory));
	ZeroMemory(angularAccelerationHistory, sizeof(angularAccelerationHistory));
}

//////////////////////////////////////////////////////////////////////////

void r3dCameraAccelerometer::Update(const D3DXMATRIXA16 &newViewMat, const D3DXMATRIXA16 &invNewViewMat)
{
	float currT = r3dGetTime();
	float dt = currT - prevUpdateTime;

	//	Advance history (fifo cache)
	for (int i = ACCELERATION_HISTORY_LEN - 1; i > 0; --i)
	{
		linearAccelerationHistory[i] = linearAccelerationHistory[i - 1];
		angularAccelerationHistory[i] = angularAccelerationHistory[i - 1];
	}

	//	Linear velocity
	r3dVector prevPos(invPrevViewMat._41, invPrevViewMat._42, invPrevViewMat._43);
	r3dVector newPos(invNewViewMat._41, invNewViewMat._42, invNewViewMat._43);

	float dist = (newPos - prevPos).Length();
	float lSpeed = dist / dt;
	linearAccelerationHistory[0] = lSpeed - prevLSpeed;
	prevLSpeed = lSpeed;

	//	Angular acceleration
	D3DXVECTOR3 fwd(0, 0, 1), prevFwd, newFwd;
	D3DXVec3TransformNormal(&prevFwd, &fwd, &invPrevViewMat);
	D3DXVec3TransformNormal(&newFwd, &fwd, &invNewViewMat);

	float angle = r3dTL::Clamp(D3DXVec3Dot(&prevFwd, &newFwd), -1.0f, 1.0f);
	angle = acosf(angle);
	float aSpeed = angle / dt;
	angularAccelerationHistory[0] = aSpeed - prevASpeed;
	prevASpeed = aSpeed;

	prevUpdateTime = currT;
	prevViewMat = newViewMat;
	invPrevViewMat = invNewViewMat;

	//	Recalc max
	maxAAcc = angularAccelerationHistory[0];
	maxLAcc = linearAccelerationHistory[0];
	for (int i = 1; i < ACCELERATION_HISTORY_LEN; ++i)
	{
		maxAAcc = R3D_MAX(maxAAcc, angularAccelerationHistory[i]);
		maxLAcc = R3D_MAX(maxLAcc, linearAccelerationHistory[i]);
	}
}
