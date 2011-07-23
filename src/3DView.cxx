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

static void renderCube(cv::Point3f pos, float r, float g, float b, float a) {
    GLfloat diffuse[] = { r, g, b, a };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glVertex3f(-.1f, .1f, .1f);
    glNormal3f(0, -1, 0);
    glVertex3f(.1f, .1f, .1f);
    glNormal3f(0, -1, 0);
    glVertex3f(.1f, .1f, -.1f);
    glNormal3f(0, -1, 0);
    glVertex3f(-.1f, .1f, -.1f);

    glNormal3f(0, 1, 0);
    glVertex3f(.1f, -.1f, .1f);
    glNormal3f(0, 1, 0);
    glVertex3f(-.1f, -.1f, .1f);
    glNormal3f(0, 1, 0);
    glVertex3f(-.1f, -.1f, -.1f);
    glNormal3f(0, 1, 0);
    glVertex3f(.1f, -.1f, -.1f);

    glNormal3f(0, 0, -1);
    glVertex3f(.1f, -.1f, .1f);
    glNormal3f(0, 0, -1);
    glVertex3f(.1f, .1f, .1f);
    glNormal3f(0, 0, -1);
    glVertex3f(-.1f, .1f, .1f);
    glNormal3f(0, 0, -1);
    glVertex3f(-.1f, -.1f, .1f);

    glNormal3f(1, 0, 0);
    glVertex3f(-.1f, -.1f, .1f);
    glNormal3f(1, 0, 0);
    glVertex3f(-.1f, .1f, .1f);
    glNormal3f(1, 0, 0);
    glVertex3f(-.1f, .1f, -.1f);
    glNormal3f(1, 0, 0);
    glVertex3f(-.1f, -.1f, -.1f);

    glNormal3f(-1, 0, 0);
    glVertex3f(.1f, -.1f, -.1f);
    glNormal3f(-1, 0, 0);
    glVertex3f(.1f, .1f, -.1f);
    glNormal3f(-1, 0, 0);
    glVertex3f(.1f, .1f, .1f);
    glNormal3f(-1, 0, 0);
    glVertex3f(.1f, -.1f, .1f);

    glNormal3f(0, 0, 1);
    glVertex3f(.1f, .1f, -.1f);
    glNormal3f(0, 0, 1);
    glVertex3f(.1f, -.1f, -.1f);
    glNormal3f(0, 0, 1);
    glVertex3f(-.1f, -.1f, -.1f);
    glNormal3f(0, 0, 1);
    glVertex3f(-.1f, .1f, -.1f);
    glEnd();
    glPopMatrix();
}

static void renderCubeEnemy(const Ball & ball) {
    renderCube(ball.position, 1.0f, 1.0f, 0.0f, 1.0f);
}

BallRenderer ballRenderers[] = {
    &renderCubeEnemy
};

ThreeDView::ThreeDView(cv::Size size, Tube tube) : tube_(tube) {
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
    glLoadIdentity();
    gluPerspective(45.0f, static_cast<float>(size.width) / size.height, 1.0f, 100.0f);
    gluLookAt(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_LIGHTING);
    GLfloat ambient[] = { 0, 0, 0, 1 };
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);

    glClearColor(.39f, .58f, .93f, 1.0f);

    scoreRenderer_ = ScoreRenderer(tube_, size);
}

ThreeDView::~ThreeDView() {
    glfwTerminate();
}

void ThreeDView::render(const Balls & balls, Hand_ hand, const GameState & state) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderTube(tube_);

    for (Balls::const_iterator it = balls.begin();
            it != balls.end(); ++it) {
        ballRenderers[it->second.type](it->second);
    }

    cv::Point3f renderHand;
    volatile const cv::Point3f & rawHand = hand->position();
    renderHand.x = (-rawHand.x + 320) * tube_.halfSize.width / 320;
    renderHand.z = (-rawHand.y + 240) * tube_.halfSize.height / 240;
    renderHand.y = 6;
    renderCube(renderHand, 0.0f, 1.0f, 0.0f, 1.0f);
    scoreRenderer_.renderScore(state);
}
