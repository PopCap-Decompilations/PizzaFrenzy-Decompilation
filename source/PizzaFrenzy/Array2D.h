// Array2D<T>: a width x height grid (CityMap's tiles, PathFinder's nodes).
#pragma once

// Stored column by column in one block: element (x, y) is m_rows[x][y]. No vtable; the members are inline (the
// copies at 0x411E20, 0x411E60 and 0x4121C0 were emitted in CityMap's object, 0x4299A0 in PathFinder's).
template <class T>
class Array2D
{
public:
	Array2D()
		: m_rows(0), m_width(0), m_height(0)
	{
	}

	// 0x411E20 (T = engine::RefPtr<Tile>; inlined in ~PathFinder for T = PathNode)
	~Array2D()
	{
		if (m_rows)
		{
			delete[] m_rows[0];
			delete[] m_rows;
		}
	}

	// 0x411E60 (T = engine::RefPtr<Tile>)
	// 0x4299A0 (T = PathNode)
	void create(int width, int height)
	{
		m_width = width;
		m_height = height;
		if (width && height)
		{
			m_rows = new T*[width];
			T* cells = new T[width * height];
			for (int x = 0; x < width; x++)
				m_rows[x] = cells + x * height;
		}
		else
			m_rows = 0;
	}

	// 0x4121C0 (T = engine::RefPtr<Tile>; inlined in PathFinder::setMap for T = PathNode)
	void resize(int width, int height)
	{
		if (width != m_width || height != m_height)
		{
			if (m_rows)
			{
				delete[] m_rows[0];
				delete[] m_rows;
			}
			create(width, height);
		}
	}

	T** m_rows;								// +0x00 one pointer per column into a single block of width * height
	int m_width;							// +0x04
	int m_height;							// +0x08
};
