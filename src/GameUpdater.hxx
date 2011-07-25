#pragma once

#include <model.hxx>
#include <HandToModel.hxx>

class GameUpdater {
    public:
        GameUpdater(Tube tube);
        void tick(double dt, Balls & balls, GameState & state, HandToModel_ hand);
        void randomizeBall(Ball & ball);
    private:
        Tube tube_;
        unsigned int seed_;
};
