#include "ScoreRenderer.hxx"
#include "heart.inc.c"

ScoreRenderer::ScoreRenderer() : texture_(0) {
}

ScoreRenderer::ScoreRenderer(const Tube & tube, cv::Size size) : tube_(tube), size_(size) {
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, heart_image.width, heart_image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, heart_image.pixel_data);
}

ScoreRenderer::~ScoreRenderer() {
    if (texture_) {
        glDeleteTextures(1, &texture_);
    }
}

ScoreRenderer::ScoreRenderer(const ScoreRenderer & obj) : tube_(obj.tube_), size_(obj.size_), texture_(obj.texture_) {
    obj.texture_ = 0;
}

const ScoreRenderer & ScoreRenderer::operator =(const ScoreRenderer & obj) {
    tube_ = obj.tube_;
    size_ = obj.size_;
    if (texture_ != obj.texture_) {
        texture_ = obj.texture_;
        obj.texture_ = 0;
    }
    return *this;
}

void ScoreRenderer::renderScore(const GameState & state) const {
    glPushAttrib(GL_ENABLE_BIT); 

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, size_.width, size_.height, 0, -10, 200);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glLoadIdentity();

    glBindTexture(GL_TEXTURE_2D, texture_);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);

    glColor4f(1, 1, 1, 1);
    for (int i = 0; i < state.own_lives; i++) {
        int x = 10 + i * (heart_image.width + 10);

        glTexCoord2f(0, 1); glVertex2f(x, 10 + heart_image.height);
        glTexCoord2f(1, 1); glVertex2f(x + heart_image.width, 10 + heart_image.height);
        glTexCoord2f(1, 0); glVertex2f(x + heart_image.width, 10);
        glTexCoord2f(0, 0); glVertex2f(x, 10);
    }

    for (int i = 0; i < state.opponent_lives; i++) {
        int x = size_.width - 10 - (i+1) * (heart_image.width + 10);

        glTexCoord2f(0, 1); glVertex2f(x, 10 + heart_image.height);
        glTexCoord2f(1, 1); glVertex2f(x + heart_image.width, 10 + heart_image.height);
        glTexCoord2f(1, 0); glVertex2f(x + heart_image.width, 10);
        glTexCoord2f(0, 0); glVertex2f(x, 10);
    }

    glEnd();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}
