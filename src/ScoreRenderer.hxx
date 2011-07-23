#pragma once

#include "model.hxx"

class ScoreRenderer {
    public:
        ScoreRenderer();
        ScoreRenderer(const Tube & tube, cv::Size size);
        ScoreRenderer(const ScoreRenderer & obj);
        const ScoreRenderer & operator =(const ScoreRenderer & obj);
        ~ScoreRenderer();
        void renderScore(const GameState & state) const;

    private:
        Tube tube_;
        cv::Size size_;
        mutable GLuint texture_;
};
