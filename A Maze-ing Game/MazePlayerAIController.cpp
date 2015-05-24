#include "MazePlayerAIController.h"
#include "MazePlayerModel.h"
#include "MazeGameWorldModel.h"
#include "mathUtils.h"
#include <numeric>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <functional>
#include "easylogging++.h"

namespace amazeinggame
{

	
	CMazePlayerAIController::CMazePlayerAIController(const CMazeGameWorldModel & in_worldModel, unsigned char in_AILevel)
		: _worldModel(&in_worldModel),_lookAheadDepth(in_AILevel)
	{
		_finishPoint = _worldModel->getFinishPoint();
		unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
		randomEngine = std::default_random_engine(seed);
	}


	CMazePlayerAIController::~CMazePlayerAIController()
	{
		//make sure _turnsAlreadTaken is present during the emptying of _turnsToBeTaken and nullifying _lastJunctionTaken - since it will add them there...
		while (!_turnsToBeTaken.empty())
			_turnsToBeTaken.pop();
		_lastJunctionTaken = nullptr;
	}


	void CMazePlayerAIController::update(CMazePlayerModel * const in_playerModel)
	{
		if ( !_worldModel)
		{	
			return; //init was not called, can't do anything...
		}
		if (_worldModel->isMazeGameWon())
		{
			in_playerModel->stop();
			return; //if game was already won, we should stop caring...
		}
		auto playerState = in_playerModel->getPlayerState();
		int x = std::round(playerState.x), y = std::round(playerState.y);
		if (playerState.speedX != 0 || playerState.speedY != 0)
		{
			//check if we're still inside the previous room
			if (x == _prevX && y == _prevY)
			{
				return;
			}
		}
		_prevX = x; _prevY = y;
		auto nextDir = getNextDirection(x, y, _prevDirection); //remember to remove the _prevDirection here, it's unncessary
		if (nextDir == maze::Direction::NotSet)
		{ //means theres a bug somewhere...
			LOG(ERROR) << "Could not figure out where to go from " << x << "," << y << std::endl;
			return;
		}
		if (nextDir != _prevDirection)
		{
			in_playerModel->stop();
			in_playerModel->setDirection(nextDir);
			LOG(DEBUG) << "Go " << nextDir << " on " << x << "," << y << std::endl;
			_prevDirection = nextDir;
		}
	}


	bool CMazePlayerAIController::getPossibleTurns(std::array<maze::Direction,4> &out_possibleTurns, unsigned int & out_numOfPossibleTurns, int & out_preferedDirectionIdx, 
		maze::Direction & out_backDirection, int x, int y, maze::Direction in_direction, int depth)
	{
		auto possibleDirections = _worldModel->getMaze().getAllPossibleDirectionsFromPosition(x, y);
		auto prevDirectionVector = maze::getVectorFromDirection(in_direction);

		out_backDirection = maze::getDirectionFromVector(-prevDirectionVector.first, -prevDirectionVector.second);
		out_numOfPossibleTurns = 0;
		out_preferedDirectionIdx = -1; //if we can continue in the same directio, this would be preferable - keep this for later use. 
		for (const auto & direction : possibleDirections)
		{
			if (direction != out_backDirection && _turnsAlreadyTaken.find(SJunction{ x, y, direction }) == std::end(_turnsAlreadyTaken))
			{
				auto directionVector = maze::getVectorFromDirection(direction);
				if (x + directionVector.first == _finishPoint.first && y + directionVector.second == _finishPoint.second)
				{
					out_preferedDirectionIdx = out_numOfPossibleTurns; //we win!!!
					out_possibleTurns[out_numOfPossibleTurns++] = direction;
					return true;
				}
				if (depth == 0)
				{
					out_possibleTurns[out_numOfPossibleTurns++] = direction;
				}
				else
				{
					//just look at the adjacent room.
					//auto nextPossibleDirections = _worldModel->getMaze().getAllPossibleDirectionsFromPosition(x + directionVector.first, y + directionVector.second);
					int preferableDirectionIdx;
					unsigned int numOfPossibleTurns;
					std::array<maze::Direction, 4> possibleTurns;
					maze::Direction backDirection;
					if (getPossibleTurns(possibleTurns, numOfPossibleTurns, preferableDirectionIdx, backDirection, 
						x + directionVector.first, y + directionVector.second, direction, depth - 1))
					{
						out_preferedDirectionIdx = out_numOfPossibleTurns; //we win!!!
						out_possibleTurns[out_numOfPossibleTurns++] = direction;
						LOG(DEBUG) << "[Depth: " << depth << "] " << x << "," << y << ": Spotted the finish point off to " << direction << std::endl;
						return true;
					}
					if (numOfPossibleTurns > 0)
					{	//this is a possible direction to take.
						if (direction == in_direction)
						{
							out_preferedDirectionIdx = out_numOfPossibleTurns;
						}
						out_possibleTurns[out_numOfPossibleTurns++] = direction;
						LOG(DEBUG) << "[Depth:" << depth << " ] " << x << "," << y << ":The direction " << direction << " can be taken, since it has " << numOfPossibleTurns << " Possible directions leading out of it" << std::endl;
					}
					else {
						LOG(DEBUG) << "[Depth:" << depth << " ] " << x << "," << y << ": The direction " << direction << " was a dead end" << std::endl;
					}
				}
			}
			if (_turnsAlreadyTaken.find(SJunction{ x, y, direction }) != std::end(_turnsAlreadyTaken))
			{
				LOG(DEBUG) << "<*>Ignore possible turn to " << direction << " on " << x << "," << y;
			}
		}
		return false;
	}

	maze::Direction CMazePlayerAIController::getNextDirection(int x, int y, maze::Direction in_direction)
	{
		//check if the new room has a junction in it

		int preferableDirectionIdx;
		unsigned int numOfPossibleTurns;
		std::array<maze::Direction, 4> possibleTurns;
		maze::Direction backDirection;
		if (getPossibleTurns(possibleTurns, numOfPossibleTurns, preferableDirectionIdx, backDirection, x, y, in_direction, _lookAheadDepth))
		{
			return possibleTurns[preferableDirectionIdx]; //this means we know where the target is, no need to mock about.
		}
		if (!isAlreadyWalking && numOfPossibleTurns > 1) //if this is the first time we walk - ignore one possible turn (we would get back to it eventually)
		{
			--numOfPossibleTurns;
			LOG(DEBUG) << "First cell: ignore last option - this is the tree's root - it can be left out" << std::endl;
			LOG(DEBUG) << "ommiting " << x << "," << y << possibleTurns[numOfPossibleTurns] << std::endl;
			isAlreadyWalking = true;
		}
		if (preferableDirectionIdx >= 0)
		{
			if (numOfPossibleTurns == 1) //no need to do anything yet, we're doing fine!
			{
				return in_direction;
			}
			if (_currentWalkState == AIWalkState::Explore)
			{ //only during exploration do we care about the current direction.
				//prefer to keep the same direction.

				//insert the rest of the turns to turnsToBeTaken if they are legal
				for (auto i = 0; i < numOfPossibleTurns; ++i)
				{
					if (i != preferableDirectionIdx)
					{
						_turnsToBeTaken.emplace( x, y, possibleTurns[i], _lastJunctionTaken );
						LOG(DEBUG) << "<*>Remember to go back to take " << possibleTurns[i] << " on " << x << "," << y << std::endl;
					}
				}
				_lastJunctionTaken = make_sharedJunction(x, y, in_direction, _lastJunctionTaken);
				LOG(DEBUG) << "<*>Last junction seen was on " << x << "," << y << " - continued going " << in_direction << std::endl;
				return in_direction;
			}
		}
		//now it's time to decide where to go!
		
		if (numOfPossibleTurns == 0) //this means there were no good turns to take - turn around, and go back to the last junction.
		{ //this should not happen in rollback mode.
			if (_currentWalkState == AIWalkState::Rollback)
			{
				LOG(ERROR) << "Got lost during rollback..." << std::endl;
			}
			if (_turnsToBeTaken.size()) //if there is where to roll back to, rollback.
			{
				_currentWalkState = AIWalkState::Rollback;
				if (_lastJunctionTaken)
				{
					_lastJunctionTaken = nullptr;
				}
				else {
					_currentWalkState = AIWalkState::Explore; //we get here only if rollback failed.
				}
			}
			return backDirection;
		}
		if (numOfPossibleTurns == 1) //if there was just one possible direction to take - take it.
		{
			return possibleTurns[0];
		}
		if (_currentWalkState == AIWalkState::Explore) //now we have no obvious choice for new direction
		{
			iota(std::begin(_possibleTurnIdx), std::begin(_possibleTurnIdx) + numOfPossibleTurns, 0);
			std::shuffle(std::begin(_possibleTurnIdx), std::begin(_possibleTurnIdx) + numOfPossibleTurns, randomEngine);
			for (auto i = 0; i < numOfPossibleTurns - 1; ++i) //first check in the waiting list
			{
				_turnsToBeTaken.emplace( x, y, possibleTurns[_possibleTurnIdx[i]], _lastJunctionTaken );
				LOG(DEBUG) << "<*>Remember to go back to take " << possibleTurns[_possibleTurnIdx[i]] << " on " << x << "," << y << std::endl;
			}
			_lastJunctionTaken = make_sharedJunction( x, y, possibleTurns[_possibleTurnIdx[numOfPossibleTurns - 1]], _lastJunctionTaken );
			LOG(DEBUG) << "<*>Last junction seen was on " << x << "," << y << " - taken " << _lastJunctionTaken->direction << std::endl;
			return possibleTurns[_possibleTurnIdx[numOfPossibleTurns - 1]];
		}
		else 
		{
			if (_turnsToBeTaken.top().x != x || _turnsToBeTaken.top().y != y)
			{
				LOG(ERROR) << "<<!>*<<!>>>Wanted to go back to " << _turnsToBeTaken.top().x << "," << _turnsToBeTaken.top().y <<
					" but wound up in " << x << "," << y;
			}
			else
			{
				LOG(DEBUG) << "Successfuly rolled back to take " << _turnsToBeTaken.top().direction << " in "
					<< _turnsToBeTaken.top().x << "," << _turnsToBeTaken.top().y;
			}
			auto turnToTake = _turnsToBeTaken.top().direction;
			_lastJunctionTaken = make_sharedJunction(_turnsToBeTaken.top());
			LOG(DEBUG) << "<*>Last junction seen was on " << x << "," << y << " - taken " << _lastJunctionTaken->direction << std::endl;
			_turnsToBeTaken.pop();
			_currentWalkState = AIWalkState::Explore;
			return turnToTake;
		}
		LOG(ERROR) << "Could not determine next move!";
		return maze::Direction::NotSet; //this should never happen...
	}

	std::shared_ptr<CMazePlayerAIController::SJunction> CMazePlayerAIController::make_sharedJunction(int x, int y, maze::Direction direction, std::shared_ptr<SJunction> in_parent)
	{
		return std::shared_ptr<SJunction>(new SJunction{ x, y, direction, in_parent }, 
			std::bind(&amazeinggame::CMazePlayerAIController::SJunctionDeleter, this, std::placeholders::_1));
	}
	std::shared_ptr<CMazePlayerAIController::SJunction> CMazePlayerAIController::make_sharedJunction(const SJunction &in_other)
	{
		return std::shared_ptr<SJunction>(new SJunction(in_other), 
			std::bind(&amazeinggame::CMazePlayerAIController::SJunctionDeleter, this, std::placeholders::_1));
	}

	//if _turnsToBeTaken and _lastJunctionTaken no longer point to a junction it means it lead to a dead end, and should
	//be put in the black list.
	void CMazePlayerAIController::SJunctionDeleter(SJunction * junctionToDelete)
	{
		junctionToDelete->parent = nullptr; 
		_turnsAlreadyTaken.emplace(*junctionToDelete); 
		LOG(DEBUG) << "adding junction " << junctionToDelete->x << "," << junctionToDelete->y 
			<< "-" << junctionToDelete->direction << " to black list" << std::endl; 
		delete junctionToDelete;
	}
}
