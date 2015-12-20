/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_DenseGrid_H
#define GwNavGen_DenseGrid_H


#include "gwnavruntime/math/box2i.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_RefCount.h"


namespace Kaim
{


/// Container that spatialize elements in a 2d grid. Optimized for grids thats are dense ie more or less in the same are.
/// DenseGrid is constructed with a noneValue. Get(x,y) returns noneValue when no value was set with Set(x,y).
template<class T>
class DenseGrid
{
public:
	class Column
	{
		KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
	public:
		Column() : m_x(KyInt32MAXVAL), m_root_y(KyInt32MAXVAL) {}
		Column(KyInt32 x) : m_x(x), m_root_y(KyInt32MAXVAL) {}
		void Set(KyInt32 y, const T& value, const T& noneValue);
		T Get(KyInt32 y, const T& noneValue) const;

	public:
		KyInt32 m_x;
		KyInt32 m_root_y;
		KyArray<T> m_plus_y;  //          [root_y..[
		KyArray<T> m_minus_y; // ]..root_y[
	};

public:
	DenseGrid() : m_root_x(KyInt32MAXVAL), m_noneValue() {}
	DenseGrid(const T& noneValue) : m_root_x(KyInt32MAXVAL), m_noneValue(noneValue) {}
	~DenseGrid() { ClearAndRelease(); }
	void ClearAndRelease();
	void Set(KyInt32 x, KyInt32 y, const T& value);
	T Get(KyInt32 x, KyInt32 y) const;

private:
	KyInt32 m_root_x;
	KyArrayPOD<Column*> m_plus_x;  //          [root_x..]
	KyArrayPOD<Column*> m_minus_x; // ]..root_x[
	T m_noneValue;
	friend class DynamicGridColumn;
};


template<class T>
class DenseGridAndArrayOfRawPtr
{
public:
	DenseGridAndArrayOfRawPtr() : m_grid(KY_NULL) {}
	~DenseGridAndArrayOfRawPtr() { ClearAndRelease(); }
	void ClearAndRelease();
	T* GetOrCreate(KyInt32 x, KyInt32 y);
	T* Get(KyInt32 x, KyInt32 y) const { return m_grid.Get(x, y); }
	const KyArrayPOD<T*>& GetArray() const { return m_array; }
	T** GetElements() { return m_array.GetDataPtr(); }
	KyUInt32 GetElementsCount() const { return m_array.GetCount(); }

private:
	DenseGrid<T*> m_grid;
	KyArrayPOD<T*> m_array;
};


template<class T>
class DenseGridAndArrayOfRefPtr
{
public:
	DenseGridAndArrayOfRefPtr() : m_grid(KY_NULL) {}
	~DenseGridAndArrayOfRefPtr() { ClearAndRelease(); }
	void ClearAndRelease();
	Ptr<T> GetOrCreate(KyInt32 x, KyInt32 y);
	Ptr<T> Get(KyInt32 x, KyInt32 y) const { return m_grid.Get(x, y); }
	const KyArray<Ptr<T> >& GetArray() const { return m_array; }
	Ptr<T>* GetElements() { return m_array.GetDataPtr(); }
	KyUInt32 GetElementsCount() const { return m_array.GetCount(); }

private:
	DenseGrid<Ptr<T> > m_grid;
	KyArray<Ptr<T> > m_array;
};


}


#include "gwnavgeneration/common/densegrid.inl"


#endif
