//=========================================================================
//	Module: EditedValueTracker.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once

#include "../../../Eternity/UndoHistory/UndoHistory.h"
#include "TimeGradient.h"

//////////////////////////////////////////////////////////////////////////

template<typename T>
class ValueTracker;

//////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
//	Value trackers tag definition. Should be defined separately for each
//	bit in 32 bit int
//-------------------------------------------------------------------------
enum TrackerTags
{
	TT_PARTICLE_EMITTER_VARS = (1 << 0),
	TT_ALL = 0xffffffff
};

//-------------------------------------------------------------------------
//	Value trackers manager. Simply update them all in one place.
//-------------------------------------------------------------------------

class IValueTracker
{
public:
	IValueTracker();
	explicit IValueTracker(uint32_t tagValue);
	virtual void Update() = 0;

	/**	Used for update masking, and selective undo actions removal. */
	uint32_t tag;
};

class ValueTrackersManager
{
	r3dTL::TArray<IValueTracker*> trackers;
	uint32_t updateMask;

public:
	ValueTrackersManager();
	void Update();
	void Register(IValueTracker *v);
	static ValueTrackersManager & Instance();
	void SetUpdateMask(uint32_t mask) { updateMask = mask; }
	uint32_t GetUpdateMask() const { return updateMask; }
};

//-------------------------------------------------------------------------
//	Undo/redo classes
//-------------------------------------------------------------------------

template <typename T>
class ValueChangeUndo: public IUndoItem
{
	T *trackedVar;
	T prevValue;
	T curValue;
	ValueTracker<T> *tracker;

	static UndoAction_e		ActionID();

public:
	ValueChangeUndo()
	: trackedVar(0)
	, prevValue(T())
	, curValue(T())
	, tracker(0)
	{
		m_sTitle = "Value change";
	}

	virtual void			Release			() { delete this; }
	virtual	UndoAction_e	GetActionID		() { return ActionID(); }

	virtual void			Undo			();
	virtual void			Redo			();

	void					SetValue		(T *tv, T pv)
	{
		trackedVar = tv;
		prevValue = pv;
		if (trackedVar)
			curValue = *trackedVar;
	}

	void					SetTracker		(ValueTracker<T> *t)
	{
		tracker = t;
	}

	static IUndoItem * CreateUndoItem	()
	{
		return gfx_new ValueChangeUndo<T>;
	};

	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ActionID();
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction( action );
	}
};

//-------------------------------------------------------------------------
//	Texture material change undo
//-------------------------------------------------------------------------

class MaterialTextureChangeUndo: public IUndoItem
{
	static const UndoAction_e ms_eActionID = UA_MATERIAL_TEXTURE_CHANGE;
	
	r3dTexture*				tex;
	r3dMaterial*			mat;
	r3dMaterial::TexType	texType;

	r3dString prevTexPath;
	r3dString curTexPath;

public:
	MaterialTextureChangeUndo()
	: tex(0)
	{
		m_sTitle = "Texture change";
	}

	virtual void			Release			() { delete this; }
	virtual	UndoAction_e	GetActionID		() { return ms_eActionID; }

	virtual void			Undo			();
	virtual void			Redo			();

	void					Do( const r3dString& path );

	void					SetData( r3dMaterial* m, r3dMaterial::TexType aTexType, r3dTexture *t, const char *prevPath, const char *curPath);

	static IUndoItem * CreateUndoItem	()
	{
		return gfx_new MaterialTextureChangeUndo;
	};

	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction( action );
	}
};

//-------------------------------------------------------------------------
//	Undo actions creator
//-------------------------------------------------------------------------

template <typename T>
ValueChangeUndo<T> * CreateUndoItem()
{
	Unsupported_type_Write_type_specialized_version_of_this_function;
};

template <>
ValueChangeUndo<int> * CreateUndoItem();

template <>
ValueChangeUndo<r3dColor> * CreateUndoItem();

template <>
ValueChangeUndo<float> * CreateUndoItem();

template <>
ValueChangeUndo<bool> * CreateUndoItem();

template <>
ValueChangeUndo<r3dString> * CreateUndoItem();

template <>
ValueChangeUndo<r3dTimeGradient2> * CreateUndoItem();

template <>
ValueChangeUndo<r3dVector> * CreateUndoItem();

//-------------------------------------------------------------------------
//	Value tracker
//-------------------------------------------------------------------------

template<typename T>
class ValueTracker : public IValueTracker
{
public:
	typedef void (*ValueChangeCallback)(T &val, ValueTracker<T> &this_);

private:
	T* trackedValue;
	T originalValue;
	ValueChangeCallback vcb;
	
public:
	ValueTracker()
	: originalValue(T())
	, trackedValue(0)
	, vcb(0)
	{
	}

	explicit ValueTracker(uint32_t tag)
	: IValueTracker(tag)
	, originalValue(T())
	, trackedValue(0)
	, vcb(0)
	{
	}

	/**	Init tracker with watched value. */
	void Init(T &val)
	{
		trackedValue = &val;
		originalValue = val;
	}

	/**	Check value for modifications of watched value, and if one is found, create undo record. */
	void Update()
	{
		if (!trackedValue)
			return;

		if (*trackedValue != originalValue && !Mouse->IsPressed(r3dMouse::mLeftButton))
		{
			typedef ValueChangeUndo<T> UndoType;
			UndoType * pUndo = CreateUndoItem<T>();
			if (pUndo)
			{
				pUndo->SetTag(tag);
				pUndo->SetValue(trackedValue, originalValue);
				pUndo->SetTracker(this);
			}

			originalValue = *trackedValue;
		}
	}

	/**	Overwrite original value. This is needed for preventing undo item creation when performing undo action. */
	void OverwriteOriginalValue(T v)
	{
		originalValue = v;
		if (vcb) vcb(originalValue, *this);
	}

	/**	Set callback that will be called when undo or redo action was performed. */
	void SetOnUndoCallback(ValueChangeCallback v)
	{
		vcb = v;
	}

	/**	Get tracked value. */
	T & GetTrackedValue() { return *trackedValue; }
};

//-------------------------------------------------------------------------
//	ValueChangeUndo Impl section
//-------------------------------------------------------------------------

template <typename T>
void ValueChangeUndo<T>::Undo()
{
	if (trackedVar)
	{
		*trackedVar = prevValue;
		if (tracker)
			tracker->OverwriteOriginalValue(prevValue);
	}
}

//////////////////////////////////////////////////////////////////////////

template <typename T>
void ValueChangeUndo<T>::Redo()
{
	if (trackedVar)
	{
		*trackedVar = curValue;
		if (tracker)
			tracker->OverwriteOriginalValue(curValue);
	}
}

//////////////////////////////////////////////////////////////////////////

template <typename T>
UndoAction_e ValueChangeUndo<T>::ActionID()
{
	Unsupported_type_Write_type_specialized_version_of_this_function;
}

template <>
UndoAction_e ValueChangeUndo<float>::ActionID();

template <>
UndoAction_e ValueChangeUndo<int>::ActionID();

template <>
UndoAction_e ValueChangeUndo<bool>::ActionID();

template <>
UndoAction_e ValueChangeUndo<r3dColor>::ActionID();

template <>
UndoAction_e ValueChangeUndo<r3dString>::ActionID();

template <>
UndoAction_e ValueChangeUndo<r3dTimeGradient2>::ActionID();

template <>
UndoAction_e ValueChangeUndo<r3dVector>::ActionID();

//////////////////////////////////////////////////////////////////////////
