#pragma once
#include <memory>
#include "CMaze.h"


struct CPlayerInnerData;

struct CMazePlayerState
{
	float x, y, speedX, speedY, currentAngle, angularSpeed, remainingAngle;
};

class CMazePlayerModel
{
public:
	CMazePlayerModel();
	~CMazePlayerModel();
	void setDirection(Maze::Direction in_newDirection);
	void stop();
	void evolve(float deltaT);
	void init(int in_mazePositionX, int in_mazePositionY, const Maze::CMaze & in_maze);
	void setPlayerMovementSpeed(float in_speed);
	CMazePlayerState getPlayerState() const;
private:
	bool isAboutToCollide();
	void moveStraight(float deltaT);
	void turn(float deltaT);
	bool checkAndUseMovementQueue();
	std::unique_ptr<CPlayerInnerData> _pInnerData; //this is the place to hide the direction of movement, so we don't have to include irrlicht from the header.
	const Maze::CMaze * _maze = nullptr; //the player can assume the maze is there - there can be no maze player without a maze...
};

