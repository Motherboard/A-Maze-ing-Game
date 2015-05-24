#include "MazeGameWorldModel.h"
#include "MazePlayerHumanController.h"
#include "MazePlayerAIController.h"
#include <random>
#include <chrono>
#include <algorithm>


namespace amazeinggame
{


	CMazeGameWorldModel::CMazeGameWorldModel()
	{
	}


	CMazeGameWorldModel::~CMazeGameWorldModel()
	{
	}
	void CMazeGameWorldModel::initGameWorld(unsigned char width, unsigned char height, unsigned char numOfHumanPlayers, unsigned char numOfAIPlayers, unsigned char AIDifficultyLevel)
	{
		_players.clear();
		_mazeWinnerIdx = -1;
		_numOfPlayers = numOfAIPlayers + numOfHumanPlayers;
		_numOfHumanPlayers = numOfHumanPlayers;
		_maze.generateMaze(width, height);
		unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
		auto randomEngine = std::default_random_engine(seed);
		int numOfPossiblePlacementsFound = 0;
		int numOfTries = 0;
		std::vector<std::pair<int, int>> possiblePlayerPlacement;
		do
		{
			auto randomXCoord = std::uniform_int_distribution<>(0, width - 1);
			auto randomYCoord = std::uniform_int_distribution<>(0, height - 1);
			_finishPoint = std::make_pair<unsigned char, unsigned char>(randomXCoord(randomEngine), randomYCoord(randomEngine));
			int playerDistanceFromFinishPoint = std::max(width, height);
			possiblePlayerPlacement = _maze.getAllEquidistantPositionsFromPosition(
				_finishPoint.first, _finishPoint.second, playerDistanceFromFinishPoint);
			
			while (possiblePlayerPlacement.size() < _numOfPlayers && playerDistanceFromFinishPoint < std::max(width, height))
			{
				auto tempMorePositions = _maze.getAllEquidistantPositionsFromPosition(
					_finishPoint.first, _finishPoint.second, ++playerDistanceFromFinishPoint);
				possiblePlayerPlacement.insert(end(possiblePlayerPlacement), begin(tempMorePositions), end(tempMorePositions));
			}
			numOfPossiblePlacementsFound = possiblePlayerPlacement.size();
			if (numOfTries > width * height)
			{
				throw std::exception("Too many players were requested - This maze is not big enough for the both of us.");
			}
		} while (numOfPossiblePlacementsFound < _numOfPlayers);
			
		std::shuffle(std::begin(possiblePlayerPlacement), std::end(possiblePlayerPlacement), randomEngine);
		_players.reserve(_numOfPlayers);
		for (auto i = 0; i < _numOfPlayers; ++i)
		{
			_players.emplace_back();
			if (i < numOfHumanPlayers)
			{
				_players.back().init(possiblePlayerPlacement[i].first, possiblePlayerPlacement[i].second, this,
					std::unique_ptr<CMazePlayerControllerInterface>(new CMazePlayerHumanController()));
			}
			else
			{
				_players.back().init(possiblePlayerPlacement[i].first, possiblePlayerPlacement[i].second, this,
					std::unique_ptr<CMazePlayerControllerInterface>(new CMazePlayerAIController(*this,AIDifficultyLevel)));

			}
		}
	}
	
	void CMazeGameWorldModel::evolve(float deltaT)
	{
		for (auto playerIdx = 0; playerIdx < _numOfPlayers; ++playerIdx)
		{
			auto & player = _players[playerIdx];
			player.evolve(deltaT);
			auto playerState = player.getPlayerState();
			if (std::round(playerState.x) == _finishPoint.first && std::round(playerState.y) == _finishPoint.second)
			{ //if player is standing on finish point, make him the winner
				_mazeWinnerIdx = playerIdx;
				for (auto i = 0; i < _numOfPlayers; ++i)
				{
					if (i != playerIdx)
					{
						_players[i].setPlayerDead();
					}
				}
			}
		}
	}
	std::pair<int, int> CMazeGameWorldModel::getFinishPoint() const
	{
		return _finishPoint;
	}
	const maze::CMaze & CMazeGameWorldModel::getMaze() const
	{
		return _maze;
	}
	const CMazePlayerModel & CMazeGameWorldModel::getHumanPlayer(unsigned char in_humanPlayerIdx) const
	{
		if (in_humanPlayerIdx > _numOfHumanPlayers)
		{
			throw std::exception("There are not enough human players.. this index was out of range");
		}
		return _players[in_humanPlayerIdx];
	}

	const CMazePlayerModel & CMazeGameWorldModel::getAIPlayer(unsigned char in_AIPlayerIdx) const
	{
		if (in_AIPlayerIdx + _numOfHumanPlayers >= _players.size())
		{
			throw std::exception("There are not enough AI players.. this index was out of range");
		}
		return _players[_numOfHumanPlayers + in_AIPlayerIdx];
	}

	unsigned char CMazeGameWorldModel::getNumOfHumanPlayers() const
	{
		return _numOfHumanPlayers;
	}
	unsigned char CMazeGameWorldModel::getNumOfAIPlayers() const
	{
		return _players.size() - _numOfHumanPlayers;
	}
	unsigned char CMazeGameWorldModel::getMazeWinnerIdx() const
	{
		return _mazeWinnerIdx;
	}
	const std::string & CMazeGameWorldModel::getMazeWinnerName() const
	{
		return _players[_mazeWinnerIdx].getPlayerName();
	}
	bool CMazeGameWorldModel::isMazeGameWon() const
	{
		return (_mazeWinnerIdx >= 0);
	}

}