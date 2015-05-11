#pragma once

#include <list>
#include <vector>
#include <stack>
#include <memory>

namespace Maze
{
	

	struct ConnectedWallSection
	{
		ConnectedWallSection(int in_x1, int in_y1, int in_x2, int in_y2);
		int x1, x2, y1, y2;
	};
	typedef std::list<ConnectedWallSection> listOfWalls_t;

	
	class CMaze
	{
	public:
		CMaze(int in_width, int in_length);
		void regenerateMaze(int in_width, int in_height);
		listOfWalls_t getMazeWalls();
	private:
		struct Cell;
		struct Wall;

		void drawMaze();
		void JoinCellGroups(Cell &in_oneCell, Cell &in_otherCell);
		size_t getCellIdxFromPosition(int x, int y);
		
		int _width, _length; //number of cells in the x and y axis respectively
		std::vector<Cell> _mazeCells;
		std::vector<Wall> _mazeWalls;

		struct Cell
		{
			Cell(int x, int y, int in_groupId);
			Cell() = default;
			int x, y;
			int cellGroupId; //the group to which this cell belongs to
			std::shared_ptr<std::list<size_t>> ptrOfListOfCellInSameGroup;
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