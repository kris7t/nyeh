#include "CamRenderer.hxx"

CamRenderer::CamRenderer(const Tube & tube) : texid_(0), tube_(tube) {}
CamRenderer::~CamRenderer() { dispose(); }

void CamRenderer::init() {
    glGenTextures(1, &texid_);
    glBindTexture(GL_TEXTURE_2D, texid_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void CamRenderer::dispose() {
    if (texid_) {
        glDeleteTextures(1, &texid_);
        texid_ = 0;
    }
}

void CamRenderer::upload(const cv::Mat& frame) {
    glBindTexture(GL_TEXTURE_2D, texid_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR,
                 GL_UNSIGNED_BYTE, frame.ptr(0));

}

void CamRenderer::render(bool blend, bool zbuf) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, texid_);
    if (blend) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    if (!zbuf) {
        glDisable(GL_DEPTH_TEST);
    }
    glColor4f(1, 1, 1, 0.4);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f(-tube_.halfSize.width, tube_.separator, -tube_.halfSize.height);

        glTexCoord2f(1, 1);
        glVertex3f(tube_.halfSize.width, tube_.separator, -tube_.halfSize.height);

        glTexCoord2f(1, 0);
        glVertex3f(tube_.halfSize.width, tube_.separator, tube_.halfSize.height);

        glTexCoord2f(0, 0);
        glVertex3f(-tube_.halfSize.width, tube_.separator, tube_.halfSize.height);
    glEnd();

    glPopAttrib();
}
