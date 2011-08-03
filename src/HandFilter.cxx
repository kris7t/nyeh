#include "HandFilter.hxx"
#include "HistogramHand.hxx"

static const float L = 1000;

HandFilter_ HandFilter::create() {
    return HandFilter_(new HandFilter());
}

HandFilter::HandFilter()
    : calibration_(HandFilter::defaultCalibration),
      kf_(6, 3, 0),
      measurement_(3, 1, CV_32F) {
    kf_.transitionMatrix = cv::Mat::eye(6, 6, CV_32F);
    kf_.measurementMatrix = (cv::Mat_<float>(3, 6) <<
        1, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0);
    kf_.measurementNoiseCov = (cv::Mat_<float>(3, 3) <<
        calibration_.sigma2_ms, 0, 0,
        0, calibration_.sigma2_ms, 0,
        0, 0, calibration_.sigma2_mr);
    kf_.processNoiseCov = cv::Mat::zeros(6, 6, CV_32F);
    kf_.statePre = cv::Mat::zeros(6, 1, CV_32F);
    kf_.errorCovPre = cv::Mat::eye(6, 6, CV_32F) * L;
}

void HandFilter::calibration(const HandFilter::Calibration & value) {
    calibration_ = value;
    kf_.measurementNoiseCov.at<float>(0, 0) = calibration_.sigma2_ms;
    kf_.measurementNoiseCov.at<float>(1, 1) = calibration_.sigma2_ms;
    kf_.measurementNoiseCov.at<float>(2, 2) = calibration_.sigma2_mr;
}

void HandFilter::correctFilter(Hand_ hand) {
    if (hand->valid()) {
        cv::Point3f measuredPos = hand->position();
        measurement_.at<float>(0, 0) = measuredPos.x;
        measurement_.at<float>(1, 0) = measuredPos.y;
        measurement_.at<float>(2, 0) = measuredPos.z;
        kf_.correct(measurement_);
    } else {
        if (kf_.statePost.step[0] == kf_.statePre.step[0]) {
            memcpy(kf_.statePost.data, kf_.statePre.data, kf_.statePre.step[0] * 6);
        } else {
            kf_.statePost = kf_.statePre.clone();
        }
        if (kf_.errorCovPost.step[0] == kf_.errorCovPre.step[0]) {
            memcpy(kf_.errorCovPost.data, kf_.errorCovPre.data, kf_.errorCovPre.step[0] * 6);
        } else {
            kf_.errorCovPost = kf_.errorCovPre.clone();
        }
    }
}

void HandFilter::updateMatrices(double t) {
    kf_.transitionMatrix.at<float>(0, 1) = t;
    kf_.transitionMatrix.at<float>(2, 3) = t;
    kf_.transitionMatrix.at<float>(4, 5) = t;

    float t2 = t*t/2;
    float t3 = t*t2;
    float t4 = t2*t2;

    float t2_s = t2 * calibration_.sigma2_s;
    float t3_s = t3 * calibration_.sigma2_s;
    float t4_s = t4 * calibration_.sigma2_s;

    float t2_r = t2 * calibration_.sigma2_r;
    float t3_r = t3 * calibration_.sigma2_r;
    float t4_r = t4 * calibration_.sigma2_r;

    kf_.processNoiseCov.at<float>(0, 0) = t4_s;
    kf_.processNoiseCov.at<float>(2, 2) = t4_s;
    kf_.processNoiseCov.at<float>(4, 4) = t4_r;

    kf_.processNoiseCov.at<float>(1, 1) = t2_s;
    kf_.processNoiseCov.at<float>(3, 3) = t2_s;
    kf_.processNoiseCov.at<float>(5, 5) = t2_r;

    kf_.processNoiseCov.at<float>(0, 1) = t3_s;
    kf_.processNoiseCov.at<float>(1, 0) = t3_s;
    kf_.processNoiseCov.at<float>(2, 3) = t3_s;
    kf_.processNoiseCov.at<float>(3, 2) = t3_s;
    kf_.processNoiseCov.at<float>(4, 5) = t3_r;
    kf_.processNoiseCov.at<float>(5, 4) = t3_r;
}

void HandFilter::makePrediction() {
    kf_.predict();
    position_.x = kf_.statePre.at<float>(0, 0);
    position_.y = kf_.statePre.at<float>(2, 0);
    position_.z = kf_.statePre.at<float>(4, 0);
    velocity_.x = kf_.statePre.at<float>(1, 0);
    velocity_.y = kf_.statePre.at<float>(3, 0);
    velocity_.z = kf_.statePre.at<float>(5, 0);
}

void HandFilter::update(Hand_ hand) {
    correctFilter(hand);
    updateMatrices(elapsed_);
    makePrediction();
}
