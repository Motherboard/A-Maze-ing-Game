#pragma once
#include <memory>
//#include "CMaze.h"
#include <string>

namespace maze {
	enum class Direction;
}
namespace amazeinggame
{

	struct CPlayerInnerData;

	class CMazePlayerControllerInterface;
	class CMazePlayerHumanController;
	class CMazeGameWorldModel;

	struct CMazePlayerState
	{
		float x, y, speedX, speedY, currentAngle, angularSpeed, remainingAngle;
		bool isPlayerDead;
	};

	class CMazePlayerModel
	{
	public:
		CMazePlayerModel();
		CMazePlayerModel(const CMazePlayerModel &) = delete;
		CMazePlayerModel(CMazePlayerModel &&);
		~CMazePlayerModel();
		void setDirection(maze::Direction in_newDirection);
		void stop();
		void evolve(float deltaT);
		void init(int in_mazePositionX, int in_mazePositionY, const CMazeGameWorldModel * const in_worldModel,
			std::unique_ptr<CMazePlayerControllerInterface> && in_controller);
		void setPlayerMovementSpeed(float in_speed);
		CMazePlayerHumanController * const getMazePlayerHumanController() const;
		CMazePlayerState getPlayerState() const;
		void setPlayerName(std::string in_playerName); 
		const std::string & getPlayerName() const;
		void setPlayerDead();
	private:
		bool isAboutToCollide();
		void moveStraight(float deltaT);
		void turn(float deltaT);
		bool checkAndUseMovementQueue();
		std::unique_ptr<CPlayerInnerData> _pInnerData; //this is the place to hide the implementation details, so we don't have to include irrlicht from the header.
		std::unique_ptr<CMazePlayerControllerInterface> _controller;
		const CMazeGameWorldModel * _worldModel = nullptr; //there can be no maze player without a world...
		std::string _name;
		bool _isPlayerDead = false;
	};

}