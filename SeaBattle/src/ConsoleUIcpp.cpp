#include <sstream>
#include "ConsoleUI.h"
#include "Placement.h"

// =================== Класс ConsoleRenderer ===================


char ConsoleRenderer::cellSymbol(CellStatus state, bool hideShips) {
    switch (state) {
    case CellStatus::Empty: return '.';
    case CellStatus::Ship: return hideShips ? '.' : '#';
    case CellStatus::Miss: return 'O';
    case CellStatus::Hit: return 'X';
    case CellStatus::Killed: return 'K';
    }
}

std::string ConsoleRenderer::renderHeader() const {
    const int size = game_.getRules().getFieldSize();
    std::string line = "   ";
    for (int col = 0; col < size; ++col) {
        if (col > 0) line += ' ';
        line += ENG_ALF.at(col);
    }
    return line;
}

std::string ConsoleRenderer::renderRow(const Board& board, int row, bool hideShips) const {
    const int size = game_.getRules().getFieldSize();
    std::string label = std::to_string(row + 1);
    if (label.size() < 2) label = " " + label;

    std::string line = label + " ";
    for (int col = 0; col < size; ++col) {
        if (col > 0) line += ' ';
        line += cellSymbol(board.cellStatusAt(Coordinate(row, col)), hideShips);
    }
    return line;
}

std::string ConsoleRenderer::outcomeText(ShotStatus outcome) {
    switch (outcome) {
    case ShotStatus::Miss:   return "Мимо!";
    case ShotStatus::Hit:    return "Попадание!";
    case ShotStatus::Killed: return "Потоплен!";
    }
    return "?";
}

void ConsoleRenderer::printBoards(bool revealAll) {
    const int size = game_.getRules().getFieldSize();
    const std::string gap = "    ";
    const std::string header = renderHeader();

    out_ << "Ваше поле";
    out_ << std::string(header.size() + gap.size() - 9, ' ');   
    out_ << "Поле противника" << '\n';

    out_ << header << gap << header << '\n';   

    for (int r = 0; r < size; ++r) {
        out_ << renderRow(game_.getPlayer(0).getBoard(), r, false)
            << gap
            << renderRow(game_.getPlayer(1).getBoard(), r, !revealAll) << '\n';
    }
}

ConsoleRenderer::ConsoleRenderer(const Game& game, std::ostream& out)
    : game_(game), out_(out) {
}

void ConsoleRenderer::onGameStarted() {
    out_ << "=== Игра началась ===\n\n";
    printBoards(false);
    out_ << '\n';
}

void ConsoleRenderer::onShot(int playerIndex, Coordinate coord, const ShotResult& result) {
    if (playerIndex == 0)
        out_ << "Вы стреляете в " << coord.toString() << ": ";
    else
        out_ << "Противник стреляет в " << coord.toString() << ": ";
    out_ << outcomeText(result.outcome) << '\n';
    printBoards(false);
    out_ << '\n';
}

void ConsoleRenderer::onGameFinished(int winnerIndex) {
    out_ << "=== Игра окончена ===\n";
    if (winnerIndex == 0)
        out_ << "Победа! Вы потопили все корабли противника.\n\n";
    else
        out_ << "Поражение. Противник потопил все ваши корабли.\n\n";

    printBoards(true);

    out_ << "\nВсего ходов: " << game_.getHistory().size() << '\n';
}



// =================== Класс ConsoleInput ===================

bool ConsoleInput::placeRandomly(Board& board, const Rules& rules) {
    RandomFleetPlacer placer;
    placer.place(board, rules);
    return true;
}

bool ConsoleInput::placeManually(Board& board, const Rules& rules) {
    board.clear();
    std::map<int, int> left = rules.getFleet();   // подставь своё имя поля/геттера
    int remaining = 0;
    for (const auto& p : left) remaining += p.second;

    out_ << "Формат: <клетка> <длина> <H|V>, например A1 4 H. Выход: q\n";

    while (remaining > 0) {
        out_ << "Осталось:";
        for (const auto& p : left)
            if (p.second > 0) out_ << ' ' << p.second << "x" << p.first;
        out_ << "\n> ";

        std::string line;
        if (!std::getline(in_, line))
            throw std::runtime_error("Ввод завершён (EOF)");
        if (line == "q" || line == "Q") {
            board.clear();
            return false;
        }

        std::istringstream iss(line);
        std::string cellText, dirText;
        int len = 0;
        if (!(iss >> cellText >> len >> dirText)) {
            out_ << "Ошибка: ожидается формат вроде A1 4 H.\n";
            continue;
        }

        Orientation orient;
        if (dirText == "H" || dirText == "h")      orient = Orientation::Horizontal;
        else if (dirText == "V" || dirText == "v") orient = Orientation::Vertical;
        else {
            out_ << "Ошибка: направление H или V.\n";
            continue;
        }

        auto it = left.find(len);
        if (it == left.end() || it->second == 0) {
            out_ << "Ошибка: кораблей такой длины не осталось.\n";
            continue;
        }

        try {
            Coordinate start = Coordinate::parse(cellText, rules.getFieldSize());
            Ship ship = makeShip(start, len, orient);
            if (!PlacementValidator::canPlace(board, ship, rules)) {
                out_ << "Ошибка: сюда корабль поставить нельзя "
                    "(выход за поле, наложение или касание).\n";
                continue;
            }
            board.placeShip(ship);
            --it->second;
            --remaining;
        }
        catch (const std::exception& e) {
            out_ << "Ошибка: " << e.what() << '\n';
        }
    }
    return true;
}

ConsoleInput::ConsoleInput(const Rules& rules, std::istream& in, std::ostream& out)
    : rules_(rules), in_(in), out_(out) {
}

std::optional<Coordinate> ConsoleInput::requestShot() {
    while (true) {
        out_ << "Ваш выстрел (например, B5): ";

        std::string line;
        if (!std::getline(in_, line)) {
            throw std::runtime_error("Ввод завершён (EOF)");
        }

        try {
            return Coordinate::parse(line, rules_.getFieldSize());
        }
        catch (const InvalidInputError& e) {
            out_ << "Ошибка: " << e.what() << ". Попробуйте ещё раз.\n";
        }
    }
}

bool ConsoleInput::requestPlacement(Board& board, const Rules& rules) {
    while (true) {
        out_ << "Как расставить корабли?\n"
            << "  1 - вручную\n"
            << "  2 - случайно\n"
            << "  3 - из файла\n"
            << "Ваш выбор: ";

        std::string line;
        if (!std::getline(in_, line)) {
            throw std::runtime_error("Ввод завершён (EOF)");
        }

        if (line == "2") {
            return placeRandomly(board, rules);
        }
        if (line == "1" || line == "3") {
            out_ << "Этот вариант пока не реализован.\n";
            continue;
        }
        out_ << "Ошибка: введите 1, 2 или 3.\n";
    }
}

