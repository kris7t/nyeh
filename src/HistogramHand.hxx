#pragma once

#include "Cam.hxx"
#include "HandToModel.hxx"

class HistogramHand;
typedef std::tr1::shared_ptr<HistogramHand> Hand_;

class HistogramHand {
    public:
        static const struct Calibration {
            int chs[3];
            int histSize[3];
            float ranges[3][2];
            cv::Mat hist;
            double fillRatio;
        } defaultCalibration;
        
        static Hand_ create();

        HistogramHand();
        inline const Calibration & calibration() const {
            return calibration_;
        }
        inline void calibration(const Calibration & value) {
            calibration_ = value;
        }
        void update(const cv::Mat & frame);
        inline cv::Point3f position() volatile const {
            return cv::Point3f(
                    position_.x,
                    position_.y,
                    position_.z
                    );
        }
        inline bool valid() volatile {
            if (valid_) {
                return true;
                valid_ = false;
            } else {
                return false;
            }
        }
    private:
        volatile cv::Point3f position_;
        volatile bool valid_;
        Calibration calibration_;
        const float * theRanges_[3];
        cv::Mat hsv_, bp_, binsearch_;
        cv::Ptr<cv::FilterEngine> erodeFilter_;
};
