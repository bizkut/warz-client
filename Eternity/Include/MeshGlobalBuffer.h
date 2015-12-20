#ifndef	__R3D_MESH_GLOBAL_BUFFER_H
#define	__R3D_MESH_GLOBAL_BUFFER_H

struct MeshGlobalBuffer 
{
	struct Buffers 
	{

		static const int ibSize = 8*1024*1024;
		static const int vbSize = 16*1024*1024;

		Buffers();

		void init();
		void release();

		void allocIB();
		void allocVB();

		r3dD3DVertexBufferTunnel VB;
		r3dD3DIndexBufferTunnel IB;
		unsigned int vbCursor;
		unsigned int ibCursor;
	};

	struct Entry 
	{
		void Set();
		void Init(int numVertices, int numIndices, int sizeofVertex, int sizeofIndex);
		void InitUnshared( class r3dVertexBuffer* vbuf, class r3dIndexBuffer* ibuf );
		void Lock(void*& vertices, void*& indices);
		void Unlock();

		r3dD3DVertexBufferTunnel VB;
		r3dD3DIndexBufferTunnel IB;

		int VBId ;

		unsigned int vbOffset;
		unsigned int vbStride;
		unsigned int vCount;
		unsigned int startIndex;

		unsigned int ibOffset;
		unsigned int ibStride;
		unsigned int iCount;
		unsigned int minVertexIndex;

	private:
		void InitVBIB( void* params ) ;
		friend void SetEntryIBVBMainThread( void * params ) ;
	};

	static MeshGlobalBuffer* instance;

	static void unloadManaged();

	void GetEntry(Entry& entry, int numVertices, int numIndices, int sizeofVertex, int sizeofIndex);
	void Clear();

	static const int maxBuffersCount = 64;
	r3dTL::TFixedArray< Buffers, maxBuffersCount > buffers ;
	int currentVB;
	int currentIB;

	MeshGlobalBuffer() ;
};



#endif //__R3D_MESH_GLOBAL_BUFFER_H
