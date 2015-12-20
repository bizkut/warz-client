/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


namespace Kaim
{

KY_INLINE Database*                       Bot::GetDatabase()                   const { return m_database;                                                   }
KY_INLINE const LivePath&                 Bot::GetLivePath()                   const { return m_livePath;                                                   }
KY_INLINE const PathEventList&            Bot::GetPathEventList()              const { return m_livePath.GetPathEventList();                                }
KY_INLINE ChannelArray*                   Bot::GetChannelArray()               const { return m_livePath.GetChannelArray();                                 }
KY_INLINE const PositionOnLivePath&       Bot::GetProgressOnLivePath()         const { return m_progressOnLivePath;                                         }
KY_INLINE BasePathProgressComputer*       Bot::GetPathProgressComputer()       const { return m_pathProgressComputer;                                       }
KY_INLINE ITrajectory*                    Bot::GetTrajectory()                 const { return m_trajectory;                                                 }
KY_INLINE Path*                           Bot::GetFollowedPath()               const { return m_livePath.GetFollowedPath();                                 }
KY_INLINE PathSource                      Bot::GetFollowedPathSource()         const { return m_livePath.GetFollowedPathSource();                           }
KY_INLINE const PathValidityInterval&     Bot::GetPathValidityInterval()       const { return m_livePath.m_validityInterval;                                }
KY_INLINE bool                            Bot::IsComputingNewPath()            const { return m_livePath.IsComputingNewPath();                              }
KY_INLINE bool                            Bot::IsPathEventListBuildCompleted() const { return GetPathEventList().IsBuildCompleted();                        }
KY_INLINE const SpatializedCylinder&      Bot::GetSpatializedCylinder()        const { return m_spatializedCylinder;                                        }
KY_INLINE const Vec3f&                    Bot::GetPosition()                   const { return m_spatializedCylinder.GetPosition();                          }
KY_INLINE const Vec3f&                    Bot::GetFrontDirection()             const { return m_nextUpdateConfig.m_frontDirection ;                         }
KY_INLINE const Vec3f&                    Bot::GetVelocity()                   const { return m_spatializedCylinder.GetVelocity();                          }
KY_INLINE const Vec3f&                    Bot::GetPreviousPosition()           const { return m_previousPosition;                                           }
KY_INLINE ProgressOnPathStatus            Bot::GetProgressOnLivePathStatus()   const { return m_progressOnPathStatus;                                       }

KY_INLINE KyFloat32                       Bot::GetRadius()                     const { return m_botConfig->m_radius;                                        }
KY_INLINE KyFloat32                       Bot::GetHeight()                     const { return m_botConfig->m_height;                                        }
KY_INLINE TrajectoryMode                  Bot::GetTrajectoryMode()             const { return m_botConfig->m_trajectoryMode;                                }
KY_INLINE KyFloat32                       Bot::GetMaxDesiredLinearSpeed()      const { return m_botConfig->m_maxDesiredLinearSpeed;                         }
KY_INLINE bool                            Bot::GetEnableAvoidance()            const { return m_botConfig->m_enableAvoidance;                               }
KY_INLINE KyFloat32                       Bot::GetTrajectoryMinimalMoveDistance() const { return m_botConfig->m_trajectoryMinimalMoveDistance;              }

KY_INLINE const BotConfig&                Bot::GetConfig()                     const { return *m_botConfig;                                                 }
KY_INLINE const PathProgressConfig&       Bot::GetPathProgressConfig()         const { return m_botConfig->m_pathProgressConfig;                            }
KY_INLINE const ShortcutTrajectoryConfig& Bot::GetShortcutTrajectoryConfig()   const { return m_botConfig->m_shortcutTrajectoryConfig;                      }
KY_INLINE const ChannelTrajectoryConfig&  Bot::GetChannelTrajectoryConfig()    const { return m_botConfig->m_channelTrajectoryConfig;                       }
KY_INLINE const ColliderCollectorConfig&  Bot::GetColliderCollectorConfig()    const { return m_botConfig->m_colliderCollectorConfig;                       }
KY_INLINE const AvoidanceConfig&          Bot::GetAvoidanceConfig()            const { return m_botConfig->m_avoidanceConfig;                               }

KY_INLINE bool                            Bot::GetDoComputeProgressOnPath()     const { return m_doComputeProgressOnPath;                                     }
KY_INLINE bool                            Bot::GetDoValidateCheckPoint()       const { return m_doValidateCheckPoint;                                       }
KY_INLINE bool                            Bot::GetDoComputeTrajectory()        const { return m_doComputeTrajectory;                                        }

// Set the Bot TrajectoryMinimalMoveDistance, nothing in particular will be recomputed after this call.
KY_INLINE void Bot::SetTrajectoryMinimalMoveDistance(KyFloat32 trajectoryMinimalMoveDistance) { m_botConfig->m_trajectoryMinimalMoveDistance = trajectoryMinimalMoveDistance; }

// not KY_INLINE to avoid weird compilation error
inline NavTrianglePtr Bot::GetNavTrianglePtr() const { return m_spatializedCylinder.GetNavTrianglePtr(m_database); }

KY_INLINE void     Bot::SetNewPathNavigationProfileId(KyUInt32 navigationProfileId) { m_newPathToDestNavigationProfileId = navigationProfileId; }
KY_INLINE KyUInt32 Bot::GetNewPathNavigationProfileId() const { return m_newPathToDestNavigationProfileId;             }

KY_INLINE void* Bot::GetBotTraverseLogicUserData() const { return m_traverseLogicUserData; }
KY_INLINE void Bot::SetBotTraverseLogicUserData(void* traverseLogicUserData) { m_traverseLogicUserData = traverseLogicUserData; }

KY_INLINE void           Bot::CancelAsyncPathComputation()  { m_livePath.CancelAsynPathComputation();      }
KY_INLINE void           Bot::InjectNewPath(Path* newPath)  { m_livePath.InjectUserPath(newPath);          }
KY_INLINE void           Bot::FlagPathAsPotentiallyDirty()  { m_livePath.ForceValidityIntervalRecompute(); }
KY_INLINE LivePath&      Bot::GetLivePath()                 { return m_livePath;                           }
KY_INLINE PathEventList& Bot::GetPathEventList()            { return m_livePath.GetPathEventList();        }

KY_INLINE IPathFinderQuery* Bot::GetPathFinderQuery() { return m_pathFinderQuery; }

KY_INLINE KyResult Bot::ComputeNewPathToDestination(const Vec3f& destPos) { return ComputeNewPathToDestination(destPos, NavGraphEdgePtr()); }

KY_INLINE void Bot::ComputeNewPathAsync(IPathFinderQuery* pathFinderQuery) { m_livePath.ComputeNewPathAsync(pathFinderQuery); }

KY_INLINE void Bot::SetDoValidateCheckPoint(bool doValidateCheckPoint) { m_doValidateCheckPoint = doValidateCheckPoint; }

KY_INLINE void Bot::SetChannelPathNeedToBeRecomputed(bool needToRecomputeChannel)
{
	if (needToRecomputeChannel)
		m_botNeedFullComputeFlags.ChannelHasBeenInvalidated();
	else
		m_botNeedFullComputeFlags.PathHasChanged();
}

KY_INLINE bool Bot::GetNeedToRecomputeChannelPath() const { return m_botNeedFullComputeFlags.GetChannelNeedFullCompute(); }

template<class TraverseLogic>
inline Vec3f Bot::ComputeMoveOnNavMeshWithTraverseLogic(const Kaim::Vec3f& velocity, KyFloat32 simulationTimeInSeconds)
{
	BotMoveOnNavMesh botMoveOnNavMesh;
	botMoveOnNavMesh.m_currentPosition         = GetPosition();
	botMoveOnNavMesh.m_velocity                = velocity; 
	botMoveOnNavMesh.m_simulationTimeInSeconds = simulationTimeInSeconds;
	botMoveOnNavMesh.m_database                = GetDatabase();
	botMoveOnNavMesh.m_traverseLogicUserData   = GetBotTraverseLogicUserData();
	botMoveOnNavMesh.m_navTrianglePtr          = GetNavTrianglePtr();

	return BotMoveOnNavMesh::ComputeMoveOnNavMesh<TraverseLogic>(botMoveOnNavMesh);
}


KY_INLINE void Bot::UpdatePosition       (const Vec3f& position)       { m_nextUpdateConfig.m_position = position; m_positionUpdated = true; }
KY_INLINE void Bot::UpdateVelocity       (const Vec3f& velocity)       { m_nextUpdateConfig.m_velocity = velocity; m_velocityUpdated = true; }
KY_INLINE void Bot::UpdateFrontDirection (const Vec3f& frontDirection) { m_nextUpdateConfig.m_frontDirection = frontDirection; m_frontDirectionUpdated = true;  }

KY_INLINE void Bot::SetNextUpdateConfig(const BotUpdateConfig& updateConfig) 
{ 
	m_nextUpdateConfig = updateConfig;
	m_positionUpdated = true;
	m_velocityUpdated = true;
	m_frontDirectionUpdated = true;
}

}
