#include <KickEntry.h>

KickEntry::KickEntry()
{
	vel = 0;
	direction = 0;
	releaseCycle = -1;
	noKickCycle = 0;
	noMoveCycle = 0;
}

bool KickEntry::operator >(const KickEntry a) const
{
	return (value > a.value);
}

bool KickEntry::operator <(const KickEntry a) const
{
	return (value < a.value);
}

//Shoot START
void ShootEntry::evaluate()
{
	int values[15];
	values[0 * 2] = 0;
	values[int(0.5 * 2)] = 1;
	values[1 * 2] = 2;
	values[int(1.5 * 2)] = 3;
	values[2 * 2] = 4;
	values[int(2.5 * 2)] = 5;
	values[3 * 2] = 6;
	values[int(3.5 * 2)] = 7;
	values[4 * 2] = 8;
	values[int(4.5 * 2)] = 9;
	values[5 * 2] = 10;
	values[int(5.5 * 2)] = 10;
	values[6 * 2] = 12;
	values[int(6.5 * 2)] = 5;
	values[7 * 2] = 5;
	values[int(7.5 * 2)] = 0;
	value = values[int(fabs(y) * 2)];
	if (isDirBad)
		value /= 2;
}

bool ShootEntry::operator >(const ShootEntry a) const
{
	if (oppReachCycle != a.oppReachCycle)
		return (oppReachCycle > a.oppReachCycle);
	if (value != a.value)
		return (value > a.value);
	return isSafe;
}

ostream &operator<<(ostream &stream, const ShootEntry &entry)
{
	LOG << "Target Y : " << entry.y << endl;
	LOG << "Step : " << entry.step << endl;
	LOG << "Release Ball Pos : " << entry.releaseBall.getPos() << endl;
	LOG << "Release Ball Vel : Mag:" << entry.releaseBall.getVel().getMagnitude() << " Dir:"
			<< entry.releaseBall.getVel().getDirection() << endl;
	LOG << "oppReachCycle : " << entry.oppReachCycle << endl;
	LOG << "oppReachPoint : " << entry.oppReachPoint << endl;
	LOG << "isSafe : " << entry.isSafe << endl;
	LOG << "minOppDist : " << entry.minOppDist << endl;
	LOG << "isDirBad : " << entry.isDirBad << endl;
	LOG << "Value : " << entry.value << endl;
	return stream;
}

//Shoot END

//Dribble START
void DribbleEntry::calculateValue()
{
	const float reachPointRate = 1;
//	const float deltaCycleRate = 2;
	const float oppCircleCountRate = 0;
	const int deltaCycleMax = 5;
	const float isCrowdedRate = -15;
	const float isAfterOffsideRate = 20;
	const float directionDeltaCycleValues[7] =
	{ 3, 0, -5, -9, -11, -30, -40 };
	int deltaReachCycle = (oppReachCycle - reachCycle);
	deltaReachCycle = min(deltaReachCycle, deltaCycleMax);
	const float deltaCycleValues[deltaCycleMax] =
	{ -5, -1, 2, 4, 6 };
	const float extraValue = 5.f;
	const float isInOppDangerAreaRate = 10;
	const float isInOurDangerAreaRate = -15;
	value = reachPointValue * reachPointRate + deltaCycleValues[deltaReachCycle - 1]
			+ oppCircleCount * oppCircleCountRate + isCrowded * isCrowdedRate
			+ isAfterOffside * isAfterOffsideRate
			+ directionDeltaCycleValues[min(directionDeltaCycle, 4u)]
			+ isInOppDangerArea * isInOppDangerAreaRate + isInOurDangerArea * isInOurDangerAreaRate
			+ extraValue;
	if (fabs(direction) > 80)
		value -= 15;
//	if (invalidOppValue < 1)
//		value -= 15;
//	if (invalidOppValue < 1.75)
//		value -= 5;

}

ostream &operator<<(ostream &stream, const DribbleEntry &entry)
{
	LOG << "Direction : " << entry.direction << endl;
	if (entry.type == ByDash)
		LOG << "Type : " << "ByDash" << endl;
	else if (entry.type == ByKick)
		LOG << "Type : " << "ByKick" << endl;
	else if (entry.type == ByMultiStepKick)
		LOG << "Type : " << "ByMultiStepKick" << endl;
	if (entry.type != ByDash)
	{
		LOG << "Vel : " << entry.vel << endl;
		LOG << "Steps : " << entry.step + 1 << endl;
		LOG << "ReachPoint : " << entry.reachPoint << endl;
		LOG << "ReachPointValue : " << entry.reachPointValue << endl;
		LOG << "ReachCycle : " << entry.reachCycle << endl;
		LOG << "OppReachCycle : " << entry.oppReachCycle << endl;
	}
	LOG << "isCrowded : " << entry.isCrowded << endl;
	LOG << "isAfterOffside : " << entry.isAfterOffside << endl;
	LOG << "directionDeltaCycle : " << entry.directionDeltaCycle << endl;
	LOG << "isInOppDangerArea : " << entry.isInOppDangerArea << endl;
	LOG << "isInOurDangerArea : " << entry.isInOurDangerArea << endl;
	LOG << "oppCircleCount : " << entry.oppCircleCount << endl;
	LOG << "invalidOppValue : " << entry.invalidOppValue << endl;
	LOG << "Value : " << entry.value << endl;
	return stream;
}
//Dribble END

//Pass START
void PassEntry::calculateValue()
{
	const float reachPointRate = 1;
//	const float deltaCycleRate = -1;
	const float oppCircleCountRate = 0;
	const int deltaCycleMax = 5;
	int deltaCycle = (oppReachCycle - tmmReachCycle);
	deltaCycle = min(deltaCycle, deltaCycleMax);
	float deltaCycleValues[5] =
//	{ -30, -5, 0, 5, 10 };
			{ -15, -7, 1, 6, 11 };
	const float isCrowdedRate = -10;
	const float directionSeeDeltaCycleRate = -3;
	const float isInOppDangerAreaRate = 20;
	const float isInOurDangerAreaRate = -30;
	const float isMarkedRate = -10;
	directionSeeDeltaCycle = min(directionSeeDeltaCycle, 5u);
	value = reachPointValue * reachPointRate + deltaCycleValues[deltaCycle - 1]
			+ oppCircleCount * oppCircleCountRate + isCrowded * isCrowdedRate
			+ directionSeeDeltaCycle * directionSeeDeltaCycleRate
			+ isInOppDangerArea * isInOppDangerAreaRate + isInOurDangerArea * isInOurDangerAreaRate
			+ isMarked * isMarkedRate;
	const float maxSpeed = 2;
	const float maxSpeedRate = 10;
	if (reachBallVel.getMagnitude() > maxSpeed)
		value -= maxSpeedRate + (maxSpeedRate * (reachBallVel.getMagnitude() - maxSpeed));
	if (invalidOppValue < 1)
		value -= 15;
	if (invalidOppValue < 2)
		value -= 10;
	value -= 5 - min(releaseBall.getPos().getDistance(reachPoint), 5.f);
	if (isAfterOffside)
		value += 15;
	if (minOppDist < 1)
		value -= 30;
	if (minOppDist < 0.5)
		value -= 60;
}

ostream &operator<<(ostream &stream, const PassType &type)
{
	switch (type)
	{
	case DIRECT:
		stream << "DIRECT";
		break;
	case INDIRECT:
		stream << "INDIRECT";
		break;
	case BREAKER:
		stream << "BREAKER";
		break;
	case COURSE:
		stream << "COURSE";
		break;
	}
	return stream;
}

ostream &operator<<(ostream &stream, const PassEntry &entry)
{
	LOG << "Type : " << entry.type << endl;
	if (entry.target)
	{
		LOG << "Receiver PLM :(Full=2)" << entry.target->getModel() << endl;
		if (entry.target->getModel() == 2)
			LOG << "Receiver UniNum : " << entry.target->getUniNum() << endl;
		LOG << "Receiver Point : " << entry.target->getPos() << endl;
	}
	LOG << "Target Point : " << entry.targetPoint << endl;
	LOG << "Direction : " << entry.direction << endl;
	LOG << "Vel : " << entry.vel << endl;
	LOG << "Step : " << entry.step << endl;
	LOG << "Release Ball Pos : " << entry.releaseBall.getPos() << endl;
	LOG << "Release Ball Vel : Mag:" << entry.releaseBall.getVel().getMagnitude() << " Dir:"
			<< entry.releaseBall.getVel().getDirection() << endl;
	LOG << "ReachPoint : " << entry.reachPoint << endl;
	LOG << "ReachPointValue : " << entry.reachPointValue << endl;
	LOG << "oppReachCycle : " << entry.oppReachCycle << endl;
	LOG << "tmmReachCycle : " << entry.tmmReachCycle << endl;
	LOG << "oppCircleCount : " << entry.oppCircleCount << endl;
	LOG << "isSafe : " << entry.isSafe << endl;
	LOG << "isCrowded : " << entry.isCrowded << endl;
	LOG << "isInOppDangerArea : " << entry.isInOppDangerArea << endl;
	LOG << "isInOurDangerArea : " << entry.isInOurDangerArea << endl;
	LOG << "directionSeeDeltaCycle : " << entry.directionSeeDeltaCycle << endl;
	LOG << "ReachBallVel : Mag:" << entry.reachBallVel.getMagnitude() << " Dir : "
			<< entry.reachBallVel.getDirection() << endl;
	LOG << "InvalidOppValue : " << entry.invalidOppValue << endl;
	LOG << "isMarked : " << entry.isMarked << endl;
	LOG << "isAfterOffside : " << entry.isAfterOffside << endl;
	LOG << "minOppDist : " << entry.minOppDist << endl;
	LOG << "minOppDistCycle : " << entry.minOppDistCycle << endl;
	LOG << "Value : " << entry.value << endl;
	return stream;
}
//Pass END
