#pragma once
#include <string>
#include <vector>

enum class Orientation {Horizontal, Vertical};
enum class CellStatus {Empty, Ship, Miss, Hit, Killed}; // статус клетки
enum class ShotStatus {Miss, Hit, Killed}; // статус выстрела
const std::string ENG_ALF = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";


class Rules {
	private:
		int fieldSize_;
	public:
		int getFieldSize() const;
};

class Coordinate {
	private:
		int row; // Буква (сбоку)(строка)
		int column; // Цифра (сверху)(столбец)
	public:
		//Конструкторы
		Coordinate();
		Coordinate(int r, int c);

		//Геттеры
		int getRow() const;
		int getColumn() const;

		//Методы
		static Coordinate parse(const std::string& text, int fieldSize);

		//Операторы

		bool operator==(const Coordinate& other) const;
};

class Ship {
	private: 
		std::vector<Coordinate>  coords_;
		std::vector<bool> hitStatus_;

	public:
		Ship();
		Ship(const std::vector<Coordinate>& coords);

		int getLength();
		std::vector<Coordinate> getCoords();
		bool contains(const Coordinate& coord) const;
		bool isSunk();



};

class Board {
	private:
		std::vector<std::vector<CellStatus>> field_;
		std::vector<Ship> ships_;
		int fieldSize_;

	public:
		Board();
		Board(int size);

		std::vector<Ship> getShips() const;

		void placeShip(Ship ship);
		void clear();
		CellStatus cellStatusAt(Coordinate coord);
		bool allSunk();

};