#include "CMaze.h"
#include <random>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <stack>

namespace maze
{

	std::pair<int, int> getVectorFromDirection(maze::Direction direction)
	{
		switch (direction)
		{
		case Direction::North: return std::make_pair<int, int>(0, 1);
		case Direction::South: return std::make_pair<int, int>(0, -1);
		case Direction::East: return std::make_pair<int, int>(1, 0);
		case Direction::West: return std::make_pair<int, int>(-1, 0);
		}
	}

	maze::Direction getDirectionFromVector(float x, float y)
	{
		//first determin the dominant direction:
		if (abs(y) > abs(x))
		{ //this is either north or south
			if (y > 0)
				return Direction::North;
			else
				return Direction::South;
		}
		else //either east or west
		{
			if (x > 0)
				return Direction::East;
			else
				return Direction::West;
		}
	}

	CMaze::Wall::Wall(size_t in_parentCellIdx, size_t in_neighbourCellIdx)
	{

		parentCellIdx = in_parentCellIdx;
		neighbourCellIdx = in_neighbourCellIdx;
		isWallPresent = true;
	}

	CMaze::Cell::Cell(int in_x, int in_y, size_t in_groupId) : x(in_x), y(in_y), cellGroupId(in_groupId)
	{
		//std::make_pair <size_t, std::forward_list<size_t> >(
		ptrOfListOfCellInSameGroup = std::make_shared<std::pair<size_t, std::forward_list<size_t>>>(1, std::forward_list < size_t > { cellGroupId });
	}

	CMaze::CMaze(int in_width, int in_length)
	{
		generateMaze(in_width, in_length);
	}


	//Create a minimal spanning tree in random order to produce a maze
	void CMaze::generateMaze(int in_width, int in_length)
	{
		using namespace std;
		_width = in_width;
		_length = in_length;
		_mazeCells.resize(_width*_length);
		_mazeWalls.resize(_mazeCells.size() * 2 - _length - _width);
		int wallIdx = 0;
		//initializing all cells with unique id, and add horizontal walls.
		for (int y = 0, cellIdx = 0; y < _length; ++y)
		{
			for (int x = 0; x < _width; ++x, ++cellIdx)
			{
				_mazeCells[cellIdx] = Cell(x, y, cellIdx);
				if (y > 0) //add horizontal walls
					_mazeWalls[wallIdx++] = Wall(cellIdx, cellIdx - _width);
			}
		}
		//now add vertical walls
		for (int x = 1; x < _width; ++x)
		{
			for (int y = 0; y < _length; ++y, ++wallIdx)
			{
				_mazeWalls[wallIdx] = Wall(x + y*_width, x - 1 + y*_width);
			}
		}
		vector<size_t> shuffledWallIndices(_mazeWalls.size());
		//fill in the vector with increasing numbers starting with 0.
		iota(begin(shuffledWallIndices), end(shuffledWallIndices), 0);
		unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
		shuffle(begin(shuffledWallIndices), end(shuffledWallIndices), default_random_engine(seed));
		for (size_t i : shuffledWallIndices)
		{
			Cell & parentCell = _mazeCells[_mazeWalls[i].parentCellIdx];
			Cell & neighbourCell = _mazeCells[_mazeWalls[i].neighbourCellIdx];
			//if the wall is between different cell groups, it can be safely removed to unite the groups.
			if (parentCell.cellGroupId != neighbourCell.cellGroupId)
			{
				_mazeWalls[i].isWallPresent = false;
				JoinCellGroups(parentCell, neighbourCell);
				//drawMaze(); //<- uncomment to see the building procedure in action.
			}
		}
		//no need to keep the list of cells in same group - as they are all in the same group now
		//reset the list
		_mazeCells.front().ptrOfListOfCellInSameGroup->second.clear(); //we could also iterate over all cells and set the pointer to nullptr, but we don't care about all the pointers keeping an empty list alive...
		_mazeCells.front().ptrOfListOfCellInSameGroup->first = 0;
	}

	maze::listOfWalls_t CMaze::getMazeWalls() const
	{
		listOfWalls_t tempWallsList;
		bool isInMiddleOfWall = false;
		int wallIdx = 0;
		//get all the horizontal wall sections:
		for (int y = 1; y < _length; ++y)
		{
			int startIdxOfWall;
			for (int x = 0; x < _width; ++x, ++wallIdx)
			{
				if (!isInMiddleOfWall && _mazeWalls[wallIdx].isWallPresent)
				{
					isInMiddleOfWall = true;
					startIdxOfWall = wallIdx;
				}
				if (isInMiddleOfWall && (!(_mazeWalls[wallIdx].isWallPresent) || (x == _width - 1)))
				{
					Cell cellOfWallBeginning = _mazeCells[_mazeWalls[startIdxOfWall].parentCellIdx];
					Cell cellOfWallEnd = _mazeCells[_mazeWalls[wallIdx].parentCellIdx];
					tempWallsList.push_front(
						ConnectedWallSection(cellOfWallBeginning.x, cellOfWallBeginning.y,
						cellOfWallEnd.x + ((_mazeWalls[wallIdx].isWallPresent && x == _width - 1) ? 1 : 0),
						cellOfWallEnd.y)); //if the wall was ended because it's the maze edge, extend it to reach the maze's edge.
					isInMiddleOfWall = false;
				}

			}
		}

		//get all the vertical wall sections:
		for (int x = 1; x < _width; ++x)
		{
			int startIdxOfWall;
			for (int y = 0; y < _length; ++y, ++wallIdx)
			{
				if (!isInMiddleOfWall && _mazeWalls[wallIdx].isWallPresent)
				{
					isInMiddleOfWall = true;
					startIdxOfWall = wallIdx;
				}
				if (isInMiddleOfWall && (!_mazeWalls[wallIdx].isWallPresent || y == _length - 1)) //this will always happen at the rightmost edge of the maze, since the horizontal walls there are not present
				{
					Cell cellOfWallBeginning = _mazeCells[_mazeWalls[startIdxOfWall].parentCellIdx];
					Cell cellOfWallEnd = _mazeCells[_mazeWalls[wallIdx].parentCellIdx];
					//same as for horizontal lines - if the line was finished by the maze's edge extend it to meet the edge.
					tempWallsList.push_front(
						ConnectedWallSection(cellOfWallBeginning.x, cellOfWallBeginning.y,
						cellOfWallEnd.x, cellOfWallEnd.y + ((_mazeWalls[wallIdx].isWallPresent && y == _length - 1) ? 1 : 0)));

					isInMiddleOfWall = false;
				}
			}
		}
		return tempWallsList; //you gotta love c++11's move :)
	}

	void CMaze::JoinCellGroups(CMaze::Cell &in_oneCell, CMaze::Cell &in_otherCell)
	{
		//use pointers, so that swapping them will not actually swap the cells (just the pointers...)
		Cell *oneCellPtr = &in_oneCell;
		Cell *otherCellPtr = &in_otherCell;
		if (in_otherCell.ptrOfListOfCellInSameGroup->first > in_oneCell.ptrOfListOfCellInSameGroup->first)
			std::swap(oneCellPtr, otherCellPtr); //we want to walk over the smaller list.
		auto otherCellIdx = getCellIdxFromPosition(otherCellPtr->x, otherCellPtr->y);
		auto oneCellIdx = getCellIdxFromPosition(oneCellPtr->x, oneCellPtr->y);
		int dx = otherCellPtr->x - oneCellPtr->x;
		if (dx < 0)
		{ //other cell is to the west
			oneCellPtr->westConnectsToCellIdx = otherCellIdx;
			otherCellPtr->eastConnectsToCellIdx = oneCellIdx;
		}
		else if (dx > 0)
		{ //other cell is to the east
			oneCellPtr->eastConnectsToCellIdx = otherCellIdx;
			otherCellPtr->westConnectsToCellIdx = oneCellIdx;
		}
		else
		{
			int dy = otherCellPtr->y - oneCellPtr->y;
			if (dy < 0)
			{ //other cell is to the south
				oneCellPtr->southConnectsToCellIdx = otherCellIdx;
				otherCellPtr->northConnectsToCellIdx = oneCellIdx;
			}
			else
			{ //other cell is to the north
				oneCellPtr->northConnectsToCellIdx = otherCellIdx;
				otherCellPtr->southConnectsToCellIdx = oneCellIdx;
			}
		}
		//since during the following iteration otherCellPtr->ptrOfListOfCellInSameGroup would be re-assigned
		//keep a copy of the original list temporarily. At the end of this function, this list would get automatically 
		//deallocated, since it's last shared pointer would go out of scope.
		auto tempListOfCellsToMove = otherCellPtr->ptrOfListOfCellInSameGroup->second;
		for (auto cellIdx : tempListOfCellsToMove)
		{
			_mazeCells[cellIdx].cellGroupId = oneCellPtr->cellGroupId;
			_mazeCells[cellIdx].ptrOfListOfCellInSameGroup = oneCellPtr->ptrOfListOfCellInSameGroup;
			oneCellPtr->ptrOfListOfCellInSameGroup->second.push_front(cellIdx);
			oneCellPtr->ptrOfListOfCellInSameGroup->first++;
		}
		otherCellPtr->ptrOfListOfCellInSameGroup = oneCellPtr->ptrOfListOfCellInSameGroup;
	}

	inline size_t CMaze::getCellIdxFromPosition(int x, int y) const
	{
		return x + y*_width;
	}

	maze::ConnectedWallSection::ConnectedWallSection(int in_x1, int in_y1, int in_x2, int in_y2) :
		x1(in_x1), y1(in_y1), x2(in_x2), y2(in_y2) {}

	std::vector<maze::Direction> CMaze::getAllPossibleDirectionsFromPosition(int x, int y) const
	{
		std::vector<maze::Direction> tempAllowedDirections;
		if (x >= _width || x < 0 || y >= _length || y < 0)
			return tempAllowedDirections;
		auto cellIdx = getCellIdxFromPosition(x, y);
		if (_mazeCells[cellIdx].eastConnectsToCellIdx >= 0)
			tempAllowedDirections.push_back(Direction::East);
		if (_mazeCells[cellIdx].westConnectsToCellIdx >= 0)
			tempAllowedDirections.push_back(Direction::West);
		if (_mazeCells[cellIdx].northConnectsToCellIdx >= 0)
			tempAllowedDirections.push_back(Direction::North);
		if (_mazeCells[cellIdx].southConnectsToCellIdx >= 0)
			tempAllowedDirections.push_back(Direction::South);
		return tempAllowedDirections;
	}

	namespace {
		struct cellTravelStruct
		{
			cellTravelStruct(long long in_cellIdx, long long in_prevCellIdx, unsigned int in_distance)
				: cellIdx(in_cellIdx), prevCellIdx(in_prevCellIdx), distance(in_distance) {}
			long long cellIdx, prevCellIdx;
			unsigned int distance;
		};
	}

	std::vector<std::pair<int, int>> CMaze::getAllEquidistantPositionsFromPosition(int x, int y, unsigned int distance) const
	{
		std::vector<std::pair<int, int>> tempLocationVec;
		if (x >= _width || x < 0 || y >= _length || y < 0)
			return tempLocationVec;
		auto cellIdx = getCellIdxFromPosition(x, y);
		decltype(cellIdx) prevCellIdx = -1;
		std::stack <cellTravelStruct> unvisitedCellsInGivenDistance;
		unsigned int currentDistance = 0;
		unvisitedCellsInGivenDistance.emplace(cellIdx, prevCellIdx, currentDistance);
		do
		{
			cellIdx = unvisitedCellsInGivenDistance.top().cellIdx;
			currentDistance = unvisitedCellsInGivenDistance.top().distance;
			prevCellIdx = unvisitedCellsInGivenDistance.top().prevCellIdx;
			unvisitedCellsInGivenDistance.pop();
			if (currentDistance == distance)
				tempLocationVec.emplace_back(_mazeCells[cellIdx].x, _mazeCells[cellIdx].y);
			else //only add new cells to visit if currentDistance is smaller than required distance...
			{
				if (_mazeCells[cellIdx].eastConnectsToCellIdx >= 0 && _mazeCells[cellIdx].eastConnectsToCellIdx != prevCellIdx)
					unvisitedCellsInGivenDistance.emplace(_mazeCells[cellIdx].eastConnectsToCellIdx, cellIdx ,currentDistance + 1);
				if (_mazeCells[cellIdx].westConnectsToCellIdx >= 0 && _mazeCells[cellIdx].westConnectsToCellIdx != prevCellIdx)
					unvisitedCellsInGivenDistance.emplace(_mazeCells[cellIdx].westConnectsToCellIdx, cellIdx ,currentDistance + 1);
				if (_mazeCells[cellIdx].northConnectsToCellIdx >= 0 && _mazeCells[cellIdx].northConnectsToCellIdx != prevCellIdx)
					unvisitedCellsInGivenDistance.emplace(_mazeCells[cellIdx].northConnectsToCellIdx, cellIdx ,currentDistance + 1);
				if (_mazeCells[cellIdx].southConnectsToCellIdx >= 0 && _mazeCells[cellIdx].southConnectsToCellIdx != prevCellIdx)
					unvisitedCellsInGivenDistance.emplace(_mazeCells[cellIdx].southConnectsToCellIdx, cellIdx ,currentDistance + 1);
			}
		} while (!unvisitedCellsInGivenDistance.empty());
		return tempLocationVec;
	}

	bool CMaze::isDirectionAllowedFromPosition(int x, int y, maze::Direction direction) const
	{
		if (x >= _width || x < 0 || y >= _length || y < 0)
			return false;
		auto cellIdx = getCellIdxFromPosition(x, y);
		switch (direction)
		{
		case Direction::East: return _mazeCells[cellIdx].eastConnectsToCellIdx >= 0;
		case Direction::West: return _mazeCells[cellIdx].westConnectsToCellIdx >= 0;
		case Direction::North: return _mazeCells[cellIdx].northConnectsToCellIdx >= 0;
		case Direction::South: return _mazeCells[cellIdx].southConnectsToCellIdx >= 0;
		}
	}

	void CMaze::showPossibleExits(int x, int y)
	{
		std::cout << "Check collision for: " << x << "," << y << " - ";
		auto cellIdx = getCellIdxFromPosition(x, y);
		std::cout << "cell " << cellIdx << " has the following exits: " << std::endl;
		if (_mazeCells[cellIdx].eastConnectsToCellIdx >= 0) std::cout << "East,"; else std::cout << "-----,";
		if (_mazeCells[cellIdx].westConnectsToCellIdx >= 0) std::cout << "West,"; else std::cout << "-----";
		if (_mazeCells[cellIdx].northConnectsToCellIdx >= 0) std::cout << "North,"; else std::cout << "-----";
		if (_mazeCells[cellIdx].southConnectsToCellIdx >= 0) std::cout << "South,"; else std::cout << "-----" << std::endl;
	}

	void CMaze::drawMaze() const
	{
		std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@[Begin Step]@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
		int verticalWallIdx = _length*(_width - 1);
		for (int y = 0; y < _length; ++y)
		{
			//draw horizontal lines.
			if (y > 0)
			{
				for (int x = 0; x < _width; ++x)
				{

					if (_mazeWalls[x + (y - 1)*_width].isWallPresent)
						std::cout << std::setw(7) << "_______";
					else
						std::cout << std::setw(7) << "       ";
				}
			}
			std::cout << std::endl;
			//write x,y of upper cell
			for (int x = 0; x < _width; ++x)
			{
				if (x > 0)
					if (_mazeWalls[verticalWallIdx + y + (x - 1)*_length].isWallPresent)
						std::cout << std::setw(1) << "|";
					else
						std::cout << std::setw(1) << " ";
				if (_mazeCells[getCellIdxFromPosition(x, y)].southConnectsToCellIdx > 0)
					std::cout << std::setw(3) << _mazeCells[_mazeCells[getCellIdxFromPosition(x, y)].southConnectsToCellIdx].x <<
					std::setw(1) << "," << std::setw(2) << _mazeCells[_mazeCells[getCellIdxFromPosition(x, y)].southConnectsToCellIdx].y;
				else
				{
					std::cout << std::setw(6) << "      ";
				}
			}
			std::cout << std::endl;
			//write cell id, and < or\and > if we can go left or right
			for (int x = 0; x < _width; ++x)
			{
				if (x > 0)
					if (_mazeWalls[verticalWallIdx + y + (x - 1)*_length].isWallPresent)
						std::cout << std::setw(1) << "|";
					else
						std::cout << std::setw(1) << " ";
				if (_mazeCells[getCellIdxFromPosition(x, y)].westConnectsToCellIdx >= 0)
					std::cout << std::setw(1) << "<";
				else
					std::cout << std::setw(1) << " ";
				std::cout << std::setw(4) << _mazeCells[getCellIdxFromPosition(x, y)].cellGroupId;
				if (_mazeCells[getCellIdxFromPosition(x, y)].eastConnectsToCellIdx >= 0)
					std::cout << std::setw(1) << ">";
				else
					std::cout << std::setw(1) << " ";
			}
			std::cout << std::endl;
			//write coordinates for going north (down)
			for (int x = 0; x < _width; ++x)
			{
				if (x > 0)
					if (_mazeWalls[verticalWallIdx + y + (x - 1)*_length].isWallPresent)
						std::cout << std::setw(1) << "|";
					else
						std::cout << std::setw(1) << " ";
				if (_mazeCells[getCellIdxFromPosition(x, y)].northConnectsToCellIdx >= 0)
					std::cout << std::setw(3) << _mazeCells[_mazeCells[getCellIdxFromPosition(x, y)].northConnectsToCellIdx].x <<
					std::setw(1) << "," << std::setw(2) << _mazeCells[_mazeCells[getCellIdxFromPosition(x, y)].northConnectsToCellIdx].y;
				else
				{
					std::cout << std::setw(6) << "      ";
				}
			}
			std::cout << std::endl;
		}
		std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@[End Step]@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
	}
}