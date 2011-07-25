#include "model.hxx"
#include "3DView.hxx"
#include "GameUpdater.hxx"
#include "Cam.hxx"
#include "Hand.hxx"
#include "HistogramHand.hxx"
#include "NetGame.hxx"
#include "NetCam.hxx"
#include "HandToModel.hxx"

volatile bool running;

void camLoop(Cam_ c, NetCam_ nc, Hand_ h, HandToModel_ htm) {
    do {
        c->grabImage();
        nc->push(c->jpeg());
        h->update(c->frame());
        htm->update(h);
    } while (running);
}

int main(int argc, char * argv[]) {
    try {
        if (argc < 4) {
            std::cout << "Usage: " << argv[0] << " <cam id> <fill ratio> <dt> [server]" << std::endl;
            return -1;
        }

        NetGame_ ng;
        NetCam_ nc;
        Ball b;
        if (argc < 5) {
            ng.reset(new NetGame());
            nc.reset(new NetCam());
            b.owner = ballOwnerLocal;
        } else {
            ng.reset(new NetGame(argv[4]));
            nc.reset(new NetCam(argv[4]));
            b.owner = ballOwnerRemote;
        }

        Cam_ c = Cam::create(atoi(argv[1]));
        Hand_ hh(new HistogramHand(atof(argv[2]), atof(argv[3])));

        hh->calibrate(c);

        Tube tube;
        tube.halfSize = cv::Size2f(1.6, 1.2);
        tube.goal = 3;
        tube.separator = 13;
        tube.opponentGoal = 23;
        tube.handMovement = 5;
        tube.handMax = 10;
        tube.spawnArea = 4;

        ThreeDView view(cv::Size(1366, 768), tube);
        HandToModel_ htm = HandToModel::create(tube);

        GameState gs;
        gs.own_lives = 3;
        gs.opponent_lives = 2;

        Balls balls;
        b.type = 1;
        b.velocity = cv::Point3f(0, -1, 0);
        b.position = cv::Point3f(-2.0f, 13.0f, 2.0f);
        balls[1] = b;

        b.type = 1;
        b.velocity = cv::Point3f(0, -1, 1);
        b.position = cv::Point3f(2.0f, 13.0f, 1.0f);
        balls[2] = b;

        b.type = 0;
        b.velocity = cv::Point3f(1, -1, 0);
        b.position = cv::Point3f(-.2f, 13.0f, .3f);
        balls[3] = b;
        GameUpdater game(tube);

        running = true;

        boost::thread camThread(boost::bind(&camLoop, c, nc, hh, htm));

        do {
            ng->sync(balls, gs, tube);
            nc->grabImage();

            glfwSetTime(0.0);
            view.render(balls, htm, gs, nc->frame(), c->frame());
            glfwSwapBuffers();
            glfwSleep(0.01);
            game.tick(glfwGetTime(), balls, gs);
            running = !glfwGetKey(GLFW_KEY_ESC);
        } while (running);

        camThread.join();
        return 0;
    } catch (const std::string& str) {
        std::cerr << "exception: " << str << std::endl;
        return -1;
    }
}
