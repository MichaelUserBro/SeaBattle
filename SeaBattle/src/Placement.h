#pragma once
#include "Core.h"
#include "Error.h"





class PlacementValidator {
	public:
		static bool canPlace(const Board&, const Ship&, const Rules&);
		static void validate(const std::vector<Ship>&, const Rules&);
};

class FleetPlacer {
	virtual void place(Board&, const Rules&) = 0;
	virtual ~FleetPlacer() = default;
};

class RandomFleetPlacer : public FleetPlacer {
	public:
		const int COUNT_OF_RESTART = 300;
		const int ATTEMP_PER_SHIP = 50;

		void place(Board& board, const Rules& rules);
};