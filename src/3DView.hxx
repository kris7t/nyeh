#pragma once

#include "model.hxx"

typedef void (* BallRenderer)(const Ball &);

class ThreeDView {
    public:
        ThreeDView(int width, int height);
        ~ThreeDView();
        void render(const Balls & balls);
};
