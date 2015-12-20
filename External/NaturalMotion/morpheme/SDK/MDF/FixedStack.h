#pragma once
// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------

/**
 * basic fixed-size stack; used in the parser when collecting data
 * from recursive rules.
 */
template <typename _T, int SIZE>
class FixedStack
{
public:

  FixedStack() : m_index(0)
  {
  }

  FixedStack(const FixedStack& rhs)
  {
    for (unsigned int i=0; i<SIZE; i++)
    {
      m_items[i] = rhs.m_items[i];
    }
    m_index = rhs.m_index;
  }

  void appendStack(const FixedStack& rhs)
  {
    for (unsigned int i=0; i<rhs.count(); i++)
    {
      push(rhs.get(i));
    }
  }

  void reset()
  {
    m_index = 0;
  }

  void push(const _T& st)
  {
    assert(m_index < SIZE);
    m_items[m_index] = st;
    m_index ++;
  }

  _T& push()
  {
    assert(m_index < SIZE);
    m_index ++;
    return m_items[m_index - 1];
  }

  _T& pop()
  {
    assert(m_index > 0);
    return m_items[--m_index];
  }

  const _T& get(unsigned int index) const
  {
    assert(index < SIZE);
    return m_items[index];
  }

  const _T& getLast() const
  {
    assert(m_index > 0);
    return m_items[m_index - 1];
  }


  unsigned int count() const { return m_index; }
  unsigned int maxSize() const { return SIZE; }
  bool isFull() const { return m_index >= SIZE; }

protected:

  _T            m_items[SIZE];
  unsigned int  m_index;
};
