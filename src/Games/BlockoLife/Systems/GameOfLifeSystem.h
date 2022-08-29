#pragma once

#include "System.h"
#include "Vector.h"

#include "Species.h"

struct CreatureCache;

class GameOfLifeSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_BL_GAMEOFLIFE; }

	GameOfLifeSystem(std::shared_ptr<World> parentWorld)
		: System(parentWorld)
	{
	}
	
	void Init(const SystemInitialiser& = s_EmptyInitialiser) override;

	void Update(float deltaSecs) final override;

private:
	// TEMP-ish structure until we decide properly how this data should be stored/cached.
	template<typename T>
	class Array2D
	{
	public:
		Array2D(size_t width, size_t height, size_t offsetX, size_t offsetY)//, const T& defaultVal)
		: m_Width(width)
		, m_Height(height)
		, m_OffsetX(offsetX)
		, m_OffsetY(offsetY)
		{
			assert(width > 0);
			assert(height > 0);
			// TODO: Compiler warning makes no sense?!
			m_Array = new T[NumCells()];
			//for (size_t i = 0; i < NumCells(); ++i) m_Array[i] = defaultVal;
		}
		~Array2D()
		{
			delete[] m_Array;
		}

		Array2D(const Array2D&) = delete;
		Array2D& operator=(const Array2D&) = delete;
		Array2D(Array2D&&) = delete;
		Array2D& operator=(Array2D&&) = delete;

		inline T& At(int x, int y)
		{
			size_t index = (y+m_OffsetY)*m_Width + (x+m_OffsetX);
			assert(index < NumCells());
			return m_Array[index];
		}
		inline T& At(const Vector2i& p)
		{
			return At(p.x, p.y);
		}

	private:
		inline size_t NumCells() const { return m_Width * m_Height; }

		T* m_Array;
		size_t m_Width;
		size_t m_Height;
		size_t m_OffsetX;
		size_t m_OffsetY;

	};

	uint8_t CountNeighbours(Array2D<CreatureCache>& cachedEntities, int x, int y, Species s);
	
	void Tick_Move_Herbivore(Array2D<CreatureCache>& cachedEntities, CreatureCache& cache, int x, int y, std::set<EntityID>& entitiesToRemove);
	void Tick_Move_Carnivore(Array2D<CreatureCache>& cachedEntities, CreatureCache& cache, int x, int y, std::set<EntityID>& entitiesToRemove);
	
	void Tick();


};
