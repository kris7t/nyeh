#pragma once

#include <model.hxx>

class GameUpdater {
    public:
        GameUpdater(cv::Size2f tubeDimsHalf);
        void tick(double dt, Balls & balls) const;
    private:
        cv::Size2f tubeDimsHalf_;
};
