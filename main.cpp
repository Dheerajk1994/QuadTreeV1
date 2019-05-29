#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>

enum CellStatus
{
	PARENT, //FOR PARENT CELLS
	EMPTY //BASE CELL THAT IS HAS NOT POINTS OR CHILDREN CELL
};

enum Quadrants {
	NORTHWEST,
	NORTHEAST,
	SOUTHWEST,
	SOUTHEAST
};

struct Position {
	int x,y;
public:
	Position() {
		x = 0;
		y = 0;
	}
	Position(int _x, int _y) {
		x = _x;
		y = _y;
	}
};

struct Cell {
	std::vector<Position> points;
	CellStatus cellStatus = CellStatus::EMPTY;
	Position topLeft, topRight, bottomLeft, bottomRight;
	Cell* northWest = nullptr;
	Cell* northEast = nullptr;
	Cell* southWest = nullptr;
	Cell* southEast = nullptr;
public:
	Cell() {
		points.reserve(4);
	}
	Cell(std::vector<Position> _points,Position _topLeft, Position _topRight, Position _bottomLeft, Position _bottomRight, Cell* nw, Cell* ne, Cell* sw, Cell* se)
	{
		points = _points;
		topLeft = _topLeft;
		topRight = _topRight;
		bottomLeft = _bottomLeft;
		bottomRight = _bottomRight;
		northWest = nw;
		northEast = ne;
		southWest = sw;
		southEast = se;
	}
	Cell(Position _topLeft, Position _topRight, Position _bottomLeft, Position _bottomRight) {
		topLeft = _topLeft;
		topRight = _topRight;
		bottomLeft = _bottomLeft;
		bottomRight = _bottomRight;
	}
};

class MyApplication : public olc::PixelGameEngine
{
	Cell* mainCell;
public:
	MyApplication(){
		sAppName = "Quad Tree Visualization";
	}
public:
	bool OnUserCreate() override
	{
		mainCell = new Cell(
			Position(0, 0),
			Position(ScreenWidth(), 0),
			Position(0, ScreenHeight()),
			Position(ScreenWidth(), ScreenHeight())
		);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override 
	{
		ListenForMouseClicks();
		return true;
	}

	void ListenForMouseClicks() {
		if (GetMouse(0).bPressed) {
			InsertIntoTree(Position(GetMouseX(), GetMouseY()), mainCell);
			Clear(olc::BLACK);
			DrawCell(mainCell);
		}
		else if (GetMouse(1).bPressed) {
			Clear(olc::BLACK);
		}
	}
	
	void InsertIntoTree(Position newPoint, Cell* cell) {
		if (cell -> cellStatus != CellStatus::PARENT && cell->points.size() < 1){
			cell->points.emplace_back(newPoint);
		}
		else if(cell -> cellStatus != CellStatus::PARENT) {
			CreateChildrenCells(cell);
			cell->cellStatus = CellStatus::PARENT;
			//split the current 4 points into new cells
			for (Position p : cell->points) {
				if (GetQuadrant(p, cell) == Quadrants::NORTHWEST) InsertIntoTree(p, cell->northWest);
				else if (GetQuadrant(p, cell) == Quadrants::NORTHEAST) InsertIntoTree(p, cell->northEast);
				else if (GetQuadrant(p, cell) == Quadrants::SOUTHWEST) InsertIntoTree(p, cell->southWest);
				else InsertIntoTree(p, cell->southEast);
			}
			cell->points.clear();
			InsertIntoTree(newPoint, cell);
		}
		else if (cell->cellStatus == CellStatus::PARENT) {
			//add the new point to the corresponding cell
			cell->points.clear();
			if (GetQuadrant(newPoint, cell) == Quadrants::NORTHWEST) InsertIntoTree(newPoint, cell->northWest);
			else if (GetQuadrant(newPoint, cell) == Quadrants::NORTHEAST) InsertIntoTree(newPoint, cell->northEast);
			else if (GetQuadrant(newPoint, cell) == Quadrants::SOUTHWEST) InsertIntoTree(newPoint, cell->southWest);
			else InsertIntoTree(newPoint, cell->southEast);
		}

	}

	void CreateChildrenCells(Cell* cell) {
		//northwest cell
		cell->northWest = new Cell();
		cell->northWest->topLeft.x = cell->topLeft.x;
		cell->northWest->topLeft.y = cell->topLeft.y;
		cell->northWest->topRight.x = (cell->topRight.x - cell->topLeft.x) / 2 + cell->topLeft.x;
		cell->northWest->topRight.y = cell->topLeft.y;

		cell->northWest->bottomLeft.x = cell->bottomLeft.x;
		cell->northWest->bottomLeft.y = (cell->bottomLeft.y - cell->topLeft.y) / 2 + cell->topLeft.y;
		cell->northWest->bottomRight.x = cell->northWest->topRight.x;
		cell->northWest->bottomRight.y = cell->northWest->bottomLeft.y;

		//std::cout << "northwest: "
		//	<< cell->northWest->topLeft.x << " " << cell->northWest->topLeft.y << ", "
		//	<< cell->northWest->topRight.x << " " << cell->northWest->topRight.y << ", "
		//	<< cell->northWest->bottomLeft.x << " " << cell->northWest->bottomLeft.y << ", "
		//	<< cell->northWest->bottomRight.x << " " << cell->northWest->bottomRight.y << "\n";


		//northeast cell
		cell->northEast = new Cell();
		cell->northEast->topLeft.x = cell->northWest->topRight.x;
		cell->northEast->topLeft.y = cell->northWest->topRight.y;
		cell->northEast->topRight.x = cell->topRight.x;
		cell->northEast->topRight.y = cell->topRight.y;

		cell->northEast->bottomLeft.x = cell->northWest->bottomRight.x;
		cell->northEast->bottomLeft.y = cell->northWest->bottomRight.y;
		cell->northEast->bottomRight.x = cell->bottomRight.x;
		cell->northEast->bottomRight.y = cell->northWest->bottomRight.y;

		//std::cout << "northEast: "
		//	<< cell->northEast->topLeft.x << " " << cell->northEast->topLeft.y << ", "
		//	<< cell->northEast->topRight.x << " " << cell->northEast->topRight.y << ", "
		//	<< cell->northEast->bottomLeft.x << " " << cell->northEast->bottomLeft.y << ", "
		//	<< cell->northEast->bottomRight.x << " " << cell->northEast->bottomRight.y << "\n";

		//southwest cell
		cell->southWest = new Cell();
		cell->southWest->topLeft.x = cell->northWest->bottomLeft.x;
		cell->southWest->topLeft.y = cell->northWest->bottomLeft.y;
		cell->southWest->topRight.x = cell->northWest->bottomRight.x;
		cell->southWest->topRight.y = cell->northWest->bottomRight.y;
			  
		cell->southWest->bottomLeft.x = cell->bottomLeft.x;
		cell->southWest->bottomLeft.y = cell->bottomLeft.y;
		cell->southWest->bottomRight.x = cell->northWest->bottomRight.x;
		cell->southWest->bottomRight.y = cell->bottomRight.y;

		//std::cout << "southwest: "
		//	<< cell->southWest->topLeft.x << " " << cell->southWest->topLeft.y << ", "
		//	<< cell->southWest->topRight.x << " " << cell->southWest->topRight.y << ", "
		//	<< cell->southWest->bottomLeft.x << " " << cell->southWest->bottomLeft.y << ", "
		//	<< cell->southWest->bottomRight.x << " " << cell->southWest->bottomRight.y << "\n";

		//southeast cell
		cell->southEast = new Cell();
		cell->southEast->topLeft.x = cell->northWest->bottomRight.x;
		cell->southEast->topLeft.y = cell->northWest->bottomRight.y;
		cell->southEast->topRight.x = cell->northEast->bottomRight.x;
		cell->southEast->topRight.y = cell->northEast->bottomRight.y;
			 
		cell->southEast->bottomLeft.x = cell->southWest->bottomRight.x;
		cell->southEast->bottomLeft.y = cell->southWest->bottomRight.y;
		cell->southEast->bottomRight.x = cell->bottomRight.x;
		cell->southEast->bottomRight.y = cell->bottomRight.y;
		//std::cout << "southeast: "
		//	<< cell->southEast->topLeft.x << " " << cell->southEast->topLeft.y << ", "
		//	<< cell->southEast->topRight.x << " " << cell->southEast->topRight.y << ", "
		//	<< cell->southEast->bottomLeft.x << " " << cell->southEast->bottomLeft.y << ", "
		//	<< cell->southEast->bottomRight.x << " " << cell->southEast->bottomRight.y << "\n";

	}


	Quadrants GetQuadrant(const Position point, const Cell* cell) {
		int xMidPoint = (cell->topRight.x - cell->topLeft.x) / 2 + cell->topLeft .x;
		int yMidPoint = (cell->bottomLeft.y - cell->topLeft.y) / 2 + cell->topLeft.y;
		//std::cout << "point: " << point.x << " " << point.y << " midpoints: " << xMidPoint << " " << yMidPoint;

		if (point.x <= xMidPoint) {
			if (point.y <= yMidPoint) {
				//std::cout << "nw\n";
				return Quadrants::NORTHWEST;
			}
			else {
				//std::cout << "sw\n";
				return Quadrants::SOUTHWEST;
			}
		}
		else {
			if (point.y <= yMidPoint) {
				//std::cout << "ne\n";
				return Quadrants::NORTHEAST;
			}
			else {
				//std::cout << "se\n";
				return Quadrants::SOUTHEAST;
			}
		}

	}

	void DrawCell(const Cell *cell) {
		if (cell == nullptr ) return;

		DrawLine(cell->topLeft.x, cell->topLeft.y, cell->topRight.x, cell->topRight.y, olc::WHITE);
		DrawLine(cell->topLeft.x, cell->topLeft.y, cell->bottomLeft.x, cell->bottomLeft.y, olc::WHITE);
		DrawLine(cell->bottomLeft.x, cell->bottomLeft.y, cell->bottomRight.x, cell->bottomRight.y, olc::WHITE);
		DrawLine(cell->topRight.x, cell->topRight.y, cell->bottomRight.x, cell->bottomRight.y, olc::WHITE);
		for (Position p : cell->points) {
			Draw(p.x, p.y, olc::GREEN);
		}

		DrawCell(cell->northWest);
		DrawCell(cell->northEast);
		DrawCell(cell->southWest);
		DrawCell(cell->southEast);
	}
};

int main(int argc, char* argv[]) {
	
	MyApplication currentApplication;

	if (currentApplication.Construct(800, 800, 1, 1)) {
		currentApplication.Start();
	}

	return 0;
}