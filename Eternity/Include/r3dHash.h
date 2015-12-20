#ifndef __R3D_HASH_H__
#define __R3D_HASH_H__

// using FNV-1a
// See http://www.isthe.com/chongo/tech/comp/fnv/
class r3dHash
{
public:
	r3dHash(const char* pText) : m_Value(MakeHash(pText)) {}

	operator unsigned () const { return m_Value; }

	static unsigned MakeHash(const char* pText);
	static uint32_t MakeHash(const char* buf, size_t count);

	const unsigned m_Value;
};

#endif //__R3D_HASH_H__