#pragma once
#include "Core.h"
#include "Error.h"



// =================== Класс Rules ===================

int Rules::getFieldSize() const { return fieldSize; }

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