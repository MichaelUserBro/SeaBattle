#pragma once
#include <string>
#include <vector>
#include <map>
#include <optional>

enum class Orientation {Horizontal, Vertical}; // ориентация корабля
enum class CellStatus {Empty, Ship, Miss, Hit, Killed}; // статус клетки
enum class ShotStatus {Miss, Hit, Killed}; // статус выстрела
const std::string ENG_ALF = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // набор всех латинских букв

struct Rules {
	int fieldSize;					 // размер поля
	bool allowTouching;				 // разрешено ли касание
	bool isShooterContinuesAfterHit; // продолжение стрельбы при попадании
	std::map<int, int> fleet;		 // map<длинна корабля, количество кораблей>

	Rules();
	Rules(int newFieldSize, bool newAllowTouching, bool newContinueShoot, std::map<int,int> newFleet);

};

class Coordinate {
	private:
		int row_;	 // Буква (сбоку)(строка)
		int column_; // Цифра (сверху)(столбец)
	public:
		//Конструкторы:
		Coordinate();
		Coordinate(int r, int c);

		//Геттеры:
		int getRow() const;
		int getColumn() const;

		//Методы:
		// Парсинг пользовательского ввода и преобразование его в координату
		static Coordinate parse(const std::string& text, int fieldSize);
		// Проверка находится ли точка внутри границ поля
		bool inBounds(int size) const;
		// Создаёт вектор всех соседей
		std::vector<Coordinate> getNeighbors(int size) const;
		// Превращает значение координат в строку
		std::string toString() const;

		//Операторы:
		bool operator==(const Coordinate& other) const;
		bool operator<(const Coordinate& other) const;
};

class Ship {
	private: 
		std::vector<Coordinate>  coords_;
		std::vector<bool> hitStatus_;

	public:
		// Конструкторы:
		Ship();
		Ship(const std::vector<Coordinate>& coords);

		// Геттеры:
		int getLength();
		std::vector<Coordinate> getCoords() const;

		// Методы:
		// Проверка содердит ли корабль клетку coord
		bool contains(const Coordinate& coord) const;
		// Проверка потоплен ли корабль
		bool isSunk();
		// Присваивает попадание в клекту с
		void hit(const Coordinate& c);


};

struct Move
{
	int playerIndex;    // Кто стрелял
	Coordinate coord;   // Куда стрелял
	ShotStatus outcome; // Какой резульат
};

struct ShotResult {
	ShotStatus outcome;           // Результат выстрела
	std::optional<Ship> sunkShip; // Потопленный корабль (если потопили)
};

class Board {
	private:
		std::vector<std::vector<CellStatus>> field_; // Поле
		std::vector<Ship> ships_;					 // Корабли
		int fieldSize_;								 // Размер

	public:
		// Конструкторы:
		Board();
		Board(int size);

		// Геттеры:
		std::vector<Ship> getShips() const;

		// Методы:
		// Выставление корабля
		void placeShip(Ship ship);
		// Очистка игрового поля
		void clear();
		// Проверка все ли корабли потоплены
		bool allSunk();
		// Проверка стреляли ли уже в точку с
		bool alreadyShot(const Coordinate& c) const;
		// Определение статуса точки coord
		CellStatus cellStatusAt(const Coordinate& coord) const;
		// Результат выстрела по клетке
		ShotResult receiveShot(const Coordinate& c);
		

};

// Создание корабля
Ship makeShip(Coordinate start, int length, Orientation orient);