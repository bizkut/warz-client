/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable:4100 4510 4610)


#include "hacdGraph.h"
#include "hacdHACD.h"
#include "hacdICHull.h"
#include <string.h>
#include <float.h>

namespace HACD
{ 

static void swap(hacd::HaI32 &v1,hacd::HaI32 &v2)
{
	hacd::HaI32 t = v1;
	v1 = v2;
	v2 = t;
}

static hacd::HaF64 max(hacd::HaF64 v1,hacd::HaF64 v2)
{
	return v1 > v2 ? v1 : v2;
}


static void set_union(HaI32Set::iterator av1,
	HaI32Set::iterator av2,
	HaI32Set::iterator bv1,
	HaI32Set::iterator bv2,
	HaI32Set &combine)
{
	for (HaI32Set::iterator i=av1; i!=av2; ++i)
	{
		combine.insert( (*i) );
	}
	for (HaI32Set::iterator i=bv1; i!=bv2; ++i)
	{
		combine.insert( (*i) );
	}
}

static void set_symmetric_difference (const HaU64Set &a, 
									const HaU64Set &b,
									HaU64Set &diff)
{
	for (HaU64Set::const_iterator i=a.begin(); i!=a.end(); ++i)
	{
		hacd::HaU64 v = (*i);
		HaU64Set::const_iterator found = b.find(v);
		if ( found == b.end() )
		{
			diff.insert(v);
		}
	}
}

	hacd::HaF64  HACD::Concavity(ICHull & ch, DPointMap & distPoints)
	{
		hacd::HaF64 concavity = 0.0;
		hacd::HaF64 distance = 0.0;       
		DPointMap::iterator itDP(distPoints.begin());
		DPointMap::iterator itDPEnd(distPoints.end());
		for(; itDP != itDPEnd; ++itDP) 
		{
			if (!(itDP->second).m_computed)
			{
				if (itDP->first >= 0)
				{
					distance = ch.ComputeDistance(itDP->first, m_points[itDP->first], m_normals[itDP->first], (itDP->second).m_computed, true);
				}
				else
				{
					distance = ch.ComputeDistance(itDP->first, m_facePoints[-itDP->first-1], m_faceNormals[-itDP->first-1], (itDP->second).m_computed, true);
				}
			}
			else
			{
				distance = (itDP->second).m_dist;
			}
			if (concavity < distance) 
			{
				concavity = distance;
			}
		}
		return concavity;
	}

	void HACD::CreateGraph(void)
	{
		STDNAME::vector< STDNAME::set< hacd::HaI32> >  vertexToTriangles;
		vertexToTriangles.resize(m_nPoints);
		for(size_t t = 0; t < m_nTriangles; ++t)
		{
			vertexToTriangles[m_triangles[t].X()].insert(static_cast<long>(t));
			vertexToTriangles[m_triangles[t].Y()].insert(static_cast<long>(t));
			vertexToTriangles[m_triangles[t].Z()].insert(static_cast<long>(t));
		}
		m_graph.Clear();
		m_graph.Allocate(m_nTriangles, 5 * m_nTriangles);
		hacd::HaU64 tr1[3];
		hacd::HaU64 tr2[3];
		hacd::HaI32 i1, j1, k1, i2, j2, k2;
		hacd::HaI32 t1, t2;
		for (hacd::HaU32 v = 0; v < m_nPoints; v++) 
		{
			HaI32Set::const_iterator it1(vertexToTriangles[v].begin()), itEnd(vertexToTriangles[v].end()); 
			for(; it1 != itEnd; ++it1)
			{
				t1 = *it1;
				i1 = m_triangles[t1].X();
				j1 = m_triangles[t1].Y();
				k1 = m_triangles[t1].Z();
				tr1[0] = GetEdgeIndex(i1, j1);
				tr1[1] = GetEdgeIndex(j1, k1);
				tr1[2] = GetEdgeIndex(k1, i1);
				HaI32Set::const_iterator it2(it1);
				for(++it2; it2 != itEnd; ++it2)
				{
					t2 = *it2;
					i2 = m_triangles[t2].X();
					j2 = m_triangles[t2].Y();
					k2 = m_triangles[t2].Z();
					tr2[0] = GetEdgeIndex(i2, j2);
					tr2[1] = GetEdgeIndex(j2, k2);
					tr2[2] = GetEdgeIndex(k2, i2);
					int shared = 0;
					for(int i = 0; i < 3; ++i)
					{
						for(int j = 0; j < 3; ++j)
						{
							if (tr1[i] == tr2[j])	
							{
								shared++;
							}
						}
					}
					if (shared == 1) // two triangles are connected if they share exactly one edge
					{
						m_graph.AddEdge(t1, t2);
					}
				}
			}
		}
		if (m_ccConnectDist >= 0.0)
		{
			m_graph.ExtractCCs();
			if (m_graph.m_nCCs > 1) 
			{
				STDNAME::vector< STDNAME::set<hacd::HaI32> > cc2V;
				cc2V.resize(m_graph.m_nCCs);
				hacd::HaI32 cc;
				for(hacd::HaU32 t = 0; t < m_nTriangles; ++t)
				{
					cc = m_graph.m_vertices[t].m_cc;
					cc2V[cc].insert(m_triangles[t].X());
					cc2V[cc].insert(m_triangles[t].Y());
					cc2V[cc].insert(m_triangles[t].Z());
				}

				for(hacd::HaU32 cc1 = 0; cc1 < m_graph.m_nCCs; ++cc1)
				{
					for(hacd::HaU32 cc2 = cc1+1; cc2 < m_graph.m_nCCs; ++cc2)
					{
						STDNAME::set<hacd::HaI32>::const_iterator itV1(cc2V[cc1].begin()), itVEnd1(cc2V[cc1].end()); 
						for(; itV1 != itVEnd1; ++itV1)
						{
							hacd::HaF64 distC1C2 = DBL_MAX;
							hacd::HaF64 dist;
							t1 = -1;
							t2 = -1;
							STDNAME::set<hacd::HaI32>::const_iterator itV2(cc2V[cc2].begin()), itVEnd2(cc2V[cc2].end()); 
							for(; itV2 != itVEnd2; ++itV2)
							{
								dist = (m_points[*itV1] - m_points[*itV2]).GetNorm();
								if (dist < distC1C2)
								{
									distC1C2 = dist;
									t1 = *vertexToTriangles[*itV1].begin();

									STDNAME::set<hacd::HaI32>::const_iterator it2(vertexToTriangles[*itV2].begin()), 
										it2End(vertexToTriangles[*itV2].end()); 
									t2 = -1;
									for(; it2 != it2End; ++it2)
									{
										if (*it2 != t1)
										{
											t2 = *it2;
											break;
										}
									}
								}
							}
							if (distC1C2 < m_ccConnectDist && t1 > 0 && t2 > 0)
							{
								m_graph.AddEdge(t1, t2);                    
							}
						}
					}
				}
			}
		}
	}
	void HACD::InitializeGraph()
	{
		hacd::HaI32 i, j, k;
		Vec3<hacd::HaF64> u, v, w, normal;
		PX_FREE(m_normals);
		m_normals = (Vec3<hacd::HaF64> *)PX_ALLOC(sizeof(Vec3<hacd::HaF64>)*m_nPoints, PX_DEBUG_EXP("HACD::InitializeGraph"));
		if (m_addFacesPoints)
		{
			PX_FREE(m_facePoints);
			PX_FREE(m_faceNormals);
			m_facePoints = (Vec3<hacd::HaF64> *)PX_ALLOC(sizeof(Vec3<hacd::HaF64>)*m_nTriangles, PX_DEBUG_EXP("HACD::InitializeGraph"));
			m_faceNormals = (Vec3<hacd::HaF64> *)PX_ALLOC(sizeof(Vec3<hacd::HaF64>)*m_nTriangles, PX_DEBUG_EXP("HACD::InitializeGraph"));
		}
		memset(m_normals, 0, sizeof(Vec3<hacd::HaF64>) * m_nPoints);
		for(hacd::HaU32 f = 0; f < m_nTriangles; f++)
		{
			i = m_triangles[f].X();
			j = m_triangles[f].Y();
			k = m_triangles[f].Z();
		
			m_graph.m_vertices[f].m_distPoints[i].m_distOnly = false;
			m_graph.m_vertices[f].m_distPoints[j].m_distOnly = false;
			m_graph.m_vertices[f].m_distPoints[k].m_distOnly = false;
			
			ICHull  * ch = PX_NEW(ICHull);
			m_graph.m_vertices[f].m_convexHull = ch;
			ch->AddPoint(m_points[i], i);
			ch->AddPoint(m_points[j], j);
			ch->AddPoint(m_points[k], k);
			ch->SetDistPoints(&m_graph.m_vertices[f].m_distPoints);

			u = m_points[j] - m_points[i];
			v = m_points[k] - m_points[i];
			w = m_points[k] - m_points[j];
			normal = u ^ v;

			m_normals[i] += normal;
			m_normals[j] += normal;
			m_normals[k] += normal;

			m_graph.m_vertices[f].m_surf = normal.GetNorm();
			m_graph.m_vertices[f].m_perimeter = u.GetNorm() + v.GetNorm() + w.GetNorm();
			
			normal.Normalize();

			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(i,j));
			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(j,k));
			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(k,i));
			if(m_addFacesPoints)
			{
				m_faceNormals[f] = normal;
				m_facePoints[f] = (m_points[i] + m_points[j] + m_points[k]) / 3.0;
				m_graph.m_vertices[f].m_distPoints[-static_cast<hacd::HaI32>(f)-1].m_distOnly = true;
			}
			if (m_addExtraDistPoints)	
			{// we need a kd-tree structure to accelerate this part!
				hacd::HaI32 i1, j1, k1;
				Vec3<hacd::HaF64> u1, v1, normal1;
				normal = -normal;
				hacd::HaF64 distance = 0.0;
				hacd::HaF64 distMin = 0.0;
				hacd::HaU32 faceIndex = m_nTriangles;
				Vec3<hacd::HaF64> seedPoint((m_points[i] + m_points[j] + m_points[k]) / 3.0);
				hacd::HaI32 nhit = 0;
				for(hacd::HaU32 f1 = 0; f1 < m_nTriangles; f1++)
				{
					i1 = m_triangles[f1].X();
					j1 = m_triangles[f1].Y();
					k1 = m_triangles[f1].Z();
					u1 = m_points[j1] - m_points[i1];
					v1 = m_points[k1] - m_points[i1];
					normal1 = (u1 ^ v1);
					if (normal * normal1 > 0.0)
					{
						nhit = IntersectRayTriangle(seedPoint, normal, m_points[i1], m_points[j1], m_points[k1], distance);
						if ((nhit==1) && ((distMin > distance) || (faceIndex == m_nTriangles)))
						{
							distMin = distance;
							faceIndex = f1;
						}

					}
				}
				if (faceIndex < m_nTriangles )
				{
					i1 = m_triangles[faceIndex].X();
					j1 = m_triangles[faceIndex].Y();
					k1 = m_triangles[faceIndex].Z();
					m_graph.m_vertices[f].m_distPoints[i1].m_distOnly = true;
					m_graph.m_vertices[f].m_distPoints[j1].m_distOnly = true;
					m_graph.m_vertices[f].m_distPoints[k1].m_distOnly = true;
					if (m_addFacesPoints)
					{
						m_graph.m_vertices[f].m_distPoints[-static_cast<hacd::HaI32>(faceIndex)-1].m_distOnly = true;
					}
				}
			}
		}
		for (hacd::HaU32 v = 0; v < m_nPoints; v++) 
		{
			m_normals[v].Normalize();
		}
	}

	void HACD::NormalizeData()
	{
		if (m_nPoints == 0)
		{
			return;
		}
		m_barycenter = m_points[0];
		Vec3<hacd::HaF64> min = m_points[0];
		Vec3<hacd::HaF64> max = m_points[0];
		hacd::HaF64 x, y, z;
		for (hacd::HaU32 v = 1; v < m_nPoints ; v++) 
		{
			m_barycenter += m_points[v];
			x = m_points[v].X();
			y = m_points[v].Y();
			z = m_points[v].Z();
			if ( x < min.X()) min.X() = x;
			else if ( x > max.X()) max.X() = x;
			if ( y < min.Y()) min.Y() = y;
			else if ( y > max.Y()) max.Y() = y;
			if ( z < min.Z()) min.Z() = z;
			else if ( z > max.Z()) max.Z() = z;
		}
		m_barycenter /= m_nPoints;
		m_diag = (max-min).GetNorm();
		const hacd::HaF64 invDiag = 2.0 * m_scale / m_diag;
		if (m_diag != 0.0)
		{
			for (hacd::HaU32 v = 0; v < m_nPoints ; v++) 
			{
				m_points[v] = (m_points[v] - m_barycenter) * invDiag;
			}
		}
	}
	void HACD::DenormalizeData()
	{
		if (m_nPoints == 0)
		{
			return;
		}
		if (m_diag != 0.0)
		{
			const hacd::HaF64 diag = m_diag / (2.0 * m_scale);
			for (hacd::HaU32 v = 0; v < m_nPoints ; v++) 
			{
				m_points[v] = m_points[v] * diag + m_barycenter;
			}
		}
	}
	HACD::HACD(void)
	{
		m_convexHulls = 0;
		m_triangles = 0;
		m_points = 0;
		m_normals = 0;
		m_nTriangles = 0;
		m_nPoints = 0;
		m_nClusters = 0;
		m_concavity = 0.0;
		m_diag = 1.0;
		m_barycenter = Vec3<hacd::HaF64>(0.0, 0.0,0.0);
		m_alpha = 0.1;
		m_beta = 0.1;
		m_nVerticesPerCH = 30;
		m_callBack = 0;
		m_addExtraDistPoints = false;
		m_addNeighboursDistPoints = false;
		m_scale = 1000.0;
		m_partition = 0;
		m_nMinClusters = 3;
		m_facePoints = 0;
		m_faceNormals = 0;
		m_ccConnectDist = 30;
	}																
	HACD::~HACD(void)
	{
		PX_FREE(m_normals);
		delete [] m_convexHulls;
		PX_FREE(m_partition);
		PX_FREE(m_facePoints);
		PX_FREE(m_faceNormals);
	}
	int iteration = 0;
	void HACD::ComputeEdgeCost(hacd::HaU32 e)
	{
		GraphEdge & gE = m_graph.m_edges[e];
		hacd::HaI32 v1 = gE.m_v1;
		hacd::HaI32 v2 = gE.m_v2;

		if (m_graph.m_vertices[v2].m_distPoints.size()>m_graph.m_vertices[v1].m_distPoints.size())
		{
			gE.m_v1 = v2;
			gE.m_v2 = v1;
			swap(v1, v2);
		}
		GraphVertex & gV1 = m_graph.m_vertices[v1];
		GraphVertex & gV2 = m_graph.m_vertices[v2];
	
		// delete old convex-hull
		delete gE.m_convexHull;
		// create the edge's convex-hull
		ICHull  * ch = PX_NEW(ICHull);
		gE.m_convexHull = ch;
		(*ch) = (*gV1.m_convexHull);
		
		// update distPoints
		gE.m_distPoints = gV1.m_distPoints;
		DPointMap::iterator itDP(gV2.m_distPoints.begin());
		DPointMap::iterator itDPEnd(gV2.m_distPoints.end());
		DPointMap::iterator itDP1;
	 
		for(; itDP != itDPEnd; ++itDP) 
		{
			itDP1 = gE.m_distPoints.find(itDP->first);
			if (itDP1 == gE.m_distPoints.end())
			{
				gE.m_distPoints[itDP->first].m_distOnly = (itDP->second).m_distOnly;
				if ( !(itDP->second).m_distOnly )
				{
					ch->AddPoint(m_points[itDP->first], itDP->first);
				}
			}
			else
			{
				if ( (itDP1->second).m_distOnly && !(itDP->second).m_distOnly)
				{
					gE.m_distPoints[itDP->first].m_distOnly = false;
					ch->AddPoint(m_points[itDP->first], itDP->first);
				}
			}
		}
		
		ch->SetDistPoints(&gE.m_distPoints);
		// create the convex-hull
		while (ch->Process() == ICHullErrorInconsistent)		// if we face problems when constructing the visual-hull. really ugly!!!!
		{
//			if (m_callBack) (*m_callBack)("\t Problem with convex-hull construction [HACD::ComputeEdgeCost]\n", 0.0, 0.0, 0);
			ch = PX_NEW(ICHull);
			CircularList<TMMVertex> & verticesCH = (gE.m_convexHull)->GetMesh().m_vertices;
			hacd::HaU32 nV = (hacd::HaU32)verticesCH.GetSize();
			hacd::HaI32 ptIndex = 0;
			verticesCH.Next();
			for(hacd::HaU32 v = 1; v < nV; ++v)
			{
				ptIndex = verticesCH.GetHead()->GetData().m_name;
				ch->AddPoint(m_points[ptIndex], ptIndex);
				verticesCH.Next();
			}
			delete gE.m_convexHull;
			gE.m_convexHull = ch;
		}
		hacd::HaF64 volume = 0.0; 
		hacd::HaF64 concavity = 0.0;
		if (ch->IsFlat())
		{
			bool insideHull;
			DPointMap::iterator itDP(gE.m_distPoints.begin());
			DPointMap::iterator itDPEnd(gE.m_distPoints.end());
			for(; itDP != itDPEnd; ++itDP) 
			{	
				if (itDP->first >= 0)
				{
					concavity = max(concavity, ch->ComputeDistance(itDP->first, m_points[itDP->first], m_normals[itDP->first], insideHull, false));
				}
			}
		}
		else
		{
			if (m_addNeighboursDistPoints)
			{  // add distance points from adjacent clusters
				HaI32Set eEdges;
				set_union(gV1.m_edges.begin(), 
					gV1.m_edges.end(), 
					gV2.m_edges.begin(), 
					gV2.m_edges.end(),
					eEdges );
				HaI32Set::const_iterator ed(eEdges.begin());
				HaI32Set::const_iterator itEnd(eEdges.end());
				hacd::HaI32 a, b, c;
				for(; ed != itEnd; ++ed) 
				{
					a = m_graph.m_edges[*ed].m_v1;
					b = m_graph.m_edges[*ed].m_v2;
					if ( a != v2 && a != v1)
					{
						c = a;
					}
					else if ( b != v2 && b != v1)
					{
						c = b;
					}
					else
					{
						c = -1;
					}
					if ( c > 0)
					{
						GraphVertex & gVC = m_graph.m_vertices[c];
						DPointMap::iterator itDP(gVC.m_distPoints.begin());
						DPointMap::iterator itDPEnd(gVC.m_distPoints.end());
						DPointMap::iterator itDP1;
						for(; itDP != itDPEnd; ++itDP) 
						{
							itDP1 = gE.m_distPoints.find(itDP->first);
							if (itDP1 == gE.m_distPoints.end())
							{
								if (itDP->first >= 0 && itDP1 == gE.m_distPoints.end() && ch->IsInside(m_points[itDP->first]))
								{
									gE.m_distPoints[itDP->first].m_distOnly = true;
								}
								else if (itDP->first < 0 && ch->IsInside(m_facePoints[-itDP->first-1]))
								{
									gE.m_distPoints[itDP->first].m_distOnly = true;
								}
							}
						}
					}
				}
			}
			concavity = Concavity(*ch, gE.m_distPoints);
		}
  
		// compute boudary edges
		hacd::HaF64 perimeter = 0.0;
		hacd::HaF64 surf    = 1.0;
		if (m_alpha > 0.0)
		{
			gE.m_boudaryEdges.clear();
			set_symmetric_difference (gV1.m_boudaryEdges, 
								  gV2.m_boudaryEdges, 
								  gE.m_boudaryEdges );
			HaU64Set::const_iterator itBE(gE.m_boudaryEdges.begin());
			HaU64Set::const_iterator itBEEnd(gE.m_boudaryEdges.end());
			for(; itBE != itBEEnd; ++itBE)
			{
					perimeter += (m_points[static_cast<hacd::HaI32>((*itBE) >> 32)] - 
								   m_points[static_cast<hacd::HaI32>((*itBE) & 0xFFFFFFFFULL)]).GetNorm();
			}
			surf    = gV1.m_surf + gV2.m_surf;
		}
		hacd::HaF64 ratio   = perimeter * perimeter / (4.0 * sc_pi * surf);
		gE.m_volume	   = (m_beta == 0.0)?0.0:ch->ComputeVolume()/pow(m_scale, 3.0);						// cluster's volume
		gE.m_surf      = surf;                          // cluster's area  
		gE.m_perimeter = perimeter;                     // cluster's perimeter
		gE.m_concavity = concavity;                     // cluster's concavity
		gE.m_error     = concavity +  m_alpha * ratio + m_beta * volume;	// cluster's priority
	}
	bool HACD::InitializePQ()
	{
		m_pqueue.reserve( (hacd::HaU32)(m_graph.m_nE + 100));
		for (hacd::HaU32 e=0; e < m_graph.m_nE; ++e) 
		{
			ComputeEdgeCost(e);
			m_pqueue.push(GraphEdgePQ(e, m_graph.m_edges[e].m_error));
		}
		return true;
	}
	void HACD::Simplify()
	{
		hacd::HaI32 v1 = -1;
		hacd::HaI32 v2 = -1;        
		hacd::HaF64 globalConcavity  = 0.0;

#if defined(PX_WINDOWS)
		hacd::HaF64 progress = 0.0;
#endif

		while ( (globalConcavity < m_concavity) && 
				(m_graph.GetNVertices() > m_nMinClusters) && 
				(m_graph.GetNEdges()> 0))   // Revise #125
		{

#if defined(PX_WINDOWS)
			hacd::HaF64 progressOld = -1.0;
			hacd::HaF64 ptgStep = 1.0;
			char msg[1024];

			progress = 100.0-m_graph.GetNVertices() * 100.0 / m_nTriangles;
			
			if (fabs(progress-progressOld) > ptgStep && m_callBack)
			{
				sprintf_s(msg,1024, "%3.2f %% V = %lu \t C = %f \t \t \r", progress, static_cast<hacd::HaU32>(m_graph.GetNVertices()), globalConcavity);
				m_callBack->progressUpdate(msg, progress, globalConcavity,  m_graph.GetNVertices());
				progressOld = progress;
				if (progress > 99.0)
				{
					ptgStep = 0.01;
				}
				else if (progress > 90.0)
				{
					ptgStep = 0.1;
				}
			}
#endif

			GraphEdgePQ currentEdge(0,0.0);
			bool done = false;
			do
			{
				done = false;
				if (m_pqueue.size() == 0)
				{
					done = true;
					break;
				}
				else
				{
					currentEdge = m_pqueue.top();
					m_pqueue.pop();
				}
			}
			while (  m_graph.m_edges[currentEdge.m_name].m_deleted || 
					 m_graph.m_edges[currentEdge.m_name].m_error != currentEdge.m_priority);

			if (m_graph.m_edges[currentEdge.m_name].m_concavity < m_concavity && !done)
			{
				globalConcavity = max(globalConcavity ,m_graph.m_edges[currentEdge.m_name].m_concavity);
				v1 = m_graph.m_edges[currentEdge.m_name].m_v1;
				v2 = m_graph.m_edges[currentEdge.m_name].m_v2;	
				// update vertex info
				m_graph.m_vertices[v1].m_error     = m_graph.m_edges[currentEdge.m_name].m_error;
				m_graph.m_vertices[v1].m_surf	   = m_graph.m_edges[currentEdge.m_name].m_surf;
				m_graph.m_vertices[v1].m_volume	   = m_graph.m_edges[currentEdge.m_name].m_volume;
				m_graph.m_vertices[v1].m_concavity = m_graph.m_edges[currentEdge.m_name].m_concavity;
				m_graph.m_vertices[v1].m_perimeter = m_graph.m_edges[currentEdge.m_name].m_perimeter;
				m_graph.m_vertices[v1].m_distPoints   = m_graph.m_edges[currentEdge.m_name].m_distPoints;
				(*m_graph.m_vertices[v1].m_convexHull) = (*m_graph.m_edges[currentEdge.m_name].m_convexHull);
				(m_graph.m_vertices[v1].m_convexHull)->SetDistPoints(&(m_graph.m_vertices[v1].m_distPoints));
				m_graph.m_vertices[v1].m_boudaryEdges   = m_graph.m_edges[currentEdge.m_name].m_boudaryEdges;
				
				// We apply the optimal ecol
				m_graph.EdgeCollapse(v1, v2);
				// recompute the adjacent edges costs
				HaI32Set::const_iterator itE(m_graph.m_vertices[v1].m_edges.begin()), 
											   itEEnd(m_graph.m_vertices[v1].m_edges.end());
				for(; itE != itEEnd; ++itE)
				{
					hacd::HaU32 e = *itE;
					ComputeEdgeCost(e);
					m_pqueue.push(GraphEdgePQ(e, m_graph.m_edges[e].m_error));
				}
			}
			else
			{
				break;
			}
		}
		while (!m_pqueue.empty())
		{
			m_pqueue.pop();
		}
		
		m_cVertices.clear();
		m_nClusters = m_graph.GetNVertices();
		m_cVertices.reserve(m_nClusters);
		for (hacd::HaU32 v = 0; v != m_graph.m_vertices.size(); ++v) 
		{
			if (!m_graph.m_vertices[v].m_deleted)
			{
#if defined(PX_WINDOWS)
				if (m_callBack) 
				{
					char msg[1024];
					sprintf_s(msg,1024, "\t CH \t %lu \t %lf \t %lf\n", static_cast<hacd::HaU32>(v), m_graph.m_vertices[v].m_concavity, m_graph.m_vertices[v].m_error);
					m_callBack->progressUpdate(msg, 0.0, 0.0, m_nClusters);
				}
#endif
				m_cVertices.push_back(v);			
			}
		}
#if defined(PX_WINDOWS)
		if (m_callBack)
		{
			char msg[1024];
			sprintf_s(msg,1024, "# clusters =  %lu \t C = %f\n", static_cast<hacd::HaU32>(m_nClusters), globalConcavity);
			m_callBack->progressUpdate(msg, progress, globalConcavity,  m_graph.GetNVertices());
		}
#endif
	}
	bool HACD::Compute(bool connectCCs, bool fullCH, bool exportDistPoints)
	{
		if ( !m_points || !m_triangles || !m_nPoints || !m_nTriangles)
		{
			return false;
		}
		hacd::HaU32 nV = m_nTriangles;
		if (m_callBack) m_callBack->progressUpdate("+ Normalizing Data\n", 0.0, 0.0, nV);
		NormalizeData();
		if (m_callBack) m_callBack->progressUpdate("+ Creating Graph\n", 0.0, 0.0, nV);
		CreateGraph();
		// Compute the surfaces and perimeters of all the faces
		if (m_callBack) m_callBack->progressUpdate("+ Initializing Graph\n", 0.0, 0.0, nV);
		InitializeGraph();
		if (m_callBack) m_callBack->progressUpdate("+ Initializing PQ\n", 0.0, 0.0, nV);
		InitializePQ();
		// we simplify the graph		
		if (m_callBack) m_callBack->progressUpdate("+ Simplification ...\n", 0.0, 0.0, m_nTriangles);
		Simplify();
		if (m_callBack) m_callBack->progressUpdate("+ Denormalizing Data\n", 0.0, 0.0, m_nClusters);
		DenormalizeData();
		if (m_callBack) m_callBack->progressUpdate("+ Computing final convex-hulls\n", 0.0, 0.0, m_nClusters);
		delete [] m_convexHulls;
		m_convexHulls = PX_NEW(ICHull)[m_nClusters];
		PX_FREE(m_partition);
		m_partition = (hacd::HaI32 *)PX_ALLOC(sizeof(hacd::HaI32)*m_nTriangles, PX_DEBUG_EXP("HACD::Compute"));
		for (hacd::HaU32 p = 0; p != m_cVertices.size(); ++p) 
		{
			hacd::HaU32 v = m_cVertices[p];
			m_partition[v] = p;
			for(hacd::HaU32 a = 0; a < m_graph.m_vertices[v].m_ancestors.size(); a++)
			{
				m_partition[m_graph.m_vertices[v].m_ancestors[a]] = p;
			}
			// compute the convex-hull
			const DPointMap & pointsCH =  m_graph.m_vertices[v].m_distPoints;
			DPointMap::const_iterator itCH(pointsCH.begin());
			DPointMap::const_iterator itCHEnd(pointsCH.end());
			for(; itCH != itCHEnd; ++itCH) 
			{
				if (!(itCH->second).m_distOnly)
				{
					m_convexHulls[p].AddPoint(m_points[itCH->first], itCH->first);
				}
			}
			m_convexHulls[p].SetDistPoints(&m_graph.m_vertices[v].m_distPoints);
			if (fullCH)
			{
				m_convexHulls[p].Process();
			}
			else
			{
				m_convexHulls[p].Process(m_nVerticesPerCH);
			}
			if (exportDistPoints)
			{
				itCH = pointsCH.begin();
				for(; itCH != itCHEnd; ++itCH) 
				{
					if ((itCH->second).m_distOnly)
					{
						if (itCH->first >= 0)
						{
							m_convexHulls[p].AddPoint(m_points[itCH->first], itCH->first);
						}
						else
						{
							m_convexHulls[p].AddPoint(m_facePoints[-itCH->first-1], itCH->first);
						}
					}
				}
			}
		}       
		return true;
	}
	
	hacd::HaU32 HACD::GetNTrianglesCH(hacd::HaU32 numCH) const
	{
		if (numCH >= m_nClusters)
		{
			return 0;
		}
		return m_convexHulls[numCH].GetMesh().GetNTriangles();
	}
	hacd::HaU32 HACD::GetNPointsCH(hacd::HaU32 numCH) const
	{
		if (numCH >= m_nClusters)
		{
			return 0;
		}
		return m_convexHulls[numCH].GetMesh().GetNVertices();
	}

	bool HACD::GetCH(hacd::HaU32 numCH, Vec3<hacd::HaF64> * const points, Vec3<hacd::HaI32> * const triangles)
	{
		if (numCH >= m_nClusters)
		{
			return false;
		}
		m_convexHulls[numCH].GetMesh().GetIFS(points, triangles);
		return true;
	}


void	ReservablePriorityQueue::reserve(hacd::HaU32 size)
{
	PX_FORCE_PARAMETER_REFERENCE(size);
}

void	ReservablePriorityQueue::push(const GraphEdgePQ &pq)
{
	mQueue.insert(pq);
}

hacd::HaU32	ReservablePriorityQueue::size(void) const
{
	return (hacd::HaU32)mQueue.size();
}

const GraphEdgePQ &	ReservablePriorityQueue::top(void)
{
	GraphEdgePQSet::const_iterator ret = mQueue.begin();
	return (*ret);
}

void	ReservablePriorityQueue::pop(void)
{
	GraphEdgePQSet::iterator ret = mQueue.begin();
	mQueue.erase(ret);
}

bool	ReservablePriorityQueue::empty(void) const
{
	return mQueue.empty();
}

}; // end of HACD namespace
