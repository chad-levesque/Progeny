#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

template <class T> class LinkedListNode
{
protected:
	T* m_pNext;			// Points to next item in the list
	T* m_pPrevious;		// Points to previous item in the list

public:
	// Constructors/destructors
	LinkedListNode()					{ m_pNext = m_pPrevious = 0; }
	~LinkedListNode()
	{
		Remove();
	}
	static void InitList(LinkedListNode<T> &nHead, LinkedListNode<T> &nTail)
	{
		nHead.m_pNext = (T*)&nTail;
		nTail.m_pPrevious = (T*)&nHead;
	}

	// Members to get the next and previous pointers (must use Insert/Remove members to set them)
	T* GetNext()				{ return m_pNext; }
	T* GetPrevious()			{ return m_pPrevious; }

	// Members to insert into, remove from, and check status in a list
	bool IsInList()				{ return (m_pNext && m_pPrevious); }
	void InsertBefore(T* pNode)
	{
		if(pNode && !IsInList())
		{
			m_pNext = pNode;
			m_pPrevious = pNode->m_pPrevious;
			pNode->m_pPrevious->m_pNext = (T*)this;
			pNode->m_pPrevious = (T*)this;
		}
	}
	void InsertAfter(T* pNode)
	{
		if(pNode && !IsInList())
		{
			m_pPrevious = pNode;
			m_pNext = pNode->m_pNext;
			pNode->m_pNext->m_pPrevious = (T*)this;
			pNode->m_pNext = (T*)this;
		}
	}
	void Remove()
	{
		if(m_pPrevious)
			m_pPrevious->m_pNext = m_pNext;
		if(m_pNext)
			m_pNext->m_pPrevious = m_pPrevious;
		m_pNext = m_pPrevious = 0;
	}
};

template <class T> class LinkedList
{
protected:
	LinkedListNode<T> m_nHead;	// Points to head item
	LinkedListNode<T> m_nTail;	// Points to tail item

public:
	// Constructors/destructors
	LinkedList()		{ LinkedListNode<T>::InitList(m_nHead, m_nTail); }
	virtual ~LinkedList()		{ RemoveAll(); }

	// Members to get the head and tail items of the list
	T* GetHead()		{ return m_nHead.GetNext(); }
	T* GetTail()		{ return m_nTail.GetPrevious(); }

	// Members to add items to the list
	void AddHead(T* p)					{ p->InsertAfter((T*)&m_nHead); }
	void AddTail(T* p)					{ p->InsertBefore((T*)&m_nTail); }
	void InsertBefore(T* p, T* pNode)	{ p->InsertBefore(pNode); }
	void InsertAfter(T* p, T* pNode)	{ p->InsertAfter(pNode); }

	// Members to remove items from the list
	T* Remove(T* p)		{ p->Remove(); return p; }
	T* RemoveHead()		{ return Remove(GetHead()); }
	T* RemoveTail()		{ return Remove(GetTail()); }
	void RemoveAll()	{ while(GetHead()->IsInList()) delete GetHead(); }

	// Members to get the size of the list
	bool IsEmpty()		{ return !GetHead()->IsInList(); }
	int GetCount()
	{
		register T* pNode = GetHead();
		int i = 0;
		for(i = 0; pNode->IsInList(); i++)
			pNode = pNode->GetNext();
		return i;
	}
};

#endif // __LINKEDLIST_H__