/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY

namespace Kaim
{


//------------------------------------------------------------
// DenseGridAndArrayOfRawPtr<T>

template<class T>
void DenseGridAndArrayOfRawPtr<T>::ClearAndRelease()
{
	m_grid.ClearAndRelease();

	for (KyUInt32 i = 0; i < m_array.GetCount(); ++i)
		delete m_array[i];
	m_array.ClearAndRelease();
}


template<class T>
T* DenseGridAndArrayOfRawPtr<T>::GetOrCreate(KyInt32 x, KyInt32 y)
{
	T* existingElement = m_grid.Get(x, y);
	if (existingElement != KY_NULL)
		return existingElement;

	T* newElement = KY_NEW T(x, y);
	m_grid.Set(x, y, newElement);
	m_array.PushBack(newElement);
	return newElement;
}


//------------------------------------------------------------
// DenseGridAndArrayOfRefPtr<T>

template<class T>
void DenseGridAndArrayOfRefPtr<T>::ClearAndRelease()
{
	m_grid.ClearAndRelease();
	m_array.ClearAndRelease();
}


template<class T>
Ptr<T> DenseGridAndArrayOfRefPtr<T>::GetOrCreate(KyInt32 x, KyInt32 y)
{
	Ptr<T> existingElement = m_grid.Get(x, y);
	if (existingElement != KY_NULL)
		return existingElement;

	Ptr<T> newElement = *KY_NEW T(x, y);
	m_grid.Set(x, y, newElement);
	m_array.PushBack(newElement);
	return newElement;
}


//------------------------------------------------------------
// DenseGrid<T>

template<class T>
void DenseGrid<T>::ClearAndRelease()
{
	m_root_x = KyInt32MAXVAL;

	for (KyUInt32 i = 0; i < m_plus_x.GetCount(); ++i)
		delete m_plus_x[i];
	m_plus_x.ClearAndRelease();

	for (KyUInt32 i = 0; i < m_minus_x.GetCount(); ++i)
		delete m_minus_x[i];
	m_minus_x.ClearAndRelease();
}

template<class T>
T DenseGrid<T>::Column::Get(KyInt32 y, const T& noneValue) const
{
	if (m_root_y == KyInt32MAXVAL)
	{
		return noneValue;
	}
	else if (y >= m_root_y)
	{
		KyInt32 idx = y - m_root_y;
		KyInt32 size = (KyInt32)m_plus_y.GetCount();
		return idx < size ? m_plus_y[idx] : noneValue;
	}
	else
	{
		KyInt32 idx = m_root_y - y - 1;
		KyInt32 size = (KyInt32)m_minus_y.GetCount();
		return idx < size ? m_minus_y[idx] : noneValue;
	}
}


template<class T>
T DenseGrid<T>::Get(KyInt32 x, KyInt32 y) const
{
	if (m_root_x == KyInt32MAXVAL)
	{
		return m_noneValue;
	}
	else if (x >= m_root_x)
	{
		KyInt32 idx = x - m_root_x;
		KyInt32 size = (KyInt32)m_plus_x.GetCount();
		if (idx >= size || m_plus_x[idx] == KY_NULL)
			return m_noneValue;
		return m_plus_x[idx]->Get(y, m_noneValue);
	}
	else
	{
		KyInt32 idx = m_root_x - x - 1;
		KyInt32 size = (KyInt32)m_minus_x.GetCount();
		if (idx >= size || m_minus_x[idx] == KY_NULL)
			return m_noneValue;
		return m_minus_x[idx]->Get(y, m_noneValue);
	}
}


template<class T>
void DenseGrid<T>::Column::Set(KyInt32 y, const T& value, const T& noneValue)
{
	if (m_root_y == KyInt32MAXVAL)
	{
		m_root_y = y;
		m_plus_y.PushBack(value);
	}
	else if (y >= m_root_y)
	{
		KyInt32 idx = y - m_root_y;
		KyInt32 size = (KyInt32)m_plus_y.GetCount();
		for (KyInt32 i = size; i <= idx; ++i)
			m_plus_y.PushBack(noneValue);
		m_plus_y[idx] = value;
	}
	else
	{
		KyInt32 idx = m_root_y - y - 1;
		KyInt32 size = (KyInt32)m_minus_y.GetCount();
		for (KyInt32 i = size; i <= idx; ++i)
			m_minus_y.PushBack(noneValue);
		m_minus_y[idx] = value;
	}
}


template<class T>
void DenseGrid<T>::Set(KyInt32 x, KyInt32 y, const T& value)
{
	if (m_root_x == KyInt32MAXVAL)
	{
		m_root_x = x;
		m_plus_x.PushBack(KY_NEW Column(x));
		return m_plus_x[0]->Set(y, value, m_noneValue);
	}
	else if (x >= m_root_x)
	{
		KyInt32 idx = x - m_root_x;
		KyInt32 size = m_plus_x.GetCount();

		for (KyInt32 i = size; i <= idx; ++i)
			m_plus_x.PushBack(KY_NULL);

		if (m_plus_x[idx] == KY_NULL)
			m_plus_x[idx] = KY_NEW Column(x);

		return m_plus_x[idx]->Set(y, value, m_noneValue);
	}
	else
	{
		KyInt32 idx = m_root_x - x - 1;
		KyInt32 size = m_minus_x.GetCount();

		for (KyInt32 i = size; i <= idx; ++i)
			m_minus_x.PushBack(KY_NULL);

		if (m_minus_x[idx] == KY_NULL)
			m_minus_x[idx] = KY_NEW Column(x);

		return m_minus_x[idx]->Set(y, value, m_noneValue);
	}
}


}


