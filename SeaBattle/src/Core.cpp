#pragma once
#include "Core.h"
#include "Error.h"
#include <random>

Ship makeShip(Coordinate start, int length, Orientation orient) {
	std::vector<Coordinate> coords;
	coords.push_back(start);
	for (int i = 1; i < length; i++) {
		Coordinate c;
		if (orient == Orientation::Horizontal) { 
			c = Coordinate(start.getRow(),start.getColumn() + i);
		}
		else {
			c = Coordinate(start.getRow() + 1, start.getColumn());
		}
		coords.push_back(c);

	}
	return Ship(coords);
}

// =================== Класс Rules ===================

int Rules::getFieldSize() const { return fieldSize_; }
bool Rules::getAllowTouching() const { return allowTouching_; }
bool Rules::isContinue() const { return isShooterContinuesAfterHit_; }
std::map<int, int> Rules::getFleet() const { return fleet_; }


// =================== Класс Coordinate ===================

Coordinate::Coordinate() : row(0), column(0) {};
Coordinate::Coordinate(int r, int c) : row(r), column(c) {};

int Coordinate::getRow() const { return row; }
int Coordinate::getColumn() const { return column; }


//Парсер - проверяет ввод пользователя и превращает его в координату
Coordinate Coordinate::parse(const std::string& text, int fieldSize) {
	std::string s;
	int row{};
	int col{};

	// Проверка является ли строка пустой
	for(char c : text) {
		if (!std::isspace(c)) { s += c; }
	}
	if (s.size() == 0) {
		throw InvalidInputError("Введена пустая строка!");
	}

	// Проверка буквы
	if (!isalpha(s[0])){
		throw InvalidInputError("Строка должна быть буквой!");
	}

	s[0] = toupper(s[0]);

	if ('A' <= s[0] && s[0] <= ENG_ALF[fieldSize - 1]) {
		row = s[0] - 'A';
	}
	else {
		throw InvalidInputError(std::string("Строка должна быть буквой A-") + ENG_ALF[fieldSize - 1] + "!");
	}

	// Проверка числа
	s = s.substr(1);
	if (s.empty()) {
		throw InvalidInputError("После буквы должен идти номер!");
	}
	for (char c : s) {
		if (!isdigit(c)) {
			throw InvalidInputError("Столбец должен обозначаться числом!");
		}
	}
	int num{};
	try {
		num = std::stoi(s);
	}
	catch(...){
		num = -1;
	}
	
	if (num <= fieldSize && num > 0) {
		col = num - 1;
	}
	else {
		throw InvalidInputError("Номер столбца должен быть от 1 до " + std::to_string(fieldSize) + "!");
	}

	return Coordinate(row, col);
}
bool Coordinate::inBounds(int size) const {
	return row >= 0 && row < size && column >= 0 && column < size;
}
std::vector<Coordinate> Coordinate::getNeighbors(int size) const {
	std::vector<Coordinate> result;
	for (int dr = -1; dr <= 1; ++dr) {
		for (int dc = -1; dc <= 1; ++dc) {
			if (dr == 0 && dc == 0) continue;      
			Coordinate n{ row + dr, column + dc };
			if (n.inBounds(size))
				result.push_back(n);
		}
	}
	return result;
}
std::string Coordinate::toString() const {
	return std::string(1, static_cast<char>('A' + column)) + std::to_string(row + 1);
}

bool Coordinate::operator==(const Coordinate& other) const {
	return row == other.row && column == other.column;
}
bool Coordinate::operator<(const Coordinate& other) const	 {
	return row < other.row || column < other.column;
}


// =================== Класс Ship ===================

Ship::Ship() : coords_(), hitStatus_() {};
Ship::Ship(const std::vector<Coordinate>& coords) : coords_(coords), hitStatus_(coords.size(), false) {
	std::sort(coords_.begin(), coords_.end());
};

int Ship::getLength() { return coords_.size(); }
std::vector<Coordinate> Ship::getCoords() const { return coords_; }


bool Ship::contains(const Coordinate& coord) const {
	for (const Coordinate& c : coords_) {
		if (c == coord) return true;
	}
	return false;
}
bool Ship::isSunk() {
	for (const bool& flag : hitStatus_) {
		if (!flag) return false;
	}
	return true;
}
void Ship::hit(const Coordinate& c) {
	for (size_t i = 0; i < coords_.size(); ++i) {
		if (coords_[i] == c) {
			hitStatus_[i] = true;
			return;
		}
	}
}

// =================== Класс Board ===================

Board::Board() : field_(), ships_(), fieldSize_() {};
Board::Board(int size) : field_(size, std::vector<CellStatus>(size, CellStatus::Empty)), ships_(), fieldSize_(size) {};

std::vector<Ship> Board::getShips() const { return ships_; }

void Board::placeShip(Ship ship) {

	for (Coordinate coords : ship.getCoords()) {
		if (field_[coords.getRow()][coords.getColumn()] != CellStatus::Empty) throw PlacementError();
	}
	for (Coordinate coords : ship.getCoords()) {
		field_[coords.getRow()][coords.getColumn()] = CellStatus::Ship;
	}
	ships_.push_back(ship);
}
void Board::clear() {
	ships_.clear();
	for (int i = 0; i < fieldSize_; i++)
		for (int j = 0; j < fieldSize_; j++)
			field_[i][j] = CellStatus::Empty;
}
bool Board::allSunk() {
	for (Ship ship : ships_)
		if (!ship.isSunk()) return false;
	return true;
}
bool Board::alreadyShot(const Coordinate& c) const {
	CellStatus s = cellStatusAt(c);
	return s == CellStatus::Miss || s == CellStatus::Hit || s == CellStatus::Killed;
}
CellStatus Board::cellStatusAt(const Coordinate& coord) const { return field_[coord.getRow()][coord.getColumn()]; }
ShotResult Board::receiveShot(const Coordinate& c) {
	if (!c.inBounds(fieldSize_)) throw InvalidInputError("Клетка вне поля!");
	if (alreadyShot(c)) throw InvalidInputError("Сюда уже стреляли!");

	CellStatus& cell = field_[c.getRow()][c.getColumn()];

	if (cell == CellStatus::Empty) {           
		cell = CellStatus::Miss;
		return { ShotStatus::Miss, std::nullopt };
	}


	cell = CellStatus::Hit;
	for (Ship& ship : ships_) {                
		if (ship.contains(c)) {
			ship.hit(c);
			if (!ship.isSunk())
				return { ShotStatus::Hit, std::nullopt };

			for (const Coordinate& p : ship.getCoords())
				field_[p.getRow()][p.getColumn()] = CellStatus::Killed;
			return { ShotStatus::Killed, ship };
		}
	}

}

