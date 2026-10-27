#pragma once
#include "Core.h"
#include "Error.h"
#include "Player.h"
#include <stdexcept>
#include <memory>
#include <array>

enum class GameState {Placement, Playing, Finished};

class GameObserver {
	public:
		virtual ~GameObserver() = 0;

		// Функция вызываемая 1 раз в начале игры
		virtual void onGameStarted() {};
		//
		virtual void onShot(int playerIndex, Coordinate coord, ShotStatus result) {}
		// Функция вызываемая по окончании игры
		virtual void onGameFinished(int winnerIndex) {}
		// 
		virtual void onInvalidInput(const std::string& message) {}




};

class Game {
	private:
		Rules rules_;
		std::array<std::unique_ptr<Player>, 2> players_;
		std::vector<GameObserver*> observers_;
		std::vector<Move> history_;
		int currentPlayer_;
		GameState currentState_;
		int winner = -1;

	public:
		Game(Rules rules, std::unique_ptr<Player> p0, std::unique_ptr<Player> p1);

		void addObserver(GameObserver* obs);
		bool setup(); // НЕДОДЕЛАНО
};