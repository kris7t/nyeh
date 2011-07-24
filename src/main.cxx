#include "model.hxx"
#include "3DView.hxx"
#include "GameUpdater.hxx"
#include "Cam.hxx"
#include "Hand.hxx"
#include "HistogramHand.hxx"
#include "NetGame.hxx"

bool running;

void camLoop(Cam_ c, Hand_ h) {
    do {
        c->grabImage();
        h->update(c->frame());
    } while (running);
}

int main(int argc, char * argv[]) {
    try {
        if (argc < 4) {
            std::cout << "Usage: " << argv[0] << " <cam id> <fill ratio> <dt> [server]" << std::endl;
            return -1;
        }

        NetGame * ng;
        Ball b;
        if (argc < 5) {
            ng = new NetGame();
            b.owner = ballOwnerLocal;
        } else {
            ng = new NetGame(argv[4]);
            b.owner = ballOwnerRemote;
        }

        Cam_ c = Cam::create(atoi(argv[1]));
        Hand_ hh(new HistogramHand(atof(argv[2]), atof(argv[3])));

        hh->calibrate(c);

        Tube tube;
        tube.halfSize = cv::Size2f(2.4, 1.6);
        tube.goal = 3;
        tube.separator = 13;
        tube.opponentGoal = 23;

        ThreeDView view(cv::Size(1366, 768), tube);

        GameState gs;
        gs.own_lives = 3;
        gs.opponent_lives = 2;

        Balls balls;
        b.type = 0;
        b.velocity = cv::Point3f(0, -1, 0);
        b.position = cv::Point3f(-2.0f, 8.0f, 2.0f);
        balls[1] = b;

        b.type = 0;
        b.velocity = cv::Point3f(0, -0.02f, 1);
        b.position = cv::Point3f(2.0f, 8.0f, 1.0f);
        balls[2] = b;

        b.type = 0;
        b.velocity = cv::Point3f(1, -0.02f, 0);
        b.position = cv::Point3f(-.2f, 8.0f, .3f);
        balls[3] = b;
        GameUpdater game(tube);

        running = true;

        boost::thread camThread(boost::bind(&camLoop, c, hh));

        do {
            glfwSetTime(0.0);
            view.render(balls, hh, gs);
            glfwSwapBuffers();
            //glfwSleep(0.01);
            cv::waitKey(10);
            game.tick(glfwGetTime(), balls);
            ng->sync(balls);
            running = !glfwGetKey(GLFW_KEY_ESC);
        } while (running);

        return 0;
    } catch (const std::string& str) {
        std::cerr << "exception: " << str << std::endl;
        return -1;
    }
}
