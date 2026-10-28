#pragma once
#include "Core.h"
#include "Placement.h"   
#include "Strategy.h"    


class InputProvider {
public:
    virtual ~InputProvider() = default;
    virtual std::optional<Coordinate> requestShot() = 0;
    virtual bool requestPlacement(Board& board, const Rules& rules) = 0;
};

class Player {
    private:
        std::string name_;
        Board board_;
    public:
        Player(std::string name, const Rules& rules);
        virtual ~Player() = default;

        const std::string& getName() const;
        Board& getBoard();
        const Board& getBoard() const;

        virtual bool placeFleet(const Rules& rules) = 0;
        virtual std::optional<Coordinate> chooseShot() = 0;
        virtual void onShotResult(Coordinate c, const ShotResult& result) = 0;

        


};

class HumanPlayer : public Player {
    private:
        InputProvider* input_;
    public:
        HumanPlayer(std::string name, const Rules& rules, InputProvider* input);

        bool placeFleet(const Rules& rules) override;
        std::optional<Coordinate> chooseShot() override;
        void onShotResult(Coordinate c, const ShotResult& result) override;

    
};

class ComputerPlayer : public Player {
    private:
        std::unique_ptr<ShootingStrategy> strategy_;
        std::unique_ptr<FleetPlacer> placer_;
    public:
        ComputerPlayer(std::string name, const Rules& rules,
            std::unique_ptr<ShootingStrategy> strategy,
            std::unique_ptr<FleetPlacer> placer);

        bool placeFleet(const Rules& rules) override;
        std::optional<Coordinate> chooseShot() override;
        void onShotResult(Coordinate c, const ShotResult& result) override;

    
};