#include <iostream>
#include <memory>
#include <exception>
#include <optional>
#include <string>

#include "Core.h"
#include "Placement.h"
#include "Strategy.h"
#include "Player.h"
#include "Game.h"
#include "ConsoleUI.h"
// TODO (Files): #include "Files.h"
// TODO (SFML):  #include "SfmlUI.h"

#ifdef _WIN32
#include <windows.h>
#endif

static void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(1251);   // вывод
    SetConsoleCP(1251);         // ввод
#endif
}

// Читает номер пункта меню из диапазона [lo, hi].
// Возвращает nullopt только при конце ввода (EOF).
static std::optional<int> readChoice(int lo, int hi) {
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line))
            return std::nullopt;
        try {
            size_t pos = 0;
            int value = std::stoi(line, &pos);
            if (line.find_first_not_of(" \t\r", pos) == std::string::npos
                && value >= lo && value <= hi)
                return value;
        }
        catch (const std::exception&) {
            // не число или слишком большое: сообщение ниже
        }
        std::cout << "Введите число от " << lo << " до " << hi << ".\n";
    }
}

static std::optional<Difficulty> readDifficulty() {
    std::cout << "\nСложность компьютера:\n"
        << "1. Лёгкая\n"
        << "2. Средняя\n"
        << "3. Сложная\n"
        << "0. Назад\n";
    auto c = readChoice(0, 3);
    if (!c || *c == 0)
        return std::nullopt;
    switch (*c) {
    case 1:  return Difficulty::Easy;
    case 2:  return Difficulty::Medium;
    default: return Difficulty::Hard;
    }
}

static void playNewGame() {
    auto difficulty = readDifficulty();
    if (!difficulty)
        return;

    Rules rules;                       // классические правила
    ConsoleInput input(rules);

    auto human = std::make_unique<HumanPlayer>("Игрок", rules, &input);
    auto computer = std::make_unique<ComputerPlayer>(
        "Компьютер", rules,
        createStrategy(*difficulty, rules),
        std::make_unique<RandomFleetPlacer>());

    Game game(rules, std::move(human), std::move(computer));

    ConsoleRenderer renderer(game);
    game.addObserver(&renderer);       // до setup(), иначе onGameStarted не придёт
    // TODO (Files): MoveLogger, StatsStorage подписать здесь же, до setup()

    if (!game.setup()) {
        std::cout << "Расстановка отменена.\n";
        return;
    }

    while (!game.isFinished())
        game.step();

    std::cout << "\nПобедитель: " << game.getPlayer(game.getWinner()).getName() << "\n";
    // TODO (Files): ResultSaver, сохранить итоговые карты
}

static void loadGame() {
    // TODO (Files): GameSaver::load и продолжение партии
    std::cout << "Загрузка партии пока не реализована.\n";
}

static void showStats() {
    // TODO (Files): StatsStorage
    std::cout << "Статистика пока не реализована.\n";
}

int main() {
    setupConsole();

    while (true) {
        std::cout << "\n=== МОРСКОЙ БОЙ ===\n"
            << "1. Новая игра\n"
            << "2. Загрузить партию\n"
            << "3. Статистика\n"
            << "0. Выход\n";

        auto choice = readChoice(0, 3);
        if (!choice || *choice == 0)
            break;

        try {
            switch (*choice) {
            case 1: playNewGame(); break;
            case 2: loadGame();    break;
            case 3: showStats();   break;
            }
        }
        catch (const std::exception& e) {
            // партия прервана, но программа продолжает работать
            std::cout << "\nИгра прервана: " << e.what() << '\n';
        }
    }
    return 0;
}