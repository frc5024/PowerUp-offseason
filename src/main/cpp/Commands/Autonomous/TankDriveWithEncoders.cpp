#include "TankDriveWithEncoders.h"
#include "../../Subsystems/DriveTrainMap.h"
#include "../../Utilities/Log.h"

/**
 * distance in inches, speed from -1 to 1
 */
TankDriveWithEncoders::TankDriveWithEncoders(double distance, double timeout)
{
	LOG("[TankDriveWithEncoders] Constructed: " << (unsigned int)this );
	SmartDashboard::PutNumber("distance", distance);
	this->dDistance = distance;
	this->dTimeout = timeout;
	if (CommandBase::pDriveTrain != nullptr)
	{
		Requires(CommandBase::pDriveTrain);
	}
	else
	{
		LOG("[TrankDriveWithEncoders] driveTrain is null!");
	}

	this->pTimer = new Timer();

	return;
}

/**
 *
 */
TankDriveWithEncoders::~TankDriveWithEncoders()
{
	delete this->pTimer;
}

/**
 *
 */
void TankDriveWithEncoders::Initialize()
{
	accelTarget = dDistance * 0.15;

	LOG("[TankDriveWithEncoders] Initialized with target: "<<dDistance<<" & accelTarget: "<<accelTarget) ;
	LOG("\t" << (unsigned int)this );

	this->pTimer->Reset();
	this->pTimer->Start();

	CommandBase::pDriveTrain->InitAutonomousMode(false);

//	if (dDistance > 0){
//		dSpeed = dSpeed*-1;
//	}
	SmartDashboard::PutNumber("Acceleration Distance", accelTarget);
	SmartDashboard::PutNumber("TargetDistance", dDistance);
	CommandBase::pDriveTrain->ResetEncoders();
//	CommandBase::pDriveTrain->InitAutonomousMode(true); // invert right front motor
//	CommandBase::pDriveTrain->Drive(dDistance, dSpeed);
//	startingPosition = CommandBase::pDriveTrain->GetRightPosition();
	return;
}

/**
 *
 */
void TankDriveWithEncoders::Execute()
{
//	::SmartDashboard::PutNumber("StartingPosition", startingPosition);


	 leftEncoderPosition =  CommandBase::pDriveTrain->GetLeftPosition();

	SmartDashboard::PutNumber("leftEncoderPosition", leftEncoderPosition);

	 rightEncoderPosition =  CommandBase::pDriveTrain->GetRightPosition();

	SmartDashboard::PutNumber("RightEncoderPosition", rightEncoderPosition);

//	inchesTraveled = encoderPosition * INCHES_PER_TICK;

	leftInchesTraveled = (leftEncoderPosition) * INCHES_PER_TICK;
	SmartDashboard::PutNumber("Left InchesTraveled", leftInchesTraveled);

	rightInchesTraveled = (rightEncoderPosition) * INCHES_PER_TICK;
	SmartDashboard::PutNumber("Right Inches Traveled", rightInchesTraveled);

	if(fabs(leftInchesTraveled) < fabs(accelTarget)){
		dLeftSpeed = fabs(minSpeed) + fabs(0.05 * leftInchesTraveled);
	}else{
		dLeftSpeed = ((fabs(dDistance) -  fabs(0.5 * leftInchesTraveled))/fabs(dDistance));
		if(dLeftSpeed < minSpeed) dLeftSpeed = minSpeed;
	}


	if(fabs(rightInchesTraveled) < fabs(accelTarget)){
			dRightSpeed = fabs(minSpeed) + fabs(0.05 * rightInchesTraveled);
		}else{
			dRightSpeed = ((fabs(dDistance) -  fabs(0.5 * rightInchesTraveled))/fabs(dDistance));
			if(dRightSpeed < minSpeed) dRightSpeed = minSpeed;
		}


	if(fabs(rightInchesTraveled)>= fabs(dDistance)){
		dRightSpeed = 0;
	}

	if(fabs(leftInchesTraveled)>= fabs(dDistance)){
		dLeftSpeed = 0;
	}

	SmartDashboard::PutNumber("Left Speed", dLeftSpeed);
	SmartDashboard::PutNumber("Right Speed", dRightSpeed);
	//	if (dDistance > 0){
	if (dDistance <0){
		dRightSpeed = dRightSpeed * -1;
		dLeftSpeed = dLeftSpeed * -1;
	}
//	if (iCounter++ > 10)
//		LOG("dLeftSpeed: "<<dLeftSpeed<<" | dRightSpeed: "<<dRightSpeed) ;
	CommandBase::pDriveTrain->TankDrive(-dRightSpeed, dRightSpeed);

	this->m_ExecuteCalledCount++;

	return;
}

/**
 *
 */
bool TankDriveWithEncoders::IsFinished()
{
//	return CommandBase::pDriveTrain->IsDriving();

	this->m_IsFinishedCalledCount++;

	if (this->pTimer->Get() > dTimeout) // stop after dTimeout seconds no matter what
	{
		LOG("[TankDriveWithEncoder] Timed out: " << dTimeout);

		return true;
	}

	if(fabs(rightInchesTraveled)>= fabs(dDistance)){
		LOG("[TankDriveWithEncoder] Surpassed distance");
		LOG("\t" << (unsigned int)this );

		LOG("\t Execute called " << this->m_ExecuteCalledCount << " times.");
		LOG("\t IsFinished called " << this->m_IsFinishedCalledCount << " times.");

		return true;
	}

	return false;
}

/**
 *
 */
void TankDriveWithEncoders::End()
{
	LOG("[TankDriveWithEncoders] Ended " << (unsigned int)this);

	CommandBase::pDriveTrain->ResetDrive();

	return;
}

/**
 *
 */
void TankDriveWithEncoders::Interrupted()
{
	LOG("[TankDriveWithEncoders] Interrupted" );

	CommandBase::pDriveTrain->ResetDrive();

	return;
}
