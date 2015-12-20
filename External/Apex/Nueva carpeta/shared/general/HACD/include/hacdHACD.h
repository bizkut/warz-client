/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#ifndef HACD_HACD_H
#define HACD_HACD_H
#include <hacdVersion.h>
#include <hacdVector.h>
#include <hacdGraph.h>
#include <hacdICHull.h>

namespace HACD
{
    const hacd::HaF64                                    sc_pi = 3.14159265;
	class HACD;

	typedef STDNAME::set< GraphEdgePQ > GraphEdgePQSet;

	// just to be able to set the capcity of the container
	class ReservablePriorityQueue
	{
	public:
		void	reserve(hacd::HaU32 size);
		void	push(const GraphEdgePQ &pq);
		hacd::HaU32	size(void) const;
		const GraphEdgePQ &	top(void);
		void	pop(void);
		bool	empty(void) const;
	private:
		GraphEdgePQSet	mQueue;
	};
	
	//! Provides an implementation of the Hierarchical Approximate Convex Decomposition (HACD) technique described in "A Simple and Efficient Approach for 3D Mesh Approximate Convex Decomposition" Game Programming Gems 8 - Chapter 2.8, p.202. A short version of the chapter was published in ICIP09 and is available at ftp://ftp.elet.polimi.it/users/Stefano.Tubaro/ICIP_USB_Proceedings_v2/pdfs/0003501.pdf
	class HACD : public UANS::UserAllocated
	{            
    public:
		class CallBackFunction
		{
		public:
			virtual bool progressUpdate(const char *message, hacd::HaF64 progress, hacd::HaF64 concavity, hacd::HaU32 nVertices) = 0;
		};



		//! Gives the triangles partitionas an array of size m_nTriangles where the i-th element specifies the cluster to which belong the i-th triangle
		//! @return triangles partition
		const hacd::HaI32 *							GetPartition() const { return m_partition;}
        //! Sets the scale factor
		//! @param scale scale factor
		void										SetScaleFactor(hacd::HaF64  scale) { m_scale = scale;}
		//! Gives the scale factor
		//! @return scale factor
		hacd::HaF64									GetScaleFactor() const { return m_scale;}
		//! Sets the call-back function
		//! @param callBack pointer to the call-back function
		void										SetCallBack(CallBackFunction  *callBack) { m_callBack = callBack;}
		//! Gives the call-back function
		//! @return pointer to the call-back function
		const CallBackFunction  *                   GetCallBack() const { return m_callBack;}
        
        //! Specifies whether faces points should be added when computing the concavity
		//! @param addFacesPoints true = faces points should be added
		void										SetAddFacesPoints(bool  addFacesPoints) { m_addFacesPoints = addFacesPoints;}
		//! Specifies wheter faces points should be added when computing the concavity
		//! @return true = faces points should be added
		bool										GetAddFacesPoints() const { return m_addFacesPoints;}
        //! Specifies whether extra points should be added when computing the concavity
		//! @param addExteraDistPoints true = extra points should be added
		void										SetAddExtraDistPoints(bool  addExtraDistPoints) { m_addExtraDistPoints = addExtraDistPoints;}
		//! Specifies wheter extra points should be added when computing the concavity
		//! @return true = extra points should be added
		bool										GetAddExtraDistPoints() const { return m_addExtraDistPoints;}
        //! Specifies whether extra points should be added when computing the concavity
		//! @param addExteraDistPoints true = extra points should be added
		void										SetAddNeighboursDistPoints(bool  addNeighboursDistPoints) { m_addNeighboursDistPoints = addNeighboursDistPoints;}
		//! Specifies wheter extra points should be added when computing the concavity
		//! @return true = extra points should be added
		bool										GetAddNeighboursDistPoints() const { return m_addExtraDistPoints;}
        //! Sets the points of the input mesh (Remark: the input points will be scaled and shifted. Use DenormalizeData() to invert those operations)
		//! @param points pointer to the input points
		void										SetPoints(Vec3<hacd::HaF64>  * points) { m_points = points;}
		//! Gives the points of the input mesh (Remark: the input points will be scaled and shifted. Use DenormalizeData() to invert those operations)
		//! @return pointer to the input points 
		const Vec3<hacd::HaF64>   *                      GetPoints() const { return m_points;}
		//! Sets the triangles of the input mesh.
		//! @param triangles points pointer to the input points
		void										SetTriangles(Vec3<hacd::HaI32>  * triangles) { m_triangles = triangles;}
		//! Gives the triangles in the input mesh 
		//! @return pointer to the input triangles 
		const Vec3<hacd::HaI32>   *			            GetTriangles() const { return m_triangles;}
		//! Sets the number of points in the input mesh.
		//! @param nPoints number of points the input mesh
		void										SetNPoints(hacd::HaU32 nPoints) { m_nPoints = nPoints;}
		//! Gives the number of points in the input mesh.
		//! @return number of points the input mesh
		hacd::HaU32									GetNPoints() const { return m_nPoints;}
		//! Sets the number of triangles in the input mesh.
		//! @param nTriangles number of triangles in the input mesh
		void										SetNTriangles(hacd::HaU32 nTriangles) { m_nTriangles = nTriangles;}
		//! Gives the number of triangles in the input mesh.
		//! @return number of triangles the input mesh
		hacd::HaU32									GetNTriangles() const { return m_nTriangles;}
		//! Sets the minimum number of clusters to be generated.
		//! @param nClusters minimum number of clusters
		void										SetNClusters(hacd::HaU32 nClusters) { m_nMinClusters = nClusters;}
		//! Gives the number of generated clusters.
		//! @return number of generated clusters
		hacd::HaU32									GetNClusters() const { return m_nClusters;}
		//! Sets the maximum allowed concavity.
		//! @param concavity maximum concavity
		void										SetConcavity(hacd::HaF64 concavity) { m_concavity = concavity;}
		//! Gives the maximum allowed concavity.
		//! @return maximum concavity
		hacd::HaF64                                      GetConcavity() const { return m_concavity;}

		//! Sets the maximum allowed distance to get CCs connected.
		//! @param concavity maximum distance to get CCs connected
		void										SetConnectDist(hacd::HaF64 ccConnectDist) { m_ccConnectDist = ccConnectDist;}
		//! Gives the maximum allowed distance to get CCs connected.
		//! @return maximum distance to get CCs connected
		hacd::HaF64                                      GetConnectDist() const { return m_ccConnectDist;}        


        //! Sets the volume weight.
		//! @param beta volume weight
        void										SetVolumeWeight(hacd::HaF64 beta) { m_beta = beta;}
		//! Gives the volume weight.
		//! @return volume weight
        hacd::HaF64                                      GetVolumeWeight() const { return m_beta;}	
		//! Sets the compacity weight (i.e. parameter alpha in ftp://ftp.elet.polimi.it/users/Stefano.Tubaro/ICIP_USB_Proceedings_v2/pdfs/0003501.pdf).
		//! @param alpha compacity weight
        void										SetCompacityWeight(hacd::HaF64 alpha) { m_alpha = alpha;}
		//! Gives the compacity weight (i.e. parameter alpha in ftp://ftp.elet.polimi.it/users/Stefano.Tubaro/ICIP_USB_Proceedings_v2/pdfs/0003501.pdf).
		//! @return compacity weight
        hacd::HaF64                                      GetCompacityWeight() const { return m_alpha;}	
		//! Sets the maximum number of vertices for each generated convex-hull.
		//! @param nVerticesPerCH maximum # vertices per CH
        void										SetNVerticesPerCH(hacd::HaU32 nVerticesPerCH) { m_nVerticesPerCH = nVerticesPerCH;}
		//! Gives the maximum number of vertices for each generated convex-hull.
		//! @return maximum # vertices per CH
		hacd::HaU32									GetNVerticesPerCH() const { return m_nVerticesPerCH;}
		//! Gives the number of vertices for the cluster number numCH.
		//! @return number of vertices
		hacd::HaU32                                      GetNPointsCH(hacd::HaU32 numCH) const;
		//! Gives the number of triangles for the cluster number numCH.
		//! @param numCH cluster's number
		//! @return number of triangles
        hacd::HaU32                                      GetNTrianglesCH(hacd::HaU32 numCH) const;
		//! Gives the vertices and the triangles of the cluster number numCH.
		//! @param numCH cluster's number
		//! @param points pointer to the vector of points to be filled
		//! @param triangles pointer to the vector of triangles to be filled
		//! @return true if sucess
        bool                                        GetCH(hacd::HaU32 numCH, Vec3<hacd::HaF64> * const points, Vec3<hacd::HaI32> * const triangles);     
		//! Computes the HACD decomposition.
		//! @param connectCCs specifies whether to connect the mesh's Connected Components by additional edges or not
		//! @param fullCH specifies whether to generate convex-hulls with a full or limited (i.e. < m_nVerticesPerCH) number of vertices
        //! @param exportDistPoints specifies wheter distance points should ne exported or not (used only for debugging).
		//! @return true if sucess
        bool										Compute(bool connectCCs=false, bool fullCH=false, bool exportDistPoints=false);
		
		//! Shifts and scales to the data to have all the coordinates between 0.0 and 1000.0.
		void										NormalizeData();
		//! Inverse the operations applied by NormalizeData().
		void										DenormalizeData();
        //! Constructor.
													HACD(void);
		//! Destructor.
													~HACD(void);

	private:
		//! Gives the edge index.
		//! @param a first vertex id
		//! @param b second vertex id
		//! @return edge's index
		static hacd::HaU64					GetEdgeIndex(hacd::HaU64 a,hacd::HaU64 b) 
													{ 
														if (a > b) return (a << 32) + b;
														else	   return (b << 32) + a;
													}
		//! Computes the concavity of a cluster.
		//! @param ch the cluster's convex-hull
		//! @param distPoints the cluster's points 
		//! @return cluster's concavity
		hacd::HaF64										Concavity(ICHull & ch, DPointMap & distPoints);
		//! Computes the perimeter of a cluster.
		//! @param triIndices the cluster's triangles
		//! @param distPoints the cluster's points 
		//! @return cluster's perimeter
        hacd::HaF64										ComputePerimeter(const HaU32Vector & triIndices) const;
		//! Creates the Graph by associating to each mesh triangle a vertex in the graph and to each couple of adjacent triangles an edge in the graph.
		//! @param connectCCs specifies whether to connect the mesh's Connected Components by additional edges or not 
        void										CreateGraph(void);	
		//! Initializes the graph costs and computes the vertices normals
        void										InitializeGraph();
		//! Computes the cost of an edge
		//! @param e edge's id
        void                                        ComputeEdgeCost(hacd::HaU32 e);
		//! Initializes the priority queue
		//! @param fast specifies whether fast mode is used
		//! @return true if success
        bool                                        InitializePQ();
        //! Cleans the intersection between convex-hulls
        void                                        CleanClusters();
        //! Computes convex-hulls from partition information
        //! @param fullCH specifies whether to generate convex-hulls with a full or limited (i.e. < m_nVerticesPerCH) number of vertices
		void										ComputeConvexHulls(bool fullCH);
		//! Simplifies the graph
		//! @param fast specifies whether fast mode is used
		void										Simplify();

	private:
		hacd::HaF64										m_scale;					//>! scale factor used for NormalizeData() and DenormalizeData()
        Vec3<hacd::HaI32> *								m_triangles;				//>! pointer the triangles array
        Vec3<hacd::HaF64> *								m_points;					//>! pointer the points array
        Vec3<hacd::HaF64> *                              m_facePoints;               //>! pointer to the faces points array
        Vec3<hacd::HaF64> *                              m_faceNormals;              //>! pointer to the faces normals array
        Vec3<hacd::HaF64> *								m_normals;					//>! pointer the normals array
        hacd::HaU32										m_nTriangles;				//>! number of triangles in the original mesh
        hacd::HaU32										m_nPoints;					//>! number of vertices in the original mesh
        hacd::HaU32										m_nClusters;				//>! number of clusters
        hacd::HaU32										m_nMinClusters;				//>! minimum number of clusters
		hacd::HaF64										m_ccConnectDist;			//>! maximum allowed distance to connect CCs
        hacd::HaF64										m_concavity;				//>! maximum concavity
		hacd::HaF64										m_alpha;					//>! compacity weigth
        hacd::HaF64                                      m_beta;                     //>! volume weigth
        hacd::HaF64										m_diag;						//>! length of the BB diagonal
		Vec3<hacd::HaF64>								m_barycenter;				//>! barycenter of the mesh
//		HaI32SetVector               m_v2T;						//!> vertex to triangle adjacency information
        HaI32Vector                         m_cVertices;				//!> array of vertices each belonging to a different cluster
        ICHull *                                    m_convexHulls;				//!> convex-hulls associated with the final HACD clusters
		Graph										m_graph;					//!> simplification graph
        hacd::HaU32                                      m_nVerticesPerCH;			//!> maximum number of vertices per convex-hull
		ReservablePriorityQueue						m_pqueue;
		HACD(const HACD & rhs);
		CallBackFunction							*m_callBack;					//!> call-back function
		hacd::HaI32 *										m_partition;				//!> array of size m_nTriangles where the i-th element specifies the cluster to which belong the i-th triangle
        bool                                        m_addFacesPoints;           //>! specifies whether to add faces points or not
        bool                                        m_addExtraDistPoints;       //>! specifies whether to add extra points for concave shapes or not
		bool										m_addNeighboursDistPoints;  //>! specifies whether to add extra points from adjacent clusters or not

	};
}
#endif