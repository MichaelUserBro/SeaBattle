#include "Placement.h"
#include <set>
#include <map>
#include <vector>
#include <random>


// =================== Класс PlacementValidator ===================

bool PlacementValidator::canPlace(const Board& board, const Ship& ship, const Rules& rules) {
	if (ship.getCoords().empty()) return false; // Проверка, что корабль занимает хотя бы одну клетку 

	for (const Coordinate& coord : ship.getCoords()) {
		if (!coord.inBounds(rules.fieldSize)) return false; // Проверка, что корабль находится внутри границ поля
		if (board.cellStatusAt(coord) != CellStatus::Empty) return false; // Проверка, что все клетки для коробля пустые

		if (!rules.allowTouching) { // Если запрещено касание двух кораблей
			for (const Coordinate& c : coord.getNeighbors(rules.fieldSize))
				if (board.cellStatusAt(c) == CellStatus::Ship) return false; // Проверка, что нет кораблей в соседних клетках
		}
	}

	return true;
}
void PlacementValidator::validate(const std::vector<Ship>& ships, const Rules& rules) {
	for (const Ship& ship : ships) {
		std::vector<Coordinate> coords = ship.getCoords();
		if (coords.size() == 0) throw PlacementError("Длина корабля не может быть равна 0!");

		bool sameRow = true;
		bool sameColumn = true;

		for (const Coordinate& c : coords) {
			if (c.getRow() != coords[0].getRow())    sameRow = false;
			if (c.getColumn() != coords[0].getColumn()) sameColumn = false;
		}

		if (!sameColumn && !sameRow) throw PlacementError("Клетки корабля не находятся на одной линии!");

		if (sameColumn) {
			for (int i = 0; i + 1 < coords.size(); i++) {
				if (coords[i].getRow() != coords[i + 1].getRow() - 1) 
					throw PlacementError("Клетки корабля не являются сплошной линией!");
			}
		}
		else {
			for (int i = 0; i + 1 < coords.size(); i++) {
				if (coords[i].getColumn() != coords[i + 1].getColumn() - 1)
					throw PlacementError("Клетки корабля не являются сплошной линией!");
			}
		}

		for (Coordinate coord : coords) {
			if (!coord.inBounds(rules.fieldSize)) throw PlacementError("Корабль выходить за границы поля!");
		}

	}
	std::set<Coordinate> used;

	for (const Ship& ship : ships) {
		for (const Coordinate& c : ship.getCoords()) {
			if (!used.insert(c).second)
				throw PlacementError("Корабли пересекаются!");
		}
	}

	if (!rules.allowTouching) {
		for (const Ship& ship : ships) {
			const std::vector<Coordinate>& coords = ship.getCoords();
			const std::set<Coordinate> own(coords.begin(), coords.end());

			for (const Coordinate& c : coords) {
				for (const Coordinate& n : c.getNeighbors(rules.fieldSize)) {
					if (used.count(n) && !own.count(n))
						throw PlacementError("Корабли поставлены рядом!");
				}
			}
		}
	}

	std::map<int, int> curFleet;
	for (const Ship& ship : ships)
		++curFleet[ship.getCoords().size()];
	if (curFleet != rules.fleet) throw PlacementError("Неверный состав кораблей!");
}

// =================== Класс RandomFleetPlacer ===================

// Placement.cpp
RandomFleetPlacer::RandomFleetPlacer() : rng_(std::random_device{}()) {} // РАЗОБРАТЬСЯ КАК РАБОТАЕТ!!!!
RandomFleetPlacer::RandomFleetPlacer(unsigned seed) : rng_(seed) {} // РАЗОБРАТЬСЯ КАК РАБОТАЕТ!!!!

void RandomFleetPlacer::place(Board& board, const Rules& rules) {

	std::vector<int> lengths;
	const auto& fleet = rules.fleet;	
	for (auto it = fleet.rbegin(); it != fleet.rend(); ++it)
		for (int k = 0; k < it->second; ++k)
			lengths.push_back(it->first);



	for (int restart = 0; restart <= COUNT_OF_RESTART; restart++) {

		board.clear();
		bool allPlaced = true;


		for (int len : lengths) {
			bool placed = false;
			for (int attemp = 0; attemp <= ATTEMP_PER_SHIP && !placed; attemp++) {
				Orientation orient = (std::uniform_int_distribution<int>(0, 1)(rng_) == 0)
					? Orientation::Horizontal : Orientation::Vertical;

				int size = rules.fieldSize;
				int maxRow = size - 1;
				int maxCol = size - 1;
				if (orient == Orientation::Horizontal) maxCol = size - len;
				else                                   maxRow = size - len;
				if (maxRow < 0 || maxCol < 0)
					throw PlacementError("Корабль не помещается на поле!");

				int row = std::uniform_int_distribution<int>(0, maxRow)(rng_);
				int col = std::uniform_int_distribution<int>(0, maxCol)(rng_);

				Ship ship = makeShip(Coordinate(row, col), len, orient);
				if (PlacementValidator::canPlace(board, ship, rules)) {
					board.placeShip(ship);
					placed = true;
				}
			}
			if (!placed) {
				allPlaced = false;
				break;
			}
		}
		if (allPlaced) return;
		
	}
	throw PlacementError("Не удалось расставить флот!");
}