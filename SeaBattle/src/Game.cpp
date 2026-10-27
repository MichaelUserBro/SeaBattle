#include "Game.h"

// =================== Класс GameObserver ===================

GameObserver::~GameObserver() = default;

// =================== Класс Game ===================

Game::Game(Rules rules, std::unique_ptr<Player> p0, std::unique_ptr<Player> p1) {
	if (p0 == nullptr || p1 == nullptr) throw std::invalid_argument("Указатель на игрока не может быть пустым!");
	currentState_ = GameState::Placement;
	currentPlayer_ = 0;
	rules_ = rules;
	players_[0] = std::move(p0);
	players_[1] = std::move(p1);



}

void Game::addObserver(GameObserver* obs) {
	if (obs == nullptr) throw std::invalid_argument("Пустой указатель на наблюдателя!");
	for(GameObserver* o: observers_)
		if (o == obs) 
			throw std::invalid_argument("Такой наблюдатель уже существует!");
	observers_.push_back(obs);
}
//НЕДОДЕЛАНО
bool Game::setup() {
	if (currentState_ != GameState::Placement) throw GameStateError::GameStateError();
}