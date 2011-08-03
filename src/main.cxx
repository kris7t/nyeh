#include "model.hxx"
#include "3DView.hxx"
#include "GameUpdater.hxx"
#include "Cam.hxx"
#include "HistogramHand.hxx"
#include "NetGame.hxx"
#include "NetCam.hxx"
#include "HandToModel.hxx"
#include "Calibrate.hxx"

static const int redBalls = 3;
static const int yellowBalls = 12;

volatile bool running;

void camLoop(Cam_ c, NetCam_ nc, Hand_ h, HandFilter_ hf) {
    do {
        c->grabImage();
        nc->push(c->jpeg());
        h->update(c->frame());
        hf->update(h);
    } while (running);
}

int main(int argc, char * argv[]) {
    try {
        if (argc < 2) {
            std::cout << "Usage: " << argv[0] << " <cam id> [server]" << std::endl;
            return -1;
        }

        NetGame_ ng;
        NetCam_ nc;
        int ballof;
        if (argc < 3) {
            ng.reset(new NetGame());
            nc.reset(new NetCam());
            ballof = 0;
        } else {
            ng.reset(new NetGame(argv[2]));
            nc.reset(new NetCam(argv[2]));
            ballof = 0xffff;
        }

        Tube tube;
        tube.halfSize = cv::Size2f(1.6, 1.2);
        tube.goal = 3;
        tube.separator = 13;
        tube.opponentGoal = 23;
        tube.handMovement = 5;
        tube.handMax = 8;
        tube.spawnArea = 4;

        Cam_ c = Cam::create(atoi(argv[1]));
        Hand_ hh = HistogramHand::create();
        HandFilter_ hf = HandFilter::create();
        HandToModel_ htm = HandToModel::create(tube);
        Calibrate::create()->run(c, hh, hf, htm);

        ThreeDView view(cv::Size(1366, 768), tube);

        GameState gs;
        gs.own_lives = 12;
        gs.opponent_lives = 12;

        GameUpdater game(tube);

        Balls balls;
        Ball b;
        b.owner = ballOwnerLocal;
        b.type = 0;
        for (int i = 0; i < yellowBalls; ++i) {
            game.randomizeBall(b);
            balls[ballof + i] = b;
        }
        b.type = 1;
        for (int i = 0; i < redBalls; ++i) {
            game.randomizeBall(b);
            balls[ballof + yellowBalls + i] = b;
        }

        running = true;

        boost::thread camThread(boost::bind(&camLoop, c, nc, hh, hf));

        do {
            glfwSetTime(0.0);
            htm->update(hf);
            view.render(balls, htm, gs, nc->frame(), c->frame());
            glfwSwapBuffers();
            glfwSleep(0.01);
            game.tick(glfwGetTime(), balls, gs, htm);
            ng->sync(balls, gs, tube);
            nc->grabImage();
            running = !glfwGetKey(GLFW_KEY_ESC) && (gs.own_lives > 0) && (gs.opponent_lives > 0);
        } while (running);
       
        int ret;
        if (gs.opponent_lives <= 0) {
            if (gs.own_lives <= 0) {
                std::cout << "\n\aTIE! Play another round?\n";
                ret = 2;
            } else {
                std::cout << "\n\aOU WON! Congratulations.\n";
                ret = 0;
            }
        } else if (gs.own_lives <= 0) {
            std::cout << "\n\aGAME OVER! Better luck next time.\n";
            ret = 3;
        } else {
            std::cout << "\n\aGame terminated before conclusion.\n";
            ret = 1;
        }
        std::cout << gs.own_ratio * 100 << "% of the balls were still in your court." << std::endl;

        camThread.join();
        return ret;
    } catch (const std::string& str) {
        std::cerr << "exception: " << str << std::endl;
        return -1;
    }
}
