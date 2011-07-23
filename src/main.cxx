#include "Cam.hxx"
#include "HistogramHand.hxx"

int main(int argc, char * argv[]) {
    if (argc < 4) {
        std::cout << "Usage: " << argv[0] << " <cam id> <fill ratio> <dt>" << std::endl;
        return -1;
    }

    Cam_ c = Cam::create(atoi(argv[1]));
    HistogramHand hh(atof(argv[2]), atof(argv[3]));

    hh.calibrate(c);

    cv::namedWindow("cam");
    cv::namedWindow("thresh");

    cv::Mat draw;

    do {
        c->grabImage();
        hh.update(c->frame());
        draw = c->frame().clone();
        if (hh.position().z > 0)
            circle(draw, cv::Point2i(hh.position().x, hh.position().y), hh.position().z, cv::Scalar(0, 255, 0), CV_FILLED);
        cv::imshow("cam", draw);
    } while (cv::waitKey(10) != 27);

    return 0;
}
