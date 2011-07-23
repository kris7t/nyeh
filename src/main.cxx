#include "model.hxx"
#include "3DView.hxx"
#include "GameUpdater.hxx"
#include "Cam.hxx"
#include "Hand.hxx"
#include "HistogramHand.hxx"

bool running;

void camLoop(Cam_ c, Hand_ h) {
    do {
        c->grabImage();
        h->update(c->frame());
    } while (running);
}

int main(int argc, char * argv[]) {
    if (argc < 4) {
        std::cout << "Usage: " << argv[0] << " <cam id> <fill ratio> <dt>" << std::endl;
        return -1;
    }

    Cam_ c = Cam::create(atoi(argv[1]));
    Hand_ hh(new HistogramHand(atof(argv[2]), atof(argv[3])));

    hh->calibrate(c);

    Tube tube;
    tube.halfSize = cv::Size2f(2.4, 1.6);
    tube.goal = 3;

    ThreeDView view(1366, 768, tube);
    
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
    GameUpdater game(tube);
    
    running = true;
    
    boost::thread camThread(boost::bind(&camLoop, c, hh));

    do {
        glfwSetTime(0.0);
        view.render(balls, hh);
        glfwSwapBuffers();
        glfwSleep(0.01);
        game.tick(glfwGetTime(), balls);
        running = !glfwGetKey(GLFW_KEY_ESC);
    } while (running);

    return 0;
}
