#ifndef __R3D_TREE_NODE_H__
#define __R3D_TREE_NODE_H__

// inherent from this class to get tree functionality
template<class T>
class r3dTreeNode
{
public:
	r3dTreeNode()
	{
		m_pParent = NULL;
		m_pChild = NULL;
		m_pSibling = NULL;
	}

	r3dTreeNode(T* pParent)
	{
		m_pParent = pParent;
		m_pChild = NULL;
		m_pSibling = NULL;
	}
	
	virtual ~r3dTreeNode()
	{
		T* child = GetChild();
		T* sibling = GetSibling();

		if (GetParent())
			GetParent()->RemoveChild((T*)this);	

		delete child;
		delete sibling;
	}

	T* GetParent() {return m_pParent;}
	T* GetSibling() {return m_pSibling;}
	T* GetChild() {return m_pChild;}
	
	void SetParent(T* pParent) { m_pParent = pParent; }
	void SetSibling(T* pSibling) 
	{
		m_pSibling = pSibling;
		if(pSibling)
			pSibling->SetParent(GetParent());
	}

	void SetChild(T* pChild) 
	{
		r3d_assert(pChild != this);

		m_pChild = pChild;
		if(pChild)
			pChild->SetParent((T*)this);
	}

	bool HasChild() {return m_pChild != NULL;}
	bool HasSibling() {return m_pSibling != NULL;}
	bool HasParent() {return m_pParent != NULL;}

	void AddChild(T* pChild)
	{
		if(GetChild())
			GetChild()->AddSibling(pChild);
		else
			SetChild(pChild);
	}

	void RemoveChild(T *pChild)
	{
		if (!pChild)
			return;
		if(GetChild()==pChild)
		{
			SetChild(GetChild()->GetSibling());
			pChild->SetParent(NULL);
			pChild->SetSibling(NULL);
		}
		else
			GetChild()->RemoveSibling(pChild);
	}

	void AddSibling(T* pSibling)
	{
		r3dTreeNode* node = this;
		r3dTreeNode* sib = NULL;
		while ((sib = node->GetSibling()))
			node = sib;

		node->SetSibling(pSibling);
	}

	void RemoveSibling(T *pSibling)
	{
		if (!pSibling)
			return;
		if(GetSibling()==pSibling)
		{
			SetSibling(GetSibling()->GetSibling());
			pSibling->SetParent(NULL);
			pSibling->SetSibling(NULL);
		}
		else
			GetSibling()->RemoveSibling(pSibling);
	}

	T* GetNextNeighbor(T* pRoot = NULL) 
	{
		if(GetSibling())
			return GetSibling();
		if(GetParent() && GetParent() != pRoot)
			return GetParent()->GetNextNeighbor(pRoot);
		return NULL;
	}

	T* GetNext(T* pRoot = NULL) 
	{
		if(GetChild())
			return GetChild();
		if(GetSibling())
			return GetSibling();
		if(GetParent() && GetParent() != pRoot)
			return GetParent()->GetNextNeighbor(pRoot);
		return NULL;
	}

	T* GetLast() 
	{
		if(GetSibling())
			return GetSibling()->GetLast();
		if(GetChild())
			return GetChild()->GetLast();
		return (T*)this;
	}

	T* GetPrevious() 
	{
		if(GetParent())
		{
			T* child = GetParent()->GetChild();
			if(child == this)
				return GetParent();
			while(child && child->GetSibling() != this)
				child = child->GetSibling();
			if(child->GetChild())
				return child->GetChild()->GetLast();
			return child;
		}
		return NULL;
	}

	T* m_pParent;
	T* m_pChild;
	T* m_pSibling;
};

#endif //__R3D_TREE_NODE_H__