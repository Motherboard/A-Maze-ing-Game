#pragma once
#include "MazePlayerControllerInterface.h"
#include "CMaze.h"
#include <stack>
#include <set>
#include <random>
#include <tuple>
#include <array>
#include <memory>

//we all know the AI could beat the player hands down
//if I so chose (using a non-greedy algorithm like dijkstra) - but that would make the game less fun to play...
//So this is a dumbed down AI, to give the human player a sense of value.



namespace amazeinggame
{

	class CMazePlayerModel;
	class CMazeGameWorldModel;


	class CMazePlayerAIController :
		public CMazePlayerControllerInterface
	{
	public:
		CMazePlayerAIController() = delete;
		CMazePlayerAIController(const CMazeGameWorldModel & in_worldModel, unsigned char in_AILevel = 3);
		virtual ~CMazePlayerAIController();
		//This function is called by the player and it, in it's turn call the player's set direction function.
		void update(CMazePlayerModel * const in_playerModel) final;
		//determines how far ahead the AI can look from where it is now. 
		//The default is 3
		void setAILevel(int in_level);
	private:
		//determines what new direction the player should take.
		maze::Direction getNextDirection(int x, int y, maze::Direction direction);
		//return true if the finish point is in sight - this would mean that the out_preferedDirectionIdx would point to
		//the turn that leads to the finish point, otherwise out_preferedDirectionIdx would either be -1 (no preference) 
		//or would be the index of the farward direction (since it's a bit faster to continue on the same direction).
		//The out_possibleTurns would contain all possible directions to take, excluding the backward direction
		//It can only be full at the beginning of the game where no backward direction exists.
		//The out_numOfPossibleTurns tells us up to where the out_possibleTurns should be considered
		//We use array and size since we don't want to create a vector each call just for 1-4 possible values.
		//The out_backDirectoin returns the backward direction in case we need to turn back
		//e.g. when out_numOfPossibleTurns is 0.
		//x and y are the current positions of the player, in_direction is the current direction, and the depth determines
		//how far ahead the AI should look.
		bool getPossibleTurns(std::array<maze::Direction, 4> &out_possibleTurns, unsigned int & out_numOfPossibleTurns, 
			int & out_preferedDirectionIdx, maze::Direction & out_backDirectoin ,
			int x, int y, maze::Direction in_direction, int depth);
		const CMazeGameWorldModel * _worldModel = nullptr;
		int _prevX = -1, _prevY = -1;
		maze::Direction _prevDirection = maze::Direction::NotSet;
		enum class AIWalkState { Explore, Rollback } _currentWalkState = AIWalkState::Explore;
		int _lookAheadDepth; //how far can the AI look ahead to see if a direction is a viable one.
		std::default_random_engine randomEngine;
		std::array<int, 4> _possibleTurnIdx;
		std::pair<int, int> _finishPoint;
		bool isAlreadyWalking = false;
		struct SJunction
		{
			SJunction(int in_x, int in_y, maze::Direction in_direction, std::shared_ptr<SJunction> in_parent = nullptr)
				:x(in_x), y(in_y), direction(in_direction), parent(in_parent)  {}
			int x, y;
			maze::Direction direction;
			std::shared_ptr<SJunction> parent;
			bool operator < (const SJunction &rhs) const {
				return std::tie(x, y, direction) < std::tie(rhs.x, rhs.y, rhs.direction);
			}
		};
		
		std::shared_ptr<SJunction> make_sharedJunction(int x, int y, maze::Direction direction, std::shared_ptr<SJunction> in_parent);
		std::shared_ptr<SJunction> make_sharedJunction(const SJunction &in_other);
		std::shared_ptr<SJunction> _lastJunctionTaken;
		std::stack<SJunction> _turnsToBeTaken;
		std::set <SJunction> _turnsAlreadyTaken;
		void SJunctionDeleter(SJunction *);
	};
}
