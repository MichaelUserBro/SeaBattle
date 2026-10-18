#pragma once
#include <string>

enum class Orientation {Horizontal, Vertical};
enum class CellStatus {Empty, Ship, Miss, Hit, Killed};
enum class ShotStatus {Miss, Hit, Killed};
const std::string ENG_ALF = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";


class Rules {
	private:
		int fieldSize;
	public:
		int getFieldSize() const;
};

class Coordinate {
	private:
		int row; // Буква (сбоку)
		int column; // Цифра (сверху)
	public:
		//Конструкторы
		Coordinate();
		Coordinate(int r, int c);

		//Геттеры
		int getRow() const;
		int getColumn() const;

		//Методы
		static Coordinate parse(const std::string& text, int fieldSize);
};