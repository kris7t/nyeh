#include "HistogramHand.hxx"

static const int chs[] = { 0, 1, 2 };
static const int histSize[] = { 30, 8, 4 };
static const float hrange[] = { 0, 180 };
static const float srange[] = { 0, 256 };
static const float * ranges[] = { hrange, srange, srange };
static const cv::Mat closekernel = (cv::Mat_<uchar>(cv::Size(5,5)) <<
    0, 1, 1, 1, 0,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    0, 1, 1, 1, 0);
static const float sigma2_s = 10;
static const float sigma2_r = 10;
static const float sigma2_ms = 4;
static const float sigma2_mr = 9;
static const float L = 100;

HistogramHand::HistogramHand(double fillratio, double t) : fillratio_(fillratio), kf(6, 3, 0), measurement(3, 1, CV_32FC1) {

    kf.transitionMatrix = (cv::Mat_<float>(6, 6) << 
        1, t, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 1, t, 0, 0,
        0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 1, t,
        0, 0, 0, 0, 0, 1);

    kf.measurementMatrix = (cv::Mat_<float>(3, 6) <<
        1, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0);

    kf.measurementNoiseCov = (cv::Mat_<float>(3, 3) <<
        sigma2_ms, 0, 0,
        0, sigma2_ms, 0,
        0, 0, sigma2_mr);

    float t2 = t*t/2;
    float t3 = t*t2;
    float t4 = t2*t2;

    float t2_s = t2 * sigma2_s;
    float t3_s = t3 * sigma2_s;
    float t4_s = t4 * sigma2_s;

    float t2_r = t2 * sigma2_r;
    float t3_r = t3 * sigma2_r;
    float t4_r = t4 * sigma2_r;

    kf.processNoiseCov = (cv::Mat_<float>(6, 6) <<
        t4_s, t3_s, 0, 0, 0, 0,
        t3_s, t2_s, 0, 0, 0, 0,
        0, 0, t4_s, t3_s, 0, 0,
        0, 0, t3_s, t2_s, 0, 0,
        0, 0, 0, 0, t4_r, t3_r,
        0, 0, 0, 0, t3_r, t2_r);

    kf.statePre = cv::Mat::zeros(6, 1, CV_32F);
    kf.errorCovPre = cv::Mat::eye(6, 6, CV_32F) * L;
}

void HistogramHand::measureHist(Cam_ cap, cv::MatND & hist, cv::Point center, int radius, bool accumulate) {
    mask.create(cv::Size(640, 480), CV_8UC1);
    mask = cv::Scalar(0);

    cv::circle(mask, center, radius, cv::Scalar(255), CV_FILLED);
    
    do {
        cap->grabImage();
        cam = cap->frame().clone();

        cv::flip(cam, cam, 1);
        cv::circle(cam, center, radius, cv::Scalar(0, 30, 220), 2);
 
        cv::imshow("Cam", cam);
    } while (cv::waitKey(10) != 13);

    cv::cvtColor(cam, hsv, CV_BGR2HSV);
    cv::calcHist(&hsv, 1, chs, mask, hist, 3, histSize, ranges, true, accumulate);
}

double HistogramHand::measureRadius(Cam_ cap) {
    do {
        cap->grabImage();
        cam = cap->frame().clone();

        update(cam);

        cv::flip(cam, cam, 1);
        if (position_.z > 0) {
            cv::circle(cam, cv::Point(position_.x, position_.y), position_.z, cv::Scalar(0, 30, 220), 2);
        }

        cv::imshow("Cam", cam);
    } while (cv::waitKey(10) != 13);

    return position_.z;
}

void HistogramHand::calibrate(Cam_ cap) {
    cv::namedWindow("Cam");

    cv::displayOverlay("Cam", "Please move the ball into the circle and press enter.\n"
                            " The better it fits, the better the results of the calibration are.", std::numeric_limits<int>::max());

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

    cv::displayOverlay("Cam", "Please move the ball as far from the camera as you will during the game and press enter.\n"
                            " Try to find a position where the ball is detected well.", std::numeric_limits<int>::max());
    minRadius_ = measureRadius(cap);

    cv::displayOverlay("Cam", "Please move the ball as close to the camera as you will during the game and press enter.\n"
                            " Try to find a position where the ball is detected well.", std::numeric_limits<int>::max());
    maxRadius_ = measureRadius(cap);

    cvDestroyWindow("Cam");
}

static inline bool findBlob(const cv::Mat & binsearch, cv::Point * loc = NULL) {
    for (int y = 0; y < binsearch.rows; ++y) {
        for (int x = 0; x < binsearch.cols; ++x) {
            if (binsearch.at<uchar>(y, x)) {
                if (loc) {
                    loc->x = x;
                    loc->y = y;
                }
                return true;
            }
        }
    }
    return false;
}

void HistogramHand::update(const cv::Mat & cam) {
    contours.clear();

    cv::cvtColor(cam, hsv, CV_BGR2HSV);
    cv::calcBackProject(&hsv, 1, chs, hist, bp, ranges);

    cv::Rect rect(0, 0, 0, 0);
    int area = 0;

    int s = 0;
    int e = 255;
    while (std::abs(s - e) > 1) {
        cv::Rect rect_iter;
        int area_iter;

        double v = (s + e) / 2;

        cv::threshold(bp, binsearch, v, 255, CV_THRESH_BINARY);
        cv::erode(binsearch, binsearch, closekernel);

        cv::Point maxl;

        if (findBlob(binsearch, &maxl) == 0) {
            e = v;
            continue;
        }

        area_iter = cv::floodFill(binsearch, maxl, cv::Scalar(0), &rect_iter, cv::Scalar(0), cv::Scalar(0), 8 | CV_FLOODFILL_FIXED_RANGE);

        if (findBlob(binsearch)) {
            s = v;
        } else {
            e = v;
            rect = rect_iter;
            area = area_iter;
        }
    }

    if (rect.height > 0 && rect.area() * fillratio_ < area) {
        //update the kalman filter

        measurement.at<float>(0, 0) = rect.x + rect.width / 2.0f;
        measurement.at<float>(1, 0) = rect.y + rect.height / 2.0f;
        measurement.at<float>(2, 0) = std::max(rect.height, rect.width) / 2.0f;

        kf.correct(measurement);
    } else {
        kf.statePost = kf.statePre.clone();
        kf.errorCovPost = kf.errorCovPre.clone();
    }

    //make the kalman filter predict
    kf.predict();

    position_.x = 639 - kf.statePost.at<float>(0, 0);
    position_.y = kf.statePost.at<float>(2, 0);
    position_.z = kf.statePost.at<float>(4, 0);

    velocity_.x = kf.statePost.at<float>(1, 0);
    velocity_.y = kf.statePost.at<float>(3, 0);
    velocity_.z = kf.statePost.at<float>(5, 0);
}

volatile const cv::Point3f & HistogramHand::position() volatile const {
    return position_;
}

volatile const cv::Point3f & HistogramHand::velocity() volatile const {
    return velocity_;
}

volatile const double & HistogramHand::minRadius() volatile const {
    return minRadius_;
}

volatile const double & HistogramHand::maxRadius() volatile const {
    return maxRadius_;
}
