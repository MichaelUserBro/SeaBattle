#include "Error.h"

InvalidInputError::InvalidInputError()
    : std::runtime_error("Ошибка ввода!") {}

InvalidInputError::InvalidInputError(const std::string& details)
    : std::runtime_error("Ошибка ввода: " + details) {}

//==============================================================

PlacementError::PlacementError()
    : std::runtime_error("Ошибка расстановки!") {}

PlacementError::PlacementError(const std::string& details)
    : std::runtime_error("Ошибка расстановки: " + details) {}

//==============================================================


FileFormatError::FileFormatError()
    : std::runtime_error("Ошибка работы с файлом!") {}

FileFormatError::FileFormatError(const std::string& details)
    : std::runtime_error("Ошибка работы с файлом: " + details) {}

//==============================================================


GameError::GameError()
    : std::runtime_error("Ошибка, нарушены правила игры!") {}

GameError::GameError(const std::string& details)
    : std::runtime_error("Ошибка, нарушены правила игры: " + details) {}

//==============================================================


GameStateError::GameStateError()
    : std::runtime_error("Ошибка статуса партии!") {}

GameStateError::GameStateError(const std::string& details)
    : std::runtime_error("Ошибка статуса партии: " + details) {}

