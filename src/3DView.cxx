#include "3DView.hxx"

bool glInited = false;

static void renderCube(cv::Point3f pos) {
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3f(-.1f, .1f, .1f);
    glVertex3f(.1f, .1f, .1f);
    glVertex3f(.1f, .1f, -.1f);
    glVertex3f(-.1f, .1f, -.1f);
    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    glVertex3f(.1f, -.1f, .1f);
    glVertex3f(-.1f, -.1f, .1f);
    glVertex3f(-.1f, -.1f, -.1f);
    glVertex3f(.1f, -.1f, -.1f);
    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    glVertex3f(.1f, -.1f, .1f);
    glVertex3f(.1f, .1f, .1f);
    glVertex3f(-.1f, .1f, .1f);
    glVertex3f(-.1f, -.1f, .1f);
    glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
    glVertex3f(-.1f, -.1f, .1f);
    glVertex3f(-.1f, .1f, .1f);
    glVertex3f(-.1f, .1f, -.1f);
    glVertex3f(-.1f, -.1f, -.1f);
    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    glVertex3f(.1f, -.1f, -.1f);
    glVertex3f(.1f, .1f, -.1f);
    glVertex3f(.1f, .1f, .1f);
    glVertex3f(.1f, -.1f, .1f);
    glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
    glVertex3f(.1f, .1f, -.1f);
    glVertex3f(.1f, -.1f, -.1f);
    glVertex3f(-.1f, -.1f, -.1f);
    glVertex3f(-.1f, .1f, -.1f);
    glEnd();
    glPopMatrix();
}

static void renderCubeEnemy(const Ball & ball) {
    renderCube(ball.position);
}

BallRenderer ballRenderers[] = {
    &renderCubeEnemy 
};

ThreeDView::ThreeDView(int width, int height) {
    if (!glInited) {
        glewInit();
        glfwInit();
        glInited = true;
    } else {
        throw "3DView already running!";
    }
    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    if (!glfwOpenWindow(width, height, 8, 8, 8, 8, 32, 0, GLFW_FULLSCREEN)) {
        throw "Cannot open OpenGL window";
    }
    
    glViewport(0, 0, width, height);
    gluPerspective(45.0f, static_cast<float>(width) / height, 1.0f, 100.0f);
    gluLookAt(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);

    glClearColor(.39f, .58f, .93f, 1.0f);
}

ThreeDView::~ThreeDView() {
    glfwTerminate();
}

void ThreeDView::render(const Balls & balls, Hand_ hand) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (Balls::const_iterator it = balls.begin();
            it != balls.end(); ++it) {
        ballRenderers[it->type](*it);
    }
    
    cv::Point3f renderHand;
    renderHand.x = (hand->position().x - 320) * 2.4 / 320;
    renderHand.z = (hand->position().y - 240) * 1.6 / 240;
    renderHand.y = 8;
    renderCube(renderHand);
}
