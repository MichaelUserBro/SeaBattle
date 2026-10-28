#include "Player.h"

// =================== Класс Player ===================

Player::Player(std::string name, const Rules& rules) : name_(name), board_(Board(rules.getFieldSize())){}

const std::string& Player::getName() const { // Проверить можно ли использовать другой тип данных
	return name_;
}
Board& Player::getBoard() {
	return board_;
}
const Board& Player::getBoard() const {
	return board_;
}

// =================== Класс HumanPlayer ===================

HumanPlayer::HumanPlayer(std::string name, const Rules& rules, InputProvider* input)
	: Player(name, rules), input_(input){
	if (input_ == nullptr)
		throw std::invalid_argument("Ошибка!");
}

bool HumanPlayer::placeFleet(const Rules& rules) {
	getBoard().clear();
	return input_->requestPlacement(getBoard(), rules);

}

std::optional<Coordinate> HumanPlayer::chooseShot() {
	return input_->requestShot();
}

void HumanPlayer::onShotResult(Coordinate c, const ShotResult& result) {

}

// =================== Класс ComputerPlayer ===================

ComputerPlayer::ComputerPlayer(std::string name, const Rules& rules,
	std::unique_ptr<ShootingStrategy> strategy,
	std::unique_ptr<FleetPlacer> placer) : Player(name,rules), 
	strategy_(std::move(strategy)), placer_(std::move(placer)){
	if (strategy_ == nullptr) throw std::invalid_argument("Ошибка!");
	if (placer_ == nullptr) throw std::invalid_argument("Ошибка!");
}

bool ComputerPlayer::placeFleet(const Rules& rules) {
	getBoard().clear();
	placer_->place(getBoard(), rules);
	return true;
}

std::optional<Coordinate> ComputerPlayer::chooseShot() {
	return strategy_->nextShot();
}

void ComputerPlayer::onShotResult(Coordinate c, const ShotResult& result) {
	strategy_->onResult(c, result);
}