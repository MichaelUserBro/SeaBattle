#pragma once
#include "core.h"
#include <random>
#include <memory>
#include <vector>
#include<deque>

enum class Difficulty {Easy,Medium,Hard};

class ShootingStrategy {
	protected:
		Rules rules_;
		std::mt19937 rng_;
		std::vector<std::vector<CellStatus>> knowledge_;

		ShootingStrategy(const Rules& rules, unsigned seed);
	public:

		virtual ~ShootingStrategy() = default;
		virtual Coordinate nextShot() = 0;
		virtual void onResult(Coordinate c, ShotResult r) = 0;

		bool isUnfired(Coordinate c) const;
		void recordResult(Coordinate c, const ShotResult& r);
		std::vector<Coordinate> emptyCells() const;
		
};

class RandomStrategy : public ShootingStrategy {
	private:
		std::vector<Coordinate> order_;
		int next_ = 0;
	public:
		RandomStrategy(const Rules& rules, unsigned seed = 0);
		Coordinate nextShot() override;
		void onResult(Coordinate c, ShotResult r) override;
};

// «аглушка
class HuntTargetStrategy : public ShootingStrategy {
public:
	explicit HuntTargetStrategy(const Rules& rules, unsigned seed = 0);
	Coordinate nextShot() override;
	void onResult(Coordinate c, ShotResult r) override;

private:
	std::vector<Coordinate> huntOrder_;   // пор€док поиска (шахматный)
	std::size_t huntPos_ = 0;
	std::deque<Coordinate> targets_;      // кандидаты в режиме добивани€
	std::vector<Coordinate> activeHits_;  // попадани€ по непотопленному кораблю
};
// «аглушка
class ProbabilityStrategy : public ShootingStrategy {
public:
	explicit ProbabilityStrategy(const Rules& rules, unsigned seed = 0);
	Coordinate nextShot() override;
	void onResult(Coordinate c, ShotResult r) override;

private:
	void computeWeights();                // TODO: подсчЄт весов

	std::vector<int> remaining_;          // длины непотопленных кораблей
	std::vector<std::vector<int>> weights_;
};


std::unique_ptr<ShootingStrategy> createStrategy(Difficulty difficulty,
	const Rules& rules,
	unsigned seed = std::random_device{}());