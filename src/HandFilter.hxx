#pragma once

#include "Elapsed.hxx"

class HistogramHand;
typedef std::tr1::shared_ptr<HistogramHand> Hand_;

class HandFilter;
typedef std::tr1::shared_ptr<HandFilter> HandFilter_;

class HandFilter {
    public:
        static const struct Calibration {
            float sigma2_s, sigma2_r, sigma2_ms, sigma2_mr;
        } defaultCalibration;
        
        static HandFilter_ create();

        HandFilter();
        void update(Hand_ hand);
        inline cv::Point3f position() volatile const {
            return cv::Point3f(
                    position_.x,
                    position_.y,
                    position_.z);
        }
        inline cv::Point3f velocity() volatile const {
            return cv::Point3f(
                    velocity_.x,
                    velocity_.y,
                    velocity_.z);
        }
        inline const Calibration & calibration() const {
            return calibration_;
        }
        void calibration(const Calibration & value);
    private:
        void correctFilter(Hand_ hand);
        void updateMatrices(double t);
        void makePrediction();
        
        volatile cv::Point3f position_, velocity_;
        Calibration calibration_;
        cv::KalmanFilter kf_;
        cv::Mat measurement_;
        Elapsed elapsed_;
};
