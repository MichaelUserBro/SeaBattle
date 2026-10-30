#include "Strategy.h"
#include "Core.h"
#include "Error.h"

// =================== Класс ShootingStrategy ===================

ShootingStrategy::ShootingStrategy(const Rules& rules, unsigned seed)
    : rules_(rules),
    knowledge_(rules.fieldSize, std::vector<CellStatus>(rules.fieldSize, CellStatus::Empty)),
    rng_(seed != 0 ? seed : std::random_device{}())
{
}

bool ShootingStrategy::isUnfired(Coordinate c) const{
    return c.inBounds(rules_.fieldSize) && knowledge_[c.getRow()][c.getColumn()] == CellStatus::Empty;
}
void ShootingStrategy::recordResult(Coordinate c,const ShotResult& r) {
    if(r.outcome == ShotStatus::Miss) knowledge_[c.getRow()][c.getColumn()] = CellStatus::Miss;
    else if(r.outcome == ShotStatus::Hit) knowledge_[c.getRow()][c.getColumn()] = CellStatus::Hit;
    else {
        for (const Coordinate& cell : r.sunkShip->getCoords()) {
            knowledge_[cell.getRow()][cell.getColumn()] = CellStatus::Killed;
            if (!rules_.allowTouching) {
                for (const Coordinate& n : cell.getNeighbors(rules_.fieldSize))
                    if (knowledge_[n.getRow()][n.getColumn()] == CellStatus::Empty)
                        knowledge_[n.getRow()][n.getColumn()] = CellStatus::Miss;
            }
        }
            

    }
}
std::vector<Coordinate> ShootingStrategy::emptyCells() const {
    std::vector<Coordinate> result;
    for (int r = 0; r < rules_.fieldSize; ++r)
        for (int c = 0; c < rules_.fieldSize; ++c)
            if (knowledge_[r][c] == CellStatus::Empty)
                result.push_back(Coordinate{ r, c });
    return result;
}

// =================== Класс RandomStrategy ===================

RandomStrategy::RandomStrategy(const Rules& rules, unsigned seed)
    : ShootingStrategy(rules, seed) {
    order_.reserve(rules_.fieldSize * rules_.fieldSize);
    for (int r = 0; r < rules_.fieldSize; ++r)
        for (int c = 0; c < rules_.fieldSize; ++c)
            order_.push_back(Coordinate{ r, c });
    std::shuffle(order_.begin(), order_.end(), rng_);
}

Coordinate RandomStrategy::nextShot() {
    if (next_ >= static_cast<int>(order_.size()))
        throw GameStateError("RandomStrategy: все клетки уже обстреляны");
    return order_[next_++];
}

void RandomStrategy::onResult(Coordinate c, ShotResult r) {
    recordResult(c, r);
}



std::unique_ptr<ShootingStrategy> createStrategy(Difficulty d, const Rules& rules, unsigned seed) {
    switch (d) {
    case Difficulty::Easy:   return std::make_unique<RandomStrategy>(rules, seed);
    case Difficulty::Medium: return std::make_unique<HuntTargetStrategy>(rules, seed);
    case Difficulty::Hard:   return std::make_unique<ProbabilityStrategy>(rules, seed);
    }
    throw std::invalid_argument("createStrategy: неизвестный уровень сложности");
}

// =================== ИИ хрючево ===================

// ---------- HuntTargetStrategy (заглушка) ----------

HuntTargetStrategy::HuntTargetStrategy(const Rules& rules, unsigned seed)
    : ShootingStrategy(rules, seed) {
    // TODO: заполнить huntOrder_ (сначала клетки с (row+col)%2==0, потом остальные,
    //       каждая группа перемешана через rng_)
}

Coordinate HuntTargetStrategy::nextShot() {
    // TODO: режимы Target (targets_) и Hunt (huntOrder_)
    // Пока: случайная неизвестная клетка
    auto cells = emptyCells();
    if (cells.empty())
        throw GameStateError("HuntTargetStrategy: нет клеток для выстрела");
    std::uniform_int_distribution<std::size_t> dist(0, cells.size() - 1);
    return cells[dist(rng_)];
}

void HuntTargetStrategy::onResult(Coordinate c, ShotResult r) {
    recordResult(c, r);
    // TODO: Hit -> activeHits_, targets_; Sunk -> очистка
}

// ---------- ProbabilityStrategy (заглушка) ----------

ProbabilityStrategy::ProbabilityStrategy(const Rules& rules, unsigned seed)
    : ShootingStrategy(rules, seed),
    weights_(rules.fieldSize, std::vector<int>(rules.fieldSize, 0)) {
    for (const auto& [length, count] : rules_.fleet)
        for (int i = 0; i < count; ++i)
            remaining_.push_back(length);
}

void ProbabilityStrategy::computeWeights() {
    // TODO: для каждой длины из remaining_, ориентации и позиции окна
    //       прибавить вклад к Empty-клеткам окна
}

Coordinate ProbabilityStrategy::nextShot() {
    // TODO: computeWeights(), выбор максимума (случайно среди равных)
    // Пока: случайная неизвестная клетка
    auto cells = emptyCells();
    if (cells.empty())
        throw GameStateError("ProbabilityStrategy: нет клеток для выстрела");
    std::uniform_int_distribution<std::size_t> dist(0, cells.size() - 1);
    return cells[dist(rng_)];
}

void ProbabilityStrategy::onResult(Coordinate c, ShotResult r) {
    recordResult(c, r);
    if (r.outcome == ShotStatus::Killed && r.sunkShip) {
        auto it = std::find(remaining_.begin(), remaining_.end(),
            static_cast<int>(r.sunkShip->getLength()));
        if (it != remaining_.end())
            remaining_.erase(it);
    }
}


