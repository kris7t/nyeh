#include "model.hxx"
#include "3DView.hxx"
#include "GameUpdater.hxx"

int main(int argc, char * argv[]) {
    ThreeDView view(1366, 768);
    bool running;
    
    Balls balls;
    Ball b;
    b.type = 0;
    b.velocity = cv::Point3f(0, -1, 0);
    b.position = cv::Point3f(-2.0f, 8.0f, 2.0f);
    balls.push_back(b);
    b.type = 0;
    b.velocity = cv::Point3f(0, -0.2f, 1);
    b.position = cv::Point3f(2.0f, 8.0f, 1.0f);
    balls.push_back(b);
    b.type = 0;
    b.velocity = cv::Point3f(1, -0.2f, 0);
    b.position = cv::Point3f(-.2f, 8.0f, .3f);
    balls.push_back(b);
    GameUpdater game(cv::Size2f(2.4f, 1.6f));

    do {
        glfwSetTime(0.0);
        view.render(balls);
        glfwSwapBuffers();
        glfwSleep(0.01);
        game.tick(glfwGetTime(), balls);
        running = !glfwGetKey(GLFW_KEY_ESC);
    } while (running);

    return 0;
}
