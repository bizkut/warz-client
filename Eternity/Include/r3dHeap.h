#ifndef __R3D_HEAP_H__
#define __R3D_HEAP_H__

template <class OBJECT, class KEY>
class r3dHeap
{
public:
	struct Record 
	{
		KEY key;
		OBJECT obj;
	};
	
	r3dHeap(int maxSize, KEY maxValue, void *buffer)
	{
		if (buffer)
		{
			m_Pool = static_cast<Record *>(buffer);
			m_Allocated = false;
		}
		else
		{
			printf("r3dHeap() - buffer is null!\n");
			r3d_assert(false);
			m_Allocated = true;
		}
		m_MaxCount = maxSize;
		m_Pool[0].key = maxValue;
		Flush();
	}

	~r3dHeap()
	{
		if (m_Allocated)
			delete [] m_Pool;
	}

	void Flush()						{ m_Count = 0; }
	KEY MaxValue ()	const				{ return m_Pool[0].key; }
	KEY Value(int i = 0) const			{ return m_Pool[i + 1].key; }		
	bool IsEmpty()						{ return (m_Count == 0); }
	bool IsFull()						{ return (m_Count == GetMaxSize()); }
	int GetCount () const				{ return m_Count; }
	int GetMaxSize () const				{ return m_MaxCount; }
	OBJECT Get(int index = 0)			{  r3d_assert(index <= m_Count); return m_Pool[index+1].obj; }
	void Push (OBJECT &obj, KEY key)
	{
		r3d_assert(m_Count < m_MaxCount);
		r3d_assert(key < MaxValue());
		++m_Count;
		SortDown(m_Count, obj, key);
	}

	void ReplaceLowest(OBJECT &obj, KEY key, int lowestIndex)
	{
		r3d_assert(key < MaxValue());
		SortDown(lowestIndex+1, obj, key);
	}

	void Pop()
	{
		int j, k;
		Record tmp;

		m_Pool[1] = m_Pool[m_Count--];
		tmp = m_Pool[1];
		k = 1;
		while ( k <= (m_Count>>1)) 
		{
			j = k + k;
			if ((j < m_Count) && (m_Pool[j].key < m_Pool[j+1].key))
				++j;
			if (tmp.key >= m_Pool[j].key) 
				break;
			m_Pool[k] = m_Pool[j];
			k = j;
		}
		m_Pool[k] = tmp;
	}

	int FindLowest()
	{
		int lowest = m_Count >> 1;
		int i = lowest+1;
		while(i < m_Count+1)
		{
			if(m_Pool[i].key < m_Pool[lowest].key)
				lowest = i;
			++i;
		}
		return lowest-1;
	}

private:
	void SortDown(int index, OBJECT &obj, KEY key)
	{
		while(m_Pool[index>>1].key < key)
		{
			m_Pool[index] = m_Pool[index>>1];
			index >>= 1;
		}
		m_Pool[index].key = key;
		m_Pool[index].obj = obj;
	}

	int m_Count;
	int m_MaxCount;

	Record *m_Pool;
	bool m_Allocated;
};

#define HEAPBUFFERSIZE(maxSize, OBJECT, KEY) (maxSize+1)*(sizeof(r3dHeap<OBJECT, KEY>::Record))

#endif //__R3D_HEAP_H__