#pragma once

#include <model.hxx>

class GameUpdater {
    public:
        GameUpdater(Tube tube);
        void tick(double dt, Balls & balls, GameState & state);
        void randomizeBall(Ball & ball);
    private:
        Tube tube_;
        unsigned int seed_;
};
