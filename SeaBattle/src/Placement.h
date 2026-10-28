#pragma once
#include "Core.h"
#include "Error.h"
#include <random>





class PlacementValidator {
	public:
		static bool canPlace(const Board&, const Ship&, const Rules&);
		static void validate(const std::vector<Ship>&, const Rules&);
};

class FleetPlacer {
	public:
		virtual void place(Board&, const Rules&) = 0;
		virtual ~FleetPlacer() = default;
};

class RandomFleetPlacer : public FleetPlacer {
	private:
		std::mt19937 rng_;
		static const int COUNT_OF_RESTART = 300;
		static const int ATTEMP_PER_SHIP = 50;

	public:

		RandomFleetPlacer();
		explicit RandomFleetPlacer(unsigned seed); // –¿«Œ¡–¿“‹—ﬂ  ¿  –¿¡Œ“¿≈“!!!!

		~RandomFleetPlacer() = default;
	
		void place(Board& board, const Rules& rules) override;

};