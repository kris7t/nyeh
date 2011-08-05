#include "Calibrate.hxx"

static const std::string windowName = "Calibration";

const HistogramHand::Calibration HistogramHand::defaultCalibration = {
    {0, 1, 2},                          // chs
    {30, 8, 4},                         // histSize
    {{0, 180}, {0, 256}, {0, 256}},     // ranges
    cv::Mat(),                          // hist
    0.6                                 // fillRatio
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
    0.0f,       // kappa
    cv::Point2f(), // offset
    cv::Size2f()   // halfSize
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
    guiBounds(cap, hand, filter);
    apply(toModel);
    cvDestroyWindow(windowName.c_str());
}

static void normalize3D(cv::Mat & hist);

void Calibrate::guiHist(Cam_ cap) {
    cv::displayOverlay(windowName,
            "Please move the ball into the circle and press enter.\n",
            5000);
    float radius = cap->size().width * 0.0625,
          padding = 2 * radius;
    cv::Size2f floatSize(cap->size().width, cap->size().height);
    measureHist(cap, {floatSize.width / 2, floatSize.height / 2}, radius, false);
    measureHist(cap, {padding, padding}, radius);
    measureHist(cap, {floatSize.width - padding, padding}, radius);
    measureHist(cap, {floatSize.width - padding, floatSize.height - padding}, radius);
    measureHist(cap, {padding, floatSize.height - padding}, radius);
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

void Calibrate::guiBounds(Cam_ cap, Hand_ hand, HandFilter_ filter) {
    cv::displayOverlay(windowName,
            "Please indicate a corner of the area in which you will"
            " move the ball during the game and press enter.",
            5000);
    cv::Point2f a = measurePosition(cap, hand, filter);
    cv::displayOverlay(windowName,
            "Please indicate another corner of the area in which you will"
            " move the ball during the game and press enter."
            "\nThe two point should form the diagonal or the antediagonal.",
            5000);
    cv::Point2f b = measurePosition(cap, hand, filter);
    toModelCalibration_.offset = {std::min(a.x, b.x), std::min(a.y, b.y)};
    toModelCalibration_.halfSize = {
        (std::max(a.x, b.x) - toModelCalibration_.offset.x) / 2,
        (std::max(a.y, b.y) - toModelCalibration_.offset.y) / 2};
}

void Calibrate::measureHist(Cam_ cap, cv::Point2f center, float radius, bool accumulate) {
    mask_.create(cap->size(), CV_8UC1);
    mask_ = cv::Scalar(0);

    cv::circle(mask_, center, radius, cv::Scalar(255), CV_FILLED);
    
    do {
        writableFlippedShot(cap);
        drawCircle(center, radius);
        imshow();
    } while (cv::waitKey(10) != 13);

    cv::cvtColor(frame_, hsv_, CV_BGR2HSV);
    cv::calcHist(&hsv_, 1, handCalibration_.chs, mask_,
            handCalibration_.hist, 3, handCalibration_.histSize,
            theRanges_, true, accumulate);
}

double Calibrate::measureRadius(Cam_ cap, Hand_ hand, HandFilter_ filter) {
    return measurePositionAndRadius(cap, hand, filter).z;
}

cv::Point2f Calibrate::measurePosition(Cam_ cap, Hand_ hand, HandFilter_ filter) {
    cv::Point3f position = measurePositionAndRadius(cap, hand, filter);
    return {position.x, position.y};
}

cv::Point3f Calibrate::measurePositionAndRadius(Cam_ cap, Hand_ hand, HandFilter_ filter) {
    cv::Point3f position;
    do {
        writableFlippedShot(cap);
        hand->update(cap->frame());
        filter->update(hand);
        position = filter->position();
        
        drawCircle({position.x, position.y}, position.z); 
        imshow();
    } while (cv::waitKey(10) != 13);
    return position;
}

void Calibrate::writableFlippedShot(Cam_ cap) {
    cap->grabImage();
    cv::flip(cap->frame(), frame_, 1);
}

void Calibrate::drawCircle(cv::Point2f center, double radius) {
    if (radius <= 0) {
        return;
    }
    cv::circle(frame_, center, radius, cv::Scalar(0, 30, 220), 2, CV_AA);
}

void Calibrate::imshow() {
    cv::imshow(windowName, frame_);
}

void normalize3D(cv::Mat & hist) {
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
