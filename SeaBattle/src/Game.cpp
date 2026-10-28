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


int Game::getWinner() const {
	if (currentState_ != GameState::Finished)
		throw GameStateError("Партия ещё не закончена");
	return winner_;
}

GameState Game::getState() const { return currentState_; }

int Game::getCurrentPlayerIndex() const { return currentPlayer_; }

const std::vector<Move>& Game::getHistory() const { return history_; }

const Player& Game::getPlayer(int i) const {
	if (i < 0 || i > 1)
		throw std::out_of_range("Индекс игрока должен быть 0 или 1");
	return *players_[i];
}

Player& Game::getPlayer(int i) {
	if (i < 0 || i > 1)
		throw std::out_of_range("Индекс игрока должен быть 0 или 1");
	return *players_[i];
}

const Rules& Game::getRules() const { return rules_; }



void Game::addObserver(GameObserver* obs) {
	if (obs == nullptr) throw std::invalid_argument("Пустой указатель на наблюдателя!");
	for(GameObserver* o: observers_)
		if (o == obs) 
			throw std::invalid_argument("Такой наблюдатель уже существует!");
	observers_.push_back(obs);
}

bool Game::setup() {
	if (currentState_ != GameState::Placement) throw GameStateError();
	if(!players_[0]->placeFleet(rules_)) return false;
	players_[1]->placeFleet(rules_);

	for (auto& p : players_) {
		PlacementValidator::validate(p->getBoard().getShips(), rules_);
	}
	currentState_ = GameState::Playing;
	currentPlayer_ = 0;
	history_.clear();
	
	for (auto& obs : observers_) {
		obs->onGameStarted();
	}
}

bool Game::step() {
	if (currentState_ != GameState::Playing) throw GameStateError();

	Player& attacker = *players_[currentPlayer_];
	Player& defender = *players_[1 - currentPlayer_];

	auto shot = attacker.chooseShot();
	
	ShotResult result{};
	try {
		defender.getBoard().receiveShot(*shot);
	}
	catch(InvalidInputError e){
		for (auto obs : observers_) {
			obs->onInvalidInput(e.what());
		}
		return false;
	}

	// Тут позже будет добавления хода в историю

	attacker.onShotResult(*shot, result);

	for (GameObserver* obs : observers_)
		obs->onShot(currentPlayer_, *shot, result);

	if (defender.getBoard().allSunk()) {
		currentState_ = GameState::Finished;
		winner_ = currentPlayer_;
		for (GameObserver* obs : observers_)
			obs->onGameFinished(winner_);
		return true;
	}

	// Кто ходит дальше
	if (result.outcome == ShotStatus::Miss || !rules_.isContinue())
		currentPlayer_ = 1 - currentPlayer_;
	
	return true;

}

bool Game::isFinished() const {
	return currentState_ == GameState::Finished;
}

size_t Game::getTurnCount() const { return history_.size(); }