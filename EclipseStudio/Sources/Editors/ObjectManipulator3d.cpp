#include "r3dPCH.h"
#include "r3d.h"
#include "../ObjectsCode/WORLD/obj_Prefab.h"

r3dString gPickedMostDerivedClass ;

void MatrixGetYawPitchRoll ( const D3DXMATRIX & mat, float & fYaw, float & fPitch, float & fRoll )
{
	// rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
	//        cx*sz           cx*cz          -sx
	//       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy
	if ( mat ( 2, 1 )  < 1.0f )
	{
		if ( mat ( 2, 1 )  > -1.0f )
		{
			fYaw = ( float ) atan2 ( mat ( 2, 0 ) , mat ( 2, 2 ) );
			fPitch =  ( float ) asin ( -mat ( 2, 1 ) );
			fRoll = ( float ) atan2 ( mat ( 0, 1 ) , mat ( 1, 1 ) );
		}
		else
		{
			// WARNING.  Not unique.  YA - ZA = ( float ) atan2 ( r01, r00)
			fYaw = ( float ) atan2 ( mat ( 1, 0 ) , mat ( 0, 0 ) );
			fPitch = R3D_PI / 2.0f;
			fRoll = 0.0f;
		}
	}
	else
	{
		// WARNING.  Not unique.  YA + ZA = ( float ) atan2 ( -r01, r00)
		fYaw = ( float ) atan2 ( -mat ( 1, 0 ) , mat ( 0, 0 ) );
		fPitch = -R3D_PI / 2.0f;
		fRoll = 0.0f;
	}
}

void NormalizeYPR ( r3dVector & ypr )
{
	while ( ypr.x < 0.0f )
		ypr.x += 2.0f*R3D_PI;
	while ( ypr.x > 2.0f*R3D_PI )
		ypr.x -= 2.0f*R3D_PI;

	while ( ypr.y < 0.0f )
		ypr.y += 2.0f*R3D_PI;
	while ( ypr.y > 2.0f*R3D_PI )
		ypr.y -= 2.0f*R3D_PI;

	while ( ypr.z < 0.0f )
		ypr.z += 2.0f*R3D_PI;
	while ( ypr.z > 2.0f*R3D_PI )
		ypr.z -= 2.0f*R3D_PI;
}

#ifndef FINAL_BUILD

#include "ui/UIimEdit2.h"
#include "gamecommon.h"
#include "../ObjectsCode/WORLD/obj_Group.h"

#include "LevelEditor.h"

#include "ObjectManipulator3d.h"

#include "../ObjectsCode/WORLD/obj_LightMesh.h"

//////////////////////////////////////////////////////////////////////////

r3dColor		/*ObjectManipulator3d::*/m_uColorElementSelected	= r3dColor24 ( 255, 255, 0 );
r3dColor		/*ObjectManipulator3d::*/m_uColorElementX			= r3dColor24 ( 255, 0, 0 );
r3dColor		/*ObjectManipulator3d::*/m_uColorElementY			= r3dColor24 ( 0, 255, 0 );
r3dColor		/*ObjectManipulator3d::*/m_uColorElementZ			= r3dColor24 ( 0, 0, 255 );
r3dColor		/*ObjectManipulator3d::*/m_uColorElementXYZ			= r3dColor ( 100, 100, 100, 100 );
r3dColor		/*ObjectManipulator3d::*/m_uColorElementXY			= r3dColor ( 255, 0, 255, 100 );
r3dColor		/*ObjectManipulator3d::*/m_uColorElementYZ			= r3dColor ( 255, 255, 0, 100 );
r3dColor		/*ObjectManipulator3d::*/m_uColorElementXZ			= r3dColor ( 0, 255, 255, 100 );
r3dColor		/*ObjectManipulator3d::*/m_uColorElementXZ_Snap		= r3dColor ( 0, 255, 255, 100 );
r3dColor		/*ObjectManipulator3d::*/m_uGridColor				= r3dColor ( 255, 0, 255 );
r3dColor		/*ObjectManipulator3d::*/m_uGridColorSelected		= r3dColor ( 255, 255, 0, 100 );
r3dColor		/*ObjectManipulator3d::*/m_uGridColorSelectedAddition= r3dColor ( 0, 255, 255, 255 );

float			/*ObjectManipulator3d::*/m_fAxisLen					= 7.0f * 0.013f;
float			/*ObjectManipulator3d::*/m_fAxisBoldness			= 10.0f;
float			/*ObjectManipulator3d::*/m_fRotateCircleBoldness	= 0.017f;
float			/*ObjectManipulator3d::*/m_fResizeSphereRadius		= 0.1f;
float			/*ObjectManipulator3d::*/m_fRotateCircleRadius		= 0.5f;
float			/*ObjectManipulator3d::*/m_fRotateCircleStepsOffset	= 0.8f;

int				/*ObjectManipulator3d::*/m_fSelectionRectTreshold	= 20;

float			/*ObjectManipulator3d::*/m_fAngleMult				= 5.0f;

typedef bool (*Win32MsgProc_fn)(UINT uMsg, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////

/// Get 3D world ray from 2D mouse click coords.
r3dRay Picker_GetClickRay ( POINT ptCursor )
{
	float fWndWidth = r3dRenderer->ScreenW;
	float fWndHeight = r3dRenderer->ScreenH;

	//Application_c::Get().WindowSize( iWndWidth, iWndHeight );

	D3DXMATRIX mView = r3dRenderer->ViewMatrix;
	D3DXMATRIX mInvView = r3dRenderer->InvViewMatrix;
	D3DXMATRIX mProj = r3dRenderer->ProjMatrix;

	// Compute the vector of the pick ray in screen space
	r3dVector v;
	v.x =  ( ( ( 2.0f * ptCursor.x ) / fWndWidth  ) - 1.f ) / mProj._11;
	v.y = -( ( ( 2.0f * ptCursor.y ) / fWndHeight ) - 1.f ) / mProj._22;
	v.z =  1.0f;

	// Transform the screen space pick ray into 3D space
	r3dVector vPickRayDir;
	r3dVector vPickRayOrig;

	D3DXVec3TransformNormal ( vPickRayDir.d3dx(), v.d3dx(), &mInvView );
	vPickRayOrig.x = mInvView._41;
	vPickRayOrig.y = mInvView._42;
	vPickRayOrig.z = mInvView._43;

	return r3dRay ( vPickRayOrig, vPickRayDir );
}

static r3dPoint2D ConvertPt ( POINT const & pt )
{
	float fWndWidth = r3dRenderer->ScreenW;
	float fWndHeight = r3dRenderer->ScreenH;

	return r3dPoint2D ( 
		float ( pt.x ) / fWndWidth
		, float ( pt.y ) / fWndHeight 
		);
}

static POINT ConvertPt ( r3dPoint2D const & v )
{
	float fWndWidth = r3dRenderer->ScreenW;
	float fWndHeight = r3dRenderer->ScreenH;

	POINT pt;
	pt.x = long ( v.x * fWndWidth );
	pt.y = long ( v.y * fWndHeight );

	return pt;
}

static r3dRect ConvertRect ( RECT const & rect )
{
	float fWndWidth = r3dRenderer->ScreenW;
	float fWndHeight = r3dRenderer->ScreenH;

	r3dRect tRect;

	tRect.org.x	 = float(rect.left)					/ fWndWidth;
	tRect.size.x = float(rect.right - rect.left)	/ fWndWidth;
	tRect.org.y	 = float(rect.top)					/ fWndHeight; 
	tRect.size.y = float(rect.bottom - rect.top)	/ fWndHeight; 

	return tRect;
}

static RECT ConvertRect ( r3dRect const & fRect )
{
	float fWndWidth = r3dRenderer->ScreenW;
	float fWndHeight = r3dRenderer->ScreenH;

	RECT rect;
	rect.left	=	( long ) ( fRect.org.x					* ( fWndWidth  ) );
	rect.right	=	( long ) ( (fRect.org.x + fRect.size.x)	* ( fWndWidth  ) );
	rect.top	=	( long ) ( fRect.org.y					* ( fWndHeight ) );
	rect.bottom	=	( long ) ( (fRect.org.y + fRect.size.y)	* ( fWndHeight ) );
	return rect;
}

static float GetScreenSize ( GameObject * pObj, r3dVector vVec, r3dVector vOffset )
{
	r3d_assert ( pObj );

	D3DXMATRIX mView = r3dRenderer->ViewMatrix;
	D3DXMATRIX mProj = r3dRenderer->ProjMatrix;

	r3dPoint3D vPos = g_Manipulator3d.GetSelectionCenter ();

	D3DXMATRIX  mTrans;
	D3DXMatrixTranslation ( &mTrans, vPos.x, vPos.y, vPos.z );

	D3DXMATRIX mFinal = mTrans * mView;
	mFinal._41 = 0.0f;
	mFinal._42 = 0.0f;

	mFinal = mFinal * mProj;

	D3DXMATRIX mFinalInv;
	D3DXMatrixInverse ( &mFinalInv, NULL, &mFinal );

	D3DXVECTOR3 vRes1, vRes2, vOffsetObject;

	if( r3dRenderer->ZDir == r3dRenderLayer::ZDIR_INVERSED )
	{
		vOffset.z = 1.0f - vOffset.z;
	}

	D3DXVec3TransformCoord ( &vRes1, vVec.d3dx(), &mFinal );
	D3DXVec3TransformCoord ( &vOffsetObject, vOffset.d3dx(), &mFinalInv );
	vOffsetObject = *vVec.d3dx() - vOffsetObject;
	D3DXVec3TransformCoord ( &vRes2, &vOffsetObject, &mFinal );

	r3dPoint2D v1 ( vRes1.x, vRes1.y );
	r3dPoint2D v2 ( vRes2.x, vRes2.y );

	return fabs ( v2.x - v1.x );
}

static void GetScaleMatrix ( GameObject * pObj, D3DXMATRIX & mScale )
{
	r3d_assert ( pObj );

	float fScale = GetScreenSize ( pObj, r3dVector ( 0.0f, 0.0f, 0.0f ), r3dVector ( 1.0f, 0.0f, 0.0f ) );
	float fNear = gCam.NearClip;

	D3DXMatrixScaling ( &mScale, fNear * m_fAxisLen / fScale, fNear * m_fAxisLen / fScale, fNear * m_fAxisLen / fScale );
}


static bool GetDistancePointToCircle ( const r3dPoint2D & vPoint, const r3dCylinder & tCircle, const D3DXMATRIX & mTransform, float fDist )
{
	D3DXMATRIX mTrans;
	D3DXMatrixTranslation ( &mTrans, tCircle.vBase.x, tCircle.vBase.y, tCircle.vBase.z );

	D3DXMATRIX mRot;
	D3DXMatrixIdentity ( &mRot );

	D3DXVECTOR3 vNormal;
	D3DXVec3Normalize ( &vNormal, tCircle.vAltitude.d3dx() );

	D3DXVECTOR3 v3 = vNormal;
	D3DXVECTOR3 v2 = D3DXVECTOR3 ( vNormal.z,0, -vNormal.x );
	D3DXVECTOR3 v1;

	if ( vNormal.z * vNormal.z + vNormal.x * vNormal.x < FLT_EPSILON )
	{
		v2.x = 1.0f;
		v2.y = 0.0f;
		v2.z = 0.0f;
	}

	D3DXVec3Cross ( &v1, &v2, &v3 );

	D3DXVec3Normalize ( &v1, &v1 );
	D3DXVec3Normalize ( &v2, &v2 );
	D3DXVec3Normalize ( &v3, &v3 );

	mRot ( 0, 0 ) = v1.x;
	mRot ( 0, 1 ) = v1.y;
	mRot ( 0, 2 ) = v1.z;

	mRot ( 1, 0 ) = v2.x;
	mRot ( 1, 1 ) = v2.y;
	mRot ( 1, 2 ) = v2.z;

	mRot ( 2, 0 ) = v3.x;
	mRot ( 2, 1 ) = v3.y;
	mRot ( 2, 2 ) = v3.z;

	mRot *= mTrans;

	DWORD dwNumSegments = 50;
	float fDeltaSegAngle  = ( 2.0f * R3D_PI / dwNumSegments );

	// Generate the group of segments for the current ring
	for( DWORD seg = 0; seg < dwNumSegments + 1; seg++ )
	{
		float x = tCircle.fRadius * cosf( seg * fDeltaSegAngle );
		float y = tCircle.fRadius * sinf( seg * fDeltaSegAngle );

		D3DXVECTOR3 vPoint3D;
		D3DXVECTOR3 vOrign = D3DXVECTOR3 ( x, y, 0.0f );
		D3DXMATRIX mRes1 = mRot * mTransform;
		D3DXVec3TransformCoord ( &vPoint3D, &vOrign, &mRes1 );
		
		r3dPoint2D vRes ( vPoint3D.x, vPoint3D.y );

		if ( ( vRes - vPoint ).Length() < m_fRotateCircleBoldness )
			return true;
	}

	return false;
}

/// 2d: x,y coordinates used
static float GetDistancePointToLine ( r3dVector vO, r3dVector vA, const r3dPoint2D & vPt )
{
	const float VEC_THRESHOLD = 0.001f;
	r3dPoint2D vAToO ( vA.x - vO.x, vA.y - vO.y );
	r3dPoint2D vDir;
	if ( vAToO.Length() > VEC_THRESHOLD )
	{
		vDir = vAToO;
		vDir.Normalize();
	}
	else
		vDir = r3dPoint2D ( 0, 0 );

	// rotate direction for 90 degrees -> perpendicular
	r3dPoint2D vPerp ( -vDir.y, vDir.x );

	r3dPoint2D vAtoPt ( vA.x-vPt.x, vA.y-vPt.y );
	r3dPoint2D vPtToO ( vPt.x-vO.x, vPt.y-vO.y );

	float fDist = fabs ( vPerp.Dot( vAtoPt ));

	float fDist2 = vDir.Dot ( vPtToO );
	if ( fDist2 < 0.0f || fDist2 > vAToO.Length() )
		fDist = FLT_MAX;

	return fDist;
}


bool GeomRayPlaneIntersection ( const r3dRay & ray, r3dPlane Plane, r3dVector * pPos )
{
	r3d_assert ( pPos );

	D3DXPLANE dxPlane ( Plane.Normal().x, Plane.Normal().y, Plane.Normal().z, Plane.Distance () );

	float t = -D3DXPlaneDotCoord ( &dxPlane, ray.org.d3dx() ) / D3DXPlaneDotNormal ( &dxPlane, ray.dir.d3dx() );

	if ( t < 0 )
		return false;

	if ( pPos )
		*pPos = ray.org + t * ( ray.dir );

	return true;
}

/*
r3dVector IntersectRayPlane ( r3dRay tRay, r3dPlane tPlane )
{
	if ( fabsf ( tRay.dir.Dot ( tPlane.m_Normal ) ) < 0.00001f )
		return r3dVector ( 0, 0, 0 );
}*/

static r3dVector CalcStepVector ( r3dVector vOrign, float fStaticVal, int * iSteps = NULL )
{
	float dVal[3] = {vOrign.x, vOrign.y, vOrign.z};
	for ( int i = 0; i < 3; i++ )
	{
		int iCnt = 0;
		int iSign = (dVal[i] < 0.0f) ? -1 : 1;
		while (dVal[i]*iSign>fStaticVal)
		{
			dVal[i]-=iSign*fStaticVal; 
			iCnt++;
		}
		dVal[i] = float(iCnt * iSign) * fStaticVal;
		if ( iSteps )
			(*iSteps) += iSign * iCnt;
	}

	return r3dVector(dVal[0],dVal[1], dVal[2]);	
}

//////////////////////////////////////////////////////////////////////////

#define GET_X_LPARAM_MY(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM_MY(lp)                        ((int)(short)HIWORD(lp))

static bool r3dObjectManipulatorWinProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_MOUSEWHEEL:
		{
			if ( ( !imgui_IsCursorOver2d () && ! imgui2_IsCursorOver2d() ) || ( uMsg != WM_LBUTTONDOWN ) )
			{
				POINT pt;
				pt.x = GET_X_LPARAM_MY ( lParam );
				pt.y = GET_Y_LPARAM_MY ( lParam );
				int delta = ( short ) HIWORD(wParam);
				return g_Manipulator3d.MouseMessage ( uMsg, pt, delta );
			}
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			return g_Manipulator3d.KeyboardMessageKeyUp ( (int)wParam );
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			return g_Manipulator3d.KeyboardMessageKeyDown ( (int)wParam );
		}
		break;
	};

	return false;
}

static void ObjectRegisterEvent ( GameObject * pObject )
{
	g_Manipulator3d.ObjectRegister(pObject);
}

static void ObjectUnregisterEvent ( GameObject * pObject )
{
	// only in editor

	obj_Group::Unsubscribe ( pObject );

	g_Manipulator3d.ObjectUnregister(pObject);
}


//////////////////////////////////////////////////////////////////////////

ObjectManipulator3d::ObjectManipulator3d ()
: m_ePickState ( PICK_UNSELECTED )
, m_ePickElementSelected ( ELEMENT_NONE )
, m_ePickAxisHighlighted ( ELEMENT_NONE )
, m_bEnabled ( false )
, m_bScaleEnable ( true )
, m_bLocalEnable ( true )
, m_bLocal ( false )
, m_bClone ( false )
, m_bFreeMove( false )
, m_sTypeFilter ( "" )
, m_bAngleSnapping ( false )
, m_fAngleSnappingVal ( R3D_PI / 4.0f )
, m_bMoveSnapping ( false )
, m_fMoveSnappingVal ( 10.0f )
, m_iAngleAlignSteps ( 0 )
, m_bSelectionRectShow ( false )
, m_bLeftBtnDown ( false )
, m_bLocked ( false )
, m_bAttachMode( false )
, m_SelectCallback( 0 )
, m_DropCallback( 0 )
, m_UndoEnabled( true )
, m_CloneEnabled( true )
{

	m_tSelectionStartPt.x = 0;
	m_tSelectionStartPt.y = 0;
	m_tSelectionEndPt.x = 0;
	m_tSelectionEndPt.y = 0;
}

ObjectManipulator3d::~ObjectManipulator3d ()
{

}

void
ObjectManipulator3d::Init()
{
	m_PickPoint.x = 0;
	m_PickPoint.y = 0;

	void RegisterMsgProc (Win32MsgProc_fn);
	RegisterMsgProc ( r3dObjectManipulatorWinProc );

	GameWorld().SetAddObjectEvent(ObjectRegisterEvent);
	GameWorld().SetDeleteObjectEvent(ObjectUnregisterEvent);
}

void
ObjectManipulator3d::Close()
{
	r3d_assert ( m_dPickedObjects.Count () == NULL );
	r3d_assert ( m_dObjects.Count () == 0 );
	void UnregisterMsgProc (Win32MsgProc_fn);
	UnregisterMsgProc ( r3dObjectManipulatorWinProc );
	GameWorld().SetAddObjectEvent(NULL);
	GameWorld().SetDeleteObjectEvent(NULL);
}

void
ObjectManipulator3d::AddImmortal( GameObject* pObject )
{
	for( int i = 0, e = m_ImmortalObjects.Count(); i < e; i ++ )
	{
		r3d_assert( m_ImmortalObjects[ i ] != pObject ) ;
	}

	m_ImmortalObjects.PushBack( pObject ) ;
}

//------------------------------------------------------------------------

int
ObjectManipulator3d::IsImmortal( GameObject* pObject )
{
	for( int i = 0, e = m_ImmortalObjects.Count(); i < e; i ++ )
	{
		if( m_ImmortalObjects[ i ] == pObject )
			return 1 ;	
	}

	return 0 ;
}

//------------------------------------------------------------------------

void
ObjectManipulator3d::RemoveImmortal( GameObject* pObject )
{
	for( int i = 0, e = m_ImmortalObjects.Count(); i < e; i ++ )
	{
		if( m_ImmortalObjects[ i ] == pObject )
		{
			m_ImmortalObjects.Erase( i ) ;
			return ;
		}
	}

	r3d_assert( false ) ;
}


r3dVector ObjectManipulator3d::GetSelectionCenter () const
{
	if ( m_dPickedObjects.Count() == 0 )
		return r3dVector ();
	//else if ( m_dPickedObjects.Count() == 1 )
	//	return m_dPickedObjects[0].pObject->GetPosition();
	else
	{
		r3dPoint3D vCenter (0,0,0);
		for ( uint32_t i = 0; i < m_dPickedObjects.Count (); i++ )
		{
			vCenter += m_dPickedObjects[i].pObject->GetPosition();
		}

		vCenter /= (float)m_dPickedObjects.Count ();

		return vCenter;
	}
} 

r3dColor ObjectManipulator3d::GetElementColor ( ObjectManipulator3d::ControlElement_e eElement ) const
{
	switch ( eElement )
	{
	case ELEMENT_X_MOVE:
	case ELEMENT_YZ_ROTATE:
	case ELEMENT_X_RESIZE:
		return m_uColorElementX;

	case ELEMENT_Y_MOVE:
	case ELEMENT_ZX_ROTATE:
	case ELEMENT_Y_RESIZE:
		return m_uColorElementY;

	case ELEMENT_Z_MOVE:
	case ELEMENT_XY_ROTATE:
	case ELEMENT_Z_RESIZE:
		return m_uColorElementZ;
	case ELEMENT_XYZ_RESIZE:
		return m_uColorElementXYZ;
	case ELEMENT_XZ_SNAP_MOVE:
		return m_uColorElementXZ_Snap;
	case ELEMENT_XZ_MOVE:
		return m_uColorElementXZ;
	case ELEMENT_XY_MOVE:
		return m_uColorElementXY;
	case ELEMENT_YZ_MOVE:
		return m_uColorElementYZ;
	}

	return r3dColor::grey;
}

r3dVector ObjectManipulator3d::GetAxis ( int iAxis ) const
{
	switch ( iAxis )
	{
	case 0:
		return r3dVector ( 1.0f, 0.0f, 0.0f );
	case 1:
		return r3dVector ( 0.0f, 1.0f, 0.0f );
	default:
		return r3dVector ( 0.0f, 0.0f, 1.0f );
	}

	r3d_assert ( 0 && "unknown axis" );

	return r3dVector ( 1.0f, 0.0f, 0.0f );
}


bool ObjectManipulator3d::GetCylinderFor ( r3dCylinder & tCylinder, ControlElement_e eElement ) const
{
	tCylinder.fRadius = m_fRotateCircleRadius;
	tCylinder.vBase = r3dVector(0,0,0);

	switch ( eElement )
	{
	case ELEMENT_XY_ROTATE:
		tCylinder.vAltitude = r3dVector ( 0.0f, 0.0f, 0.001f );
		break;
	case ELEMENT_YZ_ROTATE:
		tCylinder.vAltitude = r3dVector ( 0.001f, 0.0f, 0.0f );
		break;
	case ELEMENT_ZX_ROTATE:
		tCylinder.vAltitude = r3dVector ( 0.0f, 0.001f, 0.0f );
		break;
	default:
		return false;
	}

	return true;
}

extern bool g_bLockPoint;
extern bool g_bFocusPoint;

ObjectManipulator3d::ControlElement_e ObjectManipulator3d::Picker_PickElement ( GameObject * pObj, POINT ptCursor, bool bLocal )
{	
	if (  m_bAttachMode && g_bFocusPoint ) 
	{
		return ELEMENT_XZ_SNAP_MOVE;
	}

	float fWndWidth = r3dRenderer->ScreenW;
	float fWndHeight = r3dRenderer->ScreenH;

	D3DXMATRIX mView = r3dRenderer->ViewMatrix;
	D3DXMATRIX mProj = r3dRenderer->ProjMatrix;

	// Compute the vector of the pick in screen space
	r3dPoint2D vPt;
	vPt = ConvertPt( ptCursor );
	vPt.x =  2.0f * vPt.x - 1.0f;
	vPt.y = -( 2.0f * vPt.y - 1.0f );

	r3dVector vPos = GetSelectionCenter ();

	D3DXMATRIX  mTrans;
	D3DXMatrixTranslation ( &mTrans, vPos.x, vPos.y, vPos.z );

	D3DXMATRIX mScale;
	GetScaleMatrix ( pObj, mScale );

	D3DXMATRIX mRot;
	if ( bLocal )
	{
		mRot = pObj->GetRotationMatrix();
	}
	else
		D3DXMatrixIdentity ( &mRot );

	D3DXMATRIX  mCamera = mScale * mRot * mTrans * mView;
	D3DXMATRIX  mRes = mCamera * mProj;

	int iAxis = ELEMENT_NONE;
	float fMinDist = FLT_MAX;

	for ( int i = ELEMENT_X_MOVE; i <= ELEMENT_Z_MOVE; ++i )
	{
		r3dVector vAxis = GetAxis ( i - ELEMENT_X_MOVE );
		r3dVector vO = vAxis * m_fRotateCircleRadius;
		r3dVector vA = vAxis;

		r3dVector vCameraO;
		r3dVector vCameraA;
		D3DXVec3TransformCoord ( vCameraO.d3dx(), vO.d3dx(), &mCamera );
		D3DXVec3TransformCoord ( vCameraA.d3dx(), vA.d3dx(), &mCamera );

		r3dVector vPO;
		r3dVector vPA;
		D3DXVec3TransformCoord ( vPO.d3dx(), vO.d3dx(), &mRes );
		D3DXVec3TransformCoord ( vPA.d3dx(), vA.d3dx(), &mRes );

		if ( vCameraO.z < 0.0f )
			vPO = -vPO;

		if ( vCameraA.z < 0.0f )
			vPA = -vPA;

		float fDist = GetDistancePointToLine ( vPO, vPA, vPt );
		if ( fDist < fMinDist && fDist > 0.0f )
		{
			fMinDist = fDist;
			iAxis = i;
		}
	}

	if ( fMinDist > 1.0f / fWndHeight * m_fAxisBoldness )
		iAxis = ELEMENT_NONE;

	if ( iAxis == ELEMENT_NONE && !bLocal )
	{
		D3DXMATRIX mResRot = mScale * mRot * mTrans;
		r3dVector vSphere = r3dVector(0,0,0);
		D3DXVec3TransformCoord ( vSphere.d3dx(), vSphere.d3dx(), &mResRot );

		r3dRay tRay = Picker_GetClickRay ( ptCursor );
		float fNumerator	= tRay.dir.Dot ( vSphere - tRay.org );
		float fDenominator	= tRay.dir.Dot ( tRay.dir );

		r3dVector vClosest = ( tRay.org + ( fNumerator / fDenominator ) * tRay.dir );
		float fDist = ( vClosest - vSphere ).Length();

		if ( m_bAttachMode && g_bLockPoint )
		{
			iAxis = ELEMENT_XZ_SNAP_MOVE;
		}
		else
		{
			if ( fDist <= m_fResizeSphereRadius * mScale._11 )
				iAxis = ELEMENT_XZ_SNAP_MOVE;
		}
	}

	if ( iAxis == ELEMENT_NONE && bLocal && m_bScaleEnable )
	{
		//D3DXMATRIX mRes = mScale * mTrans;
		D3DXMATRIX mResRot = mScale * mRot * mTrans;
		for ( int i = 0; i <= ELEMENT_XYZ_RESIZE - ELEMENT_X_RESIZE && iAxis == ELEMENT_NONE; ++i )
		{
			r3dVector vSphere = r3dVector(0,0,0);
			if ( i < 3 )
				vSphere = GetAxis ( i ) * ( 1.0f + m_fResizeSphereRadius );
			D3DXVec3TransformCoord ( vSphere.d3dx(), vSphere.d3dx(), &mResRot );

			r3dRay tRay = Picker_GetClickRay ( ptCursor );
			float fNumerator	= tRay.dir.Dot ( vSphere - tRay.org );
			float fDenominator	= tRay.dir.Dot ( tRay.dir );

			r3dVector vClosest = ( tRay.org + ( fNumerator / fDenominator ) * tRay.dir );
			float fDist = ( vClosest - vSphere ).Length();

			if ( fDist <= m_fResizeSphereRadius * mScale._11 )
				iAxis = ELEMENT_X_RESIZE + i;
		}
	}

	if ( iAxis == ELEMENT_NONE )
	{
		float fParamKrit = -1.0f;
		for ( int i = ELEMENT_YZ_MOVE; i <= ELEMENT_XY_MOVE; ++i )
		{
			#define B_NUMDIM 3
			int HitBoundingBox(float minB[B_NUMDIM], float maxB[B_NUMDIM], float origin[B_NUMDIM], float dir[B_NUMDIM], float coord[B_NUMDIM]);

			r3dVector vAxis1 = m_fRotateCircleRadius * GetAxis ( i == ELEMENT_YZ_MOVE ? ELEMENT_Y_MOVE : ELEMENT_X_MOVE );
			r3dVector vAxis2 = m_fRotateCircleRadius * GetAxis ( i == ELEMENT_XY_MOVE ? ELEMENT_Y_MOVE : ELEMENT_Z_MOVE );
			r3dVector vCenter ( 0,0,0 );

			D3DXMATRIX mResRot = mScale * mRot * mTrans;
			D3DXMATRIX mResRotInv;
			D3DXMatrixInverse ( &mResRotInv, NULL, &mResRot );
			r3dRay tRay = Picker_GetClickRay ( ptCursor );
			D3DXVec3TransformCoord ( tRay.org.d3dx(), tRay.org.d3dx(), &mResRotInv );
			D3DXVec3TransformNormal ( tRay.dir.d3dx(), tRay.dir.d3dx(), &mResRotInv );

			r3dVector minB (0,0,0);
			r3dVector maxB = vAxis1 + vAxis2;
			float coord [B_NUMDIM];

			if ( HitBoundingBox ( (float*)&minB,  (float*)&maxB, (float*)&tRay.org, (float*)&tRay.dir, coord ) )
			{
				r3dVector vCenterNew ( 0,0,0 );
				r3dVector vPoint1, vPoint2, vPoint3, vCenterRect;
				D3DXVec3TransformCoord ( vPoint1.d3dx(), vCenterNew.d3dx(), &mResRot );
				D3DXVec3TransformCoord ( vPoint2.d3dx(), vAxis1.d3dx(), &mResRot );
				D3DXVec3TransformCoord ( vPoint3.d3dx(), vAxis2.d3dx(), &mResRot );
				D3DXVec3TransformCoord ( vCenterRect.d3dx(), (vAxis1+vAxis2).d3dx(), &mResRot );

				D3DXMATRIX mTransfCoord = mView * mProj;
				D3DXVec3TransformCoord ( vCenterRect.d3dx (), vCenterRect.d3dx(), &mTransfCoord );

				float zRank = vCenterRect.z;

				if( r3dRenderer->ZDir == r3dRenderLayer::ZDIR_INVERSED )
				{
					zRank = 1.0f - zRank;
				}

				if ( fParamKrit >= 0.0f && fParamKrit < zRank )
					continue;

				fParamKrit = zRank;
				
				m_MovePlane.Set ( vPoint1, vPoint2, vPoint3 );

				r3dRay tRayOrign = Picker_GetClickRay ( ptCursor );
				r3dVector vPt;
				if ( GeomRayPlaneIntersection ( tRayOrign, m_MovePlane, &vPt ) )
					m_MoveByPlaneOffset = vPos - vPt;
				else
					m_MoveByPlaneOffset = r3dVector(0,0,0);

				iAxis = i;
				continue;
			}
		}
	}

	r3dCylinder tCircle;

	for ( int i = ELEMENT_XY_ROTATE; i <= ELEMENT_ZX_ROTATE && iAxis == ELEMENT_NONE; ++i )
	{
		GetCylinderFor ( tCircle, ControlElement_e ( i ) );
		if ( GetDistancePointToCircle ( vPt, tCircle, mRes, 0.07f ) )
			iAxis = i;
	}

	return ControlElement_e ( iAxis );
}

bool ObjectManipulator3d::Picker_IsPick ( GameObject * pObj, POINT ptCursor ) const
{
	r3d_assert ( pObj );

	if ( m_sTypeFilter.c_str() && m_sTypeFilter.c_str()[0] )
	{
		if ( ( pObj->Class->Name != m_sTypeFilter.c_str () ) )
			return false;
	}


	extern gobjid_t	UI_TargetObjID;
	GameObject* TargetObj = GameWorld().GetObject(UI_TargetObjID);
 
	if (TargetObj && pObj == TargetObj ) 
		return true;

	return false;
}

void ObjectManipulator3d::Picker_DrawPicked ( GameObject * pObj, ControlElement_e eElement, bool bLocal ) const
{
	r3d_assert ( pObj );
	if ( !m_bEnabled )
		return;

	r3dRenderer->Flush ();

	D3DXMATRIX mView = r3dRenderer->ViewMatrix;

	r3dVector vPos = GetSelectionCenter ();

	D3DXMATRIX tTranslate;
	D3DXMatrixTranslation ( &tTranslate, vPos.x, vPos.y, vPos.z );

	D3DXMATRIX tScale;
	GetScaleMatrix ( pObj, tScale );

	D3DXMATRIX mRot;
	if ( bLocal )
	{
		mRot = pObj->GetRotationMatrix();
	}
	else
		D3DXMatrixIdentity ( &mRot );

	D3DXMATRIX tView = tScale * mRot * tTranslate * mView;
	D3DXMATRIX tViewNoScale = mRot * tTranslate * mView;

	D3DXMATRIX tOldView;

	tOldView = r3dRenderer->ViewMatrix ;

	if ( m_bMoveSnapping && m_fMoveSnappingVal > 0.0f )
	{
		r3dRenderer->pd3ddev->SetTransform(D3DTS_VIEW, &tViewNoScale);
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

		const int GRID_DIMENSION = 100;
		if ( m_ePickElementSelected == ELEMENT_X_MOVE )
		{
			r3dDrawUniformLine3D ( r3dPoint3D( -GRID_DIMENSION*m_fMoveSnappingVal, 0, 0), r3dPoint3D( -GRID_DIMENSION*m_fMoveSnappingVal, 0, 0), gCam, m_uGridColorSelectedAddition );
			for ( int i = -GRID_DIMENSION / 2; i < GRID_DIMENSION / 2; i++ )
				r3dDrawUniformLine3D ( r3dPoint3D( i*m_fMoveSnappingVal, -GRID_DIMENSION*m_fMoveSnappingVal, 0), r3dPoint3D(i*m_fMoveSnappingVal, GRID_DIMENSION*m_fMoveSnappingVal, 0), gCam, i == 0 ? m_uGridColorSelected : m_uGridColor );
		}
		else if ( m_ePickElementSelected == ELEMENT_Y_MOVE )
		{
			r3dDrawUniformLine3D ( r3dPoint3D( 0,  -GRID_DIMENSION*m_fMoveSnappingVal, 0), r3dPoint3D( 0, GRID_DIMENSION*m_fMoveSnappingVal, 0), gCam, m_uGridColorSelectedAddition );
			for ( int i = -GRID_DIMENSION / 2; i < GRID_DIMENSION / 2; i++ )
				r3dDrawUniformLine3D ( r3dPoint3D( -GRID_DIMENSION*m_fMoveSnappingVal, i*m_fMoveSnappingVal, 0), r3dPoint3D(GRID_DIMENSION*m_fMoveSnappingVal, i*m_fMoveSnappingVal, 0), gCam, i == 0 ? m_uGridColorSelected : m_uGridColor );
		}
		else if ( m_ePickElementSelected == ELEMENT_Z_MOVE )
		{
			r3dDrawUniformLine3D ( r3dPoint3D( 0, 0, -GRID_DIMENSION*m_fMoveSnappingVal), r3dPoint3D( 0, 0, GRID_DIMENSION*m_fMoveSnappingVal), gCam, m_uGridColorSelectedAddition );
			for ( int i = -GRID_DIMENSION / 2; i < GRID_DIMENSION / 2; i++ )
				r3dDrawUniformLine3D ( r3dPoint3D( -GRID_DIMENSION*m_fMoveSnappingVal, 0, i*m_fMoveSnappingVal), r3dPoint3D(GRID_DIMENSION*m_fMoveSnappingVal, 0, i*m_fMoveSnappingVal ), gCam, i == 0 ? m_uGridColorSelected : m_uGridColor );
		}
		else if ( m_ePickElementSelected == ELEMENT_XZ_MOVE || m_ePickElementSelected == ELEMENT_XZ_SNAP_MOVE )
		{
			r3dDrawUniformLine3D ( r3dPoint3D( 0, -GRID_DIMENSION*m_fMoveSnappingVal, 0), r3dPoint3D( 0, GRID_DIMENSION*m_fMoveSnappingVal, 0), gCam, m_uGridColorSelectedAddition );
			for ( int i = -GRID_DIMENSION / 2; i < GRID_DIMENSION / 2; i++ )
			{
				r3dDrawUniformLine3D ( r3dPoint3D( -GRID_DIMENSION*m_fMoveSnappingVal, 0, i*m_fMoveSnappingVal), r3dPoint3D(GRID_DIMENSION*m_fMoveSnappingVal, 0, i*m_fMoveSnappingVal ), gCam, i == 0 ? m_uGridColorSelected : m_uGridColor );
				r3dDrawUniformLine3D ( r3dPoint3D( i*m_fMoveSnappingVal, 0, -GRID_DIMENSION*m_fMoveSnappingVal), r3dPoint3D(i*m_fMoveSnappingVal, 0, GRID_DIMENSION*m_fMoveSnappingVal), gCam, i == 0 ? m_uGridColorSelected : m_uGridColor );
			}
		}
		else if ( m_ePickElementSelected == ELEMENT_XY_MOVE )
		{
			r3dDrawUniformLine3D ( r3dPoint3D( 0, 0, -GRID_DIMENSION*m_fMoveSnappingVal), r3dPoint3D( 0, 0, GRID_DIMENSION*m_fMoveSnappingVal), gCam, m_uGridColorSelectedAddition );
			for ( int i = -GRID_DIMENSION / 2; i < GRID_DIMENSION / 2; i++ )
			{
				r3dDrawUniformLine3D ( r3dPoint3D( -GRID_DIMENSION*m_fMoveSnappingVal, i*m_fMoveSnappingVal, 0), r3dPoint3D(GRID_DIMENSION*m_fMoveSnappingVal, i*m_fMoveSnappingVal, 0 ), gCam, i == 0 ? m_uGridColorSelected : m_uGridColor );
				r3dDrawUniformLine3D ( r3dPoint3D( i*m_fMoveSnappingVal, -GRID_DIMENSION*m_fMoveSnappingVal, 0), r3dPoint3D(i*m_fMoveSnappingVal, GRID_DIMENSION*m_fMoveSnappingVal, 0), gCam, i == 0 ? m_uGridColorSelected : m_uGridColor );
			}
		}
		else if ( m_ePickElementSelected == ELEMENT_YZ_MOVE )
		{
			r3dDrawUniformLine3D ( r3dPoint3D( -GRID_DIMENSION*m_fMoveSnappingVal, 0, 0), r3dPoint3D( GRID_DIMENSION*m_fMoveSnappingVal, 0, 0), gCam, m_uGridColorSelectedAddition );
			for ( int i = -GRID_DIMENSION / 2; i < GRID_DIMENSION / 2; i++ )
			{
				r3dDrawUniformLine3D ( r3dPoint3D( 0, -GRID_DIMENSION*m_fMoveSnappingVal, i*m_fMoveSnappingVal), r3dPoint3D(0, GRID_DIMENSION*m_fMoveSnappingVal, i*m_fMoveSnappingVal ), gCam, i == 0 ? m_uGridColorSelected : m_uGridColor );
				r3dDrawUniformLine3D ( r3dPoint3D( 0, i*m_fMoveSnappingVal, -GRID_DIMENSION*m_fMoveSnappingVal), r3dPoint3D(0, i*m_fMoveSnappingVal, GRID_DIMENSION*m_fMoveSnappingVal), gCam, i == 0 ? m_uGridColorSelected : m_uGridColor );
			}
		}

		r3dRenderer->Flush ();

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );
	}

	r3dRenderer->pd3ddev->SetTransform(D3DTS_VIEW, &tView);

	if ( m_bAngleSnapping && m_fAngleSnappingVal > 0.0f )
	{
		r3dRenderer->Flush ();

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );

		int iCnt = int ( ( 360.0f / D3DXToDegree ( m_fAngleSnappingVal ) ) + 0.5f );
		const float fAngleStep = 2 * R3D_PI / iCnt;
		float fCurAngle = 0.0f;
		if ( m_ePickElementSelected >= ELEMENT_XY_ROTATE && m_ePickElementSelected <= ELEMENT_ZX_ROTATE )
		{
			int iSpepsWrapped = m_iAngleAlignSteps;
			while ( iSpepsWrapped < 0 ) iSpepsWrapped += iCnt;
			while ( iSpepsWrapped >= iCnt ) iSpepsWrapped -= iCnt;
			for ( int i = 0; i < iCnt; i++ )
			{
				float fVal1 = m_fRotateCircleRadius * cos ( fCurAngle );
				float fVal2 = m_fRotateCircleRadius * sin ( fCurAngle );		

				r3dPoint3D vCirclePt;
				bool bSelected = (i == 0) || ( i == iSpepsWrapped );
				
				if ( m_ePickElementSelected == ELEMENT_XY_ROTATE )
					vCirclePt = r3dPoint3D(fVal2,fVal1,0);
				else if ( m_ePickElementSelected == ELEMENT_YZ_ROTATE )
					vCirclePt = true ? r3dPoint3D(0,fVal1,fVal2) : r3dPoint3D(fVal2,0,fVal1);
				else if ( m_ePickElementSelected == ELEMENT_ZX_ROTATE )
					vCirclePt = true ? r3dPoint3D(fVal2,0,fVal1) : r3dPoint3D(0,fVal1,fVal2);

				r3dDrawUniformLine3D( ( bSelected ? 0.0f : m_fRotateCircleStepsOffset ) * vCirclePt, vCirclePt, gCam, bSelected ? m_uGridColorSelected : m_uGridColor );
				fCurAngle += fAngleStep;
			}
		}

		r3dRenderer->Flush ();

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );
	}
	
	for ( int i = ELEMENT_X_MOVE; i <= ELEMENT_Z_MOVE; ++i )
	{
		r3dVector vAxis = GetAxis ( i - ELEMENT_X_MOVE );
		r3dVector vPt1 = vAxis*m_fRotateCircleRadius;
		r3dVector vPt2 = vAxis;
		if ( ( m_ePickState != PICK_ELEMENT_PICKED ) || (eElement == i) )
			r3dDrawUniformLine3D(vPt1, vPt2, gCam, eElement == i ? m_uColorElementSelected : GetElementColor ( ControlElement_e ( i ) ) );
	}

	r3dCone tCone;
	tCone.fFOV = R3D_PI / 4.0f;
	for ( int i = ELEMENT_X_MOVE; i <= ELEMENT_Z_MOVE; ++i )
	{
		r3dVector vAxis = GetAxis ( i - ELEMENT_X_MOVE );

		tCone.vDir = -vAxis;
		tCone.vCenter = vAxis;

		if ( ( m_ePickState != PICK_ELEMENT_PICKED ) || (eElement == i) )
			r3dDrawConeSolid ( tCone, gCam, eElement == i ? m_uColorElementSelected : GetElementColor ( ControlElement_e ( i ) ), 0.1f );
	}

	if ( bLocal && m_bScaleEnable )
	{
		for ( int i = ELEMENT_X_RESIZE; i <= ELEMENT_Z_RESIZE; ++i )
		{
			r3dVector vCenter = GetAxis ( i - ELEMENT_X_RESIZE ) * ( 1.0f + m_fResizeSphereRadius );
			if ( ( m_ePickState != PICK_ELEMENT_PICKED ) || (eElement == i) )
				r3dDrawSphereSolid ( vCenter, m_fResizeSphereRadius, gCam, eElement == i ? m_uColorElementSelected : GetElementColor ( ControlElement_e ( i ) ) );
		}

		// ELEMENT_XYZ_RESIZE
		if ( ( m_ePickState != PICK_ELEMENT_PICKED ) || (eElement == ELEMENT_XYZ_RESIZE) )
			r3dDrawSphereSolid ( r3dVector ( 0,0,0 ), m_fResizeSphereRadius, gCam, eElement == ELEMENT_XYZ_RESIZE ? m_uColorElementSelected : GetElementColor ( ControlElement_e ( ELEMENT_XYZ_RESIZE ) ) );

	}
	else
	{
		// ELEMENT_XZ_SNAP_MOVE
		if ( ( m_ePickState != PICK_ELEMENT_PICKED ) || (eElement == ELEMENT_XZ_SNAP_MOVE) )
		{
			//r3dRenderer->Flush();
			//r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ );

			r3dDrawSphereSolid ( r3dVector ( 0,0,0 ), m_fResizeSphereRadius, gCam, eElement == ELEMENT_XZ_SNAP_MOVE ? m_uColorElementSelected : GetElementColor ( ControlElement_e ( ELEMENT_XZ_SNAP_MOVE ) ) );
			//r3dRenderer->Flush();
			//r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );
		}
	}

	for ( int i = ELEMENT_YZ_MOVE; i <= ELEMENT_XY_MOVE; ++i )
	{
		if ( ( m_ePickState != PICK_ELEMENT_PICKED ) || (eElement == i) )
		{
			r3dVector vAxis1 = m_fRotateCircleRadius * GetAxis ( i == ELEMENT_YZ_MOVE ? ELEMENT_Y_MOVE : ELEMENT_X_MOVE );
			r3dVector vAxis2 = m_fRotateCircleRadius * GetAxis ( i == ELEMENT_XY_MOVE ? ELEMENT_Y_MOVE : ELEMENT_Z_MOVE );

			r3dColor clr = ( eElement == i ) ? m_uColorElementSelected : GetElementColor ( ControlElement_e ( i ) );

			r3dRenderer->Flush();
			
			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ );

			r3dDrawTriangle3D ( vAxis2, vAxis1, r3dVector(0,0,0), gCam, clr, NULL, NULL, true );
			r3dDrawTriangle3D ( vAxis1, vAxis2, vAxis1 + vAxis2, gCam, clr, NULL, NULL, true );
			r3dDrawTriangle3D ( r3dVector(0,0,0), vAxis1, vAxis2, gCam, clr, NULL, NULL, true );
			r3dDrawTriangle3D ( vAxis1 + vAxis2, vAxis2, vAxis1, gCam, clr, NULL, NULL, true );
			r3dRenderer->Flush();

			r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );
		}
	}

	r3dCylinder tCylinder;
	for ( int i = ELEMENT_XY_ROTATE; i <= ELEMENT_ZX_ROTATE; ++i )
	{
		GetCylinderFor ( tCylinder, ControlElement_e ( i ) );
		if ( ( m_ePickState != PICK_ELEMENT_PICKED ) || (eElement == i) )
			r3dDrawCylinder ( tCylinder, gCam, eElement == i ? m_uColorElementSelected : GetElementColor ( ControlElement_e ( i ) ), 32 );
	}

	r3dRenderer->Flush ();

	r3dRenderer->pd3ddev->SetTransform(D3DTS_VIEW, &tOldView);
}






GameObject *	g_pHoldObject = NULL;
D3DXVECTOR3		g_vHoldSnapPos;
r3dVector		g_vOffsetHoldPos;


GameObject *	g_pTgtSnapObject = NULL;
r3dVector		g_vTgtSnapPos;


bool g_bLockPoint = false;
bool g_bFocusPoint = false;

//--------------------------------------------------------------------------------------------------------
void MouseMoveUpdate()
{
	if ( ! g_Manipulator3d.IsSnapVertexes() )
	{
		g_bLockPoint = false;
		g_bFocusPoint = false;
		g_pHoldObject = NULL;
		g_pTgtSnapObject = NULL;
		return;
	}

	r3dPoint3D vDir;
	int nX, nY;

	Mouse->GetXY( nX, nY );
	r3dScreenTo3D( (float)nX, (float)nY, &vDir );

	int nNearestIndex = INVALID_INDEX;

	SnapInfo_t snap_info;
	SnapPointResult_t snap_res;

	snap_info.eType = eSnapType_Vertex;
	snap_info.fRadius = 20;
	snap_res.pObj = NULL;
	float fRadiusAbs =  snap_info.fRadius / ( r3dRenderer->ScreenW * 0.5f );

	D3DXMATRIX mWVP;

	if ( GameWorld().GetSnapPoint( r3dPoint2D( (float)nX, (float)nY ), snap_info, snap_res ) )
	{
#if 0
		D3DXVECTOR4 vvv;
		
		float fRadius = 0.03f;

		D3DXMatrixMultiply( &mWVP, &r3dRenderer->ViewMatrix, &r3dRenderer->ProjMatrix );

		float rhw = 1.0f / ( snap_res.vPos.x * mWVP._14 + snap_res.vPos.y * mWVP._24 + snap_res.vPos.z * mWVP._34 + mWVP._44 );
		float sz = rhw * ( snap_res.vPos.x * mWVP._13 + snap_res.vPos.y * mWVP._23 + snap_res.vPos.z * mWVP._33 + mWVP._43 );
		fRadiusAbs /= rhw;

		r3dDrawUniformSphere( r3dVector( snap_res.vPos ), fRadiusAbs, gCam, r3dColor( 0xffffffff ) );
#endif
	}


	g_bFocusPoint = false;


	if ( snap_res.pObj )
	{
		if ( imgui_lbp && ! g_pHoldObject )
			g_bLockPoint = true;

		g_bFocusPoint= true;
	}

	if ( imgui_lbr )
	{
		g_bLockPoint = false;
	}


	if
	(
		imgui_lbp &&
		!g_pHoldObject &&
		snap_res.pObj &&
		(g_Manipulator3d.IsObjectPicked(snap_res.pObj) || g_Manipulator3d.IsObjectPickedAsPartOfPrefab(snap_res.pObj))
	)
	{
		g_pHoldObject = snap_res.pObj;
		g_vHoldSnapPos = *snap_res.vPos.d3dx();

		g_vOffsetHoldPos = r3dVector( g_vHoldSnapPos ) - snap_res.pObj->GetPosition();

		if ( g_bLockPoint )
		{
			snap_res.pObj->SetObjFlags(OBJFLAG_SkipCastRay);
		}
	}
	else if ( imgui_lbr )
	{
		if ( g_pHoldObject )
		{
			g_pHoldObject->ResetObjFlags(OBJFLAG_SkipCastRay);
		}
		g_pHoldObject = NULL;
	}


	g_pTgtSnapObject = snap_res.pObj;
	g_vTgtSnapPos = snap_res.vPos;


	if ( g_Manipulator3d.PickedObject() )
	{
		D3DXMatrixMultiply( &mWVP, &r3dRenderer->ViewMatrix, &r3dRenderer->ProjMatrix );
		r3dVector v;
		D3DXVec3TransformCoord( ( D3DXVECTOR3 * )&v, ( D3DXVECTOR3 * )& snap_res.vPos, &mWVP );
		v.x = ( v.x + 1 ) * r3dRenderer->ScreenW * 0.5f;
		v.y = ( -v.y + 1 ) * r3dRenderer->ScreenH * 0.5f;

		float fSize = 5;
		if ( g_pTgtSnapObject != g_pHoldObject)
		{
			if ( g_pHoldObject )
				r3dDrawBox2DWireframe( nX - fSize * 0.5f, nY - fSize * 0.5f, fSize, fSize, r3dColor( 0xff2020ff ) );
			else
				r3dDrawBox2DWireframe( v.x - fSize * 0.5f, v.y - fSize * 0.5f, fSize, fSize, r3dColor( 0xffaaffff ) );
		}

		if ( g_pTgtSnapObject )
			r3dDrawBox2DWireframe( v.x - fSize * 0.5f, v.y - fSize * 0.5f, fSize, fSize, r3dColor( 0xffffff20 ) );

	}
}

extern int ObjectEditMove;

//--------------------------------------------------------------------------------------------------------
r3dVector ObjectManipulator3d::Picker_Move ( GameObject * pObj, POINT PickPoint, POINT pt2 )
{
	r3d_assert(pObj);

	r3dVector vObjPos = GetSelectionCenter();
	r3dVector vDeltaPos = pObj->GetPosition() - vObjPos;

	if ( m_bAttachMode || !ObjectEditMove )
	{
		GameObject *go = g_pHoldObject ? g_pHoldObject : pObj;
		r3dVector vHoldPos = m_bAttachMode ? g_vOffsetHoldPos + go->GetPosition() : GetSelectionCenter();

		r3dPoint3D vDir;
		int nX, nY;

		Mouse->GetXY( nX, nY );
		r3dScreenTo3D( (float)nX, (float)nY, &vDir );

		if ( vDir.y == 0 )
			return r3dPoint3D( 0, 0, 0 );


		float fFraction = -( gCam.y - vHoldPos.y ) / vDir.y;
		r3dPoint3D v = gCam + vDir * fFraction;

		if ( g_pTgtSnapObject )
		{
			vDeltaPos += g_vTgtSnapPos - vHoldPos;
		}
		else
			vDeltaPos += v - vHoldPos;

		if ( m_bMoveSnapping && m_fMoveSnappingVal > 0.0f )
		{
			vDeltaPos = CalcStepVector ( vDeltaPos, m_fMoveSnappingVal );
			if ( vDeltaPos.LengthSq() < FLT_EPSILON )
				return r3dVector ( 0,0,0 );
		}

		vObjPos += vDeltaPos;
	}
	else
	{
		extern r3dPoint3D UI_TargetPos;
		vDeltaPos += UI_TargetPos - vObjPos;
		float fOldY = vDeltaPos.y;
		vDeltaPos.y = 0;
	
		if ( m_bMoveSnapping && m_fMoveSnappingVal > 0.0f )
		{
			vDeltaPos = CalcStepVector ( vDeltaPos, m_fMoveSnappingVal );
			if ( vDeltaPos.LengthSq() < FLT_EPSILON )
				return r3dVector ( 0,0,0 );
		}

		vDeltaPos.y = fOldY;

		vObjPos += vDeltaPos;
	}

	pObj->SetPosition ( vObjPos );
	pObj->OnPickerMoved ();

	return vDeltaPos;
}

r3dVector ObjectManipulator3d::Picker_Move_Plane ( GameObject * pObj, POINT PickPoint, POINT pt2, bool bLocal )
{
	r3d_assert(pObj);

	r3dVector vObjPos = GetSelectionCenter();
	r3dVector vDeltaPos = pObj->GetPosition() - vObjPos;

	r3dRay tRay = Picker_GetClickRay ( pt2 );

	D3DXMATRIX  mTrans;
	D3DXMatrixTranslation ( &mTrans, vObjPos.x, vObjPos.y, vObjPos.z );

	D3DXMATRIX mScale;
	GetScaleMatrix ( pObj, mScale );

	D3DXMATRIX mRot;
	if ( bLocal )
		mRot = pObj->GetRotationMatrix();
	else
		D3DXMatrixIdentity ( &mRot );

	D3DXMATRIX mResRot = mScale * mRot * mTrans;
	
	r3dVector vPtDesired;
	if ( !GeomRayPlaneIntersection ( tRay, m_MovePlane, &vPtDesired ) )
		return r3dVector ( 0,0,0 );
	
	vDeltaPos += vPtDesired - vObjPos + m_MoveByPlaneOffset;

	if ( m_bMoveSnapping && m_fMoveSnappingVal > 0.0f )
	{
		vDeltaPos = CalcStepVector ( vDeltaPos, m_fMoveSnappingVal );
		if ( vDeltaPos.LengthSq() < FLT_EPSILON )
			return r3dVector ( 0,0,0 );
	}

	if ( vDeltaPos.LengthSq() < FLT_EPSILON )
		return r3dVector ( 0,0,0 );

	vObjPos += vDeltaPos;

	pObj->SetPosition ( vObjPos );
	pObj->OnPickerMoved ();

	return vDeltaPos;
}


bool ObjectManipulator3d::Picker_MoveAxis ( GameObject * pObj, ControlElement_e eElement, POINT PickPoint, POINT pt2, bool bLocal )
{
	r3d_assert(pObj);
	if ( PickPoint.x == pt2.x && PickPoint.y == pt2.y )
		return false;

	float fWndWidth = r3dRenderer->ScreenW;
	float fWndHeight = r3dRenderer->ScreenH;

	D3DXMATRIX mView = r3dRenderer->ViewMatrix;
	D3DXMATRIX mProj = r3dRenderer->ProjMatrix;

	D3DXMATRIX mRot;
	if ( bLocal )
	{
		mRot = pObj->GetRotationMatrix();
		// transpose-inverse for transformNormal fn
		D3DXMatrixInverse(&mRot, 0, &mRot);
		D3DXMatrixTranspose(&mRot, &mRot);
	}
	else
		D3DXMatrixIdentity ( &mRot );

	// Compute the vector of the pick ray in screen space

	// Get the inverse view matrix
	D3DXMATRIX m;
	D3DXMATRIX mFinal1 = mView * mProj;
	D3DXMatrixInverse ( &m, NULL, &mFinal1 );

	r3dVector vObjPos = GetSelectionCenter();

	r3dVector vProjected1;
	r3dVector vProjected2;
	D3DXVec3TransformCoord ( vProjected1.d3dx(), vObjPos.d3dx(), &mFinal1 );
	D3DXVec3TransformCoord ( vProjected2.d3dx(), vObjPos.d3dx(), &mFinal1 );

	r3dVector v1;
	r3dVector v2;

	v1.x =  ( ( ( 2.0f * PickPoint.x ) / fWndWidth  ) - 1 );
	v1.y = -( ( ( 2.0f * PickPoint.y ) / fWndHeight ) - 1 );
	v1.z = vProjected1.z;

	v2.x =  ( ( ( 2.0f * pt2.x ) / fWndWidth  ) - 1 );
	v2.y = -( ( ( 2.0f * pt2.y ) / fWndHeight ) - 1 );
	v2.z = vProjected2.z;

	r3dVector vObject1;
	r3dVector vObject2;
	D3DXVec3TransformCoord ( vObject1.d3dx(), v1.d3dx(), &m );
	D3DXVec3TransformCoord ( vObject2.d3dx(), v2.d3dx(), &m );

	bool bChanged = false;
	switch ( eElement )
	{
	case ELEMENT_X_MOVE:
	case ELEMENT_X_RESIZE:
		vObject1.y = vObject2.y = 0.0f;
		vObject1.z = vObject2.z = 0.0f;

		if ( eElement == ELEMENT_X_MOVE )
		{
			D3DXVec3TransformNormal( vObject1.d3dx(), vObject1.d3dx(), &mRot );
			D3DXVec3TransformNormal ( vObject2.d3dx(), vObject2.d3dx(), &mRot );
			r3dVector deltaV = vObject2 - vObject1;
			bChanged |= Picker_ElementMove ( pObj, deltaV );
		}
		else
			bChanged |= Picker_ElementSize ( pObj, vObject2 - vObject1, bLocal, false );
		break;

	case ELEMENT_Y_MOVE:
	case ELEMENT_Y_RESIZE:
		vObject1.x = vObject2.x = 0.0f;
		vObject1.z = vObject2.z = 0.0f;
		if ( eElement == ELEMENT_Y_MOVE )
		{
			D3DXVec3TransformNormal ( vObject1.d3dx(), vObject1.d3dx(), &mRot );
			D3DXVec3TransformNormal ( vObject2.d3dx(), vObject2.d3dx(), &mRot );
			r3dVector deltaV = vObject2 - vObject1;
			bChanged |= Picker_ElementMove ( pObj, deltaV );
		}
		else
			bChanged |= Picker_ElementSize ( pObj, vObject2 - vObject1, bLocal, false );
		break;

	case ELEMENT_Z_MOVE:
	case ELEMENT_Z_RESIZE:
		vObject1.x = vObject2.x = 0.0f;
		vObject1.y = vObject2.y = 0.0f;
		if ( eElement == ELEMENT_Z_MOVE )
		{
			D3DXVec3TransformNormal ( vObject1.d3dx(), vObject1.d3dx(), &mRot );
			D3DXVec3TransformNormal ( vObject2.d3dx(), vObject2.d3dx(), &mRot );
			r3dVector deltaV = vObject2 - vObject1;
			bChanged |= Picker_ElementMove ( pObj, deltaV );
		}
		else
			bChanged |= Picker_ElementSize ( pObj, vObject2 - vObject1, bLocal, false );
		break;
	case ELEMENT_XZ_SNAP_MOVE:
		{
			r3dVector vMove = Picker_Move ( pObj, PickPoint, pt2 );
			if ( vMove.LengthSq() > 0.000001f )
				bChanged = true;
		}
		break;

	case ELEMENT_XY_MOVE:
	case ELEMENT_YZ_MOVE:
	case ELEMENT_XZ_MOVE:
		{
			r3dVector vMove = Picker_Move_Plane ( pObj, PickPoint, pt2, m_bLocal && m_bLocalEnable );
			if ( vMove.LengthSq() > 0.000001f )
				bChanged = true;
		}
		break;

	case ELEMENT_XYZ_RESIZE:
		{
			vObject1.y = vObject2.y = 0.0f;
			vObject1.z = vObject2.z = 0.0f;

			r3dVector vDiff = vObject2 - vObject1;
			vDiff.y = vDiff.z = vDiff.x;
			//float fTmpMax = r3dTL::Max(r3dTL::Max(vDiff.x,vDiff.y),vDiff.z);
			//float fTmpMin = r3dTL::Min(r3dTL::Min(vDiff.x,vDiff.y),vDiff.z);
			//vDiff.x = vDiff.y = vDiff.z = fabsf(fTmpMin) > fabsf(fTmpMax) ? fTmpMin : fTmpMax;
			bChanged |= Picker_ElementSize ( pObj, vDiff, bLocal, true );
		}
		break;

	case ELEMENT_XY_ROTATE:
		{
			float fAngle = (bLocal ? ( v2 - v1 ).x : -( v2 - v1 ).y) * m_fAngleMult;

			bChanged |= Picker_ElementRotate ( pObj, 0, 0, fAngle, bLocal );
		}
		break;
	case ELEMENT_YZ_ROTATE:
		{
			float fAngle = (bLocal ? ( v2 - v1 ).x : ( v2 - v1 ).y) * m_fAngleMult;

			bChanged |= Picker_ElementRotate ( pObj, fAngle, 0, 0, bLocal );
		}
		break;

	case ELEMENT_ZX_ROTATE:
		{
			float fAngle = -( v2 - v1 ).x * m_fAngleMult;

			bChanged |= Picker_ElementRotate ( pObj, 0, fAngle, 0, bLocal );
		}
		break;

	case ELEMENT_ATTACH_VERT:
		break;

	default:
		r3d_assert( "Unknown axis" && 0 );
	}

	return bChanged;
}

bool ObjectManipulator3d::Picker_ElementMove ( GameObject * pObj, const r3dVector & deltaPos )
{
	r3d_assert(pObj);
	if ( deltaPos.LengthSq() == 0.0f )
		return false;

	r3dVector deltaPosConv = deltaPos;

	if ( m_bMoveSnapping && m_fMoveSnappingVal > 0.0f )
	{
		deltaPosConv = CalcStepVector ( deltaPos, m_fMoveSnappingVal );
		if ( deltaPosConv.LengthSq() < FLT_EPSILON )
			return false;
	}

	pObj->SetPosition ( pObj->GetPosition() + deltaPosConv );
	pObj->OnPickerMoved ();
	return true;
}

bool ObjectManipulator3d::Picker_ElementSize ( GameObject * pObj, const r3dVector & deltaSize, bool bLocal, bool bUniform )
{
	r3d_assert(pObj);
	if ( deltaSize.LengthSq() == 0.0f || ! bLocal )
		return false;

	const float MIN_SCALE = 0.01f;
	const float MAX_SCALE = 1000.0f;
	const float MIN_SIZE = 0.0001f;

	r3dVector vSize = pObj->GetBBoxWorld().Size;

	float sizeMax = r3dTL::Max(r3dTL::Max(fabsf(vSize.x), fabsf(vSize.y)),fabsf(vSize.z));

	r3dVector vScaleResult ( ( fabsf ( vSize.x ) < MIN_SIZE ) ? 0.0f : deltaSize.x / fabsf ( bUniform ? sizeMax : vSize.x )
		, ( fabsf ( vSize.y ) < MIN_SIZE ) ? 0.0f : deltaSize.y / fabsf ( bUniform ? sizeMax : vSize.y )
		, ( fabsf ( vSize.z ) < MIN_SIZE ) ? 0.0f : deltaSize.z / fabsf ( bUniform ? sizeMax : vSize.z ) );

	r3dVector vScl = pObj->GetScale();
	//r3dVector vRot = pObj->Orientation;

	if ( vScl.x < 0.0f )
		vScaleResult.x *= -1.0f;
	if ( vScl.y < 0.0f )
		vScaleResult.y *= -1.0f;
	if ( vScl.z < 0.0f )
		vScaleResult.z *= -1.0f;

	vScaleResult.x = R3D_CLAMP ( vScaleResult.x + 1.0f, MIN_SCALE, MAX_SCALE );
	vScaleResult.y = R3D_CLAMP ( vScaleResult.y + 1.0f, MIN_SCALE, MAX_SCALE );
	vScaleResult.z = R3D_CLAMP ( vScaleResult.z + 1.0f, MIN_SCALE, MAX_SCALE );

	r3dVector vSclNew;
	
	vSclNew = vScl * vScaleResult;

	if ( vScl.x >=   MIN_SCALE && vSclNew.x <  MIN_SCALE )
		vSclNew.x = -MIN_SCALE;
	if ( vScl.x <=  -MIN_SCALE && vSclNew.x > -MIN_SCALE )
		vSclNew.x =  MIN_SCALE;

	if ( vScl.y >=   MIN_SCALE && vSclNew.y <  MIN_SCALE )
		vSclNew.y = -MIN_SCALE;
	if ( vScl.y <=  -MIN_SCALE && vSclNew.y > -MIN_SCALE )
		vSclNew.y =  MIN_SCALE;

	if ( vScl.z >=   MIN_SCALE && vSclNew.z <  MIN_SCALE )
		vSclNew.z = -MIN_SCALE;
	if ( vScl.z <=  -MIN_SCALE && vSclNew.z > -MIN_SCALE )
		vSclNew.z =  MIN_SCALE;

	pObj->SetScale ( vSclNew );
	pObj->OnPickerScaled();
	return true;
}

bool ObjectManipulator3d::Picker_ElementRotate ( GameObject * pObj, float angleX, float angleY, float angleZ, bool bLocal )
{
	r3d_assert(pObj);
	if ( angleX == 0.0f && angleY == 0.0f && angleZ == 0.0f )
		return false;

	r3dVector vRotNew (angleX, angleY, angleZ);

	if ( m_bAngleSnapping && m_fAngleSnappingVal > 0.0f )
	{
		int iSteps = 0;
		vRotNew = CalcStepVector ( vRotNew, m_fAngleSnappingVal, &iSteps );

		m_iAngleAlignSteps += ((angleZ == 0.0f) ? 1 : -1) * ( bLocal ? -1 : 1 ) * ( iSteps );

		if ( vRotNew.LengthSq() < FLT_EPSILON )
			return false;
	}

	D3DXMATRIX mRot;
	r3dVector vRot = pObj->GetRotationVector();
	vRot.x = R3D_DEG2RAD ( vRot.x ); vRot.y = R3D_DEG2RAD ( vRot.y ); vRot.z = R3D_DEG2RAD ( vRot.z );
	NormalizeYPR ( vRot );
	D3DXMatrixRotationYawPitchRoll ( &mRot, vRot.x, vRot.y, vRot.z );

	D3DXMATRIX mRotX, mRotY, mRotZ;
	D3DXMatrixRotationX ( &mRotX, vRotNew.x );
	D3DXMatrixRotationY ( &mRotY, vRotNew.y );
	D3DXMatrixRotationZ ( &mRotZ, vRotNew.z );

	if ( bLocal )
		mRot = mRotX * mRotY * mRotZ * mRot;
	else
		mRot = mRot * mRotX * mRotY * mRotZ;
	
	mRot.m[3][0] = 0.0f; mRot.m[3][1] = 0.0f; mRot.m[3][2] = 0.0f; mRot.m[3][3] = 1.0f;
	MatrixGetYawPitchRoll ( mRot, vRot.x, vRot.y, vRot.z );
	NormalizeYPR ( vRot );
	/*
	mRot = mRotX * mRotY * mRotZ;
	D3DXVec3TransformNormal ( vRot.d3dx(), vRot.d3dx(), &mRot );
*/
	vRot.x = R3D_RAD2DEG ( vRot.x ); vRot.y = R3D_RAD2DEG ( vRot.y ); vRot.z = R3D_RAD2DEG ( vRot.z );
	pObj->SetRotationVector( vRot );
	pObj->OnPickerRotated();

	return true;
}

bool ObjectManipulator3d::Picker_IsPicked ( GameObject * pObj/* = NULL */ ) const
{
	if ( pObj )
	{
		for ( uint32_t i = 0; i < m_dPickedObjects.Count (); i++ )
		{
			if ( m_dPickedObjects[i].pObject == pObj )
				return true;
		}

		return false;
	}
			
	return ( m_dPickedObjects.Count() > 0 );
}

bool ObjectManipulator3d::Picker_IsPickedAsPartOfPerfab(GameObject * pObj) const
{
	if (!pObj)
		return false;

	for (uint32_t i = 0; i < m_dPickedObjects.Count (); i++)
	{
		GameObject *o = m_dPickedObjects[i].pObject;
		bool isPrefab = !!o->isObjType(OBJTYPE_Prefab);
		if (isPrefab)
		{
			obj_Prefab *p = static_cast<obj_Prefab*>(o);
			if (p->IsPartOfPrefab(pObj))
				return true;
		}
	}
	return false;
}

void ObjectManipulator3d::Picker_Pick ( GameObject * pObj, bool bSingle, bool pushFront )
{
	if(pObj == NULL)
		return;
	if (pObj->ObjFlags & OBJFLAG_Removed)
		return;

	if( pObj->ObjFlags & OBJFLAG_SkipDraw )
		return;

	if ( bSingle )
	{
		for ( uint32_t i = 0; i < m_dPickedObjects.Count(); i++ )
		{
			if ( !m_dPickedObjects[i].bOldSkipCastRay )
				m_dPickedObjects[i].pObject->ResetObjFlags(OBJFLAG_SkipCastRay);
		}
		m_dPickedObjects.Clear();
	}
	
	r3d_assert ( pObj );
	SelectionObj_t tInfo;
	tInfo.pObject = pObj;
	tInfo.bCloning = false;
	tInfo.bOldSkipCastRay = tInfo.pObject->ObjFlags & OBJFLAG_SkipCastRay ? true : false;
	

	//if ( !m_bLocked && ! m_bAttachMode )
	//	tInfo.pObject->SetObjFlags(OBJFLAG_SkipCastRay);


	pushFront ? m_dPickedObjects.Insert(0, tInfo) : m_dPickedObjects.PushBack(tInfo);

	m_ePickState = PICK_SELECTED;
}

void ObjectManipulator3d::PickByName	( const char* Name, int bExactMatch )
{
	char src[ 1024 ] = { 0 }, dest[ 1024 ] = { 0 } ;

	strncpy( src, Name, sizeof src - 1 ) ;
	strlwr( src ) ;

	Picker_Drop() ;

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* obj = iter.current;

		strncpy( dest, obj->FileName.c_str(), sizeof dest - 1 ) ;
		strlwr( dest ) ;

		char drive[ 16 ], dir[ 1024 ], file[ 1024 ], ext[ 1024 ] ;
		_splitpath( dest, drive, dir, file, ext ) ;

		strcpy( dest, file ) ;

		if( ( bExactMatch && strcmp( dest, src ) == 0 )
				||
			( !bExactMatch && strstr( dest, src ) != 0 ) )
		{
			Picker_Pick( obj, false ) ;
		}
	}
}

void ObjectManipulator3d::Picker_Drop ()
{
	for ( uint32_t i = 0; i < m_dPickedObjects.Count(); i++ )
	{
		if ( !m_dPickedObjects[i].bOldSkipCastRay )
			m_dPickedObjects[i].pObject->ResetObjFlags(OBJFLAG_SkipCastRay);
		m_dPickedObjects[i].pObject->OnPickerDrop();
	}

	m_dPickedObjects.Clear();

	if( m_DropCallback )
		m_DropCallback() ;

	m_ePickState = PICK_UNSELECTED;
}

void ObjectManipulator3d::Picker_Drop ( GameObject * pObj )
{
	for ( uint32_t i = 0; i < m_dPickedObjects.Count(); i++ )
	{
		if ( m_dPickedObjects[i].pObject == pObj )
		{
			if ( !m_dPickedObjects[i].bOldSkipCastRay )
				m_dPickedObjects[i].pObject->ResetObjFlags(OBJFLAG_SkipCastRay);

			m_dPickedObjects[i].pObject->OnPickerDrop();

			m_dPickedObjects.Erase(i);

			if ( m_dPickedObjects.Count() == 0 )
				m_ePickState = PICK_UNSELECTED;
			return;
		}
	}
}

void ObjectManipulator3d::Draw ()
{
	if ( !m_bEnabled )
		return;

	struct PerfMarker
	{
		PerfMarker()
		{
			D3DPERF_BeginEvent( 0, L"ObjectManipulator3d::Draw" );
		}

		~PerfMarker()
		{
			D3DPERF_EndEvent();
		}
	} perfMarker; (void)perfMarker;

	r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH ) ;

	D3DXMATRIX ident; D3DXMatrixIdentity(&ident);
	r3dRenderer->pd3ddev->SetTransform(D3DTS_WORLD, &ident);

	if ( m_bSelectionRectShow )
	{
		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );
		r3dRenderer->SetVertexShader();
		r3dRenderer->SetPixelShader();

		r3dDrawBox2DWireframe( (float)r3dTL::Min ( m_tSelectionStartPt.x, m_tSelectionEndPt.x ), (float)r3dTL::Min ( m_tSelectionStartPt.y, m_tSelectionEndPt.y ), (float)abs ( m_tSelectionStartPt.x - m_tSelectionEndPt.x ), (float)abs ( m_tSelectionStartPt.y - m_tSelectionEndPt.y ), r3dColor::white );
	}

	r3dBoundBox tBoxUnion;

	if ( m_dPickedObjects.Count() > 0 )
	{
		tBoxUnion = m_dPickedObjects[0].pObject->GetBBoxWorld ();

		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);

		r3dRenderer->SetRenderingMode( R3D_BLEND_ZC | R3D_BLEND_NOALPHA | R3D_BLEND_NZ );
		r3dRenderer->SetVertexShader();
		r3dRenderer->SetPixelShader();
	}

	for ( uint32_t i = 0; i < m_dPickedObjects.Count(); i++ )
	{
		if(m_dPickedObjects[i].pObject)
		{
			tBoxUnion.ExpandTo ( m_dPickedObjects[i].pObject->GetBBoxWorld () );
			r3d_assert ( m_dPickedObjects[i].pObject );		
			r3dDrawUniformBoundBox ( m_dPickedObjects[i].pObject->GetBBoxWorld(), gCam, r3dColor::white );
			void PushSelectedObject( GameObject* obj );
			PushSelectedObject(m_dPickedObjects[i].pObject);
			//m_dPickedObjects[i].pObject->DrawSelected( gCam, rsDrawComposite1 );
		}
	}

	if ( m_dPickedObjects.Count() > 0 )
	{
		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );

		if ( m_dPickedObjects.Count() > 1 )
			r3dDrawUniformBoundBox ( tBoxUnion, gCam, r3dColor::white );
		if ( m_ePickState != PICK_UNSELECTED )
			Picker_DrawPicked ( m_dPickedObjects[0].pObject, m_ePickAxisHighlighted, m_bLocal && m_bLocalEnable );
	}

	r3dRenderer->SetRenderingMode( R3D_BLEND_POP ) ;
}

void ObjectManipulator3d::Update ()
{
	if ( !m_bEnabled )
		return;

	if ( m_dPickedObjects.Count() == 1 )
	{
		if ( Keyboard->IsPressed(kbsLeftControl) && Keyboard->WasPressed(kbsV) )
			 m_dPickedObjects[0].pObject->SetRotationVector ( r3dVector ( 0, 0, 0 ) );
	}
}

bool ObjectManipulator3d::IsEnable () const
{
	return m_bEnabled;
}

void ObjectManipulator3d::Enable ()
{
	m_bEnabled = true;
	m_bLocked = false;
	m_bScaleEnable = true;
	m_bLocalEnable = true;
}

void ObjectManipulator3d::Disable ( bool dropPicked /*= true*/ )
{
	if( dropPicked )
	{
		Picker_Drop();
	}

	m_bEnabled = false;
	m_bScaleEnable = true;
	m_bLocalEnable = true;

	// drop 'procedural objects' if they were selected
	if( m_DropCallback )
	{
		m_DropCallback();
	}

	m_SelectCallback = NULL;
	m_DropCallback = NULL;
}

bool ObjectManipulator3d::IsLocked () const
{
	return m_bLocked;
}

void ObjectManipulator3d::Lock ()
{
	m_bLocked = true;
}

void ObjectManipulator3d::Unlock ()
{
	m_bLocked = false;
	Picker_Drop ();
}

void ObjectManipulator3d::ScaleDisable ()
{
	m_bScaleEnable = false;
	m_bLocalEnable = false;
}

void ObjectManipulator3d::ScaleEnable ()
{
	m_bScaleEnable = true;
	m_bLocalEnable = true;
}


void ObjectManipulator3d::TypeFilterSet ( const char * sFilter )
{
	m_sTypeFilter = sFilter;

	if ( m_sTypeFilter.c_str() && m_sTypeFilter.c_str()[0] )
	{
		r3dTL::TArray < GameObject * > toRemove;
		for ( uint32_t i = 0; i < m_dPickedObjects.Count(); i++ )
		{
			r3d_assert ( m_dPickedObjects[i].pObject );
			if ( m_dPickedObjects[i].pObject->Class->Name != m_sTypeFilter.c_str () )
				toRemove.PushBack(m_dPickedObjects[i].pObject);
		}

		for ( uint32_t i = 0; i < toRemove.Count(); i++ )
			Picker_Drop ( toRemove[i] );
	}
}

GameObject * ObjectManipulator3d::PickedObject () const
{
	return ( m_dPickedObjects.Count () > 0 ) ? m_dPickedObjects[0].pObject : NULL;
}

void ObjectManipulator3d::PickedObjectSet ( GameObject * pObj )
{
	bool bFilterEnabled = m_sTypeFilter.c_str() && m_sTypeFilter.c_str()[0];
	if ( pObj && ( !bFilterEnabled || ( pObj->Class->Name == m_sTypeFilter.c_str() ) ) )
	{
		if ( ( m_dPickedObjects.Count () != 1 ) || pObj != m_dPickedObjects[0].pObject )
			Picker_Pick ( pObj );
	}
	else
		Picker_Drop ();
}

void ObjectManipulator3d::PickerAddToPicked ( GameObject * pObj, bool pushFront )
{
	Picker_Pick ( pObj, false, pushFront);
}

void ObjectManipulator3d::PickerRemoveFromPicked ( GameObject * pObj )
{
	Picker_Drop ( pObj );
}


void ObjectManipulator3d::PickerResetPicked ()
{
	Picker_Drop ();
}

void ObjectManipulator3d::DeletePickedObjects ()
{
	r3dTL::TArray < GameObject * > dObjToDel;

	struct SelectionObjFinder
	{
		GameObject *objToFind;
		explicit SelectionObjFinder(GameObject *o): objToFind(o) {}
		bool operator () (const SelectionObj_t &a)
		{
			return a.pObject == objToFind;
		}
	};

	//	Remove all attached lights of obj_LightMesh objects from picked list
	//	This is needed to prevent dangling pointers in below code (obj_LightMesh manages deletion of attached light by itself)

	r3dTL::TArray < GameObject * > dontDel;
	for (uint32_t i = 0; i < m_dPickedObjects.Count (); i++)
	{
		if ( GameObject* gameObject = m_dPickedObjects[i].pObject)
		{
			if( IsImmortal( gameObject ) )
			{
				dontDel.PushBack( gameObject ) ;
				continue ;
			}

			if (m_dPickedObjects[i].pObject->Class->Name == "obj_LightMesh")
			{
				obj_LightMesh *lm = static_cast<obj_LightMesh*>(m_dPickedObjects[i].pObject);
				dontDel.PushBack(GameWorld().GetObject(lm->GetLightObjectID()));
			}
			//	Also add light mesh object if attached light is in delete list
			if (m_dPickedObjects[i].pObject->Class->Name == "obj_LightHelper")
			{
				GameObject *o = GameWorld().GetObject(m_dPickedObjects[i].pObject->ownerID);
				if (o && o->Class->Name == "obj_LightMesh")
				{
					SelectionObj_t *b = &m_dPickedObjects[0];
					SelectionObj_t *e = b + m_dPickedObjects.Count();
					SelectionObj_t *objInDelList = std::find_if(b, e, SelectionObjFinder(o));
					if (objInDelList == e)
					{
						dObjToDel.PushBack(o);
						dontDel.PushBack(m_dPickedObjects[i].pObject);
					}
				}
			}
		}
	}


	for (uint32_t i = 0; i < dontDel.Count(); ++i)
	{
		PickerRemoveFromPicked(dontDel[i]);
	}

	for ( uint32_t i = 0; i < m_dPickedObjects.Count (); i++ )
	{
		dObjToDel.PushBack(m_dPickedObjects[i].pObject);
	}

	Picker_Drop();

	UndoEntityAddDel * pUndo = NULL ;

	if( m_UndoEnabled )
	{
		pUndo = ( UndoEntityAddDel * ) g_pUndoHistory->CreateUndoItem( UA_ENT_ADDDEL );
		assert( pUndo );
	}

	for ( uint32_t i = 0; i < dObjToDel.Count (); i++ )
	{
		if(dObjToDel[i]->Class->Name == "obj_NavigationPoint")
		{
			GameWorld().DeleteObject(dObjToDel[i]);
			continue;
		}
		if ( pUndo )
		{
			EntAddDel_t st;

			st.bDelete = true;
			st.pEnt = dObjToDel[i];

			pUndo->AddItem( st );
		}
	}

	if ( pUndo )
		pUndo->Create ();

	m_dPickedObjects.Clear ();
}

void ObjectManipulator3d::SetAngleSnapping ( bool bEnable, float fValInDegree )
{
	m_bAngleSnapping = bEnable;
	m_fAngleSnappingVal = D3DXToRadian ( fValInDegree );
}

void ObjectManipulator3d::SetMoveSnapping ( bool bEnable, float fVal )
{
	m_bMoveSnapping = bEnable;
	m_fMoveSnappingVal = fVal;
}

GameObject * ObjectManipulator3d::PickedObjectGet ( int i )
{
	r3d_assert ( i >= 0 && i < (int)m_dPickedObjects.Count () );
	return m_dPickedObjects[i].pObject;
}

uint32_t ObjectManipulator3d::PickedObjectCount () const
{
	return m_dPickedObjects.Count ();
}

static void GatherHeirs( GameObject* obj, r3dTL::TArray< const AClass* >* heirs )
{
	AClass* cla = obj->Class ;

	r3dTL::TArray< const AClass* > reverseHeirs ;

	for( ; cla && cla != &AObject::ClassData; cla = cla->BaseClass )
	{
		reverseHeirs.PushBack( cla ) ;
	}

	heirs->Clear() ;
	heirs->Reserve( reverseHeirs.Count() ) ;

	for( int i = reverseHeirs.Count() - 1 ; i >= 0 ; i -- )
	{
		heirs->PushBack( reverseHeirs[ i ] ) ;
	}	
}

const AClass*
ObjectManipulator3d::GetPickedMostDerivedClass() const
{
	int e = m_dPickedObjects.Count() ;

	r3dString mostDerived ;

	if( e )
	{
		r3dTL::TArray< const AClass* > heirs ;
		GatherHeirs( m_dPickedObjects[ 0 ].pObject, & heirs ) ;

		int mostDerIdx = heirs.Count() - 1 ;
		
		r3dTL::TArray< const AClass* > cmpHeirs ;

		for( int i = 1 ; i < e; i ++ )
		{
			GameObject* picked = m_dPickedObjects[ i ].pObject ;

			GatherHeirs( picked, &cmpHeirs ) ;

			mostDerIdx = R3D_MIN( mostDerIdx, int( cmpHeirs.Count() - 1 ) ) ;

			for( int i = 0, e = mostDerIdx ; i <= e ; i ++ )
			{
				if( cmpHeirs[ i ] != heirs[ i ] )
				{
					mostDerIdx = i -  1 ;
					break ;
				}
			}

			if( mostDerIdx < 0 )
				break ;
		}

		if( mostDerIdx >= 0 )
			return heirs[ mostDerIdx ] ;
	}

	return 0 ;

}

void 
ObjectManipulator3d::GetSelectionRect( RECT* oRect ) const
{
	oRect->left = m_tSelectionStartPt.x ;
	oRect->right = m_tSelectionEndPt.x ;

	oRect->top = m_tSelectionStartPt.y ;
	oRect->bottom = m_tSelectionEndPt.y ;

}

void ObjectManipulator3d::SetSelectCallback( SelectCallback scb )
{
	m_SelectCallback = scb;
}

void ObjectManipulator3d::SetDropCallback( DropCallback dcb )
{
	m_DropCallback = dcb;
}

SelectCallback
ObjectManipulator3d::GetSelectCallback( )
{
	return m_SelectCallback;
}

DropCallback
ObjectManipulator3d::GetDropCallback( )
{
	return m_DropCallback;
}

const r3dString& ObjectManipulator3d::GetTypeFilter() const
{
	return m_sTypeFilter;
}

void ObjectManipulator3d::SetUndoEnabled( bool enabled )
{
	m_UndoEnabled = enabled;
}

bool ObjectManipulator3d::IsUndoEnabled() const
{
	return m_UndoEnabled;
}

void ObjectManipulator3d::SetCloneEnabled( bool enabled )
{
	m_CloneEnabled = enabled;
}


bool ObjectManipulator3d::MouseMessage ( UINT message, POINT pt, int delta )
{
	if ( !m_bEnabled )
		return false;

	switch ( message ) 
	{
	case WM_MBUTTONDOWN:
		{
			return MouseMiddleBtnDown ( pt, delta );
		}
		break;

	case WM_MBUTTONUP:
		{
			return MouseMiddleBtnUp( pt, delta );
		}
		break;

	case WM_LBUTTONDOWN:
		{
			return MouseLeftBtnDown ( pt, delta );
		}
		break;

	case WM_LBUTTONUP:
		{
			return MouseLeftBtnUp ( pt, delta );
		}
		break;

	case WM_MOUSEWHEEL:
		{
			//return MouseWheel( pt, delta );
		}
		break;

	case WM_MOUSEMOVE:
		{
			return MouseMove( pt, delta );
		}
		break;

	case WM_RBUTTONDOWN:
		{
			return MouseRightBtnDown( pt, delta );
		}
		break;

	case WM_RBUTTONUP:
		{
			return MouseRightBtnUp( pt, delta );
		}
		break;
	}

	return false; // not translated
}


bool ObjectManipulator3d::KeyboardMessageKeyUp ( int vik )
{
	if ( vik == VK_CONTROL )
	{
		m_bLocal = false;
		return true;
	}

	if( vik == VK_MENU )
	{
		m_bFreeMove = false;
		return true;
	}

	if ( vik == VK_SHIFT )
	{
		m_bClone = false;
		return true;
	}

	return false;
}

bool ObjectManipulator3d::KeyboardMessageKeyDown ( int vik )
{
	if ( vik == VK_CONTROL )
	{
		m_bLocal = true;
		return true;
	}

	if( vik == VK_MENU )
	{
		m_bFreeMove = true;
		return true;
	}

	if ( vik == VK_SHIFT && m_CloneEnabled )
	{
		m_bClone = true;
		return true;
	}

	return false;
}

bool ObjectManipulator3d::MouseLeftBtnDown ( POINT pt, int delta )
{
	m_bLeftBtnDown = true;

	m_tSelectionStartPt = pt;

	m_bSelectionRectShow = false;
	
	{
		D3DXMATRIX mView = r3dRenderer->ViewMatrix;
		
		r3dVector vCameraOrigin = r3dVector ( - mView._41, - mView._42, - mView._43 );

		// pick control elements
		if ( m_ePickState == PICK_SELECTED )
		{
			r3d_assert ( m_dPickedObjects.Count() > 0 );

			m_ePickElementSelected = Picker_PickElement ( m_dPickedObjects[0].pObject, pt, m_bLocal && m_bLocalEnable );

			if ( m_ePickElementSelected != ObjectManipulator3d::ELEMENT_NONE )
			{
				m_PickPoint = pt;
				m_iAngleAlignSteps = 0;

				if ( m_bClone && m_ePickElementSelected >= ELEMENT_X_MOVE && m_ePickElementSelected <= ELEMENT_XY_MOVE )
					m_ePickState = PICK_CLONE;
				else
				{
					m_ePickState = PICK_ELEMENT_PICKED;
					ObjectActionBegin ( m_ePickElementSelected );
				}
				return true;
			}
		}
	}
	
	return false;
}

extern GameObject *	g_pTgtSnapObject;
extern r3dVector g_vTgtSnapPos;
extern int UI_TargetObjectAdditionalInfo;

bool ObjectManipulator3d::MouseLeftBtnUp ( POINT pt, int delta )
{
	m_bLeftBtnDown = false;

	bool bSelectionRectShown = m_bSelectionRectShow;
	m_bSelectionRectShow = false;
	switch ( m_ePickState )
	{
	case PICK_CLONE:
	case PICK_ELEMENT_PICKED:
		
		if ( m_bAttachMode && g_pTgtSnapObject && g_pHoldObject )
		{
			r3dPoint3D v = g_vTgtSnapPos - g_vOffsetHoldPos;
			g_pHoldObject->SetPosition( v );
		}

		if ( m_ePickState == PICK_ELEMENT_PICKED )
			ObjectActionEnd ( m_ePickElementSelected );
		m_ePickState = PICK_SELECTED;
		m_ePickElementSelected = ELEMENT_NONE;
		return true;
	case PICK_MOVE:
		m_ePickState = PICK_SELECTED;
		return true;

	case PICK_UNSELECTED:
	case PICK_SELECTED:

		if ( !imgui_IsCursorOver2d () && ! imgui2_IsCursorOver2d() && !m_bLocked && !( m_bLocal  && m_bLocalEnable ) )
		{
			float fMinX = 0.f;
			float fMaxX = 0.f;
			float fMinY = 0.f;
			float fMaxY = 0.f;

			if( bSelectionRectShown )
			{
				fMinX = float(r3dTL::Min ( m_tSelectionStartPt.x, m_tSelectionEndPt.x )) / r3dRenderer->ScreenW;
				fMaxX = float(r3dTL::Max ( m_tSelectionStartPt.x, m_tSelectionEndPt.x )) / r3dRenderer->ScreenW;
				fMinY = float(r3dTL::Min ( m_tSelectionStartPt.y, m_tSelectionEndPt.y )) / r3dRenderer->ScreenH;
				fMaxY = float(r3dTL::Max ( m_tSelectionStartPt.y, m_tSelectionEndPt.y )) / r3dRenderer->ScreenH;
			}

			if ( !bSelectionRectShown || fabs( fMaxX - fMinX ) < 0.015625 && fabs( fMaxY - fMinY ) < 0.015625 )
			{
				// add objects to selection
				if ( m_bClone && m_ePickState == PICK_SELECTED )
				{
					extern gobjid_t	UI_TargetObjID;

					GameObject* TargetObj = GameWorld().GetObject(UI_TargetObjID);
					if ( TargetObj )
					{
						if ( !Picker_IsPicked ( TargetObj ) && !Picker_IsPickedAsPartOfPerfab( TargetObj ) )
						{
							Picker_Pick ( TargetObj, false );
							TargetObj->OnPickerPick(UI_TargetObjectAdditionalInfo);
						}
						else
						{
							Picker_Drop ( TargetObj );
							if (TargetObj->ownerPrefab)
								Picker_Drop(TargetObj->ownerPrefab);
						}
					}
				}
				else
				{
					// select object if no selection at all
					GameObject * pNewPicked = NULL;
					for ( uint32_t i = 0; i < m_dObjects.Count (); i++ )
					{
						if ( Picker_IsPick (m_dObjects[i], pt ) )
						{
							pNewPicked = m_dObjects[i];
							break;
						}
					}

					if ( pNewPicked )
					{
						Picker_Pick ( pNewPicked, !(  Keyboard->IsPressed(kbsLeftShift) || Keyboard->IsPressed(kbsRightShift) ) );
						pNewPicked->OnPickerPick(UI_TargetObjectAdditionalInfo);
						m_PickPoint = pt;
					}
					else
					{
						if ( Picker_IsPicked() )
						{
							// clicked on free space - drop selection		
							Picker_Drop();
						}
					}
				}

				if( m_SelectCallback )
					m_SelectCallback( PST_SINGLE, pt.x, pt.x, pt.y, pt.y ) ;
			}
			else
			{
				// select objects in rect
				
				Picker_Drop();

				for ( uint32_t i = 0; i < m_dObjects.Count (); i++ )
				{
					r3dPoint3D vPos = m_dObjects[i]->GetBBoxWorld().Org + m_dObjects[i]->GetBBoxWorld().Size * 0.5f;

					if ( ( m_dObjects[i]->ObjFlags & OBJFLAG_SkipCastRay ))
						continue;

					if ( m_sTypeFilter.c_str() && m_sTypeFilter.c_str()[0] )
					{
						if ( ( m_dObjects[i]->Class->Name != m_sTypeFilter.c_str () ) )
							continue;
					}

					D3DXMATRIX mFinal1 = r3dRenderer->ViewMatrix * r3dRenderer->ProjMatrix;
					D3DXVECTOR4 vRes;
					D3DXVec3Transform ( &vRes, vPos.d3dx(), &mFinal1 ); 
					vRes.x /= vRes.w;
					vRes.y /= vRes.w;
					vRes.z /= vRes.w;

					vRes.x = (+vRes.x) * 0.5f + 0.5f;
					vRes.y = (-vRes.y) * 0.5f + 0.5f;

					if ( vRes.z > 0.0f && vRes.z < 1.0f && fMinX < vRes.x && vRes.x < fMaxX && 
						fMinY < vRes.y && vRes.y < fMaxY )
					{
						Picker_Pick ( m_dObjects[i], false );
						m_dObjects[i]->OnPickerPick(0);
					}
				}

				if( m_SelectCallback )
					m_SelectCallback( PST_RECT, m_tSelectionStartPt.x, m_tSelectionEndPt.x, m_tSelectionStartPt.y, m_tSelectionEndPt.y ) ;
			}
		}

		return true;
	};

	return false;
}

bool ObjectManipulator3d::MouseRightBtnDown ( POINT pt, int delta )
{
	return false;
}

bool ObjectManipulator3d::MouseRightBtnUp ( POINT pt, int delta )
{
	return false;
}

bool ObjectManipulator3d::MouseMiddleBtnDown ( POINT pt, int delta )
{
	return false;
}

bool ObjectManipulator3d::MouseMiddleBtnUp ( POINT pt, int delta )
{
	switch ( m_ePickState )
	{
	case PICK_RESIZE:
		m_ePickState = PICK_SELECTED;
		return true;
	}
	return false;
}

bool ObjectManipulator3d::MouseMove ( POINT pt, int delta )
{
	m_tSelectionEndPt = pt;

	if ( ( m_ePickState == PICK_UNSELECTED || m_ePickState == PICK_SELECTED ) && !m_bLocked )
	{
		if ( ! m_bAttachMode && m_bLeftBtnDown && !( abs ( m_tSelectionStartPt.x - m_tSelectionEndPt.y ) < m_fSelectionRectTreshold && abs ( m_tSelectionStartPt.y - m_tSelectionEndPt.y ) < m_fSelectionRectTreshold ) )
			m_bSelectionRectShow = true;
	}

	switch ( m_ePickState )
	{
	case PICK_SELECTED:
		if ( m_dPickedObjects.Count() > 0 )
		{
			m_ePickAxisHighlighted = Picker_PickElement( m_dPickedObjects[0].pObject, pt, m_bLocal && m_bLocalEnable );
			return true;
		}
		break;
	case PICK_ROTATE:
		{
		}
		break;
	case PICK_PAN:
		{

		}
		break;
	case PICK_RESIZE:
		{

		}
		break;
	case PICK_CLONE:
		{
			r3dTL::TArray < GameObject * > tCloned;
			r3dTL::TArray < GameObject * > tOriginal;
			for ( uint32_t i = 0; i < m_dPickedObjects.Count (); i++ )
			{
				GameObject *go = m_dPickedObjects[i].pObject;
				//	Skip lights that attached to light mesh
				GameObject *owner = GameWorld().GetObject(go->ownerID);
				if (owner && owner->Class->Name == "obj_LightMesh" && Picker_IsPicked(owner))
				{
					continue;
				}

				GameObject * pObj = go->Clone();
				tOriginal.PushBack ( go );
				tCloned.PushBack ( pObj );
			}

			Picker_Drop();

			for ( uint32_t i = 0; i < tCloned.Count (); i++ )
			{
				Picker_Pick( tCloned[i] ? tCloned[i] : tOriginal[i], false );
				m_dPickedObjects[i].bCloning = true;
			}
			
			m_ePickState = PICK_ELEMENT_PICKED;
			ObjectActionBegin ( m_ePickElementSelected );
		}
	case PICK_ELEMENT_PICKED:
		{
			if ( m_dPickedObjects.Count () == 0 )
				return false;

			uint32_t pickedObjIdx = 0;
			for (; pickedObjIdx < m_dPickedObjects.Count() && g_pTgtSnapObject; ++pickedObjIdx)
			{
				if (m_dPickedObjects[pickedObjIdx].pObject == g_pTgtSnapObject)
					break;
			}
			if (pickedObjIdx == m_dPickedObjects.Count())
				pickedObjIdx = 0;

			SelectionObj_t &p = m_dPickedObjects[0];

			r3dPoint3D vLastPos = p.pObject->GetPosition ();
			r3dPoint3D vLastPosCenter = GetSelectionCenter ();
			D3DXMATRIX mLastRotation = p.pObject->GetRotationMatrix ();
			r3dPoint3D vLastRotationVector = p.pObject->GetRotationVector ();
			D3DXMATRIX mLastRotationInv; 
			D3DXMatrixInverse ( &mLastRotationInv, NULL, &mLastRotation );
			if ( Picker_MoveAxis ( p.pObject, m_ePickElementSelected, m_PickPoint, pt, m_bLocal && m_bLocalEnable ) )
			{
				r3dPoint3D vOffset = p.pObject->GetPosition () - vLastPos;
				D3DXMATRIX mRotation = p.pObject->GetRotationMatrix ();

				D3DXMATRIX mRotAddit = mLastRotationInv * mRotation;

				p.pObject->SetPosition(vLastPos);
				p.pObject->SetRotationVector ( vLastRotationVector );

				for ( uint32_t k = 0; k < m_dPickedObjects.Count (); k++ )
				{
					D3DXMATRIX mRot = m_dPickedObjects[k].pObject->GetRotationMatrix ();
					mRot = mRot * mRotAddit; 
					r3dVector vRot;
					MatrixGetYawPitchRoll(mRot, vRot.x, vRot.y, vRot.z );
					NormalizeYPR ( vRot );
					r3dPoint3D vOffsetRot;
					r3dPoint3D vOffsetObj = m_dPickedObjects[k].pObject->GetPosition () - vLastPosCenter;
					D3DXVec3TransformCoord(vOffsetRot.d3dx(), vOffsetObj.d3dx(), &mRotAddit );
					vOffsetRot = vOffsetRot - vOffsetObj;
					vRot.x = R3D_RAD2DEG ( vRot.x ); vRot.y = R3D_RAD2DEG ( vRot.y ); vRot.z = R3D_RAD2DEG ( vRot.z );
					m_dPickedObjects[k].pObject->SetPosition ( m_dPickedObjects[k].pObject->GetPosition () + vOffset + vOffsetRot );
					m_dPickedObjects[k].pObject->SetRotationVector( vRot );
				}
				
				m_PickPoint = pt;
			}
			return true;
		}
		break;
	case PICK_MOVE:
		{
			r3d_assert ( m_dPickedObjects.Count () > 0 );
			r3dVector vMove = Picker_Move ( g_pTgtSnapObject ? g_pTgtSnapObject : m_dPickedObjects[0].pObject, m_PickPoint, pt );
			m_PickPoint = pt;
			return true;
		}
		break;
	};

	return false;
}

void ObjectManipulator3d::ObjectActionBegin ( ControlElement_e eElem )
{
	r3d_assert ( m_dPickedObjects.Count() > 0 );
	m_dUndoInfo.Clear();

	if ( !m_bLocked && ! m_bAttachMode )
	{
		for ( uint32_t i = 0; i < m_dPickedObjects.Count (); i++ )
			m_dPickedObjects[i].pObject->SetObjFlags(OBJFLAG_SkipCastRay);
	}
	
	if( m_UndoEnabled )
	{
		for ( uint32_t i = 0; i < m_dPickedObjects.Count (); i++ )
		{
			EntInfo_t tInfo;
			tInfo.objHashID = m_dPickedObjects[i].pObject->GetHashID();
			tInfo.vPos = m_dPickedObjects[i].pObject->GetPosition();
			tInfo.vRot = m_dPickedObjects[i].pObject->GetRotationVector();
			tInfo.vScl = m_dPickedObjects[i].pObject->GetScale();
			m_dUndoInfo.PushBack(tInfo);
		}
	}
}

void ObjectManipulator3d::ObjectActionEnd ( ControlElement_e eElem )
{
	r3d_assert ( m_dPickedObjects.Count() > 0 );
	r3d_assert ( !m_UndoEnabled || m_dUndoInfo.Count() == m_dPickedObjects.Count() );

	for ( uint32_t i = 0; i < m_dPickedObjects.Count(); i++ )
	{
		SelectionObj_t &o = m_dPickedObjects[i];
		if ( !o.bOldSkipCastRay )
			o.pObject->ResetObjFlags(OBJFLAG_SkipCastRay);
		o.pObject->OnPickerActionEnd();
	}

	bool bAnyCloning = false;

	if( m_CloneEnabled )
	{
		for ( uint32_t i = 0; i < m_dPickedObjects.Count (); i++ )
		{
			if ( m_dPickedObjects[i].bCloning )
			{
				bAnyCloning = true;
				break;
			}
		}
	}

	UndoEntityAddDel * pUndoArray = NULL;
	if ( bAnyCloning && m_UndoEnabled && m_CloneEnabled )
		pUndoArray = ( UndoEntityAddDel * ) g_pUndoHistory->CreateUndoItem( UA_ENT_ADDDEL );

	if( m_CloneEnabled )
	{
		for ( uint32_t i = 0; i < m_dPickedObjects.Count (); i++ )
		{
			r3d_assert( m_dUndoInfo[i].objHashID == m_dPickedObjects[i].pObject->GetHashID() );
		
			if (m_dPickedObjects[i].bCloning )
			{
				UndoEntityAddDel * pUndo = pUndoArray;
				if ( !pUndo )
					pUndo = ( UndoEntityAddDel * ) g_pUndoHistory->CreateUndoItem( UA_ENT_ADDDEL );
		
				assert( pUndo );

				if ( pUndo )
				{
					EntAddDel_t st;

					st.bDelete = false;
					st.pEnt = m_dPickedObjects[i].pObject;

					if ( pUndoArray )
						pUndo->AddItem ( st );
					else
						pUndo->Create( st );
				}
				m_bCloning = false;
			}
		}
	}

	if(m_dPickedObjects.Count()==1)
	{
		if( m_UndoEnabled && !m_dPickedObjects[ 0 ].bCloning  )
		{
			UndoEntityChanged * pUndo = ( UndoEntityChanged * ) g_pUndoHistory->CreateUndoItem( UA_ENT_CHANGED );
			r3d_assert( pUndo );
			if ( pUndo )
			{
				EntInfo_t st_new;

				st_new.objHashID = m_dPickedObjects[0].pObject->GetHashID();
				st_new.vRot = m_dPickedObjects[0].pObject->GetRotationVector();
				st_new.vScl = m_dPickedObjects[0].pObject->GetScale();
				st_new.vPos = m_dPickedObjects[0].pObject->GetPosition();

				pUndo->Create( m_dUndoInfo[0], st_new );
			}
		}
	}
	else
	{
		if( m_UndoEnabled && m_dPickedObjects.Count() > 1 && !m_dPickedObjects[ 0 ].bCloning )
		{
			UndoEntityGroupChanged * pUndo = ( UndoEntityGroupChanged * ) g_pUndoHistory->CreateUndoItem( UA_ENT_GROUP_CHANGED );

			r3d_assert( pUndo );
			if ( pUndo )
			{
				pUndo->Create(m_dPickedObjects.Count());
				for(uint32_t i=0; i<m_dPickedObjects.Count(); ++i)
				{
					EntInfo_t st_new;

					st_new.objHashID = m_dPickedObjects[i].pObject->GetHashID();
					st_new.vRot = m_dPickedObjects[i].pObject->GetRotationVector();
					st_new.vScl = m_dPickedObjects[i].pObject->GetScale();
					st_new.vPos = m_dPickedObjects[i].pObject->GetPosition();

					pUndo->SetItem( i, m_dUndoInfo[i], st_new );
				}
			}
		}
	}

	if ( pUndoArray )
		pUndoArray->Create ();
}

void ObjectManipulator3d::GetPickedObjects( r3dTL::TArray < GameObject* >* oSelected )
{
	oSelected->Clear() ;

	for( int i = 0, e = m_dPickedObjects.Count() ; i < e ; i ++ )
	{
		oSelected->PushBack( m_dPickedObjects[ i ].pObject ) ;
	}
}

bool ObjectManipulator3d::IsSelected( GameObject* pObject )
{
	for ( uint32_t i = 0; i < m_dPickedObjects.Count(); i++ )
	{
		if ( m_dPickedObjects[i].pObject == pObject )
			return true;
	}
	return false;
}

void ObjectManipulator3d::ObjectRegister ( GameObject * pObject )
{
	r3d_assert(pObject);

	if ( ObjectFind ( pObject ) )
		return;

	m_dObjects.PushBack ( pObject );
}

void ObjectManipulator3d::ObjectUnregister ( GameObject * pObject )
{
	r3d_assert(pObject);

	Picker_Drop ( pObject );

	for ( uint32_t i = 0; i < m_dObjects.Count (); i++ )
	{
		if ( m_dObjects[i] == pObject )
		{
			m_dObjects.Erase ( i );
			break;
		}
	}
}

bool ObjectManipulator3d::ObjectFind ( GameObject * pObject )
{
	r3d_assert(pObject);
	for ( uint32_t i = 0; i < m_dObjects.Count (); i++ )
	{
		if ( m_dObjects[i] == pObject )
			return true;
	}

	return false;
}


ObjectManipulator3d g_Manipulator3d;


#endif // FINAL_BUILD