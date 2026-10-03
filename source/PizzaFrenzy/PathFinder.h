// PathNode, PathFinder and Path: the A* search over the city map's cells and the route it returns; g_directions.
#pragma once

#include <vector>

#include "engine/Object.h"
#include "engine/Point.h"

#include "Array2D.h"

class CityMap;
class Tile;

// The neighbour offsets (-1,0), (0,1), (1,0), (0,-1) at 0x532938 (dynamic initializer 0x4F88B0), walked by the
// path finder and by Vehicle::faceRoad
extern engine::Point g_directions[4];

// One search node per map cell (PathFinder::m_nodes). Implicit constructor (0x4297E0, it constructs m_cell) and
// destructor.
class PathNode
{
public:
	PathNode* m_parent;							// +0x00 previous node on the best path
	int m_f;									// +0x04 m_g + m_h, the open list's heap key
	int m_g;									// +0x08 cost from the start
	int m_h;									// +0x0C Manhattan distance to the goal
	int m_cost;									// +0x10 the cell's tile's getWeight(cell), 0 without a tile
	bool m_inOpenList;							// +0x14 set by pushOpenNode, cleared by popOpenNode
	bool m_inClosedList;						// +0x15 set when popped
	engine::Point m_cell;						// +0x18 (x, y)
};

// The predicate of the open list's std::push_heap/pop_heap/make_heap calls (inlined into their instantiations
// 0x429A90, 0x429AE0 and 0x429C40): a min-heap on PathNode::m_f.
struct PathNodeCompare
{
	bool operator()(PathNode* left, PathNode* right) const
	{
		return left->m_f > right->m_f;
	}
};

// A route returned by PathFinder::findPath (and built by Copter::findPath): the cells from the start to the goal.
// Implicit destructor.
class Path : public engine::Object
{
public:
	Path();

	std::vector<engine::Point> m_points;		// +0x0C
};

// A* search over the city map's cells, embedded in CityMap (+0x104); not polymorphic. The passability test is the
// caller's (a vehicle's), and tiles give the cost of crossing them (Tile::getWeight).
class PathFinder
{
public:
	PathFinder();
	~PathFinder();

	bool isJunction(const engine::Point& cell, bool (*isPassable)(Tile* tile, const engine::Point& cell));
	void propagate(PathNode* node, const engine::Point& goal, bool (*isPassable)(Tile* tile, const engine::Point& cell));
	void setMap(CityMap* map);
	PathNode* popOpenNode();
	void pushOpenNode(PathNode* node);
	Path* findPath(const engine::Point& start, const engine::Point& goal, bool (*isPassable)(Tile* tile, const engine::Point& cell));

	CityMap* m_map;								// +0x00 set by setMap; not reference counted
	std::vector<PathNode*> m_openList;			// +0x04 binary min-heap on PathNode::m_f
	std::vector<PathNode*> m_closedList;		// +0x14 nodes popped so far
	engine::Point m_goal;						// +0x24 goal of the current search
	Array2D<PathNode> m_nodes;					// +0x2C one node per map cell, [x][y]
};
