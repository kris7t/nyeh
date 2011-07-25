#pragma once

#include "model.hxx"

class HudRenderer {
    public:
        HudRenderer(const Tube & tube, cv::Size size);
        ~HudRenderer();

        void init();
        void dispose();

        void upload(const cv::Mat & frame);
        void renderScore(const GameState & state) const;

    private:
        Tube tube_;
        cv::Size size_, imgsize_;
        GLuint texture_[2];
};
