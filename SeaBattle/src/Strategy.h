#pragma once
#include "core.h"
#include <random>
#include <vector>

enum class Difficulty {Easy,Medium,Hard};

class ShootingStrategy {
	protected:
		Rules rules_;
		std::mt19937 rng_;
		std::vector<std::vector<CellStatus>> knowledge_;

	public:

		virtual ~ShootingStrategy() = default;
		virtual Coordinate nextShot() = 0;
		virtual void onResult(Coordinate c, ShotResult r) = 0;
		
};