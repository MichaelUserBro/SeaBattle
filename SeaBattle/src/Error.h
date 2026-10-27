#pragma once
#include <stdexcept>
#include <string>

// Ошибка - пользователь ввёл некорректные данные
class InvalidInputError : public std::runtime_error {
public:
    InvalidInputError();
    explicit InvalidInputError(const std::string& details);
};


// Ошибка - нарушены правила расстановки
class PlacementError : public std::runtime_error {
public:
    PlacementError();
    explicit PlacementError(const std::string& details);
};

// Ошибка - файл не найден или имеет неверный формат
class FileFormatError : public std::runtime_error {
public:
    FileFormatError();
    explicit FileFormatError(const std::string& details);
};

// Ошибка - нарушены правила игры
class GameRuleError : public std::runtime_error {
public:
    GameRuleError();
    explicit GameRuleError(const std::string& details);
};

class GameStateError : public std::runtime_error {
public:
    GameStateError();
    explicit GameStateError(const std::string& details);
};

