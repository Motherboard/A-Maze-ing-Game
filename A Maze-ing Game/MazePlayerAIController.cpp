#include "MazePlayerAIController.h"
#include "MazePlayerModel.h"
#include "MazeGameWorldModel.h"
#include "mathUtils.h"
#include <numeric>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <functional>


namespace amazeinggame
{

	
	CMazePlayerAIController::CMazePlayerAIController(const CMazeGameWorldModel & in_worldModel)
	{
		_worldModel = &in_worldModel;
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
			return; //init was not called, can't do anything...
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
				return;
		}
		if (maze::getDirectionFromVector(playerState.speedX, playerState.speedY) != _prevDirection && (playerState.speedX != 0 || playerState.speedY != 0))
		{
			std::cerr << playerState.x << "," << playerState.y << ": Current walking direction is " 
				<< maze::getDirectionFromVector(playerState.speedX, playerState.speedY)
				<< " But AI thinks it\'s " << _prevDirection << std::endl;
			_prevDirection = maze::getDirectionFromVector(playerState.speedX, playerState.speedY);
		}
		_prevX = x; _prevY = y;
		auto nextDir = getNextDirection(x, y, _prevDirection);
		if (nextDir == maze::Direction::NotSet)
		{ //means theres a bug somewhere...
			std::cerr << "Could not figure out where to go from " << x << "," << y << std::endl;
			return;
		}
		if (nextDir != _prevDirection)
		{
			in_playerModel->stop();
			in_playerModel->setDirection(nextDir);
			std::cerr << "Go " << nextDir << " on " << x << "," << y << std::endl;
			_prevDirection = nextDir;
		}
		else 
			std::cerr << "Continue " << _prevDirection << " on " << x << "," << y << std::endl;
	}

	maze::Direction CMazePlayerAIController::getNextDirection(int x, int y, maze::Direction in_direction)
	{
		//check if the new room has a junction in it
		auto possibleDirections = _worldModel->getMaze().getAllPossibleDirectionsFromPosition(x, y);
		auto prevDirectionVector = maze::getVectorFromDirection(in_direction);
		/*bool continueOnSameDirection = false;
		for (const auto & direction: possibleDirections)
			if (in_direction == direction)
				{continueOnSameDirection = true;break;}
		int nextX = x, nextY = y;
		if (continueOnSameDirection)
		{
			nextX = x + prevDirectionVector.first; 
			nextY = y + prevDirectionVector.second;
			possibleDirections = _worldModel->getMaze().getAllPossibleDirectionsFromPosition(nextX, nextY);
		}*/
		
		auto backDirection = maze::getDirectionFromVector(-prevDirectionVector.first, -prevDirectionVector.second);
		int numOfPossibleTurns = 0;
		maze::Direction possibleTurns[4];
		int sameDirectionIdx = -1; //if we can continue in the same directio, this would be preferable - keep this for later use. 
		for (const auto & direction : possibleDirections)
		{
			if (direction != backDirection && _turnsAlreadyTaken.find(SJunction{x,y,direction}) == std::end(_turnsAlreadyTaken))
			{
				auto directionVector = maze::getVectorFromDirection(direction);
				auto nextPossibleDirections = _worldModel->getMaze().getAllPossibleDirectionsFromPosition(x + directionVector.first, y + directionVector.second);
				if (nextPossibleDirections.size() > 1)
				{	//this is a possible direction to take.
					if (direction == in_direction)
						sameDirectionIdx = numOfPossibleTurns;
					possibleTurns[numOfPossibleTurns++] = direction;
				}
				else
				{
					auto finishPoint = _worldModel->getFinishPoint();
					if (x + directionVector.first == finishPoint.first && y + directionVector.second == finishPoint.second)
						return direction; //we win!!!
				}
					
			}
			if (_turnsAlreadyTaken.find(SJunction{ x, y, direction }) != std::end(_turnsAlreadyTaken))
				std::cerr << "<*>Ignore possible turn to " << direction << " on " << x << "," << y << std::endl;
		}
		if (sameDirectionIdx >= 0)
		{
			if (numOfPossibleTurns == 1) //no need to do anything yet, we're doing fine!
				return in_direction;
			if (_currentWalkState == AIWalkState::Explore)
			{ //only during exploration do we care about the current direction.
				//prefer to keep the same direction.

				//insert the rest of the turns to turnsToBeTaken if they are legal
				for (int i = 0; i < numOfPossibleTurns; ++i)
				{
					if (i != sameDirectionIdx)
					{
						/*SJunction currentJuntion{ x, y, possibleTurns[i] };
						if (_turnsAlreadyTaken.find(currentJuntion) == std::end(_turnsAlreadyTaken))*/
						_turnsToBeTaken.emplace( x, y, possibleTurns[i], _lastJunctionTaken );
						std::cerr << "<*>Remember to go back to take " << possibleTurns[i] << " on " << x << "," << y << std::endl;
					}
				}
				_lastJunctionTaken = make_sharedJunction(x, y, in_direction, _lastJunctionTaken);
				std::cerr << "<*>Last junction seen was on " << x << "," << y << " - continued going " << in_direction << std::endl;
				return in_direction;
			}
		}
		//now it's time to decide where to go!
		
		if (numOfPossibleTurns == 0) //this means there were no good turns to take - turn around, and go back to the last junction.
		{ //this should not happen in rollback mode.
			if (_currentWalkState == AIWalkState::Rollback)
				std::cerr << "Got lost during rollback..." << std::endl;
			if (_turnsToBeTaken.size()) //if there is where to roll back to, rollback.
			{
				_currentWalkState = AIWalkState::Rollback;
				if (_lastJunctionTaken)
				{
					std::cerr << "<*>Add turn to " << _lastJunctionTaken->direction << " on " << _lastJunctionTaken->x << "," << _lastJunctionTaken->y << " to forbiden turns" << std::endl;
					_lastJunctionTaken = nullptr;
				}
				else
					_currentWalkState = AIWalkState::Explore; //we get here only if rollback failed.
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
			for (int i = 0; i < numOfPossibleTurns - 1; ++i) //first check in the waiting list
			{
				//SJunction currentJuntion{ x, y, possibleTurns[_possibleTurnIdx[i]] };
				//if (_turnsAlreadyTaken.find(currentJuntion) == std::end(_turnsAlreadyTaken))
				_turnsToBeTaken.emplace( x, y, possibleTurns[_possibleTurnIdx[i]], _lastJunctionTaken );
				std::cerr << "<*>Remember to go back to take " << possibleTurns[_possibleTurnIdx[i]] << " on " << x << "," << y << std::endl;
			}
			_lastJunctionTaken = make_sharedJunction( x, y, possibleTurns[_possibleTurnIdx[numOfPossibleTurns - 1]], _lastJunctionTaken );
			std::cerr << "<*>Last junction seen was on " << x << "," << y << " - taken " << _lastJunctionTaken->direction << std::endl;
			return possibleTurns[_possibleTurnIdx[numOfPossibleTurns - 1]];
		}
		else
		{
			if (_turnsToBeTaken.top().x != x || _turnsToBeTaken.top().y != y)
				std::cerr << "<<!>*<<!>>>Wanted to go back to " << _turnsToBeTaken.top().x << "," << _turnsToBeTaken.top().y <<
				" but wounded up in " << x << "," << y << std::endl;
			else
				std::cerr << "Successfuly rolled back to take " << _turnsToBeTaken.top().direction << " in "
				<< _turnsToBeTaken.top().x << "," << _turnsToBeTaken.top().y << std::endl;
			auto turnToTake = _turnsToBeTaken.top().direction;
			_lastJunctionTaken = make_sharedJunction(_turnsToBeTaken.top());
			std::cerr << "<*>Last junction seen was on " << x << "," << y << " - taken " << _lastJunctionTaken->direction << std::endl;
			_turnsToBeTaken.pop();
			_currentWalkState = AIWalkState::Explore;
			return turnToTake;
		}

		return maze::Direction::NotSet; //this should never happen...
	}

	std::shared_ptr<CMazePlayerAIController::SJunction> CMazePlayerAIController::make_sharedJunction(int x, int y, maze::Direction direction, std::shared_ptr<SJunction> in_parent)
	{
		return std::shared_ptr<SJunction>(new SJunction{ x, y, direction, in_parent }, [&](SJunction *junctionToDelete){ 
			junctionToDelete->parent = nullptr; _turnsAlreadyTaken.emplace(*junctionToDelete); std::cerr << "adding junction " << junctionToDelete->x
				<< "," << junctionToDelete->y << "-" << junctionToDelete->direction << " to black list" 
				<< std::endl; delete junctionToDelete; });
	}
	std::shared_ptr<CMazePlayerAIController::SJunction> CMazePlayerAIController::make_sharedJunction(const SJunction &in_other)
	{
		return std::shared_ptr<SJunction>(new SJunction( in_other ), [&](SJunction *junctionToDelete){
			junctionToDelete->parent = nullptr; _turnsAlreadyTaken.emplace(*junctionToDelete); std::cerr << "adding junction " << junctionToDelete->x
				<< "," << junctionToDelete->y << "-" << junctionToDelete->direction << " to black list"
				<< std::endl; delete junctionToDelete; });
	}
}
