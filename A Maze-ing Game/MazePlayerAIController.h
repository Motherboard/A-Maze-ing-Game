#pragma once
#include "MazePlayerControllerInterface.h"
#include "CMaze.h"
#include <stack>
#include <set>
#include <random>
#include <tuple>
#include <array>
#include <memory>


namespace amazeinggame
{

	class CMazePlayerModel;
	class CMazeGameWorldModel;
	//we all know the AI could beat the player hands down
	//if I so chose (using a non-greedy algorithm like dijkstra) - but that would make the game less fun to play...
	//So this is a dumbed down AI, to give the human player a sense of value.
	class CMazePlayerAIController :
		public CMazePlayerControllerInterface
	{
	public:
		CMazePlayerAIController(const CMazeGameWorldModel & in_worldModel);
		virtual ~CMazePlayerAIController();
		void update(CMazePlayerModel * const in_playerModel) final;
	private:
		maze::Direction getNextDirection(int x, int y, maze::Direction direction);
		const CMazeGameWorldModel * _worldModel = nullptr;
		int _prevX = -1, _prevY = -1;
		maze::Direction _prevDirection = maze::Direction::NotSet;
		enum class AIWalkState { Explore, Rollback } _currentWalkState = AIWalkState::Explore;
		std::default_random_engine randomEngine;
		std::array<int, 4> _possibleTurnIdx;
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
		//std::set<SJunction> _turnsToBeTaken;
		std::set <SJunction> _turnsAlreadyTaken;
	};
}
