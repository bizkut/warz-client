/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NONE
#ifndef Navigation_Bot_H
#define Navigation_Bot_H


#include "gwnavruntime/world/worldelement.h"
#include "gwnavruntime/world/botconfig.h"
#include "gwnavruntime/spatialization/spatializedcylinder.h"
#include "gwnavruntime/path/livepath.h"
#include "gwnavruntime/path/positiononlivepath.h"
#include "gwnavruntime/path/ipatheventlistobserver.h"
#include "gwnavruntime/pathfollower/ipositiononpathvalidator.h"
#include "gwnavruntime/pathfollower/avoidanceconfig.h"
#include "gwnavruntime/pathfollower/manualcontrolinterval.h"
#include "gwnavruntime/pathfollower/resettrajectorystatus.h"
#include "gwnavruntime/channel/channelsectionptr.h"
#include "gwnavruntime/world/basenavigationprofile.h"
#include "gwnavruntime/world/botneedfullcomputeflags.h"
#include "gwnavruntime/world/botmoveonnavmesh.h"
#include "gwnavruntime/world/progressonpathstatus.h"

namespace Kaim
{

class Database;
class IPathEventListObserver;
class IMovePositionOnPathPredicate;
class IPathFinderQuery;





/// Class used to provide Bot initialization parameters.
class BotInitConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	BotInitConfig() { SetDefaults(); }

	/// Sets all members to their default value. After calling this method, you
	/// must set at least the mandatory m_database before using this BotInitConfig
	/// to initialize a Bot.
	void SetDefaults()
	{
		m_userData = KY_NULL;
		m_database = KY_NULL;
		m_databaseBinding = KY_NULL;
		m_startPosition.Set(0.0f, 0.0f, 0.0f);
		m_startMovingDirection.Set(1.0f, 0.0f, 0.0f);
		m_startLinearSpeed = 0.0f;
		m_startNewPathNavigationProfileId = 0;
	}


	// ---------------------------------- Public Data Members ----------------------------------

	void* m_userData;

	/// Mandatory: you must provide a valid Database on which the Bot will move.
	Database* m_database;

	/// The Databases where the Bot will be spatialized. If set to KY_NULL, the
	/// Bot will use World default DatabaseBinding that spatializes on all Databases.
	Ptr<DatabaseBinding> m_databaseBinding;

	/// NavigationProfile that will be used in the ComputeNewPathToDestination() calls.
	KyUInt32 m_startNewPathNavigationProfileId;

	Vec3f     m_startPosition;
	Vec3f     m_startMovingDirection;
	KyFloat32 m_startLinearSpeed;
};


/// Class used to provide Bot new position and velocity to be integrated at next update.
class BotUpdateConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	BotUpdateConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults()
	{
		m_position.Set(0.0f, 0.0f, 0.0f);
		m_velocity.Set(1.0f, 0.0f, 0.0f);
		m_frontDirection.Set(0.0f, 0.0f, 0.0f);
	}

	// ---------------------------------- Public Data Members ----------------------------------
	Vec3f m_position;       //< As Gameware Navigation never moves a bot, we require you to update the bot position each frame using UpdatePosition or SetNextUpdate
	Vec3f m_velocity;       //< The actual velocity of your entity migh differ from the one we gave you in the trajectory because of physics, animation, gameplay... We use this velocity for avoidance purposes.
	Vec3f m_frontDirection; //< Optional: If you are using the Channel splines follow, then your entities might face a direction that is different from the velocity. We use this information to compute the splines at startup.
};


/// This class is the world element that represent an active character in
/// Gameware Navigation. It manages character pathfinding and pathfollowing.
class Bot : public WorldElement
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)
	KY_CLASS_WITHOUT_COPY(Bot)

public:
	static  WorldElementType GetStaticType() { return TypeBot; }
	virtual WorldElementType GetType() const { return TypeBot; }

	virtual const char* GetTypeName() const { return "Bot"; }

	// ---------------------------------- Main API Functions ----------------------------------

	Bot();

	/// calls Init(initConfig)
	Bot(const BotInitConfig& initConfig);

	/// calls Init(initConfig, botConfig)
	Bot(const BotInitConfig& initConfig, const BotConfig& botConfig);

	virtual ~Bot();

	/// uses a default constructed BotConfig
	void Init(const BotInitConfig& initConfig);

	/// Initiates the Bot, but does not add it to the world.
	/// If m_database is already set, Clear() is called to clean this
	/// instance up and prepare it for re-use.
	void Init(const BotInitConfig& initConfig, const BotConfig& botConfig);

	/// Prepares this instance for destruction and / or reuse: sets all members
	/// to their default values and clears: the spatialization, the path, the trajectory
	void Clear();

	/// Adds the Bot to the Database (and thus to the World). It becomes
	/// active in the simulation, it is updated, and it is taken into account by other Bots.
	/// This increments the RefCount of this instance.
	/// NOTE: this method has immediate effect: ensure to call this outside of any
	/// Gameware Navigation computation (World::Update() or threaded query
	/// computation). However, the spatialization is not updated immediately, this
	/// will be done in next World::Update().
	void AddToDatabase();

	/// Removes the Bot from the Database (and thus from the World). It is removed
	/// from the simulation, updates stop, and it is ignored by other Bots.
	/// This decrements the RefCount of this instance.
	/// NOTE: this method has immediate effect: ensure to call this outside of any
	/// Gameware Navigation computation (Kaim::World::Update or threaded query
	/// computation). The spatialization information are immediately invalidated too.
	void RemoveFromDatabase();

	/// Mandatory: Call UpdatePosition or SetNextUpdateConfig each frame
	/// This method set the next update config position
	void UpdatePosition(const Vec3f& position);

	/// Optional: This method set the next update config velocity
	/// If you do not call UpdateVelocity or SetNextUpdateConfig, we will automatically compute the velocity based on the previous position
	void UpdateVelocity(const Vec3f& velocity);

	/// Optional: This method set the next update config front direction
	/// If you do not call UpdateFrontDirection or SetNextUpdateConfig, we will automatically compute the front direction to match the direction of the last non null velocity
	void UpdateFrontDirection(const Vec3f& frontDirection);

	/// This method has to be called regularly to update Bot position and velocity.
	/// Ideally it is called each time the corresponding character is updated in
	/// the game engine.
	/// The Bot internals are not updated immediately: the provided BotUpdateConfig
	/// is copied to be used at next World::Update to actually update the Bot internals.
	void SetNextUpdateConfig(const BotUpdateConfig& updateConfig);


	// ---------------------------------- Getters ----------------------------------

	const Vec3f&                GetPosition()                 const; /// the position passed by the user in SetNextUpdateConfig()
	const Vec3f&                GetFrontDirection()           const; /// the moving direction passed by the user in SetNextUpdateConfig()
	const Vec3f&                GetVelocity()                 const; /// the velocity vector computed from MovingDirection & LinearSpeed passed by the user in SetNextUpdateConfig()
	NavTrianglePtr              GetNavTrianglePtr()           const;

	Database*                   GetDatabase()                 const;
	IPathFinderQuery*           GetPathFinderQuery();
	BasePathProgressComputer*   GetPathProgressComputer()     const;
	ITrajectory*                GetTrajectory()               const;

	const PositionOnLivePath&   GetProgressOnLivePath()       const;
	ProgressOnPathStatus        GetProgressOnLivePathStatus() const;
	static const char*          GetProgressOnLivePathStatusDescription(ProgressOnPathStatus status);
	
	/// Return the remaining distance from the ProgressOnPath to the end of the followed path along the path.
	/// It can return KyFloat32MAXVAL if there's no path or GetProgressOnLivePathStatus is not valid
	KyFloat32 GetRemainingDistanceFromProgressToEndOfPath() const;

	const ChannelSectionPtr&    GetChannelSectionPtr()        const;

	const SpatializedCylinder&  GetSpatializedCylinder()      const;

	/// Returns Bot previous position. This can be used to achieve much more precise
	/// arrival detection and check point validation.
	const Vec3f&                GetPreviousPosition()         const;

	// ---------------------------------- Basic PathFinding functions ----------------------------------
	/// These functions are used to have the bot finding a Path very easily by only providing your AstarQuery
	/// and then the destination without going into the details of the different available Pathfinder queries
	/// and all their parameters. As an helper, they are typically  used during your first integration steps.

	/// Sets the NavigationProfile that will be used in the newt ComputeNewPathToDestination() and InitAStarQuery() calls.
	/// SetNewPathToDestNavigationProfile() does not compute any new path, an explicit call to ComputeNewPathToDestination() is necessary.
	void SetNewPathNavigationProfileId(KyUInt32 navigationProfileId);
	KyUInt32 GetNewPathNavigationProfileId() const;

	/// Launches an asynchronous path computation from the Bot position to destPos. If the Bot is already computing
	/// a Path, this computation is canceled, and a we try to launch a new path computation.
	/// The launched PathFinderQuery will be an instance returned by the CreateAStarQuery() fonction of the current NavigationProfile.
	/// If the AStarQuery cannot be initialized because of the Query Framework (We may have to wait for the next World::Update()
	/// for the query to be flushed from the QueryQueue), this function returns KY_ERROR. In this case, you could try to call it next frame.
	/// Note that this function calls internally the ComputeNewPathAsync function. If you had previously called ComputeNewPathAsync() with a
	/// custom IPathFinderQuery then it will be canceled and replaced by the AStarQuery.
	/// Optionally, you can force the path to start with a given NavGraph edge.
	KyResult ComputeNewPathToDestination(const Vec3f& destPos);
	KyResult ComputeNewPathToDestination(const Vec3f& destPos, const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode = NAVGRAPHEDGE_BIDIRECTIONAL);


	// ---------------------------------- Advanced PathFinding functions ----------------------------------
	/// These functions should be used for advanced path finding scenarios. With them you can set the IPathFinder query
	/// you want (not only an AStarQuery). You have to initialize your Query by yourself before calling the ComputeNewPathAsync()
	/// function and you are responsible for cheching the computation status (See IsComputingNewPath()) before calling ComputeNewPathAsync.

	/// Initialize a BaseAStarQuery that works is properly initialized for the Bot.
	/// This means calling BindToDatabase(), Initialize(start, dest), SetTraverseLogicUserData()
	/// and setting the SetComputeChannelMode, ChannelComputerConfig, PathFinderConfig.
	KyResult InitAStarQueryForBot(Ptr<BaseAStarQuery>& astarQuery, const Vec3f& destPos);

	/// Initialize a BaseAStarQuery that works is properly initialized for the Bot.
	/// This means calling BindToDatabase(), Initialize(start, dest), SetTraverseLogicUserData()
	/// and setting the SetComputeChannelMode, ChannelComputerConfig, PathFinderConfig.
	KyResult InitAStarQueryForBot(Ptr<BaseAStarQuery>& astarQuery, const Vec3f& destPos,
		const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode = NAVGRAPHEDGE_BIDIRECTIONAL);

	/// Get the NavigationProfile of the currently followed Path.
	BaseNavigationProfile* GetFollowedPathNavigationProfile();

	void* GetBotTraverseLogicUserData() const;
	void SetBotTraverseLogicUserData(void* traverseLogicUserData);

	/// Launches a new path computation with a pathFinderQuery.
	/// WARNING : This query must have been bound to the database in which the bot has been added and must be
	/// initialized, and your bot must not be computing a path (See IsComputingNewPath()).
	/// If you want to force a recomputation while this is running, you must explicitely call CancelAsyncPathComputation() first.
	/// \pre The bot must not be computing a Path. See IsComputingNewPath() function.
	void ComputeNewPathAsync(IPathFinderQuery* pathFinderQuery);

	/// Set a new Path, considered as the path to follow, at the next World::Update.
	/// Note : Your bot must not be computing a new path when this function is
	/// called. If it is, cancel the path computation before calling this function,
	/// to prevent the provided path from being overwritten by the path computation
	/// when it completes.
	/// You can pass KY_NULL to end path following, but non-null paths with 0 nodes
	/// will be rejected.
	void InjectNewPath(Path* newPath);

	/// Cancel the current path computation. You must call this method each time
	/// you want to compute a new path whereas the previous path computation query
	/// has not been completed.
	void CancelAsyncPathComputation();

	/// Call this method each time you want to compute a new Path. If it returns
	/// true you must call ClearFollowedPath() before querying a new path computation
	/// (or wait for the previous query completion).
	bool IsComputingNewPath() const;


	// ---------------------------------- LivePath Management ----------------------------------

	const LivePath&             GetLivePath()             const;
	const PathEventList&        GetPathEventList()        const;
	const PathValidityInterval& GetPathValidityInterval() const;
	Path*                       GetFollowedPath()         const;
	PathSource                  GetFollowedPathSource()   const;
	ChannelArray*               GetChannelArray()         const;

	/// A path with channel can be known as invalid due to its channel, this function allows the user to check if it is the case.
	/// The PathValidityInterval is maintained accordingly to the path BUT not in regard to the channel.
	/// In such a situation, the ChannelTrajectory will follow the current spline if any, 
	/// in the meantime it is expected the user recomputes a path to the current destination or a new one,
	/// using for instance: bot->ComputeNewPathToDestination(m_destination).
	bool GetNeedToRecomputeChannelPath() const;

	bool IsPathEventListBuildCompleted() const;

	/// Clears the current LivePath and associated events.
	/// Sets the PathFollowing component controls to default: target on path and
	/// Trajectory will be computed when a new path will be provided.
	void ClearFollowedPath();

	/// Returns a pointer to the PositionOnPath of the PathEvent along the path
	/// after the positionOnLivePath.
	/// If target on path is on a PathEvent, it returns the PositionOnPath of the
	/// next PathEvent.
	/// Note:  if positionOnLivePath is at the upper bound of the validity interval,
	/// it will return KY_NULL.
	const PositionOnPath* GetUpcomingEventPositionOnPath(const PositionOnLivePath& positionOnLivePath) const;

	/// Calls GetUpcomingEventPositionOnPath(const PositionOnLivePath&) using m_progressOnLivePath
	const PositionOnPath* GetUpcomingEventPositionOnPath() const;

	/// Returns a pointer to the navTag along the path after positionOnLivePath.
	/// If positionOnLivePath is on a PathEvent, it returns the NavTag between
	/// this PathEvent and the next.
	/// Note: if positionOnLivePath is at the upper bound of the validity interval,
	/// it will return KY_NULL.
	const NavTag* GetUpcomingEventNavTagFromPositionOnPath(const PositionOnLivePath& positionOnLivePath) const;

	/// Calls GetUpcomingEventNavTagFromPositionOnPath(const PositionOnLivePath&) using m_progressOnLivePath
	const NavTag* GetUpcomingEventNavTag() const;

	/// Returns a pointer to the navTag along the path before positionOnLivePath.
	/// If positionOnLivePath is on a PathEvent, it returns the NavTag between this
	/// PathEvent and the previous.
	/// Note: if positionOnLivePath is at the lower bound of the validity interval,
	/// it will return KY_NULL.
	const NavTag* GetPreviousEventNavTagFromPositionOnPath(const PositionOnLivePath& positionOnLivePath) const;

	/// Calls GetPreviousEventNavTagFromPositionOnPath(const PositionOnLivePath&) using m_progressOnLivePath
	const NavTag* GetPreviousEventNavTag() const;

	// ---------------------------------- Bot Configuration ----------------------------------

	/// Radius only impacts other surrounding bots avoidance.
	/// If avoidance is computed every frame, there is nothing to do.
	/// If avoidance is computed some time to time then avoidance computation of all surrounding bots should theorically be re-computed next frame.
	void SetRadius(KyFloat32 radius);

	/// Height is only used to send the correct visualDebug
	void SetHeight(KyFloat32 height);

	/// Trajectory will be recomputed the frame after this call.
	/// If the trajectory mode switches from TrajectoryMode_Shortcut to TrajectoryMode_Channel,
	/// this requires a path recompute that needs to be called explicitly by the user.
	void SetTrajectoryMode(TrajectoryMode trajectoryMode);

	/// The Trajectory will be recomputed the frame after this call.
	void SetMaxDesiredLinearSpeed(KyFloat32 maxDesiredLinearSpeed);

	/// EnableAvoidance impacts the Trajectory, the Trajectory will be recomputed after this call
	void SetEnableAvoidance(bool enable);

	/// The Path will *not* be recomputed after this call. The PathFinderConfig will be used by the next ComputeNewPathToDestination() calls.
	void SetPathFinderConfig(const PathFinderConfig& pathFinderConfig);

	/// The path will *not* be recomputed after this call. The ChannelComputerConfig will be used by the next ComputeNewPathToDestination() calls.
	void SetChannelComputerConfig(const ChannelComputerConfig& channelComputerConfig);

	/// The PathProgressConfig::m_pathValidationDistanceBackward and PathProgressConfig::m_pathValidationDistanceForward
	/// impact the computation of the events on the Path. The PathEventList will be recomputed the frame after this call.
	void SetPathProgressConfig(const PathProgressConfig& pathProgressConfig);

	/// The Trajectory will be recomputed the frame after this call.
	void SetShortcutTrajectoryConfig(const ShortcutTrajectoryConfig& shortcutTrajectoryConfig);

	/// The Trajectory will be recomputed the frame after this call.
	void SetChannelTrajectoryConfig(const ChannelTrajectoryConfig& channelTrajectoryConfig);

	// Set the Bot TrajectoryMinimalMoveDistance, nothing in particular will be recomputed after this call.
	void SetTrajectoryMinimalMoveDistance(KyFloat32 trajectoryMinimalMoveDistance);

	/// The Trajectory will be recomputed the frame after this call.
	void SetColliderCollectorConfig(const ColliderCollectorConfig& colliderCollectorConfig);

	/// The Trajectory will be recomputed after this call.
	void SetAvoidanceConfig(const AvoidanceConfig& avoidanceConfig);

	KyFloat32      GetRadius()                const;
	KyFloat32      GetHeight()                const;
	TrajectoryMode GetTrajectoryMode()        const;
	KyFloat32      GetMaxDesiredLinearSpeed() const;
	bool           GetEnableAvoidance()       const;
	KyFloat32      GetTrajectoryMinimalMoveDistance() const;

	const BotConfig&                GetConfig()                   const;
	const PathProgressConfig&       GetPathProgressConfig()       const;
	const ShortcutTrajectoryConfig& GetShortcutTrajectoryConfig() const;
	const ChannelTrajectoryConfig&  GetChannelTrajectoryConfig()  const;
	const ColliderCollectorConfig&  GetColliderCollectorConfig()  const;
	const AvoidanceConfig&          GetAvoidanceConfig()          const;

	// ---------------------------------- Path Following ----------------------------------

	/// When set to true, m_progressOnLivePath will be automatically updated in Bot::UpdatePathFollowing() based on a geometric criterion
	/// which is the bot crossing bisector line or plane (if in NavMesh or not) of the path edges.
	/// When set to false, it is the user's responsibility to move it forward properly, m_progressOnLivePath's positionOnPath won't be updated at all!
	/// Beware to check GetProgressOnPathStatus(), this status will be maintained to let the user know if m_progressOnLivePath is totally invalidated.
	/// You can set this to false when the Bot enters a specific NavTag
	/// associated with a custom movement you want to control on your side (ex for SmartObjects: foors, ladders...)
	/// and that you want to get have m_progressOnLivePath to move accordingly to your logic instead of the geometrical criterion.
	/// Set to true by default.
	void SetDoComputeProgressOnPath(bool doUpdateProgressOnPath);
	bool GetDoComputeProgressOnPath() const;
	
	/// When set to true, the Bot ProgressOnPath will be block at PathEvent CheckPoints until the 
	/// IPositionOnPathValidator instance of the NavigationPofile returns true.
	/// When set to false, the Bot will ignore PathEvent CheckPoints.
	/// You can set this to false typically when the Bot enters a specific NavTag
	/// associated with a custom movement you want to control on your side (ex for SmartObjects: foors, ladders...).
	/// However, the ProgressOnPath is still computed in order to keep track of the progression on the path if m_doComputeProgressOnPath.
	/// Set to true by default.
	void SetDoValidateCheckPoint(bool doValidateCheckPoint);
	bool GetDoValidateCheckPoint() const;
	
	/// When set to false, the Bot will no more compute the Trajectory.
	/// You can set this to false typically when the Bot enters a specific NavTag
	/// associated with a custom movement you want to control on your side (ex for SmartObjects: foors, ladders...).
	/// Set to true by default.
	void SetDoComputeTrajectory(bool doComputeTrajectory);
	bool GetDoComputeTrajectory() const;
	void ForceRecomputeTrajectory();

	/// The position is reached when the segment between a Bot previous and current
	/// position intersects the cylinder centered on the position with precisionRadius
	/// and database generation height.
	bool HasReachedPosition(const Vec3f& position, KyFloat32 precisionRadius) const;

	/// This function will move the progress on path forward until the predicate returns
	/// false. If it reaches the UpperBound, it stops.
	void MoveProgressOnPathForwardToNextEvent(IMovePositionOnPathPredicate* predicate);

	/// This function will move the progress on path backward until the predicate returns
	/// false. If it reaches the LowerBound, it stops.
	void MoveProgressOnPathBackwardToFirstEvent(IMovePositionOnPathPredicate* predicate);

	/// This function will move positionOnLivePath forward until the predicate returns
	/// false. If it reaches the UpperBound, it stops.
	void MovePositionOnPathForwardToNextEvent(PositionOnLivePath& positionOnLivePath, IMovePositionOnPathPredicate* predicate);

	/// This function will move positionOnLivePath backward until the predicate returns
	/// false. If it reaches the LowerBound, it stops.
	void MovePositionOnPathBackwardToFirstEvent(PositionOnLivePath& positionOnLivePath, IMovePositionOnPathPredicate* predicate);

	/// Computes the trajectory. Call this directly when you called SetDoComputeTrajectory()
	/// with false argument value but still want to use the Bot Trajectory to
	/// compute itself. This is useful namely when traversing special NavTags but
	/// still want using usual Trajectory computation namely for Bot avoidance.
	void ComputeTrajectory(KyFloat32 simulationTimeInSeconds);

	/// Helper function to compute the position the bot would reach if you apply the velocity for simulationTimeInSeconds
	/// Use this function to guarantee that the bot stays on a reachable valid position (we make use of the Move on NavMesh query)
	/// Gameware Navigation never moves the bot on its own, so you still need to call UpdatePosition/UpdateVelocity/UpdateFrontDirection or SetNextUpdateConfig
	/// This version use the TraverseLogic set on the current NavigationProfile
	Vec3f ComputeMoveOnNavMesh(const Kaim::Vec3f& velocity, KyFloat32 simulationTimeInSeconds);

	/// Helper function to compute the position the bot would reach if you apply the velocity for simulationTimeInSeconds
	/// Use this function to guarantee that the bot stays on a reachable valid position (we make use of the Move on NavMesh query)
	/// Gameware Navigation never moves the bot on its own, so you still need to call UpdatePosition/UpdateVelocity/UpdateFrontDirection or SetNextUpdateConfig
	/// This version use the TraverseLogic you provide, and therefore save a virtual call i.e faster
	template<class TraverseLogic>
	Vec3f ComputeMoveOnNavMeshWithTraverseLogic(const Kaim::Vec3f& velocity, KyFloat32 simulationTimeInSeconds);
	
	// ---------------------------------- Manual Control helper ----------------------------------
	// Following function helps setting up the PathFollowing when user wants to take control of the Bot
	// These functions computes an interval on which the Bot is going to follow the path in a particular way
	// under control of the user. this interval is computed based on the entry and the exit of NavTag along the path.
	// This interval is called ManualControlInterval.
	
	
	/// Call this function if you wish to take manual control of the Bot,
	/// i.e. if you don't need a trajectory to be computed and progress should ignore checkpoints
	/// e.g. a SmartObject takes control of the Bot.
	/// The entry point of the ManualControlInterval will be set to m_progressOnLivePath.
	/// No exit point will be computed, m_progressOnLivePath will be used at exit point,
	/// which means that ExitManualControl will rely solely on the geometric update of the m_progressOnLivePath during the Manual Control.
	/// Prefer EnterManualControl(const NavTag& navTag) which allows to force the exit point at the end of the NavTag on the path,
	/// particularly if during the Manual Control, the Bot has a trajectory that goes outside the Path (jump, animations...).
	ManualControlStatus EnterManualControl();

	/// Call this function if you wish to take manual control of the Bot on a given NavTag
	/// i.e. if you don't need a trajectory to be computed and progress should ignore checkpoints
	/// e.g. a SmartObject takes control of the Bot.
	/// The entry and exit point of the ManualControlInterval will computed from m_progressOnLivePath 
	/// based on the next or current portion of path that is on the given NavTag.
	/// On ExitManualControl, m_progressOnLivePath will be force to the exit point, if the trajectory can be reset at this point.
	/// \return ManualControlStatus indicates if the Bot is now under Manual Control or not.
	/// The following two values indicates the Bot is considered under ManualControl
	/// ManualControl_Valid indicates that both entry and exit points are found and valid, it is now up to the user to compute a trajectory
	/// ManualControl_Pending indicates that entry point is known and exit points remains under validation due to the path not being valid either after this point, they both will be updated on next updates.
	/// Other values indicate that the Bot cannot enter ManualControl.
	ManualControlStatus EnterManualControl(const NavTag* navTag);
	ManualControlStatus EnterManualControl(const DynamicNavTag* navTag);

	/// If EnterManualControl() was called without NavTag then the current m_progressOnLivePath is used. 
	/// During the Manual control, m_progressOnLivePath was updated as usual i.e. only based on geometric criterion.
	/// If EnterManualControl() was called with a specific NavTag then the m_progressOnLivePath is set to the NavTag Exit whatever its position is.
	/// \return ResetTrajectoryStatus indicates if ManualControl successfully ended or if Bot remains under ManualControl.
	/// Hence, if the value is greater or equals to ResetTrajectoryStatus_Success, m_trajectory was successfully reset, ManualControl ends and m_trajectory will be updated each frame from now on.
	/// Otherwise, the trajectory cannot be reset from the exit point, typically user can recompute a path or keep moving the Bot.
	ResetTrajectoryStatus ExitManualControl();


	// ---------------------------------- NavTag helper ----------------------------------
	// Add all NavTags along the path from a PositionOnLivePath up to a given distance
	// Used by the avoidance and Trajectory to determine which area of the NavMesh Bot is allowed to walk on
	// These methods will not empty the output array

	void ComputeNavTagArrayForward(PositionOnLivePath positionOnLivePath, KyFloat32 distance, KyArray<const NavTag*>& output) const;
	void ComputeNavTagArrayBackward(PositionOnLivePath positionOnLivePath, KyFloat32 distance, KyArray<const NavTag*>& output) const;

public: // internal
	// Main function of the Bot. Called for each bot in World::Update() every frame
	void UpdatePathFollowing(KyFloat32 simulationTimeInSeconds);

	// Tests if the trajectory can be reset from the given positionOnLivePath, if possible set m_progressOnLivePath to positionOnLivePath
	// Called internally by ExitManualControl
	ResetTrajectoryStatus ResetTrajectory(PositionOnLivePath& positionOnLivePath);

	virtual void DoSendVisualDebug(VisualDebugServer& server, VisualDebugSendChangeEvent changeEvent); // Inherited from WorldElement

	// Bot spatialization (through its SpatializedCylinder)
	void UpdateSpatialization(KyFloat32 simulationTimeInSeconds);

	// LivePath management
	LivePath& GetLivePath();
	PathEventList& GetPathEventList();
	void FlagPathAsPotentiallyDirty(); // Called in EventBroadcaster to trigger a path validity check after NavData has changed

	void ComputeBotRadiusAndHeight();

public: // DEPRECATED functions
	
	/// This was called with true by Kaim::DefaultDatabaseChangeBroadcaster and set back to false internally in the Database.
	/// it is not used anymore. Prefer using m_botNeedFullComputeFlags.ChannelHasBeenInvalidated() to tell the path must be considered invalid until a new path is computed.
	KY_DEPRECATED(void SetChannelPathNeedToBeRecomputed(bool needToRecomputeChannel));

private:
	void SetDefaults();
	void SetupBotRadiusCappedToGenerationMetrics();
	void SetupBotHeightCappedToGenerationMetrics();
	void SendPathFinderErrorDisplayList();
	void ReplaceFollowedPathWithNewPath();
	void SetTrajectory(ITrajectory* trajectory);
	void ApplyNewPosAndVelocity(KyFloat32 simulationTimeInSeconds);

	// If ShortcutTrajectory is in use it allows to skip some PathFollowing computations on several frames.
	// cf. World::SetNumberOfFramesForShortcutTrajectoryPeriodicUpdate()
	bool IsShortcutTrajectoryAllowingToSkipPathFollowingThisFrame(KyFloat32& simulationTimeInSecondsToApplyThisFrame);

public: // internal
	Database* m_database;
	void* m_traverseLogicUserData;

	Ptr<IPathFinderQuery> m_pathFinderQuery;

	// current navigation components
	Ptr<BasePathProgressComputer> m_pathProgressComputer;
	Ptr<ITrajectory>              m_trajectory;

	// Members relative to Path and Spatialization.
	LivePath m_livePath;    // Maintain Path and runtime information such as its validity
	SpatializedCylinder m_spatializedCylinder;    // Spatialization of the bot in Databases
	ChannelSectionPtr m_positionInChannel;        // Position of the bot in ChannelArray
	PositionOnLivePath m_progressOnLivePath;      // Progress of the bot along the path so far, it never goes backward on the path
	ProgressOnPathStatus m_progressOnPathStatus;  // Status relative to m_progressOnLivePath
	Vec3f m_previousPosition; // Position passed the previous frame. Used for arrival and checkpoint validation
	ManualControlInterval m_manualControlInterval;
	
	// next position, direction and linear speed to apply
	BotUpdateConfig m_nextUpdateConfig;	
	bool m_positionUpdated;
	bool m_velocityUpdated; 
	bool m_frontDirectionUpdated;

	// NavigationProfiles
	KyUInt32 m_newPathToDestNavigationProfileId;
	BotNavigationCollection m_botNavigationCollection; // the per-Bot per-NavigationProfile data

	// flags to force recompute on next Update
	BotNeedFullComputeFlags m_botNeedFullComputeFlags;

	// Target on path internal
	KyFloat32 m_nextShortcutSqDistTrigger; // for internal use

	// ColliderCollector per bot data
	Vec3f m_lastCollectorPosition;
	Ptr<QueryDynamicOutput> m_collectorOutput;
	
	KyFloat32 m_forcePassageTimer; // for internal use, negative value indicates the passage is being forced, positive value indicates when waiting, 0 otherwise

private:
	// Configuration
	Ptr<BotConfig> m_botConfig;

	bool m_doComputeProgressOnPath;
	bool m_doValidateCheckPoint;
	bool m_doComputeTrajectory;

	// VisualDebug
	Path* m_lastPathSentToVisualDebug; // used to send LivePath visual debug only when path changes
	Ptr<ITrajectory> m_previousTrajectoryToSendForVisualDebug; // used to remove trajectory VisualDebug blob in NavigationLab
	bool m_hasSpatializationChanged;
};

} // namespace Kaim

#include "gwnavruntime/world/bot.inl"

#endif // Navigation_Bot_H
