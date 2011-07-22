#include "Cam.hxx"
#include "HistogramHand.hxx"

int main(int argc, char * argv[]) {
    if (argc < 2) return -1;

    Cam_ c = Cam::create(atoi(argv[1]));
    HistogramHand hh(128, .4, 2852.11);

    hh.calibrate(c);

    cv::namedWindow("test");
    cv::namedWindow("test");

    cv::Mat draw;

    do {
        c->grabImage();
        hh.update(c->frame());
        draw = c->frame().clone();
        circle(draw, cv::Point2i(hh.position().x, hh.position().y), hh.position().z, cv::Scalar(0, 255, 0), CV_FILLED);
        cv::imshow("test", draw);
    } while (cv::waitKey(10) != 27);

    return 0;
}
