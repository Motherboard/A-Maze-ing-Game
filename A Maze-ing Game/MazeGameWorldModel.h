#pragma once

#include "CMaze.h"
#include "MazePlayerModel.h"
#include <vector>

//This class holds the game world model and the player models.
//It calls their evolve methods, and determines who wins.

namespace amazeinggame
{

	class CMazeGameWorldModel
	{
	public:
		CMazeGameWorldModel();
		~CMazeGameWorldModel();
		void initGameWorld(unsigned char width, unsigned char height, unsigned char numOfHumanPlayers, unsigned char numOfAIPlayers, unsigned char AIDifficultyLevel);
		//void assignContollerToPlayer(unsigned char playerId ,controller );
		void evolve(float deltaT);
		std::pair<int, int> getFinishPoint() const;
		const maze::CMaze & getMaze() const;
		const CMazePlayerModel & getHumanPlayer(unsigned char in_humanPlayerIdx) const;
		const CMazePlayerModel & getAIPlayer(unsigned char in_AIPlayerIdx) const;
		unsigned char getNumOfHumanPlayers() const;
		unsigned char getNumOfAIPlayers() const;
		unsigned char getMazeWinnerIdx() const;
		const std::string & getMazeWinnerName() const;
		bool isMazeGameWon() const;
		bool isWorldInitialized() const;
	private:
		maze::CMaze _maze;
		std::vector<CMazePlayerModel> _players; //first _numOfHumanPlayers are human, the rest AI
		std::pair<unsigned char, unsigned char> _finishPoint;
		unsigned char _numOfHumanPlayers;
		int _mazeWinnerIdx = -1;
		unsigned int _numOfPlayers = 0;
		bool _isInitialized = false;
	};

}