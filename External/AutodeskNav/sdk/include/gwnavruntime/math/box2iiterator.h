/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Box2iIterator_H
#define Navigation_Box2iIterator_H

#include "gwnavruntime/math/box2i.h"


namespace Kaim
{

// RowMajorIterator
// GetRowMajorIndex(const Vec2i& pos) const { return (pos.y - m_min.y) * CountX() + pos.x - m_min.x; }
class Box2iRowMajorIterator
{
	const Box2i* m_box;
	Vec2i m_pos;
	KyInt32 m_currentIdx;

public:
	KY_INLINE Box2iRowMajorIterator() : m_box(KY_NULL), m_pos(-KyInt32MAXVAL, -KyInt32MAXVAL), m_currentIdx(-1) {}
	KY_INLINE Box2iRowMajorIterator(const Box2i& box) : m_box(&box), m_pos(-KyInt32MAXVAL, -KyInt32MAXVAL), m_currentIdx(-1)
	{
		if (m_box != KY_NULL && box.IsValid())
		{
			m_pos = box.Min();
			m_currentIdx = 0;
		}
	}

	KY_INLINE bool operator==(const Box2iRowMajorIterator& it) const { return m_box == it.m_box && m_currentIdx == it.m_currentIdx; }
	KY_INLINE bool operator!=(const Box2iRowMajorIterator& it) const { return m_box != it.m_box || m_currentIdx != it.m_currentIdx; }

	KY_INLINE Box2iRowMajorIterator& operator++()
	{
		if (IsFinished() == false)
		{
			if (m_pos.x < m_box->Max().x)
			{
				++m_pos.x;
				++m_currentIdx;
			}
			else if (m_pos.y < m_box->Max().y)
			{
				m_pos.x = m_box->Min().x;
				++m_pos.y;
				++m_currentIdx;
			}
			else
			{
				m_currentIdx = -1;
			}
		}
		return *this;
	}

	KY_INLINE bool IsFinished() const { return m_currentIdx < 0 || m_box == KY_NULL; }

	KY_INLINE KyInt32 GetRowMajorIndex()  const { return m_currentIdx; }
	KY_INLINE const Vec2i& GetPos()       const { return m_pos;        }
};

// RowMajorIterator


/// browse a box that is included in a bigger box.
/// the row major index that is maintained refers to the bigger box
/// the CellPos remains in the inner box
/// \code
/// /\Y
/// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
/// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+--
/// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
/// +---+---#############################################---+--
/// |   |   # 55| 56| 57| 58| 59| 60| 61| 62| 63| 64| 65#   |
/// +---+---#---+---+---#########################---+---#---+--
/// |   |   # 44| 45|46 # 47| 48| 49| 50| 51|52 # 53| 54#   |
/// +---+---#---+---+---#---+---+---+---+---+---#---+---#---+--
/// |   |   # 33| 34|35 # 36| 37| 38| 39| 40|41 # 42| 43#   |
/// +---+---#---+---+---#---+---+---+---+---+---#---+---#---+--
/// |   |   # 22| 23|24 # 25| 26| 27| 28| 29|30 # 31| 32#   |
/// +---+---#---+---+---#########################---+---#---+--
/// |   |   # 11| 12| 13| 14| 15| 16| 17| 18| 19| 20| 21#   |
/// +---+---#---+---+---+---+---+---+---+---+---+---+---#---+--
/// |   |   # 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10#   |
/// +---+---#############################################---+--
/// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
/// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+-> X
/// Indices iteratively returned should be 25, 26, ... , 29, 30, 36, 37, ...., 
/// at each end of line in the innerBox we must jump (m_outerBox.CountX() - m_innerBox.CountX()) + 1

class Box2iIteratorInBiggerRowMajorBox
{
public:
	KY_INLINE Box2iIteratorInBiggerRowMajorBox() : m_innerBox(KY_NULL), m_outerBox(KY_NULL), m_pos(-KyInt32MAXVAL, -KyInt32MAXVAL),
		m_innerBoxIdx(-1), m_outerBoxIdx(-1) {}
	KY_INLINE Box2iIteratorInBiggerRowMajorBox(const Box2i& innerBox, const Box2i& outerBox) : m_innerBox(&innerBox), m_outerBox(&outerBox),
		m_pos(-KyInt32MAXVAL, -KyInt32MAXVAL), m_innerBoxIdx(-1), m_outerBoxIdx(-1)
	{
		Init();
	}

	KY_INLINE void Init()
	{
		if (m_innerBox != KY_NULL && m_outerBox != KY_NULL && m_innerBox->IsValid() && m_outerBox->IsValid())
		{
			KY_LOG_ERROR_IF(m_outerBox->IsInside(m_innerBox->Min()) == false || m_outerBox->IsInside(m_innerBox->Max()) == false ,
				("Error in boxes provided for Box2iRowMajorIterator. the innerBox must be fully included in the outerBox"));
			m_pos = m_innerBox->Min();
			m_innerBoxIdx = 0;
			m_outerBoxIdx = m_outerBox->GetRowMajorIndex(m_pos);
		}
	}

	KY_INLINE Box2iIteratorInBiggerRowMajorBox& operator++()
	{
		if (IsFinished() == false)
		{
			if (m_pos.x < m_innerBox->Max().x)
			{
				++m_pos.x;
				++m_innerBoxIdx;
				++m_outerBoxIdx;
				KY_ASSERT(m_outerBoxIdx == m_outerBox->GetRowMajorIndex(m_pos));
				KY_ASSERT(m_innerBoxIdx == m_innerBox->GetRowMajorIndex(m_pos));

			}
			else if (m_pos.y < m_innerBox->Max().y)
			{
				m_pos.x = m_innerBox->Min().x;
				++m_pos.y;
				++m_innerBoxIdx;
				m_outerBoxIdx += (m_outerBox->CountX() - m_innerBox->CountX()) + 1;
				KY_ASSERT(m_outerBoxIdx == m_outerBox->GetRowMajorIndex(m_pos));
				KY_ASSERT(m_innerBoxIdx == m_innerBox->GetRowMajorIndex(m_pos));
			}
			else
			{
				m_innerBoxIdx = -1;
				m_outerBoxIdx = -1;
			}
		}
		return *this;
	}

	KY_INLINE bool IsFinished() const { return m_innerBoxIdx < 0 || m_outerBoxIdx < 0 || m_innerBox == KY_NULL || m_outerBox == KY_NULL; }

	KY_INLINE KyInt32 GetInnerBoxRowMajorIndex() const { return m_innerBoxIdx; }
	KY_INLINE KyInt32 GetOuterBoxRowMajorIndex() const { return m_outerBoxIdx; }
	KY_INLINE const Vec2i& GetPos()              const { return m_pos;        }

private:
	const Box2i* m_innerBox;
	const Box2i* m_outerBox;
	Vec2i m_pos;
	KyInt32 m_innerBoxIdx;
	KyInt32 m_outerBoxIdx;
};

} // namespace Kaim


#endif

