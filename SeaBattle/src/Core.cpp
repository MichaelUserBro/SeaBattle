#pragma once
#include "Core.h"
#include "Error.h"
#include <random>

// Создание корабля
Ship makeShip(Coordinate start, int length, Orientation orient) {
	std::vector<Coordinate> coords;
	coords.push_back(start);
	for (int i = 1; i < length; i++) {
		Coordinate c;
		if (orient == Orientation::Horizontal) { 
			c = Coordinate(start.getRow(),start.getColumn() + i);
		}
		else {
			c = Coordinate(start.getRow() + i, start.getColumn());
		}
		coords.push_back(c);

	}
	return Ship(coords);
}

// =================== Структура Rules ===================

// Конструкторы:
Rules::Rules() : fieldSize(10), allowTouching(false),
isShooterContinuesAfterHit(true), fleet({ {4,1},{3,2},{2,3},{1,4} }){}
Rules::Rules(int newFieldSize, bool newAllowTouching, 
	bool newContinueShoot, std::map<int, int> newFleet) {
	fieldSize = newFieldSize;
	allowTouching = newAllowTouching;
	isShooterContinuesAfterHit = newContinueShoot;
	fleet = newFleet;
}

// =================== Класс Coordinate ===================

// Конструкторы:
Coordinate::Coordinate() : row_(0), column_(0) {};
Coordinate::Coordinate(int r, int c) : row_(r), column_(c) {};

// Геттеры:
int Coordinate::getRow() const { return row_; }
int Coordinate::getColumn() const { return column_; }

// Методы:
// Парсинг пользовательского ввода и преобразование его в координату
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
		col = s[0] - 'A';
	}
	else {
		throw InvalidInputError(std::string("Столбец должна быть буквой A-") + ENG_ALF[fieldSize - 1] + "!");
	}

	// Проверка числа
	s = s.substr(1);
	if (s.empty()) {
		throw InvalidInputError("После буквы должен идти номер!");
	}
	for (char c : s) {
		if (!isdigit(c)) {
			throw InvalidInputError("Строка должен обозначаться числом!");
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
		row = num - 1;
	}
	else {
		throw InvalidInputError("Номер строки должен быть от 1 до " + std::to_string(fieldSize) + "!");
	}

	return Coordinate(row, col);
}

// Проверка находится ли точка внутри границ поля
bool Coordinate::inBounds(int size) const {
	return row_ >= 0 && row_ < size && column_ >= 0 && column_ < size;
}

// Создаёт вектор всех соседей
std::vector<Coordinate> Coordinate::getNeighbors(int size) const {
	std::vector<Coordinate> result;
	for (int dr = -1; dr <= 1; ++dr) {
		for (int dc = -1; dc <= 1; ++dc) {
			if (dr == 0 && dc == 0) continue;      
			Coordinate n{ row_ + dr, column_ + dc };
			if (n.inBounds(size))
				result.push_back(n);
		}
	}
	return result;
}

// Превращает значение координат в строку
std::string Coordinate::toString() const {
	return std::string(1, static_cast<char>('A' + column_)) + std::to_string(row_ + 1);
}

// Операторы:
bool Coordinate::operator==(const Coordinate& other) const {
	return row_ == other.row_ && column_ == other.column_;
}
bool Coordinate::operator<(const Coordinate& other) const	 {
	if (row_ != other.row_) return row_ < other.row_;
	return column_ < other.column_;
}


// =================== Класс Ship ===================

// Конструкторы:
Ship::Ship() : coords_(), hitStatus_() {};
Ship::Ship(const std::vector<Coordinate>& coords) : coords_(coords), hitStatus_(coords.size(), false) {
	std::sort(coords_.begin(), coords_.end());
};

// Геттеры:
int Ship::getLength() { return coords_.size(); }
std::vector<Coordinate> Ship::getCoords() const { return coords_; }

// Методы:
// Проверка содердит ли корабль клетку coord
bool Ship::contains(const Coordinate& coord) const {
	for (const Coordinate& c : coords_) {
		if (c == coord) return true;
	}
	return false;
}

// Проверка потоплен ли корабль
bool Ship::isSunk() {
	for (const bool& flag : hitStatus_) {
		if (!flag) return false;
	}
	return true;
}

// Присваивает попадание в клекту с
void Ship::hit(const Coordinate& c) {
	for (int i = 0; i < coords_.size(); ++i) {
		if (coords_[i] == c) {
			hitStatus_[i] = true;
			return;
		}
	}
}

// =================== Класс Board ===================

// Конструкторы:
Board::Board() : field_(), ships_(), fieldSize_() {};
Board::Board(int size) : field_(size, std::vector<CellStatus>(size, CellStatus::Empty)), ships_(), fieldSize_(size) {};

// Геттеры:
std::vector<Ship> Board::getShips() const { return ships_; }

// Методы:
// Выставление корабля
void Board::placeShip(Ship ship) {

	for (Coordinate coords : ship.getCoords()) {
		if (field_[coords.getRow()][coords.getColumn()] != CellStatus::Empty) throw PlacementError();
	}
	for (Coordinate coords : ship.getCoords()) {
		field_[coords.getRow()][coords.getColumn()] = CellStatus::Ship;
	}
	ships_.push_back(ship);
}

// Очистка игрового поля
void Board::clear() {
	ships_.clear();
	for (int i = 0; i < fieldSize_; i++)
		for (int j = 0; j < fieldSize_; j++)
			field_[i][j] = CellStatus::Empty;
}

// Проверка все ли корабли потоплены
bool Board::allSunk() {
	for (Ship ship : ships_)
		if (!ship.isSunk()) return false;
	return true;
}

// Проверка стреляли ли уже в точку с
bool Board::alreadyShot(const Coordinate& c) const {
	CellStatus s = cellStatusAt(c);
	return s == CellStatus::Miss || s == CellStatus::Hit || s == CellStatus::Killed;
}

// Определение статуса точки coord
CellStatus Board::cellStatusAt(const Coordinate& coord) const { return field_[coord.getRow()][coord.getColumn()]; }

// Результат выстрела по клетке
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