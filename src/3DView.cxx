#include "3DView.hxx"

bool glInited = false;

static void renderTube(const Tube & tube) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    glVertex3f(tube.halfSize.width, tube.goal, -tube.halfSize.height);
    glVertex3f(tube.halfSize.width, tube.goal, tube.halfSize.height);
    glVertex3f(-tube.halfSize.width, tube.goal, tube.halfSize.height);
    glVertex3f(-tube.halfSize.width, tube.goal, -tube.halfSize.height);
    glVertex3f(tube.halfSize.width, tube.goal, -tube.halfSize.height);
    glVertex3f(tube.halfSize.width, tube.opponentGoal, -tube.halfSize.height);
    glVertex3f(-tube.halfSize.width, tube.opponentGoal, -tube.halfSize.height);
    glVertex3f(-tube.halfSize.width, tube.opponentGoal, tube.halfSize.height);
    glVertex3f(tube.halfSize.width, tube.opponentGoal, tube.halfSize.height);
    glVertex3f(tube.halfSize.width, tube.opponentGoal, -tube.halfSize.height);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-tube.halfSize.width, tube.goal, -tube.halfSize.height);
    glVertex3f(-tube.halfSize.width, tube.opponentGoal, -tube.halfSize.height);
    glVertex3f(-tube.halfSize.width, tube.goal, tube.halfSize.height);
    glVertex3f(-tube.halfSize.width, tube.opponentGoal, tube.halfSize.height);
    glVertex3f(tube.halfSize.width, tube.goal, tube.halfSize.height);
    glVertex3f(tube.halfSize.width, tube.opponentGoal, tube.halfSize.height);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(tube.halfSize.width, tube.separator, tube.halfSize.height);
    glVertex3f(-tube.halfSize.width, tube.separator, tube.halfSize.height);
    glVertex3f(-tube.halfSize.width, tube.separator, -tube.halfSize.height);
    glVertex3f(tube.halfSize.width, tube.separator, -tube.halfSize.height);
    glVertex3f(tube.halfSize.width, tube.separator, tube.halfSize.height);
    glEnd();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
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

    gluSphere(q, size, 10, 8);
    glPopMatrix();
}

static void renderSphereEnemy(const Ball & ball) {
    renderSphere(ball.position, .1f, 1,1,0);
}

static void renderSphereEnemyB(const Ball & ball) {
    renderSphere(ball.position, .1f, .8f,0,0);
}

BallRenderer ballRenderers[] = {
    &renderSphereEnemy,
    &renderSphereEnemyB
};

ThreeDView::ThreeDView(cv::Size size, Tube tube)
    : tube_(tube), hudRenderer_(tube_, size), camRenderer_(tube) {
    if (!glInited) {
        glewInit();
        glfwInit();
        glInited = true;
    } else {
        throw "3DView already running!";
    }
    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    if (!glfwOpenWindow(size.width, size.height, 8, 8, 8, 8, 32, 0, GLFW_FULLSCREEN)) {
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

    GLfloat pos[] = {-2, 0, 2, 1}; //{-tube_.halfSize.width, 0, tube_.halfSize.height, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    GLfloat spec[] = {0.2f,0.2f,0.2f,1};
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    GLfloat diff[] = {1,1,1,1};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.005f);
    GLfloat gamb[] = { 0.1f, 0.1f, 0.1f, 1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gamb);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    glEnable(GL_LIGHT1);
    pos[0] = 1, pos[1] = 0, pos[2] = 1, pos[3] = 0;
    glLightfv(GL_LIGHT1, GL_POSITION, pos);
    spec[0] = 0.0f, spec[1] = 0.0f, spec[2] = 0.0f, spec[3] = 1.0f;
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
    diff[0] = .3f, diff[1] = .3f, diff[2] = .3f, diff[3] = 1.0f;
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);

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
    renderTube(tube_);

    for (Balls::const_iterator it = balls.begin();
            it != balls.end(); ++it) {
        ballRenderers[it->second.type](it->second);
    }

    renderSphere(hand->position(), .25f, 0.0f, 1.0f, 0.0f);
    camRenderer_.render(true, true);
    hudRenderer_.upload(ownframe);
    hudRenderer_.renderScore(state);
}
