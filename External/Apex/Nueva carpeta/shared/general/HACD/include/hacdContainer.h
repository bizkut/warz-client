#ifndef HACD_CONTAINER_H

#define HACD_CONTAINER_H

#include "PlatformConfigHACD.h"
#include "hacdVector.h"


namespace HACD
{
	//! priority queque element
	class GraphEdgePQ
	{
	public:
		//! Constructor
		//! @param name edge's id
		//! @param priority edge's priority
		GraphEdgePQ(hacd::HaI32 name, hacd::HaF64 priority)
		{
			m_name = name;
			m_priority = priority;
		}
		//! Destructor
		~GraphEdgePQ(void){}
	private:
		hacd::HaI32									m_name;						//!< edge name
		hacd::HaF64									m_priority;					//!< priority
		//! Operator < for GraphEdgePQ
		friend bool                                 operator<(const GraphEdgePQ & lhs, const GraphEdgePQ & rhs);
		//! Operator > for GraphEdgePQ
		friend bool                                 operator>(const GraphEdgePQ & lhs, const GraphEdgePQ & rhs);
		friend class HACD;
	};
	inline bool										operator<(const GraphEdgePQ & lhs, const GraphEdgePQ & rhs)
	{
		if (lhs.m_priority!=rhs.m_priority) return lhs.m_priority<rhs.m_priority;
		return lhs.m_name < rhs.m_name;
	}
	inline bool										operator>(const GraphEdgePQ & lhs, const GraphEdgePQ & rhs)
	{
		if (lhs.m_priority!=rhs.m_priority) return lhs.m_priority>rhs.m_priority;
		return lhs.m_name > rhs.m_name;
	}


}; // end of HACD namespace

namespace HACD
{

typedef STDNAME::set<hacd::HaI32> HaI32Set;
typedef STDNAME::set<hacd::HaU64> HaU64Set;

typedef STDNAME::vector< hacd::HaU32 > HaU32Vector;
typedef STDNAME::vector< Vec3<hacd::HaF64> > Vec3Vector;
typedef STDNAME::vector< Vec3<hacd::HaI32> > TriVector;
typedef STDNAME::vector< hacd::HaI32 > HaI32Vector;
typedef STDNAME::vector< HaI32Set > HaI32SetVector;
typedef STDNAME::vector<GraphEdgePQ> GraphEdgePQVector;




}; // end of HACD namespace


namespace HACD
{
	class TMMEdge;
	//!	Vertex data structure used in a triangular manifold mesh (TMM).
	class TMMVertex
	{
	public:
		TMMVertex(void);
		~TMMVertex(void);

	private:
		Vec3<hacd::HaF64>										m_pos;
		hacd::HaI32												m_name;
		hacd::HaU32												m_id;
		CircularListElement<TMMEdge> *						m_duplicate;		// pointer to incident cone edge (or NULL)
		bool												m_onHull;
		bool												m_tag;
		TMMVertex(const TMMVertex & rhs);

		friend class HACD;            
		friend class ICHull;
		friend class TMMesh;
		friend class TMMTriangle;
		friend class TMMEdge;
	};

	//!	Triangle data structure used in a triangular manifold mesh (TMM).
	class TMMTriangle
	{
	public:
		TMMTriangle(void);
		~TMMTriangle(void);
	private:
		hacd::HaU32												m_id;
		CircularListElement<TMMEdge> *						m_edges[3];
		CircularListElement<TMMVertex> *					m_vertices[3];
		HaI32Set										m_incidentPoints;
		bool												m_visible;

		TMMTriangle(const TMMTriangle & rhs);

		friend class HACD;
		friend class ICHull;
		friend class TMMesh;
		friend class TMMVertex;
		friend class TMMEdge;
	};



	//!	Edge data structure used in a triangular manifold mesh (TMM).
	class TMMEdge
	{		
	public:
		TMMEdge(void);
		~TMMEdge(void);
	private:
		hacd::HaU32												m_id;
		CircularListElement<TMMTriangle> *					m_triangles[2];
		CircularListElement<TMMVertex> *					m_vertices[2];
		CircularListElement<TMMTriangle> *					m_newFace;


		TMMEdge(const TMMEdge & rhs);

		friend class HACD;
		friend class ICHull;
		friend class TMMTriangle;
		friend class TMMVertex;
		friend class TMMesh;
	};

	class Material
	{
	public:    
		Material(void);
		~Material(void){}        
		//    private:
		Vec3<hacd::HaF64>                                            m_diffuseColor;
		hacd::HaF64                                                  m_ambientIntensity;
		Vec3<hacd::HaF64>                                            m_specularColor;
		Vec3<hacd::HaF64>                                            m_emissiveColor;
		hacd::HaF64                                                  m_shininess;
		hacd::HaF64                                                  m_transparency;

		friend class TMMesh;
		friend class HACD;
	};


	//!	triangular manifold mesh data structure.
	class TMMesh
	{
	public:

		//! Returns the number of vertices>
		inline hacd::HaU32										GetNVertices() const { return m_vertices.GetSize();}
		//! Returns the number of edges
		inline hacd::HaU32										GetNEdges() const { return m_edges.GetSize();}
		//! Returns the number of triangles
		inline hacd::HaU32										GetNTriangles() const { return m_triangles.GetSize();}
		//! Returns the vertices circular list
		inline const CircularList<TMMVertex> &              GetVertices() const { return m_vertices;}
		//! Returns the edges circular list
		inline const CircularList<TMMEdge> &				GetEdges() const { return m_edges;}
		//! Returns the triangles circular list
		inline const CircularList<TMMTriangle> &			GetTriangles() const { return m_triangles;}
		//! Returns the vertices circular list
		inline CircularList<TMMVertex> &                    GetVertices() { return m_vertices;}
		//! Returns the edges circular list
		inline CircularList<TMMEdge> &                      GetEdges() { return m_edges;}
		//! Returns the triangles circular list
		inline CircularList<TMMTriangle> &                  GetTriangles() { return m_triangles;}               
		//! Add vertex to the mesh
		CircularListElement<TMMVertex> *					AddVertex() {return m_vertices.Add();}
		//! Add vertex to the mesh
		CircularListElement<TMMEdge> *						AddEdge() {return m_edges.Add();}
		//! Add vertex to the mesh
		CircularListElement<TMMTriangle> *					AddTriangle() {return m_triangles.Add();}
		//! Print mesh information 
		//!
		void                                                GetIFS(Vec3<hacd::HaF64> * const points, Vec3<hacd::HaI32> * const triangles);
		//!  
		void												Clear();
		//!
		void                                                Copy(TMMesh & mesh);
		//!
		bool												CheckConsistancy();
		//!
		bool												Normalize();
		//!
		bool												Denormalize();
		//!	Constructor
		TMMesh(void);
		//! Destructor
		virtual												~TMMesh(void);

	private:
		CircularList<TMMVertex>								m_vertices;
		CircularList<TMMEdge>								m_edges;
		CircularList<TMMTriangle>							m_triangles;
		hacd::HaF64												m_diag;						//>! length of the BB diagonal
		Vec3<hacd::HaF64>										m_barycenter;				//>! barycenter of the mesh

		// not defined
		TMMesh(const TMMesh & rhs);
		friend class ICHull;
		friend class HACD;
	};

	class DPoint;
	class HACD;
	//!	Incremental Convex Hull algorithm (cf. http://maven.smith.edu/~orourke/books/ftp.html ).
	enum ICHullError
	{
		ICHullErrorOK = 0,
		ICHullErrorCoplanarPoints,
		ICHullErrorNoVolume,
		ICHullErrorInconsistent,
		ICHullErrorNotEnoughPoints
	};
	typedef STDNAME::map<hacd::HaI32, DPoint> DPointMap;
	typedef STDNAME::vector<CircularListElement<TMMEdge> *> CircularListElementTMMEdgeVector;
	typedef STDNAME::vector<CircularListElement<TMMTriangle> *> CircularListElementTMMTriangleVector;
	class ICHull : public UANS::UserAllocated
	{
	public:
		//!
		bool												IsFlat() { return m_isFlat;}
		//! 
		DPointMap *							GetDistPoints() const { return m_distPoints;}
		//!
		void												SetDistPoints(DPointMap * distPoints) { m_distPoints = distPoints;}
		//! Returns the computed mesh
		TMMesh &                                            GetMesh() { return m_mesh;}
		//!	Add one point to the convex-hull    
		bool                                                AddPoint(const Vec3<hacd::HaF64> & point) {return AddPoints(&point, 1);}
		//!	Add one point to the convex-hull    
		bool                                                AddPoint(const Vec3<hacd::HaF64> & point, hacd::HaI32 id);
		//!	Add points to the convex-hull
		bool                                                AddPoints(const Vec3<hacd::HaF64> * points, hacd::HaU32 nPoints);	
		bool												AddPoints(Vec3Vector points);
		//!	
		ICHullError                                         Process();
		//! 
		ICHullError                                         Process(hacd::HaU32 nPointsCH);
		//!
		hacd::HaF64                                              ComputeVolume();
		//!
		bool                                                IsInside(const Vec3<hacd::HaF64> pt);
		//!
		hacd::HaF64												ComputeDistance(hacd::HaI32 name, const Vec3<hacd::HaF64> & pt, const Vec3<hacd::HaF64> & normal, bool & insideHull, bool updateIncidentPoints);
		//!
		const ICHull &                                      operator=(ICHull & rhs);        

		//!	Constructor
		ICHull(void);
		//! Destructor
		virtual                                             ~ICHull(void) {};

	private:
		//!	DoubleTriangle builds the initial hacd::HaF64 triangle.  It first finds 3 noncollinear points and makes two faces out of them, in opposite order. It then finds a fourth point that is not coplanar with that face.  The vertices are stored in the face structure in counterclockwise order so that the volume between the face and the point is negative. Lastly, the 3 newfaces to the fourth point are constructed and the data structures are cleaned up. 
		ICHullError                                         DoubleTriangle();
		//!	MakeFace creates a new face structure from three vertices (in ccw order).  It returns a pointer to the face.
		CircularListElement<TMMTriangle> *                  MakeFace(CircularListElement<TMMVertex> * v0,  
			CircularListElement<TMMVertex> * v1,
			CircularListElement<TMMVertex> * v2,
			CircularListElement<TMMTriangle> * fold);			
		//!	
		CircularListElement<TMMTriangle> *                  MakeConeFace(CircularListElement<TMMEdge> * e, CircularListElement<TMMVertex> * v);
		//!	
		bool                                                ProcessPoint();
		//!
		bool                                                ComputePointVolume(hacd::HaF64 &totalVolume, bool markVisibleFaces);
		//!
		bool                                                FindMaxVolumePoint();
		//!	
		bool                                                CleanEdges();
		//!	
		bool                                                CleanVertices(hacd::HaU32 & addedPoints);
		//!	
		bool                                                CleanTriangles();
		//!	
		bool                                                CleanUp(hacd::HaU32 & addedPoints);
		//!
		bool                                                MakeCCW(CircularListElement<TMMTriangle> * f,
			CircularListElement<TMMEdge> * e, 
			CircularListElement<TMMVertex> * v);
		void												Clear(); 
	private:
		static const hacd::HaI32                                   sc_dummyIndex;
		static const hacd::HaF64                                 sc_distMin;
		TMMesh                                              m_mesh;
		CircularListElementTMMEdgeVector         m_edgesToDelete;
		CircularListElementTMMEdgeVector         m_edgesToUpdate;
		CircularListElementTMMTriangleVector     m_trianglesToDelete; 
		DPointMap *							m_distPoints;            
		CircularListElement<TMMVertex> *					m_dummyVertex;
		Vec3<hacd::HaF64>										m_normal;
		bool												m_isFlat;


		ICHull(const ICHull & rhs);

		friend class HACD;
	};

	class DPoint  
	{
	public:       
		DPoint(hacd::HaF64 dist=0.0, bool computed=false, bool distOnly=false)
			:m_dist(dist),
			m_computed(computed),
			m_distOnly(distOnly){};
		~DPoint(){};      
	private:
		hacd::HaF64													m_dist;
		bool													m_computed;
		bool                                                    m_distOnly;
		friend class TMMTriangle;
		friend class TMMesh;
		friend class GraphVertex;
		friend class GraphEdge;
		friend class Graph;
		friend class ICHull;
		friend class HACD;
	};


	class GraphVertex  
	{
	public:
		bool                                    AddEdge(hacd::HaI32 name) 
		{ 
			m_edges.insert(name); 
			return true; 
		}
		bool                                    DeleteEdge(hacd::HaI32 name);        
		GraphVertex();
		~GraphVertex(){ delete m_convexHull;};      
	private:
		hacd::HaI32                                    m_name;
		HaI32Set                          m_edges;
		hacd::HaI32                                    m_cc;        
		bool                                    m_deleted;
		HaU32Vector	                    m_ancestors;
		DPointMap					m_distPoints;

		hacd::HaF64                                  m_error;
		hacd::HaF64                                  m_surf;
		hacd::HaF64                                  m_volume;
		hacd::HaF64                                  m_perimeter;
		hacd::HaF64                                  m_concavity;
		ICHull *                                m_convexHull;
		HaU64Set			m_boudaryEdges;


		friend class GraphEdge;
		friend class Graph;
		friend class HACD;
	};

	class GraphEdge 
	{
	public:
		GraphEdge();
		~GraphEdge(){delete m_convexHull;};
	private:
		hacd::HaI32                                    m_name;
		hacd::HaI32                                    m_v1;
		hacd::HaI32                                    m_v2;
		DPointMap					m_distPoints;
		hacd::HaF64                                  m_error;
		hacd::HaF64                                  m_surf;
		hacd::HaF64                                  m_volume;
		hacd::HaF64                                  m_perimeter;
		hacd::HaF64                                  m_concavity;
		ICHull *                                m_convexHull;
		HaU64Set			m_boudaryEdges;
		bool                                    m_deleted;



		friend class GraphVertex;
		friend class Graph;
		friend class HACD;
	};

	typedef STDNAME::vector<GraphEdge> GraphEdgeVector;
	typedef STDNAME::vector<GraphVertex> GraphVertexVector;

}; // end of HACD namespace

#endif
