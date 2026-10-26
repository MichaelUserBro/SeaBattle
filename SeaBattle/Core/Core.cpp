#pragma once
#include "Core.h"
#include "Error.h"



// =================== Класс Rules ===================

int Rules::getFieldSize() const { return fieldSize_; }

// =================== Класс Coordinate ===================

Coordinate::Coordinate() : row(0), column(0) {};
Coordinate::Coordinate(int r, int c) : row(r), column(c) {};

int Coordinate::getRow() const { return row; }
int Coordinate::getColumn() const { return column; }


//Парсес - проверяет ввод пользователя и превращает его в координату
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

bool Coordinate::operator==(const Coordinate& other) const {
	return row == other.row && column == other.column;
}

// =================== Класс Ship ===================

Ship::Ship() : coords_(), hitStatus_() {};
Ship::Ship(const std::vector<Coordinate>& coords) : coords_(coords), hitStatus_(coords.size(), false) {};

int Ship::getLength() { return coords_.size(); }
std::vector<Coordinate> Ship::getCoords() { return coords_; }


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
CellStatus Board::cellStatusAt(Coordinate coord) { return field_[coord.getRow()][coord.getColumn()]; }
bool Board::allSunk() {
	for (Ship ship : ships_)
		if (!ship.isSunk()) return false;
	return true;
}