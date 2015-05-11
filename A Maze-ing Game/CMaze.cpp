#include "CMaze.h"
#include <random>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iostream>
#include <iomanip>



Maze::CMaze::Wall::Wall(size_t in_parentCellIdx, size_t in_neighbourCellIdx)
{

	parentCellIdx = in_parentCellIdx;
	neighbourCellIdx = in_neighbourCellIdx;
	isWallPresent = true;
}

Maze::CMaze::Cell::Cell(int in_x, int in_y, int in_groupId) : x(in_x), y(in_y), cellGroupId(in_groupId) 
{
	ptrOfListOfCellInSameGroup = std::make_shared<std::list<size_t>>(1,cellGroupId);
}

Maze::CMaze::CMaze(int in_width, int in_length)
{
	regenerateMaze(in_width, in_length);
}


//Create a minimal spanning tree in random order to produce a maze
void Maze::CMaze::regenerateMaze(int in_width, int in_length)
{
	using namespace std;
	_width = in_width;
	_length = in_length;
	_mazeCells.resize(_width*_length);
	_mazeWalls.resize(_mazeCells.size()*2-_length-_width);
	int wallIdx = 0;
	//initializing all cells with unique id, and add horizontal walls.
	for (int y = 0, cellIdx = 0; y < _length; ++y)
	{
		for (int x = 0; x < _width; ++x, ++cellIdx)
		{
			_mazeCells[cellIdx] = Cell(x,y,cellIdx);
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
			//drawMaze(); <- uncomment to see the building procedure in action.
		}
	}
}

Maze::listOfWalls_t Maze::CMaze::getMazeWalls()
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
				tempWallsList.push_back(
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
			if (isInMiddleOfWall && (!_mazeWalls[wallIdx].isWallPresent || y == _length-1)) //this will always happen at the rightmost edge of the maze, since the horizontal walls there are not present
			{
				Cell cellOfWallBeginning = _mazeCells[_mazeWalls[startIdxOfWall].parentCellIdx];
				Cell cellOfWallEnd = _mazeCells[_mazeWalls[wallIdx].parentCellIdx];
				//same as for horizontal lines - if the line was finished by the maze's edge extend it to meet the edge.
				tempWallsList.push_back(
					ConnectedWallSection(cellOfWallBeginning.x, cellOfWallBeginning.y,
					cellOfWallEnd.x, cellOfWallEnd.y + ((_mazeWalls[wallIdx].isWallPresent && y == _length - 1) ? 1 : 0)));
				
				isInMiddleOfWall = false;
			}
		}
	}
	return tempWallsList; //you gotta love c++11's move :)
}

void Maze::CMaze::JoinCellGroups(Maze::CMaze::Cell &in_oneCell, Maze::CMaze::Cell &in_otherCell)
{
	Cell *oneCellPtr = &in_oneCell;
	Cell *otherCellPtr = &in_otherCell;
	if (in_otherCell.ptrOfListOfCellInSameGroup->size() > in_oneCell.ptrOfListOfCellInSameGroup->size())
		std::swap(oneCellPtr, otherCellPtr); //we want to walk over the smaller list.
	auto otherCellIdx = getCellIdxFromPosition(otherCellPtr->x, otherCellPtr->y);
	auto oneCellIdx = getCellIdxFromPosition(oneCellPtr->x, oneCellPtr->y);
	auto tempListOfCellsToMove = otherCellPtr->ptrOfListOfCellInSameGroup;
	for (auto cellIdx : *tempListOfCellsToMove)
	{
		_mazeCells[cellIdx].cellGroupId = oneCellPtr->cellGroupId;
		_mazeCells[cellIdx].ptrOfListOfCellInSameGroup = oneCellPtr->ptrOfListOfCellInSameGroup;
		oneCellPtr->ptrOfListOfCellInSameGroup->push_back(cellIdx);
	}
	otherCellPtr->ptrOfListOfCellInSameGroup = oneCellPtr->ptrOfListOfCellInSameGroup;
}

inline size_t Maze::CMaze::getCellIdxFromPosition(int x, int y)
{
	return x + y*_width;
}

Maze::ConnectedWallSection::ConnectedWallSection(int in_x1, int in_y1, int in_x2, int in_y2) :
x1(in_x1), y1(in_y1), x2(in_x2), y2(in_y2) {}

void Maze::CMaze::drawMaze()
{
	int verticalWallIdx = _length*(_width - 1);
	for (int y = 0; y < _length; ++y)
	{
		//draw horizontal lines.
		if (y > 0)
		{
			for (int x = 0; x < _width; ++x)
			{
				
				if (_mazeWalls[x + (y - 1)*_width].isWallPresent)
					std::cout << std::setw(3) << "___";
				else
					std::cout << std::setw(3) << "   ";
			}
		}
		std::cout << std::endl;
		for (int x = 0; x < _width; ++x)
		{
			if (x > 0)
				if (_mazeWalls[verticalWallIdx + y + (x-1)*_length].isWallPresent)
					std::cout << std::setw(1) << "|";
				else
					std::cout << std::setw(1) << " ";
			std::cout << std::setw(2) << _mazeCells[getCellIdxFromPosition(x, y)].cellGroupId;
		}
		std::cout << std::endl;
	}	
}