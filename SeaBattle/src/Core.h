#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <optional>

enum class Orientation {Horizontal, Vertical};
enum class CellStatus {Empty, Ship, Miss, Hit, Killed}; // статус клетки
enum class ShotStatus {Miss, Hit, Killed}; // статус выстрела
const std::string ENG_ALF = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

class Rules {
	private:
		int fieldSize_;			   // размер поля
		bool allowTouching_;	   // разрешено ли касание
		bool isShooterContinuesAfterHit_;
		std::map<int, int> fleet_; // map<длинна корабля, количество кораблей>
	public:
		int getFieldSize() const;
		bool getAllowTouching() const;
		bool isContinue() const;
		std::map<int, int> getFleet() const; 
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
		bool inBounds(int size) const;
		std::vector<Coordinate> getNeighbors(int size) const;

		//Операторы

		bool operator==(const Coordinate& other) const;
		bool operator<(const Coordinate& other) const;
};

class Ship {
	private: 
		std::vector<Coordinate>  coords_;
		std::vector<bool> hitStatus_;

	public:
		Ship();
		Ship(const std::vector<Coordinate>& coords);

		int getLength();
		std::vector<Coordinate> getCoords() const;
		bool contains(const Coordinate& coord) const;
		bool isSunk();
		void hit(const Coordinate& c);


};

// Пустая хуйня (заглушка)
struct Move
{

};

struct ShotResult {
	ShotStatus outcome;
	std::optional<Ship> sunkShip;
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
		bool allSunk();
		bool alreadyShot(const Coordinate& c) const;
		CellStatus cellStatusAt(const Coordinate& coord) const;
		ShotResult receiveShot(const Coordinate& c);
		

};




Ship makeShip(Coordinate start, int length, Orientation orient);

