#include "Calibrate.hxx"

static const std::string windowName = "Calibration";

const HistogramHand::Calibration HistogramHand::defaultCalibration = {
    {0, 1, 2},                          // chs
    {30, 8, 4},                         // histSize
    {{0, 180}, {0, 256}, {0, 256}},     // ranges
    cv::Mat(),                          // hist
    0.2                                 // fillRatio
};


const HandFilter::Calibration HandFilter::defaultCalibration = {
    5,      // sigma2_s
    7,      // sigma2_r
    .1,     // sigma2_ms
    .1      // sigma2_mr
};

const HandToModel::Calibration HandToModel::defaultCalibration = {
    1.0f,       // minRadius
    1.0f,       // maxRadius
    0.0f        // kappa
};

Calibrate_ Calibrate::create() {
    return Calibrate_(new Calibrate());
}

Calibrate::Calibrate()
    : handCalibration_(HistogramHand::defaultCalibration),
      filterCalibration_(HandFilter::defaultCalibration),
      toModelCalibration_(HandToModel::defaultCalibration),
      theRanges_{handCalibration_.ranges[0],
          handCalibration_.ranges[1],
          handCalibration_.ranges[2]} {
}

void Calibrate::run(Cam_ cap, Hand_ hand, HandFilter_ filter, HandToModel_ toModel) {
    cv::namedWindow(windowName);
    guiHist(cap);
    apply(hand);
    apply(filter);
    guiRadius(cap, hand, filter);
    apply(toModel);
    cvDestroyWindow(windowName.c_str());
}

static void normalize3D(cv::Mat & hist) {
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

void Calibrate::guiHist(Cam_ cap) {
    cv::displayOverlay(windowName,
            "Please move the ball into the circle and press enter.\n",
            5000);
    measureHist(cap, cv::Point(160, 120), 30, false);
    measureHist(cap, cv::Point(60, 60), 30);
    measureHist(cap, cv::Point(260, 60), 30);
    measureHist(cap, cv::Point(260, 180), 30);
    measureHist(cap, cv::Point(60, 180), 30);
    measureHist(cap, cv::Point(160, 120), 10);
    measureHist(cap, cv::Point(60, 60), 10);
    measureHist(cap, cv::Point(260, 60), 10);
    measureHist(cap, cv::Point(260, 180), 10);
    measureHist(cap, cv::Point(60, 180), 10);
    normalize3D(handCalibration_.hist);
}

void Calibrate::guiRadius(Cam_ cap, Hand_ hand, HandFilter_ filter) {
    cv::displayOverlay(windowName,
            "Please move the ball as far from the camera as you will"
            " during the game and press enter.",
            5000);
    toModelCalibration_.minRadius = measureRadius(cap, hand, filter);
    cv::displayOverlay(windowName,
            "Please move the ball as close to the camera as you will"
            " during the game and press enter.",
            5000);
    toModelCalibration_.maxRadius = measureRadius(cap, hand, filter);
    toModelCalibration_.kappa = toModelCalibration_.minRadius
        / (toModelCalibration_.maxRadius - toModelCalibration_.minRadius);
}

void Calibrate::measureHist(Cam_ cap, cv::Point2f center, int radius, bool accumulate) {
    mask_.create(cv::Size(320, 240), CV_8UC1);
    mask_ = cv::Scalar(0);

    cv::circle(mask_, center, radius, cv::Scalar(255), CV_FILLED);
    
    do {
        writableFlippedShot(cap);
        cv::circle(frame_, center, radius, cv::Scalar(0, 30, 220), 2, CV_AA);
        cv::imshow(windowName, frame_);
    } while (cv::waitKey(10) != 13);

    cv::cvtColor(frame_, hsv_, CV_BGR2HSV);
    cv::calcHist(&hsv_, 1, handCalibration_.chs, mask_,
            handCalibration_.hist, 3, handCalibration_.histSize,
            theRanges_, true, accumulate);
}

double Calibrate::measureRadius(Cam_ cap, Hand_ hand, HandFilter_ filter) {
    cv::Point3f position;
    do {
        writableFlippedShot(cap);
        hand->update(cap->frame());
        filter->update(hand);
        
        position = filter->position();
        if (position.z > 0) {
            cv::circle(frame_, cv::Point(position.x, position.y), position.z,
                    cv::Scalar(0, 30, 220), 2, CV_AA);
        }

        cv::imshow(windowName, frame_);
    } while (cv::waitKey(10) != 13);
    return position.z;
}

void Calibrate::writableFlippedShot(Cam_ cap) {
    cap->grabImage();
    cv::flip(cap->frame(), frame_, 1);
}

