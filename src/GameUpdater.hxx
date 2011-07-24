#pragma once

#include <model.hxx>

class GameUpdater {
    public:
        GameUpdater(Tube tube);
        void tick(double dt, Balls & balls, GameState & state) const;
    private:
        Tube tube_;
};
