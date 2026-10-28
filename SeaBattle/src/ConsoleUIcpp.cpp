#include "ConsoleUI.h"

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

ConsoleInput::ConsoleInput(const Rules& rules, std::istream& in, std::ostream& out)
    : rules_(rules), in_(in), out_(out) {
}

std::optional<Coordinate> ConsoleInput::requestShot() {
    return std::nullopt;
}

bool ConsoleInput::requestPlacement(Board& board, const Rules& rules) {
    return false;
}

