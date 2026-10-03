// PathNode, PathFinder and Path: the A* search over the city map's cells and the route it returns; g_directions.
#include "PathFinder.h"

#include <stdlib.h>
#include <algorithm>

#include "CityMap.h"
#include "Tile.h"

// g_directions at 0x532938 (dynamic initializer 0x4F88B0)
engine::Point g_directions[4] = { engine::Point(-1, 0), engine::Point(0, 1), engine::Point(1, 0), engine::Point(0, -1) };

// 0x429770
bool PathFinder::isJunction(const engine::Point& cell, bool (*isPassable)(Tile* tile, const engine::Point& cell))
{
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		engine::Point neighbour = cell + g_directions[i];
		if (isPassable(m_map->getTile(neighbour), neighbour))
			count++;
	}
	return count > 2;
}

// 0x429820
void PathFinder::propagate(PathNode* node, const engine::Point& goal, bool (*isPassable)(Tile* tile, const engine::Point& cell))
{
	for (int i = 0; i < 4; i++)
	{
		engine::Point cell = node->m_cell;
		int cost = 0;
		engine::Point next = cell + g_directions[i];
		if (isPassable(m_map->getTile(next), next))
		{
			while (true)
			{
				if (isJunction(next, isPassable))
				{
					cell = next;
					break;
				}
				if (next == goal)
				{
					cell = next;
					break;
				}
				cell = next;
				cost += m_nodes.m_rows[cell.x][cell.y].m_cost;
				next = cell + g_directions[i];
				if (!isPassable(m_map->getTile(next), next))
					break;
			}
		}
		if (cell == node->m_cell)
			return;
		PathNode* neighbour = &m_nodes.m_rows[cell.x][cell.y];
		if (neighbour == node->m_parent)
			return;
		if (!isPassable(m_map->getTile(neighbour->m_cell), neighbour->m_cell))
			return;
		// the original compares with the propagating node's cost, not the neighbour's
		int g = node->m_g + neighbour->m_cost + cost;
		if (g >= node->m_g)
			return;
		neighbour->m_g = g;
		neighbour->m_parent = node;
		neighbour->m_f = g + neighbour->m_h;
		if (neighbour->m_inOpenList)
			return;
		propagate(neighbour, goal, isPassable);
	}
}

// 0x429CF0
void PathFinder::setMap(CityMap* map)
{
	m_map = map;
	m_nodes.resize(m_map->getCols(), m_map->getRows());
	for (int x = 0; x < m_map->getCols(); x++)
	{
		for (int y = 0; y < m_map->getRows(); y++)
		{
			m_nodes.m_rows[x][y].m_inClosedList = false;
			m_nodes.m_rows[x][y].m_inOpenList = false;
			m_nodes.m_rows[x][y].m_parent = NULL;
			m_nodes.m_rows[x][y].m_cell.set(x, y);
		}
	}
}

// 0x429DC0
PathFinder::~PathFinder()
{
}

// 0x429E50
PathNode* PathFinder::popOpenNode()
{
	PathNode* node = m_openList.front();
	std::pop_heap(m_openList.begin(), m_openList.end(), PathNodeCompare());
	m_openList.pop_back();
	node->m_inOpenList = false;
	return node;
}

// 0x42A370
PathFinder::PathFinder()
{
	m_map = NULL;
}

// 0x42A3E0
Path::Path()
{
}

// 0x42A570
void PathFinder::pushOpenNode(PathNode* node)
{
	m_openList.push_back(node);
	std::push_heap(m_openList.begin(), m_openList.end(), PathNodeCompare());
	node->m_inOpenList = true;
}

// 0x42A600
Path* PathFinder::findPath(const engine::Point& start, const engine::Point& goal, bool (*isPassable)(Tile* tile, const engine::Point& cell))
{
	for (int x = 0; x < m_map->getCols(); x++)
	{
		for (int y = 0; y < m_map->getRows(); y++)
		{
			m_nodes.m_rows[x][y].m_inClosedList = false;
			m_nodes.m_rows[x][y].m_inOpenList = false;
			m_nodes.m_rows[x][y].m_parent = NULL;
			m_nodes.m_rows[x][y].m_cell.set(x, y);
			m_nodes.m_rows[x][y].m_g = 0;
			m_nodes.m_rows[x][y].m_h = 0;
			m_nodes.m_rows[x][y].m_f = 0;
			Tile* tile = m_map->getTile(m_nodes.m_rows[x][y].m_cell);
			if (tile)
				m_nodes.m_rows[x][y].m_cost = tile->getWeight(m_nodes.m_rows[x][y].m_cell);
			else
				m_nodes.m_rows[x][y].m_cost = 0;
		}
	}
	m_closedList.clear();
	m_openList.clear();
	m_goal = goal;

	PathNode* node = &m_nodes.m_rows[start.x][start.y];
	node->m_g = node->m_cost;
	node->m_cell = start;
	node->m_h = abs(m_goal.x - node->m_cell.x) + abs(m_goal.y - node->m_cell.y);
	node->m_f = node->m_g + node->m_h;
	pushOpenNode(node);

	while (!m_openList.empty())
	{
		PathNode* current = popOpenNode();
		m_closedList.push_back(current);
		current->m_inClosedList = true;
		if (current->m_cell == goal)
		{
			Path* path = new Path();
			PathNode* step = current;
			do
			{
				path->m_points.push_back(step->m_cell);
				step = step->m_parent;
			}
			while (step);
			std::reverse(path->m_points.begin(), path->m_points.end());
			return path;
		}

		for (int i = 0; i < 4; i++)
		{
			engine::Point cell = current->m_cell;
			int cost = 0;
			engine::Point next = cell + g_directions[i];
			if (isPassable(m_map->getTile(next), next))
			{
				while (true)
				{
					if (isJunction(next, isPassable))
					{
						cell = next;
						break;
					}
					if (next == goal)
					{
						cell = next;
						break;
					}
					cell = next;
					cost += m_nodes.m_rows[cell.x][cell.y].m_cost;
					next = cell + g_directions[i];
					if (!isPassable(m_map->getTile(next), next))
						break;
				}
			}
			if (cell == current->m_cell)
				continue;
			PathNode* neighbour = &m_nodes.m_rows[cell.x][cell.y];
			if (neighbour == current->m_parent)
				continue;
			if (!isPassable(m_map->getTile(neighbour->m_cell), neighbour->m_cell))
				continue;
			int g = current->m_g + neighbour->m_cost + cost;
			if (neighbour->m_inOpenList)
			{
				if (g < neighbour->m_g)
				{
					neighbour->m_g = g;
					neighbour->m_parent = current;
					neighbour->m_f = g + neighbour->m_h;
					std::make_heap(m_openList.begin(), m_openList.end(), PathNodeCompare());
				}
			}
			else if (neighbour->m_inClosedList)
			{
				if (g < neighbour->m_g)
				{
					neighbour->m_parent = current;
					neighbour->m_g = g;
					neighbour->m_h = abs(m_goal.x - neighbour->m_cell.x) + abs(m_goal.y - neighbour->m_cell.y);
					neighbour->m_f = g + neighbour->m_h;
					pushOpenNode(neighbour);
					propagate(neighbour, goal, isPassable);
				}
			}
			else
			{
				neighbour->m_parent = current;
				neighbour->m_g = g;
				neighbour->m_h = abs(m_goal.x - neighbour->m_cell.x) + abs(m_goal.y - neighbour->m_cell.y);
				neighbour->m_f = g + neighbour->m_h;
				pushOpenNode(neighbour);
			}
		}
	}
	return NULL;
}
