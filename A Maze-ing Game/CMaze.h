#pragma once

#include <forward_list>
#include <vector>
#include <stack>
#include <memory>

//This file holds the CMaze class, the CMaze class is responsible to generate a minimal spanning tree for a 2d graph of
//a run-time determined size.
//It can also return all equidistant points from a given point in the maze, as well as the available directions from 
//a given point in the maze.


namespace maze
{
	

	struct ConnectedWallSection
	{
		ConnectedWallSection(unsigned int in_x1, unsigned int in_y1, unsigned int in_x2, unsigned int in_y2);
		unsigned int x1, x2, y1, y2;
	};
	typedef std::forward_list<ConnectedWallSection> listOfWalls_t;

	//north is positive y, south is negative y, west is negative x, east is positive x
	enum class Direction { North, South, East, West, NotSet};

	std::ostream & operator << (std::ostream &, Direction);

	std::pair<int, int> getVectorFromDirection(Direction direction);
	Direction getDirectionFromVector(float x, float y);

	//the main class

	class CMaze
	{
	public:
		CMaze(unsigned int in_width, unsigned int in_length);
		CMaze() = default;
		void generateMaze(unsigned int in_width, unsigned int in_height);
		//get list of the maze walls, continous sections of cell walls are merged into one wall "line"
		listOfWalls_t getMazeWalls() const;
		//returns all valid directions for a given position.
		std::vector<Direction> getAllPossibleDirectionsFromPosition(unsigned int x, unsigned int y) const;
		//return all equidistant coordinated (in L1) from a given position
		std::vector<std::pair<int, int>> getAllEquidistantPositionsFromPosition(unsigned int x, unsigned int y, unsigned int distance) const;
		//check if a direction is valid from this position (i.e. if it is not bounded by a wall)
		bool isDirectionAllowedFromPosition(unsigned int x, unsigned int y, Direction direction) const;
	private:
		struct Cell;
		struct Wall;
		//for debuging purposes, draw the maze in textual form
		void drawMaze() const;
		//also for debuging, report possible exits from a given position
		void showPossibleExits(unsigned int x, unsigned int y);
		//when removing a wall between to cells while creating the maze we unite the two cell's groups.
		void JoinCellGroups(Cell &in_oneCell, Cell &in_otherCell);
		//return the cell position in the _mazeCells vector according to it's position
		size_t getCellIdxFromPosition(unsigned int x, unsigned int y) const;
		
		unsigned int _width, _length; //number of cells in the x and y axis respectively
		std::vector<Cell> _mazeCells;
		std::vector<Wall> _mazeWalls;
		struct Cell
		{
			Cell(unsigned int x, unsigned int y, size_t in_groupId);
			Cell() = default;
			unsigned int x, y;
			size_t cellGroupId; //the group to which this cell belongs to

			//the list of cell in same group is used only during construction for code simplification 
			//it would be cleared after the initialization is complete.
			//using a forward list saves space over a list and run time over a vector, but we lose size()
			//this is why a pair is used - the first element encodes the size of the list.
			//could have used boost's slist, but it seemed an overkill to include boost just for that...
			std::shared_ptr<std::pair<size_t,std::forward_list<size_t>>> ptrOfListOfCellInSameGroup;
			//determine the connection between cells - negative indices indicate the inexistance of an edge.
			//north is "up" (y is larger), east is "right" (x is larger) 
			long long northConnectsToCellIdx = -1, southConnectsToCellIdx = -1
				, eastConnectsToCellIdx = -1, westConnectsToCellIdx = -1; 
		};
		struct Wall
		{
			Wall(size_t in_parentCellIdx, size_t in_neighbourCellIdx);
			Wall() = default;
			size_t parentCellIdx, neighbourCellIdx;
			bool isWallPresent;
		};
	};
	
}