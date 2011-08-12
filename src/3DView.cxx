#include "3DView.hxx"

bool glInited = false;

static void doStrip(float sx, float sz, float ex, float ez, float y1, float y2, int div) {
    float xw = (ex - sx) / div;
    float zw = (ez - sz) / div;
    for (int i = 0; i <= div; ++i) {
        glVertex3f(sx + i*xw, y1, sz + i*zw);
        glVertex3f(sx + i*xw, y2, sz + i*zw);
    }
}

static void renderTubeWall(const Tube & tube, int ydiv, int div) {
    GLfloat col[] = {.39f, .58f, .93f, 1};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, col);
    glMaterialfv(GL_FRONT, GL_AMBIENT, col);

    //float yw = (tube.opponentGoal - tube.goal) / ydiv;
    float ym = (tube.opponentGoal - tube.goal) / (ydiv * ydiv);

    for (int y = 0; y < ydiv; ++y) {
        float y1 = tube.goal + (y+1) * (y+1) * ym;
        float y2 = tube.goal + (y*y) * ym;

        glBegin(GL_QUAD_STRIP);
        glNormal3f(0,0,1);
        doStrip(-tube.halfSize.width, -tube.halfSize.height,
                tube.halfSize.width, -tube.halfSize.height,
                y1, y2, div);
        glEnd();

        glBegin(GL_QUAD_STRIP);
        glNormal3f(-1,0,0);
        doStrip(tube.halfSize.width, -tube.halfSize.height,
                tube.halfSize.width, tube.halfSize.height,
                y1, y2, div);
        glEnd();

        glBegin(GL_QUAD_STRIP);
        glNormal3f(0,0,-1);
        doStrip(tube.halfSize.width, tube.halfSize.height,
                -tube.halfSize.width, tube.halfSize.height,
                y1, y2, div);

        glEnd();

        glBegin(GL_QUAD_STRIP);
        glNormal3f(1,0,0);
        doStrip(-tube.halfSize.width, tube.halfSize.height,
                -tube.halfSize.width, -tube.halfSize.height,
                y1, y2, div);
        glEnd();
    }

}

static void renderSphere(const cv::Point3f pos, float size, float r, float g, float b) {
    static GLUquadric * q = gluNewQuadric();

    GLfloat diffuse[] = { r, g, b, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    GLfloat amb[] = {r,g,b, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    //glMaterialfv(GL_FRONT, GL_SPECULAR, amb);

    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);

    gluSphere(q, size, 320.*sqrtf(size)/pos.y, 250.*sqrtf(size)/pos.y);
    glPopMatrix();
}

ThreeDView::ThreeDView(cv::Size size, Tube tube)
    : tube_(tube), hudRenderer_(tube_, size), camRenderer_(tube) {
    if (!glInited) {
        glInited = true;
    } else {
        throw "3DView already running!";
    }
    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    if (!glfwOpenWindow(size.width, size.height, 8, 8, 8, 0, 24, 0, GLFW_FULLSCREEN)) {
        throw "Cannot open OpenGL window";
    }

    glViewport(0, 0, size.width, size.height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, static_cast<float>(size.width) / size.height, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    {
        GLfloat pos[] = {-tube_.halfSize.width/2, 0, tube_.halfSize.height/2, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, pos);
        GLfloat spec[] = {0.2f,0.2f,0.2f,1};
        glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
        GLfloat diff[] = {1,1,1,1};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.015f);
        GLfloat gamb[] = { 0.1f, 0.1f, 0.1f, 1};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gamb);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    }

    {
        glEnable(GL_LIGHT1);
        GLfloat pos[] = {0, -1, 0, 0};
        glLightfv(GL_LIGHT1, GL_POSITION, pos);
        GLfloat spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
        GLfloat diff[] = { 0.3f, 0.3f, 0.3f, 1};
        glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);

    glClearColor(.39f, .58f, .93f, 1.0f);

    camRenderer_.init();
    hudRenderer_.init();
}

ThreeDView::~ThreeDView() {
    camRenderer_.dispose();
    hudRenderer_.dispose();
    glfwTerminate();
}

void ThreeDView::render(const Balls & balls, HandToModel_ hand,
                        const GameState & state, const cv::Mat & frame,
                        const cv::Mat & ownframe) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camRenderer_.upload(frame);
    camRenderer_.render(false, false);
    renderTubeWall(tube_, 5, 3);

    for (Balls::const_iterator it = balls.begin();
            it != balls.end(); ++it) {
        BallType type = ballTypes[it->second.type];
        renderSphere(it->second.position, type.size, type.r, type.g, type.b);
    }

    renderSphere(hand->position(), handSize, 0.0f, 1.0f, 0.0f);
    camRenderer_.render(true, true);
    hudRenderer_.upload(ownframe);
    hudRenderer_.renderScore(state);
}
