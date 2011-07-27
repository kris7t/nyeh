#include "HudRenderer.hxx"
#include "heart.inc.c"

static const int heartColWidth = 2;
static const float heartSpacing = 5;
static const float ballsBarHeight = 30;
static const float ballsBarHalfWidth = 150;

HudRenderer::HudRenderer(const Tube & tube, cv::Size size) : tube_(tube), size_(size) {
    for (int i = 0; i < 2; ++i) {
        texture_[i] = 0;
    }
}
HudRenderer::~HudRenderer() { dispose(); }

void HudRenderer::init() {
    glGenTextures(2, texture_);
    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, texture_[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glBindTexture(GL_TEXTURE_2D, texture_[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, heart_image.width, heart_image.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, heart_image.pixel_data);
}

void HudRenderer::dispose() {
    glDeleteTextures(2, texture_);
    for (int i = 0; i < 2; ++i) {
        texture_[i] = 0;
    }
}

void HudRenderer::upload(const cv::Mat & frame) {
    glBindTexture(GL_TEXTURE_2D, texture_[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR,
                 GL_UNSIGNED_BYTE, frame.ptr(0));
    imgsize_ = cv::Size(frame.cols / 2, frame.rows / 2);
}

static void drawHeart(int x, int y) {
    glTexCoord2f(0, 1); glVertex2f(x, y + heart_image.height);
    glTexCoord2f(1, 1); glVertex2f(x + heart_image.width, y + heart_image.height);
    glTexCoord2f(1, 0); glVertex2f(x + heart_image.width, y);
    glTexCoord2f(0, 0); glVertex2f(x, y);
}

void HudRenderer::renderScore(const GameState & state) const {
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

    glBindTexture(GL_TEXTURE_2D, texture_[0]);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);

    glColor4f(1, 1, 1, 1);
    for (int i = 0; i < state.own_lives / heartColWidth; ++i) {
        for (int j = 0; j < heartColWidth; ++j) {
            int x = heartSpacing + j * (heart_image.width + heartSpacing);
            int y = heartSpacing + i * (heart_image.height + heartSpacing);
            drawHeart(x, y);
        }
    }
    for (int i = 0; i < state.own_lives % heartColWidth; ++i) {
        int x = heartSpacing + i * (heart_image.width + heartSpacing);
        int y = heartSpacing + (state.own_lives / heartColWidth) * (heart_image.height + heartSpacing);
        drawHeart(x, y);
    }

    for (int i = 0; i < state.opponent_lives / heartColWidth; ++i) {
        for (int j = 0; j < heartColWidth; ++j) {
            int x = size_.width - heartSpacing - (j+1) * (heart_image.width + heartSpacing);
            int y = heartSpacing + i * (heart_image.height + heartSpacing);
            drawHeart(x, y);
        }
    }
    for (int i = 0; i < state.opponent_lives % heartColWidth; ++i) {
        int x = size_.width - heartSpacing - (i+1) * (heart_image.width + heartSpacing);
        int y = heartSpacing + (state.opponent_lives / heartColWidth) * (heart_image.height + heartSpacing);
        drawHeart(x, y);
    }

    glEnd();

    glBindTexture(GL_TEXTURE_2D, texture_[1]);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex2f(0, size_.height);
    glTexCoord2f(0, 1); glVertex2f(imgsize_.width, size_.height);
    glTexCoord2f(0, 0); glVertex2f(imgsize_.width, size_.height - imgsize_.height);
    glTexCoord2f(1, 0); glVertex2f(0, size_.height - imgsize_.height);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);

    float pos = (state.own_ratio -.5) * 2 * ballsBarHalfWidth;
    glBegin(GL_QUADS);
    glColor4f(1,0,0, .8f);
    glVertex2f(size_.width / 2 - ballsBarHalfWidth, ballsBarHeight);
    glVertex2f(size_.width / 2 + pos, ballsBarHeight);
    glVertex2f(size_.width / 2 + pos, 0);
    glVertex2f(size_.width / 2 - ballsBarHalfWidth, 0);

    glColor4f(0,1,0, .6f);
    glVertex2f(size_.width / 2 + pos, ballsBarHeight);
    glVertex2f(size_.width / 2 + ballsBarHalfWidth, ballsBarHeight);
    glVertex2f(size_.width / 2 + ballsBarHalfWidth, 0);
    glVertex2f(size_.width / 2 + pos, 0);
    glEnd();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}
