#pragma once
#include <iostream>
#include <optional>
#include "Core.h"
#include "Player.h"
#include "Game.h"


class ConsoleRenderer : public GameObserver {
	private:
		const Game& game_;
		std::ostream& out_;

		static char cellSymbol(CellStatus state, bool hideShips);
		std::string renderHeader() const;
		std::string renderRow(const Board& board, int row, bool hideShips) const;
		void printBoards(bool revealAll);
		static std::string outcomeText(ShotStatus outcome);
	public:
		ConsoleRenderer(const Game& game, std::ostream& out = std::cout);

		void onGameStarted() override;
		void onShot(int playerIndex, Coordinate coord, const ShotResult& result) override;
		void onGameFinished(int winnerIndex) override;
		void onInvalidInput(const std::string& message) override;
};

class ConsoleInput : public InputProvider {
	private:
		const Rules& rules_;
		std::istream& in_;
		std::ostream& out_;

		bool placeRandomly(Board& board, const Rules& rules);
		bool placeManually(Board& board, const Rules& rules);
	public:
		ConsoleInput(const Rules& rules, std::istream& in = std::cin, std::ostream& out = std::cout);

		std::optional<Coordinate> requestShot() override;
		bool requestPlacement(Board&, const Rules&);



};