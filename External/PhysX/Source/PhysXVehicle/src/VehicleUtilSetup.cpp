// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include "PxVehicleSDK.h"
#include "PxVehicleUtilSetup.h"
#include "PxVehicleDrive4W.h"
#include "PxVehicleWheels.h"
#include "PxVehicleLinearMath.h"
#include "PsFoundation.h"
#include "PsUtilities.h"

namespace physx
{

void PxVehicle4WEnable3WMode(const bool removeFrontWheel, PxVehicleWheelsSimData& wheelsSimData, PxVehicleDriveSimData4W& driveSimData)
{
	const PxU32 wheelToRemove = removeFrontWheel ? PxVehicleDrive4W::eFRONT_LEFT_WHEEL : PxVehicleDrive4W::eREAR_LEFT_WHEEL;
	const PxU32 wheelToModify =  removeFrontWheel ? PxVehicleDrive4W::eFRONT_RIGHT_WHEEL : PxVehicleDrive4W::eREAR_RIGHT_WHEEL;

	//Disable the front left wheel.
	wheelsSimData.disableWheel(wheelToRemove);

	//Now reposition the front-right wheel so that it lies at the centre of the front axle.
	{
		PxVec3 offsets[4]={
			wheelsSimData.getWheelCentreOffset(PxVehicleDrive4W::eFRONT_LEFT_WHEEL),
			wheelsSimData.getWheelCentreOffset(PxVehicleDrive4W::eFRONT_RIGHT_WHEEL),
			wheelsSimData.getWheelCentreOffset(PxVehicleDrive4W::eREAR_LEFT_WHEEL),
			wheelsSimData.getWheelCentreOffset(PxVehicleDrive4W::eREAR_RIGHT_WHEEL)};

			offsets[wheelToModify].x=0;

			wheelsSimData.setWheelCentreOffset(PxVehicleDrive4W::eFRONT_LEFT_WHEEL,offsets[PxVehicleDrive4W::eFRONT_LEFT_WHEEL]);
			wheelsSimData.setWheelCentreOffset(PxVehicleDrive4W::eFRONT_RIGHT_WHEEL,offsets[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL]);
			wheelsSimData.setWheelCentreOffset(PxVehicleDrive4W::eREAR_LEFT_WHEEL,offsets[PxVehicleDrive4W::eREAR_LEFT_WHEEL]);
			wheelsSimData.setWheelCentreOffset(PxVehicleDrive4W::eREAR_RIGHT_WHEEL,offsets[PxVehicleDrive4W::eREAR_RIGHT_WHEEL]);
	}
	{
		PxVec3 suspOffset=wheelsSimData.getSuspForceAppPointOffset(wheelToModify);
		suspOffset.x=0;
		wheelsSimData.setSuspForceAppPointOffset(wheelToModify,suspOffset);
	}
	{
		PxVec3 tireOffset=wheelsSimData.getTireForceAppPointOffset(wheelToModify);
		tireOffset.x=0;
		wheelsSimData.setTireForceAppPointOffset(wheelToModify,tireOffset);
	}

	if(PxVehicleDrive4W::eFRONT_RIGHT_WHEEL==wheelToModify)
	{
		//Disable the ackermann steer correction because we only have a single steer wheel now.
		PxVehicleAckermannGeometryData ackermannData=driveSimData.getAckermannGeometryData();
		ackermannData.mAccuracy=0.0f;
		driveSimData.setAckermannGeometryData(ackermannData);
	}

	//We need to set up the differential to make sure that the missing wheel is ignored.
	PxVehicleDifferential4WData diffData =driveSimData.getDiffData();
	if(PxVehicleDrive4W::eFRONT_RIGHT_WHEEL==wheelToModify)	
	{
		diffData.mFrontBias=PX_MAX_F32;
		diffData.mFrontLeftRightSplit=0.0f;
	}
	else
	{
		diffData.mRearBias=PX_MAX_F32;
		diffData.mRearLeftRightSplit=0.0f;
	}
	driveSimData.setDiffData(diffData);

	//The front-right wheel needs to support the mass that was supported by the disabled front-left wheel.
	//Update the suspension data to preserve both the natural frequency and damping ratio.
	PxVehicleSuspensionData suspData=wheelsSimData.getSuspensionData(wheelToModify);
	suspData.mSprungMass*=2.0f;
	suspData.mSpringStrength*=2.0f;
	suspData.mSpringDamperRate*=2.0f;
	wheelsSimData.setSuspensionData(wheelToModify,suspData);
}

void PxVehicle4WEnable3WTadpoleMode(PxVehicleWheelsSimData& wheelsSimData, PxVehicleDriveSimData4W& driveSimData)
{
	PxVehicle4WEnable3WMode(false,wheelsSimData,driveSimData);
}

void PxVehicle4WEnable3WDeltaMode(PxVehicleWheelsSimData& wheelsSimData, PxVehicleDriveSimData4W& driveSimData)
{
	PxVehicle4WEnable3WMode(true,wheelsSimData,driveSimData);
}

void PxVehicleComputeSprungMasses(const PxU32 numSprungMasses, const PxVec3* sprungMassCoordinates, const PxVec3& centreOfMass, const PxReal totalMass, const PxU32 gravityDirection, PxReal* sprungMasses)
{
	if(1==numSprungMasses)
	{
		sprungMasses[0]=totalMass;
	}
	else if(2==numSprungMasses)
	{
		PxVec3 v=sprungMassCoordinates[0];
		v[gravityDirection]=0;
		PxVec3 w=sprungMassCoordinates[1]-sprungMassCoordinates[0];
		w[gravityDirection]=0;
		w.normalize();

		PxVec3 cm=centreOfMass;
		cm[gravityDirection]=0;
		PxF32 t=w.dot(cm-v);
		PxVec3 p=v+w*t;

		PxVec3 x0=sprungMassCoordinates[0];
		x0[gravityDirection]=0;
		PxVec3 x1=sprungMassCoordinates[1];
		x1[gravityDirection]=0;
		const PxF32 r0=(x0-p).dot(w);
		const PxF32 r1=(x1-p).dot(w);

		const PxF32 m0=totalMass*r1/(r1-r0);
		const PxF32 m1=totalMass-m0;

		sprungMasses[0]=m0;
		sprungMasses[1]=m1;
	}
	else if(3==numSprungMasses)
	{
		const PxU32 d0=(gravityDirection+1)%3;
		const PxU32 d1=(gravityDirection+2)%3;

		MatrixNN A(3);
		VectorN b(3);
		A.set(0,0,sprungMassCoordinates[0][d0]);
		A.set(0,1,sprungMassCoordinates[1][d0]);
		A.set(0,2,sprungMassCoordinates[2][d0]);
		A.set(1,0,sprungMassCoordinates[0][d1]);
		A.set(1,1,sprungMassCoordinates[1][d1]);
		A.set(1,2,sprungMassCoordinates[2][d1]);
		A.set(2,0,1);
		A.set(2,1,1);
		A.set(2,2,1);
		b[0]=totalMass*centreOfMass[d0];
		b[1]=totalMass*centreOfMass[d1];
		b[2]=totalMass;

		VectorN result(3);
		MatrixNNLUSolver solver;
		solver.decomposeLU(A);
		solver.solve(b,result);

		sprungMasses[0]=result[0];
		sprungMasses[1]=result[1];
		sprungMasses[2]=result[2];
	}
	else if(numSprungMasses>=4)
	{
		const PxU32 d0=(gravityDirection+1)%3;
		const PxU32 d1=(gravityDirection+2)%3;

		const PxF32 mbar = totalMass/(numSprungMasses*1.0f);

		//See http://en.wikipedia.org/wiki/Lagrange_multiplier
		//particularly the section on multiple constraints.

		//3 Constraint equations.
		//g0 = sum_ xi*mi=xcm	
		//g1 = sum_ zi*mi=zcm	
		//g2 = sum_ mi = totalMass		
		//Minimisation function to achieve solution with minimum mass variance.
		//f = sum_ (mi - mave)^2 
		//Lagrange terms (N equations, N+3 unknowns)
		//2*mi  - xi*lambda0 - zi*lambda1 - 1*lambda2 = 2*mave

		MatrixNN A(numSprungMasses+3);
		VectorN b(numSprungMasses+3);

	
		//g0, g1, g2
		for(PxU32 i=0;i<numSprungMasses;i++)
		{
			A.set(0,i,sprungMassCoordinates[i][d0]);	//g0
			A.set(1,i,sprungMassCoordinates[i][d1]);	//g1
			A.set(2,i,1.0f);							//g2
		}
		for(PxU32 i=numSprungMasses;i<numSprungMasses+3;i++)
		{
			A.set(0,i,0);								//g0 independent of lambda0,lambda1,lambda2
			A.set(1,i,0);								//g1 independent of lambda0,lambda1,lambda2
			A.set(2,i,0);								//g2 independent of lambda0,lambda1,lambda2
		}
		b[0] = totalMass*(centreOfMass[d0]);			//g0
		b[1] = totalMass*(centreOfMass[d1]);			//g1
		b[2] = totalMass;								//g2

		//Lagrange terms.
		for(PxU32 i=0;i<numSprungMasses;i++)
		{
			//Off-diagonal terms from the derivative of f
			for(PxU32 j=0;j<numSprungMasses;j++)
			{
				A.set(i+3,j,0);
			}
			//Diagonal term from the derivative of f
			A.set(i+3,i,2);

			//Derivative of g
			A.set(i+3,numSprungMasses+0,sprungMassCoordinates[i][d0]);
			A.set(i+3,numSprungMasses+1,sprungMassCoordinates[i][d1]);
			A.set(i+3,numSprungMasses+2,1.0f);

			//rhs.
			b[i+3] = 2*mbar;
		}

		//Solve Ax=b
		VectorN result(numSprungMasses+3);
		MatrixNNLUSolver solver;
		solver.decomposeLU(A);
		solver.solve(b,result);

		for(PxU32 i=0;i<numSprungMasses;i++)
		{
			sprungMasses[i]=result[i];
		}
	}
}

}//physx