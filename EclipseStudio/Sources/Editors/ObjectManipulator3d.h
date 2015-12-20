#pragma once 
#include "r3dPCH.h"
#include "r3d.h"
#include "r3dPoint.h"

class ObjectManipulator3d;
class Coord3dObject;
class GameObject;

#include "LevelEditor.h"

enum PickerSelectType
{
	PST_SINGLE,
	PST_RECT
};

typedef void (*SelectCallback )( PickerSelectType, int, int, int, int ) ;
typedef void (*DropCallback)() ;

class ObjectManipulator3d
{
public:
	ObjectManipulator3d ();
	~ObjectManipulator3d ();

	void			Init();
	void			Close();

	void			AddImmortal( GameObject* pObject ) ;
	int				IsImmortal( GameObject* pObject ) ;
	void			RemoveImmortal( GameObject* pObject ) ;

	bool			IsSelected( GameObject* pObject );

	void			ObjectRegister ( GameObject * pObject );
	void			ObjectUnregister ( GameObject * pObject );
	bool			ObjectFind ( GameObject * pObject );

	void			Draw ();
	void			Update ();

	bool			IsEnable () const;
	void			Enable ();
	void			Disable ( bool dropPicked = true );

	bool			IsLocked () const;
	void			Lock ();
	void			Unlock ();

	void			ScaleDisable ();
	void			ScaleEnable ();

	void			TypeFilterSet ( const char * sFilter );
	GameObject *	PickedObject () const;
	void			PickedObjectSet ( GameObject * pObj );

	void			PickerAddToPicked ( GameObject * pObj, bool pushFront = false );
	void			PickerRemoveFromPicked ( GameObject * pObj );
	void			PickerResetPicked ();

	void			DeletePickedObjects ();

	bool			MouseMessage ( UINT message, POINT pt, int delta );
	bool			KeyboardMessageKeyUp	( int vik );
	bool			KeyboardMessageKeyDown	( int vik );

	void			SetAngleSnapping ( bool bEnable, float fValInDegree );
	void			SetMoveSnapping ( bool bEnable, float fVal );

	GameObject *	PickedObjectGet ( int i );	
	uint32_t		PickedObjectCount () const;

	const AClass*	GetPickedMostDerivedClass() const;

	void			GetSelectionRect( RECT* oRect ) const;
	void			SetSelectCallback( SelectCallback scb );
	void			SetDropCallback( DropCallback dcb );

	SelectCallback	GetSelectCallback();
	DropCallback	GetDropCallback();

	const r3dString& GetTypeFilter() const;

	void			SetUndoEnabled( bool enabled );
	bool			IsUndoEnabled() const;

	void			SetCloneEnabled( bool enabled );
	
private:

	enum PickState_e
	{ 
		PICK_ELEMENT_PICKED = 1
		, PICK_SELECTED
		, PICK_UNSELECTED
		, PICK_PAN
		, PICK_ROTATE
		, PICK_MOVE
		, PICK_CLONE
		, PICK_RESIZE
		, PICK_ATTACH_POINT
	};

	enum ControlElement_e
	{
		ELEMENT_X_MOVE			///< X axis
		,ELEMENT_Y_MOVE			///< Y axis
		,ELEMENT_Z_MOVE			///< Z axis
		,ELEMENT_XZ_SNAP_MOVE	///< XY plane
		,ELEMENT_YZ_MOVE		///< YZ plane
		,ELEMENT_XZ_MOVE		///< XZ plane
		,ELEMENT_XY_MOVE		///< XY plane
		,ELEMENT_X_RESIZE		///< X sphere
		,ELEMENT_Y_RESIZE		///< Y sphere
		,ELEMENT_Z_RESIZE		///< Z sphere
		,ELEMENT_XYZ_RESIZE 	///< XYZ sphere
		,ELEMENT_XY_ROTATE		///< XY circle
		,ELEMENT_YZ_ROTATE		///< YZ circle
		,ELEMENT_ZX_ROTATE		///< ZX circle
		,ELEMENT_ATTACH_VERT	///< ZX circle
		,ELEMENT_NONE
	};

public:
	
	

	r3dVector			GetSelectionCenter () const;

	void				ToggleSnapVertexes()		{ m_bAttachMode = !m_bAttachMode; }
	void				SetSnapVetexes(bool doSnap)	{ m_bAttachMode = doSnap; }
	bool				IsSnapVertexes()			{ return m_bAttachMode; }

	void				GetPickedObjects( r3dTL::TArray < GameObject* >* oSelected ) ;

	void				PickByName			( const char* Name, int bExactMatch );
	bool				IsObjectPicked(GameObject *go) { return Picker_IsPicked(go); }
	bool				IsObjectPickedAsPartOfPrefab(GameObject *go) { return Picker_IsPickedAsPartOfPerfab(go); }

private:

	bool				m_bAttachMode;

	r3dColor			GetElementColor ( ControlElement_e eElement ) const;
	r3dVector			GetAxis ( int iAxis ) const;
	bool				GetCylinderFor ( r3dCylinder & tCylinder, ControlElement_e eElement ) const;

	ControlElement_e	Picker_PickElement	( GameObject * pObj, POINT ptCursor, bool bLocal );	///< returns axis that was picked by the cursor
	bool				Picker_IsPick		( GameObject * pObj, POINT ptCursor ) const;	///< returns true if the object was picked by the cursor
	void				Picker_DrawPicked	( GameObject * pObj, ControlElement_e eElementt, bool bLocal ) const;
	r3dVector			Picker_Move			( GameObject * pObj, POINT PickPoint, POINT pt2 );
	r3dVector			Picker_Move_Plane	( GameObject * pObj, POINT PickPoint, POINT pt2, bool bLoca ); // move by plane
	bool				Picker_MoveAxis		( GameObject * pObj, ControlElement_e eElement, POINT PickPoint, POINT pt2, bool bLocal );
	bool				Picker_ElementMove	( GameObject * pObj, const r3dVector & deltaPos );
	bool				Picker_ElementSize  ( GameObject * pObj, const r3dVector & deltaSize, bool bLocal, bool bUniform );
	bool				Picker_ElementRotate( GameObject * pObj, float angleX, float angleY, float angleZ, bool bLocal );

	bool				Picker_IsPicked		( GameObject * pObj = NULL ) const;
	void				Picker_Pick			( GameObject * pObj, bool bSingle = true, bool pushFront = false );
	void				Picker_Drop			();
	void				Picker_Drop			( GameObject * pObj );
	bool				Picker_IsPickedAsPartOfPerfab(GameObject * pObj) const;

	bool				MouseLeftBtnDown	( POINT pt, int delta );
	bool				MouseLeftBtnUp		( POINT pt, int delta );
	bool				MouseRightBtnDown	( POINT pt, int delta );
	bool				MouseRightBtnUp		( POINT pt, int delta );
	bool				MouseMiddleBtnDown	( POINT pt, int delta );
	bool				MouseMiddleBtnUp	( POINT pt, int delta );
	bool				MouseMove			( POINT pt, int delta );

	void				ObjectActionBegin   ( ControlElement_e eElem );
	void				ObjectActionEnd   ( ControlElement_e eElem );

private:

	bool			m_bCloning;

	typedef r3dTL::TArray < GameObject * > Objects_t;

	// snapping info
	bool			m_bAngleSnapping;
	float			m_fAngleSnappingVal;
	
	bool			m_bMoveSnapping;
	float			m_fMoveSnappingVal;

	PickState_e		m_ePickState;
	ControlElement_e m_ePickElementSelected;
	ControlElement_e m_ePickAxisHighlighted;
	POINT			m_PickPoint;
	r3dPlane		m_MovePlane;
	r3dVector		m_MoveByPlaneOffset;

	POINT			m_tSelectionStartPt;
	POINT			m_tSelectionEndPt;
	bool			m_bSelectionRectShow;
	bool			m_bLeftBtnDown;

	int				m_iAngleAlignSteps;
	
	struct SelectionObj_t
	{
		GameObject * pObject;
		bool bOldSkipCastRay;

		bool bCloning;
	};

	r3dTL::TArray < SelectionObj_t > m_dPickedObjects;

	r3dTL::TArray<EntInfo_t>		m_dUndoInfo;

	Objects_t		m_dObjects;
	bool			m_bEnabled;
	bool			m_bScaleEnable;
	bool			m_bLocalEnable;
	bool			m_bLocked;
	bool			m_bLocal;
	bool			m_bClone;
	bool			m_bFreeMove;
	r3dString		m_sTypeFilter;

	Objects_t		m_ImmortalObjects;

	SelectCallback	m_SelectCallback;
	DropCallback	m_DropCallback;

	bool			m_UndoEnabled;
	bool			m_CloneEnabled;

} extern g_Manipulator3d;

bool GeomRayPlaneIntersection ( const r3dRay & ray, r3dPlane Plane, r3dVector * pPos );
r3dRay Picker_GetClickRay ( POINT ptCursor );
