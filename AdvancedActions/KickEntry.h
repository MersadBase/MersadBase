#ifndef KICKENTRY_H_
#define KICKENTRY_H_

#include <Command.h>
#include <WorldModel.h>
#include <Logger.h>
#include <Formation.h>

class ChainAction;

class KickEntry
{
public:
	KickEntry();
	float vel;
	float direction;
	unsigned releaseCycle;
	unsigned noKickCycle; //don't kick the ball again untill noKickCycle
	unsigned noMoveCycle; //don't intercept the ball even if you are the fastest
	float value;
	bool operator>(const KickEntry a) const;
	bool operator<(const KickEntry a) const;
};

//Dribble START
enum DribbleType
{
	ByDash, ByKick, ByMultiStepKick
};

class DribbleEntry: public KickEntry
{
public:
	unsigned step;
	Point reachPoint;
	float reachPointValue;
	Point target;
	int reachCycle;
	int oppReachCycle;
	DribbleType type;
	int oppCircleCount; //number of opps within a circle
	bool isCrowded;
	bool isAfterOffside;
	unsigned directionDeltaCycle;
	bool isInOppDangerArea;
	bool isInOurDangerArea;
	float invalidOppValue;
	void calculateValue();
};
ostream &operator<<(ostream &stream, const DribbleEntry &entry);
//Dribble END

//Pass START
enum PassType
{
	DIRECT, INDIRECT, BREAKER, COURSE
};
class PassEntry: public KickEntry
{
public:
	PassType type;
	const Player* target; //not int because of half players
	unsigned targetUniNum;
	PlayerModel targetModel;
	int tmmReachCycle;
	int oppReachCycle;
	Point reachPoint;
	Point targetPoint;
	Ball releaseBall;
	unsigned step;
	vector<ChainAction> actions;
	float reachDist; //target distance to ball when in kickable(for checking safety)
	float reachPointValue;
	int oppCircleCount; //number of opps within a circle
	unsigned directionSeeDeltaCycle;
	bool isCrowded;
	bool isSafe;
	bool isInOppDangerArea;
	bool isInOurDangerArea;
	Vector reachBallVel;
	float invalidOppValue;
	bool isMarked;//different from isCrowded . in baraye jayee ke dar aval hastesh hast momkene collision bede bahash.
	bool isAfterOffside;
	float minOppDist;
	unsigned minOppDistCycle;
	void calculateValue();
//	bool operator<(const PassEntry& a) const;
};
ostream &operator<<(ostream &stream, const PassType &entry);
ostream &operator<<(ostream &stream, const PassEntry &entry);
//Pass END

//Shoot START
class ShootEntry: public KickEntry
{
public:
	float y;
	int oppReachCycle; //opp goalie reach cycle
	Point oppReachPoint;
	unsigned step;
	vector<ChainAction> actions;
	Ball releaseBall;
	bool isSafe;
	float minOppDist;
	bool isDirBad;
	void evaluate(); //sets the value
	bool operator>(const ShootEntry a) const;
};
ostream &operator<<(ostream &stream, const ShootEntry &entry);
//Shoot END

#endif /* KICKENTRY_H_ */
