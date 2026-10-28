#include "Placement.h"
#include <set>
#include <map>
#include <vector>
#include <random>

// =================== Класс PlacementValidator ===================

bool PlacementValidator::canPlace(const Board& board, const Ship& ship, const Rules& rules) {
	if (ship.getCoords().empty()) return false; // Проверка, что корабль занимает хотя бы одну клетку 

	for (const Coordinate& coord : ship.getCoords()) {
		if (!coord.inBounds(rules.getFieldSize())) return false; // Проверка, что корабль находится внутри границ поля
		if (board.cellStatusAt(coord) != CellStatus::Empty) return false; // Проверка, что все клетки для коробля пустые

		if (!rules.getAllowTouching()) { // Если запрещено касание двух кораблей
			for (const Coordinate& c : coord.getNeighbors(rules.getFieldSize()))
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
			if (!coord.inBounds(rules.getFieldSize())) throw PlacementError("Корабль выходить за границы поля!");
		}

	}
	std::set<Coordinate> used;

	for (const Ship& ship : ships) {
		for (const Coordinate& c : ship.getCoords()) {
			if (!used.insert(c).second)
				throw PlacementError("Корабли пересекаются!");
		}
	}

	if (!rules.getAllowTouching()) {
		for (const Ship& ship : ships) {
			const std::vector<Coordinate>& coords = ship.getCoords();
			const std::set<Coordinate> own(coords.begin(), coords.end());

			for (const Coordinate& c : coords) {
				for (const Coordinate& n : c.getNeighbors(rules.getFieldSize())) {
					if (used.count(n) && !own.count(n))
						throw PlacementError("Корабли поставлены рядом!");
				}
			}
		}
	}

	std::map<int, int> curFleet = { {4,0},{3,0},{2,0},{1,0} };
	for (const Ship& ship : ships) {
		curFleet[ship.getCoords().size()] += 1;
	}
	if (curFleet != rules.getFleet()) throw PlacementError("Неверный состав кораблей!");
}

// =================== Класс RandomFleetPlacer ===================

void RandomFleetPlacer::place(Board& board, const Rules& rules) {
	board.clear();
	std::vector<int> lengths;
	for (auto& pair : rules.getFleet()) {
		for (int i = 0; i < pair.second; i++) {
			lengths.push_back(pair.first);
		}
	}
	int j = 1;
	for (; j <= COUNT_OF_RESTART; j++) {
		int i = 1;
		for (int len : lengths) {
			for (; i <= ATTEMP_PER_SHIP; i++) {
				Orientation orient = randomOrientation();
				int row{};
				int col{};
				if (orient == Orientation::Horizontal) {
					std::uniform_int_distribution<int> row(0, rules.getFieldSize() - len);
					std::uniform_int_distribution<int> сol(0, rules.getFieldSize() - 1);
				}
				else {
					std::uniform_int_distribution<int> row(0, rules.getFieldSize() - 1);
					std::uniform_int_distribution<int> сol(0, rules.getFieldSize() - len);
				}

				Ship ship = makeShip(Coordinate(row, col), len, orient);
				if (PlacementValidator::canPlace(board, ship, rules)) {
					board.placeShip(ship);
					break;
				}
			}
			if (i == ATTEMP_PER_SHIP) break;
		}
		if (i == ATTEMP_PER_SHIP) board.clear();
		
	}
	if (j == COUNT_OF_RESTART) throw PlacementError("Не удалось расставить флот!");
}