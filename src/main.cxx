#include "Cam.hxx"
#include "HistogramHand.hxx"

int main(int argc, char * argv[]) {
    if (argc < 5) {
        std::cout << "Usage: " << argv[0] << " <cam id> <threshold> <fill ratio> <dt>" << std::endl;
        return -1;
    }

    Cam_ c = Cam::create(atoi(argv[1]));
    HistogramHand hh(atoi(argv[2]), atof(argv[3]), atof(argv[4]));

    hh.calibrate(c);

    cv::namedWindow("test");
    cv::namedWindow("test");

    cv::Mat draw;

    do {
        c->grabImage();
        hh.update(c->frame());
        draw = c->frame().clone();
        if (hh.position().z > 0)
            circle(draw, cv::Point2i(hh.position().x, hh.position().y), hh.position().z, cv::Scalar(0, 255, 0), CV_FILLED);
        cv::imshow("test", draw);
    } while (cv::waitKey(10) != 27);

    return 0;
}
