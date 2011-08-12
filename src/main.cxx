#include "model.hxx"
#include "3DView.hxx"
#include "GameUpdater.hxx"
#include "Cam.hxx"
#include "HistogramHand.hxx"
#include "NetGame.hxx"
#include "NetCam.hxx"
#include "HandToModel.hxx"
#include "Calibrate.hxx"

namespace po = boost::program_options;

static constexpr int redBalls = 3;
static constexpr int yellowBalls = 12;

volatile bool running;

namespace cv {

void validate(boost::any & v, const std::vector<std::string> & values, Size *, int) {
    static boost::regex reg(R"***((\d+)x(\d+))***");

    po::validators::check_first_occurrence(v);
    const std::string & s = po::validators::get_single_string(values);
    boost::smatch match;
    if (boost::regex_match(s, match, reg)) {
        int width = boost::lexical_cast<int>(match[1]),
            height = boost::lexical_cast<int>(match[2]);
        v = Size(width, height);
    } else {
        throw po::validation_error(po::validation_error::invalid_option_value);
    }
}

template <typename T>
std::ostream & operator <<(std::ostream & output, const Size_<T> & size) {
    output << size.width << "x" << size.height;
    return output;
}

}

void camLoop(Cam_ c, NetCam_ nc, Hand_ h) {
    do {
        c->grabImage();
        nc->push(c->jpeg());
        h->update(c->frame());
    } while (running);
}

int main(int argc, char * argv[]) {
    try {
        // initializing GLFW here to get desktop mode for program options
        glewInit();
        glfwInit();

        GLFWvidmode desktopMode;
        glfwGetDesktopMode(&desktopMode);
        cv::Size default_screen_res(desktopMode.Width, desktopMode.Height),
            default_cam_res(640, 480);
        po::options_description desc("Allowed options");
        desc.add_options()
            ("screen-res,r", po::value<cv::Size>()->default_value(default_screen_res), "Screen resolution, required.")
            ("cam-res,R", po::value<cv::Size>()->default_value(default_cam_res), "Webcam resolution.")
            ("cam,c", po::value<int>()->default_value(0), "Webcam id.")
            ("host,h", po::value<std::string>(), "When specified, instead of setting up a server,"
                                                 "the game will connect to the host as the client.")
        ;
        po::variables_map vm;
        try {
            po::store(po::parse_command_line(argc, argv, desc), vm);
            po::notify(vm);
        } catch (po::error & e) {
            std::cout << e.what() << "\n\n"
                << "Usage: " << argv[0] << " [options]\n"
                << desc << std::endl;
            return -1;
        }

        NetGame_ ng;
        NetCam_ nc;
        int ballof;
        if (vm.count("host")) {
            std::string host = vm["host"].as<std::string>();
            ng.reset(new NetGame(argv[2]));
            nc.reset(new NetCam(argv[2]));
            ballof = 0xffff;
        } else {
            ng.reset(new NetGame());
            nc.reset(new NetCam());
            ballof = 0;
        }

        Tube tube;
        tube.halfSize = cv::Size2f(1.6, 1.2);
        tube.goal = 3;
        tube.separator = 13;
        tube.opponentGoal = 23;
        tube.handMovement = 5;
        tube.handMax = 8;
        tube.spawnArea = 4;

        Cam_ c = Cam::create(vm["cam-res"].as<cv::Size>(), vm["cam"].as<int>());
        Hand_ hh = HistogramHand::create();
        HandFilter_ hf = HandFilter::create();
        HandToModel_ htm = HandToModel::create(tube);
        Calibrate::create()->run(c, hh, hf, htm);

        ThreeDView view(vm["screen-res"].as<cv::Size>(), tube);

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

        boost::thread camThread(boost::bind(&camLoop, c, nc, hh));

        do {
            glfwSetTime(0.0);
            hf->update(hh);
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
                std::cout << "\n\aYOU WON! Congratulations.\n";
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
