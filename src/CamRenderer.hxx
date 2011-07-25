#pragma once

#include "model.hxx"

class CamRenderer {
public:
    CamRenderer(const Tube & tube);
    ~CamRenderer();

    void init();
    void dispose();
    void upload(const cv::Mat & frame);
    void render(bool blend, bool zbuf);

private:
    GLuint texid_;
    Tube tube_;
};
