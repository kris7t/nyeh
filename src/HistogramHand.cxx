#include "HistogramHand.hxx"

static const int chs[] = { 0, 1, 2 };
static const int histSize[] = { 30, 8, 4 };
static const float hrange[] = { 0, 180 };
static const float srange[] = { 0, 256 };
static const float * ranges[] = { hrange, srange, srange };
static const cv::Mat closekernel = (cv::Mat_<uchar>(cv::Size(5,5)) << 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0);

HistogramHand::HistogramHand(double thresh, double fillratio, double projection) {
    thresh_ = thresh;
    fillratio_ = fillratio;
    projection_ = projection;
}

void HistogramHand::measureHist(Cam_ cap, cv::MatND & hist, cv::Point center, int radius, bool accumulate) {
    mask.create(cv::Size(640, 480), CV_8UC1);
    mask = cv::Scalar(0);

    cv::circle(mask, center, radius, cv::Scalar(255), CV_FILLED);
    
    do {
        cap->grabImage();
        cam = cap->frame().clone();

        cv::circle(cam, center, radius, cv::Scalar(0, 30, 220), 2);
 
        cv::imshow("Cam", cam);
    } while (cv::waitKey(10) != 13);

    cv::cvtColor(cam, hsv, CV_BGR2HSV);
    cv::calcHist(&hsv, 1, chs, mask, hist, 3, histSize, ranges, true, accumulate);
}

void HistogramHand::calibrate(Cam_ cap) {
    cv::namedWindow("Cam");
    measureHist(cap, hist, cv::Point(320, 240), 60, false);
    measureHist(cap, hist, cv::Point(120, 120), 60);
    measureHist(cap, hist, cv::Point(520, 120), 60);
    measureHist(cap, hist, cv::Point(520, 360), 60);
    measureHist(cap, hist, cv::Point(120, 360), 60);
    measureHist(cap, hist, cv::Point(320, 240), 20);
    measureHist(cap, hist, cv::Point(120, 120), 20);
    measureHist(cap, hist, cv::Point(520, 120), 20);
    measureHist(cap, hist, cv::Point(520, 360), 20);
    measureHist(cap, hist, cv::Point(120, 360), 20);
    cvDestroyWindow("Cam");

    double max = 0;

    for (int i = 0; i < hist.size.p[0]; ++i) {
        for (int j = 0; j < hist.size.p[1]; ++j) {
            for (int k = 0; k < hist.size.p[2]; ++k) {
                max = std::max(max, static_cast<double>(hist.at<float>(i,j,k)));
            }
        }
    }

    for (int i = 0; i < hist.size.p[0]; ++i) {
        for (int j = 0; j < hist.size.p[1]; ++j) {
            for (int k = 0; k < hist.size.p[2]; ++k) {
                float & r = hist.at<float>(i,j,k);
                r = r / max * 255;
            }
        }
    }
}

void HistogramHand::update(const cv::Mat & cam) {
    contours.clear();

    cv::cvtColor(cam, hsv, CV_BGR2HSV);
    cv::calcBackProject(&hsv, 1, chs, hist, bp, ranges);
    cv::threshold(bp, bp, thresh_, 255, CV_THRESH_BINARY);
    cv::morphologyEx(bp, bp, CV_MOP_CLOSE, closekernel);

    binary = bp.clone();

    cv::findContours(bp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for(auto it = contours.begin(); it != contours.end(); ++it)
        cv::convexHull(*it, *it);

    cv::Point2f center_best;
    float radius_best = -1;

    for(auto it = contours.begin(); it != contours.end(); ++it) {
        cv::Point2f center;
        float radius;
        cv::minEnclosingCircle(*it, center, radius);

        mask = cv::Scalar(0);
        circle(mask, center, radius, cv::Scalar(255), CV_FILLED);
        cv::bitwise_and(binary, mask, maskedbinary);

        if (radius > radius_best && countNonZero(maskedbinary) > fillratio_ * countNonZero(mask)) {
            radius_best = radius;
            center_best = center;
        }
    }
    
    std::cout << "radius: " << radius_best << "\r" << std::flush;

    if (radius_best > 0) {
        position_.x = center_best.x;
        position_.y = center_best.y;
        position_.z = projection_ / radius_best;
    }
}

volatile const cv::Point3f & HistogramHand::position() volatile const {
    return position_;
}

volatile const cv::Vec3f & HistogramHand::velocity() volatile const {
    return velocity_;
}
