#pragma once

#include "ScoreRenderer.hxx"
#include "model.hxx"
#include "Hand.hxx"

typedef void (* BallRenderer)(const Ball &);

class ThreeDView {
    public:
        ThreeDView(cv::Size size, Tube tube);
        ~ThreeDView();
        void render(const Balls & balls, Hand_ hand, const GameState & state) const;
    private:
        Tube tube_;
        ScoreRenderer scoreRenderer_;
};
