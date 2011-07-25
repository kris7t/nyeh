#pragma once

#include "HudRenderer.hxx"
#include "CamRenderer.hxx"
#include "model.hxx"
#include "HandToModel.hxx"

typedef void (* BallRenderer)(const Ball &);

class ThreeDView {
    public:
        ThreeDView(cv::Size size, Tube tube);
        ~ThreeDView();
    void render(const Balls & balls, HandToModel_ hand, const GameState & state,
                const cv::Mat& frame, const cv::Mat & ownframe);
    private:
        Tube tube_;
        HudRenderer hudRenderer_;
        CamRenderer camRenderer_;
};
