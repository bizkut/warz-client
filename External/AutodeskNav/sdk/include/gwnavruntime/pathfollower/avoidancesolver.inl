/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


namespace Kaim
{

template<class TLogic>
void AvoidanceSolver<TLogic>::ComputeAllowedNavTags( AvoidanceSolverConfig& config )
{
	Kaim::Bot* bot = config.m_bot;
	m_localCostAwareTraverseLogicData.m_userTraverseLogicData = bot->GetBotTraverseLogicUserData();
	
	m_localCostAwareTraverseLogicData.m_allowedNavTagForTraversal.Clear();
	KyFloat32 navTagDiscoveryDistance = config.m_bot->GetPathProgressConfig().m_navTagDiscoveryDistance;
	bot->ComputeNavTagArrayForward(bot->GetProgressOnLivePath(), navTagDiscoveryDistance,
		m_localCostAwareTraverseLogicData.m_allowedNavTagForTraversal);
	bot->ComputeNavTagArrayBackward(bot->GetProgressOnLivePath(), navTagDiscoveryDistance,
		m_localCostAwareTraverseLogicData.m_allowedNavTagForTraversal);
}

template<class TLogic>
IAvoidanceComputer::AvoidanceResult AvoidanceSolver<TLogic>::Solve( AvoidanceSolverConfig& config
	, const Vec3f& previousVelocity
	, const Vec3f& desiredVelocity
	, Vec3f&       outputVelocity)
{
	ComputeAllowedNavTags( config );

	// Clear previous computations
	m_velocitySamples.Clear();

	// Extract some information
	Vec2f     desiredDirection  = desiredVelocity.Get2d();
	KyFloat32 movingSpeed       = desiredDirection.Normalize();
	SetTraverseLogicUserData(config.m_bot->GetBotTraverseLogicUserData());

	if(movingSpeed == 0.0f)
	{
		outputVelocity = desiredVelocity;
		return IAvoidanceComputer::NoAvoidance;
	}

	// First, we pick a smart and stable moving direction
	Vec2f movingDirection = desiredDirection;
	ComputeMovingDirection( config, movingDirection );

	// Then, we compute how much time remains
	KyFloat32 timeHorizon = ComputeTimeHorizon( config, movingSpeed );

	if(timeHorizon == 0.0f)
	{
		// Already at destination, so no avoidance needed
		outputVelocity = desiredVelocity;
		return IAvoidanceComputer::NoAvoidance;
	}

	// We generate a set of turning velocities
	ComputeTurningVelocitiesSamples( config, movingDirection, timeHorizon );

	// Score samples based on the expected collision time
	bool hadCollision = ScoreForTimeToCollision(config, timeHorizon, movingSpeed);
	if(!hadCollision)
	{
		outputVelocity = desiredVelocity;
		return IAvoidanceComputer::NoAvoidance;
	}

	// Smooth the time to collision samples to better reflect a continuous state
	SmoothTimeToCollisionSamples();

	// Score based on the desired direction
	ScoreForDistanceToDirection(desiredDirection, &VelocitySample::ScoreDesiredVelocity);

	// Also compute the influence of the previous moving direction
	Vec2f     previousDirection = previousVelocity.Get2d();
	KyFloat32 previousSpeed     = previousDirection.Normalize();
	ScoreForPreviousDirection(previousDirection, previousSpeed);

	// There is one special sample, that is the no avoidance one
	VelocitySample noAvoidanceSample;
	ComputeNoAvoidanceVelocitySample(noAvoidanceSample, desiredDirection);
	
	// By default, choose the no avoidance sample
	VelocitySample favoriteSample;
	favoriteSample.m_normalizedDirection                 = noAvoidanceSample.m_normalizedDirection;
	favoriteSample.m_type                      = noAvoidanceSample.m_type;
	favoriteSample.m_timeToImpactScore         = noAvoidanceSample.m_timeToImpactScore;
	favoriteSample.m_smoothedTimeToImpactScore = noAvoidanceSample.m_smoothedTimeToImpactScore;
	favoriteSample.m_desiredVelocityScore      = noAvoidanceSample.m_desiredVelocityScore;
	favoriteSample.m_previousVelocityScore     = noAvoidanceSample.m_previousVelocityScore;
	favoriteSample.m_collisionTime             = noAvoidanceSample.m_collisionTime;
	favoriteSample.m_colliderIndex             = noAvoidanceSample.m_colliderIndex;

	// Select the best avoidance sample
	SelectBestVelocitySample(config, favoriteSample, previousDirection);

	if( config.m_bot->GetAvoidanceConfig().m_enableSlowingDown)
	{
		if( favoriteSample.m_smoothedTimeToImpactScore < 1.0f )
		{
			favoriteSample.m_normalizedDirection *= favoriteSample.m_smoothedTimeToImpactScore;
		}
	}

	if((favoriteSample.m_collisionTime <= config.m_bot->GetAvoidanceConfig().m_stopCollisionTime) && (config.m_bot->GetAvoidanceConfig().m_enableStop))
	{
		favoriteSample.m_normalizedDirection.Set(0.0f, 0.0f);
		favoriteSample.m_type = IAvoidanceComputer::Stop;
			
		// There is a special case we'd like to deal with: contouring a single obstacle
		// to do so, we need to know if there was a single collider involved
		KyInt32 colliderIndex = noAvoidanceSample.m_colliderIndex;
		if(colliderIndex==-1)
			colliderIndex = favoriteSample.m_colliderIndex;
		if(colliderIndex!=-1)
		{
			// If a single collider is involved, let's try to by pass thru right or left
			Vec2f faceDirection = noAvoidanceSample.m_normalizedDirection;
			Vec2f sideDirection( -faceDirection.y, faceDirection.x );
			Vec2f leftSideMarquer  = config.m_workspace->m_colliders[colliderIndex].m_position + sideDirection * (config.m_workspace->m_colliders[colliderIndex].m_radius + config.m_workspace->m_rootCollider.m_radius + config.m_avoidanceComputerConfig->m_safetyDistance) * 3.0f;
			Vec2f rightSideMarquer = config.m_workspace->m_colliders[colliderIndex].m_position - sideDirection * (config.m_workspace->m_colliders[colliderIndex].m_radius + config.m_workspace->m_rootCollider.m_radius + config.m_avoidanceComputerConfig->m_safetyDistance) * 3.0f;
			const Vec3f& botPosition = config.m_colliderSpatializedPoint->GetPosition();
			Vec2f leftDirection  = leftSideMarquer - botPosition.Get2d();
			Vec2f rightDirection = rightSideMarquer - botPosition.Get2d();

			KyFloat32 distanceToLeft = leftDirection.Normalize();
			KyFloat32 distanceToRight = rightDirection.Normalize();
			Vec2f avoidanceDirection = distanceToLeft < distanceToRight ? leftDirection : rightDirection;
			KyFloat32 timeToCollision  = config.m_workspace->ComputeTimeToCollision(config.m_bot,avoidanceDirection*movingSpeed, config.m_avoidanceComputerConfig->m_safetyDistance, colliderIndex);
			if(timeToCollision > (config.m_bot->GetAvoidanceConfig().m_stopCollisionTime))
			{
				if( IsDirectionValid(config, avoidanceDirection, movingSpeed, 0.1f) )
				{
					favoriteSample.m_normalizedDirection = avoidanceDirection; 
					favoriteSample.m_type = IAvoidanceComputer::Turn;
				}
			} 
		}
	}

	// Let's push each intersecting entities away from each other
	KyUInt32 colliderCount = config.m_workspace->m_colliders.GetCount();
	for (KyUInt32 i = 0; i < colliderCount; ++i)
	{
		const AvoidanceCollider& avoidanceCollider = config.m_workspace->m_colliders[i];
		KyFloat32 radius = avoidanceCollider.m_radius + config.m_workspace->m_rootCollider.m_radius;
		Vec2f diff = config.m_workspace->m_rootCollider.m_position - avoidanceCollider.m_position;
		if(diff.GetSquareLength() < radius * radius)
		{
			if(DotProduct(avoidanceCollider.m_velocity, favoriteSample.m_normalizedDirection) > 0.0f)
			{
				KyFloat32 penetrationDepth = diff.Normalize();
				penetrationDepth = radius - penetrationDepth;
				Vec2f finalDirection = favoriteSample.m_normalizedDirection;

				KyFloat32 pushForceStrength =  (penetrationDepth / config.m_workspace->m_rootCollider.m_radius);

				finalDirection = ((1.0f -pushForceStrength) * finalDirection) + (Vec2f(diff.x, diff.y) * pushForceStrength);
				finalDirection.Normalize();

				if( IsDirectionValid(config, finalDirection, movingSpeed, 0.1f) )
				{
					favoriteSample.m_normalizedDirection = finalDirection;
					favoriteSample.m_type = IAvoidanceComputer::Turn;
				}
			}
		}
	}
	
	outputVelocity = Vec3f(favoriteSample.m_normalizedDirection.x, favoriteSample.m_normalizedDirection.y, 0.0f);
	outputVelocity *= movingSpeed;

	if(config.m_bot->GetCurrentVisualDebugLOD() == VisualDebugLOD_Maximal)
		DebugDraw(config, previousVelocity, desiredVelocity, outputVelocity, noAvoidanceSample, favoriteSample, movingSpeed);

	return favoriteSample.m_type;
}

template<class TLogic>
bool AvoidanceSolver<TLogic>::IsDirectionValid(AvoidanceSolverConfig& config, const Vec2f& direction, KyFloat32 speed, KyFloat32 time)
{
	KyFloat32 deltaPosition = speed * time;
	Vec3f botPosition = config.m_bot->GetPosition();

	ChannelSectionPtr currentChannelSectionPtr = config.m_channelSectionPtr;
	// Test against channel border
	if (currentChannelSectionPtr.IsValid() == true) 
	{
		RayCastInChannel rayCastInChannel;
		rayCastInChannel.Initialize(botPosition.Get2d(), currentChannelSectionPtr, direction);
		rayCastInChannel.Perform();
		if((rayCastInChannel.m_result != RayCastInChannel_Success) && (rayCastInChannel.m_result != RayCastInChannel_PassedChannelLastSection))
		{
			return false;
		}
	}

	// Test against navmesh border
	const NavTrianglePtr botTrianglePtr = config.m_colliderSpatializedPoint->GetNavTrianglePtr(config.m_database);

	Kaim::WorldIntegerPos botPositionIntegerPos;
	const Kaim::DatabaseGenMetrics& databaseGenMetrics = config.m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(botPosition, botPositionIntegerPos);

	RayCastQuery<LocalCostAwareTraverseLogic<TLogic> > rayCastQuery;
	rayCastQuery.BindToDatabase(config.m_database);
	rayCastQuery.SetTraverseLogicUserData(&m_localCostAwareTraverseLogicData);
	rayCastQuery.SetPositionSpatializationRange(PositionSpatializationRange(KyFloat32MAXVAL, KyFloat32MAXVAL));
	rayCastQuery.Initialize(botPosition, direction * deltaPosition);
	rayCastQuery.SetStartTrianglePtr(botTrianglePtr);
	rayCastQuery.SetPerformQueryStat(QueryStat_Avoidance);
	rayCastQuery.SetStartIntegerPos(botPositionIntegerPos);

	rayCastQuery.PerformQueryWithInputCoordPos(config.m_database->GetWorkingMemory());

	RayCastQueryResult queryResult = rayCastQuery.GetResult();
	if(queryResult == RAYCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION) 
	{
		return false;
	}

	return true;
}

template<class TLogic>
void AvoidanceSolver<TLogic>::ComputeMovingDirection(const AvoidanceSolverConfig& config, Vec2f& movingDirection)
{
	// Instead of using the desired velocity as the moving direction, We aim at
	// staying withing the channel.
	if (config.m_channelSectionPtr.IsValid() == true)
	{
		ChannelSectionPtr section = config.m_channelSectionPtr;

		// If the current bot section is first or last one, we can't compute the
		// corresponding edge. Since first and last sections are aligned on their
		//  neighbor, we instead use these sections to compute a moving direction.
		if (section.IsFirstSection())
			++section.m_sectionIdx;
		else if (section.IsLastSection())
			--section.m_sectionIdx;

		const Gate& prevGate = section.GetPreviousGate();
		const Gate& nextGate = section.GetNextGate();
		if (config.m_channelSectionPtr.GetLastGatePathNodeIdx() != config.m_channelSectionPtr.GetNextGatePathNodeIdx())
		{
			const Vec3f nextGateMidPoint = 0.5f * (nextGate.m_leftPos + nextGate.m_rightPos);
			const Vec3f prevGateMidPoint = 0.5f * (prevGate.m_leftPos + prevGate.m_rightPos);
			Vec2f channelDirection = nextGateMidPoint.Get2d() - prevGateMidPoint.Get2d();
			channelDirection.Normalize();

			movingDirection = channelDirection;
		}
	}
	else
	{
		// Or to create // samples to the path
		const Vec3f& edgeStart = config.m_bot->GetLivePath().GetFollowedPath()->GetPathEdgeStartPosition( config.m_bot->GetProgressOnLivePath().GetPathEdgeIndex() );
		const Vec3f& edgeEnd   = config.m_bot->GetLivePath().GetFollowedPath()->GetPathEdgeEndPosition( config.m_bot->GetProgressOnLivePath().GetPathEdgeIndex() );
		Vec2f pathDirection = edgeEnd.Get2d() - edgeStart.Get2d();
		pathDirection.Normalize();

		// Still take target into influence
		pathDirection += movingDirection;
		pathDirection *= 0.5f;
		pathDirection.Normalize();

		movingDirection = pathDirection;
	}
}

template<class TLogic>
KyFloat32 AvoidanceSolver<TLogic>::ComputeTimeHorizon( const AvoidanceSolverConfig& config, KyFloat32 movingSpeed )
{
	// To avoid avoiding too far from the destination, we need to compute an average time to destination
	const Channel* channel = config.m_bot->m_positionInChannel.GetChannel();
	Vec3f dest = config.m_bot->GetLivePath().GetFollowedPath()->GetPathEndPosition();
	if(channel)
		dest = channel->GetGate(channel->GetGateCount() - 1).m_pathPos;

	const Vec3f& botPosition = config.m_colliderSpatializedPoint->GetPosition();
	KyFloat32 timeToDest = ((dest - botPosition).GetLength2d() + (config.m_bot->GetRadius() * 2.0f)) / movingSpeed;
	KyFloat32 minimalTimeToCollision = config.m_avoidanceComputerConfig->m_minimalTimeToCollision;
	minimalTimeToCollision = minimalTimeToCollision < timeToDest ? minimalTimeToCollision : timeToDest;

	return minimalTimeToCollision;
}

template<class TLogic>
void AvoidanceSolver<TLogic>::ComputeTurningVelocitiesSamples( const AvoidanceSolverConfig& config, const Vec2f& movingDirection, KyFloat32 timeHorizon )
{
	// Limit the angle span space based on the remaining time to collision
	KyFloat32 angleSpanRatio = timeHorizon / config.m_avoidanceComputerConfig->m_minimalTimeToCollision;
	KyFloat32 angleSpan      = angleSpanRatio * config.m_avoidanceComputerConfig->m_avoidanceAngleSpan  * (KyFloat32)KY_MATH_PI / 180.0f;
	KyUInt32 sampleCount     = config.m_avoidanceComputerConfig->m_avoidanceSampleCount;

	KyFloat32 fAngleIncrement = angleSpan / sampleCount;

	Vec2f right      = movingDirection;
	KyFloat32 fAngle = (-fAngleIncrement * 0.5f) - (sampleCount * 0.5f * fAngleIncrement);

	KyFloat32 fRightCos = cosf(fAngle);
	KyFloat32 fRightSin = sinf(fAngle);

	right = Vec2f( (right[0] * fRightCos) - (right[1] * fRightSin), (right[0] * fRightSin) + (right[1] * fRightCos)); 

	fRightCos = cosf(fAngleIncrement);
	fRightSin = sinf(fAngleIncrement);

	VelocitySample velocitySample;
	while(fAngle <= (angleSpan * 0.5f))
	{
		velocitySample.m_normalizedDirection = right;
		velocitySample.m_type      = IAvoidanceComputer::Turn;
		m_velocitySamples.PushBack(velocitySample);

		right = Vec2f( (right[0] * fRightCos) - (right[1] * fRightSin), (right[0] * fRightSin) + (right[1] * fRightCos)); 
		right.Normalize();

		fAngle += fAngleIncrement;
	}
}

template<class TLogic>
bool AvoidanceSolver<TLogic>::ScoreForTimeToCollision(const AvoidanceSolverConfig& config, KyFloat32 timeHorizon, KyFloat32 movingSpeed)
{
	Vec3f botPosition = config.m_bot->GetPosition();

	bool hadCollision = false;
	for (KyUInt32 i = 0; i < m_velocitySamples.GetCount(); ++i)
	{
		VelocitySample& velocitySample = m_velocitySamples[i];

		const AvoidanceWorkspace* workspace = config.m_workspace;
		velocitySample.m_colliderIndex = -1;
		velocitySample.m_collisionTime = workspace->ComputeTimeToCollision(config.m_bot, velocitySample.m_normalizedDirection * movingSpeed, config.m_avoidanceComputerConfig->m_safetyDistance, velocitySample.m_colliderIndex);
		if(velocitySample.m_collisionTime > timeHorizon)
			velocitySample.m_collisionTime = timeHorizon;
		velocitySample.m_timeToImpactScore =  velocitySample.m_collisionTime / timeHorizon;

		if(velocitySample.m_timeToImpactScore != 1.0f)
			hadCollision = true;
	}

	// Test against navmesh border
	const NavTrianglePtr botTrianglePtr = config.m_colliderSpatializedPoint->GetNavTrianglePtr(config.m_database);
	KyFloat32 horizon = timeHorizon * 0.25f;

	Kaim::WorldIntegerPos botPositionIntegerPos;
	const Kaim::DatabaseGenMetrics& databaseGenMetrics = config.m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(botPosition, botPositionIntegerPos);

	DiskCollisionQuery<LocalCostAwareTraverseLogic<TLogic> > diskCollisionQuery;
	diskCollisionQuery.BindToDatabase(config.m_database);
	diskCollisionQuery.SetTraverseLogicUserData(&m_localCostAwareTraverseLogicData);
	diskCollisionQuery.SetPositionSpatializationRange(PositionSpatializationRange(KyFloat32MAXVAL, KyFloat32MAXVAL));
	diskCollisionQuery.Initialize(botPosition, movingSpeed * horizon);
	diskCollisionQuery.SetPerformQueryStat(QueryStat_Avoidance);
	diskCollisionQuery.SetCenterIntegerPos(botPositionIntegerPos);
	diskCollisionQuery.SetCenterTrianglePtr(botTrianglePtr);

	diskCollisionQuery.PerformQueryWithInputCoordPos(config.m_database->GetWorkingMemory());

	if( diskCollisionQuery.GetResult() != DISKCOLLISION_DONE_DISK_FIT )
	{
		for (KyUInt32 i = 0; i < m_velocitySamples.GetCount(); ++i)
		{
			VelocitySample& velocitySample = m_velocitySamples[i];
			Vec2f direction = velocitySample.m_normalizedDirection;

			// KY_TODO: Use a Trapezoid Query instead of sampling 
			RayCastQuery<LocalCostAwareTraverseLogic<TLogic> > rayCastQuery;
			rayCastQuery.BindToDatabase(config.m_database);
			rayCastQuery.SetTraverseLogicUserData(&m_localCostAwareTraverseLogicData);
			rayCastQuery.SetPositionSpatializationRange(PositionSpatializationRange(KyFloat32MAXVAL, KyFloat32MAXVAL));
			rayCastQuery.Initialize(botPosition, direction * (movingSpeed * horizon));
			rayCastQuery.SetStartTrianglePtr(botTrianglePtr);
			rayCastQuery.SetPerformQueryStat(QueryStat_Avoidance);
			rayCastQuery.SetStartIntegerPos(botPositionIntegerPos);

			rayCastQuery.PerformQueryWithInputCoordPos(config.m_database->GetWorkingMemory());

			RayCastQueryResult queryResult = rayCastQuery.GetResult();
			if(queryResult == RAYCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION) 
			{
				KyFloat32 timeToCollision = (rayCastQuery.GetCollisionPos() - botPosition).GetLength2d() / movingSpeed;

				if(horizon > timeHorizon)
					horizon = timeHorizon;
				if(timeToCollision > horizon)
					timeToCollision = horizon;

				KyFloat32 timeToImpactScore = timeToCollision /  horizon;
				if( timeToImpactScore < velocitySample.m_timeToImpactScore )
				{
					velocitySample.m_timeToImpactScore = timeToImpactScore;
					velocitySample.m_collisionTime = timeToCollision;
					velocitySample.m_colliderIndex = -1;
				}
			}
		}
	}

	ChannelSectionPtr currentChannelSectionPtr = config.m_channelSectionPtr;
	// Test against channel border
	if (currentChannelSectionPtr.IsValid() == true) 
	{
		RayCastInChannel rayCastInChannel;
		for (KyUInt32 i = 0; i < m_velocitySamples.GetCount(); ++i)
		{
			VelocitySample& velocitySample = m_velocitySamples[i];
			rayCastInChannel.Initialize(config.m_bot->GetPosition().Get2d(), currentChannelSectionPtr, velocitySample.m_normalizedDirection);
			rayCastInChannel.Perform();
			if(rayCastInChannel.m_result != RayCastInChannel_Success)
			{
				velocitySample.m_timeToImpactScore = 0.0f;
				velocitySample.m_collisionTime     = 0.0f;
				velocitySample.m_colliderIndex     = -1;
			}
		}
	}

	return hadCollision;
}

template<class TLogic>
void AvoidanceSolver<TLogic>::SmoothTimeToCollisionSamples() 
{
	KyInt32 turnSampleCount = (KyInt32)m_velocitySamples.GetCount();
	for (KyInt32 i = 0; i < turnSampleCount; ++i)
	{
		VelocitySample& velocitySample = m_velocitySamples[i];
		velocitySample.m_smoothedTimeToImpactScore  = 0.0f;
		int sampleCount = 0;

		if((i-2) >= 0)
		{
			velocitySample.m_smoothedTimeToImpactScore += m_velocitySamples[i-2].m_timeToImpactScore;
			sampleCount++;
		}

		if((i-1) >= 0)
		{
			velocitySample.m_smoothedTimeToImpactScore += m_velocitySamples[i-1].m_timeToImpactScore;
			sampleCount++;
		}

		velocitySample.m_smoothedTimeToImpactScore += m_velocitySamples[i].m_timeToImpactScore;
		sampleCount++;

		if((i+1) < turnSampleCount)
		{
			velocitySample.m_smoothedTimeToImpactScore += m_velocitySamples[i+1].m_timeToImpactScore;
			sampleCount++;
		}

		if((i+2) < turnSampleCount)
		{
			velocitySample.m_smoothedTimeToImpactScore += m_velocitySamples[i+2].m_timeToImpactScore;
			sampleCount++;
		}

		velocitySample.m_smoothedTimeToImpactScore /= sampleCount;
	}
}

template<class TLogic>
void AvoidanceSolver<TLogic>::ScoreForDistanceToDirection(const Vec2f& direction, VelocitySample::ScoreVelocityFunction scoreFunction) 
{
	KyFloat32 maxDistance = 0.0f;
	for (KyUInt32 i = 0; i < m_velocitySamples.GetCount(); ++i)
	{
		VelocitySample& velocitySample = m_velocitySamples[i];
		KyFloat32 distance = (velocitySample.m_normalizedDirection - direction).GetSquareLength();
		maxDistance = maxDistance < distance ? distance : maxDistance;
	}
	for (KyUInt32 i = 0; i < m_velocitySamples.GetCount(); ++i)
	{
		VelocitySample& velocitySample = m_velocitySamples[i];
		KyFloat32 distance = (velocitySample.m_normalizedDirection - direction).GetSquareLength();
		KyFloat32 score = 1.0f - (distance / maxDistance);
		(velocitySample.*scoreFunction)(score);
	}
}

template<class TLogic>
void AvoidanceSolver<TLogic>::ScoreForPreviousDirection(const Vec2f& previousDirection, KyFloat32 previousSpeed) 
{
	if(previousSpeed > 0.0f)
	{
		ScoreForDistanceToDirection(previousDirection, &VelocitySample::ScorePreviousVelocity);

		// We still want to favor the desired velocity sample if there is no collisions nearby
		// So can previous velocity influence in that case
		for (KyUInt32 i = 0; i < m_velocitySamples.GetCount(); ++i)
		{
			VelocitySample& velocitySample = m_velocitySamples[i];
			if(velocitySample.m_type == IAvoidanceComputer::NoAvoidance)
			{
				if(velocitySample.m_smoothedTimeToImpactScore == 1.0f)
					velocitySample.m_previousVelocityScore = 1.0f;
				break;
			}

		}
	} else {
		for (KyUInt32 i = 0; i < m_velocitySamples.GetCount(); ++i)
		{
			VelocitySample& velocitySample = m_velocitySamples[i];
			velocitySample.m_previousVelocityScore = 1.0f;
		}
	}
}

template<class TLogic>
void AvoidanceSolver<TLogic>::ComputeNoAvoidanceVelocitySample(VelocitySample& noAvoidanceSample, const Vec2f& desiredDirection) 
{
	// Find the 2 closest samples to the desired velocity
	KyUInt32 closestSample   = FindClosestSampleToDirection(desiredDirection); 
	KyUInt32 nextSampleLeft  = closestSample == 0 ? closestSample : closestSample -1; 
	KyUInt32 nextSampleRight = closestSample == m_velocitySamples.GetCount()-1 ? closestSample : closestSample + 1;
	KyUInt32 secondSample = nextSampleLeft;
	if( (m_velocitySamples[nextSampleLeft].m_normalizedDirection - desiredDirection).GetSquareLength() >  
		(m_velocitySamples[nextSampleRight].m_normalizedDirection - desiredDirection).GetSquareLength() )
		secondSample = nextSampleRight;

	// Compute the no avoidance sample score
	noAvoidanceSample.m_normalizedDirection = desiredDirection;
	noAvoidanceSample.m_type      = IAvoidanceComputer::NoAvoidance;

	noAvoidanceSample.m_timeToImpactScore         = 0.5f * (m_velocitySamples[closestSample].m_timeToImpactScore + m_velocitySamples[secondSample].m_timeToImpactScore);
	noAvoidanceSample.m_smoothedTimeToImpactScore = 0.5f * (m_velocitySamples[closestSample].m_smoothedTimeToImpactScore + m_velocitySamples[secondSample].m_smoothedTimeToImpactScore);
	noAvoidanceSample.m_desiredVelocityScore      = 1.0f;
	noAvoidanceSample.m_previousVelocityScore     = 0.5f * (m_velocitySamples[closestSample].m_previousVelocityScore + m_velocitySamples[secondSample].m_previousVelocityScore);
	noAvoidanceSample.m_collisionTime             = 0.5f * (m_velocitySamples[closestSample].m_collisionTime + m_velocitySamples[secondSample].m_collisionTime);
	noAvoidanceSample.m_colliderIndex             = m_velocitySamples[closestSample].m_colliderIndex != -1 ? m_velocitySamples[closestSample].m_colliderIndex : m_velocitySamples[secondSample].m_colliderIndex;
}

template<class TLogic>
KyUInt32 AvoidanceSolver<TLogic>::FindClosestSampleToDirection(const Vec2f& direction) 
{
	KyUInt32 selectedSample = 0;
	KyFloat32 maxDistance   = (m_velocitySamples[0].m_normalizedDirection - direction).GetSquareLength();

	for (KyUInt32 i = 1; i < m_velocitySamples.GetCount(); ++i)
	{
		VelocitySample& velocitySample = m_velocitySamples[i];
		KyFloat32 distance = (velocitySample.m_normalizedDirection - direction).GetSquareLength();
		if(distance < maxDistance)
		{
			maxDistance = distance;
			selectedSample = i;
		}
	}
	return selectedSample;
}

template<class TLogic>
void AvoidanceSolver<TLogic>::SelectBestVelocitySample(const AvoidanceSolverConfig& config, VelocitySample& favoriteSample, const Vec2f& previousDirection) 
{
	bool shouldAvoid = false;

	// Is there anything to avoid anyway?
	for (KyUInt32 i = 0; i < m_velocitySamples.GetCount(); ++i)
	{
		VelocitySample& velocitySample = m_velocitySamples[i];
		if( velocitySample.m_timeToImpactScore != 1.0f )
		{
			shouldAvoid = true;
			break;
		}
	}

	if(shouldAvoid)
	{
		KyFloat32 bestScore = favoriteSample.ComputeFinalScore(config.m_bot->GetAvoidanceConfig());

		for (KyUInt32 i = 0; i < m_velocitySamples.GetCount(); ++i)
		{
			VelocitySample& velocitySample = m_velocitySamples[i];

			if(velocitySample.ComputeFinalScore(config.m_bot->GetAvoidanceConfig()) == bestScore)
			{
				if(velocitySample.m_type == IAvoidanceComputer::NoAvoidance)
				{
					bestScore = velocitySample.ComputeFinalScore(config.m_bot->GetAvoidanceConfig());
					favoriteSample.m_normalizedDirection = velocitySample.m_normalizedDirection;
					favoriteSample.m_type = velocitySample.m_type;
					favoriteSample.m_collisionTime = velocitySample.m_collisionTime;
					favoriteSample.m_smoothedTimeToImpactScore = velocitySample.m_smoothedTimeToImpactScore;
					favoriteSample.m_colliderIndex = velocitySample.m_colliderIndex;
				} else if(favoriteSample.m_type != IAvoidanceComputer::NoAvoidance)
				{
					// Favor the closest one to the previous velocity
					if((previousDirection - velocitySample.m_normalizedDirection).GetSquareLength() < (previousDirection - favoriteSample.m_normalizedDirection).GetSquareLength())
					{
						bestScore = velocitySample.ComputeFinalScore(config.m_bot->GetAvoidanceConfig());
						favoriteSample.m_normalizedDirection = velocitySample.m_normalizedDirection;
						favoriteSample.m_type = velocitySample.m_type;
						favoriteSample.m_collisionTime = velocitySample.m_collisionTime;
						favoriteSample.m_smoothedTimeToImpactScore = velocitySample.m_smoothedTimeToImpactScore;
						favoriteSample.m_colliderIndex = velocitySample.m_colliderIndex;
					}
				}

			} else if( velocitySample.ComputeFinalScore(config.m_bot->GetAvoidanceConfig())>bestScore)
			{
				bestScore = velocitySample.ComputeFinalScore(config.m_bot->GetAvoidanceConfig());
				favoriteSample.m_normalizedDirection = velocitySample.m_normalizedDirection;
				favoriteSample.m_type = velocitySample.m_type;
				favoriteSample.m_collisionTime = velocitySample.m_collisionTime;
				favoriteSample.m_smoothedTimeToImpactScore = velocitySample.m_smoothedTimeToImpactScore;
				favoriteSample.m_colliderIndex = velocitySample.m_colliderIndex;
			}
		}
	}
}


template<class TLogic>
void AvoidanceSolver<TLogic>::DebugDrawSample(ScopedDisplayList& displayList_Samples, const VelocitySample& velocitySample, const Vec3f& rootPosition, VelocitySample::ScoreAccessorFunction scoreAccessorFunction, const AvoidanceConfig& config, KyFloat32 movingSpeed)
{
	static const KyUInt32 textSize = 128;
	char text[textSize];

	KyFloat32 score = 1.0f;
	if(scoreAccessorFunction != NULL)
		score = (velocitySample.*scoreAccessorFunction)(config);

	Vec3f position = rootPosition + Vec3f(velocitySample.m_normalizedDirection.x, velocitySample.m_normalizedDirection.y, 0.0f) * movingSpeed;
	Vec3f height   = Vec3f(0.0f, 0.0f, 2.0f) * score;

	displayList_Samples.PushLine(position, position + height, VisualColor::Red);

	if(scoreAccessorFunction == NULL)
	{
		displayList_Samples.PushLine(rootPosition, position, VisualColor::Orange);
	}

	SFsprintf(text, textSize, "%.2f", score);
	displayList_Samples.PushText(position + height + Vec3f(0.0f, 0.0f, 0.1f), VisualColor::White, text);
}

template<class TLogic>
void AvoidanceSolver<TLogic>::DebugDrawSamples(const char* groupName, const char* displayListName, Kaim::World* world, KyUInt32 visualDebugId, const Vec3f& rootPosition, VelocitySample::ScoreAccessorFunction scoreAccessorFunction, const AvoidanceConfig& config, KyFloat32 movingSpeed)
{
	ScopedDisplayList displayList_Samples(world, DisplayList_Enable);
	displayList_Samples.InitSingleFrameLifespan(displayListName, groupName, visualDebugId);
	displayList_Samples.HideByDefaultInNavigationLab();

	for (KyUInt32 i = 0; i < m_velocitySamples.GetCount(); ++i)
	{
		VelocitySample& velocitySample = m_velocitySamples[i];
		DebugDrawSample(displayList_Samples, velocitySample, rootPosition, scoreAccessorFunction, config, movingSpeed);
	}
}

template<class TLogic>
void AvoidanceSolver<TLogic>::DebugDraw(AvoidanceSolverConfig& config, const Vec3f& previousVelocity, const Vec3f& desiredVelocity, const Vec3f& outputVelocity, const VelocitySample& noAvoidanceSample, const VelocitySample& favoriteSample, KyFloat32 movingSpeed)
{
	const char groupName[64] = "AvoidanceSolver_UtilityFunctionSolver";

	ScopedDisplayList displayList_PreviousVelocity     (config.m_bot->GetWorld(), DisplayList_Enable);
	ScopedDisplayList displayList_DesiredVelocity      (config.m_bot->GetWorld(), DisplayList_Enable);
	ScopedDisplayList displayList_Settings             (config.m_bot->GetWorld(), DisplayList_Enable);
	ScopedDisplayList displayList_FinalVelocity        (config.m_bot->GetWorld(), DisplayList_Enable);
	ScopedDisplayList displayList_DesiredVelocityScore (config.m_bot->GetWorld(), DisplayList_Enable);
	ScopedDisplayList displayList_Collisions           (config.m_bot->GetWorld(), DisplayList_Enable);

	displayList_PreviousVelocity    .InitSingleFrameLifespan("Inputs - previousVelocity"          , groupName, config.m_bot->GetVisualDebugId());
	displayList_DesiredVelocity     .InitSingleFrameLifespan("Inputs - DesiredVelocity"           , groupName, config.m_bot->GetVisualDebugId());
	displayList_Settings            .InitSingleFrameLifespan("Inputs - Settings"                  , groupName, config.m_bot->GetVisualDebugId());
	displayList_FinalVelocity       .InitSingleFrameLifespan("Output - Output Velocity"           , groupName, config.m_bot->GetVisualDebugId());
	displayList_DesiredVelocityScore.InitSingleFrameLifespan("7 - Final desired velocity score"   , groupName, config.m_bot->GetVisualDebugId());
	displayList_Collisions          .InitSingleFrameLifespan("Detected Collisions"                , groupName, config.m_bot->GetVisualDebugId());

	displayList_PreviousVelocity    .HideByDefaultInNavigationLab();
	displayList_DesiredVelocity     .HideByDefaultInNavigationLab();
	displayList_Settings            .HideByDefaultInNavigationLab();
	displayList_FinalVelocity       .HideByDefaultInNavigationLab();
	displayList_DesiredVelocityScore.HideByDefaultInNavigationLab();
	displayList_Collisions          .HideByDefaultInNavigationLab();

	displayList_PreviousVelocity.PushLine(config.m_bot->GetPosition(), config.m_bot->GetPosition() + previousVelocity, VisualColor::Red);
	displayList_DesiredVelocity. PushLine(config.m_bot->GetPosition(), config.m_bot->GetPosition() + desiredVelocity, VisualColor::Green);

	static const KyUInt32 textSize = 64;
	char text[textSize];

	SFsprintf(text, textSize, "MinimalTimeToCollision = %.2f", config.m_avoidanceComputerConfig->m_minimalTimeToCollision);
	displayList_Settings.PushText(config.m_bot->GetPosition() + Vec3f(0.0f, 0.0f, 2.1f), VisualColor::White, text);

	SFsprintf(text, textSize, "DesiredVelocityInfluence = %.2f", config.m_avoidanceComputerConfig->m_desiredVelocityInfluence);
	displayList_Settings.PushText(config.m_bot->GetPosition() + Vec3f(0.0f, 0.0f, 2.3f), VisualColor::White, text);

	SFsprintf(text, textSize, "PreviousVelocityInfluence = %.2f", config.m_avoidanceComputerConfig->m_previousVelocityInfluence);
	displayList_Settings.PushText(config.m_bot->GetPosition() + Vec3f(0.0f, 0.0f, 2.5f), VisualColor::White, text);

	SFsprintf(text, textSize, "TimeToCollisionInfluence = %.2f", config.m_avoidanceComputerConfig->m_timeToCollisionInfluence);
	displayList_Settings.PushText(config.m_bot->GetPosition() + Vec3f(0.0f, 0.0f, 2.7f), VisualColor::White, text);

	DebugDrawSamples(groupName, "0 - Generated Samples", config.m_bot->GetWorld(), config.m_bot->GetVisualDebugId(), config.m_bot->GetPosition(), NULL, config.m_bot->GetAvoidanceConfig(), movingSpeed);
	DebugDrawSamples(groupName, "1 - CollisionTime", config.m_bot->GetWorld(), config.m_bot->GetVisualDebugId(), config.m_bot->GetPosition(), &VelocitySample::GetCollisionTime, config.m_bot->GetAvoidanceConfig(), movingSpeed);
	DebugDrawSamples(groupName, "2 - TimeToImpact", config.m_bot->GetWorld(), config.m_bot->GetVisualDebugId(), config.m_bot->GetPosition(), &VelocitySample::GetTimeToImpactScore, config.m_bot->GetAvoidanceConfig(), movingSpeed);
	DebugDrawSamples(groupName, "3 - Smoothed TimeToImpact", config.m_bot->GetWorld(), config.m_bot->GetVisualDebugId(), config.m_bot->GetPosition(), &VelocitySample::GetSmoothedTimeToImpactScore, config.m_bot->GetAvoidanceConfig(), movingSpeed);
	DebugDrawSamples(groupName, "4 - Desired Velocity", config.m_bot->GetWorld(), config.m_bot->GetVisualDebugId(), config.m_bot->GetPosition(), &VelocitySample::GetDesiredVelocityScore, config.m_bot->GetAvoidanceConfig(), movingSpeed);
	DebugDrawSamples(groupName, "5 - Previous Velocity", config.m_bot->GetWorld(), config.m_bot->GetVisualDebugId(), config.m_bot->GetPosition(), &VelocitySample::GetPreviousVelocityScore, config.m_bot->GetAvoidanceConfig(), movingSpeed);
	DebugDrawSamples(groupName, "6 - Final Turn Velocities Scores", config.m_bot->GetWorld(), config.m_bot->GetVisualDebugId(), config.m_bot->GetPosition(), &VelocitySample::ComputeFinalScore, config.m_bot->GetAvoidanceConfig(), movingSpeed);
	DebugDrawSample(displayList_DesiredVelocityScore, noAvoidanceSample, config.m_bot->GetPosition(), &VelocitySample::ComputeFinalScore, config.m_bot->GetAvoidanceConfig(), movingSpeed);

	displayList_FinalVelocity.PushLine(config.m_bot->GetPosition(), config.m_bot->GetPosition() + outputVelocity, VisualColor::White);

	switch(favoriteSample.m_type)
	{
	case(IAvoidanceComputer::NoAvoidance)      : { SFsprintf(text, textSize, "NoAvoidance"); break;}
	case(IAvoidanceComputer::SlowDown)         : { SFsprintf(text, textSize, "SlowDown");break;}
	case(IAvoidanceComputer::Accelerate)       : { SFsprintf(text, textSize, "Accelerate");break;}
	case(IAvoidanceComputer::Turn)             : { SFsprintf(text, textSize, "Turn");break;}
	case(IAvoidanceComputer::Stop)             : { SFsprintf(text, textSize, "Stop");break;}
	case(IAvoidanceComputer::SolutionNotFound) : { SFsprintf(text, textSize, "SolutionNotFound");break;}
	case(IAvoidanceComputer::NoTrajectory)     : { SFsprintf(text, textSize, "NoTrajectory");break;}
	default: { break;}
	}
	displayList_FinalVelocity.PushText(config.m_bot->GetPosition() + Vec3f(0.0f, 0.0f, 2.9f), VisualColor::White, text);

	for (KyUInt32 i = 0; i < m_velocitySamples.GetCount(); ++i)
	{
		VelocitySample& velocitySample = m_velocitySamples[i];
		if(velocitySample.m_colliderIndex != -1)
		{
			Vec3f intersect = config.m_bot->GetPosition() + (Vec3f(velocitySample.m_normalizedDirection.x, velocitySample.m_normalizedDirection.y, 0.0f) * movingSpeed * velocitySample.m_collisionTime);
			VisualShapeColor colorA; colorA.m_lineColor = VisualColor::Orange; colorA.m_triangleColor = VisualColor::Blue; 
			float altitude = config.m_bot->GetPosition().z;
			displayList_Collisions.PushDisk(Vec3f(intersect.x, intersect.y, altitude), config.m_workspace->m_rootCollider.m_radius,6, colorA);
			displayList_Collisions.PushLine(Vec3f(intersect.x, intersect.y, altitude), Vec3f(intersect.x, intersect.y, altitude + 1.0f), VisualColor::White);
			displayList_Collisions.PushLine(Vec3f(intersect.x, intersect.y, altitude), config.m_bot->GetPosition(), VisualColor::IndianRed);
			SFsprintf(text, textSize, "%.2f", velocitySample.m_collisionTime);
			displayList_Collisions.PushText(Vec3f(intersect.x, intersect.y, altitude + 1.1f), VisualColor::White, text);

			const AvoidanceCollider& finalCollider = config.m_workspace->m_colliders[velocitySample.m_colliderIndex];
			intersect = Vec3f(finalCollider.m_position.x, finalCollider.m_position.y, altitude) + (Vec3f(finalCollider.m_velocity.x, finalCollider.m_velocity.y, 0.0f) * velocitySample.m_collisionTime);
			colorA.m_lineColor = VisualColor::Green;  colorA.m_triangleColor = VisualColor::Red; 
			displayList_Collisions.PushDisk(Vec3f(intersect.x, intersect.y, altitude), finalCollider.m_radius,6, colorA);
			displayList_Collisions.PushLine(Vec3f(intersect.x, intersect.y, altitude), Vec3f(finalCollider.m_position.x, finalCollider.m_position.y, altitude), VisualColor::IndianRed);
		}
	}
}


}
