#include "Placement.h"

// ===================  ласс PlacementValidator ===================

bool PlacementValidator::canPlace(const Board& board, const Ship& ship, const Rules& rules) {
	if (ship.getCoords().empty()) return false; // ѕроверка, что корабль занимает хот€ бы одну клетку 

	for (const Coordinate& coord : ship.getCoords()) {
		if (!coord.inBounds(rules.getFieldSize())) return false; // ѕроверка, что корабль находитс€ внутри границ пол€
		if (board.cellStatusAt(coord) != CellStatus::Empty) return false; // ѕроверка, что все клетки дл€ коробл€ пустые

		if (!rules.getAllowTouching()) { // ≈сли запрещено касание двух кораблей
			for (const Coordinate& c : coord.getNeighbors(rules.getFieldSize()))
				if (board.cellStatusAt(c) == CellStatus::Ship) return false; // ѕроверка, что нет кораблей в соседних клетках
		}
	}

	return true;
}
void PlacementValidator::validate(const std::vector<Ship>& ships, const Rules& rules) {
	for (const Ship& ship : ships) {
		std::vector<Coordinate> coords = ship.getCoords();
		if (coords.size() == 0) throw PlacementError("ƒлина корабл€ не может быть равна 0!");

		bool sameRow = true;
		bool sameColumn = true;

		for (const Coordinate& c : coords) {
			if (c.getRow() != coords[0].getRow())    sameRow = false;
			if (c.getColumn() != coords[0].getColumn()) sameColumn = false;
		}

		if (!sameColumn && !sameRow) throw PlacementError(" летки корабл€ не наход€тс€ на одной линии!");

		if (sameColumn) {
			for (int i = 0; i + 1 < coords.size(); i++) {
				if (coords[i].getRow() != coords[i + 1].getRow() - 1) 
					throw PlacementError(" летки корабл€ не €вл€ютс€ сплошной линией!");
			}
		}
		else {
			for (int i = 0; i + 1 < coords.size(); i++) {
				if (coords[i].getColumn() != coords[i + 1].getColumn() - 1)
					throw PlacementError(" летки корабл€ не €вл€ютс€ сплошной линией!");
			}
		}
	}
}